//-----------------------------------------------------------
//
// Description:
//      Interface class for the Fast Hough Transfom (FHT)
//      algorithm based on CUDA.  
//      Takes framework clusters and hands them to the 
//      GPU for processing.
//      -- implementaion
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



#include "fastHoughGPU_IFC.h"
#include <TVector3.h>
#include <assert.h>
#include <stdlib.h>
#include "fastHoughGPU_IFC.cuh"



fastHoughGPU_IFC::fastHoughGPU_IFC(float SCALING , int MAXSIZE) {
  
  _initP = false;
  _initC = false;
  _MAXSIZE = MAXSIZE;
  _RIEMANNSCALING = SCALING;
  _CUTX = true;

  _threads = 128; //standard value
  
  _cutoff = 0.7f;    //default values
  _cutoffLevel = 4;
  
  _p0 = (float*) malloc(2*MAXSIZE*sizeof(float));
  _p1 = (float*) malloc(2*MAXSIZE*sizeof(float));
  _p2 = (float*) malloc(2*MAXSIZE*sizeof(float));
  _p3 = (float*) malloc(2*MAXSIZE*sizeof(float));
  _p4 = (float*) malloc(2*MAXSIZE*sizeof(float));

  _votes = (uint*) malloc(MAXSIZE*sizeof(uint));
  
    
  allocateArray((void**)&_votes_d, MAXSIZE*sizeof(uint));

   
  allocateArray((void**)&_p0_d, _MAXSIZE*2*sizeof(float));
  allocateArray((void**)&_p1_d, _MAXSIZE*2*sizeof(float));
  allocateArray((void**)&_p2_d, _MAXSIZE*2*sizeof(float));
  allocateArray((void**)&_p3_d, _MAXSIZE*2*sizeof(float));
  allocateArray((void**)&_p4_d, _MAXSIZE*2*sizeof(float));

  _hitlist = NULL;

}

fastHoughGPU_IFC::~fastHoughGPU_IFC() {

  //TODO: implement!!! need more wrappers for cudaFree()

}



void
fastHoughGPU_IFC::initClusters(std::vector<PndTpcCluster*> clist) {
  
  int size=0;
  for(int i=0; i<clist.size(); ++i) {
    TVector3 pos = (clist[i])->pos();
    if(pos.X()>0)
      size++;    
  }
  _nClusters=size;

  if(_nClusters<10){
    std::cerr<<"\nfastHoughGPU_IFC::initClusters: "
	     <<"Not enough valid clusters ("<<_nClusters<<")!"<<std::endl;
    std::cerr<<"Aborting ..."<<std::endl;
    return;
  }

  std::cout<<"\nfastHoughGPU_IFC::initClusters: Iitialized with "
	   <<_nClusters<<" Clusters"<<std::endl;
  
  //allocate host and GPU arrays 
  _clusterPos = (float*) malloc(3*size*sizeof(float));
  allocateArray((void**)&_clusterPos_d, 3*size*sizeof(float));
  //resulting parameter space positions:
  allocateArray((void**)&_clusterData_d, 5*size*sizeof(float));
  

  int count=0;
  
  //fill host position array
  for(int i=0; i<clist.size(); ++i) {
    TVector3 pos = (clist[i])->pos();
    if(pos.X()<0) {
      count++;
      continue;
    }
    _clusterPos[3*(i-count)] = (float)pos.X();
    _clusterPos[3*(i-count)+1] = (float)pos.Y();
    _clusterPos[3*(i-count)+2] = (float)pos.Z(); 
  }

  copyArrayToDevice(_clusterPos_d, _clusterPos,_nClusters*3*sizeof(float));
  //kernel invocation via wrapper:
  int blocks = _nClusters / _threads + 1;
  callRiemannKernel(_clusterPos_d, _clusterData_d, _nClusters, 
		    _RIEMANNSCALING, _threads, blocks);
  
  //result resides on the GPU 
    
  _initC=true;
}


uint*
fastHoughGPU_IFC::getVotes() {

  return _votes;
  
}

void 
fastHoughGPU_IFC::setHitList(char* hl, int activeNodes) {

  //size in bytes
  int CHUNK = _nClusters/(sizeof(char)*8)+1;
  
  if(_hitlist!=NULL){
    free(_hitlist);
    free(_hitlist_lastgen);
    freeArray(_hitlist_d);
    freeArray(_hitlist_lastgen_d);
  }  
    
  //allocate hitlist arrays
  _hitlist = (char*) malloc(32*CHUNK*activeNodes);
  _hitlist_lastgen = (char*) malloc(CHUNK*activeNodes);
  
  allocateArray((void**)&_hitlist_d, 32*CHUNK*activeNodes);
  allocateArray((void**)&_hitlist_lastgen_d, CHUNK*activeNodes);
    
  memcpy(_hitlist_lastgen, hl, CHUNK*activeNodes);
  
  for(int n=0; n<activeNodes; n++) {
    for(int s=0; s<32; s++) {
      memcpy(_hitlist+CHUNK*(n*32+s), _hitlist_lastgen+n*CHUNK, CHUNK);
    }
  }
  
  copyArrayToDevice(_hitlist_lastgen_d, _hitlist_lastgen, activeNodes*CHUNK);
  copyArrayToDevice(_hitlist_d, _hitlist, activeNodes*CHUNK*32);
	    
  std::cout<<"Copied new hitlist to device"<<std::endl;
  
}
 


void
fastHoughGPU_IFC::initParameterSpace(std::vector<float> mins,
				       std::vector<float> maxs) {

  assert(mins.size()==5);
  assert(maxs.size()==5);
  
  float _mins[5];
  float _maxs[5];

  for(int i=0; i<5; i++) {
    _mins[i] = mins[i];
    _maxs[i] = maxs[i];
  }

  //wrapper function call
  setParameterSpace(_mins, _maxs);

  
  _initP=true;
}

void
fastHoughGPU_IFC::initParameterSpace(float* mins,
				     float* maxs) {
  
  float _mins[5];
  float _maxs[5];

  for(int i=0; i<5; i++) {
    _mins[i] = mins[i];
    _maxs[i] = maxs[i];
  }

  //wrapper function call
  setParameterSpace(_mins, _maxs);
  
  _initP=true;
}
    
  
  
void
fastHoughGPU_IFC::testIntersection(std::vector<Hough5DNode*>* nodes,
				   int level, int THRESHOLD) {

  assert(nodes->size()<_MAXSIZE);
  if(_initP && _initC) {

    for(int n=0; n<nodes->size(); n++) {
      //TODO: implement ONE call in Hough5DNode, make ONE array
      float* p0 = (nodes->at(n))->getProjection0();
      float* p1 = (nodes->at(n))->getProjection1();
      float* p2 = (nodes->at(n))->getProjection2();
      float* p3 = (nodes->at(n))->getProjection3();
      float* p4 = (nodes->at(n))->getProjection4();
      
      
      _p0[2*n] = p0[0];
      _p0[2*n+1] = p0[1];
      _p1[2*n] = p1[0];
      _p1[2*n+1] = p1[1];
      _p2[2*n] = p2[0];
      _p2[2*n+1] = p2[1];
      _p3[2*n] = p3[0];
      _p3[2*n+1] = p3[1];
      _p4[2*n] = p4[0];
      _p4[2*n+1] = p4[1];
      _votes[n] = (uint)0;
    }

    
    copyArrayToDevice(_p0_d, _p0,nodes->size()*2*sizeof(float));
    copyArrayToDevice(_p1_d, _p1,nodes->size()*2*sizeof(float));
    copyArrayToDevice(_p2_d, _p2,nodes->size()*2*sizeof(float));
    copyArrayToDevice(_p3_d, _p3,nodes->size()*2*sizeof(float));
    copyArrayToDevice(_p4_d, _p4,nodes->size()*2*sizeof(float));
    
    
    //choose the kernel to call based on nClusters and nNodes
    if(0) {
    //if(_nClusters > nodes->size()) {
      int blocks = _nClusters / _threads + 1;      
      //kernel call (does a threadSync)
      callIntersectKernel(nodes->size(),level,_nClusters,_clusterData_d,
			  _hitlist_d, _hitlist_lastgen_d,
			  _p0_d,_p1_d,_p2_d,_p3_d,_p4_d,_votes_d, 
			  _threads, blocks);
      
      threadSync();
    }

    else {
      
      int blocks = nodes->size() / _threads + 1;
      //kernel call (does a threadSync)
      callIntersectKernel2(nodes->size(),level,_nClusters, _clusterData_d,
			   _hitlist_d, _hitlist_lastgen_d,
			   _p0_d,_p1_d,_p2_d,_p3_d,_p4_d,_votes_d, 
			   _threads, blocks);
      threadSync();
    }

    //throw away cutoff*100% of each mothers' sons
    int blocks = nodes->size() /_threads + 1;
     
    
    if(level>_cutoffLevel) {
      callCutoffKernel(_cutoff, nodes->size(), _votes_d, _threads, blocks);
    }

    int CHUNK = _nClusters/(sizeof(char)*8)+1;
    
    copyArrayFromDevice(_votes, _votes_d, 
			nodes->size()*sizeof(uint));
    copyArrayFromDevice(_hitlist, _hitlist_d, 
			CHUNK*nodes->size());
          
  }
}
  
  
void
fastHoughGPU_IFC::setKernelPars(uint threads) {
  _threads = threads;
}

