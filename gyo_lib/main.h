#include<iostream>
#include<cstdlib>
#include<string>
#include<cstdio>
#include<time.h>

#ifdef __APPLE__
#include<OpenCL/opencl.h>
#else
#include<CL/cl.h>
#endif //__APPLE

#define MAX_SOURCE_SIZE (0x100000)

float *cl_api(int fvar, int fwid, float *mtrx1, int lvar, int lwid, float *mtrx2, int size, float *Out);
