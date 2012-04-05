/*!
  \brief Class for optical material representation.

  Radiation hard glass.

  See
  http://www.schott.com/advanced_optics/english/abbe_datasheets/schott_datasheetinquiryglass_lf5g15.pdf

*/


#ifndef PNDDRCOPTMATLF5G15_H
#define PNDDRCOPTMATLF5G15_H


#include "PndDrcOptMatAbs.h"

#include "TRandom3.h"

class PndDrcOptMatLF5G15 : public PndDrcOptMatAbs
{
 public:

  //!                                             Empty constructor.
  PndDrcOptMatLF5G15();

  /*! Copy constructor.
    /param Object to copy.
  */
  PndDrcOptMatLF5G15(const PndDrcOptMatLF5G15& mat);

  /*! Assignment operator.
    /param Object to assign.
  */
  PndDrcOptMatLF5G15& operator=(const PndDrcOptMatLF5G15& mat);

  // Implementation of pure virtual functions from base class.
  PndDrcOptMatLF5G15* Clone() const;
  double RefIndex(const double lambda) const;
  double RefIndexDeriv(const double lambda) const;
  double Extinction(const double lambda) const {return 0;};
  bool   AbsorptionFlag(double lambda, double length) const;
  string Name() const {return fName;};

 protected:

  double  fB1;                          //!<   Sellmeier coefficient for refractive index.
  double  fB2;                          //!<   Sellmeier coefficient for refractive index.
  double  fB3;                          //!<   Sellmeier coefficient for refractive index.
  double  fC1;                          //!<   Sellmeier coefficient for refractive index.
  double  fC2;                          //!<   Sellmeier coefficient for refractive index.
  double  fC3;                          //!<   Sellmeier coefficient for refractive index.

 private:
  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param s The object to copy.
  */
  void Copy(const PndDrcOptMatLF5G15& s);


};
#endif
