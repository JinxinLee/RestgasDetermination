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

  const Drc::Reflectivity Query(const PndDrcPhoton&    ph,
				const XYZVector        normal,
				const double           n_next     = 1,
				const Drc::ReflDir     direction = Drc::ReflOut) const;

};
#endif
