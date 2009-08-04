// first try

#include <list>
#include <vector>
#include <iostream>
#include "dataLoader.h"
#include "Hough5DNode.h"
#include <device_functions.h>
#include <cuda_runtime.h>



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


__constant__ float* clusterPos_d;
//TODO: make paramPos constant as well!
__constant__ float* globalMins_d;
__constant__ float* globalMaxs_d;



__global__ void riemannTransform(float* clPos, float* paramPos, int nClusters,
				 float SCALING) {
  //can we make something SHARED here?
  int tID = blockIdx.x * gridDim.x + threadIdx.x;

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
			      uint* votes) {

  //float* _mins = simParams_d.mins;
  //float* _maxs = simParams_d.maxs;

  float* _mins = globalMins_d;
  float* _maxs = globalMaxs_d;
  
  
  float _side = 1.f/(powf(2,level));
  float PI_180 = 3.14159/180;
  
  int tID = blockIdx.x * gridDim.x + threadIdx.x;
  if(tID < nClusters) {

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
	  signs1+=(diff > 0);
	}
      }
      if(signs1 == 4 || signs1 == 0) 
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
      
      //atomicAdd(&(votes[n]),(uint)1);
      		
    }
  }
}






// MAIN --------------------------------------------------------------------  
  

int main(int argc, char** argv) {

  extern char *optarg;
  int c;
  
  int TREE_DEPTH = 6;  //number of space divisions
  int THRESHOLD = 40;
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
  
  for(int i=0; i<5; i++) 
    std::cout<<_mins[i]<<"   "<<_maxs[i]<<std::endl;
     
  std::cout<<"Parameter arrays set"<<std::endl;

   //make sim paramaters globally available on GPU
  cudaMalloc((void**) &globalMins_d, sizeof(float)*5);
  cudaMalloc((void**) &globalMaxs_d, sizeof(float)*5);
  cudaMemcpy(globalMins_d, _mins, 
  	     sizeof(float)*5, cudaMemcpyHostToDevice);
  cudaMemcpy(globalMaxs_d, _maxs, 
  	     sizeof(float)*5, cudaMemcpyHostToDevice);
  
  std::cout<<"blub"<<std::endl;

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
  

  //cudaMemcpy(result, clusterData_d, nClusters*5*sizeof(float), 
  //	     cudaMemcpyDeviceToHost );

  
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
  
  //setting up parameter space
  //root->setParamSpace(mins, maxs);
  
  std::list<Hough5DNode*> parent_list;
  std::vector<Hough5DNode*> solution_list;
  
  //evil shit --- init everything in array form

  uint* votes, *votes_d;
  float* proj0, *proj0_d;
  float* proj1, *proj1_d;
  float* proj2, *proj2_d;
  float* proj3, *proj3_d;
  float* proj4, *proj4_d;
  
  int nNodes = 1;  //number of active nodes
  
  cudaMalloc((void**) &votes_d, nNodes*sizeof(uint));
  cudaMalloc((void**) &proj0_d, nNodes*2*sizeof(float));
  cudaMalloc((void**) &proj1_d, nNodes*2*sizeof(float));
  cudaMalloc((void**) &proj2_d, nNodes*2*sizeof(float));
  cudaMalloc((void**) &proj3_d, nNodes*2*sizeof(float));
  cudaMalloc((void**) &proj4_d, nNodes*2*sizeof(float)); 

  votes = (uint*) malloc(nNodes*sizeof(uint));
  votes[0] = 0;

   
  //level = 0;
  //sideLength = 1.f/(pow(2,level));
  proj0 = root->getProjection0();
  proj1 = root->getProjection1();
  proj2 = root->getProjection2();
  proj3 = root->getProjection3();
  proj4 = root->getProjection4();
  
  cudaMemcpy(votes_d, votes,  
	     nNodes*sizeof(uint), cudaMemcpyHostToDevice);
  cudaMemcpy(proj0_d, proj0,  
	     nNodes*2*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(proj1_d, proj1,  
	     nNodes*2*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(proj2_d, proj2,  
	     nNodes*2*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(proj3_d, proj3,  
	     nNodes*2*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(proj4_d, proj4,  
	     nNodes*2*sizeof(float), cudaMemcpyHostToDevice);
  
  
  std::cout<<"Starting kernel testIntersect . . ."<<std::endl;
  testIntersect<<< dimGrid, dimBlock >>> (1, 0, nClusters,
					  clusterData_d, 	
					  proj0_d, proj1_d, proj2_d,
					  proj3_d, proj4_d, votes_d);
   
  std::cout<<"Finished executing kernel" <<std::endl;
  uint* votes_result = (uint*) malloc(1*sizeof(uint));
  
  cudaMemcpy(votes_result, votes_d, 1*sizeof(uint), 
  	     cudaMemcpyDeviceToHost );

  
  std::cout << "\nThere have been "<<votes_result[0]
	    <<" of "<<nClusters<<" hyperplane crossings in root"
	    <<std::endl;
  
  // if(root->getVote() < THRESHOLD)
  //  return 0;
  
    
  // // made it through root, begin oct-tree search ------------------------
  
//   parent_list.push_back(root);
//   std::cout<<"Starting Oct-Tree search ..."<<std::endl;

//   //int counter = 0;
//   while(parent_list.size()>0) {
//     Hough5DNode* the_node = (*parent_list.begin());
//     //std::cout<<"Starting with mother:"<<std::endl;
//     //the_node->print();
//     if(the_node->getLevel() >= TREE_DEPTH) {
//       solution_list.push_back(the_node);
//       parent_list.pop_front();
//       continue;
//     }
//     float* sons = the_node->getSonArray();
//     bool* hitList = the_node->getHitList();
//     for(int s=0; s<32; s++) {
      
//       parent_list.push_back(new Hough5DNode(sons+5*s, 
// 					    the_node->getLevel()+1,
// 					    points));
      
//       Hough5DNode* the_son = parent_list.back();
//       //the_son->setParamSpace(the_node->getParamMins(),
//       //		     the_node->getParamMaxs());
//       //the_son->print();

//       //now loop over points for this son and do hit check
//       for(int i=0; i<points; i++) {
// 	//we don't need to check if mother wasn't hit
// 	if(!hitList[i])
// 	  continue;
// 	Hyperplane5D plane =  hyperplanes[i];
// 	plane.testIntersect(*the_son);
//       }
//       if(the_son->getLevel() < 4) {
// 	if(the_son->getVote() < THRESHOLD) {
// 	  //std::cout<<"Deleting SON from list"<<std::endl;
// 	  delete parent_list.back();
// 	  parent_list.pop_back();
// 	}
//       }
//       else{
// 	if(the_son->getVote() < the_node->getVote()*0.99) {
// 	  //std::cout<<"Deleting SON from list"<<std::endl;
// 	  delete parent_list.back();
// 	  parent_list.pop_back();
// 	}

//       }
//     } //end loop over sons
//       //std::cout<<" . . . erasing mother . . . ";
//     //(*parent_list.begin())->print();
//     delete parent_list.front();
//     parent_list.pop_front();
//     //std::cout<<"parent_list now: "<<std::endl;
//     //std::list<Hough5DNode*>::iterator it;
//     //for(it = parent_list.begin(); it!=parent_list.end(); it++) 
//     // (*it)->print();
//     //std::cout<<" ------------------------------------------ \n"
//     //     <<std::endl;
    
//   }
    
//   std::cout<<"There have been "<<solution_list.size()
// 	   <<" solutions: \n"<<std::endl;


//   TFile* file = new TFile("plots.root");
//   TH2D* phic = (TH2D*)file->Get("phic_80");
  


//   std::vector<TBox*> boxlist;
    
//   for(int s=0; s<solution_list.size(); s++) {
//     //(solution_list[s])->print();
//     float* center = (solution_list[s])->getCenter();
//     float length = (solution_list[s])->getSideLength();
//     float x1 = (center[3] - 0.5*length)*(m_Max-m_Min);
//     float x2 = (center[3] + 0.5*length)*(m_Max-m_Min);
//     float y1 = (center[4] - 0.5*length)*(t_Max-t_Min);
//     float y2 = (center[4] + 0.5*length)*(t_Max-t_Min);
//     boxlist.push_back(new TBox(x1,y1,x2,y2));
//   }
  

//   std::cout<<"Side Length of root: "<<root->getSideLength()
// 	   <<std::endl;

//   gStyle->SetPalette(1);
  
//   TCanvas* canv = new TCanvas();
//   //canv->SetGrayscale();
//   houghRZ->Draw();


//   for(int b=0; b<boxlist.size(); ++b) {
//     (boxlist[b])->SetLineColor(kPink+10);
//     (boxlist[b])->SetFillStyle(0);
//     (boxlist[b])->Draw("l");
//   }

//   TCanvas* canv2 = new TCanvas();
//   phic->Draw("COLZ");

//   std::vector<TBox*> boxlist2;
    
//   for(int s=0; s<solution_list.size(); s++) {
//     //(solution_list[s])->print();
//     float* center = (solution_list[s])->getCenter();
//     float length = (solution_list[s])->getSideLength();
//     float x1 = (center[0] - 0.5*length)*(phi_Max-phi_Min) +90;
//     float x2 = (center[0] + 0.5*length)*(phi_Max-phi_Min) +90;
//     float y1 = (center[2] - 0.5*length)*(c_Max-c_Min);
//     float y2 = (center[2] + 0.5*length)*(c_Max-c_Min);
//     boxlist2.push_back(new TBox(x1,y1,x2,y2));
//     //boxlist.back()->Print();
//   }

//   for(int b=0; b<boxlist2.size(); ++b) {
//     (boxlist2[b])->SetLineColor(kPink+10);
//     (boxlist2[b])->SetFillStyle(0);
//     (boxlist2[b])->Draw("l");
//   }
  

//   TH2D* sebastian_stinkt = new TH2D("seb", "Sebastian riecht streng",
// 				    100,phi_Min,phi_Max, 100, m_Min, m_Max);
//   TCanvas* canv3 = new TCanvas();


//   std::vector<TBox*> boxlist3;
//   //sparse->Projection(0,3)->Draw("COLZ");
//   sebastian_stinkt->Draw();
//   for(int s=0; s<solution_list.size(); s++) {
//     float* center = (solution_list[s])->getCenter();
//     float length = (solution_list[s])->getSideLength();
//     float x1 = (center[0] - 0.5*length)*(phi_Max-phi_Min) +90;
//     float x2 = (center[0] + 0.5*length)*(phi_Max-phi_Min) +90;
//     float y1 = (center[3] - 0.5*length)*(m_Max-m_Min);
//     float y2 = (center[3] + 0.5*length)*(m_Max-m_Min);
//     boxlist3.push_back(new TBox(x1,y1,x2,y2));
//     //boxlist.back()->Print();
//   }

//   for(int b=0; b<boxlist3.size(); ++b) {
//     (boxlist3[b])->SetLineColor(kPink+10);
//     (boxlist3[b])->SetFillStyle(0);
//     (boxlist3[b])->Draw("l");
//   }
      
//   gApplication->SetReturnFromRun(true);
//   gSystem->Run();
  
  
}
