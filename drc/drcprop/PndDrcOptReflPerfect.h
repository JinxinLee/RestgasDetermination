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

  const Drc::Reflectivity Query(const PndDrcPhoton& ph,
				       const XYZVector  normal) const;

};
#endif
