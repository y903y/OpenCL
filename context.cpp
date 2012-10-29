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
    cout<<"Platform Name{"<< id[i] <<"} = "<<name<<endl;

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


  /*context作成*/
  //cl_context_properties properties_cpugpu[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)id[0],(cl_context_properties)id[1], 0}; 
  cl_context_properties properties_gpu[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)id[1], 0};    
  cl_device_id device_list_cpu, device_list_gpu;
  cl_uint num_devices_cpu, num_devices_gpu;
  cl_int getdevice;
  getdevice = clGetDeviceIDs(id[0], CL_DEVICE_TYPE_ALL, 4, &device_list_cpu, &num_devices_cpu);
  cout << getdevice << endl;
  getdevice = clGetDeviceIDs(id[1], CL_DEVICE_TYPE_ALL, 4, &device_list_gpu, &num_devices_gpu);
  cout << getdevice << endl;
  // cl_device_id devices[] = {device};

  cout << CL_CONTEXT_PLATFORM <<" "<< id[0] << " " << id[1] <<" "<< num_devices_cpu <<" "<< num_devices_gpu <<endl;

  cl_int status_cpu, status_gpu;
  cl_context context_cpu; 
  cl_context context_gpu;
  cl_device_id devicecpugpu[] = {device_list_cpu, device_list_gpu};
  //context = clCreateContextFromType(NULL, CL_DEVICE_TYPE_GPU, NULL, NULL, &status);
  cout <<"make context "<<devicecpugpu<<endl;
  //context_cpu = clCreateContext(properties_cpugpu, num_devices_cpu + num_devices_gpu, devicecpugpu, NULL, NULL, &status_cpu);
  context_gpu = clCreateContext(properties_gpu,num_devices_gpu, &device_list_gpu, NULL, NULL, &status_gpu);
  cout << "Context_cpu : " << status_cpu <<"  Context_gpu : "<< status_gpu << endl;
}
