#ifndef TCHOUGH1_H
#define TCHOUGH1_H

#include"TCabsHough.h"
#include"TH2.h"
#include"TF1.h"
#include"TCanvas.h"

class TChough1 : public TCabsHough{
 public:

  virtual ~TChough1();

  virtual void doHough();

  TChough1(const TVector3& _yp,const TVector3& _zp);

  virtual bool hot(int index);

  virtual void draw(bool stop=true,int _x=50,int _y=50,int _w=600,int _h=600);


 private:
  double aymax,bymax;


  TCanvas *canvas1;

  TH2D* houghHistoYZ;
  static const int Max_NumberOfHits = 500;

  TF1* houghLinesYZ[Max_NumberOfHits];


  Int_t           NumberOfHits;
  Double_t        HitCoordinates[Max_NumberOfHits][2];   //[NumberOfHits]


  int selHits[Max_NumberOfHits];
  int nSelHits;

  double r1Y;
  double r2Y;

  double minAY;
  double maxAY;
  double minBY;
  double maxBY;
  
  int nBinsAY;
  int nBinsBY;




  void makeHoughHisto();
  void makeHoughLines();
  int binWeight(int ia, int ib, int ihit);
  void findMaxInHisto(double& amax,double& bmax);
  void findHitsOnTrack();

  void cleanup();

};



#endif
