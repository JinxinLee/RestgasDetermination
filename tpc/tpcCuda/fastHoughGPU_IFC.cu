//-----------------------------------------------------------
//
// Description:
//      C-style wrapper functions for the CUDA calls
//      -- implementation
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


#include "fastHoughGPU_kernel.cu"
#include "fastHoughGPU_IFC.cuh"
#include <iostream>
#include <cutil_inline.h>


extern "C" {

  void allocateArray(void **devPtr, int size) {
    cutilSafeCall(cudaMalloc(devPtr, size));  
  }
  
  void freeArray(void *devPtr) {
    cutilSafeCall(cudaFree(devPtr));
  }
  
  void threadSync() {
    cutilSafeCall(cudaThreadSynchronize());
  }
  
  void copyArrayFromDevice(void* host, const void* device, 
			   int size) {
    cutilSafeCall(cudaMemcpy(host, device, size, cudaMemcpyDeviceToHost));
  }
  
  void copyArrayToDevice(void* device, const void* host, int size) {
    
    cutilSafeCall(cudaMemcpy(device, host, size, cudaMemcpyHostToDevice));
  }

  //void copyArrayToSymbol(float* host) {
  // cutilSafeCall(cudaMemcpyToSymbol(clusterDataConst_d,host, 10000*sizeof(float)));
  //}
  
  
  void setParameterSpace(float* mins, float* maxs){
    cutilSafeCall(cudaMemcpyToSymbol(globalMins_d, mins, 5*sizeof(float)));
    cutilSafeCall(cudaMemcpyToSymbol(globalMaxs_d, maxs, 5*sizeof(float)));  
  }
  
  void callRiemannKernel(float* pos_d, float* data_d, 
			 int nCl, float SCALING, int threads, int blocks) {
    
    dim3 dimGrid(blocks);
    dim3 dimBlock(threads);
    std::cout<<"Starting RiemannKernel ..."<<std::endl;
    std::cout<<"BLOCKS: "<<blocks<<"   THREADS: "<<threads<<std::endl;
    
    riemannTransform<<< dimGrid, dimBlock >>>(pos_d, data_d,
					      nCl, SCALING);
    threadSync();
    std::cout<<"Finished executing RiemannKernel" <<std::endl;
    
    
  }

  
  void callIntersectKernel(int nodes, int level, int nCl,
			   float* data_d, 
			   char* hl, char* hl_lg,
			   float* p0_d,
			   float* p1_d, float* p2_d, float* p3_d,
			   float* p4_d, uint* votes_d,
			   int threads, int blocks) {
    
    
    dim3 dimGrid(blocks);
    dim3 dimBlock(threads);
    

    dim3 dimBlock2 = 512;
    dim3 dimGrid2 = nodes / 512 + 1;
    
    cleanUpVotes<<< dimGrid2, dimBlock2 >>> (nodes, votes_d);
    
    threadSync();

    std::cout<<"Starting IntersectKernel ..."<<std::endl;
    std::cout<<"BLOCKS: "<<blocks<<"   THREADS: "<<threads<<std::endl;
    
    testIntersect<<< dimGrid, dimBlock >>> (nodes, level, nCl,
					    data_d,
					    hl, hl_lg,
					    p0_d, p1_d, p2_d,
					    p3_d, p4_d, votes_d);
    threadSync();
  }
  
  void callIntersectKernel2(int nodes, int level, int nCl,
			    float* data_d, 
			    char* hl, char* hl_lg,
			    float* p0_d,
			    float* p1_d, float* p2_d, float* p3_d,
			    float* p4_d, uint* votes_d,
			    int threads, int blocks) {
    
    
    dim3 dimGrid(blocks);
    dim3 dimBlock(threads);
    

    dim3 dimBlock2 = 512;
    dim3 dimGrid2 = nodes / 512 + 1;
    
    cleanUpVotes<<< dimGrid2, dimBlock2 >>> (nodes, votes_d);
    
    threadSync();

    std::cout<<"Starting IntersectKernel2 ..."<<std::endl;
    std::cout<<"BLOCKS: "<<blocks<<"   THREADS: "<<threads<<std::endl;
    
    testIntersect2<<< dimGrid, dimBlock >>> (nodes, level, nCl,
					     data_d, 
					     hl, hl_lg,
					     p0_d, p1_d, p2_d,
					     p3_d, p4_d, votes_d);
    threadSync();
  }


  void callCutoffKernel(float cutoff, int nodes, uint* votes, 
			int threads, int blocks) {

    dim3 dimGrid(blocks);
    dim3 dimBlock(threads);
    
    std::cout<<"Calling cutoff Kernel ..."<<std::endl;
    std::cout<<"BLOCKS: "<<blocks<<"   THREADS: "<<threads<<std::endl;
  
    cutoffKernel<<< dimGrid, dimBlock >>> (cutoff, nodes, votes);
    

  }

  /* void callJanitor(char* _hitlist_d, char* _hitlist_lastgen_d,
		   int nodes, int clusters,int threads, int blocks) {
  
  
    int CHUNK = clusters/(sizeof(char)*8)+1;
    
    dim3 dimGrid(blocks);
    dim3 dimBlock(threads);

    std::cout<<"Calling janitor ..."<<std::endl;
    std::cout<<"BLOCKS: "<<blocks<<"   THREADS: "<<threads<<std::endl;
    
    hitlistJanitor<<< dimGrid, dimBlock >>> (_hitlist_d, _hitlist_lastgen_d,
					     nodes, clusters, CHUNK);
					     }*/
					     

} //extern C
