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
#include "TClonesArray.h"
#include "TStopwatch.h"
// PANDA-root
#include "PndEmcMapper.h"
#include "PndEmcPoint.h"
#include "PndMCTrack.h"



int filterEvents(int pid  = 211);

#endif// End of interface definition
