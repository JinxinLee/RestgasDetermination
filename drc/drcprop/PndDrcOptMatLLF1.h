/*! 
  \brief Class for optical material representation.

  In this case the glass N-LLF1 (extra light flint) from Schott. See 
  http://www.schott.com/optics_devices/german/products/flash/abbediagramm_flash.html

*/


#ifndef PNDDRCOPTMATLLF1_H
#define PNDDRCOPTMATLLF1_H


#include "PndDrcOptMatAbs.h"

#include "TRandom.h"

class PndDrcOptMatLLF1 : public PndDrcOptMatAbs
{
 public:

  //!                                             Empty constructor.
  PndDrcOptMatLLF1();                

  /*! Copy constructor.
    /param Object to copy.
  */ 
  PndDrcOptMatLLF1(const PndDrcOptMatLLF1& mat);

  /*! Assignment operator.
    /param Object to assign.
  */ 
  PndDrcOptMatLLF1& operator=(const PndDrcOptMatLLF1& mat);

  // Implementation of pure virtual functions from base class.
  PndDrcOptMatLLF1* Clone() const;
  double RefIndex(const double lambda) const;
  double RefIndexDeriv(const double lambda) const;
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
  void Copy(const PndDrcOptMatLLF1& s);         

 
};
#endif
