
#include "fastHoughGPU_kernel.cu"
#include "fastHoughGPU_IFC.cuh"
#include <iostream>
//#include <cudart.h>

extern "C" {

  void allocateArray(void **devPtr, int size) {
    cudaMalloc(devPtr, size);  
  }
  
  void freeArray(void *devPtr) {
    cudaFree(devPtr);
  }
  
  void threadSync() {
    cudaThreadSynchronize();
  }
  
  void copyArrayFromDevice(void* host, const void* device, 
			   int size) {
    cudaMemcpy(host, device, size, cudaMemcpyDeviceToHost);
  }
  
  void copyArrayToDevice(void* device, const void* host, int size) {
    
    cudaMemcpy(device, host, size, cudaMemcpyHostToDevice);
  }
  
  //copy to constant memory
  void setParameterSpace(float* mins, float* maxs){
    cudaMemcpyToSymbol(globalMins_d, mins, 5*sizeof(float));
    cudaMemcpyToSymbol(globalMaxs_d, maxs, 5*sizeof(float));  
  }
  
  void callRiemannKernel(float* pos_d, float* data_d, 
			 int nCl, float SCALING, int threads, int blocks) {
    
    dim3 dimGrid(blocks);
    dim3 dimBlock(threads);
    std::cout<<"Starting RiemannKernel ..."<<std::endl;
    std::cout<<"BLOCKS: "<<blocks<<"   THREADS: "<<threads<<std::endl;
    
    riemannTransform<<< dimGrid, dimBlock >>>(pos_d, data_d,
					      nCl, SCALING);
    std::cout<<"Finished executing RiemannKernel" <<std::endl;
    
  }

  
  void callIntersectKernel(int nodes, int level, int nCl,
			   float* data_d, float* p0_d,
			   float* p1_d, float* p2_d, float* p3_d,
			   float* p4_d, uint* votes_d,
			   int threads, int blocks) {
    
    dim3 dimGrid(blocks);
    dim3 dimBlock(threads);
    std::cout<<"Starting IntersectKernel ..."<<std::endl;
    std::cout<<"BLOCKS: "<<blocks<<"   THREADS: "<<threads<<std::endl;
    
    testIntersect<<< dimGrid, dimBlock >>> (nodes, level, nCl,
					  data_d, p0_d, p1_d, p2_d,
					  p3_d, p4_d, votes_d);
   
  }
  
  

}
