/*! 
  \brief Class for optical material representation.

  In this case the fused silica grade Q0 from Lithotec.
  This is more an example: the material parameters are
  general fused quartz parameters.

  The following cefficients come from a fit to data of
  http://r.home.cern.ch/r/richrd26/www/hmpid/richsim.html (UV)
  and from visible light data from Kohlrausch (german text book)


*/


#ifndef PNDDRCOPTMATLITHOTECQ0_H
#define PNDDRCOPTMATLITHOTECQ0_H

#include "PndDrcOptMatAbs.h"

#include "TRandom.h"

class PndDrcOptMatLithotecQ0 : public PndDrcOptMatAbs
{
 public:

  //!                                             Empty constructor.
  PndDrcOptMatLithotecQ0();                

  /*! Copy constructor.
    /param Object to copy.
  */ 
  PndDrcOptMatLithotecQ0(const PndDrcOptMatLithotecQ0& mat);

  /*! Assignment operator.
    /param Object to assign.
  */ 
  PndDrcOptMatLithotecQ0& operator=(const PndDrcOptMatLithotecQ0& mat);

  // Implementation of pure virtual functions from base class.
  PndDrcOptMatLithotecQ0* Clone() const;
  double RefIndex(const double lambda) const;
  double RefIndexDeriv(const double lambda) const;
  double Extinction(const double lambda) const {return 0;};
  bool   AbsorptionFlag(double lambda, double length) const;  
 protected:

  double  fF1;                          //!<  Sellmeier coefficient for refractive index. 
  double  fE1;                          //!<  Sellmeier coefficient for refractive index. 
  double  fF2;                          //!<  Sellmeier coefficient for refractive index.  
  double  fE2;                          //!<  Sellmeier coefficient for refractive index.  
  mutable TRandom fRan;                 //!<  Random generator.              

 private:
  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param s The object to copy.
  */
  void Copy(const PndDrcOptMatLithotecQ0& s);         


 
};
#endif
