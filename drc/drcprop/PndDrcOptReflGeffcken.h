
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

class PndDrcOptReflGeffcken : public PndDrcOptReflAbs
{

 public:
 
  // implementation of abstract base class function
  PndDrcOptReflGeffcken* Clone() const;

  // implementation of abstract base class function
  const Drc::Reflectivity Query(const PndDrcPhoton&    ph,
				const XYZVector        normal,
				const double           n_next     = 1,
				const Drc::ReflDir     direction = Drc::ReflOut) const;

};
#endif
