/* $Id: */

// -------------------------------------------------------------------------
// -----                    PndGemEventBuilderOnTracks source file         -----
// -----                  Created 22/08/2013 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** PndGemEventBuilderOnTracks
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 22.08.2013
 *@version 1.0
 **
 ** PANDA task class for event building basing on reconstructed GEM tracks
 ** Task level RECO
 **/

#include "PndGemEventBuilderOnTracks.h"

#include "PndTrack.h"

#include "PndDetectorList.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom2.h"

#include <iomanip>

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::fixed;
using std::right;
using std::left;
using std::setw;
using std::setprecision;
using std::set;
using std::map;

using std::sort;

bool CompareRecoEvents(RecoEvent ev1, RecoEvent ev2) { return (ev1.meanTime<ev2.meanTime); }

// -----   Default constructor   ------------------------------------------
PndGemEventBuilderOnTracks::PndGemEventBuilderOnTracks()
  : FairTask("GEMEventBuilder", 0),
    fGemTracks     (NULL),
    fTNofEvents    (0),
    fTNofTracks    (0),
    fTNofRecoEvents(0),
    fExecTime      (0.)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndGemEventBuilderOnTracks::PndGemEventBuilderOnTracks(Int_t iVerbose) 
  : FairTask("GEMEventBuilder", iVerbose),
    fGemTracks     (NULL),
    fTNofEvents    (0),
    fTNofTracks    (0),
    fTNofRecoEvents(0),
    fExecTime      (0.)
{
}
// -------------------------------------------------------------------------



// -----   Constructor with name   -----------------------------------------
PndGemEventBuilderOnTracks::PndGemEventBuilderOnTracks(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose),
    fGemTracks     (NULL),
    fTNofEvents    (0),
    fTNofTracks    (0),
    fTNofRecoEvents(0),
    fExecTime      (0.)
{
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndGemEventBuilderOnTracks::~PndGemEventBuilderOnTracks() {
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndGemEventBuilderOnTracks::Exec(Option_t* opt) {
  //  cout << endl << "======== PndGemEventBuilderOnTracks::Exec(Event = " << fTNofEvents << " ) ====================" << endl;
  fTimer.Start();

  fTNofTracks += fGemTracks->GetEntries();

  PndTrack* tempTrack = NULL;

  for ( Int_t itrack = 0 ; itrack < fGemTracks->GetEntries() ; itrack++ ) {
    tempTrack = (PndTrack*)fGemTracks->At(itrack);
    //    cout << "    " << fTNofEvents << " . " << itrack << " @ " << tempTrack->GetTimeStamp() << "ns, trying to match to one of " << fRecoEvents.size() << " reco events" << endl;

    Int_t revNo = CompareTrackToPreviousEvents(tempTrack);

    //    cout << "    ----> matched to " << revNo << endl;
  }

  fTNofEvents += 1;

  fExecTime += fTimer.RealTime();
  fTimer.Stop();  

}
// -------------------------------------------------------------------------

// -----   Private method CompareTrackToPreviousEvents   -------------------
Int_t PndGemEventBuilderOnTracks::CompareTrackToPreviousEvents(PndTrack* tempTrack) {
  Double_t trackTime = tempTrack->GetTimeStamp();
  Int_t    recoEvent = -1;
  for ( Int_t irev = 0 ; irev < fRecoEvents.size() ; irev++ ) {	
    RecoEvent iterREV = fRecoEvents[irev];

    // SHOULD BE SOMETHING MORE SERIOUS THAN "2."
    // 26.08 - just checked the reconstructed track time difference,
    // got a distribution with sigma of 9.49795e-01
    // Setting to 3.0
    if ( TMath::Abs(trackTime-iterREV.meanTime) < 3. ) { 
      if ( recoEvent != -1 ) {
	//	cout << "track @ " << trackTime << " matches to two reco events: " << irev << " and " << recoEvent << endl;
      }
      else {
	recoEvent = irev;
      }
    }
  }

  // matching event found
  if ( recoEvent != -1 ) { 
    //    cout << "adding track @ " << trackTime << " to event @ " << fRecoEvents[recoEvent].meanTime << flush;
    fRecoEvents[recoEvent].meanTime = (fRecoEvents[recoEvent].meanTime*fRecoEvents[recoEvent].nofTracks+trackTime)/((Double_t)(fRecoEvents[recoEvent].nofTracks+1));
    //    cout << " to get event @ " << fRecoEvents[recoEvent].meanTime << " (" << fRecoEvents[recoEvent].nofTracks << ")" << endl;
    fRecoEvents[recoEvent].nofTracks = fRecoEvents[recoEvent].nofTracks+1;
  }
  else {
    RecoEvent tempREV;
    tempREV.meanTime  = trackTime;
    tempREV.nofTracks = 1;

    fRecoEvents.push_back(tempREV);
  }

  return recoEvent;
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void PndGemEventBuilderOnTracks::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");
}
// -------------------------------------------------------------------------




// -----   Private method Init   -------------------------------------------
InitStatus PndGemEventBuilderOnTracks::Init() {

  // Get input array
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");

  fGemTracks = (TClonesArray*) ioman->GetObject("GEMTrack");

  return kSUCCESS;
}
// -------------------------------------------------------------------------




// -----   Private method ReInit   -----------------------------------------
InitStatus PndGemEventBuilderOnTracks::ReInit() {

  // Create sectorwise digi sets
  //  MakeSets();

  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Finish   ------------------------------------------
void PndGemEventBuilderOnTracks::Finish() {
  fTNofRecoEvents = fRecoEvents.size();

  std::sort(fRecoEvents.begin(),fRecoEvents.end(),CompareRecoEvents);

  for ( Int_t irev = 0 ; irev < fTNofRecoEvents ; irev++ ) {
    cout << "reco Event " << irev << " at " << fRecoEvents[irev].meanTime << "ns has " << fRecoEvents[irev].nofTracks << " tracks." << endl;
  }

  cout << "-------------------- " << fName.Data() << " : Summary -----------------------" << endl;
  cout << " Events:        " << setw(10) << fTNofEvents     << endl;
  cout << " Tracks:        " << setw(10) << fTNofTracks     << "    ( " << (Double_t)fTNofTracks   /((Double_t)fTNofEvents) << " per event )" << endl;
  cout << " Reco Events:   " << setw(10) << fTNofRecoEvents << endl;
  cout << "---------------------------------------------------------------------" << endl; 
  cout << " >>> EB >>> all   time  = " << fExecTime << "s" << endl;
  cout << "---------------------------------------------------------------------" << endl; 
}
// -------------------------------------------------------------------------


ClassImp(PndGemEventBuilderOnTracks)

