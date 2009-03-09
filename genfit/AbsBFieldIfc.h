// Description:
//      Abstract interface to a BField
//      User has to inherit his adaptor from this class
/**
 *  @author Christian H&ouml;ppner (Technische Universit&auml;t M&uuml;nchen, original author)
 *  @author Sebastian Neubert  (Technische Universit&auml;t M&uuml;nchen, original author)
 * 
 */

#ifndef ABSBFIELDIFC_H
#define ABSBFIELDIFC_H

#include "TVectorT.h"

class AbsBFieldIfc {
public:

  // Constructors/Destructors ---------
  AbsBFieldIfc(){;}
  virtual ~AbsBFieldIfc(){;}

  // Accessors -----------------------
  virtual TVectorT<double> get(const TVectorT<double>& pos) const =0;
 
};

#endif
