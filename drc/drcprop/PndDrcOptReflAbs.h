/*! 
  \brief Class for reflectivity of surfaces. 

  Defines the reflectivity of surfaces, mirrors etc.
  A mirror can have ReflTransmitted, ReflAbsorbed, or ReflReflected.
  A eg. quartz bar can have ReflTransmitted (lost) or ReflRefracted, that is
  the refractive index determines further fate. 
*/
#include "TRandom.h"

#ifndef PNDDRCOPTREFLABS_H
#define PNDDRCOPTREFLABS_H

namespace Drc
{
/*! \enum PndDrc::Reflectivity An enum holding the status of a photon at a surface.*/
 enum Reflectivity
    {
      ReflTransmitted,         //!< Photon is transmitted (internal refl, dichr. mirr.)
      ReflAbsorbed,            //!< Photon is absorbed (mirror).
      ReflReflected,           //!< Photon is reflected (mirror).
      ReflRefracted,           //!< Photon will be refracted (int. reflection or not)
    };
}


class PndDrcPhoton;

#include "Math/Vector3D.h"
using ROOT::Math::XYZVector;
//class XYZVector;

class PndDrcOptReflAbs
{
 protected:
  
  //!                                             Random generator.  
  mutable TRandom fRan;

 public:
 
  //! Destructor.
  virtual ~PndDrcOptReflAbs(){};           

  /*! \brief Virtual copy constructor.
    \return Pointer to new allocated memory.
  */
  virtual PndDrcOptReflAbs* Clone() const = 0;

  /*! \brief Reflectivity of surface 

  used for mirror surfaces or surfaces with internal reflection.
  \param ph The photon
  \param normal The normal vector of the surface.
  */
  virtual const Drc::Reflectivity Reflectivity(PndDrcPhoton& ph,
					       XYZVector  normal) const = 0;
};
#endif
