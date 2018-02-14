#ifndef PNDEMCFPGAFILTERCF_HH
#define PNDEMCFPGAFILTERCF_HH

#include "PndEmcPSAFPGAFilterLine.h"
#include <vector>

class PndEmcPSAFPGAFilterCF : public PndEmcPSAFPGAFilterLine {

 public:
  PndEmcPSAFPGAFilterCF();
  virtual ~PndEmcPSAFPGAFilterCF();
  void set(unsigned int newBufferSize, float newRatio);
  void resetToZero();
  virtual float put(float valueToStore);
 private:
  std::vector<float> buffer;
  unsigned int bpointer;
  float ratio;
};

#endif
