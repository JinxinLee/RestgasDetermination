#include"../src/TCtrack.h"
#include"../src/TCalign.h"
#include<iostream>
#include<vector>

#include "TString.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1.h"
#include "TF1.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TPaveText.h"
#include "TROOT.h"
#include "TRandom.h"

using namespace std;

void doIt(){

  TRandom R(0);

  int id=240;

  TCalign* A = TCalign::getInstance("alignTest.txt");

  TMatrixT<double> rot(3,3);
  rot[1][1]=1.;
  double angle=20./180.*TMath::Pi();
  rot[0][0]=TMath::Cos(angle);;
  rot[2][2]=TMath::Cos(angle);
  rot[0][2]=TMath::Sin(angle);
  rot[2][0]=-1.*TMath::Sin(angle);

  TVector3 trans(0.,0.,0.);
  double pitch=0.3;
  A->setConv(id,trans,rot,pitch);
  
  rot.Print();
  

  TCtrack T;

  TVector3 startPos(0.1,0.1,0.1);

  double thX=30.;
  double thY=45.;
  TVector3 direction(1.,1.,1.);
  direction.SetX(TMath::Tan(thX*TMath::Pi()/180.));
  direction.SetY(TMath::Tan(thY*TMath::Pi()/180.));

  TVector3 err(0.1,0.05,0.2);

  direction.SetMag(1.);

  TMatrixT<double> rotTrans = rot;
  rotTrans.T();
  rotTrans.Print();
  TVector3 newDirection = rotTrans * direction;
  double dx=direction.X();
  double dy=direction.Y();
  double dz=direction.Z();
  double dxpr=newDirection.X();
  double dypr=newDirection.Y();
  double dzpr=newDirection.Z();
  PRINTF(sin(angle));
  PRINTF(cos(angle));
  direction.Print();
  newDirection.Print();
  PRINTF(180./TMath::Pi()*atan(dxpr/dzpr));
  PRINTF(180./TMath::Pi()*atan(dypr/dzpr));
  //  PRINTF(180./TMath::Pi()*atan(dx/(-1.*sin(angle)*dy+cos(angle)*dz)));
  PRINTF(180./TMath::Pi()*atan((cos(angle)*dx-sin(angle)*dz)/(sin(angle)*dx+cos(angle)*dz)));
  //  PRINTF(180./TMath::Pi()*atan((cos(angle)*dy+sin(angle)*dz)/(-1.*sin(angle)*dy+cos(angle)*dz)));
  PRINTF(180./TMath::Pi()*atan(dy/(sin(angle)*dx+cos(angle)*dz)));

  double distance=5.;
  int nSteps=10;


  for(int i=0;i<nSteps;++i){

    TVector3 pos(startPos.X()+direction.X()*distance*i,
		 startPos.Y()+direction.Y()*distance*i,
		 startPos.Z()+direction.Z()*distance*i);
   
    /*
    pos.SetX(R.Gaus(pos.X(),err.X()));
    pos.SetY(R.Gaus(pos.Y(),err.Y()));
    pos.SetZ(R.Gaus(pos.Z(),err.Z()));
    */

    double amp=((i%4)+1);

    TCcluster c(pos,err,amp,id);
    c.setFit(true);
    T.addCluster(c);
    //c.print();
  }
  T.fit(id);
  //T.print();
  PRINTF(T.getThX());
  PRINTF(T.getThY());
  T.draw();

}
