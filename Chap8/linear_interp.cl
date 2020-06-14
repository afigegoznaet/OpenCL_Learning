#ifdef FP_64
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#endif

__kernel void add(__global float *a, __global float *b, __global float *c) {

	*c = *a + *b;
}

__kernel void
LinearInterpolateDD(const __global double *dfX1, const __global double *dfY1,
					const __global double *dfX2, const __global double *dfY2,
					const __global double *dfX, __global double *dfY) {
	*dfY = (*dfY1 * (*dfX2 - *dfX) + *dfY2 * (*dfX - *dfX1)) / (*dfX2 - *dfX1);
}


__kernel void LinearInterpolateDDParallel(const __global double *Xs,
										  const __global double *Ys,
										  const __global double *dfX,
										  __global double *		 dfY) {
	size_t xIndex = get_global_id(0);
	double X = dfX[xIndex];
	double X1 = Xs[xIndex * 10];
	double Y1 = Ys[xIndex * 10];
	double X2 = Xs[xIndex * 10 + 1];
	double Y2 = Ys[xIndex * 10 + 1];
	dfY[xIndex] = (Y1 * (X2 - X) + Y2 * (X - X1)) / (X2 - X1);
}
