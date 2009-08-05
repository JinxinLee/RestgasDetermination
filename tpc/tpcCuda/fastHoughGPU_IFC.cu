

#include "fastHoughGPU_kernel.cu"
#include <cuda_runtime.h>


void allocateArray(void **devPtr, int size) {
  cudaMalloc(devPtr, size);  
}

void freeArray(void *devPtr)
{
  cudaFree(devPtr);
}

void threadSync()
{
  cudaThreadSynchronize();
}

void copyArrayFromDevice(void* host, const void* device, 
			 int size) {
  cudaMemcpy(host, device, size, cudaMemcpyDeviceToHost);
}

void copyArrayToDevice(void* device, const void* host, int size) {

  cudaMemcpy(device, host, size, cudaMemcpyHostToDevice));
}

//copy to constant memory
void setParameterSpace(float* mins, float* maxs){
  cudaMemcpyToSymbol(globalMins_d, mins, 5*sizeof(float));
  cudaMemcpyToSymbol(globalMaxs_d, maxs, 5*sizeof(float));  
}
