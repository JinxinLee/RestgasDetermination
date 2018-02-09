// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#ifndef DECAYTREEFITTER_RECOTRACKSTATEPROVIDER_H
#define DECAYTREEFITTER_RECOTRACKSTATEPROVIDER_H 1

//#include "TrackInterfaces/ITrackStateProvider.h"
//#include "TrackKernel/TrackTraj.h"
//FIXME Make a Panda state provider...
#include "RhoCalculationTools.h"
//#include "LineTool.h"
#include "State.h"
#include "Rtypes.h"
#include "RhoCandidate.h"
#include "TVectorD.h"
#include "TMatrixDSym.h"

namespace DecayTreeFitter
{
  // wrapper around ITrackStateProviderTool
  class RecoTrackStateProvider
  {
  public:
    RecoTrackStateProvider(double ztolerance)
    : m_ztolerance(ztolerance) {};

    RecoTrackStateProvider()
    : m_ztolerance(0) {};
    
    virtual ~RecoTrackStateProvider(){};

    void state( DecayTreeFitter::State& aState, RhoCandidate* track) const
    {
      return state(aState, track, track->Pos().x(), track->Pos().y(), track->Pos().z());
  	};
    
    double ToleranceZ() const {return m_ztolerance;};
    void SetToleranceZ(double z) {m_ztolerance=z;};

    void state( DecayTreeFitter::State& aState, RhoCandidate* track, double vx, double vy, double vz) const
    {
      TVectorD statevec = aState.stateVector();
      TMatrixDSym statecov = aState.covariance();
      double newz=RhoCalculationTools::StateFromTrajectory( statevec, statecov, track, vx,  vy, vz,  m_ztolerance) ;
      aState.setState(statevec);
      aState.setZ(newz);
      aState.setCovariance(statecov);
    };
    
    void P6FromTrack (TVectorD& p6, TMatrixDSym& cov6, RhoCandidate* track, double z) const {
      RhoCalculationTools::P6FromTrajectory(p6, cov6, track, z,  m_ztolerance);
    };
  private:
    double m_ztolerance ;
    bool m_usetraj ;
    ClassDef ( RecoTrackStateProvider,1 )
  } ;
}

#endif
