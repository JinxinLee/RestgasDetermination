#ifndef PndMQSdsPixelBackMapping_H
#define PndMQSdsPixelBackMapping_H

#include "PndSdsDigiPixel.h"
#include "PndSdsHit.h"
#include "PndGeoHandling.h"
#include <vector>

class PndMQSdsPixelBackMapping
{
  public :  
  PndMQSdsPixelBackMapping();
  PndMQSdsPixelBackMapping(PndGeoHandling* geo);
  virtual ~PndMQSdsPixelBackMapping();

  void SetVerbose (Int_t level) {fVerbose = level;};
  virtual PndSdsHit GetCluster(std::vector<PndSdsDigiPixel*>& pixelArray) = 0;

  virtual void SetGeoHandling (PndGeoHandling* geo){fGeoH = geo;}
      
  PndGeoHandling* fGeoH;
  Int_t fVerbose;

};

#endif

