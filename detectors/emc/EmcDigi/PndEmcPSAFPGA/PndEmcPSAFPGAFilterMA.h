#ifndef PNDEMCFPGAFILTERMA_HH
#define PNDEMCFPGAFILTERMA_HH

#include <vector>

class PndEmcPSAFPGAFilterMA
{
 public:
  PndEmcPSAFPGAFilterMA();
  ~PndEmcPSAFPGAFilterMA();
  void set(unsigned int newBufferSize);
  void resetToZero();
  float put(float valueToStore);
 private:
  std::vector<float> buffer;
  unsigned int bpointer;
  float sum;
};

#endif
