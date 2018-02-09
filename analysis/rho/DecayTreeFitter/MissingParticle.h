// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#ifndef MISSINGPARTICLE_H
#define MISSINGPARTICLE_H 1
#include "Rtypes.h"

#include "ParticleBase.h"

namespace DecayTreeFitter
{

  class MissingParticle : public ParticleBase
  {
  public:
    MissingParticle(RhoCandidate* bc, const ParticleBase* mother) ;
    virtual ~MissingParticle() ;

    virtual ErrCode initPar1(FitParams*) ;
    virtual ErrCode initPar2(FitParams*) { return ErrCode::success ; }

    virtual std::string parname(int index) const ;
    virtual int dim() const  { return hasMassConstraint() ? 3 : 4 ; }
    virtual int momIndex() const { return index() ; }
    virtual bool hasEnergy() const { return hasMassConstraint() ? false : true ; }
    virtual int type() const { return kMissingParticle ; }
    virtual void addToConstraintList(constraintlist& /*alist*/, int /*depth*/) const {}
    ClassDef ( MissingParticle,1 )
  } ;

}
#endif
