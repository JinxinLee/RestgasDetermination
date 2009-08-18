//-----------------------------------------------------------
//
// Description:
//      Interface class for the Fast Hough Transfom (FHT)
//      algorithm based on CUDA.  
//      Takes framework clusters and hands them to the 
//      GPU for processing.
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



#ifndef GPU_INTERFACE_H
#define GPU_INTERFACE_H

#include "PndTpcCluster.h"
#include "Hough5DNode.h"
//#include "fastHoughGPU_kernel.cuh"
#include <vector>


//Interface class for GPU FHT

typedef unsigned int uint;

class fastHoughGPU_IFC {


 public:

  fastHoughGPU_IFC(float RIEMANNSCALING=40.f, int MAXSIZE=1000000);
  ~fastHoughGPU_IFC();

  void initClusters(std::vector<PndTpcCluster*>);

  // takes mins and maxs in format (phi, theta, c, m, t)
  void initParameterSpace(std::vector<float> mins, std::vector<float> maxs);
  void initParameterSpace(float* mins, float* maxs);

  //takes hitlist of all ACTIVE NODES as input and CREATES 
  //hitlists of the sons --- copies both to the device 
  void setHitList(char* hl, int activeNodes); 
  
  void testIntersection(std::vector<Hough5DNode*> nodes,
			int level, int THRESHOLD);
  uint* getVotes();
  //float* getCenter() {return _center;}
  char* getHitList() {return _hitlist;}

  void setKernelPars(uint threadsPerBlock);
  
  
 private:

  bool _initC;
  bool _initP;
  uint _nClusters;
  uint* _votes;
  uint _level;
  
  uint _threads;   //threads per block
  

  //CPU data arrays
  float* _clusterPos;
    
  float* _p0;
  float* _p1;
  float* _p2;
  float* _p3;
  float* _p4;
  
  float* _mins;
  float* _maxs;

  char* _hitlist;
  char* _hitlist_lastgen;
  
  
  //GPU data arrays
  float* _clusterPos_d;
  float* _clusterData_d; //will be moved to __constant__
    
  float* _p0_d;
  float* _p1_d;
  float* _p2_d;
  float* _p3_d;
  float* _p4_d;
  
  uint* _votes_d;

  char* _hitlist_d;
  char* _hitlist_lastgen_d;



  //parameters are CONSTANT on GPU, defined in the KERNEL cu
  

  int _MAXSIZE;    //size of allocated arrays (has to be bigger than 
                   //potential nNodes (sons))
  
  float _RIEMANNSCALING; //scaling of the padplane to be in the scale
                         //of the Riemann Sphere

  bool _CUTX;      //only use clusters with X>0? (standard)
  
};


#endif
