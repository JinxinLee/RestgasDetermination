#ifndef HISTCONTAINERS_H
#define HISTCONTAINERS_H

#include <vector>
#include <string>
class TProfile;
class TH2D;
class TH1D;
class TCtrack;
struct trackHistContainer{
  TH1D *histogramChi2Rrough;
  TH1D *histogramChi2rough;
  TH1D *histogramChi2Prob;
  TH1D *histogramChi2;
  TH1D *histogramRedChi2;
  TH1D *histogramNDF;
  TH1D *histogramAx;
  TH1D *histogramAy;
  TH1D *histogramBx;
  TH1D *histogramBy;
  TH2D *hitmapTPC;

  std::vector<TH1D*> residual;
  std::vector<TH2D*> residualVu2Dhist;
  std::vector<TProfile*> residualVuProfile;
  std::vector<TH2D*> residualVuTrack2Dhist;
  std::vector<TProfile*> residualVuTrackProfile;
  std::vector<TH1D*> xPos;
  std::vector<TH1D*> yPos;
  std::vector<TH1D*> u;
  std::vector<TH1D*> trackErr;
  std::vector<TProfile*> trackErrProfile;
  void fillRes(TCtrack* track, bool unbiased=false);
  trackHistContainer();
  ~trackHistContainer();
  void write(std::string outfile);
};

#endif
