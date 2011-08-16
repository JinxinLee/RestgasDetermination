// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
// 
// created 2011
//-----------------------------------------------------
#include "PndDrcOptReflGeffcken.h"


//----------------------------------------------------------------------
PndDrcOptReflGeffcken* PndDrcOptReflGeffcken::Clone() const
{
  return new PndDrcOptReflGeffcken(*this);
}
//----------------------------------------------------------------------
const Drc::Reflectivity PndDrcOptReflGeffcken::Query(const PndDrcPhoton&    ph,
						     const XYZVector        normal,
						     const double           n_next,
						     const Drc::ReflDir     direction) const
{
      return Drc::ReflAbsorbed;
}
