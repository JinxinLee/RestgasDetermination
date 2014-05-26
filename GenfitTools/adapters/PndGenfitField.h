#ifndef PNDGENFITFIELD_H
#define PNDGENFITFIELD_H

#include"core/include/AbsBField.h"

/** @brief  Magnetic field
 *
 *  @author Promme (Prometeusz Jasinski)
 * //  modified by Elisabetta Prencipe 19/5/2014
 */

class PndGenfitField : public AbsBField
{

 public:
  //! define the field in this ctor (?)
  PndGenfitField();

  //! return value at position
  TVector3 get(const TVector3& pos) const;
  
 private:
  
};

#endif
