/*  * このプログラムはinputD.txtから数値を読み込んで
 * 計算時間をはかるOpenCLプログラムです
 * */


#include<iostream>
#include<cstdlib>
#include<string>
#include<cstdio>
#include<time.h>
#include"api.h"

#define MAX_SOURCE_SIZE (0x100000)

using namespace std;

float cl_api(int fvar, int fwid, float *mtrx1, int lvar, int lwid, float *mtrx2,int size, float *Out);

int main()
{
  clock_t t1, t2;
  t1 = clock();

  //ファイル読み込み用
  int fvar=4096, fwid=4096, lvar=4096, lwid=4096;
  float *mtrx1,*mtrx2,*Out;
  FILE *fp;
  if ((fp = fopen("inputD.txt","r")) == NULL)
  {
    printf("file open error!!\n");
    exit(EXIT_FAILURE);
  }
  int size;
  fscanf(fp, "%d", &size);

  mtrx1 = (float*) malloc(size * size * sizeof(float));
  mtrx2 = (float*) malloc(size * size * sizeof(float));
  Out = (float*) malloc(size * size * sizeof(float));

  int i, j;
  for(i=0;i<size;i++)
  {
    for(j=0;j<size;j++)
    {
      Out[i*size+j] = 0;
      fscanf(fp, "%f", &mtrx1[i * size + j]);
    }
  }

  for(i=0;i<size;i++)
  {
    for(j=0;j<size;j++)
    {
      fscanf(fp,"%f", &mtrx2[i * size + j]);
    }
  }

  fclose(fp);

  //OPENCLAPI
  *Out = cl_api(fvar,fwid,mtrx1,lvar,lwid,mtrx2,size,Out);

  //結果表示
  cout<<"加算結果"<<endl;
  for(int i = 0 ; i < fvar ; i++){
    for(int j = 0 ; j < fwid ; j++){
      cout<< Out[i*fwid+j] << " " ;
    }
    cout << endl;
  } 

  free(mtrx1);
  free(mtrx2);
  free(Out);
}

/*
float cl_api(int fvar, int fwid, float *mtrx1, int lvar, int lwid, float *mtrx2,int size, float *Out)
{
  // プラットフォームIDを取得する
  //1.デバイスの取得
  //2.コンテキスト作成
  cl_int status;
  cl_platform_id platforms[2];
  cl_uint num_platforms;
  cl_context context;
  cl_device_id device_list[4];//なぜか2じゃできない
  cl_uint num_device;

  status = clGetPlatformIDs(2, platforms, &num_platforms);
  if (status != CL_SUCCESS || num_platforms <= 0)
  {
    fprintf(stdout, "clGetPlatformIDs failed.\n");
    printf("%d\n",status);
    return 1;
  }
  // 最初の要素として返されたプラットフォームIDを、プロパティにセットする
  cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[0], 0}; 

  //1.デバイスの取得
  status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_CPU, 1, &device_list[0], &num_device);

  context = clCreateContext(properties, num_device, &device_list[0], NULL, NULL, &status);

  cout <<"status: "<< status << endl;
  if(status != CL_SUCCESS)
  {
    cout << "clCreateContext failed" << status<<endl;
  }

  //3.コマンドキューの作成
  cl_command_queue queue;
  queue = clCreateCommandQueue(context, device_list[0], 0, &status);

  //4.プログラムオブジェクトの作成
  FILE *fp;
  size_t source_size;
  char *source_str;
  const char fileName[] = "./calc.cl";

  fp = fopen(fileName, "r");
  if(!fp){
    fprintf(stderr, "Failed to leas kernel.\n");
    exit(1);
  }
  //cout << "malloc begin\n";
  source_str = (char *)malloc(MAX_SOURCE_SIZE);
  //cout << "malloc end\n";
  source_size = fread(source_str,1,MAX_SOURCE_SIZE, fp);
  //cout << "fread end\n";
  fclose(fp);

  cl_program program;
  program = clCreateProgramWithSource(context, 1, (const char**) &(source_str), &source_size, &status);
  //cout << "clCreatePWS end\n";
  //5.プログラムのビルド
  status = clBuildProgram(program, num_device, &device_list[0], NULL, NULL, NULL);

  //build error
  if(status != CL_SUCCESS)
  {
    cout << "build failed \n";
    cl_program program_err;

    size_t logsize;
    status = clGetProgramBuildInfo(program_err, device_list[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &logsize);
    if(status != CL_SUCCESS){
      //ログを格納するためのバッファをアロケートする
      char *logbuffer;
      logbuffer = new char[logsize +1];
      if(logbuffer == NULL){
        printf("memory allocation failed.\n");
        //return;

        status = clGetProgramBuildInfo(program_err, device_list[0], CL_PROGRAM_BUILD_LOG, logsize, logbuffer, NULL);
        if(status == CL_SUCCESS)
        {
          logbuffer[logsize] = '\0';
          cout << "build log" << endl;
          cout << logbuffer << endl;
        }
        delete [] logbuffer;
      }
    }
  }

  cl_uint pro_info;
  status = clGetProgramInfo(program, CL_PROGRAM_NUM_DEVICES, sizeof(pro_info), &pro_info, NULL);
  cout << "pro_info: "<< status<<endl;

  //6.カーネルの作成
  cl_kernel kernel;
  kernel = clCreateKernel(program, "calc", &status);
  //7メモリオブジェクトの作成
  //7-1CPU
  cl_mem memIn1;
  memIn1 = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int)*fvar*fwid, (void*)mtrx1, &status);
  cout <<"mem: "<<status<<" ";

  cl_mem memIn2;
  memIn2 = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int)*lvar*lwid, (void*)mtrx2, &status);
  cout << status <<" ";

  cl_mem memOut;
  memOut = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_int)*fvar*fwid, NULL, &status);
  cout<<status<<endl;
  cout << "mem end"<< endl;
  //8.カーネルに渡す引数の設定
  //8-1CPU
  status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memIn1);
  status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&memIn2);
  status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&memOut);

  //9.カーネルの実行
  size_t globalsize[] = {fvar*fwid};
  status = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, globalsize, 0, 0, NULL, NULL);

  //10.結果の取得
  //float *Out;
  //Out = (float*) malloc(size * size * sizeof(float));
  status = clEnqueueReadBuffer(queue, memOut, CL_TRUE, 0, sizeof(cl_int)*fvar*fwid, Out, 0, NULL, NULL);

  return *Out;

  //11.リソースの開放
  clReleaseMemObject(memOut);
  clReleaseMemObject(memIn1);
  clReleaseKernel(kernel);
  clReleaseProgram(program);
  clReleaseCommandQueue(queue);
  clReleaseContext(context);

}
*/
