// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
// 
// created 2007
//-----------------------------------------------------
#include "PndDrcOptReflGray.h"

#include "PndDrcSurfAbs.h"
#include "PndDrcPhoton.h"

//----------------------------------------------------------------------
PndDrcOptReflGray* PndDrcOptReflGray::Clone() const
{
  return new PndDrcOptReflGray(*this);
}
//----------------------------------------------------------------------
const Drc::Reflectivity PndDrcOptReflGray::Query(const PndDrcPhoton&    ph,
						 const XYZVector        normal,
						 const double           n_next,
						 const Drc::ReflDir     direction) const
{
  double lambda = ph.Wavelength();


  double ran = gRandom->Uniform(0.0,1.0);

  if (ran>fReflProb || lambda<400)
    {
      return Drc::ReflTransmitted;
    }
  else
    {
      return Drc::ReflReflected;
    }

}
