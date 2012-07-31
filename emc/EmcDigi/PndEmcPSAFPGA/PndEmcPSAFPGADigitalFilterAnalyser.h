#ifndef PNDEMCFPGADIGITALFILTERANALYSER_HH
#define PNDEMCFPGADIGITALFILTERANALYSER_HH

#include "PndEmcPSAFPGASampleAnalyser.h"
#include <string>

class PndEmcPSAFPGAFilterMWD;

#define MAX_NUMBER_OF_HITS 100

class PndEmcPSAFPGADigitalFilterAnalyser : public PndEmcPSAFPGASampleAnalyser {

 public:
  PndEmcPSAFPGADigitalFilterAnalyser();
  PndEmcPSAFPGADigitalFilterAnalyser(const std::string& fname);
  ~PndEmcPSAFPGADigitalFilterAnalyser();


  virtual void setBaselineInterval(int anl_start, int bl_stop);
  virtual float baseline();
  virtual void init(PndEmcPSAFPGASampleAnalyser::SampleAnalyserParams &params);
  virtual void reset();
  virtual void put(float valueToStore);

 private:
  unsigned int analysis_start_position;
  unsigned int baseline_stop_position;
  float baseline_value_calculated;

  PndEmcPSAFPGAFilterMWD *MWD_filter;

  ClassDef(PndEmcPSAFPGADigitalFilterAnalyser,1);
};

#endif
