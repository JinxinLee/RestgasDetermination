// -------------------------------------------------------------------------
// -----                   PndSttFindTracks source file                -----
// -------------------------------------------------------------------------

#include "PndSttFindTracks.h"

#include "PndSttTrack.h"
#include "PndSttTrackFinder.h"

#include "FairHit.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;
using std::string;

// -----   Default constructor   -------------------------------------------
PndSttFindTracks::PndSttFindTracks() 
  : FairTask("STT Find GFTracks") 
{
  fFinder      = NULL;
  fTrackArray  = NULL;
  fNofTracks   = 0;
  fVerbose     = 1;
  fCollectionsComplete = kFALSE;
  fPersistence = kTRUE;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndSttFindTracks::PndSttFindTracks(PndSttTrackFinder* finder, 
				   Int_t verbose)
  : FairTask("STT Find GFTracks") 
{
  fFinder      = finder;
  fTrackArray  = NULL;
  fNofTracks   = 0;
  fVerbose     = verbose;
  fCollectionsComplete = kFALSE;
  fPersistence = kTRUE;
}
// -------------------------------------------------------------------------



// -----   Constructor with name and title   -------------------------------
PndSttFindTracks::PndSttFindTracks(const char* name, const char* title, 
				   PndSttTrackFinder* finder,
				   Int_t verbose) 
  : FairTask(name) 
{
  fFinder      = finder;
  fTrackArray  = NULL;
  fNofTracks   = 0;
  fVerbose     = verbose;
  fCollectionsComplete = kFALSE;
  fPersistence = kTRUE;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndSttFindTracks::~PndSttFindTracks() 
{
  fTrackArray->Delete();
  fHitCollectionNames.clear();  
  fPointCollectionNames.clear();
}
// -------------------------------------------------------------------------



// -----   Public method Init (abstract in base class)  --------------------
InitStatus PndSttFindTracks::Init() 
{
  fEventCounter = 0;

  // Check for GFTrack finder
  if (! fFinder) {
    cout << "-E- PndSttFindTracks::Init: No track finder selected!" << endl;
    return kERROR;
  }

  FairRootManager
      *ioman = FairRootManager::Instance();
  
  if (!ioman) 
  {
      cout << "-E- PndSttFindTracks::AddHitCollection: "
	   << "RootManager not instantised!" << endl;
      return kFATAL;
  }

  // Create and register SttTrack array
  fTrackArray = new TClonesArray("PndSttTrack",100);
  ioman->Register("STTTrack", "STT", fTrackArray, fPersistence);
  
  // Set verbosity of track finder
  fFinder->SetVerbose(fVerbose);
  
  // Call the Init method of the track finder
  fFinder->Init();

  return kSUCCESS;
}

// -------------------------------------------------------------------------
void PndSttFindTracks::AddHitCollectionName(char *hitCollectionName, char *pointCollectionName)
{
    string
	newPointName(pointCollectionName),
	newHitName(hitCollectionName);

    fHitCollectionNames.push_back(newHitName);
    fPointCollectionNames.push_back(newPointName);
}

void PndSttFindTracks::AddHitCollection(char const *hitCollectionName, char const *pointCollectionName)
{
    // Get and check FairRootManager
    FairRootManager
	*ioman = FairRootManager::Instance();
    
    if (!ioman) 
    {
	cout << "-E- PndSttFindTracks::AddHitCollection: "
	     << "RootManager not instantised!" << endl;
    }
    
    // Get hit Array
    TClonesArray
	*fHitArray = (TClonesArray*) ioman->GetObject(hitCollectionName);

    if (!fHitArray) 
    {
	cout << "-W- PndSttFindTracks::AddHitCollection: No " << hitCollectionName << " STT hit array!"
	     << endl;
    }

    // Get point Array
    TClonesArray
	*fPointArray = (TClonesArray*) ioman->GetObject(pointCollectionName);

    if (!fPointArray) 
    {
	cout << "-W- PndSttFindTracks::AddHitCollection: No " << pointCollectionName << " STT hit array!"
	     << endl;
    }

    fFinder->AddHitCollection(fHitArray, fPointArray);
}

void PndSttFindTracks::AddAllCollections() 
{
    if (!fCollectionsComplete)
    {
	for (int counter = 0; counter < fHitCollectionNames.size(); counter++)
	{
	    AddHitCollection(fHitCollectionNames[counter].c_str(), fPointCollectionNames[counter].c_str());
	}
	fCollectionsComplete = kTRUE;
    }
}

// -----   Public method Exec   --------------------------------------------
void PndSttFindTracks::Exec(Option_t* opt) 
{
  if(fVerbose) cout << "******* event # " << fEventCounter << "*******" << endl;
  fEventCounter++;
  
  AddAllCollections();
  
  fTrackArray->Clear();
  
  fNofTracks = fFinder->DoFind(fTrackArray);
  
  for (Int_t iTrack=0; iTrack < fTrackArray->GetEntriesFast(); iTrack++) 
    {
      PndSttTrack* track = (PndSttTrack*) fTrackArray->At(iTrack);
      track->SortHits();
    }
}
// -------------------------------------------------------------------------



// -----   Public method Finish   ------------------------------------------
void PndSttFindTracks::Finish() 
{
  fFinder->Finish();
}
// -------------------------------------------------------------------------




ClassImp(PndSttFindTracks)
