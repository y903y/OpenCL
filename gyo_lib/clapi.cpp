clapi::clapi()
{

}

clapi::startcalc()
{
  status = clGetPlatformIDs(2, platforms, &num_platforms);
  if (status != CL_SUCCESS || num_platforms <= 0)
  {   
    fprintf(stdout, "clGetPlatformIDs failed.\n");
    printf("%d\n",status);
  }
  properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[0], 0};
  //デバイスの取得
  status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_CPU, 1, &device_list[0], &num_device);

  context = clCreateContext(properties, num_device, &device_list[0], NULL, NULL, &status);

  cout <<"status: "<< status << endl;
  if(status != CL_SUCCESS)
  {   
    cout << "clCreateContext failed" << status<<endl;
  }   
  //create command_queue
  queue = clCreateCommandQueue(context, device_list[0], 0, &status);
  //create program_object
  program = clCreateProgramWithSource(context, 1, (const char**) &(source_str), &source_size, &status);
  //build program
  status = clBuildProgram(program, num_device, &device_list[0], NULL, NULL, NULL);
  status = clGetProgramInfo(program, CL_PROGRAM_NUM_DEVICES, sizeof(pro_info), &pro_info, NULL);
  //create kernel
  kernel = clCreateKernel(program, "calc", &status);
  //create mem_object
  memIn1 = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int)*fvar*fwid, (void*)mtrx1, &status);
  memIn2 = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int)*lvar*lwid, (void*)mtrx2, &status);
  memOut = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_int)*fvar*fwid, NULL, &status);
  //setKernelArg
  status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memIn1);
  status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&memIn2);
  status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&memOut);

  //do kernel
  status = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, globalsize, 0, 0, NULL, NULL);

  status = clEnqueueReadBuffer(queue, memOut, CL_TRUE, 0, sizeof(cl_int)*fvar*fwid, Out, 0, NULL, NULL);

  return Out;
}

clapi::builderror()
{
  if(status != CL_SUCCESS)
  {
    cout << "build failed \n";
    cl_program program_err=0;

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
}

clapi::~clapi()
{
  clReleaseMemObject(memOut);
  clReleaseMemObject(memIn1);
  clReleaseKernel(kernel);
  clReleaseProgram(program);
  clReleaseCommandQueue(queue);
  clReleaseContext(context);
}
