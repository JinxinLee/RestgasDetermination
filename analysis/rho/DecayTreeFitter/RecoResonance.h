// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#ifndef RECORESONANCE_H
#define RECORESONANCE_H 1

#include "RecoComposite.h"
#include "Rtypes.h"

namespace DecayTreeFitter
{

  class RecoResonance : public RecoComposite
  {
  public:
    RecoResonance(RhoCandidate* bc, const ParticleBase* mother) ;
    virtual ~RecoResonance() ;

    virtual int dim() const { return hasEnergy() ? 4 : 3 ; } // (px,py,pz,(E))

    virtual ErrCode projectConstraint(Constraint::Type, const FitParams*, Projection&) const ;
    virtual ErrCode initPar1(FitParams*) ;
    virtual ErrCode initPar2(FitParams*) ;
    virtual int type() const { return kRecoResonance ; }

    virtual int posIndex() const { return mother()->posIndex()  ; }
    virtual int momIndex() const { return index() ; }
    virtual int lenIndex() const { return -1 ; }

    virtual std::string parname(int index) const ;

    virtual void addToConstraintList(constraintlist& alist, int depth) const {
      alist.push_back( Constraint(this,Constraint::resonance,depth,dimM()) ) ; }

  private:
    ClassDef ( RecoResonance,1 )
  } ;

}

#endif
