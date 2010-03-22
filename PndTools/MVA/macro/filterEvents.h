/*
 * Filter Monte Carlo events for a given pdg code, if the particle has
 * been stable up to the first hit on the EMC detector.
 */
#ifndef FILTEREVENTS_H
#define FILTEREVENTS_H
// C++ includes
#include <iostream>
#include <vector>

// Root
#include "TFile.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TClonesArray.h"
#include "TStopwatch.h"

// PANDA-root
#include "PndMCTrack.h"
#include "PndEmcMapper.h"
#include "PndEmcPoint.h"
#include "PndEmcCluster.h"
#include "PndEmcXClMoments.h"
#include "PndEmcStructure.h"

#include "PndTrack.h"
#include "FairTrackParP.h"
#include "TGeoManager.h"

int filterEvents(int pid  = 211, const std::string outFileName="FilterOut.root");

#endif// End of interface definition
