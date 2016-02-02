// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#ifndef PARTICLEBASE_H
#define PARTICLEBASE_H 1
#include "Rtypes.h"

#include <string>
#include <vector>
#include "Constraint.h"
#include "Projection.h"
#include "ChiSquare.h"

#include "RhoCandidate.h"

namespace DecayTreeFitter
{
  class FitParams ;
  class Configuration ;

  class ParticleBase
  {
  public:
    enum ParticleType {kInteractionPoint,
      kRecoComposite,kRecoResonance,
      kInternalParticle,kRecoTrack,
      kResonance,kRecoPhoton,
      kMissingParticle} ;
    typedef std::vector<ParticleBase*> ParticleContainer ;

    // 'default' constructor
    ParticleBase(RhoCandidate* bc, const ParticleBase* mother) ;

    // constructor used for InteractionPoint
    ParticleBase(const std::string& name) ;

    virtual ~ParticleBase() ;

    static ParticleBase* createParticle(RhoCandidate* bc,
                                        const ParticleBase* mother,
                                        const Configuration& config) ;

    virtual int dim() const = 0 ;
    virtual void updateIndex(int& offset) ;
    virtual ErrCode initPar1(FitParams*) = 0 ; // init everything that does not need mother vtx
    virtual ErrCode initPar2(FitParams*) = 0 ; // everything else
    virtual ErrCode initCov(FitParams*) const  ;
    virtual std::string parname(int index) const ;
    virtual void print(const FitParams*) const ;

    const ParticleBase* locate(RhoCandidate* bc) const ;
    //    void locate(RhoCandidateID& pid, ParticleContainer& result ) ;
    RhoCandidate* particle() const { return m_particle ; }

    virtual int index() const { return m_index ; }
    const ParticleBase* mother() const { return m_mother ; }
    const std::string& name() const { return m_name ; }

    virtual ErrCode projectGeoConstraint(const FitParams*, Projection&) const ;
    virtual ErrCode projectMassConstraint(const FitParams*, Projection&) const ;
    virtual ErrCode projectConstraint(Constraint::Type, const FitParams*, Projection&) const ;

    // indices to fit parameters
    virtual int type() const = 0 ;
    virtual int posIndex() const { return -1 ; }
    virtual int lenIndex() const { return -1 ; }
    virtual int momIndex() const { return -1 ; }

    // does the particle have a 3-momentum or a 4-momentum ?
    virtual bool hasEnergy() const { return false ; }

    // does the particle have is own decay vertex ? (resonances and
    // recoparticles do not)
    virtual bool hasPosition() const { return false ; }

    int eneIndex() const { return hasEnergy() ? momIndex()+3 : -1 ; }

    // calculates the global chisquare (pretty useless)
    virtual double chiSquareD(const FitParams*) const ;

    // access to particle PDT parameters
    double pdtMass() const { return m_pdtMass ; }
    double pdtWidth() const { return m_pdtWidth ; }
    double pdtCLifeTime() const { return m_pdtCLifeTime ; }
    double pdtTau() const { return m_pdtMass >0 ? m_pdtCLifeTime/m_pdtMass : 0 ; }
    int charge() const { return m_charge ; }

    // return a trajectory
    //    virtual const LHCb::Trajectory* trajectory() const { return 0 ; }

    // access to daughters
    typedef std::vector<ParticleBase*> daucontainer ;
    typedef daucontainer::const_iterator const_iterator ;

    const daucontainer& daughters() const { return m_daughters ; }
    const_iterator begin() const { return m_daughters.begin() ; }
    const_iterator end()   const { return m_daughters.end() ; }
    ParticleBase* addDaughter(RhoCandidate*, const Configuration& config) ;
    void removeDaughter(const ParticleBase* pb) ;

    typedef std::vector< std::pair<const ParticleBase*,int> > indexmap ;
    virtual void retrieveIndexMap(indexmap& anindexmap) const ;

    void setMother(const ParticleBase* m) { m_mother = m ; }

    typedef std::vector<DecayTreeFitter::Constraint> constraintlist ;
    virtual void addToConstraintList(constraintlist& alist, int depth) const = 0 ;
    virtual int nFinalChargedCandidates() const ;
    void setParticle(RhoCandidate* bc) { m_particle = bc ; }

    // collect all particles emitted from vertex with position posindex
    void collectVertexDaughters( daucontainer& particles, int posindex ) ;
    // set the mass constraint for this particle. return true if value changed
    bool setMassConstraint(bool add) {
      std::swap(add,m_hasMassConstraint) ;
      return add != m_hasMassConstraint ;
    }
    // set the mass of the mass constraint (use with care!)
    void setMassConstraint( double mass ) {
      m_hasMassConstraint = true ;
      m_pdtMass = mass ;
    }

    ChiSquare chiSquare( const FitParams* fitparams ) const ;

  protected:
    static double pdtCLifeTime(RhoCandidate* bc)  ;
    //static bool isAResonance(RhoCandidate* bc) ;
    static bool isAResonance(const TParticlePDG* bc) ;// TODO
    //static double bFieldOverC() { return RhoCalculationTools::GetBz ( TVector3(0.,0.,0.) ) / TMath::C() ; } //FIXME: Is that an issue???   // Bz/c
    static double bFieldOverC() ; //FIXME: Is that an issue???   // Bz/c
    ErrCode initTau(FitParams* par) const ;
    void makeName(RhoCandidate* bc)  ;
    daucontainer& daughters() { return m_daughters ; }
    bool hasMassConstraint() const { return m_hasMassConstraint ; }
  protected:
    void setIndex(int i) { m_index = i ; }
    void setName(const std::string& n) { m_name = n ; }
  private:
    RhoCandidate* m_particle ;
    const ParticleBase* m_mother ;
    ParticleContainer m_daughters ;
    const TParticlePDG* m_prop ;
    int m_index ;
    double m_pdtMass ;      // cached mass
    double m_pdtWidth ;     // particle width (for mass constraints)
    double m_pdtCLifeTime ; // cached lifetime
    int m_charge ;      // charge
    std::string m_name ;
    bool m_hasMassConstraint ;
    ClassDef ( ParticleBase,1 )
  } ;

}

#endif
