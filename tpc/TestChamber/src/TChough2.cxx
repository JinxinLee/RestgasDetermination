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
using std::pair;
using std::cout;
using std::endl;
TChough2::TChough2(const TVector3& _yp,const TVector3& _zp,double _rR) : TCabsHough(_yp,_zp){
  cutR=_rR;
  nBinsR = 50;
  nBinsTheta =50;

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
  
  for(unsigned int i=0;i<ypHit.size();++i){
    z[i]=zpHit.at(i);
    y[i]=ypHit.at(i);
  }
  for(unsigned int i=0;i<maxVector.size();++i){
    thetaMax_[i]=maxVector.at(i).first;
    rMax_[i]=maxVector.at(i).second;
    thetaMaxE_[i]=0;
    rMaxE_[i]=cutR;
    
  }
  static TRandom r(0);
  char buf[10];
  sprintf(buf,"c%5.5f",r.Uniform());
  TCanvas *c = new TCanvas(buf,"Hough Transformation",_z,_y,_w,_h);
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
  for(unsigned int i=0;i<ypHit.size();++i){
   if(hot(i)){
      selHits.push_back(i);
      houghLines[i]->SetLineStyle(1);
      houghLines[i]->SetLineWidth(1);
      houghLines[i]->SetLineColor(kGreen);
    }
  }
  cout<<"checking hotnes"<<endl;

  double zsel[selHits.size()];
  double ysel[selHits.size()];
  for(unsigned int i=0;i<selHits.size();++i){
    zsel[i]=zpHit.at(selHits.at(i));
    ysel[i]=ypHit.at(selHits.at(i));
  }
  TGraph* gsel = new TGraph(selHits.size(),zsel,ysel);
  gsel->SetMarkerStyle(20);
  gsel->SetMarkerColor(kGreen);
  gsel->Draw("P");


  c->cd(2);
  houghHisto->Draw("colz");


  maxPoints->Draw("P");
  for(int i=0;i<NumberOfHits;++i){
    houghLines[i]->Draw("same");
  }

  if(stop){
    
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
    cout<<"hitY "<<i<<" "<<ypHit.at(i)-minY<<", ";
    HitCoordinates[i][1] = zpHit.at(i)-minZ;
    cout<<"hitZ "<<i<<" "<<zpHit.at(i)-minZ<<endl;
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
  
  double thetaAtMax=maxBinTheta*rangeTheta/nBinsTheta+minTheta;
  double rAtMax=maxBinR*rangeR/nBinsTheta;
  double rPoint=z*cos(thetaAtMax) +y*sin(thetaAtMax);
  discr=-fabs(rPoint-rAtMax);
  cout<<"discr "<<-discr<<endl;
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
	houghLines[i] = new TF1(bufName,buf,minTheta,maxTheta);
	//	  houghLines[i]->SetLineColor(kBlue);
	houghLines[i]->SetLineStyle(2);
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
      double z = HitCoordinates[ihit][1];
      double y = HitCoordinates[ihit][0];
      double stepTheta=rangeTheta/nBinsTheta;
      double stepR=rangeR/nBinsR;
      double rTmp=z*cos(iTheta*stepTheta-minTheta)+y*sin(iTheta*stepTheta-minTheta);
      int iR=(int)rTmp/stepR;
      houghHisto->SetBinContent(iTheta,iR,houghHisto->GetBinContent(iTheta,iR) + 1);

      
    }
  }
}


int TChough2::binWeight(int iTheta, int iR, int ihit) {
  return 1;
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
  double rangeY=maxY-minY;
  double rangeZ=maxZ-minZ;
  cout<<"rangeY "<<rangeY<<endl;
  cout<<"rangeZ "<<rangeZ<<endl;
  maxR=rangeY*rangeZ / sqrt(pow(rangeY,2) + pow(rangeZ,2));
  maxTheta=atan(rangeY/rangeZ)+3.141592654/2;
  minTheta=-maxTheta;
  if( fabs(maxY-minY)<1.E-10)minY-=1.E-5;
  if( fabs(maxZ-minZ)<1.E-10)minZ-=1.E-5;

  
}
