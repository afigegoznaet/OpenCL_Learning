#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

int main(void) {

   /* Host/device data structures */
   cl_platform_id platform[3];
   cl_device_id device, devices[2];
   cl_uint vector_width;
   cl_int err;

   	char name[256];
	int valueSize;

   cl_uint num, i, devNum,j;
   /* Identify a platform */
   err = clGetPlatformIDs(3, platform, &num);
   printf("Platforms: %d\n", num);
   if(err < 0) {
      perror("Couldn't find any platforms");
      exit(1);
   }

   /* Access a device */
	for( i=0; i<num ; i++ )
		{
			printf("platform %d\n",i);
		err = clGetDeviceIDs(platform[i], CL_DEVICE_TYPE_ALL, 2, devices, &devNum);
		printf("Devices: %d\n", devNum);
		if(err == CL_DEVICE_NOT_FOUND) {
			printf("CPU\n");
		  err = clGetDeviceIDs(platform[i], CL_DEVICE_TYPE_CPU, 1, devices, &devNum);
	printf("CPU Devices: %d\n", devNum);
		}
		if(err < 0) {
		  perror("Couldn't access any devices");
		  exit(1);
		}
		for(j=0;j<devNum;j++)
		{
			device = devices[j];

				clGetDeviceInfo(devices[j], CL_DEVICE_NAME, valueSize, name, &valueSize);
				printf("Device name: %s\n", name);
				/* Obtain the device data */
				err = clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR,
					 sizeof(vector_width), &vector_width, NULL);
				if(err < 0) {
				  perror("Couldn't read device properties");
				  exit(1);
				}
				printf("Preferred vector width in chars: %u\n", vector_width);
				clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT,
					 sizeof(vector_width), &vector_width, NULL);
				printf("Preferred vector width in shorts: %u\n", vector_width);
				clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT,
					 sizeof(vector_width), &vector_width, NULL);
				printf("Preferred vector width in ints: %u\n", vector_width);
				clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG,
					 sizeof(vector_width), &vector_width, NULL);
				printf("Preferred vector width in longs: %u\n", vector_width);
				clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT,
					 sizeof(vector_width), &vector_width, NULL);
				printf("Preferred vector width in floats: %u\n", vector_width);
				clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE,
					 sizeof(vector_width), &vector_width, NULL);
				printf("Preferred vector width in doubles: %u\n", vector_width);

				#ifndef MAC
				clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF,
					 sizeof(vector_width), &vector_width, NULL);
				printf("Preferred vector width in halfs: %u\n", vector_width);
				#endif

		}

   }

   return 0;
}
