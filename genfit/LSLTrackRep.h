/** @addtogroup genfit
 * @{ */


/**
 *  @author Christian H&ouml;ppner (Technische Universit&auml;t M&uuml;nchen, original author)
 *  @author Sebastian Neubert  (Technische Universit&auml;t M&uuml;nchen, original author)
 * 
 */
#ifndef LSLTRACKREP_H
#define LSLTRACKREP_H

#include <ostream> // remove if you do not need streaming op

#include "TVectorT.h"
#include "TMatrixT.h"

#include "AbsTrackRep.h"

// Collaborating Class Declarations --
class AbsBField;
class AbsNystromEQM;

/** @brief a local straight line track representation
 *
 * The parameterization for the local straight line track representation is:
 * - x (in global coordinates)
 * - y (in global coordinates)
 * - dx/dz
 * - dy/dz
 * - 1/p
 *
 * The free parameter is z (in global coordinates)
 *
 * The LSLTrackRep can only extrapolate to detector planes perpendicular 
 * to the z-axis and as such does not fulfill the general genfit criteria.
 *
 * If no BField is specified through AbsBFieldIfc a constant field of 
 * B=(0,0,2T) is hardcoded.
 *
 * LSLTrackRep uses a simple Nystrom-type integrator to extrapolate the track
 * parameters in the BField.
 *
 * Material effects are NOT taken into account.
 */
class LSLTrackRep : public AbsTrackRep {
public:

  // Constructors/Destructors ---------
  LSLTrackRep();
  LSLTrackRep(double z, double x, double y, 
	      double dxdz, double dydz, double invp,
	      double sigx, double sigy, 
	      double sigdxdz, double sigdydz, 
	      double siginvp, 
	      AbsBField* field);
  LSLTrackRep(const LSLTrackRep& rep); // copy ctor

  virtual ~LSLTrackRep();


  virtual AbsTrackRep* clone()const {return new LSLTrackRep(*this);}

  virtual AbsTrackRep* prototype()const{return new LSLTrackRep();}

  // Operators
  friend std::ostream& operator<< (std::ostream& s, const LSLTrackRep& me);

  // Accessors -----------------------
  void init(const TVector3& pos,
	    double dxdz, double dydz, double invp,
	    double sigx, double sigy,
	    double sigdxdz, double sigdydz, 
	    double siginvp,
	    AbsBField* field);

  // Modifiers
  void SetBField(AbsBField* b);
  void setReferencePlane(const DetPlane& pl) {s=pl.getO().Z();
                                              _refPlane=pl;}
  void setRungeKuttaAcc(double acc){_acc=acc;}
  void setRungeKuttaAdaptive(bool f){_adaptive=f;}
  // Operations ----------------------
  virtual double extrapolate(const DetPlane&, TMatrixT<double>& statePred);
  //virtual void extrapolate(const DetPlane&, 
  //			   const TMatrixT<double>& stateFrom, 
  //			   TMatrixT<double>& stateResult);

  virtual double extrapolate(const DetPlane&, 
			     TMatrixT<double>& statePred,
			     TMatrixT<double>& covPred);


  virtual void extrapolateToPoca(const TVector3&,
				 TVector3& poca,
				 TVector3& dirInPoca);


  virtual void stepalong(double h);

  virtual TVector3 getPos(const DetPlane&) ;
  virtual TVector3 getMom(const DetPlane&) ;
  virtual void getPosMom(const DetPlane&,TVector3& pos, TVector3& mom) ;
  virtual TVectorT<double> getGlobal(); // (x,y,z,px,py,pz)
  virtual TMatrixT<double> getGlobalCov(); // covariances

  virtual double getCharge()const {double fact= inverted ? -1.:1.;
    return state[4][0]<0 ? -fact : fact;}

  virtual void switchDirection(){};
  
private:

  // Private Data Members ------------

  // Private Methods -----------------
  // equation of motion in a magnetic field
  //static TVectorT<double> eqm(const TVectorT<double>& u,
  //			      const TVectorT<double>& uprim,
  //			      const TVectorT<double>& par);

  double s;

  double _acc; // accuracy of runge kutta; default: 1E-2cm
  bool _adaptive;

  AbsNystromEQM* _eqm; //!

  // calculate jacobian of extrapolation
  void Jacobian(const DetPlane& pl,
		const TMatrixT<double>& statePred,
		TMatrixT<double>& jacResult);

 public:
  ClassDef(LSLTrackRep,1)

};


#endif


/** @} */
