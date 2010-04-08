#include "PndMvdPixelBackMapping.h"

PndMvdPixelBackMapping::PndMvdPixelBackMapping()
{
  fGeoH = new PndGeoHandling(gGeoManager);
  fVerbose = 0;
}
      
PndMvdPixelBackMapping::PndMvdPixelBackMapping(std::vector<PndMvdDigiPixel> digiArray, std::vector<Double_t> params)
{
  fDigiArray = digiArray;
  fParams    = params;
  fGeoH = new PndGeoHandling(gGeoManager);
  fVerbose = 0;
  if (fParams.size() > 4) ChargeReco = new PndMvdRecoCharge(fParams[4],fParams[5],fParams[6]);
  else ChargeReco = new PndMvdRecoCharge();
}

PndMvdPixelBackMapping::~PndMvdPixelBackMapping()
{
  delete(fGeoH);
  delete(ChargeReco);
}
            

