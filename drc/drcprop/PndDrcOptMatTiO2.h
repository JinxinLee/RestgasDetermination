/*!
  \brief Class for optical material representation.

  In this case TiO2. Data are from
  http://refractiveindex.info/?group=CRYSTALS&material=TiO2
  Absorption is ignored.
*/


#ifndef PNDDRCOPTMATTIO2_H
#define PNDDRCOPTMATTIO2_H


#include "PndDrcOptMatAbs.h"

#include "TRandom3.h"

class PndDrcOptMatTiO2 : public PndDrcOptMatAbs
{
 public:

  //!                                             Empty constructor.
  PndDrcOptMatTiO2();

  /*! Copy constructor.
    /param Object to copy.
  */
  PndDrcOptMatTiO2(const PndDrcOptMatTiO2& mat);

  /*! Assignment operator.
    /param Object to assign.
  */
  PndDrcOptMatTiO2& operator=(const PndDrcOptMatTiO2& mat);

  // Implementation of pure virtual functions from base class.
  PndDrcOptMatTiO2* Clone() const;
  double RefIndex(const double lambda) const;
  double RefIndexDeriv(const double lambda) const;
  double Extinction(const double lambda) const {return 0;};
  bool   AbsorptionFlag(double lambda, double length) const;
  string Name() const {return fName;};

 protected:

  double  fC1;                          //!<   Sellmeier coefficient for refractive index.
  double  fC2;                          //!<   Sellmeier coefficient for refractive index.
  double  fC3;                          //!<   Sellmeier coefficient for refractive index.
  mutable TRandom3 fRan;                //!<   Random generator.

 private:
  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param s The object to copy.
  */
  void Copy(const PndDrcOptMatTiO2& s);


};
#endif
