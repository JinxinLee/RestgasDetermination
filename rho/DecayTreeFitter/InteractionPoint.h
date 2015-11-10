// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#ifndef DECAYTREEFITTER_INTERACTIONPOINT_H
#define DECAYTREEFITTER_INTERACTIONPOINT_H 1

#include "ParticleBase.h"
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

  class InteractionPoint : public ParticleBase
  {
  public:
    InteractionPoint(const RhoVector3Err& ipvertex, RhoCandidate* daughter, const Configuration& config) ;
    InteractionPoint(const RhoLorentzVectorErr& ipmom, RhoCandidate* daughter, const Configuration& config) ;
    InteractionPoint(const RhoLorentzVectorErr& ipmom, const RhoVector3Err& ipvertex, RhoCandidate* daughter, const Configuration& config) ;

    virtual int dim() const { return (m_haspos ? 3 : 0); } // (x,y,z are the parameters here, beam moemntum is modifying the daughter) 
    virtual ErrCode initPar1(FitParams*) ;
    virtual ErrCode initPar2(FitParams*) ;
    virtual ErrCode initCov(FitParams*) const ;
    ErrCode initMom( FitParams* fitparams ) const ;

    virtual int type() const { return kInteractionPoint ; }
    virtual std::string parname(int index) const ;

    virtual double chiSquare(const FitParams* par) const ;

    ErrCode projectIPConstraint(const FitParams* fitpar, Projection&) const ;
    ErrCode projectBeamConstraint(const FitParams* fitpar, Projection&) const ;
    virtual ErrCode projectConstraint(Constraint::Type, const FitParams*, Projection&) const ;

    virtual void addToConstraintList(constraintlist& alist, int depth) const ;

    int posIndex() const { return index() ; }
    int momIndex() const { return (*begin())->momIndex(); }
    virtual bool hasEnergy() const { return true ; }
    virtual bool hasPosition() const { return m_haspos ; }

  private:
    TVector3       m_ipPos ;    // interaction point position
    TLorentzVector m_ipMom ;    // Beam Momentum
    TMatrixDSym m_ipCov ;    // cov matrix
    TMatrixDSym m_ipCovInv ; // inverse of cov matrix
    TMatrixDSym m_ipPosCov ;    // cov matrix
    TMatrixDSym m_ipPosCovInv ; // inverse of cov matrix
    TMatrixDSym m_ipMomCov ;    // cov matrix
    TMatrixDSym m_ipMomCovInv ; // inverse of cov matrix
    bool m_haspos;
    bool m_hasmom;
    bool m_hasposcov;
    bool m_hasmomcov;
    ClassDef ( InteractionPoint,1 )
  } ;

}


#endif
