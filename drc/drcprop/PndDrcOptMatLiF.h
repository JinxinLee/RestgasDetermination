/*!
  \brief Class for optical material representation.

  In this case lithium fluoride. See
  http://ircamera.as.arizona.edu/nircam/materials/SPIE_Aug05/5904-25_Leviton.pdf

*/


#ifndef PNDDRCOPTMATLIF_H
#define PNDDRCOPTMATLIF_H

#include "PndDrcOptMatAbs.h"

#include "TRandom3.h"

class PndDrcOptMatLiF : public PndDrcOptMatAbs
{
 public:

  //!                                             Empty constructor.
  PndDrcOptMatLiF();

  /*! Copy constructor.
    /param Object to copy.
  */
  PndDrcOptMatLiF(const PndDrcOptMatLiF& mat);

  /*! Assignment operator.
    /param Object to assign.
  */
  PndDrcOptMatLiF& operator=(const PndDrcOptMatLiF& mat);

  // Implementation of pure virtual functions from base class.
  PndDrcOptMatLiF* Clone() const;
  double RefIndex(const double lambda) const;
  double RefIndexDeriv(const double lambda) const;
  double Extinction(const double lambda) const {return 0;};
  bool   AbsorptionFlag(double lambda, double length) const;  //!< not yet implemented.
  string Name() const {return fName;};

 protected:

  double  fB1;                          //!<    Sellmeier coefficient for refractive index.
  double  fB2;                          //!<    Sellmeier coefficient for refractive index.
  double  fB3;                          //!<    Sellmeier coefficient for refractive index.
  double  fC1;                          //!<    Sellmeier coefficient for refractive index.
  double  fC2;                          //!<    Sellmeier coefficient for refractive index.
  double  fC3;                          //!<    Sellmeier coefficient for refractive index.

 private:
  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param s The object to copy.
  */
  void Copy(const PndDrcOptMatLiF& s);

  /*! \brief calculate Sellmeier coefficients which are here given as function
    of temperature. Source is the paper
    http://ircamera.as.arizona.edu/nircam/materials/SPIE_Aug05/5904-25_Leviton.pdf

    \param b1 Sellmeier coefficient.
    \param b2 Sellmeier coefficient.
    \param b3 Sellmeier coefficient.
    \param c1 Sellmeier coefficient [nm2].
    \param c2 Sellmeier coefficient [nm2].
    \param c3 Sellmeier coefficient [nm2].
    \param temp temperature in kelvin.
  */
  void CalcSellmeier(double& b1, double& b2, double& b3,
		     double& c1, double& c2, double& c3,
		     double temp);

};
#endif
