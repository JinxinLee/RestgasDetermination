#ifndef FITTER_SL2DTRACKREP_H
#define FITTER_SL2DTRACKREP_H

#include "AbsTrackRep.h"

class SL2DTrackRep : public AbsTrackRep {

    private:
        static const int NparTrackRep = 4;
    public:
        SL2DTrackRep();
        SL2DTrackRep(double ax,double bx, double ay, double by, double sigax, double sigay, double sigay, double sigby);
        virtual ~SL2DTrackRep();
  virtual AbsTrackRep* clone(){return new SL2DTrackRep(*this);}
  virtual AbsTrackRep* prototype(){return new SL2DTrackRep();}

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

};



#endif // FITT_SL2DTRACKREP_H
