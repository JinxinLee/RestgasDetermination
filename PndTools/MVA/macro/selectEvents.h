/*
 * Filter Monte Carlo events for a given pdg code, if the particle has
 * been stable up to the first hit on the EMC detector.
 */
#ifndef SELECT_EVENTS_H
#define SELECT_EVENTS_H

// C++ includes
#include <iostream>
#include <vector>
#include <limits>
#include <utility>

// Root
#include "TFile.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TClonesArray.h"
#include "TStopwatch.h"

// PANDA_Root
#include "PndMCTrack.h"
#include "PndEmcMapper.h"
#include "PndEmcPoint.h"
#include "PndEmcCluster.h"
#include "PndEmcBump.h"
#include "PndEmcXClMoments.h"
#include "PndEmcStructure.h"
#include "PndTrack.h"
#include "FairTrackParP.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "PndEmcGeoPar.h"
#include "PndEmcClusterEnergySums.h"

// 11 = e-
void selectEvents(int pdg,
		  const std::string& partName ,
		  const std::string& paramFile,
		  const std::string& simFile  ,
		  const std::string& digiFile ,
		  const std::string& recoFile ,
		  const std::string& outFileName
		  );
#endif// End of interface
