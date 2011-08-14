#include "TRandom3.h"

#ifndef PNDDRCOPTREFLABS_H
#define PNDDRCOPTREFLABS_H

namespace Drc
{
/*! \enum Drc::Reflectivity 
  An enum holding the status of a photon at a surface.*/
 enum Reflectivity
    {
      ReflTransmitted,         //!< Photon is transmitted (dichr. mirr., coatings)
      ReflAbsorbed,            //!< Photon is absorbed (mirror).
      ReflReflected,           //!< Photon is reflected (mirror).
      ReflRefracted,           //!< Photon will be refracted and is lost if not Fresnel reflected.
    };
}


namespace Drc
{
/*! \enum Drc::ReflDir 
  An enum holding the direction of a photon at a surface entering or leaving a volume.
  This is important for coatings and dichroic mirrors, where the layer sequence plays a role.*/
 enum ReflDir
    {
      ReflIn,         //!< Photon is entering volume.
      ReflOut         //!< Photon is leaving volume.
    };
}


class PndDrcPhoton;

#include "Math/Vector3D.h"
using ROOT::Math::XYZVector;
//class XYZVector;

/*!
  \brief Class for reflectivity of surfaces.

  Defines the reflectivity of surfaces, mirrors etc.
  A mirror can have 

  \sa Drc::Reflectivity
*/

class PndDrcOptReflAbs
{
 protected:

  //!                                             Random generator.
  mutable TRandom3 fRan;

 public:

  //! Destructor.
  virtual ~PndDrcOptReflAbs(){};

  /*! \brief Virtual copy constructor.
    \return Pointer to new allocated memory.
  */
  virtual PndDrcOptReflAbs* Clone() const = 0;

  /*! \brief Query reflectivity of surface 

  used for mirror surfaces or surfaces with internal reflection.
  \param ph The photon
  \param normal The normal vector of the surface.
  \param n_in Refractive index of current medium.
  \param ex_in Extinction coefficient for current medium.
  \param n_out Refractive index of next medium.
  \param ex_out Extinction coefficient for next medium.
  \param direction Photon leaving or entering volume.
  \return Status of the photon.
  */
  virtual const Drc::Reflectivity Query(const PndDrcPhoton&    ph,
					const XYZVector        normal,
					const double           n_in      = 1,
					const double           ex_in     = 0,
					const double           n_out     = 1,
					const double           ex_out    = 0,
					const Drc::ReflDir     direction = Drc::ReflOut) const = 0;
};
#endif
