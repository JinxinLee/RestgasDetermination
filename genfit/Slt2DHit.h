#ifndef FITTER_SLT2DHIT_H
#define FITTER_SLT2DHIT_H

#include "AbsRecoHit.h"
#include "AbsTrackRep.h"

#include "TMatrixT.h"

    
class Slt2DHit : public AbsRecoHit {
 private:
  static const int NparHitRep = 2;
 public:
  virtual ~Slt2DHit();
  Slt2DHit();
  Slt2DHit(double x, double y, double z, double dx, double dy);
  void setHit(double x, double y, double z, double dx, double dy); 
  virtual double residualScalar(double s, const AbsTrackRep* stateVector,
								const TMatrixT<double>& state);

  virtual void setHMatrix(double s, const AbsTrackRep* stateVector,
						  const TMatrixT<double>& state);

  virtual AbsRecoHit* clone(){return new Slt2DHit(*this);}

};



#endif //FITTER_SLT2DHIT_H
