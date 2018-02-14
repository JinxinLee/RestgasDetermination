/*
 *=====================================================
 *
 *  CBM Level 1 Reconstruction
 *
 *  Authors: M.Zyzak
 *
 *  e-mail :
 *
 *=====================================================
 *
 *  Finds Particles: Lambdas, K0
 *
 */

#include "KFMCParticle.h"


KFMCParticle::KFMCParticle() :fDaughterIds(), fMCTrackID(-1), fMotherId(-1), fPDG(0), fIsReconstructable(0)
{
}

KFMCParticle::~KFMCParticle()
{
}

void KFMCParticle::AddDaughter( int i )
{
  fDaughterIds.push_back(i);
}
