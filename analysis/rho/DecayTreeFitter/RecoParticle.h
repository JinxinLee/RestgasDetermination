// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#ifndef RECOPARTICLE_H
#define RECOPARTICLE_H 1

#include "ParticleBase.h"
#include "Rtypes.h"

namespace DecayTreeFitter
{

  class RecoParticle : public ParticleBase
  {
  public:
    RecoParticle(RhoCandidate* bc, const ParticleBase* mother) ;
    virtual ~RecoParticle() ;

    virtual int dimM() const = 0; // dimension of the measurement
    virtual ErrCode initPar1(FitParams*) { return ErrCode::success ; }
    //virtual ErrCode initCov(FitParams*) const ;
    virtual std::string parname(int index) const ;
    virtual int dim() const { return 3; }   //(px,py,pz)

    virtual int momIndex() const { return index() ; }
    virtual bool hasEnergy() const { return false ; }

    virtual ErrCode projectRecoConstraint(const FitParams* fitparams, Projection& p) const = 0 ;
    virtual ErrCode projectConstraint(Constraint::Type, const FitParams*, Projection&) const ;
    virtual double chiSquare(const FitParams* fitparams) const ;

    ClassDef ( RecoParticle,1 )
  } ;

}
#endif
