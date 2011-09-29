#include "TRandom3.h"

#ifndef PNDDRCOPTREFLABS_H
#define PNDDRCOPTREFLABS_H

namespace Drc
{
/*! \enum Drc::Reflectivity 
  An enum holding the status of a photon at a surface.*/
 enum Reflectivity
    {
      ReflTransmitted,         //!< Photon is transmitted (dichr. mirr., coatings, gray filter)
      ReflAbsorbed,            //!< Photon is absorbed (mirror).
      ReflReflected,           //!< Photon is reflected (mirror).
      ReflRefracted,           //!< Photon will be refracted after a later Fresnel check.
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

namespace Drc
{
/*! \enum Drc::PolDir 
  An enum holding the electrical polarization vector which should be computed.
  This is important for coatings and dichroic mirrors, where the layer sequence plays a role.*/
 enum PolDir
    {
      PolDirS,         //!< Polarization is perpendicular to plane of incidence.
      PolDirP,         //!< Polarization is parallel to plane of incidence.
      PolDirBoth,      //!< Polarization will be calculated for both, perp.and para.
      PolDirPhot       //!< Polarization taken from Photon (not yet implemented).
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

  //! Constructor
  PndDrcOptReflAbs();
  


 public:

  /*! \brief Copy constructor.
    \param s Object to copy.
  */
  PndDrcOptReflAbs(const PndDrcOptReflAbs& s);  

  /*! \brief assignment operator
    \param s Object to assign.
    \return result.
  */
  PndDrcOptReflAbs& operator=(const PndDrcOptReflAbs& s);
  

  virtual ~PndDrcOptReflAbs();

  /*! \brief Virtual copy constructor.
    \return Pointer to new allocated memory.
  */
  virtual PndDrcOptReflAbs* Clone() const = 0;

  /*! \brief Verbosity

  Set the verbosity. Range is 0 to 5. 
  \verbatim
  0=quiet, 
  1=constructors,destructors,
  2=member functions, 
  3=functionality
  4=photons
  5=everything
  \endverbatim
  \param level The verbosity level
  */
  void SetVerbosity(int level){fVerbosity=level;};

  /*! \brief Verbosity.
    \return Verbosity
    \sa SetVerbosity()
  */
  const int Verbosity() const {return fVerbosity;};

  /*! \brief Query reflectivity of surface 

  used for mirror surfaces or surfaces with internal reflection.
  \param ph The photon
  \param normal The normal vector of the surface.
  \param n_next Refractive index of next medium.
  \param direction Photon leaving or entering volume.
  \return Status of the photon.

  The refractive index of the actual medium is accessible via 
  \sa PndDrcPhoton::Device()

  */
  virtual const Drc::Reflectivity Query(const PndDrcPhoton&    ph,
					const XYZVector        normal,
					const double           n_next     = 1,
					const Drc::ReflDir     direction  = Drc::ReflOut) const = 0;


  /*! \brief Polarization direction

  The electrical polarization for which the reflectivity shoul be calculated can be set. This routine returns 
  the set value
  \return polarization direction.
  */
  const Drc::PolDir PolarizationDirection(){return fPolDir;};
  
  /*! \brief Set electrical polarization vector

  \param poldir Electrical polarization vector
  */
  void SetPolarizationDirection(Drc::PolDir poldir){fPolDir = poldir;};

  protected:
   int                  fVerbosity;          //!< Verbosity from 0 to 5.   
   Drc::PolDir          fPolDir;             //!< Comp. for which polarization vector.
 
 private:
  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param s The object to copy.
  */
  void Copy(const PndDrcOptReflAbs& s);         

};
#endif
