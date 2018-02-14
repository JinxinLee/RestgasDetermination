#include "PndMQSdsPixelBackMapping.h"
#include "FairLogger.h"

PndMQSdsPixelBackMapping::PndMQSdsPixelBackMapping()
{
  fGeoH = PndGeoHandling::Instance();
  fVerbose = 0;
}
      
PndMQSdsPixelBackMapping::PndMQSdsPixelBackMapping(PndGeoHandling* geo)
{
  fGeoH = geo;
  fVerbose = 0;
}

PndMQSdsPixelBackMapping::~PndMQSdsPixelBackMapping()
{
}
            

