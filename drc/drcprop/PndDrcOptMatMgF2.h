/*!
  \brief Class for optical material representation.

  In this case MgF2. Data are from
  http://www.cvimellesgriot.com/products/Documents/Catalog/Dispersion_Equations.pdf

  Birefringence is ignored, the ordinary values are taken.
  Absorption is ignored.
*/


#ifndef PNDDRCOPTMATMGF2_H
#define PNDDRCOPTMATMGF2_H


#include "PndDrcOptMatAbs.h"

#include "TRandom3.h"

class PndDrcOptMatMgF2 : public PndDrcOptMatAbs
{
 public:

  //!                                             Empty constructor.
  PndDrcOptMatMgF2();

  /*! Copy constructor.
    /param Object to copy.
  */
  PndDrcOptMatMgF2(const PndDrcOptMatMgF2& mat);

  /*! Assignment operator.
    /param Object to assign.
  */
  PndDrcOptMatMgF2& operator=(const PndDrcOptMatMgF2& mat);

  // Implementation of pure virtual functions from base class.
  PndDrcOptMatMgF2* Clone() const;
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
  void Copy(const PndDrcOptMatMgF2& s);


};
#endif
