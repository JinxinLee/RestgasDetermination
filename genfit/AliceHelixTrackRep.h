#ifndef FITTERALICEHELIXTRACKREP_H
#define FITTERALICEHELIXTRACKREP_H

#include "TMatrixT.h"

#include "AbsTrackRep.h"


class AliceHelixTrackRep : public AbsTrackRep {

 private:

  static const int NparTrackRep = 5;
  
  double extrapolateZ(const TMatrixT<double>& S, double rNew, double rOld) const;
  double extrapolatePhi(const TMatrixT<double>& S, double rNew, double rOld) const;

  double functionForNumerical(int i, int j, double x, const TMatrixT<double>& oldState, double rNew, double rOld) const;
  void makeJacobianNumerical(double,double,
			     const TMatrixT<double>&,
			     TMatrixT<double>&,
			     double) const;

  void makeJacobianAnalytical(double,double,
			 const TMatrixT<double>&,
			 TMatrixT<double>&) const;


  void modStartValues();

 public:
  AliceHelixTrackRep();
  AliceHelixTrackRep(double initialR, 
					 const TMatrixT<double>& initialState,
					 const TMatrixT<double>& initialCov);
  AliceHelixTrackRep(double initialR,double phi,double z,
					 double lambda,double C,double D0,
					 double dphi,double dz,
					 double dlambda,double dC,double dD0);
  virtual ~AliceHelixTrackRep();

  virtual AbsTrackRep* clone(){return new AliceHelixTrackRep(*this);}

  virtual AbsTrackRep* prototype(){return new AliceHelixTrackRep();}

  //overwrite from AbsTrackRep
  virtual void init(const TVector3& startpoint, 
					const TVector3& startmomentum,
					const TVector3& locBField,
					const int charge);



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

  ClassDef(AliceHelixTrackRep,1)

};


#endif // FITTERALICEHELIXTRACKREP_H
