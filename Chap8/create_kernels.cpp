#define __CL_ENABLE_EXCEPTIONS
//#define __NO_STD_VECTOR

#include <fstream>
#include <iostream>
#include <iterator>

#define CL_HPP_MINIMUM_OPENCL_VERSION 100
#define CL_HPP_TARGET_OPENCL_VERSION 110

#ifdef MAC
#include <OpenCL/cl.hpp>
#else
#include <CL/cl2.hpp>
#endif

int main(void) {

	std::vector<cl::Platform> platforms;
	std::vector<cl::Device>   devices;
	std::vector<cl::Kernel>   allKernels;
	std::string				  kernelName;

	try {
		// Place the GPU devices of the first platform into a context
		cl::Platform::get(&platforms);
		platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);
		cl::Context context(devices);

		// Create and build program
		std::ifstream programFile("kernels.cl");
		std::string   programString(std::istreambuf_iterator<char>(programFile),
									(std::istreambuf_iterator<char>()));
		cl::Program::Sources source(1, programString);
		cl::Program			 program(context, source);
		program.build(devices);

		// Create individual kernels
		cl::Kernel addKernel(program, "add");
		cl::Kernel subKernel(program, "subtract");
		// cl::Kernel multKernel(program, "multiply");

		// Create all kernels in program
		program.createKernels(&allKernels);
		for (unsigned int i = 0; i < allKernels.size(); i++) {
			kernelName = allKernels[i].getInfo<CL_KERNEL_FUNCTION_NAME>();
			std::cout << "Kernel: " << kernelName << std::endl;
		}
	} catch (cl::Error e) {
		std::cout << e.what() << ": Error code " << e.err() << std::endl;
	}

	return 0;
}
