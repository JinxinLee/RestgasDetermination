/*! 
  \brief Class for reflectivity of surfaces. 

  Defines the reflectivity of a generic semitransparent mirror.
  There is a member to adjust the reflectivity. Light which is not 
  reflected, is transmitted. It acts like a gray filter. 
*/

#ifndef PNDDRCOPTREFLGRAY_H
#define PNDDRCOPTREFLGRAY_H


#include "PndDrcOptReflAbs.h"

class PndDrcOptReflGray : public PndDrcOptReflAbs
{
  public:
 
  // implementation of abstract base class function
  PndDrcOptReflGray* Clone() const;

  /*! \brief Reflectivity of surface 

  used for mirror surfaces or surfaces with internal reflection.
  \param ph The photon
  \param normal Normal vector of surface
  \return Status of the photon.
  */
  const Drc::Reflectivity Query(const PndDrcPhoton& ph,
				       const XYZVector  normal) const;


  /*! \brief Set reflection probability.

  \param refl Reflection probability [0,1].
  */
  void SetReflProb(float refl) {fReflProb=refl;};
  
  /*! \brief Reflection probability.

  \return Reflection probability [0,1].
  */
  float ReflProb() const {return fReflProb;};
  
  private:

  float fReflProb;                  //!< Reflection probability

  

};
#endif
