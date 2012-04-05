/*!
  \brief Class for optical material representation.

  Radiation hard glass.

  See
  http://www.schott.com/advanced_optics/english/abbe_datasheets/schott_datasheetinquiryglass_k5g20.pdf

*/


#ifndef PNDDRCOPTMATK5G20_H
#define PNDDRCOPTMATK5G20_H


#include "PndDrcOptMatAbs.h"

#include "TRandom3.h"

class PndDrcOptMatK5G20 : public PndDrcOptMatAbs
{
 public:

  //!                                             Empty constructor.
  PndDrcOptMatK5G20();

  /*! Copy constructor.
    /param Object to copy.
  */
  PndDrcOptMatK5G20(const PndDrcOptMatK5G20& mat);

  /*! Assignment operator.
    /param Object to assign.
  */
  PndDrcOptMatK5G20& operator=(const PndDrcOptMatK5G20& mat);

  // Implementation of pure virtual functions from base class.
  PndDrcOptMatK5G20* Clone() const;
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
  void Copy(const PndDrcOptMatK5G20& s);


};
#endif
