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
void trackHistContainer::fillRes(TCtrack* track,bool unbiased){
  TCalign* a = TCalign::getInstance();
  TMatrixT<double> rot(3,3);
  TVector3 dummyV;
  double dummyD;
  
  histogramChi2Prob->Fill(TMath::Prob(track->getChi2(),track->getNDF()));
  histogramChi2->Fill(track->getChi2());
  histogramRedChi2->Fill(track->getChi2()/track->getNDF());
  histogramChi2rough->Fill(track->getChi2()/track->getNDF());
  histogramChi2Rrough->Fill(track->getChi2()/track->getNDF());
  histogramNDF->Fill(track->getNDF());
  histogramAx->Fill(track->getAx());
  histogramAy->Fill(track->getAy());
  histogramBx->Fill(track->getBx());
  histogramBy->Fill(track->getBy());
  hitmapTPC->Fill(track->getAx()*100+track->getBx(),track->getAy()*100+track->getBy());

  for(unsigned int i=0;i<track->nCl();++i){
      
    TCcluster tmpcl = track->getCl(i);
    double uCl=tmpcl.posUVW().x();
    double z=tmpcl.posXYZ().z();
    TVector3 trackCross(track->getAx()*z+track->getBx(),track->getAy()*z+track->getBy(),z);
    TVector3 trackCrossUVW=a->XYZtoUVW(tmpcl.getId(),trackCross);
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
    bool fill=false;
    if(unbiased&&!(tmpcl.getFit())){
      fill=true;
    }else if(!unbiased&&(tmpcl.getFit())){
      fill=true;
    }
    if(fill&&tmpcl.getId()>0&&tmpcl.getId()<9){
      if(!unbiased){
        //      std::cout<<"filling "<<(tmpcl.getId()-1)<<std::endl;
        residual.at(tmpcl.getId()-1)->Fill(resid.x());
        residualVu2Dhist.at(tmpcl.getId()-1)->Fill(uCl,resid.x());
        residualVuProfile.at(tmpcl.getId()-1)->Fill(uCl,resid.x());
        residualVuTrack2Dhist.at(tmpcl.getId()-1)->Fill(trackCrossUVW.x(),resid.x());
        residualVuTrackProfile.at(tmpcl.getId()-1)->Fill(trackCrossUVW.x(),resid.x());
        xPos.at(tmpcl.getId()-1)->Fill(tmpcl.posXYZ().x());
        yPos.at(tmpcl.getId()-1)->Fill(tmpcl.posXYZ().y());
        u.at(tmpcl.getId()-1)->Fill(uCl);
        trackErr.at(tmpcl.getId()-1)->Fill(sqrt(sigmaU[0][0]));
        trackErrProfile.at(tmpcl.getId()-1)->Fill(uCl,sqrt(sigmaU[0][0]));
      }else{
        double residX=trackCrossUVW.x()-uCl;
        residual.at(tmpcl.getId()-1)->Fill(residX);
        residualVu2Dhist.at(tmpcl.getId()-1)->Fill(uCl,residX);
        residualVuProfile.at(tmpcl.getId()-1)->Fill(uCl,residX);
        residualVuTrack2Dhist.at(tmpcl.getId()-1)->Fill(trackCrossUVW.x(),residX);
        residualVuTrackProfile.at(tmpcl.getId()-1)->Fill(trackCrossUVW.x(),residX);
        xPos.at(tmpcl.getId()-1)->Fill(tmpcl.posXYZ().x());
        yPos.at(tmpcl.getId()-1)->Fill(tmpcl.posXYZ().y());
        u.at(tmpcl.getId()-1)->Fill(uCl);
        trackErr.at(tmpcl.getId()-1)->Fill(sqrt(sigmaU[0][0]));
        trackErrProfile.at(tmpcl.getId()-1)->Fill(uCl,sqrt(sigmaU[0][0]));
      }
    }
  }
  //  cout<<endl<<endl;
}
  
trackHistContainer::trackHistContainer(){
  histogramChi2Rrough= new TH1D("redChi2rough", "redChi2rough", 10000, 0, 100000);
  histogramChi2rough = new TH1D("redChi2r", "redChi2r", 5000, 0, 1000);
  histogramRedChi2 = new TH1D("redChi2", "redChi2", 1000, 0, 50);
  histogramChi2 = new TH1D("Chi2", "Chi2", 1000, 0, 50);
  histogramChi2Prob = new TH1D("Chi2Prob", "Chi2Prob", 1000, 0, 1);
  histogramNDF = new TH1D("NDF", "NDF", 8, 0, 8);
  histogramAx = new TH1D("trackAx", "trackAx", 10000, -0.2, 0.2);
  histogramAy = new TH1D("trackAy", "trackAy", 10000, -0.2, 0.2);
  histogramBx = new TH1D("trackBx", "trackBx", 10000, -4, 4);
  histogramBy = new TH1D("trackBy", "trackBy", 10000, -2, 2);
  hitmapTPC=new TH2D("hitmapTPC", "hitmapTPC", 256,-20,20,256,-20,20);
  for(unsigned int i=0;i<8;++i){
    char buf[10];
    sprintf(buf,"%i",(i+1));
    string name="residual_det";
    name+=buf;
    if(i>1&&i<6){
      residual.push_back(new TH1D(name.c_str(),name.c_str(),32000,-0.05,0.05));
    }else{
      residual.push_back(new TH1D(name.c_str(),name.c_str(),16000,-1,1));
    }

    name="residual_vs_u_det";
    name+=buf;
    residualVu2Dhist.push_back(new TH2D(name.c_str(), name.c_str(), 800, 0,10.24,2400, -.5,.5));
    name="residual_vs_u_prof_det";
    name+=buf;
    residualVuProfile.push_back(new TProfile(name.c_str(), name.c_str(), 800, 0,10.24,-1,1));
    name="residual_vs_u_track_det";
    name+=buf;
    residualVuTrack2Dhist.push_back(new TH2D(name.c_str(), name.c_str(), 800, 0,10.24,2400, -.5,.5));
    name="residual_vs_u_track_prof_det";
    name+=buf;
    residualVuTrackProfile.push_back(new TProfile(name.c_str(), name.c_str(), 800, 0,10.24,-1,1));
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
    trackErr.push_back(new TH1D(name.c_str(),name.c_str(),4000,-0.01,0.01));
    name="trackErrPro_det";
    name+=buf;
    trackErrProfile.push_back(new TProfile(name.c_str(), name.c_str(), 400, 0,10.24,-0.1,0.1));
  }
}
trackHistContainer::~trackHistContainer(){
  for(unsigned int i=0;i<8;++i){
    delete residual.at(i);
    delete residualVu2Dhist.at(i);
    delete residualVuProfile.at(i);
    delete residualVuTrack2Dhist.at(i);
    delete residualVuTrackProfile.at(i);
    delete xPos.at(i);
    delete yPos.at(i);
    delete u.at(i);
    delete trackErr.at(i);
    delete trackErrProfile.at(i);
  }
  delete hitmapTPC;
  delete histogramNDF;
  delete histogramChi2;
  delete histogramChi2rough;
  delete histogramChi2Rrough;
  delete histogramAx;
  delete histogramAy;
  delete histogramBx;
  delete histogramBy;
  delete histogramRedChi2;
  delete histogramChi2Prob;
}
void trackHistContainer::write(std::string outfile){
  TFile* file = new TFile(outfile.c_str(),"RECREATE");
  for(unsigned int i=0;i<8;++i){
    residual.at(i)->Write();
    residualVu2Dhist.at(i)->Write();
    residualVuProfile.at(i)->Write();
    residualVuTrack2Dhist.at(i)->Write();
    residualVuTrackProfile.at(i)->Write();
    xPos.at(i)->Write();
    yPos.at(i)->Write();
    u.at(i)->Write();
    trackErr.at(i)->Write();
    trackErrProfile.at(i)->Write();
  }
  hitmapTPC->Write();
  histogramNDF->Write();
  histogramChi2->Write();
  histogramRedChi2->Write();
  histogramChi2Prob->Write();
  histogramChi2rough->Write();
  histogramChi2Rrough->Write();
  histogramAx->Write();
  histogramAy->Write();
  histogramBx->Write();
  histogramBy->Write();

  file->Close();
  delete file;
}
