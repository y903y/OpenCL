#include"main.h"

using namespace std;

float * cl_api(int fvar, int fwid, float *mtrx1, int lvar, int lwid, float *mtrx2,int size, float *Out)
{
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
        source_str = (char *)malloc(MAX_SOURCE_SIZE);
        source_size = fread(source_str,1,MAX_SOURCE_SIZE, fp);
        fclose(fp);

        ///////////////////////////////////////////////////////////////
        //build error
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
        ///////////////////////////////////////////////////////////////

        //9.カーネルの実行
        size_t globalsize[] = {fvar*fwid};

        //10.結果の取得
        //float *Out;
        //Out = (float*) malloc(size * size * sizeof(float));

        return Out;

}
