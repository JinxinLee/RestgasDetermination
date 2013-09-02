#ifndef PNDEMCFPGAFILTERMWD_HH
#define PNDEMCFPGAFILTERMWD_HH

#include "PndEmcPSAFPGAFilterLine.h"
#include <vector>

class PndEmcPSAFPGAFilterMWD: public PndEmcPSAFPGAFilterLine {

 public:
  PndEmcPSAFPGAFilterMWD();
  virtual ~PndEmcPSAFPGAFilterMWD();
	void resetToZero() ;
  void set(float tau, unsigned int newBufferSize);
  virtual float put(float valueToStore);

 private:
  std::vector<float> buffer;
  unsigned int bpointer;
  float sum;
  float lambda;
};

#endif
