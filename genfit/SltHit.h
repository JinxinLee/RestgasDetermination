#ifndef FITTER_SLTHIT_H
#define FITTER_SLTHIT_H

#include "AbsRecoHit.h"
#include "AbsTrackRep.h"

#include "TMatrixT.h"

    
class SltHit : public AbsRecoHit {
 private:
  static const int NparHitRep = 1;
 public:
  virtual ~SltHit();
  SltHit();
  SltHit(double y, double z, double dy);
  void setHit(double y, double z, double dy);
  virtual double residualScalar(double s, const AbsTrackRep* stateVector,
								const TMatrixT<double>& state);
  virtual void setHMatrix(double s, const AbsTrackRep* stateVector,
						  const TMatrixT<double>& state);

  virtual AbsRecoHit* clone(){return new SltHit(*this);}

};




#endif //FITTER_SLTHIT_H
