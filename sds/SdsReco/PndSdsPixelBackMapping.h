#ifndef PNDSDSPIXELBACKMAPPING_H
#define PNDSDSPIXELBACKMAPPING_H

#include "PndSdsDigiPixel.h"
#include "PndSdsHit.h"
#include "PndSdsGeoHandling.h"
#include <vector>

class PndSdsPixelBackMapping
{
  public :  
  PndSdsPixelBackMapping();
  PndSdsPixelBackMapping(std::vector<PndSdsDigiPixel> digiArray, std::vector<Double_t> params);
  ~PndSdsPixelBackMapping();

  void SetVerbose (Int_t level) {fVerbose = level;};
  virtual PndSdsHit GetCluster() = 0;
      
  std::vector<PndSdsDigiPixel> fDigiArray;
  std::vector<Double_t> fParams;
  PndSdsGeoHandling* fGeoH;
  Int_t fVerbose;
};

#endif

