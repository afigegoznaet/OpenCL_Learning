#ifdef FP_64
#pragma OPENCL EXTENSION cl_khr_fp64: enable
#endif

__kernel void add(__global float *a,
                  __global float *b,
                  __global float *c) {
   
   *c = *a + *b;
}

__kernel void LinearInterpolateDD(const __global  double *dfX1, const __global double *dfY1,
								  const __global  double *dfX2, const __global double *dfY2,
								  const __global  double *dfX, __global double *dfY){
	*dfY = (*dfY1 * (*dfX2 - *dfX) + *dfY2 * (*dfX - *dfX1)) / (*dfX2 - *dfX1);
}
