#ifndef PNDEMCFPGAMLINFITTER_HH
#define PNDEMCFPGAMLINFITTER_HH

#include "PndEmcPSAFPGAFilterMA.h"

class PndEmcPSAFPGAMLinFitter{
 public:
  PndEmcPSAFPGAMLinFitter() {};
  ~PndEmcPSAFPGAMLinFitter() {};
  void set(int newBufferSize);
  void putPoint(double ix,double iy);
  void fit();
  double offset();
  double slope();
  double average();
 private:
  PndEmcPSAFPGAFilterMA mx;
  PndEmcPSAFPGAFilterMA mxx;
  PndEmcPSAFPGAFilterMA mxy;
  PndEmcPSAFPGAFilterMA my;
  double x;
  double xx;
  double xy;
  double y;
  double a,k;
  int Np;
};

#endif
