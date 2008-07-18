#ifndef PNDMVDPIXELBACKMAPPING_H
#define PNDMVDPIXELBACKMAPPING_H

#include "PndMvdDigiPixel.h"
#include "PndMvdHit.h"
#include "PndMvdGeoHandling.h"
#include <vector>

class PndMvdPixelBackMapping
{
  public :  PndMvdPixelBackMapping(){
        fGeoH = new PndMvdGeoHandling(gGeoManager);
        fVerbose = 0;
      };
      PndMvdPixelBackMapping(std::vector<PndMvdDigiPixel> digiArray, std::vector<Double_t> params, TString geoFile){
        fDigiArray = digiArray;
        fParams    = params;
        fGeoH = new PndMvdGeoHandling(geoFile.Data());
        fVerbose = 0;
      }
      ~PndMvdPixelBackMapping(){
        delete(fGeoH);
      }
      
      void SetVerbose (Int_t level) {fVerbose = level;};
      virtual PndMvdHit GetCluster() = 0;
      
  std::vector<PndMvdDigiPixel> fDigiArray;
  std::vector<Double_t> fParams;
  PndMvdGeoHandling* fGeoH;
  Int_t fVerbose;
};

#endif

