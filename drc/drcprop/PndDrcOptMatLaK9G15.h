/*!
  \brief Class for optical material representation.

  Radiation hard glass.

  In this case the glass N-LaK9G15 (borosilicate glass) from Schott. See
  http://www.schott.com/advanced_optics/english/abbe_datasheets/schott_datasheetinquiryglass_lak9g15.pdf

*/


#ifndef PNDDRCOPTMATLaK9G15_H
#define PNDDRCOPTMATLaK9G15_H


#include "PndDrcOptMatAbs.h"

#include "TRandom3.h"

class PndDrcOptMatLaK9G15 : public PndDrcOptMatAbs
{
 public:

  //!                                             Empty constructor.
  PndDrcOptMatLaK9G15();

  /*! Copy constructor.
    /param Object to copy.
  */
  PndDrcOptMatLaK9G15(const PndDrcOptMatLaK9G15& mat);

  /*! Assignment operator.
    /param Object to assign.
  */
  PndDrcOptMatLaK9G15& operator=(const PndDrcOptMatLaK9G15& mat);

  // Implementation of pure virtual functions from base class.
  PndDrcOptMatLaK9G15* Clone() const;
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
  void Copy(const PndDrcOptMatLaK9G15& s);


};
#endif
