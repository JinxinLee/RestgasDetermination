//wrap cuda calls

extern "C"
{

  void allocateArray(void **devPtr, int size);
  
  void freeArray(void *devPtr);
  
  void threadSync();

  void copyArrayFromDevice(void* host, const void* device, 
			   int size);
  
  void copyArrayToDevice(void* device, const void* host, 
			 int size);
  
  void setParameterSpace(float* mins, float* maxs);

  void callRiemannKernel(float* pos_d, float* data_d, 
			 int nCl, float SCALING);

  void callIntersectKernel(int nodes, int level, int nCL,
			   float* data_d, float* p0_d,
			   float* _p1_d, float* _p2_d, float* _p3_d,
			   float* _p4_d, uint* _votes_d);

  
} //extern C
