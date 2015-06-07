#ifndef PNDCASTATIONSTT_H
#define PNDCASTATIONSTT_H

#include "PndCADef.h"

#include <vector>
using std::vector;

class PndCAHitSTT{
 public:
  char fISta, fISec;
  int fOrigID;
  float fU, fDR;

  friend bool operator<(const PndCAHitSTT& a, const PndCAHitSTT& b) {
    return (a.fISec < b.fISec) || ((a.fISec == b.fISec) && (a.fU < b.fU));
  }
};

class PndCAStationSTTSector{
 public:
  void Init(){
    fNHits = 0;
    fFirstHit = 0;
  }
  int fNHits;
  int fFirstHit;
};

class PndCAStationSTT{
 public:
  void Init(){
    fResolution = float_v(35697.3e-8);
    fHits1D.clear();
    fHits1D.reserve(100);
    for( int i=0; i<fgNSectors; i++ ) fSectors[i].Init();
  }
  static const int fgNSectors = 6;
  float_v fResolution;
  float_v fSin, fCos;
  PndCAStationSTTSector fSectors[fgNSectors];
  vector<PndCAHitSTT> fHits1D;
};

#endif
