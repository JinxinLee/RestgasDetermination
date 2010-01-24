/*!
  \brief Class for photo cathode efficiency


*/


#ifndef PNDDRCEFFIPERFECT_H
#define PNDDRCEFFIPERFECT_H


#include "PndDrcEffiAbs.h"

#include "TRandom3.h"


class PndDrcEffiPerfect : public PndDrcEffiAbs
{
 public:

  //!                                             Empty constructor.
  PndDrcEffiPerfect();

  /*! Copy constructor.
    /param effi Object to copy.
  */
  PndDrcEffiPerfect(const PndDrcEffiPerfect& effi);

  /*! Assignment operator.
    /param effi Object to assign.
  */
  PndDrcEffiPerfect& operator=(const PndDrcEffiPerfect& effi);

  // Implementation of pure virtual functions from base class.
  PndDrcEffiPerfect* Clone() const;
  bool   EffiFlag(double lambda, double cosine) const;
  string Name() const {return fName;};

  //protected:
  //static  TRandom3 fRan;                //!<   Random generator.


 private:
  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param s The object to copy.
  */
  void Copy(const PndDrcEffiPerfect& s);


};

//TRandom3  PndDrcEffiPerfect::fRan;
#endif
