// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#include "RecoResonance.h"
//#include "Event/Particle.h"
#include "FitParams.h"

using namespace DecayTreeFitter;

ClassImp(RecoResonance);

extern int vtxverbose ;

DecayTreeFitter::RecoResonance::RecoResonance(RhoCandidate* bc, const ParticleBase* aMother)
: RecoComposite(bc,aMother) {}


DecayTreeFitter::RecoResonance::~RecoResonance() {}

ErrCode
DecayTreeFitter::RecoResonance::initPar1(FitParams* fitparams)
{
  if(vtxverbose>5){std::cout<<"RecoResonance::initPar1: - "<<std::endl;}
  int posindex = posIndex() ;
  int momindex = momIndex() ;

  //quick map for parameters
  int indmap[7]  ;
  for(int i=0; i<3; ++i) indmap[i]   = posindex+i ;
  for(int i=0; i<4; ++i) indmap[i+3] = momindex+i ;

  // copy the 'measurement' -> this overwrites mother position !
  for(int row=0; row<dimM(); row++)
    fitparams->par()(indmap[row]) = m_m(row) ;
  return ErrCode::success ;
}

ErrCode
DecayTreeFitter::RecoResonance::initPar2(FitParams* /*fitparams*/)
{
  if(vtxverbose>5){std::cout<<"RecoResonance::initPar2: - "<<std::endl;}
  // nothing to do!
  return ErrCode::success ;
}

ErrCode
DecayTreeFitter::RecoResonance::projectConstraint(Constraint::Type aType,
                                 const FitParams* fitparams,
                                 Projection& p) const
{
  ErrCode aStatus ;
  switch(aType) {
    case Constraint::resonance:
      aStatus |= projectRecoComposite(fitparams,p) ;
      break ;
    default:
      aStatus |= ParticleBase::projectConstraint(aType,fitparams,p) ;
  }
  return aStatus ;
}

std::string DecayTreeFitter::RecoResonance::parname(int ind) const
{
  return ParticleBase::parname(ind+4) ;
}
