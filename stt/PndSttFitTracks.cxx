// -------------------------------------------------------------------------
// -----                    PndSttFitTracks source file                -----
// -----                  Created 18/02/05  by V. Friese               -----
// -------------------------------------------------------------------------

#include "PndSttFitTracks.h"

#include "PndSttTrackFitter.h"
#include "PndSttTrack.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;
using std::string;

// -----   Default constructor   -------------------------------------------
PndSttFitTracks::PndSttFitTracks() 
{
  fFitter        = NULL;
  fTrackArray    = NULL;
  fNofTracks     = 0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndSttFitTracks::PndSttFitTracks(const char* name,
				 const char* title,
				 PndSttTrackFitter* fitter)
  : FairTask(name) 
{
  fFitter        = fitter;
  fTrackArray    = NULL;
  fNofTracks     = 0;
  fCollectionsComplete = kFALSE;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndSttFitTracks::~PndSttFitTracks() 
{ 
  fHitCollectionNames.clear();  
}
// -------------------------------------------------------------------------



// -----   Public method Init (abstract in base class)  --------------------
InitStatus PndSttFitTracks::Init() 
{
  // Check for GFTrack fitter
  if (! fFitter) 
    {
      cout << "-E- PndSttFitTracks: No track fitter selected!" << endl;
      return kERROR;
    }

  // Get and check FairRootManager
  FairRootManager
    *ioman = FairRootManager::Instance();

  if (! ioman) 
    {
      cout << "-E- PndSttFitTracks::Init: "
	   << "RootManager not instantised!" << endl;
      return kFATAL;
    }

  // Get SttTrack array
  fTrackArray  = (TClonesArray*) ioman->GetObject("STTTrack"); //=>SG
  if ( ! fTrackArray) 
    {
      cout << "-E- PndSttFitTracks::Init: No SttTrack array!"
	   << endl;
      return kERROR;
    }

  // Call the Init method of the track fitter
  fFitter->Init();
  
  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndSttFitTracks::Exec(Option_t* opt) 
{
    AddAllCollections();

  if ( !fTrackArray ) 
    return; // =>SG
  
  Int_t nTracks = fTrackArray->GetEntriesFast();

  for (Int_t iTrack=0; iTrack<nTracks; iTrack++) 
    {
      PndSttTrack* pTrack = (PndSttTrack*)fTrackArray->At(iTrack);
      fFitter->DoFit(pTrack);
    }
}
// -------------------------------------------------------------------------



// -----   Public method Finish   ------------------------------------------
void PndSttFitTracks::Finish() 
{ 
}
// -------------------------------------------------------------------------

void PndSttFitTracks::AddHitCollectionName(char *hitCollectionName)
{
    string
	newHitName(hitCollectionName);

    fHitCollectionNames.push_back(newHitName);
}

void PndSttFitTracks::AddHitCollection(char const *hitCollectionName)
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

    fFitter->AddHitCollection(fHitArray);
}

void PndSttFitTracks::AddAllCollections() 
{
    if (!fCollectionsComplete)
    {
	for (int counter = 0; counter < fHitCollectionNames.size(); counter++)
	{
	    AddHitCollection(fHitCollectionNames[counter].c_str());
	}
	fCollectionsComplete = kTRUE;
    }
}



ClassImp(PndSttFitTracks)
