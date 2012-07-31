#ifndef PNDEMCFPGAFILTERCF_HH
#define PNDEMCFPGAFILTERCF_HH

#include <vector>

class PndEmcPSAFPGAFilterCF{
 public:
  PndEmcPSAFPGAFilterCF();
  ~PndEmcPSAFPGAFilterCF();
  void set(unsigned int newBufferSize, float newRatio);
  void resetToZero();
  float put(float valueToStore);
 private:
  std::vector<float> buffer;
  unsigned int bpointer;
  float ratio;
};

#endif
