#ifndef PNDMVDPIXELBACKMAPPING_H
#define PNDMVDPIXELBACKMAPPING_H

#include "PndMvdDigiPixel.h"
#include "PndMvdHit.h"
#include "PndGeoHandling.h"
#include "PndMvdRecoCharge.h"
#include <vector>

class PndMvdPixelBackMapping
{
  public :  
  PndMvdPixelBackMapping();
  PndMvdPixelBackMapping(std::vector<PndMvdDigiPixel> digiArray, std::vector<Double_t> params);
  ~PndMvdPixelBackMapping();

  void SetVerbose (Int_t level) {fVerbose = level;};
  virtual PndMvdHit GetCluster() = 0;
      
  std::vector<PndMvdDigiPixel> fDigiArray;
  std::vector<Double_t> fParams;
  PndGeoHandling* fGeoH;
  Int_t fVerbose;
  
  protected :
  PndMvdRecoCharge* ChargeReco;
};

#endif

