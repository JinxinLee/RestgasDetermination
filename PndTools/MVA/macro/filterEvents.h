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
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "PndEmcGeoPar.h"

// 11 = e-
void filterEvents(int pdg = 11, const std::string& partName = "Particle",
		  const std::string& paramFile = "params.root",
		  const std::string& simFile   = "simfile.root",
		  const std::string& digiFile  = "digifile.root",
		  const std::string& recoFile  = "recofile.root",
		  const std::string& outFileName = "FilterOut.root"
		  );

void mergeTreeFiles(const std::string& outFile = "MergeOutput.root");
#endif// End of interface definition
