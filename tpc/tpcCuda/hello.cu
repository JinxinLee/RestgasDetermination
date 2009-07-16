#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH1F.h"
#include "PndTpcDigi.h"
#include "TClonesArray.h"
#include "TCanvas.h"
#include "dataLoader.h"
#include "device_functions.h"

using std::cout;
using std::endl;

//#include "hello.cuh"

//#include "hello_kernel.cu"


__global__ void readAmps(float* amp_in, float* amp_out) {
  
  int tID = blockIdx.x * gridDim.x + threadIdx.x;
  amp_out[tID] = sqrt(amp_in[tID]);
}



int main() {

  
  dataLoader loader("2Gev_G3_ALICE_L5_1MeV_cuts_with_PIPE_MVD_10k_evts.raw.root");

  unsigned int nDigis=loader.nDigis();

  std::cout<< "Number of Digis: " << nDigis << endl;
  std::cout<<"Converting To float array ... input floats:"<<std::endl;

  float* amp_h;
  float* amp_out_h;
  
  amp_h = (float*) malloc(nDigis*sizeof(float));
  amp_out_h = (float*) malloc(nDigis*sizeof(float));

  for(unsigned int i=0; i<nDigis; ++i){ 
    amp_h[i] = (float)(loader.getDigiAmp(i));
    //cout << amp_h[i] << endl;
  }
  
  //device pointers
  float* amp_in_d;
  float* amp_out_d;

  cudaMalloc((void**) &amp_in_d, nDigis*sizeof(float));
  cudaMalloc((void**) &amp_out_d, nDigis*sizeof(float));
  
  //copy to device
   cudaMemcpy(amp_in_d, amp_h, nDigis*sizeof(float), cudaMemcpyHostToDevice);
  

  int nThreads = 256;
  int nBlocks = nDigis/nThreads +1 ;

  cout << "NBlocks=" << nBlocks << endl;
  
  //launch kernel
  dim3 dimGrid(nBlocks);
  dim3 dimBlock(nThreads);
  
  std::cout<<"Starting kernel ..."<<std::endl;
  readAmps<<< dimGrid, dimBlock >>>(amp_in_d, amp_out_d);
  std::cout<<"Finished executing kernel" <<std::endl;
  cudaMemcpy(amp_out_h, amp_out_d, nDigis*sizeof(float), 
  	     cudaMemcpyDeviceToHost );
 
 for(int i=0; i<25; i++) {
   double in = loader.getDigiAmp(i);
   std::cout<<"In: "<<in<<"    Out: "<<amp_out_h[i]<<std::endl;
  }
   
}
