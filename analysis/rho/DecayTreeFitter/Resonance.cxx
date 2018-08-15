// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#include <iomanip>
#include <algorithm>

#include "Resonance.h"

using namespace DecayTreeFitter;

ClassImp(Resonance);

extern int vtxverbose ;

DecayTreeFitter::Resonance::Resonance(RhoCandidate* bc, const ParticleBase* aMother,
                     const Configuration& config)
: InternalParticle(bc,aMother,config)
{
}

DecayTreeFitter::Resonance::~Resonance() {}

ErrCode DecayTreeFitter::Resonance::initPar1(FitParams* fitparams)
{
  if(vtxverbose>5){std::cout<<"Resonance::initPar1: - "<<std::endl;}
  ErrCode status ;
  for(daucontainer::const_iterator it = begin() ; it != end() ; ++it)
    status |= (*it)->initPar1(fitparams) ;
  return status ;
}

ErrCode DecayTreeFitter::Resonance::initPar2(FitParams* fitparams)
{
  if(vtxverbose>5){std::cout<<"Resonance::initPar2: - "<<std::endl;}
  ErrCode status ;
  for(daucontainer::const_iterator it = begin() ; it != end() ; ++it)
    status |= (*it)->initPar2(fitparams) ;
  initMom( fitparams ) ;
  return status ;
}

std::string DecayTreeFitter::Resonance::parname(int ind) const
{
  return ParticleBase::parname(ind+4) ;
}
