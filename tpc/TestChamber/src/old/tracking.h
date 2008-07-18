#ifndef TRACKING_H
#define TRACKING_H

#include <TROOT.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TH3.h>
#include <TH2.h>
#include <TF1.h>

#include <vector>
#include <iostream>
#include <string>

#include "fit.h"

#include "PndTpcCluster.h"
#include "AnalysisEvent.h"

class tracking {
 private:


  bool stopROOT;
  int counter;

  TDirectory dir;
  std::string name;
  TCanvas *canvas1;
  TCanvas *canvas2;

  static const int Max_NumberOfHits = 500;

  std::vector<TGraph*> xzHitsVec;
  std::vector<TGraph*> yzHitsVec;
  TGraphErrors* xzHits;
  TGraphErrors* yzHits;
  TGraphErrors* xzErrHits;
  TGraphErrors* yzErrHits;
  TGraph* xzSelHits;
  TGraph* yzSelHits;
  TH2D* xzHits_H;
  TH2D* yzHits_H;

  TF1* xzFit;
  TF1* yzFit;

  TH3D* hits3D;

  TH2D* houghHistoXZ;
  TH2D* houghHistoYZ;

  TF1* houghLinesXZ[Max_NumberOfHits];
  TF1* houghLinesYZ[Max_NumberOfHits];


  TH1D *biasedResidX;
  TH1D *biasedResidY;
  TH1D *biasedResidZ;
  
  TH1D *chi2reduced;


  int selHits[Max_NumberOfHits];
  int nSelHits;

  void makeHoughHisto(int xyswitch, int nevt);
  void makeHoughLines(int xyswitch);
  void makeHitGraph(int xyswitch);
  void makeSelHitGraph(int xyswitch);

  int fitAll(double ax_s, double bx_s, double ay_s, double by_s,
			 fitReturn_t &fitReturn);

  int binWeight(int ia, int ib, int ihit , int xyswitch);

  void findMaxInHisto(int xyswitch,double& amax,double& bmax);
  void findHitsOnTrack(int xyswitch,double a0,double b);

  void cleanup();

  unsigned int mounting;


  //these are filed in ctor
  double maxX;
  double maxY;
  double maxZ;
  
  double minAX;
  double maxAX;
  double minBX;
  double maxBX;
  double minAY;
  double maxAY;
  double minBY;
  double maxBY;
  
  int nBinsAX;
  int nBinsBX;
  int nBinsAY;
  int nBinsBY;
  
  double r1X;
  double r2X;

  double r1Y; 
  double r2Y; 
  int visflag;
  int minNhitsOnTrack;

  Int_t           NumberOfHits;
  Double_t        HitCoordinates[Max_NumberOfHits][3];   //[NumberOfHits]
  Double_t        HitCoordinatesError[Max_NumberOfHits][3];   //[NumberOfHits]
  Double_t        HitCharge[Max_NumberOfHits];   //[NumberOfHits]
  Int_t        clusterSize[Max_NumberOfHits];   //[NumberOfHits]
  Int_t        nPad[Max_NumberOfHits];   //[NumberOfHits]
  Int_t        nPadX[Max_NumberOfHits];   //[NumberOfHits]
  Int_t        nPadY[Max_NumberOfHits];   //[NumberOfHits]
  
 public:
  
  tracking(int _visflag, double _r1, double _r2, int _nHOT, bool _stopROOT,std::string _name, unsigned int _mounting);
  virtual ~tracking();

  bool processEvent(std::vector<PndTpcCluster*>*,int, AnalysisEvent *analysisEvt);

};

#endif

 
 
