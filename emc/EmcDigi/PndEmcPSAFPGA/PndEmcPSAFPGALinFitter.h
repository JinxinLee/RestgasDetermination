#ifndef PNDEMCFPGALINFITTER_HH
#define PNDEMCFPGALINFITTER_HH

class PndEmcPSAFPGALinFitter{
 public:
  PndEmcPSAFPGALinFitter();
  ~PndEmcPSAFPGALinFitter();
  void reset();
  void putPoint(double ix, double iy);
  void fit();
  double offset();
  double slope();
  double average();
  double averageX();
 private:
  double x;
  double xx;
  double xy;
  double y;
  double a,k;
  unsigned int Np;
};

#endif
