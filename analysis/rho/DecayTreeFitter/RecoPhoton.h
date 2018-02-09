// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#ifndef RECOPHOTON_H
#define RECOPHOTON_H 1

#include "RecoParticle.h"
#include "TVector3.h"
#include "Rtypes.h"

namespace DecayTreeFitter
{

  class RecoPhoton : public RecoParticle
  {
  public:

    RecoPhoton(RhoCandidate* bc, const ParticleBase* mother) ;
    virtual ~RecoPhoton() ;

    virtual int dimM() const { return 3 ; }
    virtual ErrCode initPar1(FitParams*) ;
    virtual ErrCode initPar2(FitParams*) ;

    virtual ErrCode initCov(FitParams*) const ;
    virtual int type()     const { return kRecoPhoton ; }
    virtual ErrCode projectRecoConstraint(const FitParams*,Projection&) const ;
    ErrCode updCache() ;

    virtual void addToConstraintList(constraintlist& alist, int depth) const {
      alist.push_back( Constraint(this,Constraint::photon,depth,dimM()) ) ; }

  private:
    virtual ErrCode initParPhoton(FitParams*, const TVector3& motherpos) const ;
    double m_z ;
    TVector3 m_m ;
    TMatrixDSym m_V ;
    ClassDef (RecoPhoton ,1 )
  } ;

}
#endif
