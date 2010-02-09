#include "PndDetectorList.h"
#include "PndPidCorrelator.h"
#include "PndPidCandidate.h"
#include "PndTrack.h"
#include "PndTrackID.h"

#include "FairTrackParH.h"
#include "FairMCApplication.h"
#include "FairRunAna.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"

#include "TObjArray.h"
#include "TVector3.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"

#include <cmath>

#include "PndPidCorrelator.h"

//_________________________________________________________________
Bool_t PndPidCorrelator::GetTpcInfo(PndTrack* track, PndPidCandidate* pidCand) {
  // TO BE IMPLEMENTED 
}

ClassImp(PndPidCorrelator)
