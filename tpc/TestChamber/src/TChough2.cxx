#include "TChough2.h"
#include<cmath>
#include"TGraph.h"
#include"TGraphErrors.h"
#include"TCanvas.h"
#include"TApplication.h"
#include"TSystem.h"
#include"TRandom.h"
#include"TEllipse.h"
#include <utility>
#include <iostream>
#include "TH2D.h"
#include "TROOT.h"
#include "TStyle.h"
using std::pair;
using std::cout;
using std::endl;
TChough2::TChough2(const TVector3& _yp,const TVector3& _zp,double _rR) : TCabsHough(_yp,_zp){

  nBinsR = 500;
  nBinsTheta =500;
 
  cutR=_rR;

  canvas1 = NULL;
  houghHisto=NULL;
  for(int i=0;i<Max_NumberOfHits;i++) {
	houghLines[i]=NULL;
  }
}

TChough2::~TChough2(){
}

void TChough2::draw(bool stop,int _z,int _y,int _w,int _h){
  double z[ypHit.size()];
  double y[ypHit.size()];
  double thetaMax_[maxVector.size()];
  double thetaMaxE_[maxVector.size()];
  double rMax_[maxVector.size()];
  double rMaxE_[maxVector.size()];
  
  for(unsigned int i=0;i<(ypHit.size());++i){
    z[i]=zpHit.at(i);
    y[i]=ypHit.at(i);
  }
  for(unsigned int i=0;i<maxVector.size();++i){
    thetaMax_[i]=maxVector.at(i).first*binTheta+minTheta -binTheta/2;
    rMax_[i]=maxVector.at(i).second*binR+minR-binR/2;
    thetaMaxE_[i]=0;
    if(cutR==0){
      cutR=binR/2;
    }
    rMaxE_[i]=cutR;
    
  }
  static TRandom r(0);
  char buf[10];
  
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1);

  sprintf(buf,"c%5.5f",r.Uniform());
  TCanvas *c = new TCanvas(buf,"Hough Transformation R #theta",_z,_y,_w,_h);
  TGraph* g = new TGraph(ypHit.size(),z,y);
  TGraphErrors* maxPoints = new TGraphErrors(maxVector.size(),thetaMax_,rMax_,thetaMaxE_,rMaxE_);

  c->Divide(1,2);
  c->cd(1);
  g->SetTitle("");
  g->GetXaxis()->SetTitle("z");
  g->SetMarkerStyle(24);
  g->SetMarkerSize(2.);
  g->Draw("AP");
    
  std::vector<int> selHits;
  std::vector<int> selHits2;
  for(unsigned int i=0;i<ypHit.size();++i){
    if(hot(i,0)){
      selHits.push_back(i);
      houghLines[i]->SetLineStyle(1);
      houghLines[i]->SetLineWidth(1);
      houghLines[i]->SetLineColor(kGreen);
    }
    if(hot(i,1)){
      selHits2.push_back(i);
      houghLines[i]->SetLineStyle(1);
      houghLines[i]->SetLineWidth(1);
      houghLines[i]->SetLineColor(kBlue);
    }
    if(hot(i,1)&&hot(i,0)){
      houghLines[i]->SetLineStyle(1);
      houghLines[i]->SetLineWidth(1);
      houghLines[i]->SetLineColor(kRed);

    }
  }
  cout<<"checking hotnes"<<endl;

  double zsel[selHits.size()];
  double ysel[selHits.size()];

  double zsel2[selHits.size()];
  double ysel2[selHits.size()];
  for(unsigned int i=0;i<selHits.size();++i){
    cout<<"hitIndex "<<selHits.at(i)<<" Max nr "<<0<<endl;
    zsel[i]=zpHit.at(selHits.at(i));
    ysel[i]=ypHit.at(selHits.at(i));
  }
  for(unsigned int i=0;i<selHits2.size();++i){
    cout<<"hitIndex "<<selHits2.at(i)<<" Max nr "<<1<<endl;
    zsel2[i]=zpHit.at(selHits2.at(i));
    ysel2[i]=ypHit.at(selHits2.at(i));
  }
  if(selHits.size()>0){
    TGraph* gsel = new TGraph(selHits.size(),zsel,ysel);
    gsel->SetMarkerStyle(2);
    gsel->SetMarkerColor(kGreen);
    gsel->Draw("P");
  }
 if(selHits2.size()>0){
    TGraph* gsel2 = new TGraph(selHits2.size(),zsel2,ysel2);
    gsel2->SetMarkerStyle(2);
    gsel2->SetMarkerColor(kBlue);
    gsel2->Draw("P");
  }

  c->cd(2);
  houghHisto->Draw("colz");

  for(int i=0;i<NumberOfHits;++i){
    houghLines[i]->SetLineWidth(1);
    houghLines[i]->Draw("same");
  }

  maxPoints->SetMarkerStyle(2);
  maxPoints->SetMarkerSize(5.);
  maxPoints->SetMarkerColor(kRed);
  maxPoints->Draw("*P");
  c->Update();
  c->Modified();
  if(stop){
    cout<<"run"<<endl;
    gApplication->SetReturnFromRun(kTRUE);
    gSystem->Run();
  }

}

void TChough2::doHough(){
  cleanup();
  NumberOfHits = nHits();
  assert(NumberOfHits<=Max_NumberOfHits);
  for(int i=0;i<NumberOfHits;i++) {

    HitCoordinates[i][0] = ypHit.at(i)-minY;
    // cout<<"hitY "<<i<<" "<<ypHit.at(i)-minY<<", ";
    HitCoordinates[i][1] = zpHit.at(i)-minZ;
    //cout<<"hitZ "<<i<<" "<<zpHit.at(i)-minZ<<endl;
  }

  makeHoughLines();
  makeHoughHisto();

  findMaxInHisto(maxVector,nMax);

}

bool TChough2::hot(int clIndex,int maxIndex){
  assert(clIndex<nHits());
  assert(clIndex>-1);
  assert(maxIndex<nMax);
  double z,y;
  double discr;

  z = HitCoordinates[clIndex][1];
  y = HitCoordinates[clIndex][0];
  
  int maxBinTheta=maxVector.at(maxIndex).first;
  int maxBinR=maxVector.at(maxIndex).second;
  
  double thetaAtMax=maxBinTheta*binTheta+minTheta-binTheta/2;
  double rAtMax=maxBinR*binR+minR-binR/2;
  double rPoint=z*cos(thetaAtMax) +y*sin(thetaAtMax);
  discr=-fabs(rPoint-rAtMax);
  // cout<<"discr "<<-discr<<endl;
  if(cutR==0){
    cutR=binR/2;
  }
  discr+=cutR;
  if(discr<0){
    return false;
  }
  return true;
  
}


void TChough2::findMaxInHisto(std::vector<std::pair<int,int> > &_maxVector, int _nMax) {
  TH2D* houghHistoCopy = new TH2D(*houghHisto);
  for(int i=0;i<nMax;i++){
    int maxBinTheta;
    int maxBinR;
    int zDummy;
    houghHistoCopy->GetMaximumBin(maxBinTheta,maxBinR,zDummy);
    for (int k=maxBinTheta-5;k<=maxBinTheta+10;k++){
      for(int j=maxBinR-5;j<=maxBinR+10;j++){
        if(k<0){
          continue;
        }
        else if(k>nBinsTheta){
          continue;
        }
        if(j<0){
          continue;
        }
        else if(j>nBinsR){
          continue;
        }
        houghHistoCopy->SetBinContent(k,j,0);
      }
    }
    pair<int,int> max;
    max.first=maxBinTheta;
    max.second=maxBinR;
    cout<<"maxBinTheta "<<maxBinTheta<<" maxBinR "<<maxBinR<<endl;
    maxVector.push_back(max);
  }
  delete houghHistoCopy;

}



void TChough2::cleanup() {
  //  std::cout << "Cleaning up..." << std::endl;

  delete canvas1;



  delete houghHisto;

  for(int i=0;i<Max_NumberOfHits;i++) {
	delete houghLines[i];
  }

  for(int i=0;i<Max_NumberOfHits;i++) {
	selHits[i] = -1;
	nSelHits = 0;
  }

  canvas1 = NULL;

  maxVector.clear();

  houghHisto=NULL;

  for(int i=0;i<Max_NumberOfHits;i++) {
	houghLines[i]=NULL;
  }
  //  std::cout << "done cleaning up..." << std::endl;

}

 
 

/*
  r(theta) = z*cos(theta) + y*sin(theta)
*/
void TChough2::makeHoughLines(){
  char buf[50];
  char bufName[50];
  for(int i=0;i<NumberOfHits;i++) {
	sprintf(buf,"%f*sin(x)+%f*cos(x)",HitCoordinates[i][0],HitCoordinates[i][1]);
	sprintf(bufName,"copy%f*sin(x)+%f*cos(x)",HitCoordinates[i][0],HitCoordinates[i][1]);
	//	cout<<"minTheta "<<minTheta<<" maxTheta "<<maxTheta<<endl;
	houghLines[i] = new TF1(bufName,buf,minTheta,maxTheta);
	//	  houghLines[i]->SetLineColor(kBlue);
	houghLines[i]->SetLineStyle(1);
  }
}


void TChough2::findHitsOnTrack(){
 

  for(int i=0;i<Max_NumberOfHits;i++) {
	selHits[i] = -1;
  }
  nSelHits = 0;
  

  for(int ihit=0;ihit<NumberOfHits;ihit++) {
    if(hot(ihit)) {
      selHits[nSelHits] = ihit;
      nSelHits++;
      houghLines[ihit]->SetLineStyle(1);
      houghLines[ihit]->SetLineWidth(1);
      houghLines[ihit]->SetLineColor(kGreen);
    }
  }
}


void TChough2::makeHoughHisto(){
  char buf[50];
  static TRandom r(0);
  sprintf(buf,"houghHisto%5.5f",r.Uniform());
  houghHisto = new TH2D(buf,"",nBinsTheta,minTheta,maxTheta,nBinsR,minR,maxR);
  houghHisto->SetStats(kFALSE);
  houghHisto->GetYaxis()->SetTitle("R");
  houghHisto->GetXaxis()->SetTitle("#theta");
  

  for(int ihit=0;ihit<NumberOfHits;ihit++) {///NumberOfHits;ihit++) {
    for(int iTheta=0;iTheta<nBinsTheta;iTheta++) {
      for(int iR=0;iR<nBinsR;iR++) {
	int weight = binWeight(iTheta,iR,ihit);
	if(weight>-1){
	  houghHisto->SetBinContent(iTheta+1,iR+1,houghHisto->GetBinContent(iTheta+1,iR+1) + 1/*weight*/);
	}
      
      }
    }
  }
}


int TChough2::binWeight(int iTheta, int iR, int ihit) {
  /*
    linearizing the r(theta) in the interval of the bin
    r=r(theta) + dr/dtheta * dTheta 
  */
  double z = HitCoordinates[ihit][1];
  double y = HitCoordinates[ihit][0];

  double theta0 = iTheta*binTheta+minTheta;
  double theta1 = theta0 + binTheta;

  double r0=iR*binR+minR;
  double r1=r0+binR;
  
  double r_at_theta0 = z*cos(theta0)+y*sin(theta0);
  double r_at_theta1 = z*cos(theta1)+y*sin(theta1);
  
  double theta_at_r0=(r0-r_at_theta0)*(theta1-theta0)/(r_at_theta1-r_at_theta0)+theta0;
  double theta_at_r1=(r1-r_at_theta0)*(theta1-theta0)/(r_at_theta1-r_at_theta0)+theta0;

  bool left=false;
  bool right=false;
  bool top=false;
  bool bottom=false;
  
  if(r_at_theta0>r0 && r_at_theta0<r1) left=true;
  if(r_at_theta1>r0 && r_at_theta1<r1) right=true;
  if(theta_at_r0>theta0 && theta_at_r0<theta1) bottom=true;
  if(theta_at_r1>theta0 && theta_at_r1<theta1) top=true;

  double distance=-1;
  if(left&&right) {
    distance = sqrt( pow(r_at_theta0-r_at_theta1,2.) + pow(binTheta,2.));
  }
  
  if(left&&bottom) distance = sqrt( pow(theta_at_r0-theta0,2.) +pow(r_at_theta0-r0,2.) );
  if(left&&top)    distance = sqrt( pow(theta_at_r1-theta0,2.) +pow(r_at_theta0-r1,2.) );
  if(bottom&&top)  distance = sqrt( pow(theta_at_r0 - theta_at_r1,2.)+pow(binR,2.));
  if(bottom&&right)distance = sqrt(pow(theta1-theta_at_r0,2.)+pow(r0-r_at_theta1,2.));
  if(right&&top)   distance = sqrt(pow(r1-r_at_theta1,2.) + pow(theta1-theta_at_r1,2.));
  //  cout<<distance<<" ";
  double maxDistance = sqrt(pow(binTheta,2.)+pow(binR,2.));
  int weight = (int)((distance * 100) / maxDistance);
  return weight;

}

void TChough2::convert(std::vector<TCcluster>& _c){
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
  minY=1.E50;
  maxY=-1.E50;
  minZ=1.E50;
  maxZ=-1.E50;
  
  for(unsigned int i=0;i<_c.size();++i){
	TVector3 hitPrime = S * _c.at(i).posXYZ();
	//xprime isnt needed only doing transform in one plane
        double y = hitPrime.Y();
        double z = hitPrime.Z();
	
        if(y>maxY) maxY=y;
	if(y<minY) minY=y;
	if(z>maxZ) maxZ=z;
	if(z<minZ) minZ=z;
        ypHit.push_back(y);
	zpHit.push_back(z);
  }
  minR=0;
  /*
    Assuming that a track has to pass all detectors, but not nececarily having hits inn all
    maxR is the distance from one corner of a rectangle with sides rangeY and rangeZ to the diagonal
    max/minTheta is calculated using the same asumption
  */
  rangeY=maxY-minY;
  rangeZ=maxZ-minZ;
  
  maxR=rangeY*rangeZ / sqrt(pow(rangeY,2) + pow(rangeZ,2));
  minR=-maxR;
  rangeR=maxR-minR;
  maxTheta=3.141592654/2+atan(rangeY/rangeZ);
  minTheta=3.141592654/2-atan(rangeY/rangeZ);
  rangeTheta=maxTheta-minTheta;
  binTheta=rangeTheta/nBinsTheta;
  binR=rangeR/nBinsR;
  if( fabs(maxY-minY)<1.E-10)minY-=1.E-5;
  if( fabs(maxZ-minZ)<1.E-10)minZ-=1.E-5;

  
}
