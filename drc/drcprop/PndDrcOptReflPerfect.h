/*! 
  \brief Class for reflectivity of surfaces. 

  Defines the reflectivity of a perfect mirror. 
*/

#ifndef PNDDRCOPTREFLPERFECT_H
#define PNDDRCOPTREFLPERFECT_H

#include "PndDrcOptReflAbs.h"

class PndDrcOptReflPerfect : public PndDrcOptReflAbs
{

 public:
 
  // implementation of abstract base class function
  PndDrcOptReflPerfect* Clone() const;

  /*! \brief Reflectivity of surface 

  used for mirror surfaces or surfaces with internal reflection.
  \param ph The photon
  \param normal Normal vector of surface
  */
  const Drc::Reflectivity Reflectivity(PndDrcPhoton& ph,
				       XYZVector  normal) const;

};
#endif
