/*!
  \brief Class for optical material representation.

  In this case the glass N-LaK33A (Lanthanum crown) from Schott. See
  http://www.schott.com/optics_devices/german/products/flash/abbediagramm_flash.html

*/


#ifndef PNDDRCOPTMATNLAK33A_H
#define PNDDRCOPTMATNLAK33A_H

#include "PndDrcOptMatAbs.h"

#include "TRandom3.h"

class PndDrcOptMatNLAK33A : public PndDrcOptMatAbs
{
 public:

  //!                                             Empty constructor.
  PndDrcOptMatNLAK33A();

  /*! Copy constructor.
    /param Object to copy.
  */
  PndDrcOptMatNLAK33A(const PndDrcOptMatNLAK33A& mat);

  /*! Assignment operator.
    /param Object to assign.
  */
  PndDrcOptMatNLAK33A& operator=(const PndDrcOptMatNLAK33A& mat);

  // Implementation of pure virtual functions from base class.
  PndDrcOptMatNLAK33A* Clone() const;
  double RefIndex(const double lambda) const;
  double RefIndexDeriv(const double lambda) const;
  double Extinction(const double lambda) const {return 0;};
  bool   AbsorptionFlag(double lambda, double length) const;
  string Name() const {return fName;};


 protected:

  double  fB1;                          //!< Sellmeier coefficient for refractive index.
  double  fB2;                          //!< Sellmeier coefficient for refractive index.
  double  fB3;                          //!< Sellmeier coefficient for refractive index.
  double  fC1;                          //!< Sellmeier coefficient for refractive index.
  double  fC2;                          //!< Sellmeier coefficient for refractive index.
  double  fC3;                          //!< Sellmeier coefficient for refractive index.
  mutable TRandom3 fRan;                //!< Random generator.

 private:
  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param s The object to copy.
  */
  void Copy(const PndDrcOptMatNLAK33A& s);


};
#endif
