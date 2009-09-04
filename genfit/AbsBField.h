// Description:
//      Abstract interface to a BField
//      User has to inherit his adaptor from this class
/**
 *  @author Christian H&ouml;ppner (Technische Universit&auml;t M&uuml;nchen, original author)
 *  @author Sebastian Neubert  (Technische Universit&auml;t M&uuml;nchen, original author)
 * 
 */

#ifndef ABSBFIELD_H
#define ABSBFIELD_H

#include "TVector3.h"

class AbsBField {
public:

  // Constructors/Destructors ---------
  AbsBField(){;}
  virtual ~AbsBField(){;}

  // Accessors -----------------------
  virtual TVector3 get(const TVector3&) const =0;
 
};

#endif
