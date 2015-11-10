// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#ifndef EXTERNALBTAPARTICLE_H
#define EXTERNALBTAPARTICLE_H 1

#include "ParticleBase.h"
#include "TVectorD.h"
#include "TMatrixDSym.h"
#include "Rtypes.h"

namespace DecayTreeFitter
{

  class RecoComposite : public ParticleBase
  {
  public:
    RecoComposite(RhoCandidate* bc, const ParticleBase* mother) ;
    virtual ~RecoComposite() ;

    // the number of parameters
    virtual int dim() const { return m_hasEnergy ? 8 : 7 ; }// (x,y,z,t,px,py,pz,(E))

    // the number of 'measurements'
    int dimM() const        { return m_hasEnergy ? 7 : 6 ; }
    ErrCode projectRecoComposite(const FitParams*, Projection&) const ;
    virtual ErrCode projectConstraint(Constraint::Type, const FitParams*, Projection&) const ;

    virtual ErrCode initPar1(FitParams*) ;
    virtual ErrCode initPar2(FitParams*) ;
    virtual int type() const { return kRecoComposite ; }

    virtual int posIndex() const { return index()   ; }
    virtual int lenIndex() const { return index()+3 ; }
    virtual int momIndex() const { return index()+4 ; }

    virtual bool hasEnergy() const { return m_hasEnergy ; }
    virtual bool hasPosition() const { return true ; }

    virtual void updCache() ;
    virtual double chiSquare(const FitParams* fitparams) const ;

    virtual void addToConstraintList(constraintlist& alist, int depth) const {
      alist.push_back( Constraint(this,Constraint::composite,depth,dimM()) ) ;
      alist.push_back( Constraint(this,Constraint::geometric,depth,3) ) ;
    }

  protected: // I hate this, so we need to change the design ...
             // cache
    TVectorD m_m ;    // 'measurement' (x,y,zpx,py,pz,E)
    TMatrixDSym m_matrixV ; // covariance in measurement
    bool m_hasEnergy ;
    ClassDef ( RecoComposite,1 )
  } ;

}

#endif
