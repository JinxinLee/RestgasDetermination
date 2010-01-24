/*!
  \brief Class for optical material representation.

  This class represents Marcol7 oil. Data are from the thesis Michel Sorel from MiniBooNE.

*/


#ifndef PNDDRCOPTMATMARCOL7_H
#define PNDDRCOPTMATMARCOL7_H


#include "PndDrcOptMatAbs.h"

#include "TRandom3.h"

class PndDrcOptMatMarcol7 : public PndDrcOptMatAbs
{
 public:

  //!                                             Empty constructor.
  PndDrcOptMatMarcol7();

  /*! Copy constructor.
    \param mat Object to copy.
  */
  PndDrcOptMatMarcol7(const PndDrcOptMatMarcol7& mat);

  /*! Assignment operator.
    \param mat Object to assign.
  */
  PndDrcOptMatMarcol7& operator=(const PndDrcOptMatMarcol7& mat);

  // Implementation of pure virtual functions from base class.
  PndDrcOptMatMarcol7* Clone() const;
  double RefIndex(const double lambda) const;
  double RefIndexDeriv(const double lambda) const;
  double Extinction(const double lambda) const {return 0;};
  bool   AbsorptionFlag(double lambda, double length) const;
  string Name() const {return fName;};

 protected:

  mutable TRandom3 fRan;                //!<   Random generator.

 private:

  double fNd;                           //!<   Cauchy equation parameter.
  double fB;                            //!<   Cauchy equation parameter.
  double fLambdaD;                       //!<   Cauchy equation parameter.
  

  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param s The object to copy.
  */
  void Copy(const PndDrcOptMatMarcol7& s);
};
#endif
