#ifndef PNDEMCFPGASAMPLEANALYSER_HH
#define PNDEMCFPGASAMPLEANALYSER_HH

#include <string>

class PndEmcPSAFPGAFilterCF;
class PndEmcPSAFPGAFilterMA;
class PndEmcPSAFPGAFilterDelay;
class PndEmcPSAFPGALinFitter;

#include "PndEmcAbsPSA.h"

#define MAX_NUMBER_OF_HITS 100

class PndEmcPSAFPGASampleAnalyser : public PndEmcAbsPSA{

 public:
  PndEmcPSAFPGASampleAnalyser();
  PndEmcPSAFPGASampleAnalyser(const std::string &fname);
  virtual ~PndEmcPSAFPGASampleAnalyser();

  virtual void initFromFile(const std::string &fname);
  virtual void reset() = 0;
  virtual void put(float valueToStore) = 0;
  virtual void setBaseline(float newBaseline){baseline_value = newBaseline;};

  virtual float baseline() = 0;
  int nHits(){return Number_of_hits;};
  float sampleTime(int index);
  float sampleAmplitude(int index);
  float sampleIntegral(int index);
		
  virtual Int_t Process(const PndEmcWaveform *waveform);
  virtual void Reset(){this->reset();};
  virtual void GetHit(Int_t i,Double_t &energy, Double_t &time);
  virtual void Init(const std::vector<Double_t> &params);

  struct SampleAnalyserParams {
	  int ma_trig_M;
	  float hit_threshold;
	  int cf_delay;
	  float cf_ratio;
	  int cf_fitter_length;
	  int cf_fit_offset;
	  int mwd_length;
	  float mwd_tau;
		float clock_unit;//unit time per point
		float iafactor;//integral vs ampltitue
  } ;

 protected:
  virtual void init(SampleAnalyserParams &params);

  SampleAnalyserParams analyserParams;

  float baseline_value;
  unsigned int local_time;
  bool pulse_detected;
  int rough_pulse_timing;
  int cf_zero_crossing_points;

  PndEmcPSAFPGAFilterCF     *CF_filter;
  PndEmcPSAFPGAFilterMA     *MA_triggering;
  PndEmcPSAFPGAFilterDelay  *MA_delay;
  PndEmcPSAFPGAFilterDelay  *CF_delay;
  PndEmcPSAFPGAFilterDelay  *Signal_delay;
  PndEmcPSAFPGALinFitter    *CF_Fitter;

  // features of the detected pulses
  int Number_of_hits;
  float time[MAX_NUMBER_OF_HITS];
  float integral[MAX_NUMBER_OF_HITS];
  float amplitude[MAX_NUMBER_OF_HITS];
 
  ClassDef(PndEmcPSAFPGASampleAnalyser,1);
};

#endif
