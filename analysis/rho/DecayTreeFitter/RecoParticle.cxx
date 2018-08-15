// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#include <stdio.h>

//#include "Event/Particle.h"
#include "RecoParticle.h"
#include "FitParams.h"

using namespace DecayTreeFitter;
ClassImp(RecoParticle);

extern int vtxverbose ;

DecayTreeFitter::RecoParticle::RecoParticle(RhoCandidate* bc, const ParticleBase* aMother)
: ParticleBase(bc,aMother)
{
}

DecayTreeFitter::RecoParticle::~RecoParticle()
{
}

std::string DecayTreeFitter::RecoParticle::parname(int ind) const
{
  return ParticleBase::parname(ind+4) ;
}

ErrCode
DecayTreeFitter::RecoParticle::projectConstraint(Constraint::Type aType,
                                const FitParams* fitparams,
                                Projection& p) const
{
  ErrCode aStatus ;
  switch(aType) {
    case Constraint::track:
    case Constraint::photon:
      aStatus |= projectRecoConstraint(fitparams,p) ;
      break ;
    default:
      aStatus |= ParticleBase::projectConstraint(aType,fitparams,p) ;
  }
  return aStatus ;
}

double DecayTreeFitter::RecoParticle::chiSquare(const FitParams* fitparams) const
{
  std::cout<<" Marke 3" <<std::endl;
  // project
  Projection p(fitparams->dim(),dimM()) ;
  projectRecoConstraint(fitparams,p) ;
  return p.chiSquare() ;
}
