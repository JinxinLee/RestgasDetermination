// ******************************************************
// Interface to DecayTreeFitter Package
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
// Usage in macros:
// RhoCandidate* candidate = ...; 
// RhoDecayTreeFitter fitter(candidate);
// fitter.SetMassConstraint(candidate->Daughter(1)); // optional
// 
// 
//  
// 
// 
// 
// 
// ******************************************************

#ifndef RHODECAYTREEFITTER_h
#define RHODECAYTREEFITTER_h 1

#include "RecoTrackStateProvider.h"
#include "Fitter.h"

class RhoDecayTreeFitter : public DecayTreeFitter::Fitter
{
  public:
  
    /// constructor from the particle (decay head)
    RhoDecayTreeFitter ( RhoCandidate*   bc, int verbosity = 0) ;

    /// constructor from the particle (decay head) and primary vertex
    RhoDecayTreeFitter ( RhoCandidate*   bc, const RhoVector3Err& pv, int verbosity = 0) ;

    /// constructor from the particle (decay head) and pbeam
    RhoDecayTreeFitter ( RhoCandidate*   bc, const RhoLorentzVectorErr& lv, int verbosity = 0) ;

    /// constructor from the particle (decay head), pbeam and primary vertex
    RhoDecayTreeFitter ( RhoCandidate*   bc, const RhoLorentzVectorErr& lv, const RhoVector3Err& pv, int verbosity = 0) ;
    
    /// destructor
    ~RhoDecayTreeFitter ();
    
  private:
  
    ClassDef ( RhoDecayTreeFitter,1 )
};

#endif
