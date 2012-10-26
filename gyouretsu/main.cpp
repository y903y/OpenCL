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

/*手入力用  
  int fvar, fwid, lvar, lwid;
	cout << "行列の大きさを指定して下さい(ex. 3 4 3 4)\n" << endl;
	cin >> fvar;
	cin >> fwid;
	cin >> lvar;
	cin >> lwid;

	int *mtrx1, *mtrx2, *Out;
	mtrx1 = (int*)malloc(sizeof(int)*fvar*fwid);
	mtrx2 = (int*)malloc(sizeof(int)*lvar*lwid);
	Out	  = (int*)malloc(sizeof(int)*lvar*lwid);

	cout << "1つめの行列の要素を入力して下さい(ex. 3 3 3)\n" << endl;
	for(int i=0; i<fvar*fwid;i++){
		cin >> mtrx1[i];	
	}
	cout << "2つめの行列の要素を入力して下さい(ex. 3 3 3)\n" << endl;
	for(int i = 0; i < lvar * lwid; i++){
		cin >> mtrx2[i];
	}
*/

 //ファイル読み込み用
 int fvar=4096, fwid=4096, lvar=4096, lwid=4096;
 float *mtrx1,*mtrx2,*Out;
 FILE *fp;
 if ((fp = fopen("inputD.txt","r")) == NULL){
    printf("file open error!!\n");
    exit(EXIT_FAILURE);
 }
 int size;
 fscanf(fp, "%d", &size);
  
 mtrx1 = (float*) malloc(size * size * sizeof(float));
 mtrx2 = (float*) malloc(size * size * sizeof(float));
 Out = (float*) malloc(size * size * sizeof(float));

 int i, j;
 for(i=0;i<size;i++){
  for(j=0;j<size;j++){
    Out[i*size+j] = 0;
    fscanf(fp, "%f", &mtrx1[i * size + j]);
  }
 }

 for(i=0;i<size;i++){
  for(j=0;j<size;j++){
    fscanf(fp,"%f", &mtrx2[i * size + j]);
  }
 }

 fclose(fp);

  /*for(int i = 0; i < fvar * (wid; i++){
		cout << mtrx1[i] << endl;
	}

	cout<< "\n" <<endl;

	for(int i = 0; i < fvar*fwid; i++){
		cout << mtrx2[i] << endl;
	}*/

	// プラットフォームIDを取得する
	cl_int status;
	cl_platform_id platforms[10];
	cl_uint num_platforms;
	status = clGetPlatformIDs(2,
		platforms,
		&num_platforms);
	if (status != CL_SUCCESS || num_platforms <= 0) {        
		fprintf(stdout, "clGetPlatformIDs failed.\n");
		printf("%d\n",status);
		return 1;
	}

	// 最初の要素として返されたプラットフォームIDを、プロパティにセットする
	cl_context_properties properties[]
	= {CL_CONTEXT_PLATFORM,
    (cl_context_properties)platforms[1],
    0};


	//1.コンテキスト作成
	//	cl_int status;
	cl_context context;
	context = clCreateContextFromType(
		properties,
		CL_DEVICE_TYPE_GPU,
		NULL,
		NULL,
		&status);

	if(status != CL_SUCCESS){
		printf("clCreateContextFromType failed %d.\n",status);
	}

	//2.デバイスの取得
	static const int MaxDevices = 10;
	cl_device_id devices[MaxDevices];
	size_t size_return;
	status = clGetContextInfo(context,
		CL_CONTEXT_DEVICES,
		sizeof(devices),
		devices,
		&size_return);

	//3.コマンドキューの作成
	cl_command_queue queue;
	queue = clCreateCommandQueue(
		context,
		devices[0],
		0,
		&status);

	//4.プログラムオブジェクトの作成
	//FILE *fp;
	//errno_t err;
	size_t source_size;
	char *source_str;
	const char fileName[] = "./calc.cl";

	fp = fopen(fileName, "r");
	//err = fopen_s(&fp,fileName,"r");
	if(!fp){
		fprintf(stderr, "Failed to leas kernel.\n");
		exit(1);
	}
	source_str = (char *)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str,1,MAX_SOURCE_SIZE, fp);
	fclose(fp);

	cl_program program;
	program= clCreateProgramWithSource(
		context,
		1,
		(const char**) &(source_str),
		&source_size,
		&status);

	//5.プログラムのビルド
	size_t logsize;
	status = clGetProgramBuildInfo(program,
		devices[0],
		CL_PROGRAM_BUILD_LOG,
		0,
		NULL,
		&logsize);
	if(status != CL_SUCCESS){
		//ログを格納するためのバッファをアロケートする
		char *logbuffer;
		logbuffer = new char[logsize +1];
		if(logbuffer == NULL){
			printf("memory allocation failed.\n");
			return -1;
	}

	status = clGetProgramBuildInfo(
		program,
		devices[0],
		CL_PROGRAM_BUILD_LOG,
		logsize,
		logbuffer,
		NULL
		);
	//logbuffer[logsize - 1] = '\0'; 
	//printf("%s\n",logbuffer);
	}
	status = clBuildProgram(
		program,
		1,
		devices,
		NULL,
		NULL,
		NULL);

	//6.カーネルの作成
	cl_kernel kernel;
	kernel = clCreateKernel(program, "calc",&status);

	//7メモリオブジェクトの作成
	cl_mem memIn1;
	memIn1 = clCreateBuffer(
		context,
		CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(cl_int)*fvar*fwid,
		mtrx1,
		&status);

	cl_mem memIn2;
	memIn2 = clCreateBuffer(
		context,
		CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(cl_int)*lvar*lwid,
		mtrx2,
		&status);
	//printf("memIn2 = %d",memIn2);

	cl_mem memOut;
	memOut = clCreateBuffer(
		context,
		CL_MEM_WRITE_ONLY,
		sizeof(cl_int)*fvar*fwid,
		NULL,
		&status);

	//8.カーネルに渡す引数の設定
	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memIn1);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&memIn2);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&memOut);

	//9.カーネルの実行
	size_t globalsize[] = {fvar*fwid};
  
  clock_t t3, t4;
  t3 = clock();
	status = clEnqueueNDRangeKernel(
		queue,
		kernel,
		1,
		NULL,
		globalsize,
		0,
		0,
		NULL,
		NULL);
  t4 = clock();
  printf("calc time = %f\n", (double)(t4 - t3) / CLOCKS_PER_SEC);

	//10.結果の取得
	status = clEnqueueReadBuffer(
		queue,
		memOut,
		CL_TRUE,
		0,
		sizeof(cl_int)*fvar*fwid,
		Out,
		0,
		NULL,
		NULL);
	//結果表示
	/*cout<<"加算結果"<<endl;
	for(int i = 0 ; i < fvar ; i++){
		for(int j = 0 ; j < fwid ; j++){
			cout<< Out[i*fwid+j] << " " ;
		}
		cout << endl;
	}
  */

	//11.リソースの開放
	clReleaseMemObject(memOut);
	clReleaseMemObject(memIn1);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

  t2 = clock();
  printf("time = %f\n", (double)(t2 - t1) / CLOCKS_PER_SEC);

  cout << size << endl;
  //逐次実行
  t1 = clock();
  for(i=0;i<size;i++){
    for(j=0;j<size;j++){
      Out[i*size+j] = mtrx1[i*size+j] + mtrx2[i*size+j];
    }
  }
  t2 = clock();
  printf("sequential time = %f\n", (double)(t2 - t1) / CLOCKS_PER_SEC);

  free(mtrx1);
  free(mtrx2);
  free(Out);
}
