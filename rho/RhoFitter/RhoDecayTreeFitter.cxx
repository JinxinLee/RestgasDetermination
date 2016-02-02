// ******************************************************
// Interface to DecayTreeFitter Package
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#include "RhoDecayTreeFitter.h"

ClassImp(RhoDecayTreeFitter);
  
/// constructor from the particle (decay head)
RhoDecayTreeFitter::RhoDecayTreeFitter ( RhoCandidate*   bc, int verbosity) 
: Fitter(bc,new DecayTreeFitter::RecoTrackStateProvider(0.05), verbosity)
{
  // we create the Stateprovider so we have to delete it ourselves later!
}

/// constructor from the particle (decay head) and primary vertex
RhoDecayTreeFitter::RhoDecayTreeFitter ( RhoCandidate*   bc, const RhoVector3Err& pv, int verbosity) 
: Fitter(bc, pv,new DecayTreeFitter::RecoTrackStateProvider(0.05), verbosity)
{
  // we create the Stateprovider so we have to delete it ourselves later!
}

/// constructor from the particle (decay head) and pbeam
RhoDecayTreeFitter::RhoDecayTreeFitter ( RhoCandidate*   bc, const RhoLorentzVectorErr& lv, int verbosity) 
: Fitter(bc, lv,new DecayTreeFitter::RecoTrackStateProvider(0.05), verbosity)
{
  // we create the Stateprovider so we have to delete it ourselves later!
}


/// constructor from the particle (decay head), pbeam and primary vertex
RhoDecayTreeFitter::RhoDecayTreeFitter ( RhoCandidate*   bc, const RhoLorentzVectorErr& lv, const RhoVector3Err& pv, int verbosity)
: Fitter(bc, lv, pv,new DecayTreeFitter::RecoTrackStateProvider(0.05), verbosity)
{
  // we create the Stateprovider so we have to delete it ourselves later!
}

    
/// destructor
RhoDecayTreeFitter::~RhoDecayTreeFitter ()
{
  if(m_extrapolator) delete m_extrapolator;
}
    
