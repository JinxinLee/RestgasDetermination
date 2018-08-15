#ifndef PNDEMCFPGAFILTERDELAY_HH
#define PNDEMCFPGAFILTERDELAY_HH

#include <vector>

class PndEmcPSAFPGAFilterDelay {
 public:
  PndEmcPSAFPGAFilterDelay();
  ~PndEmcPSAFPGAFilterDelay();
  void set(unsigned int newBufferSize);
  float put(float valueToStore);
  void resetToZero();
 private:
  std::vector<float> buffer;
  unsigned int bpointer;

};

#endif
