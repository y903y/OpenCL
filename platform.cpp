#include <CL/cl.h>
#include <iostream>

using namespace std;

int main(){

  /*--Platform取得--*/
  cl_platform_id id[3];//プラットホームID

  cl_int num = 3;//プラットホームを３つまで見つける
  cl_uint Rnum;//実際に取得したプラットホーム数
  cl_char name[1024];//プラットホーム名

  clGetPlatformIDs(num,id,&Rnum);
  cout<<"Platform Max Number = "<<Rnum<<endl<<endl;

  for(int i = 0;i < Rnum;i++)//プラットホームを探し、すべてのプラットホームを表示
  {

   clGetPlatformInfo(id[i],CL_PLATFORM_NAME,sizeof(name),name,NULL);
    cout<<"Platform Name = "<<name<<endl;

    /*--デバイス取得--*/
    cl_device_id dev[10];//デバイスID(構造体のアドレスが格納される)
    
    cl_uint DC;//実際に取得したデバイス数
    cl_char buff[1024];//デバイス名
    cl_int num1 = 10;
    clGetDeviceIDs(id[i],CL_DEVICE_TYPE_ALL,num1,dev,&DC);//プラットホームID,デバイスタイプ,
    cout<<"Device Max Number = "<<DC<<endl;

    for(int j= 0;j < DC;j++)
    {
      
      clGetDeviceInfo(dev[j],CL_DEVICE_NAME,sizeof(buff),buff,NULL);
      cout<<" Device Name = "<<buff<<endl;
    }
    cout << endl;
  }
}
