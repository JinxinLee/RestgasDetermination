#ifndef HELPERS_H
#define HELPERSH_

#include <list>
#include <vector>
#include <string>
#include <TProfile.h>
#include <TGraph.h>
//static would imply functions only valid for this file
class CsGEMCluster;
class TCcluster;
class TCevent;
class TCtrack;
class TGraph;
class TGraph2D;
class TH1D;
class TH2D;
class TH1I;

class HistContainer{
 public:
  HistContainer();
  ~HistContainer();
  void fillRes(TCtrack* track,std::string alignmentFile);
  void write(std::string filename="clusterMultipCut.root");
  void ampFiller(int detID, double amp, double noise,double ratioA1A3=0, double ratioA2A3=0);
  
  TH2D *hitmapGM1;
  TH2D *hitmapGM2;
  TH2D *hitmapSI1;
  TH2D *hitmapSI2;

  TH1I *histogramSI1X;
  TH1I *histogramSI1Y;
  TH1I *histogramSI2X;
  TH1I *histogramSI2Y;
  TH1I *histogramGM1X;
  TH1I *histogramGM1Y;
  TH1I *histogramGM2X;
  TH1I *histogramGM2Y;

  TH1I *histogramNhitsSI1X;
  TH1I *histogramNhitsSI1Y;
  TH1I *histogramNhitsSI2X;
  TH1I *histogramNhitsSI2Y;
  TH1I *histogramNhitsGM1X;
  TH1I *histogramNhitsGM1Y;
  TH1I *histogramNhitsGM2X;
  TH1I *histogramNhitsGM2Y;

  TH1I *histogramNclustersSI1X;
  TH1I *histogramNclustersSI1Y;
  TH1I *histogramNclustersSI2X;
  TH1I *histogramNclustersSI2Y;
  TH1I *histogramNclustersGM1X;
  TH1I *histogramNclustersGM1Y;
  TH1I *histogramNclustersGM2X;
  TH1I *histogramNclustersGM2Y;

  TH2D *histogramSI1XampRatio;
  TH2D *histogramSI1YampRatio;
  TH2D *histogramSI2XampRatio;
  TH2D *histogramSI2YampRatio;
  TH2D *histogramGM1XampRatio;
  TH2D *histogramGM1YampRatio;
  TH2D *histogramGM2XampRatio;
  TH2D *histogramGM2YampRatio;

  TH1D *histogramSI1XclNoise;
  TH1D *histogramSI1YclNoise;
  TH1D *histogramSI2XclNoise;
  TH1D *histogramSI2YclNoise;
  TH1D *histogramGM1XclNoise;
  TH1D *histogramGM1YclNoise;
  TH1D *histogramGM2XclNoise;
  TH1D *histogramGM2YclNoise;

  TH1I *histogramEvent;
  TH1I *histogramStart;
  TH1I *histogramEnd;
  TH1I *histogramTrack;
  TH1D *histogramChi2rough;
  TH1D *histogramChi2;
  TH1D *histogramNDF;

  TH1D *histogramSI1Xhitpoint;
  TH1D *histogramSI1Yhitpoint;
  TH1D *histogramSI2Xhitpoint;
  TH1D *histogramSI2Yhitpoint;
  TH1D *histogramGM1Xhitpoint;
  TH1D *histogramGM1Yhitpoint;
  TH1D *histogramGM2Xhitpoint;
  TH1D *histogramGM2Yhitpoint;

  TH1D *histogramSI1XAmp;
  TH1D *histogramSI1YAmp;
  TH1D *histogramSI2XAmp;
  TH1D *histogramSI2YAmp;
  TH1D *histogramGM1XAmp;
  TH1D *histogramGM1YAmp;
  TH1D *histogramGM2XAmp;
  TH1D *histogramGM2YAmp;
  
  TH1D *histogramSI1XU;
  TH1D *histogramSI1YU;
  TH1D *histogramSI2XU;
  TH1D *histogramSI2YU;
  TH1D *histogramGM1XU;
  TH1D *histogramGM1YU;
  TH1D *histogramGM2XU;
  TH1D *histogramGM2YU;

  TH1D *histogramSI1Xerr;
  TH1D *histogramSI1Yerr;
  TH1D *histogramSI2Xerr;
  TH1D *histogramSI2Yerr;
  TH1D *histogramGM1Xerr;
  TH1D *histogramGM1Yerr;
  TH1D *histogramGM2Xerr;
  TH1D *histogramGM2Yerr;

  TH1D *histogramSI1XhitpointBbest;
  TH1D *histogramSI1YhitpointBbest;
  TH1D *histogramSI2XhitpointBbest;
  TH1D *histogramSI2YhitpointBbest;
  TH1D *histogramGM1XhitpointBbest;
  TH1D *histogramGM1YhitpointBbest;
  TH1D *histogramGM2XhitpointBbest;
  TH1D *histogramGM2YhitpointBbest;

  TH1D *histogramSI1XUbBest;
  TH1D *histogramSI1YUbBest;
  TH1D *histogramSI2XUbBest;
  TH1D *histogramSI2YUbBest;
  TH1D *histogramGM1XUbBest;
  TH1D *histogramGM1YUbBest;
  TH1D *histogramGM2XUbBest;
  TH1D *histogramGM2YUbBest;

  TH1D *histogramSI1XunBiResi;
  TH1D *histogramSI1YunBiResi;
  TH1D *histogramSI2XunBiResi;
  TH1D *histogramSI2YunBiResi;
  TH1D *histogramGM1XunBiResi;
  TH1D *histogramGM1YunBiResi;
  TH1D *histogramGM2XunBiResi;
  TH1D *histogramGM2YunBiResi;

  TH2D *histogramSI1XunBiResiVu2d;
  TH2D *histogramSI1YunBiResiVu2d;
  TH2D *histogramSI2XunBiResiVu2d;
  TH2D *histogramSI2YunBiResiVu2d;
  TH2D *histogramGM1XunBiResiVu2d;
  TH2D *histogramGM1YunBiResiVu2d;
  TH2D *histogramGM2XunBiResiVu2d;
  TH2D *histogramGM2YunBiResiVu2d;

  TH1D *histogramSI1Xresi;
  TH1D *histogramSI1Yresi;
  TH1D *histogramSI2Xresi;
  TH1D *histogramSI2Yresi;
  TH1D *histogramGM1Xresi;
  TH1D *histogramGM1Yresi;
  TH1D *histogramGM2Xresi;
  TH1D *histogramGM2Yresi;
  
  TProfile *histogramSI1XresiVu;
  TProfile *histogramSI1YresiVu;
  TProfile *histogramSI2XresiVu;
  TProfile *histogramSI2YresiVu;
  TProfile *histogramGM1XresiVu;
  TProfile *histogramGM1YresiVu;
  TProfile *histogramGM2XresiVu;
  TProfile *histogramGM2YresiVu;

  TH2D *histogramSI1XresiVu2d;
  TH2D *histogramSI1YresiVu2d;
  TH2D *histogramSI2XresiVu2d;
  TH2D *histogramSI2YresiVu2d;
  TH2D *histogramGM1XresiVu2d;
  TH2D *histogramGM1YresiVu2d;
  TH2D *histogramGM2XresiVu2d;
  TH2D *histogramGM2YresiVu2d;

  TH1D *histogramSI1XtrackErr;
  TH1D *histogramSI1YtrackErr;
  TH1D *histogramSI2XtrackErr;
  TH1D *histogramSI2YtrackErr;
  TH1D *histogramGM1XtrackErr;
  TH1D *histogramGM1YtrackErr;
  TH1D *histogramGM2XtrackErr;
  TH1D *histogramGM2YtrackErr;

  TH2D *histogramSI1XtrackErrVu2d;
  TH2D *histogramSI1YtrackErrVu2d;
  TH2D *histogramSI2XtrackErrVu2d;
  TH2D *histogramSI2YtrackErrVu2d;
  TH2D *histogramGM1XtrackErrVu2d;
  TH2D *histogramGM1YtrackErrVu2d;
  TH2D *histogramGM2XtrackErrVu2d;
  TH2D *histogramGM2YtrackErrVu2d;

  TH1D *histogramGM1XYratio;
  TH1D *histogramGM2XYratio;
  TH1D *histogramGM1YXratio;
  TH1D *histogramGM2YXratio;

  TH1D *histogramGM1XYScaledRatio;
  TH1D *histogramGM2XYScaledRatio;
  TH1D *histogramGM1YXScaledRatio;
  TH1D *histogramGM2YXScaledRatio;
  TH1D *histogramGM1SmallBigScaledRatio;
  TH1D *histogramGM2SmallBigScaledRatio;

  TH2D *histogramTrackErrMatrix;

};
//class TProfile;
/*
  Does a cut on the difference a2-a0, a very crude cut, but nescesary for the silicons to get the multiplicity down.
  Cuts away all clusters with a2-a0 <= cut
*/

void ampDiffCut(const std::list<CsGEMCluster*> &clusterList,std::vector<TCcluster> &tcClusters, double cut, int detID, std::string alignmentFile,int& counter, HistContainer* hcont,double umin=-1, double umax=-1);
/*
  Does a cut on the ratio a1/a2 and a0/a2, a cut on the bananaplot
  Cuts away clusters with a1/a2>cut1 and a0/a2>cut0
*/
void ampRatioCut(const std::list<CsGEMCluster*> &clusterList,
		 std::vector<TCcluster> &tcClusters, 
		 double cutA1A3, double cutA2A3, 
		 int detID,std::string alignmentFile, 
		 int& counter, HistContainer* hcont, 
		 double umin =-1, double umax=-1);
void ampRatioNoiseCut(const std::list<CsGEMCluster*> &clusterList,
		      std::vector<TCcluster> &tcClusters, 
		      double cutA1A3, double cutA2A3,double cut, 
		      int detID,std::string alignmentFile, 
		      int& counter, HistContainer* hcont, 
		      double umin =-1, double umax=-1);
void clusterFiller(TCcluster cl, std::vector<TCcluster>& clusters, 
		   TH1D* hitPointHist,TH1D* uHist, 
		   TH1D* errHist, TGraph2D* graph2d, 
		   TGraph* graph, 
		   int& counter, bool x);
void dispDraw(TCtrack* track1, TCtrack* track2, 
	      TGraph* clFit_x1,TGraph* clFit_y1, 
	      TGraph* clFit_x2, TGraph* clFit_y2 ,  
	      TGraph* x_event,TGraph* y_event);


#endif
