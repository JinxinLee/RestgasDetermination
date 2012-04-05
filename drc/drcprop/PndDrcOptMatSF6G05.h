/*!
  \brief Class for optical material representation.

  Radiation hard glass.

  See
  http://www.schott.com/advanced_optics/english/abbe_datasheets/schott_datasheetinquiryglass_sf6g05.pdf

*/


#ifndef PNDDRCOPTMATSF6G05_H
#define PNDDRCOPTMATSF6G05_H


#include "PndDrcOptMatAbs.h"

#include "TRandom3.h"

class PndDrcOptMatSF6G05 : public PndDrcOptMatAbs
{
 public:

  //!                                             Empty constructor.
  PndDrcOptMatSF6G05();

  /*! Copy constructor.
    /param Object to copy.
  */
  PndDrcOptMatSF6G05(const PndDrcOptMatSF6G05& mat);

  /*! Assignment operator.
    /param Object to assign.
  */
  PndDrcOptMatSF6G05& operator=(const PndDrcOptMatSF6G05& mat);

  // Implementation of pure virtual functions from base class.
  PndDrcOptMatSF6G05* Clone() const;
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
  void Copy(const PndDrcOptMatSF6G05& s);


};
#endif
