/*
 * clapi.cpp
 *
 *  Created on: 2012/11/08
 *      Author: y-kuwa
 */
#include "clapi.h"
#include <cstdio>
#include <iostream>

using namespace std;

clapi::clapi() {

}


clapi::clapi(string tmp){
  filename=tmp;
}


clapi::~clapi(){
  clReleaseMemObject(memOut);
  for(int i=0;i < num_hikisu;i++) clReleaseMemObject(memIn[i]);
  clReleaseKernel(kernel);
  clReleaseProgram(program);
  clReleaseCommandQueue(queue);
  clReleaseContext(context);
}


bool clapi::hikisu(int n, ...) {
  num_hikisu = n;
  va_list l;
  va_start(l, n);

  for (int t = 0; t < n ; t++) { //forでループさせ渡された引数１個ずつについて処理する。すべてsに+=していく。
    size[t] = va_arg(l,int);//double型配列の個数
    s[t] = va_arg(l,double*); //可変長引数を取り出す. 第一引数はva_list型の変数。第二引数には取り出す型。
  }
  return true;
}
bool clapi::doOpenCL() {
  status = clGetPlatformIDs(2, platforms, &num_platforms);
  if (status != CL_SUCCESS || num_platforms <= 0) {
    fprintf(stdout, "clGetPlatformIDs failed.\n");
    printf("%d\n", status);
    return false;
  }
  // 最初の要素として返されたプラットフォームIDを、プロパティにセットする
  properties[0] = CL_CONTEXT_PLATFORM;
  properties[1]=(cl_context_properties)platforms[0];
  properties[2]=0;

  //1.デバイスの取得
  status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_CPU, 1, &device_list[0], &num_device);

  context = clCreateContext(properties, num_device, &device_list[0], NULL,NULL, &status);

  if (status != CL_SUCCESS) {
    cout << "clCreateContext failed" << status << endl;
  }

  //3.コマンドキューの作成
  queue = clCreateCommandQueue(context, device_list[0], 0, &status);
  
  //4.プログラムオブジェクトの作成
  FILE *fp;
  size_t source_size;
  char *source_str;

  fp = fopen(filename.c_str(), "r");
  if (!fp) {
    fprintf(stderr, "Failed to leas kernel.\n");
    return false;
  }
  source_str = (char *) malloc(MAX_SOURCE_SIZE);
  source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
  fclose(fp);

  program = clCreateProgramWithSource(context, 1, (const char**) &(source_str), &source_size, &status);
  //5.プログラムのビルド
  status = clBuildProgram(program, num_device, &device_list[0], NULL, NULL, NULL);

  //build error
  if (status != CL_SUCCESS) {
    builderr();
    return false;
  }

  status = clGetProgramInfo(program, CL_PROGRAM_NUM_DEVICES, sizeof(pro_info), &pro_info, NULL);

  //6.カーネルの作成
  kernel = clCreateKernel(program, "calc", &status);

  //7メモリオブジェクトの作成
  for(int i = 0 ; i<num_hikisu ; i++)
  {
    memIn[i] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(double)*size[i], (void*) s[i], &status);
  }
  memOut = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(double)*size[1] , NULL, &status);

  //8.カーネルに渡す引数の設定
  status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *) &memOut);

  for(int i = 0; i< num_hikisu; i++)
  {
    status = clSetKernelArg(kernel,i,sizeof(cl_mem),(void *) &memIn[i]);
  }

  //9.カーネルの実行
  size_t globalsize[] = { size[0] };
  status = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, globalsize, NULL, 0, NULL, NULL);

  //10.結果の取得
  Out = (double*) malloc(size[0] * sizeof(double));
  status = clEnqueueReadBuffer(queue, memOut, CL_TRUE, 0, sizeof(double)*size[1], Out, 0, NULL, NULL);

  return true;
}


double* clapi::getOut(){
  return Out;
}


void clapi::builderr() {
  cout << "build failed \n";
  cl_program program_err;

  size_t logsize;
  status = clGetProgramBuildInfo(program_err, device_list[0],
      CL_PROGRAM_BUILD_LOG, 0, NULL, &logsize);
  if (status != CL_SUCCESS) {
    //ログを格納するためのバッファをアロケートする
    char *logbuffer;
    logbuffer = new char[logsize + 1];
    if (logbuffer == NULL) {
      printf("memory allocation failed.\n");
      //return;

      status = clGetProgramBuildInfo(program_err, device_list[0],
          CL_PROGRAM_BUILD_LOG, logsize, logbuffer, NULL);
      if (status == CL_SUCCESS) {
        logbuffer[logsize] = '\0';
        cout << "build log" << endl;
        cout << logbuffer << endl;
      }
      delete[] logbuffer;
    }
  }
}

