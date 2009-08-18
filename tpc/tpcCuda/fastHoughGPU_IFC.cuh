//-----------------------------------------------------------
//
// Description:
//      C-style wrapper functions for the CUDA calls
//      
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer      TU Munich       (original author)
//
//
//-----------------------------------------------------------

extern "C"
{

  void allocateArray(void **devPtr, int size);
  
  void freeArray(void *devPtr);
  
  void threadSync();

  void copyArrayFromDevice(void* host, const void* device, 
			   int size);
  
  void copyArrayToDevice(void* device, const void* host, 
			 int size);

  //copy Riemann-results to constant memory
  //void copyArrayToSymbol(float* host);
  
  //copy parameter settings to constant memory
  void setParameterSpace(float* mins, float* maxs);

  void callRiemannKernel(float* pos_d, float* data_d, 
			 int nCl, float SCALING, int threads, int blocks);

  //parallel on Clusters
  void callIntersectKernel(int nodes, int level, int nCL,
			   float* data_d, 
			   char* hl, char* hl_lg,
			   float* p0_d,
			   float* _p1_d, float* _p2_d, float* _p3_d,
			   float* _p4_d, uint* _votes_d, int threads,
			   int blocks);

  //version parallel on Nodes
  void callIntersectKernel2(int nodes, int level, int nCL,
			    float* data_d, 
			    char* hl, char* hl_lg,
			    float* p0_d,
			    float* _p1_d, float* _p2_d, float* _p3_d,
			    float* _p4_d, uint* _votes_d, int threads,
			    int blocks);

  /*void callJanitor(char* _hitlist_d, char* _hitlist_lastgen_d,
    int nodes, int clusters, int _threads, int blocks); */
  
} //extern C
