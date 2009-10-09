// -------------------------------------------------------------------------
// -----                  CbmStsMatchTracks source file                -----
// -----                  Created 24/11/05  by V. Friese               -----
// -------------------------------------------------------------------------


#include <iostream>

#include "TClonesArray.h"

#include "FairMCPoint.h"
#include "FairRootManager.h"
#include "PndSttMatchTracks.h"
#include "PndSttHit.h"
#include "PndSttTrack.h"
#include "PndSttTrackMatch.h"
#include <vector>
using namespace std;

// -----   Default constructor   -------------------------------------------
PndSttMatchTracks::PndSttMatchTracks() 
  : FairTask("STT track match") {
  fTracks     = NULL;
  fMatches    = NULL;
  fVerbose    = 1;
  fCollectionsComplete = kFALSE;
}
// -------------------------------------------------------------------------



// -----   Constructor with verbosity level   ------------------------------
PndSttMatchTracks::PndSttMatchTracks(Int_t verbose) 
  : FairTask("STT track match") {
  fTracks     = NULL;
  fMatches    = NULL;
  fVerbose    = verbose;
  fCollectionsComplete = kFALSE;
}
// -------------------------------------------------------------------------



// -----   Constructor with name, title and verbosity  ---------------------
PndSttMatchTracks::PndSttMatchTracks(const char* name, const char* title,
				     Int_t verbose) 
  : FairTask(name) {
  fTracks     = NULL;
  fMatches    = NULL;
  fVerbose    = verbose;
  fCollectionsComplete = kFALSE;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndSttMatchTracks::~PndSttMatchTracks() 
{
  fHitCollectionNames.clear();  
  fPointCollectionNames.clear();
}
// -------------------------------------------------------------------------




// -----   Public method Init   --------------------------------------------
InitStatus PndSttMatchTracks::Init() {
  
  // Get FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- PndSttMatchTracks::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get SttTrack Array
  fTracks = (TClonesArray*) ioman->GetObject("STTTrack");
  if ( ! fTracks ) {
    cout << "-E- PndSttMatchTracks::Init: No SttTrack array!" << endl;
    return kERROR;
  }

  // Create and register SttTrackMatch array
  fMatches = new TClonesArray("PndSttTrackMatch",100);
  ioman->Register("STTTrackMatch", "STT", fMatches, kTRUE);

  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndSttMatchTracks::Exec(Option_t* opt) 
{
  AddAllCollections();
  
  if (fHitCollectionList.GetEntries() == 0)
  {
      cout << "-E- PndSttTrackFinderIdeal::DoFind: "
	   << "No hit arrays present, call AddHitCollection() first (at least once)! " << endl;
  }

  if (fPointCollectionList.GetEntries() == 0)
  {
      cout << "-E- PndSttTrackFinderIdeal::DoFind: "
	   << "No point arrays present, call AddHitCollection() first (at least once)! " << endl;
  }

  // Clear output array
  fMatches->Delete();

  // Create some pointers and variables
  PndSttTrack*      track = NULL;
  PndSttHit*        mHit  = NULL;
  FairMCPoint*       point = NULL;

  Int_t nHits    = 0;
  Int_t nMCTracks = 0;
  Int_t iPoint    = 0;
  Int_t iFlag     = 0;
  Int_t iMCTrack  = 0;
  Int_t nAll      = 0;
  Int_t nTrue     = 0;
  Int_t nWrong    = 0;
  Int_t nFake     = 0;
  Int_t nHitSum     = 0;
  Int_t nTrueSum    = 0;
  Int_t nWrongSum   = 0;
  Int_t nFakeSum    = 0;
  Int_t nMCTrackSum = 0;

   // Loop over SttTracks
  Int_t nTracks = fTracks->GetEntriesFast();

  for (Int_t iTrack=0; iTrack<nTracks; iTrack++) 
    {
      track = (PndSttTrack*) fTracks->At(iTrack);
      if ( ! track) 
	{
	  cout << "-W- PndSttMatchTracks::Exec: Empty SttTrack at " 
	       << iTrack << endl;
	  continue;
	}
      
      nHits = track->GetNofHits();

      nAll = nTrue = nWrong = nFake = nMCTracks = 0;

      fMatchMap.clear();
      if (fVerbose > 2) cout << endl << "Track " << iTrack << ", Hits " 
			     << nHits << endl;

      // Loop over Hits of track
      for (Int_t iMHit=0; iMHit<nHits; iMHit++) 
	{
	    // alter here
	  mHit = GetHitFromCollections(track->GetHitIndex(iMHit));

	  if ( ! mHit ) 
	    {
	      cout << "-E- PndSttMatchTracks::Exec: "
		   << "No Hit " << iMHit << " for track " << iTrack << endl;
	      continue;
	    }

	  iPoint = mHit->GetRefIndex();
	  
	  if ( iPoint < 0 ) 
	    {        
	      nFake++;
	      continue;
	    }
	  
	  // alter here
	  point = GetPointFromCollections(track->GetHitIndex(iMHit));
	  
	  if ( ! point ) 
	    {
	      cout << "-E- PndSttMatchTracks::Exec: "
		   << "Empty MCPoint " << iPoint << " from Hit " << iMHit
		   << " (track " << iTrack << ")" << endl;
	      continue;
	    }
	  
	  iMCTrack = point->GetTrackID();
	  
	  if ( fVerbose > 2 ) cout << "Track " << iTrack << ", hit "
				   << track->GetHitIndex(iMHit) 
				   << ", STTPoint " << iPoint << ", MCTrack "
				   << iMCTrack << endl;
	  fMatchMap[iMCTrack]++;
	}
    
      // Search for best matching MCTrack
      iMCTrack = -1;

      map<Int_t, Int_t>::iterator it;
      for (it=fMatchMap.begin(); it!=fMatchMap.end(); it++) 
	{
	  if (fVerbose > 2) cout << it->second 
				 << " common points wth MCtrack " 
				 << it->first << endl;
	  nMCTracks++;
	  nAll += it->second;
	  if ( it->second > nTrue ) 
	    {
	      iMCTrack = it->first;
	      nTrue    = it->second;	
	    }
	}
    
      nWrong = nAll - nTrue;
      if (fVerbose>1) cout << "-I- PndSttMatchTracks: STTTrack " << iTrack 
			   << ", MCTrack " << iMCTrack << ", true " 
			   << nTrue << ", wrong " << nWrong << ", fake " 
			   << nFake << ", #MCTracks " << nMCTracks << endl;
      
      // Create SttTrackMatch
      new ((*fMatches)[iTrack]) PndSttTrackMatch(iMCTrack, nTrue, 
						 nWrong, nFake, 
						 nMCTracks);
      
      // Some statistics
      nHitSum     += nHits;
      nTrueSum    += nTrue;
      nWrongSum   += nWrong;
      nFakeSum    += nFake;
      nMCTrackSum += nMCTracks;

    } // GFTrack loop

  // Event statistics

  Double_t qTrue = 0.;
  if ( nHitSum) 
      qTrue  = Double_t(nTrueSum)  / Double_t(nHitSum) * 100.;

  if (fVerbose) 
  {
      Double_t 
	  qWrong = 0.,
	  qFake = 0.,
	  qMC = 0.;
      
      if (nHitSum)
      {
	  qWrong = Double_t(nWrongSum) / Double_t(nHitSum) * 100.;
	  qFake  = Double_t(nFakeSum)  / Double_t(nHitSum) * 100.;
      }
      if (nTracks)
	  qMC = Double_t(nMCTrackSum) / Double_t(nTracks);

    cout << endl;
    cout << "-------------------------------------------------------" 
	 << endl;
    cout << "-I-              STT GFTrack Matching                 -I-"
	 << endl;
    cout << "Reconstructed STTTracks : " << nTracks << endl;;
    cout << "True  hit assignments   : " << qTrue  << " %" << endl;
    cout << "Wrong hit assignments   : " << qWrong << " %" << endl;
    cout << "Fake  hit assignments   : " << qFake  << " %" << endl;
    cout << "MCTracks per STTTrack   : " << qMC << endl;
    cout << "--------------------------------------------------------" 
	 << endl;
  }
  else cout << "-I- PndSttMatchTracks: rec. " << nTracks << ", quota "
	    << qTrue << " % " << endl;

}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void PndSttMatchTracks::AddHitCollectionName(char *hitCollectionName, char *pointCollectionName)
{
    string
	newPointName(pointCollectionName),
	newHitName(hitCollectionName);

    fHitCollectionNames.push_back(newHitName);
    fPointCollectionNames.push_back(newPointName);
}

void PndSttMatchTracks::AddHitCollection(char const *hitCollectionName, char const *pointCollectionName)
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

    fHitCollectionList.Add(fHitArray); 
    fPointCollectionList.Add(fPointArray);
}

void PndSttMatchTracks::AddAllCollections() 
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

PndSttHit* PndSttMatchTracks::GetHitFromCollections(Int_t hitCounter)
{
    PndSttHit
	*retval = NULL;
 
    Int_t
	relativeCounter = hitCounter;

    for (Int_t collectionCounter = 0; collectionCounter < fHitCollectionList.GetEntries(); collectionCounter++)
    {
	Int_t
	    size = ((TClonesArray *)fHitCollectionList.At(collectionCounter))->GetEntriesFast();

	if (relativeCounter < size)
	{
	    retval = (PndSttHit*) ((TClonesArray *)fHitCollectionList.At(collectionCounter))->At(relativeCounter);
	    break;
	}
	else
	{
	    relativeCounter -= size;
	}
    }
    return retval;
}

FairMCPoint* PndSttMatchTracks::GetPointFromCollections(Int_t hitCounter)
{
    FairMCPoint
	*retval = NULL;
 
    Int_t
	relativeCounter = hitCounter;

    for (Int_t collectionCounter = 0; collectionCounter < fHitCollectionList.GetEntries(); collectionCounter++)
    {
	Int_t
	    size = ((TClonesArray *)fHitCollectionList.At(collectionCounter))->GetEntriesFast();

	if (relativeCounter < size)
	{
	    Int_t
		tmpHit = ((PndSttHit*) ((TClonesArray *)fHitCollectionList.At(collectionCounter))->At(relativeCounter))->GetRefIndex();
	    
	    retval = (FairMCPoint*) ((TClonesArray *)fPointCollectionList.At(collectionCounter))->At(tmpHit);
	    
	    break;
	}
	else
	{
	    relativeCounter -= size;
	}
    }
    return retval;
}

// -----   Public method Finish   ------------------------------------------
void PndSttMatchTracks::Finish() { }
// -------------------------------------------------------------------------


    
ClassImp(PndSttMatchTracks)


      

      
