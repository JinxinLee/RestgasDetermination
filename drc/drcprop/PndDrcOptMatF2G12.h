/*!
  \brief Class for optical material representation.

  In this case the glass F2G12 (flint glass) from Schott.
  Data from www.knightoptical.com/
  

*/


#ifndef PNDDRCOPTMATF2G12_H
#define PNDDRCOPTMATF2G12_H


#include "PndDrcOptMatAbs.h"

#include "TRandom3.h"

class PndDrcOptMatF2G12 : public PndDrcOptMatAbs
{
 public:

  //!                                             Empty constructor.
  PndDrcOptMatF2G12();

  /*! Copy constructor.
    /param Object to copy.
  */
  PndDrcOptMatF2G12(const PndDrcOptMatF2G12& mat);

  /*! Assignment operator.
    /param Object to assign.
  */
  PndDrcOptMatF2G12& operator=(const PndDrcOptMatF2G12& mat);

  // Implementation of pure virtual functions from base class.
  PndDrcOptMatF2G12* Clone() const;
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
  void Copy(const PndDrcOptMatF2G12& s);


};
#endif
