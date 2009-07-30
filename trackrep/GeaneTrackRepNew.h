//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      a GEANE (sd-system)  track representation
//      (q/p, v',w',v,w) 
//      (v,w) refers to DetPlane system
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Andrea Fontana       INFN
//
//
//-----------------------------------------------------------

#ifndef GeaneTRACKREPNEW_HH
#define GeaneTRACKREPNEW_HH

// Base Class Headers ----------------
#include "AbsTrackRep.h"
#include "FairTrackParP.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include "TVectorT.h"


// Collaborating Class Declarations --
class FairGeaneProNew;


class GeaneTrackRepNew : public AbsTrackRep {
public:

  // Constructors/Destructors ---------
  GeaneTrackRepNew();
  GeaneTrackRepNew(FairGeaneProNew* geane,
		const DetPlane& plane, // will be defined at origin of plane
		const TVector3& mom,
		const TVector3& poserr,
		const TVector3& momerr,
		double q,
		int PDGCode);


  virtual ~GeaneTrackRepNew();


  virtual AbsTrackRep* clone() const {return new GeaneTrackRepNew(*this);}
  virtual AbsTrackRep* prototype()const{return new GeaneTrackRepNew();}

  // Operators
  friend std::ostream& operator<< (std::ostream& s, const GeaneTrackRepNew& me);

  // Accessors -----------------------

  // Modifiers
  void setReferencePlane(const DetPlane& pl) {_refPlane=pl;}

  // Operations ----------------------

  virtual double extrapolate(const DetPlane&, TMatrixT<double>& statePred);
  //virtual void extrapolate(const DetPlane&, 
  //			   const TMatrixT<double>& stateFrom 
  //			   TMatrixT<double>& stateResult);

  virtual double extrapolate(const DetPlane&, 
			   TMatrixT<double>& statePred,
			   TMatrixT<double>& covPred);

  //these two are overwritting ABsTrackRep methods
  void extrapolateToPoca(const TVector3& pos,
			 TVector3& poca,
			 TVector3& dirInPoca);

  void extrapolateToLine(const TVector3& point1,
	 		 const TVector3& point2,
			 TVector3& poca,
			 TVector3& dirInPoca,
			 TVector3& poca_onwire);


  TVector3 getPocaOnLine(const TVector3& p1, 
			 const TVector3& p2, 
			 bool back=false);

  virtual TVector3 getPos(const DetPlane&) ;
  virtual TVector3 getMom(const DetPlane&) ;
  virtual void getPosMom(const DetPlane&,TVector3& pos,TVector3& mom) ;
  virtual void getPosMomCov(const DetPlane& pl,TVector3& pos,TVector3& mom,TMatrixT<double>& cov);
  virtual double getCharge()const {return state[0][0] > 0 ? 1.: -1.;}
  int getPropDir() {return _backw;} 
  FairGeaneProNew* getPropagator() {return _geane;}
  int getPDG() {return _pdg;};

  void setPropagator(FairGeaneProNew* g){_geane=g;}
  void setPropDir(int d){_backw=d;} 
  void switchDirection(){_backw=-_backw;}

  // (-1,0,1) -> (backward prop,decide myself,forward)

private:

  // Private Data Members ------------
  FairGeaneProNew* _geane; //!
  double _spu; // sign of z-component of momentum
  int _pdg; // pdg code of the particle to be tracked
  int _backw; // (-1,0,1) -> (backward prop,decide myself,forward)

  // Private Methods -----------------
  
  // calculate jacobian of extrapolation
  //void Jacobian(const DetPlane& pl,
  //	const TMatrixT<double>& statePred,
  //	TMatrixT<double>& jacResult);

 public:
  ClassDef(GeaneTrackRepNew,1)

};


#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
