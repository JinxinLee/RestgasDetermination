#ifndef TCHOUGH2_H
#define TCHOUGH2_H

#include"TCabsHough.h"
#include"TH2.h"
#include"TF1.h"
#include"TCanvas.h"
#include "TCevent.h"
class TChough2 : public TCabsHough{
 public:

  virtual ~TChough2();
  void convert(std::vector<TCcluster>& _c);
  virtual void doHough();
  /*
    nMax beeing the number of straight lines sought
  */
  virtual void make(std::vector<TCcluster>& _c, int _nOnMax=3, double _nSigma=10, bool _weighting=false){
    nSigma=_nSigma;
    weighting=_weighting;
    nOnMax=_nOnMax;
    convert(_c);
    doHough();
  }
  /*
    Requiering tracks to go in z-direction range of theta and R is calculated. The default cut is one bin-width.
  */
  TChough2(const TVector3& _yp,const TVector3& _zp,int _nBinsTheta=50,int _nBinsR=50,double _cutR=0.1, double _cutTheta=0.002,double _minTheta=1.484, double _maxTheta=1.658, double _minR=-10, double _maxR=10);

  virtual bool hot(int clIndex, int maxIndex);//is cluster clIndex on line maxIndex
  virtual bool hot(int clIndex){
    return hot(clIndex,0);
  }
  void setResolution(std::vector<double> _res){
    if(_res.size()==resolution.size()){
      resolution=_res;
    }
  }
  void draw(bool stop=true,int _x=50,int _y=50,int _w=600,int _h=600,TCevent* mcTruth=NULL);//at the moment only possible if draws two first lines
  
  double getNmax(){
    //std::cout<<"getNmax"<<maxVector.size()<<std::endl;
    return maxVector.size();
  }
 private:
  int nOnMax;//n hits for a max
  int nMax;
  double nSigma;
  bool weighting;
  std::vector< std::pair<int,int> > maxVector;// Vector containing the maximas of the hough image
  double cutR; 
  double cutTheta;
  TCanvas *canvas1;
  TH2D* houghHisto;
  static const int Max_NumberOfHits = 500;
  std::vector<double> resolution;
  TF1* houghLines[Max_NumberOfHits];
  TF1* houghLinesUpperBound[Max_NumberOfHits];
  TF1* houghLinesLowerBound[Max_NumberOfHits];
  Int_t NumberOfHits;
  Double_t HitCoordinates[Max_NumberOfHits][2];   //[NumberOfHits] 
  int selHits[Max_NumberOfHits];//for drawing line 1
  int nSelHits;
  int selHits2[Max_NumberOfHits];//for drawing line 2
  int nSelHits2;
  
  
  double minR;
  double maxR;
  double rangeR;

  double minTheta;
  double maxTheta;
  double rangeTheta;

  double minY;
  double maxY;
  double rangeY;

  double minZ;
  double maxZ;
  double rangeZ;

  int nBinsR;
  int nBinsTheta;

  double binTheta;
  double binR;

  void makeHoughHisto();//creates and fills histogram
  void makeHoughLines();//makes the houghlines for drawing
  int binWeight(int iR, int iTheta, int iHit);//weight function, not used due to scaling between R and Theta
  void findMaxInHisto(std::vector<std::pair<int,int> > &_maxVector, int _nMax);
  void findHitsOnTrack();
  double findMaxAroundPoint(TH2D* histo,int& maxBinTheta,int& maxBinR);
  void cleanup();

};



#endif
