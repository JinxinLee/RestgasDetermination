/*!
  \brief Class for optical material representation.

  This material class is for testing with a manually determined refraction index.
*/


#ifndef PNDDRCOPTMATCONSTINDEX_H
#define PNDDRCOPTMATCONSTINDEX_H

#include "PndDrcOptMatAbs.h"

class PndDrcOptMatConstIndex : public PndDrcOptMatAbs
{
 public:

  //                                             Empty constructor.
//   PndDrcOptMatConstIndex();

  //! Normal construcor.
  PndDrcOptMatConstIndex(double refIndex = 1);

  /*! Copy constructor.
    /param Object to copy.
  */
  PndDrcOptMatConstIndex(const PndDrcOptMatConstIndex& mat);

  /*! Assignment operator.
    /param Object to assign.
  */
  PndDrcOptMatConstIndex& operator=(const PndDrcOptMatConstIndex& mat);

  // Implementation of pure virtual functions from base class.
  PndDrcOptMatConstIndex* Clone() const;
  double RefIndex(const double lambda) const;
  double RefIndexDeriv(const double lambda) const;
  double Extinction(const double lambda) const {return 0;};
  bool   AbsorptionFlag(double lambda, double length) const;
  string Name() const {return fName;};

 private:
  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param s The object to copy.
  */
  void Copy(const PndDrcOptMatConstIndex& s);


  double            fRefIndex;                     //!< Refraction index.

};
#endif
