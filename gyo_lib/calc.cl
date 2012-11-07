
__kernel void
calc(
__global const float *in1,
__global const float *in2,
__global float *out)

{
int index = get_global_id(0);
out[index] = in1[index]+in2[index];
}
