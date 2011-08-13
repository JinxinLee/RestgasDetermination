
#ifndef PNDDRCOPTREFLNONE_H
#define PNDDRCOPTREFLNONE_H

/*! 
  \brief Class for reflectivity of surfaces. 

  Defines the reflectivity of black surface, that is no reflectivity. 
*/

#include "PndDrcOptReflAbs.h"

class PndDrcOptReflNone : public PndDrcOptReflAbs
{

 public:
 
  // implementation of abstract base class function
  PndDrcOptReflNone* Clone() const;

  const Drc::Reflectivity Query(const PndDrcPhoton& ph,
				       const XYZVector  normal) const;

};
#endif
