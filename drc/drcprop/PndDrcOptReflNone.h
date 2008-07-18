
#ifndef PNDDRCOPTREFLNONE_H
#define PNDDRCOPTREFLNONE_H

/*! 
  \brief Class for reflectivity of surfaces. 

  Defines the reflectivity of balck surface, that is no reflectivity. 
*/

#include "PndDrcOptReflAbs.h"

class PndDrcOptReflNone : public PndDrcOptReflAbs
{

 public:
 
  // implementation of abstract base class function
  PndDrcOptReflNone* Clone() const;

  /*! \brief Reflectivity of surface 

  used for mirror surfaces or surfacec with internal reflection.
  \param ph The photon
  \param normal Normal vector of surface
  */
  const Drc::Reflectivity Reflectivity(PndDrcPhoton& ph,
				       XYZVector  normal) const;

};
#endif
