//-----------------------------------------------------------
//
// Description:
//      CUDA kernel code for the Fast Hough Transform (FHT)
//      algorithm. Requires CUDA hardware revision  1.1 
//      or above for atomic compability
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




#include "fastHoughGPU_kernel.cuh"
#include <cmath>


//global Hough parameter space
__device__ __constant__ float globalMins_d[5];
__device__ __constant__ float globalMaxs_d[5];



//MINITS NUMBER OF CLUSTERS TO 2000 !!! has to go later on, or 
//nvidia builds cards with bigger constant memory :)
//__device__ __constant__ float clusterDataConst_d[10000];


__device__ bool getBit(char* c, int n) {
  return (c[n>>3] & (1 << (n & 7)))!=0;
}


__device__ void clearBit(char* c, int n) {
  c[n>>3] = c[n>>3] & ~(1 << (n & 7 ));
}


__global__ void riemannTransform(float* clPos, float* paramPos, int nClusters,
				 float SCALING) {
  
  //can we make something SHARED here?
  int tID = blockIdx.x * blockDim.x + threadIdx.x;
  
  if(tID<nClusters) {
    float x,y,z;
    
    x = clPos[tID*3];
    y = clPos[tID*3+1];
    z = clPos[tID*3+2];
    
    float r = sqrt(x*x+y*y);
    paramPos[tID*5+3] = r;
    paramPos[tID*5+4] = z;
    
    float r_R = r / SCALING;
    float phi = atan(y/x);
    float x_R = r_R * cos(phi)/(1+r_R*r_R);
    float y_R = r_R * sin(phi)/(1+r_R*r_R);
    float z_R = r_R*r_R/(1+r_R*r_R); 
    
    paramPos[tID*5] = x_R;
    paramPos[tID*5+1] = y_R;
    paramPos[tID*5+2] = z_R;
  }
    
  __syncthreads();
}



//------------------------------------------------------------------------

//use votes_lastgen to determine hitlist size!
//but then: what data structure do I use?


//parallel on Cluster-level
__global__ void testIntersect(int nNodes, int level, int nClusters,
			      float* clusterData_d, 
			      char* hitlist_d, char* hitlist_lastgen_d,
			      float* proj0, float* proj1, float* proj2,
			      float* proj3, float* proj4, 
			      uint* votes) {

  float _side = 1.f/powf(2,level);
  float PI_180 = 3.141592654f / 180.f;
  
  int tID = blockIdx.x * blockDim.x + threadIdx.x;

  //size in bits for one chunk of the hitlist
  int HITLISTCHUNK = nClusters/(sizeof(char)*8)+1;
  
   if(tID<nClusters) {

     //get this hyperplanes' data:
     float x_R = clusterData_d[tID*5];
     float y_R = clusterData_d[tID*5+1];
     float z_R = clusterData_d[tID*5+2];
     float R = clusterData_d[tID*5+3];
     float Z = clusterData_d[tID*5+4];
     
     //get Node data
     
     for(int n=0; n<nNodes; ++n) {
        
       //check in mothers' hitlist
       if(level>1)  {
	 if(!(getBit(hitlist_lastgen_d, n/32*HITLISTCHUNK + tID)))
	   continue;
       }
	       
       float mCoords[2];
       mCoords[0] = proj3[n*2];
       mCoords[1] = proj3[n*2+1];
       
       float tCoords[2];
       tCoords[0] = proj4[n*2];
       tCoords[1] = proj4[n*2+1];
       
       
       //test for intersection in M-T space -------------------
       
       int signs1 = 0;
       float t_m, diff;
       for(uint m_it=0; m_it<2; m_it++) {
	 t_m = -R*(mCoords[m_it]*(globalMaxs_d[3]-globalMins_d[3])) + Z;
	 for(uint t_it=0; t_it<2; t_it++) {
	   diff = tCoords[t_it]*(globalMaxs_d[4]-globalMins_d[4]) - t_m;
	   signs1+=signbit(diff);
	 }
       }
       
             
      //test for interesection in (phi, theta, c) ------------
      
      float phi1 = (proj0[n*2]+0.5f)*(globalMaxs_d[0]-globalMins_d[0])+globalMins_d[0];; 
      float phi2 = (proj0[n*2+1]+0.5f)*(globalMaxs_d[0]-globalMins_d[0])+globalMins_d[0];;
     
      float phiCoords[2] = {phi1, phi2};
      
      float theta1 = (proj1[n*2]+0.5f)*(globalMaxs_d[1]-globalMins_d[1])+globalMins_d[1];
      float theta2 = (proj1[n*2+1]+0.5f)*(globalMaxs_d[1]-globalMins_d[1])+globalMins_d[1];
      
      float thetaCoords[2] = {theta1, theta2};
      
      float c1 = proj2[2*n] * (globalMaxs_d[2] - globalMins_d[2]);
      float c2 = proj2[2*n+1] * (globalMaxs_d[2] - globalMins_d[2]);

      
      int sign=0;
      float n1, n2, n3, p_temp, t_temp;     
      for(int p=0; p<2; ++p){
	p_temp = phiCoords[p] * PI_180;
	for(int t=0; t<2; ++t) {
	  t_temp = thetaCoords[t] * PI_180;
	  n1 = __fmul_rn(__sinf(t_temp),__cosf(p_temp));
	  n2 = __fmul_rn(__sinf(t_temp),__sinf(p_temp));
          n3 = __cosf(t_temp);
      
	  float c = x_R*n1 + y_R*n2 + z_R*n3;
	  
	  diff=c1-c;  
 	  sign+=signbit(diff); 
 	  diff=c2-c; 
 	  sign+=signbit(diff);
	  
	  	  
	}
      }
      
      if(!(sign == 8 || sign == 0 || signs1 == 4 || signs1 == 0)) {
	atomicInc(votes+n,(uint)nClusters+1);           		
      }
      else
	clearBit(hitlist_d, n*HITLISTCHUNK + tID);	
	
     }
   }
}



//intersection test parallelized in Nodes
__global__ void testIntersect2(int nNodes, int level, int nClusters,
			       float* clusterData_d,
			       char* hitlist_d, char* hitlist_lastgen_d,
			       float* proj0, float* proj1, float* proj2,
			       float* proj3, float* proj4,
			       uint* votes) {
  
  float _side = 1.f/powf(2,level);
  float PI_180 = 3.141592654f / 180.f;
  
  int tID = blockIdx.x * blockDim.x + threadIdx.x;

  //size in bits for one chunk of the hitlist
  int HITLISTCHUNK = nClusters/(sizeof(char)*8)+1;
  
   if(tID<nNodes) {

     float mCoords[2];
     mCoords[0] = proj3[tID*2];
     mCoords[1] = proj3[tID*2+1];
     
     float tCoords[2];
     tCoords[0] = proj4[tID*2];
     tCoords[1] = proj4[tID*2+1];
     
     float phi1 = (proj0[tID*2]+0.5f)*(globalMaxs_d[0]-globalMins_d[0])+globalMins_d[0];;
     float phi2 = (proj0[tID*2+1]+0.5f)*(globalMaxs_d[0]-globalMins_d[0])+globalMins_d[0];;
     
     float phiCoords[2] = {phi1, phi2};
      
     float theta1 = (proj1[tID*2]+0.5f)*(globalMaxs_d[1]-globalMins_d[1])+globalMins_d[1];
     float theta2 = (proj1[tID*2+1]+0.5f)*(globalMaxs_d[1]-globalMins_d[1])+globalMins_d[1];
      
     float thetaCoords[2] = {theta1, theta2};
     
     float c1 = proj2[2*tID] * (globalMaxs_d[2] - globalMins_d[2]);
     float c2 = proj2[2*tID+1] * (globalMaxs_d[2] - globalMins_d[2]);

     
     //get Node data
     
     for(int n=0; n<nClusters; ++n) {
       
       if(level>1)  {
	 if(!(getBit(hitlist_lastgen_d, tID/32*HITLISTCHUNK + n)))
	   continue;
       }
       
       

       //get this hyperplanes' data:
       float x_R = clusterData_d[n*5];
       float y_R = clusterData_d[n*5+1];
       float z_R = clusterData_d[n*5+2];
       float R = clusterData_d[n*5+3];
       float Z = clusterData_d[n*5+4];
       
       
       //test for intersection in M-T space -------------------
       
       int signs1 = 0;
       float t_m, diff;
       for(uint m_it=0; m_it<2; m_it++) {
	 t_m = -R*(mCoords[m_it]*(globalMaxs_d[3]-globalMins_d[3])) + Z;
	 for(uint t_it=0; t_it<2; t_it++) {
	   diff = tCoords[t_it]*(globalMaxs_d[4]-globalMins_d[4]) - t_m;
	   //signs1+=(uint)(diff > 0);
	   signs1+=signbit(diff);
	 }
       }
       
       
       //test for interesection in (phi, theta, c) ------------
       
       int sign=0;
       float n1, n2, n3, p_temp, t_temp;
       for(int p=0; p<2; ++p){
	 p_temp = phiCoords[p] * PI_180;
	 for(int t=0; t<2; ++t) {
	   t_temp = thetaCoords[t] * PI_180;
	   n1 = __fmul_rn(__sinf(t_temp),__cosf(p_temp));
	   n2 = __fmul_rn(__sinf(t_temp),__sinf(p_temp));
	   n3 = __cosf(t_temp);
      
	   float c = x_R*n1 + y_R*n2 + z_R*n3;
	   
	   diff=c1-c;
	   sign+=signbit(diff);
	   diff=c2-c;
	   sign+=signbit(diff);
	   
	 }
       }
       
       if(!(sign == 8 || sign == 0 || signs1 == 4 || signs1 == 0)) {
	 atomicInc(votes+tID,(uint)nClusters+1);
       }
       else
	 clearBit(hitlist_d, tID*HITLISTCHUNK + n);	
     }
   }
}





__global__ void cleanUpVotes(int nNodes, uint* votes) {

  int tID = blockIdx.x * blockDim.x + threadIdx.x;

  if(tID<nNodes)
    votes[tID] = (uint)0;
}


/*
//set up new hitlist
 __global__ void hitlistJanitor(char* hitlist_d, char* hitlist_lastgen_d,
				int nodes, int nClusters, int CHUNK) {

  int tID = blockIdx.x * blockDim.x + threadIdx.x;
  
  if(tID<nodes) {
    cudaMemcpy(hitlist_d+CHUNK*tID, hitlist_lastgen_d+CHUNK*tID/32,CHUNK,
	       cudaMemcpyDeviceToDevice);
  }
  } */
    










