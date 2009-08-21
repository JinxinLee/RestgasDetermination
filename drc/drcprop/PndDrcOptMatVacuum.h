/*!
  \brief Class for optical material representation.

  This material class represents simply nothing (vacuum).
*/


#ifndef PNDDRCOPTMATVACUUM_H
#define PNDDRCOPTMATVACUUM_H

#include "PndDrcOptMatAbs.h"

class PndDrcOptMatVacuum : public PndDrcOptMatAbs
{
 public:

  //!                                             Empty constructor.
  PndDrcOptMatVacuum();

  /*! Copy constructor.
    /param Object to copy.
  */
  PndDrcOptMatVacuum(const PndDrcOptMatVacuum& mat);

  /*! Assignment operator.
    /param Object to assign.
  */
  PndDrcOptMatVacuum& operator=(const PndDrcOptMatVacuum& mat);

  // Implementation of pure virtual functions from base class.
  PndDrcOptMatVacuum* Clone() const;
  double RefIndex(const double lambda) const;
  double RefIndexDeriv(const double lambda) const;
  double Extinction(const double lambda) const {return 0;};
  bool   AbsorptionFlag(double lambda, double length) const;
  string Name() const {return fName;};

 private:
  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param s The object to copy.
  */
  void Copy(const PndDrcOptMatVacuum& s);



};
#endif
