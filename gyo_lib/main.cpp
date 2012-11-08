/*  * このプログラムはinputD.txtから数値を読み込んで
 * 計算時間をはかるOpenCLプログラムです
 * */

#include"api.h"

#define MAX_SOURCE_SIZE (0x100000)

using namespace std;

//float cl_api(int fvar, int fwid, float *mtrx1, int lvar, int lwid, float *mtrx2,int size, float *Out);

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
	cl_api(fvar,fwid,mtrx1,lvar,lwid,mtrx2,size,Out);

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
