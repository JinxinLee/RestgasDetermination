/*!
  \brief Class for optical material representation.

  Radiation hard nonscintillating crystal with high refractive index.

  http://refractiveindex.info/?group=CRYSTALS&material=PbF2
*/


#ifndef PNDDRCOPTMATPBF2_H
#define PNDDRCOPTMATPBF2_H


#include "PndDrcOptMatAbs.h"

#include "TRandom3.h"

class PndDrcOptMatPbF2 : public PndDrcOptMatAbs
{
 public:

  //!                                             Empty constructor.
  PndDrcOptMatPbF2();

  /*! Copy constructor.
    /param Object to copy.
  */
  PndDrcOptMatPbF2(const PndDrcOptMatPbF2& mat);

  /*! Assignment operator.
    /param Object to assign.
  */
  PndDrcOptMatPbF2& operator=(const PndDrcOptMatPbF2& mat);

  // Implementation of pure virtual functions from base class.
  PndDrcOptMatPbF2* Clone() const;
  double RefIndex(const double lambda) const;
  double RefIndexDeriv(const double lambda) const;
  double Extinction(const double lambda) const {return 0;};
  bool   AbsorptionFlag(double lambda, double length) const;
  string Name() const {return fName;};

 protected:

  double  fB1;                          //!<   Sellmeier coefficient for refractive index.
  double  fB2;                          //!<   Sellmeier coefficient for refractive index.
  double  fB3;                          //!<   Sellmeier coefficient for refractive index.
  double  fB4;                          //!<   Sellmeier coefficient for refractive index.
  double  fC1;                          //!<   Sellmeier coefficient for refractive index.
  double  fC2;                          //!<   Sellmeier coefficient for refractive index.
  double  fC3;                          //!<   Sellmeier coefficient for refractive index.
  double  fC4;                          //!<   Sellmeier coefficient for refractive index.

 private:
  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param s The object to copy.
  */
  void Copy(const PndDrcOptMatPbF2& s);


};
#endif
