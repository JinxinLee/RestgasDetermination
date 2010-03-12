// -------------------------------------------------------------------------
// -----                   PndSttFindTracks source file                -----
// -------------------------------------------------------------------------

#include "PndSttFindTracks.h"

#include "PndSttTrackFinder.h"
#include "PndTrackCand.h"
#include "PndSttHelixHit.h"
#include "FairHit.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;
using std::string;

// -----   Default constructor   -------------------------------------------
PndSttFindTracks::PndSttFindTracks() 
  : FairTask("STT Find Tracks") 
{
  fFinder      = NULL;
  fTrackCandArray  = NULL; 
  fHelixHitArray  = NULL; 
  fNofTracks   = 0;
  fVerbose     = 1;
  fCollectionsComplete = kFALSE;
  fPersistence = kTRUE;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndSttFindTracks::PndSttFindTracks(PndSttTrackFinder* finder, 
				   Int_t verbose)
  : FairTask("STT Find Tracks") 
{
  fFinder      = finder;
  fTrackCandArray  = NULL; 
  fHelixHitArray  = NULL; 
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
  fTrackCandArray  = NULL; 
  fHelixHitArray  = NULL; 
  fNofTracks   = 0;
  fVerbose     = verbose;
  fCollectionsComplete = kFALSE;
  fPersistence = kTRUE;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndSttFindTracks::~PndSttFindTracks() 
{
  fTrackCandArray->Delete(); 
  fHelixHitArray ->Delete();
  fHitCollectionNames.clear();  
  fPointCollectionNames.clear();
}
// -------------------------------------------------------------------------



// -----   Public method Init (abstract in base class)  --------------------
InitStatus PndSttFindTracks::Init() 
{
  fEventCounter = 0;

  // Check for Track finder
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

  fTrackCandArray = new TClonesArray("PndTrackCand",100); 
  ioman->Register("STTTrackCand", "STT", fTrackCandArray, fPersistence); 
  
  fHelixHitArray = new TClonesArray("PndSttHelixHit",100); 
  ioman->Register("STTPRHelixHit", "STT", fHelixHitArray, fPersistence); 
  

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
  // CHECK !!! IMPORTANT: event 0 (the first) 

  if(fVerbose) cout << "******* event # " << fEventCounter << "*******" << endl;
  fEventCounter++;
  
  AddAllCollections();
  
  fTrackCandArray->Clear(); 
  fHelixHitArray->Clear(); 

  
  fNofTracks = fFinder->DoFind(fTrackCandArray, fHelixHitArray); 
  
  
  for (Int_t iTrack=0; iTrack < fTrackCandArray->GetEntriesFast(); iTrack++) 
    { 
      PndTrackCand* trackCand = (PndTrackCand*) fTrackCandArray->At(iTrack);
      trackCand->Sort();
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
