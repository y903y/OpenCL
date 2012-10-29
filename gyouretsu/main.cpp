/*  * このプログラムはinputD.txtから数値を読み込んで
 * 計算時間をはかるOpenCLプログラムです
 * */


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

using namespace std;

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

  // プラットフォームIDを取得する
  cl_int status, status_cpu, status_gpu;
  cl_platform_id platforms[2];
  cl_uint num_platforms;
  status = clGetPlatformIDs(2, platforms, &num_platforms);
  if (status != CL_SUCCESS || num_platforms <= 0) 
  { 
    fprintf(stdout, "clGetPlatformIDs failed.\n");
    printf("%d\n",status);
    return 1;
  }
  // 最初の要素として返されたプラットフォームIDを、プロパティにセットする
  cl_context_properties properties_cpu[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[0], 0};
  cl_context_properties properties_gpu[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[1], 0};

  //1.デバイスの取得
  cl_device_id device_list[4];//なぜか2じゃできない
  cl_uint num_device_cpu, num_device_gpu;
  status_cpu = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_CPU, 1, &device_list[0], &num_device_cpu);
  status_gpu = clGetDeviceIDs(platforms[1], CL_DEVICE_TYPE_GPU, 4, &device_list[1], &num_device_gpu);
  //cout <<"cpu: "<< status_cpu <<" "<< device_list[0] <<" "<<num_device_cpu<<" gpu: "<< status_gpu << " " << device_list[1] << " " << num_device_gpu << endl;

  //2.コンテキスト作成
  cl_context context_cpu, context_gpu;
  context_cpu = clCreateContext(properties_cpu, num_device_cpu, &device_list[0], NULL, NULL, &status_cpu);
  context_gpu = clCreateContext(properties_gpu, num_device_gpu, &device_list[1], NULL, NULL, &status_gpu);

  cout <<"status: "<< status_cpu << " "<< status_gpu << endl;
  if(status != CL_SUCCESS | status != CL_SUCCESS)
  {
    printf("clCreateContext failed %d.\n",status_cpu);
    cout << "clCreateContext failed" << status_gpu<<endl;
  }

  //cout << "test\n";
  //3.コマンドキューの作成
  cl_command_queue queue_c, queue_g;
  queue_c = clCreateCommandQueue(context_cpu, device_list[0], 0, &status_cpu);
  queue_g = clCreateCommandQueue(context_gpu, device_list[1], 0, &status_gpu);
  //cout << "test2\n";
  //4.プログラムオブジェクトの作成
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
  
  cl_program program_cpu, program_gpu;
  program_cpu = clCreateProgramWithSource(context_cpu, 1, (const char**) &(source_str), &source_size, &status_cpu);
  program_gpu = clCreateProgramWithSource(context_gpu, 1, (const char**) &(source_str), &source_size, &status_gpu);
  //cout << "clCreatePWS end\n";
  //5.プログラムのビルド
  status = clBuildProgram(program_cpu, num_device_cpu, &device_list[0], NULL, NULL, NULL);
  status = clBuildProgram(program_gpu, num_device_gpu, &device_list[1], NULL, NULL, NULL);
  
  //build error
  if(status_cpu != CL_SUCCESS | status_gpu != CL_SUCCESS)
  {
    cout << "build failed \n";
    cl_program program_err;
    for(int i = 0; i<4 ; i++)
    {
      if(i=0) program_err = program_cpu;
      else if(i=1) program_err = program_gpu; 
      size_t logsize;
      status = clGetProgramBuildInfo(program_err, device_list[i], CL_PROGRAM_BUILD_LOG, 0, NULL, &logsize);
      if(status != CL_SUCCESS){
        //ログを格納するためのバッファをアロケートする
        char *logbuffer;
        logbuffer = new char[logsize +1];
        if(logbuffer == NULL){
          printf("memory allocation failed.\n");
          //return;
        }

        status = clGetProgramBuildInfo(program_err, device_list[i], CL_PROGRAM_BUILD_LOG, logsize, logbuffer, NULL);
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
  /*
  size_t logsize;
  cout <<"test"<<endl;
  cl_build_status *build_status;
  status = clGetProgramBuildInfo(program_cpu, device_list[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &logsize);
  status_cpu = clGetProgramBuildInfo(program_cpu, device_list[0], CL_PROGRAM_BUILD_STATUS, logsize, build_status, NULL);
  cout << "test"<<endl;
  cout << status_cpu <<" "<<build_status<< endl;
  status = clGetProgramBuildInfo(program_gpu, device_list[1], CL_PROGRAM_BUILD_LOG, 0, NULL, &logsize);
  status_gpu = clGetProgramBuildInfo(program_gpu, device_list[1], CL_PROGRAM_BUILD_STATUS, logsize, build_status, NULL);
  cout << status_gpu <<" "<<build_status<< endl;
  */
  cl_uint pro_info;
  status_cpu = clGetProgramInfo(program_cpu, CL_PROGRAM_NUM_DEVICES, sizeof(pro_info), &pro_info, NULL);
  cout << "cpu_pro_info: "<< status_cpu << " " <<pro_info<<endl;
  status_gpu = clGetProgramInfo(program_gpu, CL_PROGRAM_NUM_DEVICES, sizeof(pro_info), &pro_info, NULL);
  cout << "gpu_pro_info: " << status_gpu << " " << pro_info << endl;

  //6.カーネルの作成
  cl_kernel kernel_c, kernel_g;
  kernel_c = clCreateKernel(program_cpu, "calc", &status_cpu);
  kernel_g = clCreateKernel(program_gpu, "calc", &status_gpu);
  //cout << "create kernel end\n";
  //7メモリオブジェクトの作成
  //7-1CPU
  cl_mem cmemIn1;
  cmemIn1 = clCreateBuffer(context_cpu, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int)*fvar*fwid, mtrx1, &status_cpu);
  cl_mem cmemIn2;
  cmemIn2 = clCreateBuffer(context_cpu, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int)*lvar*lwid, mtrx2, &status_cpu);
  cl_mem cmemOut;
  cmemOut = clCreateBuffer(context_cpu, CL_MEM_WRITE_ONLY, sizeof(cl_int)*fvar*fwid, NULL, &status_cpu);
  //7-2GPU
  cl_mem gmemIn1;
  gmemIn1 = clCreateBuffer(context_gpu, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int)*fvar*fwid, mtrx1, &status_gpu);
  cl_mem gmemIn2;
  gmemIn2 = clCreateBuffer(context_gpu, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int)*lvar*lwid, mtrx2, &status_gpu);
  cl_mem gmemOut;
  gmemOut = clCreateBuffer(context_gpu, CL_MEM_WRITE_ONLY, sizeof(cl_int)*fvar*fwid, NULL, &status_gpu);
  //cout << "mem end"<< endl;
  //8.カーネルに渡す引数の設定
  //8-1CPU
  status = clSetKernelArg(kernel_c, 0, sizeof(cl_mem), (void *)&cmemIn1);
  status = clSetKernelArg(kernel_c, 1, sizeof(cl_mem), (void *)&cmemIn2);
  status = clSetKernelArg(kernel_c, 2, sizeof(cl_mem), (void *)&cmemOut);
  //8-2GPU
  status = clSetKernelArg(kernel_g, 0, sizeof(cl_mem), (void *)&gmemIn1);
  status = clSetKernelArg(kernel_g, 1, sizeof(cl_mem), (void *)&gmemIn2);
  status = clSetKernelArg(kernel_g, 2, sizeof(cl_mem), (void *)&gmemOut);
  //cout <<"set end"<<endl;
  //イベント
  //cl_int wait;
  cl_event gpu;
  cl_int exec_status;
  size_t event_size;
  char event[1000];
  cl_command_type command_type;
  //status = clWaitForEvents(1, *wait);

  //9.カーネルの実行
  //cout << "実行"<<endl;
  size_t globalsize[] = {fvar*fwid};
  //cout << "globalsizeal size" <<endl;

  clock_t ct3, ct4, gt3, gt4;
  gt3 = clock();
  status_gpu = clEnqueueNDRangeKernel(queue_g, kernel_g, 1, NULL, globalsize, 0, 0, NULL, &gpu);
  //cout <<"do kernel: "<< status_gpu <<endl;
  gt4 = clock();
  /*status = clGetEventInfo(gpu, CL_EVENT_COMMAND_QUEUE, sizeof(char)*1000, event, &event_size);
    cout << status << " " << event << endl;
    status = clGetEventInfo(gpu, CL_EVENT_COMMAND_TYPE, sizeof(char)*1000, event, &event_size);
    cout << status << " " << event << endl;
    status = clGetEventInfo(gpu, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(exec_status), &exec_status, NULL);
    cout << status << "! " ;
    switch (exec_status)
    {
    case CL_QUEUED:
    cout << "CL_QUEUED" << endl;
    break;
    case CL_SUBMITTED:
    cout << "CL_SUBMITTED" << endl;
    break;
    case CL_RUNNING:
    cout << "CL_RUNNING" << endl;
    break;
    case CL_COMPLETE:
    cout << "CL_COMPLETE" << endl;
    break;
    }
    status = clGetEventInfo(gpu, CL_EVENT_REFERENCE_COUNT, sizeof(char)*1000, event, &event_size);
    cout << status << " " << event << endl;*/
  cout << "GPU計算終了！！！！！！！！！！！！！！" << endl;

  ct3 = clock();
  status_cpu = clEnqueueNDRangeKernel(queue_c, kernel_c, 1, NULL, globalsize, 0, 0, NULL, NULL);
  ct4 = clock();
  //status = clGetEventInfo(gpu, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(exec_status), &exec_status, NULL);
  //cout << status << " " << exec_status << endl ;
  cout << "CPU計算終了！！！！！！！！！！！！！！" << endl;

  cout <<"実行結果　cpu:" <<status_cpu << " gpu:" << status_gpu << endl;

  //10.結果の取得
  float *cOut, *gOut;
  cOut = (float*) malloc(size * size * sizeof(float));
  gOut = (float*) malloc(size * size * sizeof(float));
  status_cpu = clEnqueueReadBuffer(queue_c, cmemOut, CL_TRUE, 0, sizeof(cl_int)*fvar*fwid, cOut, 0, NULL, NULL);
  status_gpu = clEnqueueReadBuffer(queue_g, gmemOut, CL_TRUE, 0, sizeof(cl_int)*fvar*fwid, gOut, 0, NULL, NULL);
  cout << status_cpu << " " << status_gpu << endl;

  //結果表示
  /*cout<<"加算結果"<<endl;
    for(int i = 0 ; i < fvar ; i++){
    for(int j = 0 ; j < fwid ; j++){
    cout<< gOut[i*fwid+j] << " " ;
    }
    cout << endl;
    }
  */

  //11.リソースの開放
  clReleaseMemObject(cmemOut);
  clReleaseMemObject(cmemIn1);
  clReleaseKernel(kernel_c);
  clReleaseProgram(program_cpu);
  clReleaseCommandQueue(queue_c);
  clReleaseContext(context_cpu);

  clReleaseMemObject(gmemOut);
  clReleaseMemObject(gmemIn1);
  clReleaseKernel(kernel_g);
  clReleaseProgram(program_gpu);
  clReleaseCommandQueue(queue_g);
  clReleaseContext(context_gpu);

  t2 = clock();
  printf("all time = %f\n", (double)(t2 - t1) / CLOCKS_PER_SEC);

  cout << size << endl;
  //逐次実行
  t1 = clock();
  for(i=0;i<size;i++){
    for(j=0;j<size;j++){
      Out[i*size+j] = mtrx1[i*size+j] + mtrx2[i*size+j];
    }
  }
  t2 = clock();
  printf("cpu calc time = %f\n", (double)(ct4 - ct3) / CLOCKS_PER_SEC);
  printf("gpu calc time = %f\n", (double)(gt4 - gt3) / CLOCKS_PER_SEC);
  printf("sequential time = %f\n", (double)(t2 - t1) / CLOCKS_PER_SEC);

  free(mtrx1);
  free(mtrx2);
  free(Out);
}
