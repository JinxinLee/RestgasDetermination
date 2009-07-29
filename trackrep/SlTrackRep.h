#ifndef SLTRACKREP_HH
#define SLTRACKREP_HH


#include "AbsTrackRep.h"



class SlTrackRep : public AbsTrackRep {
public:

  // Constructors/Destructors ---------
  SlTrackRep();
  SlTrackRep(const TMatrixT<double>&, const TMatrixT<double>&);


  virtual ~SlTrackRep();


  virtual AbsTrackRep* clone() const {return new SlTrackRep(*this);}
  virtual AbsTrackRep* prototype()const{return new SlTrackRep();}

  void setReferencePlane(const DetPlane& pl) {_refPlane=pl;}

  // Operations ----------------------

  virtual double extrapolate(const DetPlane&, 
			   TMatrixT<double>& statePred,
			   TMatrixT<double>& covPred);


  void extrapolateToPoca(const TVector3& pos,
			 TVector3& poca,
			 TVector3& dirInPoca);

  void extrapolateToLine(const TVector3& point1,
	 		 const TVector3& point2,
			 TVector3& poca,
			 TVector3& dirInPoca,
			 TVector3& poca_onwire);


  virtual TVector3 getPos(const DetPlane&) ;
  virtual TVector3 getMom(const DetPlane&) ;
  virtual void getPosMom(const DetPlane&,TVector3& pos,TVector3& mom) ;
  virtual double getCharge()const {return 0;}

  void switchDirection(){_backw=-_backw;}

private:

  // Private Data Members ------------
  int _backw; // (-1,0,1) -> (backward prop,decide myself,forward)

 public:
  ClassDef(SlTrackRep,1)

};


#endif

