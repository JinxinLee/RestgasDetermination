#include "fastHoughGPU_kernel.cuh"


//__constant__ float* clusterPos_d;

//TODO: make clusterPos (paramPos, outp of riemannTransform) CONSTANT

__constant__ float globalMins_d[5];
__constant__ float globalMaxs_d[5];


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



//TODO: BITWISE extraction of sign, avoid logical statements!
__global__ void testIntersect(int nNodes, int level, int nClusters,
			      float* clusterData_d, 
			      float* proj0, float* proj1, float* proj2,
			      float* proj3, float* proj4, 
			      uint* votes) {

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
      
      if(sign == 32 || sign==0)
	continue;
          
      atomicAdd(&(votes[n]),(uint)1);           		
    }
  }
}


