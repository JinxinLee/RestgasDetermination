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

  const Drc::Reflectivity Query(const PndDrcPhoton& ph,
				const XYZVector  normal) const;



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
