// C/C++ includes
#include <iostream.h>
#include <iomanip>
#include <set>
#include <vector.h>
#include <iterator>
#include <memory>

// Root includes
#include "TClonesArray.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"

//PANDA include
#include "PndEmcDigi.h"
#include "PndTofPoint.h"
#include "PndMvdMCPoint.h"
#include "PndTpcPoint.h"
#include "PndTofHit.h"
#include "PndEmcCluster.h"
#include "PndEmcMapper.h"
#include "PndEmcTwoCoordIndex.h"
#include "PndPidCand.h"
#include "CbmMCTrack.h"
#include "FairMCPoint.h"
#include "FairTrackParH.h"


void fillTree(TString const points, TString const tracks, 
	      Int_t const pdg_code, 
	      TTree& outTree);
