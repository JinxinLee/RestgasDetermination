#include "PndSdsPixelBackMapping.h"

PndSdsPixelBackMapping::PndSdsPixelBackMapping()
{
  Fatal("PndSdsPixelBackMapping", "We don't want to crate that here, right?");
  if(0==fGeoH) fGeoH = PndGeoHandling::Instance();
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
            

