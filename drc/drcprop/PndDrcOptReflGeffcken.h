
#ifndef PNDDRCOPTREFLNONE_H
#define PNDDRCOPTREFLNONE_H

/*! 
  \brief Class for reflectivity of surfaces. 

  Defines the reflectivity of a coated surface. The Geffcken coating is a historical
  broad band coating. Walter Geffcken (1942)

  Eugene Hecht Optics,
  H.Bach, D. Krause Thin Films on Glass

*/

#include "PndDrcOptReflAbs.h"
#include "PndDrcOptMatAbs.h"


#include <list>
using std::list;

class PndDrcOptReflGeffcken : public PndDrcOptReflAbs
{

 public:
 
  PndDrcOptReflGeffcken(); //! Empty constructor.
  

  // implementation of abstract base class function
  PndDrcOptReflGeffcken* Clone() const;

  

  /*! \brief Copy constructor
    \param s The reflectivity
  */
  PndDrcOptReflGeffcken(const PndDrcOptReflGeffcken& s);

  /*! \brief Assignment operator.
    \param s The object to assign.
  */
  PndDrcOptReflGeffcken& operator=(const PndDrcOptReflGeffcken& s);

  //! Destructor.
  virtual ~PndDrcOptReflGeffcken();


  // implementation of abstract base class function
  const Drc::Reflectivity Query(const PndDrcPhoton&    ph,
				const XYZVector        normal,
				const double           n_next     = 1,
				const Drc::ReflDir     direction = Drc::ReflOut) const;


  /*! \brief Probability for photon reflection at surface 

  used for mirror surfaces or surfaces with internal reflection.
  \param ph The photon
  \param normal The normal vector of the surface.
  \param n_next Refractive index of next medium.
  \param direction Photon leaving or entering volume.
  \return Reflection probability probability.

  The refractive index of the actual medium is accessible via 
  \sa PndDrcPhoton::Device()

  */
  const double ReflProb(const PndDrcPhoton&    ph,
			const XYZVector        normal,
			const double           n_next     = 1,
			const Drc::ReflDir     direction = Drc::ReflOut) const;


  protected:
  list<PndDrcOptMatAbs*>     fLayerMaterialList;                 //!< Layers of material
  list<double>               fLayerThicknessList;                //!< Layer thicknesses [nm]
  PndDrcOptMatAbs*           fLayerMaterialLow;                  //!< Low refractive index material.
  PndDrcOptMatAbs*           fLayerMaterialHigh;                 //!< High refractive index material.


  private:
  /*! \brief Copy function for assigment and copy operator.
    \param s The reflectivity
  */
  void Copy(const PndDrcOptReflGeffcken& s);
};
#endif
