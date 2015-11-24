// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#ifndef RECOTRACK_H
#define RECOTRACK_H 1

//#include "GaudiKernel/Vector3DTypes.h"
#include "RecoParticle.h"
#include "Configuration.h"
#include "TVector3.h"
#include "LineTool.h"
#include "State.h"
#include "Rtypes.h"

//#include "PndTrack.h"
#include "RecoTrackStateProvider.h"

//class ITrackStateProvider ;
//namespace LHCb {
//  class TrackTraj ;
//}
//
namespace DecayTreeFitter
{

  class RecoTrack : public RecoParticle
  {
  public:
    RecoTrack(RhoCandidate* bc, const ParticleBase* mother,
              const Configuration& config) ;
    virtual ~RecoTrack() {};

    virtual ErrCode initPar2(FitParams*) ;
    virtual ErrCode initCov(FitParams*) ;
    virtual int dimM() const { return 5 ; }
    virtual int type() const { return kRecoTrack ; }

    virtual ErrCode projectRecoConstraint(const FitParams*, Projection&) const ;
    ErrCode updCache(const FitParams* fitparams) ; //[R.K.'14] not't cache right now
    //static void setApplyCovCorrection(bool b=true) { gApplyCovCorrection = b ; }
    //static void correctCov(HepSymMatrix& V) ;

    virtual int nFinalChargedCandidates() const { return 1 ; }

    virtual void addToConstraintList(constraintlist& alist, int depth) const {
      alist.push_back(Constraint(this,Constraint::track,depth,dimM()) ) ;
    }
    //ErrCode updFltToMother(const FitParams* fitparams) ;
    void setFlightLength(double flt) { m_flt = flt ; }
    //    const PndTrack& track() const { return *m_track ; }
    //FIXME state needed?
    const DecayTreeFitter::State& state() const { return m_state ; }
    const DecayTreeFitter::State& closestCachedState( double z ) ;
  private:
    const TVector3 m_bfield ;
	  const RhoCandidate* m_candidate;
    //    const PndTrack* m_track ;
    const RecoTrackStateProvider* m_stateprovider ;
    bool m_useTrackTraj ;
    //    const LHCb::TrackTraj* m_tracktraj ;
    bool m_cached ;
    double m_flt ;
    DecayTreeFitter::State m_state ;
	  std::vector<DecayTreeFitter::State> m_StateCache;
    //    double m_bremEnergy ;
    //    double m_bremEnergyCov ;
    ClassDef ( RecoTrack,1 )
  } ;

}
#endif
