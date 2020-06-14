#define __CL_ENABLE_EXCEPTIONS
//#define __NO_STD_VECTOR

#include <fstream>
#include <iostream>
#include <iterator>
#include <chrono>

#define CL_HPP_MINIMUM_OPENCL_VERSION 100
#define CL_HPP_TARGET_OPENCL_VERSION 110

#ifdef MAC
#include <OpenCL/cl.hpp>
#else
#include <CL/cl2.hpp>
#endif
using s_clock = std::chrono::steady_clock;

constexpr auto arrSize = 30000;
void		   LinearInterpolateDD(const double *dfX1, const double *dfY1,
								   const double *dfX2, const double *dfY2,
								   const double *dfX, double *dfY) {
	  *dfY = (*dfY1 * (*dfX2 - *dfX) + *dfY2 * (*dfX - *dfX1)) / (*dfX2 - *dfX1);
}


int main(void) {
	std::vector<cl::Platform> platforms;
	std::vector<cl::Device>	  platformDevices, ctxDevices;
	std::string				  device_name;

	std::string			kernelName;
	int					x{};
	std::vector<double> Xs(arrSize);
	std::generate(Xs.begin(), Xs.end(), [x]() mutable { return x++; });
	std::vector<double> Ys(arrSize);
	std::generate(Ys.begin(), Ys.end(), [x]() mutable { return x += 2; });

	x = -1;

	std::vector<double> Xpts(arrSize / 10);
	std::generate(Xpts.begin(), Xpts.end(), [x, &Xs]() mutable {
		x++;
		return sqrt((1 + Xs[x * 10]) * Xs[(x + 1) * 10 - 1]);
	});
	std::vector<double> Ypts(arrSize / 10, 1);

	try {
		cl::Platform::get(&platforms);
		// Access all devices in first platform
		for (int j = 0; j < platforms.size(); j++) {
			size_t i = 0;
			platforms[j].getDevices(CL_DEVICE_TYPE_ALL, &platformDevices);

			// Create context and access device names
			cl::Context context(platformDevices);
			ctxDevices = context.getInfo<CL_CONTEXT_DEVICES>();
			for (i = 0; i < ctxDevices.size(); i++) {
				auto device_name = ctxDevices[i].getInfo<CL_DEVICE_NAME>();
				std::cout << "Device: " << device_name.c_str() << std::endl;
			}
		}
		// Place the GPU devices of the first platform into a context
		cl::Platform::get(&platforms);
		platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &platformDevices);
		cl::Context context(platformDevices);

		// Create and build program
		std::ifstream programFile("linear_interp.cl");
		std::string	  programString(std::istreambuf_iterator<char>(programFile),
									(std::istreambuf_iterator<char>()));
		cl::Program::Sources source(1, programString);
		cl::Program			 program(context, source);
		program.build(platformDevices);

		// Create individual kernels
		cl::Kernel linearKernel(program, "LinearInterpolateDD");
		double	   X1 = 1, Y1 = 2, X2 = 3, Y2 = 4, X = 2;
		double *   output{}, *newX;
		cl::Buffer bufX1(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
						 sizeof(double), &X1, NULL);
		cl::Buffer bufY1(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
						 sizeof(double), &Y1, NULL);
		cl::Buffer bufX2(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
						 sizeof(double), &X2, NULL);
		cl::Buffer bufY2(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
						 sizeof(double), &Y2, NULL);
		cl::Buffer bufX(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
						sizeof(double), &X, NULL);

		cl::Buffer output_buffer(context, CL_MEM_WRITE_ONLY, sizeof(double),
								 NULL, NULL);
		cl::CommandQueue queue(context, platformDevices[0]);

		linearKernel.setArg(0, bufX1);
		linearKernel.setArg(1, bufY1);
		linearKernel.setArg(2, bufX2);
		linearKernel.setArg(3, bufY2);
		linearKernel.setArg(4, bufX);
		linearKernel.setArg(5, output_buffer);

		auto start = s_clock::now();
		for (int i = 0; i < arrSize; i++) {
			newX = (double *)queue.enqueueMapBuffer(bufX, CL_TRUE, CL_MAP_WRITE,
													0, sizeof(double));
			*newX = sqrt(i) + 1.5;
			queue.enqueueUnmapMemObject(bufX, newX);
			queue.enqueueTask(linearKernel);
			output = (double *)queue.enqueueMapBuffer(
				output_buffer, CL_TRUE, CL_MAP_READ, 0, sizeof(double));


			// std::cout << "output: " << *output << '\n';
			queue.enqueueUnmapMemObject(output_buffer, output);
		}
		auto end = s_clock::now();
		std::cout << "One by One OpenCL run time: " << (end - start).count()
				  << '\n';


		start = s_clock::now();
		for (int i = 0; i < arrSize; i++) {
			X = sqrt(i) + 1.5;
			LinearInterpolateDD(&X1, &Y1, &X2, &Y2, &X, output);
			// std::cout << "output: " << *output << '\n';
		}
		end = s_clock::now();
		std::cout << "One by One local run time: " << (end - start).count()
				  << '\n';


		cl::Kernel linearParallelKernel(program, "LinearInterpolateDDParallel");

		cl::Buffer bufXs(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
						 sizeof(double) * Xs.size(), Xs.data(), NULL);
		cl::Buffer bufYs(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
						 sizeof(double) * Ys.size(), Ys.data(), NULL);
		cl::Buffer bufXpts(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
						   sizeof(double) * Xpts.size(), Xpts.data(), NULL);
		cl::Buffer bufYpts(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,
						   sizeof(double) * Ypts.size(), Ypts.data(), NULL);

		linearParallelKernel.setArg(0, bufXs);
		linearParallelKernel.setArg(1, bufYs);
		linearParallelKernel.setArg(2, bufXpts);
		linearParallelKernel.setArg(3, bufYpts);


		start = s_clock::now();
		// queue.enqueueTask(linearParallelKernel);
		queue.enqueueNDRangeKernel(linearParallelKernel, cl::NullRange,
								   cl::NDRange(Ypts.size()), cl::NDRange(2));
		auto outData = (double *)queue.enqueueMapBuffer(
			bufYpts, CL_TRUE, CL_MAP_READ, 0, sizeof(double) * Ypts.size());


		// std::cout << "output: " << *output << '\n';
		queue.enqueueUnmapMemObject(bufYpts, outData);
		end = s_clock::now();
		std::cout << "Parallel OpenCL run time: " << (end - start).count()
				  << '\n';

		for (int i = 0; i < arrSize / 10000; i++) {
			std::cout << "output: " << outData[i] << '\n';
		}

	} catch (cl::Error e) {
		std::cout << e.what() << ": Error code " << e.err()
				  << "\nWhat: " << e.what() << "" << std::endl;
	}

	return 0;
}
