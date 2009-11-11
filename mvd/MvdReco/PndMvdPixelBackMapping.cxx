#include "PndMvdPixelBackMapping.h"

PndMvdPixelBackMapping::PndMvdPixelBackMapping()
{
  fGeoH = new PndMvdGeoHandling(gGeoManager);
  fVerbose = 0;
}
      
PndMvdPixelBackMapping::PndMvdPixelBackMapping(std::vector<PndMvdDigiPixel> digiArray, std::vector<Double_t> params)
{
  fDigiArray = digiArray;
  fParams    = params;
  fGeoH = new PndMvdGeoHandling(gGeoManager);
  fVerbose = 0;
}

PndMvdPixelBackMapping::~PndMvdPixelBackMapping()
{
  delete(fGeoH);
}
            

