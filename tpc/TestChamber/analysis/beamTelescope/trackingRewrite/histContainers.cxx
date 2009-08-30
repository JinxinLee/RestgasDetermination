#include <TMatrixT.h>
#include <TVector3.h>
#include <TFile.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TProfile.h>
#include <iostream>
#include <string>
#include "histContainers.h"
#include "TCtrack.h"
#include "TCalign.h"
using std::string;
void trackHistContainer::fillRes(TCtrack* track){
  TCalign* a = TCalign::getInstance();
  TMatrixT<double> rot(3,3);
  TVector3 dummyV;
  double dummyD;

  histogramChi2->Fill(track->getChi2()/track->getNDF());
  histogramChi2rough->Fill(track->getChi2()/track->getNDF());
  histogramChi2Rrough->Fill(track->getChi2()/track->getNDF());
  histogramNDF->Fill(track->getNDF());
  for(unsigned int i=0;i<track->nCl();++i){
      
    TCcluster tmpcl = track->getCl(i);
    double uCl=tmpcl.posUVW().x();
    double z=tmpcl.posXYZ().z();
    TVector3 resid=tmpcl.getRes();

    TMatrixT<double> coVarTrackParam = track->getErrMatrix();
    a->getConv(tmpcl.getId(),dummyV,rot,dummyD);
    
    TMatrixT<double> D(2,1);
    D[0][0]=rot[0][0];
    D[1][0]=rot[1][0];
    TMatrixT<double> D_T= D.T();
    D.T();
    TMatrixT<double> T(4,2);
    T[0][0]=z; T[0][1]=0;
    T[1][0]=1; T[1][1]=0;
    T[2][0]=0; T[2][1]=z;
    T[3][0]=0; T[3][1]=1;
    TMatrixT<double> T_T=T.T();
    T.T();
    TMatrixT<double> sigmaU= D_T*T_T*coVarTrackParam*T*D;
    //    double dx=1;//sqrt(z*z*dax*dax+dbx*dbx+2*z*covar[0][1]);
    //double dy=1;//sqrt(z*z*day*day+dby*dby+2*z*covar[2][3]);
    //  cout<<tmpcl.getId()<<"  Z   "<<z<<" ....................dx "<<dx<<" dy "<<dy<<endl;
    if(tmpcl.getFit()&&tmpcl.getId()>0&&tmpcl.getId()<9){
      residual.at(tmpcl.getId()-1)->Fill(resid.x());
      residualVu2Dhist.at(tmpcl.getId()-1)->Fill(uCl,resid.x());
      residualVuProfile.at(tmpcl.getId()-1)->Fill(uCl,resid.x());
      xPos.at(tmpcl.getId()-1)->Fill(tmpcl.posXYZ().x());
      yPos.at(tmpcl.getId()-1)->Fill(tmpcl.posXYZ().y());
      u.at(tmpcl.getId()-1)->Fill(uCl);
      trackErr.at(tmpcl.getId()-1)->Fill(sqrt(sigmaU[0][0]));
      trackErrProfile.at(tmpcl.getId()-1)->Fill(uCl,sqrt(sigmaU[0][0]));
    }
  }
  //  cout<<endl<<endl;
}
  
trackHistContainer::trackHistContainer(){
  histogramChi2Rrough= new TH1D("Chi2rough", "Chi2rough", 10000, 0, 100000);
  histogramChi2rough = new TH1D("Chi2r", "Chi2r", 5000, 0, 1000);
  histogramChi2 = new TH1D("Chi2", "Chi2", 1000, 0, 50);
  histogramNDF = new TH1D("NDF", "NDF", 8, 0, 8);
  for(unsigned int i=0;i<8;++i){
    char buf[10];
    sprintf(buf,"%i",(i+1));
    string name="residual_det";
    name+=buf;
    residual.push_back(new TH1D(name.c_str(),name.c_str(),4000,-1,1));

    name="residual_vs_u_det";
    name+=buf;
    residualVu2Dhist.push_back(new TH2D(name.c_str(), name.c_str(), 400, 0,10.24,400, -1,1));
    name="residual_vs_u_prof_det";
    name+=buf;
    residualVuProfile.push_back(new TProfile(name.c_str(), name.c_str(), 400, 0,10.24,-1,1));
    name="x_det";
    name+=buf;
    xPos.push_back(new TH1D(name.c_str(),name.c_str(),4000,-10,10));
    name="y_det";
    name+=buf;
    yPos.push_back(new TH1D(name.c_str(),name.c_str(),4000,-10,10));
    name="u_det";
    name+=buf;
    u.push_back(new TH1D(name.c_str(),name.c_str(),4000,0,10.24));
    name="trackErr_det";
    name+=buf;
    trackErr.push_back(new TH1D(name.c_str(),name.c_str(),4000,-1,1));
    name="trackErrPro_det";
    name+=buf;
    trackErrProfile.push_back(new TProfile(name.c_str(), name.c_str(), 400, 0,10.24,-1,1));
  }
}
trackHistContainer::~trackHistContainer(){
  for(unsigned int i=0;i<8;++i){
    delete residual.at(i);
    delete residualVu2Dhist.at(i);
    delete residualVuProfile.at(i);
    delete xPos.at(i);
    delete yPos.at(i);
    delete u.at(i);
    delete trackErr.at(i);
    delete trackErrProfile.at(i);
  }
  delete histogramNDF;
  delete histogramChi2;
  delete histogramChi2rough;
  delete histogramChi2Rrough;
}
void trackHistContainer::write(std::string outfile){
  TFile* file = new TFile(outfile.c_str(),"RECREATE");
  for(unsigned int i=0;i<8;++i){
    residual.at(i)->Write();
    residualVu2Dhist.at(i)->Write();
    residualVuProfile.at(i)->Write();
    xPos.at(i)->Write();
    yPos.at(i)->Write();
    u.at(i)->Write();
    trackErr.at(i)->Write();
    trackErrProfile.at(i)->Write();
  }
  histogramNDF->Write();
  histogramChi2->Write();
  histogramChi2rough->Write();
  histogramChi2Rrough->Write();


  file->Close();
  delete file;
}
