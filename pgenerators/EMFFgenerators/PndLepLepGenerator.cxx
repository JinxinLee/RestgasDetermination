//----------------------------------------------------//
//            PndLepLepGenerator source file            //
//----------------------------------------------------//

/**
 * @author Manuel Zambrana <zambrana@kph.uni-mainz.de> and Dmitry Khaneftd <khaneftd@kph.uni-mainz.de>
 *
 * Mainz, May 2011
**/

#include "PndLepLepGenerator.h"
#include "FairPrimaryGenerator.h"

// Default constructor
PndLepLepGenerator::PndLepLepGenerator()
{
}

PndLepLepGenerator::~PndLepLepGenerator()
{
}

Bool_t PndLepLepGenerator::Init()
{
  mz_pp_to_leplep_vandewi_init(seed,particleID,P,gegm,cos_theta_min,cos_theta_max);
}

Bool_t PndLepLepGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{
  mz_pp_to_leplep_vandewi_event(particleID,P,gegm,cos_theta_min,cos_theta_max,lepplus_p,lepminus_p); 

  //set the lepton mass
  if(particleID==0) PDGParticleID=11;
  if(particleID==1) PDGParticleID=13;
  if(particleID==2) PDGParticleID=15;

  primGen->AddTrack(-1*PDGParticleID, lepplus_p[1], lepplus_p[2], lepplus_p[3], 0.0, 0.0, 0.0);
  primGen->AddTrack(PDGParticleID, lepminus_p[1], lepminus_p[2], lepminus_p[3], 0.0, 0.0, 0.0);

  return kTRUE;
}

ClassImp(PndLepLepGenerator)
