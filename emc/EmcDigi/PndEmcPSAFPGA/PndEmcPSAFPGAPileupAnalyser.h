#ifndef PNDEMCFPGAPILEUPANALYSER_HH
#define PNDEMCFPGAPILEUPANALYSER_HH

#include "PndEmcPSAFPGASampleAnalyser.h"
#include <string>

class PndEmcWaveform;
class PndEmcPSAFPGAFilterCF;
class PndEmcPSAFPGAFilterMA;
class PndEmcPSAFPGAFilterMWD;
class PndEmcPSAFPGAFilterDelay;
class PndEmcPSAFPGALinFitter;
class TF1;

#define MAX_NUMBER_OF_HITS 100

class PndEmcPSAFPGAPileupAnalyser : public PndEmcPSAFPGASampleAnalyser {

 public:
  PndEmcPSAFPGAPileupAnalyser();
  virtual ~PndEmcPSAFPGAPileupAnalyser();
  virtual void SetVerbose(Int_t verbose=0) { fVerbose = verbose; }

  enum pileup_t {kSingle=0, kPileup1=1, kPileup2=2, kInvalid=99};

  virtual void initFromFile() {};
  virtual void Init(const std::vector<Double_t> &params, TF1* R_thres, TF1* R_mean, float extBaselineValue=0);
  virtual void Init(const std::vector<Double_t> &params, TF1* R_thres, TF1* R_mean, unsigned int baselineStartSample, unsigned int baselineStopSample);

  virtual void GetHit(Int_t i, Double_t &Energy, Double_t &Time);
  virtual void GetHit(Int_t i, Double_t &Energy, Double_t &Time, pileup_t &PileupType);

  void GetEnergyMeasures(Int_t i, Double_t &Amplitude, Double_t &Integral);
  virtual void reset();
  virtual void put(float valueToStore);

  virtual float baseline() {return baseline_value;};
  

protected:
  enum {kWindow, kExtern, kFollow} baselineMode;

  virtual void setBaseline(float newBaseline);
  virtual void setBaselineWindow(unsigned int startSample, unsigned int stopSample);
  virtual void InitParameters(const std::vector<Double_t> &params);
  virtual void Init2(const std::vector<Double_t> &params, TF1* R_thres, TF1* R_mean);
  Int_t fVerbose;

  unsigned int BaselineStartSample;
  unsigned int BaselineStopSample;
  float BaselineSum;

  PndEmcPSAFPGAFilterMWD* MWD_filter1;
  PndEmcPSAFPGAFilterMWD* MWD_filter2;
  PndEmcPSAFPGAFilterMWD* MWD_filter3;
  PndEmcPSAFPGAFilterDelay* CF_prev;

  bool energy_finished;
  bool timing_finished;
  bool func_defined;

  unsigned int cf_crossing;
  bool in_cfRise;
  float cfZero[MAX_NUMBER_OF_HITS];
  float cfRise[MAX_NUMBER_OF_HITS];
  pileup_t pileups[MAX_NUMBER_OF_HITS];

  TF1* Int_thres;  
  TF1* Int_mean;  



  ClassDef(PndEmcPSAFPGAPileupAnalyser,2);
};





#endif
