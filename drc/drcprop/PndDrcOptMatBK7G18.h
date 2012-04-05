/*!
  \brief Class for optical material representation.

  Radiation hard glass.

  In this case the glass N-BK7G18 (borosilicate glass) from Schott. See
  http://www.schott.com/advanced_optics/english/abbe_datasheets/schott_datasheetinquiryglass_bk7g18.pdf

*/


#ifndef PNDDRCOPTMATBK7G18_H
#define PNDDRCOPTMATBK7G18_H


#include "PndDrcOptMatAbs.h"

#include "TRandom3.h"

class PndDrcOptMatBK7G18 : public PndDrcOptMatAbs
{
 public:

  //!                                             Empty constructor.
  PndDrcOptMatBK7G18();

  /*! Copy constructor.
    /param Object to copy.
  */
  PndDrcOptMatBK7G18(const PndDrcOptMatBK7G18& mat);

  /*! Assignment operator.
    /param Object to assign.
  */
  PndDrcOptMatBK7G18& operator=(const PndDrcOptMatBK7G18& mat);

  // Implementation of pure virtual functions from base class.
  PndDrcOptMatBK7G18* Clone() const;
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
  void Copy(const PndDrcOptMatBK7G18& s);


};
#endif
