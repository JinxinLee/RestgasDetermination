/*!
  \brief Class for bialkali photo cathode efficiency

  for definition see paper Motta et al., NIMA539 (2005) 217 and
  the  Figure 1 for standard bialkali.
*/


#ifndef PNDDRCEFFIBIALKALI_H
#define PNDDRCEFFIBIALKALI_H


#include "PndDrcEffiAbs.h"

#include "TRandom3.h"


class PndDrcEffiBialkali : public PndDrcEffiAbs
{
 public:
  //! Empty construktor
  PndDrcEffiBialkali();

  /*! Copy constructor.
    /param effi Object to copy.
  */
  PndDrcEffiBialkali(const PndDrcEffiBialkali& effi);

  /*! Assignment operator.
    /param effi Object to assign.
  */
  PndDrcEffiBialkali& operator=(const PndDrcEffiBialkali& effi);

  // Implementation of pure virtual functions from base class.
  PndDrcEffiBialkali* Clone() const;
  bool   EffiFlag(double lambda, double cosine) const;
  string Name() const {return fName;};

  //protected:


 private:
  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param s The object to copy.
  */
  void Copy(const PndDrcEffiBialkali& s);

  static  TRandom3 fRan;                //!<   Random generator.
  double fEffiArray[80];                //!<   Array holding the efficiencies.
};

#endif
