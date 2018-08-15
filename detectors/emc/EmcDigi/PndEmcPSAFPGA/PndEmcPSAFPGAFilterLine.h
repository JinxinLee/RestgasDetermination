#ifndef PNDEMCFPGAFILTERLINE_HH
#define PNDEMCFPGAFILTERLINE_HH

class PndEmcPSAFPGAFilterLine{
 public:
  PndEmcPSAFPGAFilterLine(){};
  virtual ~PndEmcPSAFPGAFilterLine(){};
  virtual float put(float valueToAdd) = 0;
};

#endif
