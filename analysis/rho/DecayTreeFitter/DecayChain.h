// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#ifndef DECAYCHAIN_H
#define DECAYCHAIN_H 1

#include <map>
#include "ParticleBase.h"
#include "MergedConstraint.h"
#include "Rtypes.h"

//namespace LHCb {
//  class Particle ;
//  class VertexBase ;
//}

namespace DecayTreeFitter {

  class FitParams ;
  class ParticleBase ;

  class DecayChain
  {
  public:
    DecayChain() : m_mother(0) {}

    DecayChain(RhoCandidate* bc, const Configuration& config)  ;
    DecayChain(RhoCandidate* bc, const RhoVector3Err& pv, const Configuration& config)  ;
    DecayChain(RhoCandidate* bc, const RhoLorentzVectorErr& lv, const Configuration& config)  ;
    DecayChain(RhoCandidate* bc, const RhoLorentzVectorErr& lv, const RhoVector3Err& pv, const Configuration& config)  ;

    virtual ~DecayChain() ;

    int dim() const { return m_dim ; }

    void initConstraintList() ;
    ErrCode init(FitParams* par) ;
    ErrCode filter(FitParams* par, bool firstpass=true) ;
    double chiSquare(const FitParams* par) const ;

    ParticleBase* mother() { return m_mother ; }
    const ParticleBase* cand() { return m_cand ; }
    const ParticleBase* mother() const { return m_mother ; }
    const ParticleBase* locate(RhoCandidate* bc) const ;
    //    void locate( RhoCandidate* pid, ParticleBase::ParticleContainer& result ) ;

    int index(RhoCandidate* bc) const ;
    int posIndex(RhoCandidate* bc) const ;
    int momIndex(RhoCandidate* bc) const ;
    int lenIndex(RhoCandidate* bc) const ;
    void setOwner(bool b) { m_isOwner=b ;}
    //int momIndex() const ;

    void printConstraints(std::ostream& os=std::cout) const ;
    void setMassConstraint( RhoCandidate* bc, bool add=true) ;
    void setMassConstraint( RhoCandidate* bc, double mass) ;
    //    void setMassConstraint( RhoCandidate* pid, bool add=true) ;
    //    void setMassConstraint( RhoCandidate* pid, double mass) ;

    ChiSquare chiSquare( RhoCandidate* bc, const FitParams* fitpars ) const ;
  private:
    int m_dim ;
    ParticleBase* m_mother ;     // head of decay tree
    const ParticleBase* m_cand ; // fit candidate (not same to mother in case of bs/be constraint)
    ParticleBase::constraintlist m_constraintlist ;
    std::vector<Constraint*> m_mergedconstraintlist ;
    MergedConstraint m_mergedconstraint ;
    typedef std::map<RhoCandidate*,const ParticleBase*> ParticleMap ;
    mutable ParticleMap m_particleMap ;
    bool m_isOwner ;
    ClassDef ( DecayChain,1 )
  } ;

}



#endif
