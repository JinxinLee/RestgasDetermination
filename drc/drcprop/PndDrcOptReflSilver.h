/*! 
  \brief Class for reflectivity of surfaces. 

  Defines the reflectivity of a generic silver mirror. See
  http://www.newport.com/store/genproduct.aspx?id=141088&lang=1033&Section=Spec
*/

#ifndef PNDDRCOPTREFLSILVER_H
#define PNDDRCOPTREFLSILVER_H


#include "PndDrcOptReflAbs.h"

class PndDrcOptReflSilver : public PndDrcOptReflAbs
{

 public:
 
  // implementation of abstract base class function
  PndDrcOptReflSilver* Clone() const;

  /*! \brief Reflectivity of surface 

  used for mirror surfaces or surfacec with internal reflection.
  \param ph The photon
  \param normal Normal vector of surface
  */
  const Drc::Reflectivity Reflectivity(PndDrcPhoton& ph,
				       XYZVector  normal) const;

};
#endif
