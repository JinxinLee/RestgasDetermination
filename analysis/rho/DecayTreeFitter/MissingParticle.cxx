// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
/**
 * FIXME
 To finish the implementation, this is what we need to do: We would like to make the dimension of the missing particle depend on
 the mass constraint. That means that any call to 'setMassConstraint' should invalidate the 'indices'. The creation of indices is
 done with 'DecayChain::updateIndex'. We could call this from 'initPar' (or from any other place that initialized the
 constraints. That also means that 'setMassConstraint' should change Fitter.m_fitStatus to Unfitted'. Finally, to make sure that
 we don't break anything, we need a test case.

 */
#include "TLorentzVector.h"
#include "FitParams.h"
#include "MissingParticle.h"
//#include "Event/Particle.h"

using namespace DecayTreeFitter;

ClassImp(MissingParticle);

extern int vtxverbose ;

DecayTreeFitter::MissingParticle::MissingParticle(RhoCandidate* bc, const ParticleBase* aMother)
: ParticleBase(bc,aMother)
{
  // this will be one of the very few particles for which we adjust
  // the dimension if there is a constraint
}

DecayTreeFitter::MissingParticle::~MissingParticle() {}

ErrCode DecayTreeFitter::MissingParticle::initPar1(FitParams* fitpar)
{
  if(vtxverbose>5){std::cout<<"MissingParticle::initPar1: - "<<std::endl;}
  // take them from the bc
  TLorentzVector p4 = particle()->P4() ;
  int momindex = momIndex();
  fitpar->par()(momindex+0) = p4.Px() ;
  fitpar->par()(momindex+1) = p4.Py() ;
  fitpar->par()(momindex+2) = p4.Pz() ;
  if(hasEnergy()) fitpar->par()(momindex+3) = p4.E() ;
  return ErrCode() ;
}

std::string DecayTreeFitter::MissingParticle::parname(int ind) const
{
  return ParticleBase::parname(ind+4) ;
}
