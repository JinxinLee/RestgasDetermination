#include "glpk.h"

#include "PndSecondaryTrackFinder.h"

#include "PndSttHit.h"
#include "PndSttTrack.h"
#include "PndSttPoint.h"
#include "PndSttHelixHit.h"
#include "PndSttSingleStraw.h"
#include "PndSttTube.h"
#include "PndSttMapCreator.h"

#include "PndSdsHit.h"
#include "PndSdsMCPoint.h"

#include "PndTrackCand.h"
#include "PndTrackCandHit.h"
#include "PndTrack.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairTrackParP.h"

#include "TGeoManager.h"
#include "TClonesArray.h"
#include "TGeoVolume.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TH1F.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TGeoTube.h"

#include <iostream>
#include <cmath>


using namespace std;


// -----   Default constructor   -------------------------------------------
PndSecondaryTrackFinder::PndSecondaryTrackFinder() : FairTask("STT Stt-Mvd Tracking") { 
  fPersistence = kTRUE;
  fVerbose = 0;
  sprintf(fSttBranch,"STTHit");
  sprintf(fMvdPixelBranch,"MVDHitsPixel");
  sprintf(fMvdStripBranch,"MVDHitsStrip");
}
// -------------------------------------------------------------------------

PndSecondaryTrackFinder::PndSecondaryTrackFinder(Int_t verbose) : FairTask("STT Stt-Mvd Tracking") { 
  fPersistence = kTRUE;
  fVerbose = verbose;
  sprintf(fSttBranch,"STTHit");
  sprintf(fMvdPixelBranch,"MVDHitsPixel");
  sprintf(fMvdStripBranch,"MVDHitsStrip");
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndSecondaryTrackFinder::~PndSecondaryTrackFinder() { 

}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndSecondaryTrackFinder::Init() {
  
  

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndSecondaryTrackFinder::Init: "
	 << "RootManager not instantiated, return!" << endl;
    return kFATAL;
  }
//  -----   maps of STT tubes
  // CHECK added 
  PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  fSttTubeArray = mapper->FillTubeArray();
 //----------------------------------------------------  end map




//    get   the MCTrack  array
  fMCTrackArray = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCTrackArray) 
    {
      cout << "-E- PndSecondaryTrackFinder::Init: No MCTrack array, return!"
	   << endl;
      return kERROR;
    }



 // Get SttTrackCand array  dal pattern recognition di STT
  fSttTrackCandArray  = (TClonesArray*) ioman->GetObject("STTTrackCand"); 
  if ( ! fSttTrackCandArray) 
    {
      cout << "-E- PndSecondaryTrackFinder::Init: No SttTrack Cand  array, return!"
	   << endl;
      return kERROR;
    }
	
  // Get input array   questi sono i MC point di STT
  fSttPointArray = (TClonesArray*) ioman->GetObject("STTPoint");
  if ( ! fSttPointArray ) {
    cout << "-W- PndSttHelixHitProducer::Init: "
	 << "No STTPoint array, return!" << endl;
    return kERROR;
  }

  // Get input array   hit di STT after digi
  fSttHitArray = (TClonesArray*) ioman->GetObject(fSttBranch);
//  fSttHitArray = (TClonesArray*) ioman->GetObject("STTHit");
  if ( ! fSttHitArray ) {
    cout << "-W- PndSecondaryTrackFinder::Init: "
	 << "No STTHit array, return!" << endl;
    return kERROR;
  }
  // Create and register output array for PndTrackCand of Stt+Mvd combined

  fSttMvdPndTrackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("SttMvdTrackCand","SttMvd",fSttMvdPndTrackCandArray, kTRUE);


  // Create and register output array for PndTrack of Stt+Mvd combined

  fSttMvdPndTrackArray = new TClonesArray("PndTrack");
  ioman->Register("SttMvdTrack","SttMvd",fSttMvdPndTrackArray, kTRUE);



//  -------------------------   get the Mvd hits
  fMvdPixelHitArray = (TClonesArray*) ioman->GetObject(fMvdPixelBranch);
//  fMvdPixelHitArray = (TClonesArray*) ioman->GetObject("MVDHitsPixel");
  if ( !fMvdPixelHitArray){
    std::cout << "-W- PndSecondaryTrackFinder::Init: " << "No MVD Pixel hitArray, return!" << std::endl;
    return kERROR;
  }
  fMvdStripHitArray = (TClonesArray*) ioman->GetObject(fMvdStripBranch);
//  fMvdStripHitArray = (TClonesArray*) ioman->GetObject("MVDHitsStrip");

  if ( !fMvdStripHitArray){
    std::cout << "-W- PndSecondaryTrackFinder::Init: " << "No MVD Strip hitArray, return!" << std::endl;
    return kERROR;
  }

//  -------------------------   get the Mvd track candidates

  fMvdTrackCandArray = (TClonesArray*) ioman->GetObject("MVDRiemannTrackCand");
  if ( !fMvdTrackCandArray){
    std::cout << "-W- PndSecondaryTrackFinder::Init: " << "No MVD TrackCand Array, return!" << std::endl;
    return kERROR;
  }

  cout << "-I- PndSecondaryTrackFinder: Initialization successfull" << endl;
  
//  -------------------------   get the Mvd MC points

  fMvdMCPointArray = (TClonesArray*) ioman->GetObject("MVDPoint");
  if ( !fMvdMCPointArray){
    std::cout << "-W- PndSecondaryTrackFinder::Init: " << "No MVD MC Point Array, return!" << std::endl;
    return kERROR;
  }

  cout << "-I- PndSecondaryTrackFinder: Initialization successfull" << endl;
  








  return kSUCCESS;

}

// -------------------------------------------------------------------------

// CHECK added 
void PndSecondaryTrackFinder::SetParContainers() {
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}




void PndSecondaryTrackFinder::WriteHistograms(){


}
void PndSecondaryTrackFinder::Exec(Option_t* opt) {


}


//   switch()    {
//     case FairRootManager::Instance()->GetBranchId(fMvdPixelBranch):
//       hit = (FairHit*) fMvdPixelHitArray->At(ihit);
//       break;
//     case FairRootManager::Instance()->GetBranchId(fMvdStripBranch):
//       hit = (FairHit*) fMvdStripHitArray->At(ihit);
//       break;
//     case FairRootManager::Instance()->GetBranchId(fSttBranch):
//       hit = (FairHit*) fSttHitArray->At(ihit);
//       break;
//     case FairRootManager::Instance()->GetBranchId(fGemBranch):
//       hit = (FairHit*) fGemHitArray->At(ihit);
//   }


void PndSecondaryTrackFinder::OrderHits(TClonesArray *hitarray, Int_t *sorthits)
{

  std::vector<double> distances;
 std::multimap<double, int> mapdistances;

  for(int ihit = 0; ihit < hitarray->GetEntriesFast(); ihit++) {
    FairHit* hit = (FairHit*) hitarray->At(ihit);
    if(!hit) continue;
    
    TVector3 distance3;
    hit->Position(distance3);
    double distance = distance3.Perp();

    distances.push_back(distance);
    mapdistances.insert(std::pair<double, int>(distance, ihit));
 }

  std::sort(distances.begin(), distances.end());

  std::sort(distances.begin(), distances.end());

  double tmpdistance = 0;
  int counter = 0;
  for(int j = 0; j < distances.size(); j++) {
    double d = distances[j];

    if(tmpdistance < d) tmpdistance = d;
    else continue;

    std::multimap<double, int>::iterator it;
    int count = 0;
    int n = mapdistances.count(tmpdistance);

    for(it = mapdistances.begin(); it != mapdistances.end(); ++it)
      {
	if(count == n) break;
	if((*it).first != tmpdistance) continue;

	sorthits[counter] = (*it).second;
	count++;
   	counter++;
     }
  }
}




ClassImp(PndSecondaryTrackFinder)
