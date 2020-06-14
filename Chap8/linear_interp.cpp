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


void LinearInterpolateDD(const double *dfX1, const double *dfY1,
						 const double *dfX2, const double *dfY2,
						 const double *dfX, double *dfY) {
	*dfY = (*dfY1 * (*dfX2 - *dfX) + *dfY2 * (*dfX - *dfX1)) / (*dfX2 - *dfX1);
}


int main(void) {

	std::vector<cl::Platform> platforms;
	std::vector<cl::Device>   devices;
	std::string				  kernelName;

	try {
		// Place the GPU devices of the first platform into a context
		cl::Platform::get(&platforms);
		platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);
		cl::Context context(devices);

		// Create and build program
		std::ifstream programFile("linear_interp.cl");
		std::string   programString(std::istreambuf_iterator<char>(programFile),
									(std::istreambuf_iterator<char>()));
		cl::Program::Sources source(1, programString);
		cl::Program			 program(context, source);
		program.build(devices);

		// Create individual kernels
		cl::Kernel linearKernel(program, "LinearInterpolateDD");
		double	 X1 = 1, Y1 = 2, X2 = 3, Y2 = 4, X = 2;
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
		cl::CommandQueue queue(context, devices[0]);

		linearKernel.setArg(0, bufX1);
		linearKernel.setArg(1, bufY1);
		linearKernel.setArg(2, bufX2);
		linearKernel.setArg(3, bufY2);
		linearKernel.setArg(4, bufX);
		linearKernel.setArg(5, output_buffer);

		auto start = s_clock::now();
		for (int i = 0; i < 30; i++) {
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
		std::cout << "Run time: " << (end - start).count() << '\n';


		start = s_clock::now();
		for (int i = 0; i < 30; i++) {
			X = sqrt(i) + 1.5;
			LinearInterpolateDD(&X1, &Y1, &X2, &Y2, &X, output);
			// std::cout << "output: " << *output << '\n';
		}
		end = s_clock::now();
		std::cout << "Run time: " << (end - start).count() << '\n';

	} catch (cl::Error e) {
		std::cout << e.what() << ": Error code " << e.err() << "hz " << e.what()
				  << std::endl;
	}

	return 0;
}
