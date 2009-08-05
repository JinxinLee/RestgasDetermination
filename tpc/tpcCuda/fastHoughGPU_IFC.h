
#ifndef GPU_INTERFACE_H
#define GPU_INTERFACE_H

#include "PndTpcCluster.h"
#include "Hough5DNode.h"
#include "fastHoughGPU_kernel.cuh"
#include <vector>


//Interface class for GPU FHT

typedef unsigned int uint;

class fastHoughGPU_IFC {


 public:

  fastHoughGPU_IFC(float RIEMANNSCALING = 40, int MAXSIZE=1000000)
  ~fastHoughGPU_IFC();

  void initClusters(std::vector<PndTpcCluster*>);

  // takes mins and maxs in format (phi, theta, c, m, t)
  void initParameterSpace(std::vector<float> mins, std::vector<float> maxs);
  
  void testIntersect(std::vector<Hough5DNode*> nodes,
		     int level, int THRESHOLD);
  uint* getVotes() {return _votes;}

  void setKernelPars(uint threadsPerBlock, uint blocks);
  
  
 private:

  bool _initC;
  bool _initP;
  uint _nClusters;
  uint* _votes;
  uint _level;
  
  uint _threads;
  uint _blocks;
  

  //CPU data arrays
  float* _clusterPos;
    
  float* _p0;
  float* _p1;
  float* _p2;
  float* _p3;
  float* _p4;
  
  float* _mins;
  float* _maxs;

  //GPU data arrays
  float* _clusterPos_d;
  float* _clusterData_d; //MAKE CONSTANT /TEXTURE
    
  float* _p0_d;
  float* _p1_d;
  float* _p2_d;
  float* _p3_d;
  float* _p4_d;

  uint* _votes_d;
  
  //parameters are CONSTANT on GPU, defined in the KERNEL cu


  int _MAXSIZE;    //size of allocated arrays (has to be bigger than 
                   //nNodes (sons)
  
  int _RIEMANNSCALING; //scaling of the padplane to be in the scale
                       //of the Riemann Sphere

  bool _CUTX;      //only use clusters with X>0? (standard)
  
};


#endif
