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

  const Drc::Reflectivity Query(const PndDrcPhoton&    ph,
				const XYZVector        normal,
				const double           n_in      = 1,
				const double           ex_in     = 0,
				const double           n_out     = 1,
				const double           ex_out    = 0,
				const Drc::ReflDir     direction = Drc::ReflOut) const;

};
#endif
