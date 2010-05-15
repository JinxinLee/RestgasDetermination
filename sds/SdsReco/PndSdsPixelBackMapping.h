#ifndef PNDSDSPIXELBACKMAPPING_H
#define PNDSDSPIXELBACKMAPPING_H

#include "PndSdsDigiPixel.h"
#include "PndSdsHit.h"
#include "PndGeoHandling.h"
#include <vector>

class PndSdsPixelBackMapping
{
  public :  
  PndSdsPixelBackMapping();
  PndSdsPixelBackMapping(PndGeoHandling* geo);
  ~PndSdsPixelBackMapping();

  void SetVerbose (Int_t level) {fVerbose = level;};
  virtual PndSdsHit GetCluster(std::vector<PndSdsDigiPixel> pixelArray) = 0;
      
  std::vector<PndSdsDigiPixel> fDigiArray;
  PndGeoHandling* fGeoH;
  Int_t fVerbose;

};

#endif

