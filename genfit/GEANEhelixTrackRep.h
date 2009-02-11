#ifndef FITTERGEANEHELIXTRACKREP_H
#define FITTERGEANEHELIXTRACKREP_H

#include "TMatrixT.h"

#include "AbsTrackRep.h"
#include "FairTrackParH.h"

class GEANEhelixTrackRep : public AbsTrackRep {

 private:

  static const int NparTrackRep = 5;
  
 public:
  GEANEhelixTrackRep();
  GEANEhelixTrackRep(const FairTrackParH& par);
  virtual ~GEANEhelixTrackRep();

  virtual AbsTrackRep* clone(){return new GEANEhelixTrackRep(*this);}

  virtual AbsTrackRep* prototype(){return new GEANEhelixTrackRep();}

  //overwrite from AbsTrackRep
  virtual void init(const TVector3& startpoint, 
					const TVector3& startmomentum,
					const TVector3& locBField,
					const int charge);



  virtual void predict(const DetPlane& plane,
		       TMatrixT<double>& statePred, 
		       TMatrixT<double>& covPred,  
		       TMatrixT<double>& jacobian);

  virtual void extrapolate(const DetPlane& plane, TMatrixT<double>& statePred);
  virtual void extrapolate(const DetPlane& plane,
                           TMatrixT<double>& statePred,
                           TMatrixT<double>& covPred,
                           TMatrixT<double>& jacobian);
 

  virtual inline int getDim() const {
    return NparTrackRep;
  }

  virtual TVector3 getPos(const DetPlane& plane) 
  {
    throw;
    return TVector3();
  }

  virtual TVector3 getMom(const DetPlane& plane) 
  {
    throw;
    return TVector3();
  }

  virtual void setReferencePlane(const DetPlane& plane){
    throw;
  }

  virtual int getCharge() const {
    throw;
    return 0;
  }

  ClassDef(GEANEhelixTrackRep,1)

};


#endif // FITTERGEANEHELIXTRACKREP_H
