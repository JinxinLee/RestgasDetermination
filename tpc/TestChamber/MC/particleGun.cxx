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
#include "TVector3.h"
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
    mom.SetMag(confD["MOMENTUM"]);
  }while(!acceptance(pos,mom));
  ++counter;
  if(counter % 1000 == 0){
    std::cout << counter << " events generated from " << ((float)attemptCounter) << " attempts" << std::endl;
  }

}

int main(int argc,char **argv){

  if(argc != 3) {
    std::cerr << "NumArgs is not 2. Args: configfile outfile" << std::endl;
    throw;
  }
  

  std::string args[argc-1];
  for(int i=0;i<argc-1;++i){
    args[i]=argv[i+1];
  }
  
  std::cout << "Opening config file " << args[0] << "..." << std::endl;
  cf = ConfigFile( args[0].c_str() );

  std::cout << "Opening output file " << args[1] << "..." << std::endl;
  std::ofstream f(args[1].c_str());

  readConfI("NEV");
  readConfI("PDG");
  readConfD("MOMENTUM");
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

  readConfI("ROOTOUTPUT");

  bool ROOT=false;
  if(confI["ROOTOUTPUT"]==1)ROOT=true;

  TApplication* app;
  TCanvas* canv;
  TPolyLine3D* lines[confI["NEV"]];
  if(ROOT){
    app = new TApplication("app",NULL,NULL);
    canv = new TCanvas("canv","");
    canv->Draw();
  }

  for(int iev=0;iev<confI["NEV"];++iev){
    TVector3 pos,mom;
    makeCosmic(pos,mom);

    if(ROOT){
      lines[iev] = new TPolyLine3D(2);
      TVector3 _dir = mom;
      _dir.SetMag(3);
      static const double z1Draw = confD["TOPZ"]-5.;
      static const double z2Draw = confD["BOTZ"]+5.;
      double x,y;
      getXY(z1Draw,pos,mom,x,y);
      lines[iev]->SetPoint(0,x,y,z1Draw);
      getXY(z2Draw,pos,mom,x,y);
      lines[iev]->SetPoint(1,x,y,z2Draw);
    }

    f << "1 " << iev << " " << pos.X() << " "
      << pos.Y() << " " << pos.Z() << std::endl;
    f << confI["PDG"]  << " " << mom.X() << " "
      << mom.Y() << " " << mom.Z() << std::endl;

  }
  f.close();
  if(ROOT){
    for(int iev=0;iev<confI["NEV"];++iev){
      lines[iev]->Draw("same");
    }
    app->Run();
  }
}
