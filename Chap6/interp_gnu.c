#define _CRT_SECURE_NO_WARNINGS
#define PROGRAM_FILE "interp_gnu.cl"
#define KERNEL_FUNC "interp"

#define SCALE_FACTOR 4

#define PNG_DEBUG 4
#include <png.h>
#include <pngstruct.h>
#include <pnginfo.h>
#define INPUT_FILE "Lenna_1.png"
#define OUTPUT_FILE "output.png"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif


void printer(png_structp str, png_const_charp chr) {
	printf("Test\n");
	printf("%s\n", chr);
}

/* Find a GPU or CPU associated with the first available platform */
cl_device_id create_device() {

	cl_platform_id platform;
	cl_device_id   dev;
	int			   err;

	/* Identify a platform */
	err = clGetPlatformIDs(1, &platform, NULL);
	if (err < 0) {
		perror("Couldn't identify a platform");
		exit(1);
	}

	/* Access a device */
	err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
	if (err == CL_DEVICE_NOT_FOUND) {
		err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
	}
	if (err < 0) {
		perror("Couldn't access any devices");
		exit(1);
	}

	return dev;
}

/* Create program from a file and compile it */
cl_program build_program(cl_context ctx, cl_device_id dev,
						 const char *filename) {

	cl_program program;
	FILE *	   program_handle;
	char *	   program_buffer;
	char *	   program_log;
	size_t	   program_size;
	size_t	   log_size;
	int		   err;
	char	   arg[20];

	/* Read program file and place content into buffer */
	program_handle = fopen(filename, "r");
	if (program_handle == NULL) {
		perror("Couldn't find the program file");
		exit(1);
	}
	fseek(program_handle, 0, SEEK_END);
	program_size = ftell(program_handle);
	rewind(program_handle);
	program_buffer = (char *)malloc(program_size + 1);
	program_buffer[program_size] = '\0';
	fread(program_buffer, sizeof(char), program_size, program_handle);
	fclose(program_handle);

	/* Create program from file */
	program = clCreateProgramWithSource(ctx, 1, (const char **)&program_buffer,
										&program_size, &err);
	if (err < 0) {
		perror("Couldn't create the program");
		exit(1);
	}
	free(program_buffer);

	/* Create build argument */
	sprintf(arg, "-DSCALE=%u", SCALE_FACTOR);

	/* Build program */
	err = clBuildProgram(program, 0, NULL, arg, NULL, NULL);
	if (err < 0) {

		/* Find size of log and print to std output */
		clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 0, NULL,
							  &log_size);
		program_log = (char *)malloc(log_size + 1);
		program_log[log_size] = '\0';
		clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, log_size + 1,
							  program_log, NULL);
		printf("%s\n", program_log);
		free(program_log);
		exit(1);
	}

	return program;
}

png_infop read_image_data(const char *filename, png_bytep *input,
						  png_bytep *output) {

	/* Open input file */
	FILE *png_input;
	if ((png_input = fopen(filename, "rb")) == NULL) {
		perror("Can't read input image file");
		exit(1);
	}


	/* Read image data */
	png_structp png_ptr =
		png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, printer, printer);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	png_init_io(png_ptr, png_input);
	png_read_info(png_ptr, info_ptr);

	// png_set_palette_to_rgb(png_ptr);
	// png_read_update_info(png_ptr, info_ptr);
	// png_read_info(png_ptr, info_ptr);

	int color = png_get_color_type(png_ptr, info_ptr);
	int row_bytes = png_get_rowbytes(png_ptr, info_ptr);


	printf("row byte_in: %d\n", row_bytes);


	size_t h = info_ptr->height;
	/* Allocate input/output memory and initialize data */
	*input = malloc(h * row_bytes * 8);
	*output = malloc(h * row_bytes * SCALE_FACTOR * SCALE_FACTOR * 8);
	for (size_t i = 0; i < h; i++) {
		png_read_row(png_ptr, *input + i * row_bytes, NULL);
	}

	/* Close input file */
	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
	fclose(png_input);
	return info_ptr;
}

void write_image_data(const char *filename, png_bytep data,
					  png_infop info_ptr) {

	/* Open output file */
	FILE *png_output;
	if ((png_output = fopen(filename, "wb")) == NULL) {
		perror("Create output image file");
		exit(1);
	}

	/* Write image data */
	png_structp png_ptr =
		png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info_ptr_out = png_create_info_struct(png_ptr);
	png_init_io(png_ptr, png_output);
	png_set_IHDR(png_ptr, info_ptr_out, info_ptr->width * SCALE_FACTOR,
				 info_ptr->height * SCALE_FACTOR, 8, info_ptr->color_type,
				 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
				 PNG_FILTER_TYPE_DEFAULT);


	if (info_ptr->num_palette)
		png_set_PLTE(png_ptr, info_ptr_out, info_ptr->palette,
					 info_ptr->num_palette);

	png_write_info(png_ptr, info_ptr_out);
	for (size_t i = 0; i < info_ptr_out->height; i++) {
		png_write_row(png_ptr,
					  data + i * png_get_rowbytes(png_ptr, info_ptr_out));
	}

	/* Close file */
	png_write_end(png_ptr, NULL);
	png_destroy_write_struct(&png_ptr, &info_ptr_out);
	png_destroy_read_struct((png_structp)NULL, &info_ptr, (png_infopp)NULL);
	fclose(png_output);
}

int main(int argc, char **argv) {

	/* Host/device data structures */
	cl_device_id	 device;
	cl_context		 context;
	cl_command_queue queue;
	cl_program		 program;
	cl_kernel		 kernel;
	cl_int			 err;
	size_t			 global_size[2];

	/* Image data */
	png_bytep		input_pixels;
	png_bytep		output_pixels;
	cl_image_format png_format;
	cl_mem			input_image;
	cl_mem			output_image;
	// size_t			width;
	// size_t			height;
	size_t origin[3];
	size_t region[3];

	/* Open input file and read image data */
	png_infop info_ptr =
		read_image_data(INPUT_FILE, &input_pixels, &output_pixels);

	/* Create a device and context */
	device = create_device();
	context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
	if (err < 0) {
		perror("Couldn't create a context");
		exit(1);
	}

	/* Create kernel */
	program = build_program(context, device, PROGRAM_FILE);
	kernel = clCreateKernel(program, KERNEL_FUNC, &err);
	if (err < 0) {
		printf("Couldn't create a kernel: %d", err);
		exit(1);
	};

	/* Create input image object */
	png_format.image_channel_order = CL_RGBA;
	png_format.image_channel_data_type = CL_UNSIGNED_INT8;
	input_image = clCreateImage2D(
		context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, &png_format,
		info_ptr->width, info_ptr->height, 0, (void *)input_pixels, &err);
	if (err < 0) {
		perror("Couldn't create the image object");
		exit(1);
	};

	/* Create output image object */
	output_image = clCreateImage2D(
		context, CL_MEM_WRITE_ONLY, &png_format, SCALE_FACTOR * info_ptr->width,
		SCALE_FACTOR * info_ptr->height, 0, NULL, &err);
	if (err < 0) {
		perror("Couldn't create the image object");
		exit(1);
	};

	/* Create buffer */
	if (err < 0) {
		perror("Couldn't create a buffer");
		exit(1);
	};

	/* Create kernel arguments */
	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_image);
	err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &output_image);
	if (err < 0) {
		printf("Couldn't set a kernel argument");
		exit(1);
	};

	/* Create a command queue */
	queue = clCreateCommandQueue(context, device, 0, &err);
	if (err < 0) {
		perror("Couldn't create a command queue");
		exit(1);
	};

	/* Enqueue kernel */
	global_size[0] = info_ptr->width;
	global_size[1] = info_ptr->height;
	err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_size, NULL, 0,
								 NULL, NULL);
	if (err < 0) {
		perror("Couldn't enqueue the kernel");
		exit(1);
	}

	/* Read the image object */
	origin[0] = 0;
	origin[1] = 0;
	origin[2] = 0;
	region[0] = SCALE_FACTOR * info_ptr->width;
	region[1] = SCALE_FACTOR * info_ptr->height;
	region[2] = 1;
	err = clEnqueueReadImage(queue, output_image, CL_TRUE, origin, region, 0, 0,
							 (void *)output_pixels, 0, NULL, NULL);
	if (err < 0) {
		perror("Couldn't read from the image object");
		exit(1);
	}

	/* Create output PNG file and write data */
	write_image_data(OUTPUT_FILE, output_pixels, info_ptr);

	/* Deallocate resources */

	clReleaseMemObject(input_image);
	clReleaseMemObject(output_image);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(queue);
	clReleaseProgram(program);
	clReleaseContext(context);
	free(input_pixels);
	free(output_pixels);
	return 0;
}
