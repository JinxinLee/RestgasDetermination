// first try

#include <list>
#include <vector>
#include <iostream>
#include "dataLoader.h"
#include "Hough5DNode.h"
#include <device_functions.h>
#include <cuda_runtime.h>
//#include <cutil_inline.h>


// struct Node {
//  float center[5];
//  float proj0[2];
//  float proj1[2];
//  float proj2[2];
//  float proj3[2];
//  float proj4[2];
//  int level;
//  int sideLength;
//  int vote;
// };


typedef unsigned int uint;


//__constant__ float* clusterPos_d;

//TODO: make clusterPos (paramPos, outp of riemannTransform) CONSTANT/ TEXTURE

__device__ __constant__ float globalMins_d[5];
__device__ __constant__ float globalMaxs_d[5];



__global__ void riemannTransform(float* clPos, float* paramPos, int nClusters,
				 float SCALING) {
  //can we make something SHARED here?
  int tID = blockIdx.x * blockDim.x + threadIdx.x;

  if(tID<nClusters) {
    
    float x,y,z;
    
    x = clPos[tID*3];
    y = clPos[tID*3+1];
    z = clPos[tID*3+2];
    
    float r = __fsqrt_rn(x*x+y*y);
    paramPos[tID*5+3] = r;
    paramPos[tID*5+4] = z;
    
    float r_R = r / SCALING;
    float phi = atan(y/x);
    float x_R = r_R * __cosf(phi)/(1+r_R*r_R);
    float y_R = r_R * __sinf(phi)/(1+r_R*r_R);
    float z_R = r_R*r_R/(1+r_R*r_R); 
    
    paramPos[tID*5] = x_R;
    paramPos[tID*5+1] = y_R;
    paramPos[tID*5+2] = z_R;
  }
    
  __syncthreads();
}



//------------------------------------------------------------------------


__global__ void testIntersect(int nNodes, int level, int nClusters,
			      float* clusterData_d, 
			      float* proj0, float* proj1, float* proj2,
			      float* proj3, float* proj4, 
			      uint* votes, float* test) {

  float* _mins = globalMins_d;  
  float* _maxs = globalMaxs_d;
    
  float _side = 1.f/(powf(2,level));
  float PI_180 = 3.141592654f / 180.f;
  
  int tID = blockIdx.x * blockDim.x + threadIdx.x;

   if(tID<nClusters) {

    //get this hyperplanes' data:
    float x_R = clusterData_d[tID*5];
    float y_R = clusterData_d[tID*5+1];
    float z_R = clusterData_d[tID*5+2];
    float R = clusterData_d[tID*5+3];
    float Z = clusterData_d[tID*5+4];

    //get Node data
        
    for(int n=0; n<nNodes; ++n) {
            
      float m1 = proj3[n*2];
      float m2 = proj3[n*2+1];
      float mCoords[2] = {m1,m2};
      
      float t1 = proj4[n*2];
      float t2 = proj4[n*2+1];
      float tCoords[2] = {t1,t2};
      
      //test for intersection in M-T space -------------------
      int signs1 = 0;
      for(int m_it=0; m_it<2; m_it++) {
	float t_m = -R*(mCoords[m_it]*(_maxs[3]-_mins[3])) + Z;
	for(int t_it=0; t_it<2; t_it++) {
	  float diff = tCoords[t_it]*(_maxs[4]-_mins[4]) - t_m;
	  signs1+=(int)(diff > 0);
	}
      }

      //has to be tested if this is good or bad for performance
      if(signs1 == 0 || signs1 == 4)
	continue;
      
           
      //test for interesection in (phi, theta, c) ------------
      
      float phi1 = proj0[n*2]*(_maxs[0] - _mins[0]) + 90; 
      float phi2 = proj0[n*2+1]*(_maxs[0] - _mins[0]) + 90;
      float phiCoords[2] = {phi1, phi2};
      
      float theta1 = (proj1[n*2] + 0.5)* (_maxs[1] - _mins[1]) +_mins[1];
      float theta2 = (proj1[n*2+1] + 0.5)* (_maxs[1] - _mins[1]) +_mins[1];
      float thetaCoords[2] = {theta1, theta2};

      float c1 = proj2[n*2] * (_maxs[2] - _mins[2]);
      float c2 = proj2[n*2+1] * (_maxs[2] - _mins[2]) ;
      
      int sign=0;
      
      for(int p=0; p<2; p++)
	for(int t=0; t<2; t++) {
	  float n1 = __sinf(thetaCoords[t]*PI_180)*__cosf(phiCoords[p]*PI_180);
	  float n2 = __sinf(thetaCoords[t]*PI_180)*__sinf(phiCoords[p]*PI_180);
          float n3 = __cosf(thetaCoords[t]*PI_180);
      
	  float c = x_R*n1 + y_R*n2 + z_R*n3;
	  
	  sign+=(int)(c1-c > 0);
	  sign+=(int)(c2-c > 0);
	}
      
      test[tID] = sign;
      
      if(sign == 32 || sign==0)
	continue;
          
      atomicAdd(&(votes[n]),(uint)1);           		
    }
  }
}




// MAIN --------------------------------------------------------------------  
  

int main(int argc, char** argv) {

  extern char *optarg;
  int c;
  int TREE_DEPTH = 6;  //number of space divisions
  int THRESHOLD = 40;
   
  while ((c = getopt(argc, argv, "t:d:")) != -1)
    switch (c) {
    case 't':
      THRESHOLD = atoi(optarg);
      break;
    case 'd':
      TREE_DEPTH = atoi(optarg);
      break;
    }


  //READ data and CREATE histograms and data containers -----------------

  bool CUT_CHAMBER=true;   //only collect hits with x>0;
    
  uint EVENT=6;
  
  TString dir = "../../DATA/";
  TString project = "Test10";
  project=dir+project;
  TString reco_filename = project+".reco.root";
  
  dataLoader loader(reco_filename);
  loader.setEvent(EVENT);
  int nCL = loader.nClusters();



  //set global parameters ----------------------------------

  
  float RIEMANNSCALING = 40;

  float m_Max = 1.;
  float m_Min = -1.;
  float t_Max = 5.;
  float t_Min = -5.;
  float phi_Min = 0;
  float phi_Max = 180;
  float theta_Min = 65;
  float theta_Max = 95;
  float c_Min = -0.1;
  float c_Max = 0.1;

  std::cout<<"Setting parameter arrays . . ."<<std::endl;

  float _mins[5] = {phi_Min, theta_Min, c_Min, m_Min, t_Min};
  float _maxs[5] = {phi_Max, theta_Max, c_Max, m_Max, t_Max};
  
  //for(int i=0; i<5; i++) 
  //  std::cout<<_mins[i]<<"   "<<_maxs[i]<<std::endl;
     
  std::cout<<"Parameter arrays set"<<std::endl;
  

  //make sim paramaters globally available on GPU
  cudaMemcpyToSymbol(globalMins_d, _mins, 
		     sizeof(float)*5);
  cudaMemcpyToSymbol(globalMaxs_d, _maxs, 
		     sizeof(float)*5);

  


  //loop over clusters --------------------------------------------------
  int nClusters = 0;  //clusters we will use afterwards
  for(int c=0; c<nCL; ++c) {
    float x,y,z;
    loader.getClusterPosF(c,x,y,z);
    if(x<0 && CUT_CHAMBER)
      continue;
    nClusters++;
  }

  std::cout<<"\nnClusters:" <<nClusters<<std::endl;
  
  float* clusterData_d;    //OPTIMIZE 
  float* clusterPos_d;
    
  cudaMalloc((void**) &clusterData_d, nClusters*5*sizeof(float));
  cudaMalloc((void**) &clusterPos_d, nClusters*3*sizeof(float));
  float* clusterPos = (float*) malloc(nClusters*3*sizeof(float));

  int count=0;
  for(int c=0; c<nCL; ++c) {
    float x,y,z;
    loader.getClusterPosF(c,x,y,z);
    if(x<0) {
      count++;
      continue;
    }
    clusterPos[3*(c-count)] = x;
    clusterPos[3*(c-count)+1] = y;
    clusterPos[3*(c-count)+2] = z;   
      
  }
  
    
  //copy raw cluster positions to device
  cudaMemcpy(clusterPos_d, clusterPos, 
	     nClusters*3*sizeof(float), cudaMemcpyHostToDevice);
  


  //Riemann trafo on the device -----------------------------------------
  int nTHREADS = 128;

  int nBLOCKS = nClusters / nTHREADS + 1;
  dim3 dimGrid(nBLOCKS);
  dim3 dimBlock(nTHREADS);


  std::cout<<"Starting kernel ..."<<std::endl;
  std::cout<<"nBLOCKS: "<<nBLOCKS<<"   nTHREADS: "<<nTHREADS<<std::endl;
  riemannTransform<<< dimGrid, dimBlock >>>(clusterPos_d, clusterData_d,
					    nClusters, RIEMANNSCALING);
  std::cout<<"Finished executing kernel" <<std::endl;
  float* result = (float*) malloc(5*nClusters*sizeof(float));
  

  cudaMemcpy(result, clusterData_d, nClusters*5*sizeof(float), 
  	     cudaMemcpyDeviceToHost );

  
  //for(int i=0; i<nClusters; ++i) {
  //  std::cout<<result[i*5]<<"   "<<result[i*5+1]<<"   "<<result[i*5+2]
  //	     <<"   "<<result[i*5+3]<<"   "<<result[i*5+4]
  //     <<"   "<<i<<std::endl;
  //}

  //cudaFree(clusterPos_d);
  //cudaFree(clusterData_d);
  //free(clusterPos);
  //free(result); 
  
 


  // FAST HOUGH SEARCH --------------------------------------------------
  
  
  //init rood node
  std::cout<<"Init root node: "<<std::endl;
  float center[5] = {0.f,0.f,0.f,0.f,0.f};
  Hough5DNode* root = new Hough5DNode(center, 0, nClusters); 

  int MAXSIZE=1000000;
  
  //setting up parameter space
  //root->setParamSpace(mins, maxs);
  
  std::vector<Hough5DNode*> parent_list;
  std::vector<Hough5DNode*> solution_list;
  
  //evil shit --- init everything in array form

  uint* votes, *votes_d;
  float* proj0, *proj0_d;
  float* proj1, *proj1_d;
  float* proj2, *proj2_d;
  float* proj3, *proj3_d;
  float* proj4, *proj4_d;

  votes = (uint*) malloc(MAXSIZE*1*sizeof(uint));
  votes[0] = 0; //setting to zero
  proj0 = (float*) malloc(MAXSIZE*2*sizeof(float));
  proj1 = (float*) malloc(MAXSIZE*2*sizeof(float));
  proj2 = (float*) malloc(MAXSIZE*2*sizeof(float));
  proj3 = (float*) malloc(MAXSIZE*2*sizeof(float));
  proj4 = (float*) malloc(MAXSIZE*2*sizeof(float));

  float* test, *test_d;
  
  int nNodes = 1;  //number of active nodes
  
  cudaMalloc((void**) &votes_d, MAXSIZE*sizeof(uint));
  cudaMalloc((void**) &proj0_d, MAXSIZE*2*sizeof(float));
  cudaMalloc((void**) &proj1_d, MAXSIZE*2*sizeof(float));
  cudaMalloc((void**) &proj2_d, MAXSIZE*2*sizeof(float));
  cudaMalloc((void**) &proj3_d, MAXSIZE*2*sizeof(float));
  cudaMalloc((void**) &proj4_d, MAXSIZE*2*sizeof(float)); 
  
  cudaMalloc((void**) &test_d, nClusters*sizeof(float));
  
  test = (float*) malloc(nClusters*sizeof(float));
  
  for(int i=0; i<nClusters; i++)
    test[i]=0;

  cudaMemcpy(test_d, test,  
	     nClusters*sizeof(int), cudaMemcpyHostToDevice);
  
   
  proj0 = root->getProjection0();
  proj1 = root->getProjection1();
  proj2 = root->getProjection2();
  proj3 = root->getProjection3();
  proj4 = root->getProjection4();
  
  cudaMemcpy(votes_d, votes, nNodes*sizeof(uint), cudaMemcpyHostToDevice);
  cudaMemcpy(proj0_d, proj0, nNodes*2*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(proj1_d, proj1, nNodes*2*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(proj2_d, proj2, nNodes*2*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(proj3_d, proj3, nNodes*2*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(proj4_d, proj4, nNodes*2*sizeof(float), cudaMemcpyHostToDevice);
  
  
  //KERNEL CALL
  std::cout<<"Starting kernel testIntersect . . ."<<std::endl;
  testIntersect<<< dimGrid, dimBlock >>> (1, 0, nClusters,
					  clusterData_d, 	
					  proj0_d, proj1_d, proj2_d,
					  proj3_d, proj4_d, votes_d,
					  test_d);
   
  std::cout<<"Finished executing kernel" <<std::endl;
  uint* votes_result = (uint*) malloc(MAXSIZE*1*sizeof(uint));
  //float* test_result = (float*) malloc(nClusters*sizeof(float));
  
  cudaMemcpy(votes_result, votes_d, 1*sizeof(uint), 
  	     cudaMemcpyDeviceToHost );
  //cudaMemcpy(test_result, test_d, nClusters*sizeof(int), 
  //	     cudaMemcpyDeviceToHost );
  
  //cudaFree(votes_d); free(votes);
  //cudaFree(proj0_d); free(proj0);
  //cudaFree(proj1_d); free(proj1);
  //cudaFree(proj2_d); free(proj2);
  //cudaFree(proj3_d); free(proj3);
  //cudaFree(proj4_d); free(proj4);

    
  std::cout << "\nThere have been "<<votes_result[0]
	    <<" of "<<nClusters<<" hyperplane crossings in root"
	    <<std::endl;
  
  if(votes_result[0] >= THRESHOLD)
    parent_list.push_back(root);
  else
    return 0;

  std::cout<<"\nMade it through rootnode, starting FHT 32-tree search . . .\n"
	   <<std::endl;
  //free(votes_result);

  
 // made it through root, begin oct-tree search ------------------------
  
  std::vector<Hough5DNode*> _newParents;

  //Hough5DNode* blub = new Hough5DNode(NULL,0,100);
  //blub->print();
  //delete blub;
  //std::cout<<"BLUB DELETED"<<std::endl;

  
  
  for(int L=1; L<TREE_DEPTH; L++) {
    
    //float *_proj0_d,*_proj1_d,*_proj2_d,*_proj3_d,*_proj4_d;
    
    std::cout<<"LEVEL "<<L<<std::endl;
    int nNodes = parent_list.size(); //number of active nodes
    std::cout<<"    "<<nNodes<<" active Nodes"<<std::endl;
    //votes = (uint*) malloc(nNodes*sizeof(uint));
    for(int v=0; v<nNodes; v++)
      votes[v]=0;
    
    //std::cout<<"allocating arrays"<<std::endl;
    //CRASHES IN LEVEL 2
    //float* _proj0 = (float*) malloc(nNodes*32*2*sizeof(float));
    //float* _proj1 = (float*) malloc(nNodes*32*2*sizeof(float));
    //float* _proj2 = (float*) malloc(nNodes*32*2*sizeof(float));
    //float* _proj3 = (float*) malloc(nNodes*32*2*sizeof(float));
    //float* _proj4 = (float*) malloc(nNodes*32*2*sizeof(float));
    
    //std::cout<<"allocating CUDA arrays"<<std::endl;
    //cudaMalloc((void**) &votes_d, nNodes*sizeof(uint));
    //cudaMalloc((void**) &_proj0_d, nNodes*2*sizeof(float));
    //cudaMalloc((void**) &_proj1_d, nNodes*2*sizeof(float));
    //cudaMalloc((void**) &_proj2_d, nNodes*2*sizeof(float));
    //cudaMalloc((void**) &_proj3_d, nNodes*2*sizeof(float));
    //cudaMalloc((void**) &_proj4_d, nNodes*2*sizeof(float)); 
        
    std::cout<<"starting loop over parents"<<std::endl;

        //can I do this ON THE GPU??
    for(int p=0; p<nNodes; p++) {
      Hough5DNode* the_node = parent_list[p];
      the_node->print();
      float* sons = the_node->getSonArray();
      for(int s=0; s<32; ++s) {
	_newParents.push_back(new Hough5DNode(sons+5*s,L,nClusters));
	Hough5DNode* the_son=_newParents.back();
	float* p0 = the_son->getProjection0();
	float* p1 = the_son->getProjection1();
	float* p2 = the_son->getProjection2();
	float* p3 = the_son->getProjection3();
	float* p4 = the_son->getProjection4();
	proj0[p*32*2+2*s] = p0[0];
	proj0[p*32*2+2*s+1] = p0[1];
	proj1[p*32*2+2*s] = p1[0];
	proj1[p*32*2+2*s+1] = p1[1];
	proj2[p*32*2+2*s] = p2[0];
	proj2[p*32*2+2*s+1] = p2[1];
	proj3[p*32*2+2*s] = p3[0];
	proj3[p*32*2+2*s+1] = p3[1];
	proj4[p*32*2+2*s] = p4[0];
	proj4[p*32*2+2*s+1] = p4[1];
      }
      delete the_node;
    }

    std::cout<<"finished loop over parents"<<std::endl;
    
    cudaMemcpy(votes_d, votes, nNodes*32*sizeof(uint), cudaMemcpyHostToDevice);
    cudaMemcpy(proj0_d, proj0, nNodes*2*32*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(proj1_d, proj1, nNodes*2*32*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(proj2_d, proj2, nNodes*2*32*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(proj3_d, proj3, nNodes*2*32*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(proj4_d, proj4, nNodes*2*32*sizeof(float), cudaMemcpyHostToDevice);

    parent_list.clear();
    
    //KERNEL CALL
    std::cout<<"Starting kernel testIntersect . . ."<<std::endl;
    testIntersect<<< dimGrid, dimBlock >>> (nNodes*32, L, nClusters,
					    clusterData_d, 	
					    proj0_d, proj1_d, proj2_d,
					    proj3_d, proj4_d, votes_d,
					    test_d);
      
    std::cout<<"Finished executing kernel" <<std::endl;
    //uint* votes_result = (uint*) malloc(nNodes*32*sizeof(uint));
    
    cudaMemcpy(votes_result, votes_d, nNodes*32*sizeof(uint), 
	       cudaMemcpyDeviceToHost );
    
    
    //cudaFree(votes_d); free(votes);
    //cudaFree(_proj0_d); free(_proj0);
    //cudaFree(_proj1_d); free(_proj1);
    //cudaFree(_proj2_d); free(_proj2);
    //cudaFree(_proj3_d); free(_proj3);
    //cudaFree(_proj4_d); free(_proj4);
    
    //clean up
    int count=0;
    for(int s=0; s<nNodes*32; ++s) {
      if(votes_result[s]<THRESHOLD)
	delete _newParents[s];
      else {
	parent_list.push_back(_newParents[s]);
	count++;
      }
    }
    _newParents.clear();
    std::cout<<"    "<<count<<" of "<<nNodes*32<<" nodes survived"<<std::endl;
  }
    
}



  

  
  

