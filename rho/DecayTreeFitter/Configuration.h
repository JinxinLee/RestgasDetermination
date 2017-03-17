// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#ifndef DECAYTREEFITTER_CONFIG_H
#define DECAYTREEFITTER_CONFIG_H 1

//class ITrackStateProvider ;
#include "RecoTrackStateProvider.h"
#include "Rtypes.h"

namespace DecayTreeFitter
{
  class Configuration
  {
  public:
    Configuration(const RecoTrackStateProvider* stateprovider=0)
    : m_stateprovider(stateprovider),
    m_useTrackTraj( true )
    {}

    virtual ~Configuration(){};
    
    const RecoTrackStateProvider* stateProvider() const { return m_stateprovider ; }

    bool useTrackTraj() const { return m_stateprovider && m_useTrackTraj ; }

  private:
    const RecoTrackStateProvider* m_stateprovider ;
    bool m_useTrackTraj ;
    ClassDef ( Configuration,1 )
  } ;
}

#endif
