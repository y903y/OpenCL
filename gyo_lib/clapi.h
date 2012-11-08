class clapi{
  public:
    clapi();
    startcalc();
    ~clapi();
  private:
    cl_int status;
    cl_platform_id platforms[2];
    cl_uint num_platforms;
    cl_context context;
    cl_device_id device_list[4];//なぜか2じゃできない
    cl_uint num_device;
    cl_context_properties properties[];
    cl_command_queue queue;
    cl_program program;
    cl_uint pro_info;
    cl_kernel kernel;
    cl_mem memIn1;
    cl_mem memIn2;
    cl_mem memOut;

};

class hikisu{
  public:
    int fvar;
    int fwid;
    float *mtrx1;
    int lvar;
    int lwid;
    float *mtrx2;
    int size;
    float *Out;
   size_t globalsize[] = {fvar*fwid};
  private:
};
