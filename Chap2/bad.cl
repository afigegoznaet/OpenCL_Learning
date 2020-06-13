__kernel void bad(__global flot *a,
                   __global float *b,
                   __global float *c) {
   
   *c = *a + *b;
}
