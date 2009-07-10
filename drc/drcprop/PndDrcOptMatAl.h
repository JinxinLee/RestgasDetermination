/*! 
  \brief Class for optical material representation.

  In this case the glass N-Al (extra light flint) from Schott. See 
  http://www.schott.com/optics_devices/german/products/flash/abbediagramm_flash.html

*/


#ifndef PNDDRCOPTMATAl_H
#define PNDDRCOPTMATAl_H


#include "PndDrcOptMatAbs.h"

#include "TRandom.h"

class PndDrcOptMatAl : public PndDrcOptMatAbs
{
 public:

  //!                                             Empty constructor.
  PndDrcOptMatAl();                

  /*! Copy constructor.
    /param Object to copy.
  */ 
  PndDrcOptMatAl(const PndDrcOptMatAl& mat);

  /*! Assignment operator.
    /param Object to assign.
  */ 
  PndDrcOptMatAl& operator=(const PndDrcOptMatAl& mat);

  // Implementation of pure virtual functions from base class.
  PndDrcOptMatAl* Clone() const;
  double RefIndex(const double lambda) const;
  double RefIndexDeriv(const double lambda) const;
  double Extinction(const double lambda) const;
  bool   AbsorptionFlag(double lambda, double length) const;  


 private:
  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param s The object to copy.
  */
  void Copy(const PndDrcOptMatAl& s);         

 
};
#endif
