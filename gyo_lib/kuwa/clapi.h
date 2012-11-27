/*
 * clapi.h
 *
 */

#ifndef CLAPI_H_
#define CLAPI_H_

#ifdef __APPLE__
#include<OpenCL/opencl.h>
#else
#include<CL/cl.h>
#endif //__APPLE

#include<stdarg.h>
#include<string>
#include<cstdio>
#include<iostream>

#define MAX_SOURCE_SIZE (0x100000)

using namespace std;

class clapi {
public:
	clapi();
	clapi(string);
	~clapi();
	bool clauto(int , ...);
	bool doOpenCL();
	double* getOut();
        //cl_device_id device_list[5];
private:
	void builderr();
	cl_int status;
	cl_platform_id platforms[2];
	cl_uint num_platforms;
	cl_context context;
	//cl_device_id device_list[4]; //なぜか2じゃできない
	cl_uint num_device;
        cl_uint ndt;
	//cl_context_properties properties[3];
	
        cl_command_queue queue;

        cl_command_queue queue2;
        cl_command_queue queue3;


	cl_program program;
	cl_uint pro_info;
	cl_kernel kernel;
	cl_mem memIn[10];
	cl_mem memOut;
	string filename;
	double* s[10];
	int size[10];
	int num_hikisu;
	double* Out;
};

#endif /* CLAPI_H_ */
