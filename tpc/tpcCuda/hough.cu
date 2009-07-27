#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TVector3.h"
#include "PndTpcDigi.h"
#include "TClonesArray.h"
#include <vector>
#include "dataLoader.h"
#include "device_functions.h"

using std::cout;
using std::endl;

//#include "hello.cuh"

//#include "hello_kernel.cu"


__global__ void readAmps(float3* amp_in, float3* amp_out, int nCl) {
  
  int tID = blockIdx.x * gridDim.x + threadIdx.x;
  if(tID<nCl) {
  } 
    
  
}

int main() {

  dataLoader loader("Test.reco.root");

  unsigned int nClusters=loader.nClusters();

  std::cout<< "Number of Clusters: " << nClusters << endl;
  std::cout<<"Converting To float array ... input floats:"<<std::endl;

  float3* pos_h;
  float3* pos_out_h;
  
  pos_h = (float3*) malloc(nClusters*sizeof(float));
  pos_out_h = (float3*) malloc(nClusters*sizeof(float));

  std::vector<TVector3> blub;

  for(unsigned int i=0; i<nClusters; ++i){ 
    float3 pos;
    float r, phi, z;
    double r_d, phi_d, z_d;
    loader.getClusterPosPolar(i, r_d, phi_d, z_d);
    pos.x = r;
    pos.y = phi;
    pos.z = z;
    pos_h[i] = pos;
    //cout << amp_h[i] << endl;
  }
  
  //device pointers
  float3* pos_in_d;
  float3* pos_out_d;

  cudaMalloc((void**) &pos_in_d, nClusters*sizeof(float3));
  cudaMalloc((void**) &pos_out_d, nClusters*sizeof(float3));
  
  //copy to device
   cudaMemcpy(pos_in_d, pos_h, nClusters*sizeof(float), cudaMemcpyHostToDevice);
  

  int nThreads = 512;
  int nBlocks = nClusters/nThreads +1 ;

  cout << "NBlocks=" << nBlocks << endl;
  
  //launch kernel
  dim3 dimGrid(nBlocks);
  dim3 dimBlock(nThreads);
  
  std::cout<<"Starting kernel ..."<<std::endl;
  readAmps<<< dimGrid, dimBlock >>>(pos_in_d, pos_out_d, nClusters);
  std::cout<<"Finished executing kernel" <<std::endl;
  cudaMemcpy(pos_out_h, pos_out_d, nClusters*sizeof(float3), 
  	     cudaMemcpyDeviceToHost );
 
   
}
