/*! 
  \brief Class for reflectivity of surfaces. 

  Models the internal reflection probability of a surface.
*/

#ifndef PNDDRCOPTREFLSURF_H
#define PNDDRCOPTREFLSURF_H

#include "PndDrcOptReflAbs.h"

class PndDrcOptReflSurf : public PndDrcOptReflAbs
{

 public:
 
  //! Empty constructor
  PndDrcOptReflSurf();
  


  // implementation of abstract base class function
  PndDrcOptReflSurf* Clone() const;

  const Drc::Reflectivity Query(const PndDrcPhoton&    ph,
				const XYZVector        normal,
				const double           n_in      = 1,
				const double           ex_in     = 0,
				const double           n_out     = 1,
				const double           ex_out    = 0,
				const Drc::ReflDir     direction = Drc::ReflOut) const;



  /*! \brief Set the internal reflection probability
    \param p The propability
  */
  void SetProb(double p){fProb=p;};

  /*! \brief Internal reflection probability
    \return The propability
  */
  double Prob() const {return fProb;};
  
  private:
  double   fProb; //!< Reflection probability
   

};
#endif
