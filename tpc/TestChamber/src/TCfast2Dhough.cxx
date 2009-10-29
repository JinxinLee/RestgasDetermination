#include <vector>
#include <list>
#include <iostream>

#include <TRandom.h>
#include <TH2D.h>
#include <TBox.h>
#include <TCevent.h>
#include <TCcluster.h>
#include <TVector.h>
#include <TStyle.h>
#include <TApplication.h>
#include <TSystem.h>
#include <TROOT.h>

#include "TCabsHough.h"
#include "TCfast2Dhough.h"
#include "Hyperplane2D.h"
#include "Hough2DNode.h"
bool compareNodes (Hough2DNode* n1, Hough2DNode* n2) { 
  return (n1->getVote() > n2->getVote()); 
}
using std::cout;
using std::endl;
TCfast2DHough::TCfast2DHough(const TVector3 firstAxis, const TVector3 secondAxis)
  :TCabsHough(firstAxis,secondAxis){
  dynamicParSpace=false;
  canv=NULL;
  houghSpace=NULL;
  clear();
  m_Max =0.2f;
  m_Min =-0.2f;
  t_Max = 1.5f;
  t_Min =-1.5f;
    TREE_DEPTH = 5;  //number of space divisions
  THRESHOLD = 4;
 
}
TCfast2DHough::TCfast2DHough(const TVector3 firstAxis, const TVector3 secondAxis,bool dynamicParSpace_)
  :TCabsHough(firstAxis,secondAxis){
   debug=false;
  canv=NULL;
  houghSpace=NULL;
  clear();
  m_Max =0.2;
  m_Min =-0.2f;
  t_Max = 1.5f;
  t_Min =-1.5f;
  dynamicParSpace=dynamicParSpace_;
 
  TREE_DEPTH = 5;  //number of space divisions
  THRESHOLD = 4;

}
void TCfast2DHough::make(std::vector<TCcluster>& _c){

  nClusters=0;
  convert(_c);
  float mins[2] = {m_Min, t_Min};
  float maxs[2] = {m_Max, t_Max};
  for(unsigned int i=0;i<ypHit.size();i++){
    float y=ypHit.at(i);
    float z=zpHit.at(i);
    hyperplanes.push_back(new Hyperplane2D(y,z,i));
    (hyperplanes.back())->setParamSpace(mins, maxs);
    
    nClusters++;
  } 
  int BIN_m=500;
  int BIN_t=500;
    char buf[50];
    char bufName[50];
    if(yp.x()==-1&&zp.z()==1){
      sprintf(buf,"houghspaceXZ");
      sprintf(bufName,"Hough Space XZ");
    }else if(yp.y()==1&&zp.z()==1){
      sprintf(buf,"houghspaceYZ");
      sprintf(bufName,"Hough Space YZ");
    }else{
      static TRandom r(0);
      sprintf(buf,"houghspace%f",r.Uniform());
      sprintf(bufName,"Hough Space%f",r.Uniform());
    }
  if(houghSpace==NULL){
    if(debug){
      cout<<"houghSpace created"<<endl;
    }
    houghSpace = new TH2D(buf, bufName, 
			  BIN_m, m_Min, m_Max, BIN_t, t_Min, t_Max);
  }
  

  float tBinWidth = (t_Max - t_Min)/BIN_t;
  
  for(unsigned int m=0; m<ypHit.size(); ++m) {
    char buf[50];
    char bufName[50];
    float y = ypHit.at(m);
    float z = zpHit.at(m);
    sprintf(buf,"%f-%f*x",y,z);
    sprintf(bufName,"copy%f-%f*x",y,z);
    //    cout<<"m_Min"<<m_Min<<endl;
    houghLines.push_back(new TF1(bufName,buf,m_Min,m_Max));
    //	  houghLinesYZ[i]->SetLineColor(kBlue);
    houghLines.back()->SetLineStyle(1);
    houghLines.back()->SetLineWidth(1);
    /*
    for(int t=0; t<BIN_t; ++t) {
      float T = (t+0.5)*tBinWidth + t_Min;
      float M = (T-y) / (z*(-1.));
      houghSpace->Fill(M,T);
      if(t%250==0&&debug){
	cout<<"t "<<t<<" tBinWidth "<<tBinWidth<<endl
	    <<" t_Min "<<t_Min<<" T "<<T<<" y "<<y<<" z "<<z<<endl
	    <<" t_Max "<<t_Max<<" t_Min "<<t_Min<<" BIN_t "<<BIN_t<<endl;
	cout<<"clIn "<<m<<" m "<<M<<" t "<<T<<endl;
      }
    }
    */
  }
  doHough();
}
void TCfast2DHough::doHough(){
  if(debug){
    std::cout<<"Init root node: "<<std::endl;
  }
  float center[2] = {0.f,0.f};
  Hough2DNode* root = new Hough2DNode(center, 0, nClusters); 
  std::vector<Hough2DNode*> parent_list;
  float* corners = root->getCorners();
  if(debug){
    for(int i=0; i<8; i++) {
      if(i%2==0)
	std::cout<<std::endl;
      if(i%2==0){
	std::cout<<((corners[i]+0.5)*(m_Max-m_Min)+m_Min)<<"   ";
      }else{
	std::cout<<((corners[i]+0.5)*(t_Max-t_Min)+t_Min)<<"   ";
      }
    }
  }
  for(int i=0; i<nClusters; i++) {
    Hyperplane2D* plane = hyperplanes.at(i);
    plane->testIntersect(*root);
  }
  if(debug){
    std::cout << "\nThere have been "<<root->getVote()
	      <<" of "<<nClusters<<" hyperplane crossings in root"
	      <<std::endl;
    if(root->getVote() < THRESHOLD){
      cout<<"top node not enough votes"<<endl;
    }
  }
  parent_list.push_back(root);
   if(debug){
    std::cout<<"Starting Oct-Tree search ..."<<std::endl;
  }
/*
  old main loop, somehow flawed, memory leak
  while(parent_list.size()>0) {
    Hough2DNode* the_node = (*parent_list.begin());
    if(the_node->getLevel() >= TREE_DEPTH) {
      node_list.push_back(the_node);
      parent_list.pop_front();
      continue;
    }
    float* sons = the_node->getSonArray();
    bool* hitList = the_node->getHitList();
    for(int s=0; s<4; s++) {
      parent_list.push_back(new Hough2DNode(sons+2*s, 
					    the_node->getLevel()+1,
					    nClusters));
      Hough2DNode* the_son = parent_list.back();
      //now loop over points for this son and do hit check
      for(int i=0; i<nClusters; i++) {
	if(debug){
	  cout<<"bool "<<i<<" mainloop: "<<hitList[i]<<endl;
	}
	if(!hitList[i]){
	  continue;//we don't need to check if mother wasn't hit
	}
	Hyperplane2D* plane =  hyperplanes[i];
	plane->testIntersect(*the_son);
      }
      if(the_son->getLevel() < 6) {
	if(the_son->getVote() < THRESHOLD) {
	  delete parent_list.back();
	  parent_list.pop_back();
	}
      }
      else{
	if(the_son->getVote() < the_node->getVote()*0.92) {
	  delete parent_list.back();
	  parent_list.pop_back();
	}
      }
    } //end loop over sons
    delete parent_list.front();
    parent_list.pop_front();    
  }
  if(debug){
    std::cout<<"There have been "<<node_list.size()
	     <<" solutions: \n"<<std::endl;
  }
*/  





   std::vector<Hough2DNode*> temp_sons;
   for(int l=0; l<TREE_DEPTH; ++l) {
     std::vector<Hough2DNode*> new_nodes;
     if(debug){
       std::cout<<"level: "<<l<<std::endl;
       std::cout<<"parent_list size: "<<parent_list.size()<<std::endl;
     }
     int counter=0;
     //create new nodes
     for(unsigned int n=0; n<parent_list.size(); ++n) {
    
       Hough2DNode* the_node=parent_list.at(n);
       float* sons = the_node->getSonArray();
      
       //hitlist of the mother
       bool* hitList = the_node->getHitList();
      
       //create sons
       for(int s=0; s<4; ++s) {
	 temp_sons.push_back(new Hough2DNode(sons+2*s, 
					     the_node->getLevel()+1,
					     nClusters));

	 Hough2DNode* the_son = temp_sons.at(temp_sons.size()-1);
	 //now loop over points for this son and do hit check
	 for(int i=0; i<nClusters; i++) {
	   //we don't need to check if mother wasn't hit
	   if(!hitList[i]){
	     continue;
	   }
	   //std::cout<<"intersection test for son "<<s<<std::endl;
	   Hyperplane2D* plane =  hyperplanes.at(i);
	   plane->testIntersect(the_son);
	 }	
	
	 if(the_son->getLevel() < 6) {
	   if(the_son->getVote() < THRESHOLD) {
	     //TODO: optimize
	     delete temp_sons.at(temp_sons.size()-1);
	     temp_sons.at(temp_sons.size()-1) = NULL;
	   }
	 }
	 //dynamic thresholding
	 else{
	   if(the_son->getVote() < the_node->getVote()*0.95) {
	     delete temp_sons.at(temp_sons.size()-1);
	     temp_sons.at(temp_sons.size()-1) = NULL;
	   }
	 }
	 
       } //end loop over sons
       free(sons);//should find alternative measure

       //keep sons that passed the test
       for(int s=0; s<4; s++) {
	 if(temp_sons.at(s)!=NULL)
	   new_nodes.push_back(temp_sons.at(s));
       }
       temp_sons.clear();
       //erase parent (no longer needed)
       if(l!=TREE_DEPTH-1)
	 delete parent_list.at(n);

     } //end loop over parent_list
	  
     if(l!=TREE_DEPTH-1)
       parent_list = new_nodes;
   }


 


   
   maxVote=-1;
  Hough2DNode* maxNode=NULL;
  for(std::vector<Hough2DNode*>::iterator it=parent_list.begin();it!=parent_list.end();++it){
    if((*it)->getVote()>maxVote){
      maxVote=(*it)->getVote();
      maxNode=(*it);
    }
  }
  if(debug){
    cout<<"maxVote "<<maxVote<<endl;
  }
  if(parent_list.size()>0){
    bool cont=true;
    int counter =0;
    //   debug=true;
    while(cont) {
      counter++;
      //sort nodes by final votes
      bool* bestHitList = parent_list.front()->getHitList();
      
       if(debug){
	std::cout<<parent_list.front()->getVote()<<std::endl;
	std::cout<<std::endl;	  
	for(int b=0; b<nClusters; b++){
	  //std::cout<<" bool nr "<<b<<": "<<*((bool*)(bestHitList+b*sizeof(bool)) )<<endl;
	  if(*((bool*)(bestHitList+b*sizeof(bool)) )) printf("bool nr %d is true\n",b);
	  else printf("bool nr %d is false\n",b);

	}

      }
    
       sort(parent_list.begin(), parent_list.end(), compareNodes);
      //extract clusters from best node
      bestHitList = parent_list.front()->getHitList();
      if(debug){
	std::cout<<parent_list.front()->getVote()<<std::endl;
	for(int b=0; b<nClusters; b++){
	  std::cout<<"bool "<<b<<": "<<bestHitList[b]<<endl;
	}
	std::cout<<std::endl;
      }
    
      int hits=0;
      for(int c=0; c<nClusters; c++) {
	if(bestHitList[c]){
	  if(debug){
	    cout<<"c "<<c<<": "<<bestHitList[c]<<" ";
	  }
	  ++hits;
	}
      }
      if(debug){
	cout<<endl;
      }
      if(hits<THRESHOLD) {
	if(debug){
	  cout<<"to few hits aborting********************************************************"<<endl;
	}
	cont=false;
	continue;
      }
      for(int i=0;i<nClusters;++i){
	solution_list.push_back(bestHitList[i]);
      }
      //remove hits for first node from all others
      for(int p=0; p<nClusters; p++) {
	if(!bestHitList[p]){
	  continue;
	}
	for(unsigned int n=0; n<parent_list.size(); n++){
	  if(debug){
	    cout<<" node n planes "<<parent_list.at(n)->getNplanes()<<" n clusters "<<nClusters<<endl;
	  }
	  if(parent_list.at(n)->checkHit(p)){
	    if(debug){
	      cout<<"hit "<<p<<" removed from node "<<n<<endl;
	    }
	    parent_list.at(n)->removeHit(p);
	  }
	}
      }
     
      solution_node_list.push_back(parent_list.front());
    }
    if(debug){
      std::cout<<"amount of maxima: "<<solution_node_list.size()<<endl;
    }
  }
  if(debug){
    cout<<"corners in solution node list************************************"<<endl;
  }
  for(unsigned int nli=0;nli<solution_node_list.size();++nli){
    float* corners2 = solution_node_list.at(nli)->getCorners();
    if(debug){
      cout<<"solution "<<nli<<endl;
    }
    if(debug){
      for(int ii=0; ii<8; ii++) {
	if(ii%2==0)
	std::cout<<std::endl;
	if(ii%2==0){
	  std::cout<<((corners2[ii]+0.5)*(m_Max-m_Min)+m_Min)<<"   ";
	}else{
	  std::cout<<((corners2[ii]+0.5)*(t_Max-t_Min)+t_Min)<<"   ";
	}
      }
    }

  }
}
void TCfast2DHough::draw(bool stop,int _x,int _y,int _w,int _h,TCevent* mcTruth){
  static TRandom r(0);
  char buf[10];
  sprintf(buf,"c%5.5f",r.Uniform());
  if(canv==NULL){
    cout<<endl<<endl<<"canvas created"<<endl<<endl<<endl;
    canv = new TCanvas(buf,buf,_x,_y,_w,_h);
  }else{
    delete canv;
    cout<<endl<<endl<<"new canvas created"<<endl<<endl<<endl;
    canv = new TCanvas(buf,buf,_x,_y,_w,_h);
  }
  
 
  gROOT->SetSelectedPad(canv->cd(1));
  
  for(unsigned int s=0; s<solution_node_list.size(); s++) {
    float* center = (solution_node_list[s])->getCenter();
    std::cout<<"maxima "<<(center[0]+0.5)*(m_Max-m_Min)
	     <<" "<<(center[1]+0.5)*(t_Max-t_Min)<<std::endl;
    float length = (solution_node_list[s])->getSideLength();
    float x1 = (center[0] - 0.5*length+0.5)*(m_Max-m_Min)+m_Min;
    float x2 = (center[0] + 0.5*length+0.5)*(m_Max-m_Min)+m_Min;
    float y1 = (center[1] - 0.5*length+0.5)*(t_Max-t_Min)+t_Min;
    float y2 = (center[1] + 0.5*length+0.5)*(t_Max-t_Min)+t_Min;
    boxlist.push_back(new TBox(x1,y1,x2,y2));
  }
  gStyle->SetPalette(1);
  houghSpace->SetStats(kFALSE);
  houghSpace->Draw();
  for(unsigned int h=0;h<ypHit.size();++h){
    houghLines.at(h)->Draw("same");
  }
  for(unsigned int b=0; b<boxlist.size(); ++b) {
    (boxlist[b])->SetLineColor(kPink+10);
    (boxlist[b])->SetFillStyle(0);
    (boxlist[b])->SetLineWidth(2);
    (boxlist[b])->Draw("l");
  }
  houghSpace->GetXaxis()->SetTitle("m");
  houghSpace->GetYaxis()->SetTitle("t (cm)");
  canv->Modified();
  canv->Update();
  if(stop){
    gApplication->SetReturnFromRun(true);
    gSystem->Run();
  }
}
TCfast2DHough::~TCfast2DHough(){
  delete houghSpace;
  for(unsigned int i =0; i<solution_node_list.size();++i){
    delete solution_node_list.at(i);
  }
  solution_node_list.clear();

  for(unsigned int i =0; i<hyperplanes.size();++i){
    delete hyperplanes.at(i);
  }
  hyperplanes.clear();
  if(canv!=NULL){
    delete canv;
  }

  for(unsigned int i=0;i<boxlist.size();++i){
    delete (boxlist.at(i));
  }
  boxlist.clear();
  for(unsigned int i=0;i<houghLines.size();++i){
    delete houghLines.at(i);
  }
  houghLines.clear();
}
void TCfast2DHough::convert(std::vector<TCcluster>& _c){
  TVector3 xp=yp.Cross(zp);
  TMatrixT<double> S(3,3);
  S[0][0]=xp.X();
  S[1][0]=xp.Y();
  S[2][0]=xp.Z();
  S[0][1]=yp.X();
  S[1][1]=yp.Y();
   S[2][1]=yp.Z();
  S[0][2]=zp.X();
  S[1][2]=zp.Y();
  S[2][2]=zp.Z();

  //TMatrixT<double> Stransp = S;
  //Stransp.T();

  clear();
  float yMax=-999999;
  float yMin=999999;
  float zMax=-999999;
  float zMin=999999;
  for(unsigned int i=0;i<_c.size();++i){
    TVector3 hitPrime = S * _c.at(i).posXYZ();
    //xprime isnt needed
    float y=hitPrime.Y();
    float z=hitPrime.Z();
    if(debug){
      std::cout<<"hough y "<<y<<" z "<<z<<std::endl;
    }
    ypHit.push_back(y);

    zpHit.push_back(z);
    if(y>yMax){
      yMax=y;
    }
    if(z>zMax){
      zMax=z;
    }
    if(y<yMin){
      yMin=y;
    }
    if(z<zMin){
      zMin=z;
    }
  }
  if((zMax - zMin)!=0){
    m_Min=-(yMax-yMin)/(zMax - zMin)-0.01;
  }else{
    m_Min =-0.2f;
  }
  m_Max=-m_Min;

  t_Min=yMin-m_Max*zMin-0.1;
  t_Max=yMax-m_Min*zMax+0.1;
}
void TCfast2DHough::clear(){
   
 
  if(canv!=NULL){
    canv->Clear();
  }
  if(houghSpace!=NULL){
    delete houghSpace;
    houghSpace=NULL;
  }
  ypHit.clear();
  zpHit.clear();
  for(unsigned int i=0;i<hyperplanes.size();++i){
    delete hyperplanes.at(i);
  }
  for(unsigned int i=0;i<node_list.size();++i){
    delete (node_list.at(i));
  }
  for(unsigned int i=0;i<boxlist.size();++i){
    delete (boxlist.at(i));
  }

  for(unsigned int i=0;i<houghLines.size();++i){
    delete (houghLines.at(i));
  }
  houghLines.clear();
  boxlist.clear();
  hyperplanes.clear();
  solution_list.clear();//content deleted with node_list
  solution_node_list.clear();//content deleted with node_list
  node_list.clear();

}
 
