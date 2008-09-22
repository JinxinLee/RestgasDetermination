//g++ particleGun.cxx -o particleGun `root-config --glibs --cflags`
#include<iostream>
#include<sstream>
#include<fstream>
#include<vector>
#include<map>

#include"ConfigFile.h"

#include "TRandom.h"
#include "TApplication.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TVector3.h"
#include "TF1.h"
#include "TMath.h"
#include "TPolyLine3D.h"

std::map<std::string,int> confI;
std::map<std::string,double> confD;
ConfigFile cf;
TRandom myRand(0);

void readConfI(std::string var) {
  if(!(cf.readInto(confI[var] , var) )) {
    std::cerr << "Reading parameter " << var << " from conf file failed ->abort"
	      << std::endl;
    throw;
  }
}
void readConfD(std::string var) {
  if(!(cf.readInto(confD[var] , var) )) {
    std::cerr << "Reading parameter " << var << " from conf file failed ->abort"
	      << std::endl;
    throw;
  }
}

void getXY(double z,const TVector3& pos, const TVector3& mom,double& x,double& y){
  /*
    X=P+t*M
    z-component: z=Pz+t*Mz <=> t=(z-Pz)/Mz
  */
  double t=(z-pos.Z())/mom.Z();
  x=pos.X()+t*mom.X();
  y=pos.Y()+t*mom.Y();
}
void getXZ(double y,const TVector3& pos, const TVector3& mom,double& x,double& z){
  /*
    X=P+t*M
    y-component: y=Py+t*My <=> t=(y-Py)/My
  */
  double t=(y-pos.Y())/mom.Y();
  x=pos.X()+t*mom.X();
  z=pos.Z()+t*mom.Z();
}
void getYZ(double x,const TVector3& pos, const TVector3& mom,double& y,double& z){
  /*
    X=P+t*M
    x-component: x=Px+t*Mx <=> t=(x-Px)/Mx
  */
  double t=(x-pos.X())/mom.X();
  y=pos.Y()+t*mom.Y();
  z=pos.Z()+t*mom.Z();
}

bool acceptance(const TVector3& pos, const TVector3& mom){
  double x,y,z;

  //check for hit in bottom scintillator
  getXY(confD["BOTZ"],pos,mom,x,y);
  if(x<confD["BOTXMIN"] || x>confD["BOTXMAX"]) return false;
  if(y<confD["BOTYMIN"] || y>confD["BOTYMAX"]) return false;


  //check for penetration of upper and lower 'endcap' of readout volume
  getXY(confD["TPCZMIN"],pos,mom,x,y);
  if(x>confD["TPCXMIN"] && x<confD["TPCXMAX"] && 
     y>confD["TPCYMIN"] && y<confD["TPCYMAX"]) return true;
  getXY(confD["TPCZMAX"],pos,mom,x,y);
  if(x>confD["TPCXMIN"] && x<confD["TPCXMAX"] && 
     y>confD["TPCYMIN"] && y<confD["TPCYMAX"]) return true;
  
  //check for crossing of readout volume without endcap penetration
  getYZ(confD["TPCXMIN"],pos,mom,y,z);
  if(y>confD["TPCYMIN"] && y<confD["TPCYMAX"] && 
     z>confD["TPCZMIN"] && z<confD["TPCZMAX"]) return true;
  getYZ(confD["TPCXMAX"],pos,mom,y,z);
  if(y>confD["TPCYMIN"] && y<confD["TPCYMAX"] && 
     z>confD["TPCZMIN"] && z<confD["TPCZMAX"]) return true;
  getXZ(confD["TPCYMIN"],pos,mom,x,z);
  if(x>confD["TPCXMIN"] && x<confD["TPCXMAX"] && 
     z>confD["TPCZMIN"] && z<confD["TPCZMAX"]) return true;
  getXZ(confD["TPCYMAX"],pos,mom,x,z);
  if(x>confD["TPCXMIN"] && x<confD["TPCXMAX"] && 
     z>confD["TPCZMIN"] && z<confD["TPCZMAX"]) return true;


  return false;

}

void makeCosmic(TVector3& pos,TVector3& mom){
  static int counter(0);
  static int attemptCounter(0);
  do{
    ++attemptCounter;
    pos.SetZ(confD["TOPZ"]);
    pos.SetX(myRand.Uniform(confD["TOPXMIN"],confD["TOPXMAX"]));
    pos.SetY(myRand.Uniform(confD["TOPYMIN"],confD["TOPYMAX"]));
    
    mom.SetXYZ(1.,0.,-1.);
    mom.SetPhi(myRand.Uniform(0.,2.*TMath::Pi()));
    mom.SetTheta(myRand.Uniform(0.5*TMath::Pi(),TMath::Pi()));
    mom.SetMag(1.);
  }while(!acceptance(pos,mom));
  ++counter;
  if(counter % 1000 == 0){
    std::cout << counter << " events generated from " << ((float)attemptCounter) << " attempts" << std::endl;
  }

}

int main(int argc,char **argv){

  if(argc != 2) {
    std::cerr << "NumArgs is not 1. Args: configfile" << std::endl;
    throw;
  }
  

  std::string args[argc-1];
  for(int i=0;i<argc-1;++i){
    args[i]=argv[i+1];
  }
  
  std::cout << "Opening config file " << args[0] << "..." << std::endl;
  cf = ConfigFile( args[0].c_str() );

  readConfI("NEV");
  readConfD("TOPXMIN");
  readConfD("TOPXMAX");
  readConfD("TOPYMIN");
  readConfD("TOPYMAX");
  readConfD("TOPZ");
  readConfD("BOTXMIN");
  readConfD("BOTXMAX");
  readConfD("BOTYMIN");
  readConfD("BOTYMAX");
  readConfD("BOTZ");
  readConfD("TPCXMIN");
  readConfD("TPCXMAX");
  readConfD("TPCYMIN");
  readConfD("TPCYMAX");
  readConfD("TPCZMIN");
  readConfD("TPCZMAX");


  TApplication* app;
  TCanvas* canv;
  app = new TApplication("app",NULL,NULL);
  canv = new TCanvas("canv","");
  canv->Draw();

  TH1D *hTL = new TH1D("hTL","hTL",100,0.,13.);
  TH1I *hNCL = new TH1I("hNCL","hNCL",30,0,30);
  TH1D *hZ = new TH1D("hZ","hZ",100,0.,8.);

  static const double avDistance=.5;
  char buf[50];
  sprintf(buf,"1/%f*exp(-x/%f)",avDistance,avDistance);
  TF1* distF = new TF1("distF",buf,0.,1.E2);
  

  for(int iev=0;iev<confI["NEV"];++iev){
    TVector3 pos,mom;
    
    makeCosmic(pos,mom);

    double trackLength;
    bool p1z=false;
    bool p2z=false;
    bool p1x=false;
    bool p2x=false;
    bool p1y=false;
    bool p2y=false;
    double x1z,x2z,y1z,y2z;
    double z1x,z2x,y1x,y2x;
    double x1y,x2y,z1y,z2y;


    getXY(confD["TPCZMIN"],pos,mom,x1z,y1z);
    getXY(confD["TPCZMAX"],pos,mom,x2z,y2z);
    getYZ(confD["TPCXMIN"],pos,mom,y1x,z1x);
    getYZ(confD["TPCXMAX"],pos,mom,y2x,z2x);
    getXZ(confD["TPCYMIN"],pos,mom,x1y,z1y);
    getXZ(confD["TPCYMAX"],pos,mom,x2y,z2y);

    if(x1z>confD["TPCXMIN"]&&x1z<confD["TPCXMAX"]&&
       y1z>confD["TPCYMIN"]&&y1z<confD["TPCYMAX"])p1z=true;
    if(x2z>confD["TPCXMIN"]&&x2z<confD["TPCXMAX"]&&
       y2z>confD["TPCYMIN"]&&y2z<confD["TPCYMAX"])p2z=true;
    if(y1x>confD["TPCYMIN"]&&y1x<confD["TPCYMAX"]&&
       z1x>confD["TPCZMIN"]&&z1x<confD["TPCZMAX"])p1x=true;
    if(y2x>confD["TPCYMIN"]&&y2x<confD["TPCYMAX"]&&
       z2x>confD["TPCZMIN"]&&z2x<confD["TPCZMAX"])p2x=true;
    if(x1y>confD["TPCXMIN"]&&x1y<confD["TPCXMAX"]&&
       z1y>confD["TPCZMIN"]&&z1y<confD["TPCZMAX"])p1y=true;
    if(x2y>confD["TPCXMIN"]&&x2y<confD["TPCXMAX"]&&
       z2y>confD["TPCZMIN"]&&z2y<confD["TPCZMAX"])p2y=true;

    //std::cout << p1x << " " << p2x << " " << p1y << " " << p2y << " " << p1z << " " << p2z << std::endl;
    int count=0;
    if(p1x)++count;
    if(p2x)++count;
    if(p1y)++count;
    if(p2y)++count;
    if(p1z)++count;
    if(p2z)++count;
    assert(count == 2);

    std::vector<TVector3> vecs;
    TVector3 p1;
    
    if(p1x){p1.SetXYZ(confD["TPCXMIN"],y1x,z1x);vecs.push_back(p1);}
    if(p2x){p1.SetXYZ(confD["TPCXMAX"],y2x,z2x);vecs.push_back(p1);}
    if(p1y){p1.SetXYZ(x1y,confD["TPCYMIN"],z1y);vecs.push_back(p1);}
    if(p2y){p1.SetXYZ(x2y,confD["TPCYMAX"],z2y);vecs.push_back(p1);}
    if(p1z){p1.SetXYZ(x1z,y1z,confD["TPCZMIN"]);vecs.push_back(p1);}
    if(p2z){p1.SetXYZ(x2z,y2z,confD["TPCZMAX"]);vecs.push_back(p1);}

    assert(vecs.size()==2);

    trackLength=(vecs[0]-vecs[1]).Mag();
    //    std::cout << trackLength << std::endl << "===============" << std::endl;
    hTL->Fill(trackLength);


    if(vecs[0].z()<vecs[1].z()) {
      TVector3 temp;
      temp=vecs[0];
      vecs[0]=vecs[1];
      vecs[1]=temp;
    }
    
    int nClus=1;
    double distance=avDistance;
    for(int i=1;i<100;++i){
      if(trackLength/avDistance > i){
	nClus=i+2;
      }
    }
    hNCL->Fill(nClus);


    

    if(nClus>1){
      distance=trackLength/(nClus-1);
      std::cout << avDistance << " " << distance << std::endl;
      //TAG hZ->Fill(vecs[0].z());
      TVector3 point=vecs[0];
      TVector3 direction=mom;
      direction.Print();
      TVector3 v=vecs[1]-vecs[0];
      v.SetMag(1.);
      v.Print();
      direction.SetMag(1.);
      for(int i=0;i<nClus;++i){
	point+=distance*direction;
	//TAG hZ->Fill(point.z());
      }
    }
  }
  new TCanvas("c1","c1");
  hTL->Draw();
  new TCanvas("c2","c2");
  //  hZ->Draw();
  for(int i=0;i<1000000;++i){
    //    hZ->Fill(distF->GetRandom());
    double r = gRandom->Uniform();
  }
  hZ->Draw();
  new TCanvas("c3","c3");
  hNCL->Draw();
  //distF->Draw();
  app->Run();
  
 
}
