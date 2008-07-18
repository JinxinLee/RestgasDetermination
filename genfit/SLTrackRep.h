#ifndef FITTER_SLTRACKREP_H
#define FITTER_SLTRACKREP_H

#include "AbsTrackRep.h"

class SLTrackRep : public AbsTrackRep {
  
 private:
  static const int NparTrackRep = 2;
 public:
  SLTrackRep();
  SLTrackRep(double a, double b, double siga, double sigb);
  virtual ~SLTrackRep();
  virtual AbsTrackRep* clone(){return new SLTrackRep(*this);}
  virtual AbsTrackRep* prototype(){return new SLTrackRep();}
  
  virtual void predict(AbsRecoHit* hit,
					   TMatrixT<double>& statePred,
                       TMatrixT<double>& covPred, 
					   TMatrixT<double>& jacobian,
					   double& sPred);
  virtual void extrapolate(double sExtrapolateTo, TMatrixT<double>& statePred);
  virtual void extrapolate(double sExtrapolateFrom, double sExtrapolateTo, 
						   const TMatrixT<double>& stateFrom, 
						   TMatrixT<double>& stateResult);
  virtual void extrapolate(double sExtrapolateTo, 
						   TMatrixT<double>& statePred,
						   TMatrixT<double>& covPred,
						   TMatrixT<double>& jacobian);
  
  virtual inline int getDim() const {
	return NparTrackRep;
  }    


  virtual TVector3 getPos(double z) 
  {
    throw;
    return TVector3();
  }

  virtual TVector3 getMom(double z) 
  {
    throw;
    return TVector3();
  }

  ClassDef(SLTrackRep,1)
  
};

  

#endif // FITT_SLTRACKREP_H
