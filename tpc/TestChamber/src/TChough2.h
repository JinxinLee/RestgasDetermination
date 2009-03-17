#ifndef TCHOUGH2_H
#define TCHOUGH2_H

#include"TCabsHough.h"
#include"TH2.h"
#include"TF1.h"
#include"TCanvas.h"

class TChough2 : public TCabsHough{
 public:

  virtual ~TChough2();
  void convert(std::vector<TCcluster>& _c);
  virtual void doHough();
  /*
    nMax beeing the number of straight lines sought
  */
  virtual void make(std::vector<TCcluster>& _c, int _nMax=1){
    nMax=_nMax;
    convert(_c);
    doHough();
  }
  /*
    Default values is theta ranging from 0 to pi.Going from 0 - 2pi would alow tracks going both ways, but is not interesting for the beam telescope at elsa. 
  */
  TChough2(const TVector3& _yp,const TVector3& _zp,double _rR=0.1);
  /*
    Given the index of a cluster, the index of the cluster in the vector handed in by the make method
    and the index of the maximum wanted, the method returns a boolean true if the cluster belongs to the line found
  */
  virtual bool hot(int clIndex, int maxIndex);
  virtual bool hot(int clIndex){
    return hot(clIndex,0);
  }

  virtual void draw(bool stop=true,int _x=50,int _y=50,int _w=600,int _h=600);
  

 private:
  int nMax;
  /*
    Vector containing the maximas of the hough image
  */
  std::vector< std::pair<int,int> > maxVector;
  double cutR;

  TCanvas *canvas1;

  TH2D* houghHisto;
  static const int Max_NumberOfHits = 500;
  
  TF1* houghLines[Max_NumberOfHits];

  Int_t NumberOfHits;
  Double_t HitCoordinates[Max_NumberOfHits][2];   //[NumberOfHits] 
  
  int selHits[Max_NumberOfHits];//for drawing
  int nSelHits;
  double rangeR;
  double minR;
  double maxR;

  double rangeTheta;
  double minTheta;
  double maxTheta;

  double minY;
  double maxY;
  double minZ;
  double maxZ;
  int nBinsR;
  int nBinsTheta;

  void makeHoughHisto();
  void makeHoughLines();
  int binWeight(int iR, int iTheta, int iHit);
  void findMaxInHisto(std::vector<std::pair<int,int> > &_maxVector, int _nMax);
  void findHitsOnTrack();

  void cleanup();

};



#endif
