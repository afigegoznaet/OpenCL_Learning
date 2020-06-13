constant sampler_t sampler =
	CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_LINEAR;

__kernel void interp(read_only image2d_t  src_image,
					 write_only image2d_t dst_image) {

	float4 pixel;
	int	   scl = 3;
	size_t x = get_global_id(0);
	size_t y = get_global_id(1);

	/* Determine input coordinate */
	float2 input_coord = (float2)(x, y);

	/* Determine output coordinate */
	int2 output_coord = (int2)(scl * x, scl * y);


	/* Compute interpolation */
	for (int i = 0; i < scl; i++) {
		for (int j = 0; j < scl; j++) {
			pixel = read_imagef(
				src_image, sampler,
				(float2)(input_coord
						 + (float2)(1.0f * i / scl, 1.0f * j / scl)));

			write_imagef(dst_image, output_coord + (int2)(i, j), pixel);
		}
	}
}
