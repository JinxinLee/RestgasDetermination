#include "PndSdsPixelBackMapping.h"

PndSdsPixelBackMapping::PndSdsPixelBackMapping()
{
  fGeoH = new PndGeoHandling(gGeoManager);
  fVerbose = 0;
}
      
PndSdsPixelBackMapping::PndSdsPixelBackMapping(std::vector<PndSdsDigiPixel> digiArray, std::vector<Double_t> params)
{
  fDigiArray = digiArray;
  fParams    = params;
  fGeoH = new PndGeoHandling(gGeoManager);
  fVerbose = 0;
}

PndSdsPixelBackMapping::~PndSdsPixelBackMapping()
{
  delete(fGeoH);
}
            

