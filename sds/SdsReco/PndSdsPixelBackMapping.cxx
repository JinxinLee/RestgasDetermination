#include "PndSdsPixelBackMapping.h"

PndSdsPixelBackMapping::PndSdsPixelBackMapping()
{
  fGeoH = new PndSdsGeoHandling(gGeoManager);
  fVerbose = 0;
}
      
PndSdsPixelBackMapping::PndSdsPixelBackMapping(std::vector<PndSdsDigiPixel> digiArray, std::vector<Double_t> params)
{
  fDigiArray = digiArray;
  fParams    = params;
  fGeoH = new PndSdsGeoHandling(gGeoManager);
  fVerbose = 0;
}

PndSdsPixelBackMapping::~PndSdsPixelBackMapping()
{
  delete(fGeoH);
}
            

