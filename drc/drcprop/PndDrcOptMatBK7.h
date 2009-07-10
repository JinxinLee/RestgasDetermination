/*! 
  \brief Class for optical material representation.

  In this case the glass N-BK7 (borosilicate glass) from Schott. See 
  http://www.schott.com/optics_devices/german/products/flash/abbediagramm_flash.html

*/


#ifndef PNDDRCOPTMATBK7_H
#define PNDDRCOPTMATBK7_H


#include "PndDrcOptMatAbs.h"

#include "TRandom.h"

class PndDrcOptMatBK7 : public PndDrcOptMatAbs
{
 public:

  //!                                             Empty constructor.
  PndDrcOptMatBK7();                

  /*! Copy constructor.
    /param Object to copy.
  */ 
  PndDrcOptMatBK7(const PndDrcOptMatBK7& mat);

  /*! Assignment operator.
    /param Object to assign.
  */ 
  PndDrcOptMatBK7& operator=(const PndDrcOptMatBK7& mat);

  // Implementation of pure virtual functions from base class.
  PndDrcOptMatBK7* Clone() const;
  double RefIndex(const double lambda) const;
  double RefIndexDeriv(const double lambda) const;
  double Extinction(const double lambda) const {return 0;};
  bool   AbsorptionFlag(double lambda, double length) const;  

 protected:

  double  fB1;                          //!<   Sellmeier coefficient for refractive index.                     
  double  fB2;                          //!<   Sellmeier coefficient for refractive index.
  double  fB3;                          //!<   Sellmeier coefficient for refractive index.   
  double  fC1;                          //!<   Sellmeier coefficient for refractive index.    
  double  fC2;                          //!<   Sellmeier coefficient for refractive index. 
  double  fC3;                          //!<   Sellmeier coefficient for refractive index.    
  mutable TRandom fRan;                 //!<   Random generator.        

 private:
  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param s The object to copy.
  */
  void Copy(const PndDrcOptMatBK7& s);         

 
};
#endif
