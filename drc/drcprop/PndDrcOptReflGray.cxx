// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
// 
// created 2007
//-----------------------------------------------------
#include "PndDrcOptReflGray.h"

#include "PndDrcPhoton.h"

//----------------------------------------------------------------------
PndDrcOptReflGray* PndDrcOptReflGray::Clone() const
{
  return new PndDrcOptReflGray(*this);
}
//----------------------------------------------------------------------
const Drc::Reflectivity PndDrcOptReflGray::Reflectivity(PndDrcPhoton&  ph,
						       XYZVector   normal) 
const
{
  double lambda = ph.Wavelength();


  double ran = fRan.Uniform(0.0,1.0);

  if (ran>fReflProb || lambda<400)
    {
      return Drc::ReflTransmitted;
    }
  else
    {
      return Drc::ReflReflected;
    }

}
