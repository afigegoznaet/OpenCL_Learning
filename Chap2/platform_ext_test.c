#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

int main() {

	/* Host data structures */
	cl_platform_id *platforms;
	cl_device_id *devices;
	cl_uint i,j, num_platforms, num_devices;
	cl_int err, platform_index = -1;

	char *current_pos;
	/* Extension data */
	char* ext_data;
	size_t ext_size;
	const char icd_ext[] = "cl_khr_icd";

	/* Find number of platforms */
	err = clGetPlatformIDs(1, NULL, &num_platforms);

	printf("Num platforms: %d\n",num_platforms);

	if(err < 0) {
		perror("Couldn't find any platforms.");
		exit(1);
	}

	/* Access all installed platforms */
	platforms = (cl_platform_id*)
		malloc(sizeof(cl_platform_id) * num_platforms);
	clGetPlatformIDs(num_platforms, platforms, NULL);

	/* Find extensions of all platforms */
	for(i=0; i<num_platforms; i++) {

/** clGetDeviceInfo(cl_device_id	 /* device */
/**					  cl_device_info  /* param_name */
/**					  size_t			 /* param_value_size */
/**					  void *			 /* param_value */
/**					  size_t *		  /* param_value_size_ret */

	char name[256], dev_ext_data[4096];
	int valueSize;
	clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 0, NULL, &valueSize);
	memset(name,'\0',valueSize);
	 err = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, valueSize, name, NULL);
	printf("Platform name: %s\n", name);


	err = clGetDeviceIDs(platforms[i],CL_DEVICE_TYPE_ALL,0, NULL, &num_devices);
	printf("Num devices: %d\n",num_devices);
	devices = (cl_device_id*) malloc(sizeof(cl_device_id) * num_devices);
	clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);

	for(j=0; j < num_devices; j++){
		clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 0, NULL, &valueSize);
		//memset(name,valueSize,'\0');
		clGetDeviceInfo(devices[j], CL_DEVICE_NAME, valueSize, name, &valueSize);
		printf("Device name: %u %d %s\n",devices[j],valueSize, name);

		clGetDeviceInfo(devices[j], CL_DEVICE_EXTENSIONS,
		4096 * sizeof(char), dev_ext_data, NULL);

		current_pos  = strchr(dev_ext_data, ' ');
		while (current_pos){
			*current_pos = '\n';
			current_pos = strchr(current_pos,' ');
		}

		printf("Dev EXTENSIONS: %s\n\n", dev_ext_data);

	}


		/* Find size of extension data */
		err = clGetPlatformInfo(platforms[i],
			CL_PLATFORM_EXTENSIONS, 0, NULL, &ext_size);
		if(err < 0) {
			perror("Couldn't read extension data.");
			exit(1);
		}

		/* Access extension data */
		ext_data = (char*)malloc(ext_size);
		clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS,
				ext_size, ext_data, NULL);

		current_pos  = strchr(ext_data, ' ');
		while (current_pos){
			*current_pos = '\n';
			current_pos = strchr(current_pos,' ');
		}
		printf("Platform %d supports extensions: %s\n\n", i, ext_data);

		/* Look for ICD extension */
		if(strstr(ext_data, icd_ext) != NULL) {
			free(ext_data);
			platform_index = i;
			 printf("Num platforms: %d %d\n",i, num_platforms);
		}else
		free(ext_data);
	}

	/* Display whether ICD extension is supported */
	if(platform_index > -1)
		printf("Platform %d supports the %s extension.\n",
				platform_index, icd_ext);
	else
		printf("No platforms support the %s extension.\n", icd_ext);

	/* Deallocate resources */
	free(platforms);
	return 0;
}
