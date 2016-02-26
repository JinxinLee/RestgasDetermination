// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#ifndef DECAYTREEFITTER_INTERACTIONPOINT_H
#define DECAYTREEFITTER_INTERACTIONPOINT_H 1

#include "InternalParticle.h"
//#include "RecoParticle.h"
//#include "GaudiKernel/GenericVectorTypes.h"
//#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "TMatrixD.h"
#include "RhoVector3Err.h"
#include "Rtypes.h"

namespace LHCb
{
  class VertexBase ;
}

namespace DecayTreeFitter
{

  class InteractionPoint : public InternalParticle
  {
  public:
    InteractionPoint(RhoCandidate* daughter, const Configuration& config) ;
    InteractionPoint(const RhoVector3Err& ipvertex, RhoCandidate* daughter, const Configuration& config) ;
    InteractionPoint(const RhoLorentzVectorErr& ipmom, RhoCandidate* daughter, const Configuration& config) ;
    InteractionPoint(const RhoLorentzVectorErr& ipmom, const RhoVector3Err& ipvertex, RhoCandidate* daughter, const Configuration& config) ;

    virtual int dim() const { return 7; } // (fix to the "first" internal particle ) 
    virtual ErrCode initPar1(FitParams*) ;
    virtual ErrCode initCov(FitParams*) const ;
    //ErrCode initMom( FitParams* fitparams ) const ;

    virtual int type() const { return kInteractionPoint ; }

    virtual double chiSquare(const FitParams* par) const ;

    ErrCode projectIPConstraint(const FitParams* fitpar, Projection&) const ;
    ErrCode projectBeamConstraint(const FitParams* fitpar, Projection&) const ;
    virtual ErrCode projectConstraint(Constraint::Type, const FitParams*, Projection&) const ;

    virtual void addToConstraintList(constraintlist& alist, int depth) const ;


  private:
    TVector3       m_ipPos ;       // interaction point position
    TMatrixDSym    m_ipPosCov ;    // cov matrix
    TMatrixDSym    m_ipPosCovInv ; // inverse of cov matrix
    TLorentzVector m_ipMom ;       // Beam Momentum
    TMatrixDSym    m_ipMomCov ;    // cov matrix
    TMatrixDSym    m_ipMomCovInv ; // inverse of cov matrix
    bool m_hasposcon;
    bool m_hasmomcon;
    bool m_hasposcov;
    bool m_hasmomcov;
    ClassDef ( InteractionPoint,1 )
  } ;

}


#endif
