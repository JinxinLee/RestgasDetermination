#ifndef RKTRACKREP_H
#define RKTRACKREP_H

/*
  Track parametrization adapted from the COMPASS experimt's analysis 
  software Phast. The state vector is (X,Y,dX/dZ,dY/dZ,p/|vec{p}|)
 */

#include "AbsTrackRep.h"
#include "AbsBField.h"

class AbsBField;

class RKtrackRep : public AbsTrackRep {
public:

  // Constructors/Destructors ---------
  RKtrackRep();
  RKtrackRep(const TVector3& pos,
	     const TVector3& mom,
	     const TVector3& poserr,
	     const TVector3& momerr,
	     const double& q,
	     const int& PDGCode,
	     const AbsBField* f);

  virtual ~RKtrackRep();


  virtual AbsTrackRep* clone() const {return new RKtrackRep(*this);}
  virtual AbsTrackRep* prototype()const{return new RKtrackRep();}

  virtual double extrapolate(const DetPlane&, 
			   TMatrixT<double>& statePred,
			   TMatrixT<double>& covPred);

  /*
  void extrapolateToPoca(const TVector3& pos,
			 TVector3& poca,
			 TVector3& dirInPoca);

  void extrapolateToLine(const TVector3& point1,
	 		 const TVector3& point2,
			 TVector3& poca,
			 TVector3& dirInPoca,
			 TVector3& poca_onwire);
  */

  virtual TVector3 getPos(const DetPlane&);
  virtual TVector3 getMom(const DetPlane&);
  virtual void getPosMom(const DetPlane&,TVector3& pos,TVector3& mom);

  virtual double getCharge()const {;}

  void switchDirection(){;}

private:
  const AbsBField *field;//!
  int pdg;
  double myZ() const;
  TMatrixT<double> cov15to25(const TMatrixT<double>& cov15) const;
  TMatrixT<double> cov25to15(const TMatrixT<double>& cov25) const;

  void addNoise(double x,double RadLen,const TMatrixT<double>& state,TMatrixT<double>& cov15);
  bool RKutta (double* SU,double* VO, double& Path) const;
  double Extrap( double Z, double& zOut, TMatrixT<double>& stateOut, TMatrixT<double>& covOut) const;
 public:
  ClassDef(RKtrackRep,1)


};


#endif

