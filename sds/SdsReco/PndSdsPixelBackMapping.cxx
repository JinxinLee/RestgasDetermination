#include "PndSdsPixelBackMapping.h"
#include "FairLogger.h"

PndSdsPixelBackMapping::PndSdsPixelBackMapping()
{
  fGeoH = PndGeoHandling::Instance();
  fVerbose = 0;
}
      
PndSdsPixelBackMapping::PndSdsPixelBackMapping(PndGeoHandling* geo)
{
  fGeoH = geo;
  fVerbose = 0;
}

PndSdsPixelBackMapping::~PndSdsPixelBackMapping()
{
}
            

