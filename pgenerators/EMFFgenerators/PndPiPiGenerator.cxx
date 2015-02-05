//----------------------------------------------------//
//            PndPiPiGenerator source file            //
//----------------------------------------------------//

/**
 * @author Manuel Zambrana <zambrana@kph.uni-mainz.de> and Dmitry Khaneftd <khaneftd@kph.uni-mainz.de>
 *
 * Mainz, May 2011
**/

#include "PndPiPiGenerator.h"
#include "FairPrimaryGenerator.h"

// Default constructor
PndPiPiGenerator::PndPiPiGenerator() : P(0), seed(0), cos_theta_min(0), cos_theta_max(1) 
{
}

PndPiPiGenerator::~PndPiPiGenerator()
{
}

Bool_t PndPiPiGenerator::Init()
{
  mz_pp_to_pipi_vandewi_init(seed,P,cos_theta_min,cos_theta_max);
}

Bool_t PndPiPiGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{
  mz_pp_to_pipi_vandewi_event(P,cos_theta_min,cos_theta_max,piplus_p,piminus_p); 

  primGen->AddTrack(211, piplus_p[1], piplus_p[2], piplus_p[3], 0.0, 0.0, 0.0);
  primGen->AddTrack(-211, piminus_p[1], piminus_p[2], piminus_p[3], 0.0, 0.0, 0.0);

  return kTRUE;
}

ClassImp(PndPiPiGenerator)
