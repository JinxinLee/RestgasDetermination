#include "PndMCTrackEnumAssociator.h"

#include "PndTrack.h"
#include "PndTrackID.h"

#include "FairRunAna.h"
#include "PndSttPoint.h"
#include "PndSttHit.h"
#include "PndSdsMCPoint.h"
#include "PndSdsHit.h"
#include "PndSdsCluster.h"
#include "PndSdsDigiPixel.h"
#include "PndSdsDigiStrip.h"
#include "PndEmcCluster.h"
#include "PndEmcBump.h"
#include "PndGemMCPoint.h"
#include "PndGemHit.h"
#include "PndFtsPoint.h"
#include "PndFtsHit.h"
#include "PndMCTrack.h"
#include "PndDetectorList.h"
#include "FairMCApplication.h"
#include "FairRootManager.h"
#include "FairTask.h"


#include <iomanip>

//_________________________________________________________________
PndMCTrackEnumAssociator::PndMCTrackEnumAssociator() 
{
  //---
  //fTrackArray = new TClonesArray("PndTrack");
  fTrackIDArray = new TClonesArray("PndTrackID");
  fTrackInBranchName  = ""; 
  fTrackOutBranchName = "";
  fPersistence   = kTRUE;
  fVerbose       = kFALSE;
}

//_________________________________________________________________
PndMCTrackEnumAssociator::PndMCTrackEnumAssociator(const char *name,
                                           const char *title):FairTask(name)
{
  //---
  //fTrackArray = new TClonesArray("PndTrack");
  fTrackIDArray = new TClonesArray("PndTrackID");
  fTrackInBranchName  = ""; 
  fTrackOutBranchName = "";
  fPersistence   = kTRUE;
  fVerbose       = kFALSE;
}

//_________________________________________________________________
PndMCTrackEnumAssociator::~PndMCTrackEnumAssociator()
{
}

//_________________________________________________________________
InitStatus PndMCTrackEnumAssociator::Init() {
  
  //  cout << "InitStatus PndMCTrackEnumAssociator::Init\n\n";
  cout << "-I- PndMCTrackEnumAssociator::Init" << endl;
  
  FairRootManager *fManager =FairRootManager::Instance();	
  
  fTrackArray = (TClonesArray *)fManager->GetObject(fTrackInBranchName);
  if ( ! fTrackArray ) {
    cout << "-I-  PndMCTrackEnumAssociator::Init: No PndTrack array!" << endl;
    return kERROR;
  }
  
  
  fSttHitInput   = (TClonesArray *)fManager->GetObject("STTHit");
  if ( ! fSttHitInput ) 
  {
    cout << "-W- PndMCTrackEnumAssociator::Init: No STTHit array" << endl;
  }
  
  fMvdMCArray = (TClonesArray*) fManager->GetObject("MVDPoint");
  if ( ! fMvdMCArray )
  {
    cout << "-W-  PndMCTrackEnumAssociator::Init: No MVDPoint array" << endl;
  }
  
  fMvdStripHitArray = (TClonesArray*) fManager->GetObject("MVDHitsStrip");
  if ( !fMvdStripHitArray)
  {
    cout << "-W- PndMCTrackEnumAssociator::Init: No MVDHitsStrip array" << endl;
  }
  
  fMvdPixelHitArray = (TClonesArray*) fManager->GetObject("MVDHitsPixel");
  if ( !fMvdPixelHitArray)
  {
    cout << "-W- PndMCTrackEnumAssociator::Init: " << "No fMvdPixelHitArray" << endl;
  }
  
  fGemInput   = (TClonesArray *)fManager->GetObject("GEMHit");
  if ( ! fGemInput ) 
  {
    cout << "-W- PndMCTrackEnumAssociator::Init: No GEMHit array" << endl;
  }
  
  fGemMCArray = (TClonesArray*) fManager->GetObject("GEMPoint");
  if ( ! fGemMCArray )
  {
    cout << "-W- PndMCTrackEnumAssociator::Init: No GEMPoint array" << endl;
  }
  
  fFtsInput   = (TClonesArray *)fManager->GetObject("FTSHit");
  if ( ! fFtsInput ) 
  {
    cout << "-W- PndMCTrackEnumAssociator::Init: No FTSHit array" << endl;
  }
  
  fFtsMCArray = (TClonesArray*) fManager->GetObject("FTSPoint");
  if ( ! fFtsMCArray )
  {
    cout << "-W- PndMCTrackEnumAssociator::Init: No FTSPoint array" << endl;
  }
  
  if (fVerbose)   cout << "-I- PndMCTrackEnumAssociator::Init: Verbose Mode ON" << endl;
  
  Register();
  
  return kSUCCESS;
}


//_________________________________________________________________
Int_t PndMCTrackEnumAssociator::GetMvdHitPixels(Int_t index) {
  // Taking points from PndMvdHitPixel
  
  PndSdsHit* hit = (PndSdsHit*) fMvdPixelHitArray->At(index);
  if (!hit)
  {
    cout << "-E- PndMCTrackEnumAssociator::GetMvdHitPixels: MvdHitPixel #" << index << " does not exist!!!" << endl;
    return -1;
  }
  if (hit->GetRefIndex()!=-1)
  {
    PndSdsMCPoint* myPoint = (PndSdsMCPoint*)(fMvdMCArray->At(hit->GetRefIndex()));
    if (!myPoint)
    {
      cout << "-E- PndMCTrackEnumAssociator::GetMvdHitPixels: MvdMCPoint #" << hit->GetRefIndex() << " does not exist!!!" << endl;
      return -1;
    }
    return (myPoint->GetTrackID());
  }
  else
  {
    return -1;
  }
}

//_________________________________________________________________
Int_t PndMCTrackEnumAssociator::GetMvdHitStrips(Int_t index)
{
  // Taking points from PndMvdHitStrip
  
  PndSdsHit* hit = (PndSdsHit*) fMvdStripHitArray->At(index);
  if (!hit)
  {
    cout << "-E- PndMCTrackEnumAssociator::GetMvdHitStrips: MvdHitStrip #" << index << " does not exist!!!" << endl;
    return -1;
  }
  if (hit->GetRefIndex()!=-1)
  {
    PndSdsMCPoint* myPoint = (PndSdsMCPoint*)(fMvdMCArray->At(hit->GetRefIndex()));
    if (!myPoint)
    {
      cout << "-E- PndMCTrackEnumAssociator::GetMvdHitStrips: MvdMCPoint #" << hit->GetRefIndex() << " does not exist!!!" << endl;
      return -1;
    }
    return (myPoint->GetTrackID());
  }
  else
  {
    return -1;
  }
}


//_________________________________________________________________
Int_t PndMCTrackEnumAssociator::GetSttHits(Int_t index)
{
  // Taking points from PndSttHit
  
  PndSttHit* sttHit = (PndSttHit*) fSttHitInput->At(index);
  if (!sttHit)
  {
    cout << "-E- PndMCTrackEnumAssociator::GetSttHits: SttHit #" << index << " does not exist!!!" << endl;
    return -1;
  }
  return (sttHit->GetRefIndex());
} 

//_________________________________________________________________
Int_t PndMCTrackEnumAssociator::GetGemHits(Int_t index)
{
  // Taking points from PndGemHits
  
  PndGemHit* gemHit = (PndGemHit*) fGemInput->At(index);
  if (!gemHit)
  {
    cout << "-E- PndMCTrackEnumAssociator::GetGemHits: GemHit #" << index << " does not exist!!!" << endl;
    return -1;
  }
  if (gemHit->GetRefIndex()!=-1) 
  {
    PndGemMCPoint* myPoint = (PndGemMCPoint*)(fGemMCArray->At(gemHit->GetRefIndex()));
    if (!myPoint)
    {
      cout << "-E- PndMCTrackEnumAssociator::GetGemHits: GemMCPoint #" << gemHit->GetRefIndex() << " does not exist!!!" << endl;
      return -1;
    }
    return(myPoint->GetTrackID());
  }
  else
  {
    return -1;
  }
} 

//_________________________________________________________________
Int_t PndMCTrackEnumAssociator::GetFtsHits(Int_t index)
{
  // Taking points from PndGemHits
  
  PndFtsHit* ftsHit = (PndFtsHit*) fFtsInput->At(index);
  if (!ftsHit)
  {
    cout << "-E- PndMCTrackEnumAssociator::GetFtsHits: FtsHit #" << index << " does not exist!!!" << endl;
    return -1;
  }

  if (ftsHit->GetRefIndex()!=-1) 
  {
    PndFtsPoint* myPoint = (PndFtsPoint*)(fFtsMCArray->At(ftsHit->GetRefIndex()));
    if (!myPoint)
    {
      cout << "-E- PndMCTrackEnumAssociator::GetFtsHits: FtsPoint #" << ftsHit->GetRefIndex() << " does not exist!!!" << endl;
      return -1;
    }
    return(myPoint->GetTrackID());
  }
  else
  {
    return -1;
  }
} 

//_________________________________________________________________
void PndMCTrackEnumAssociator::Exec(Option_t * option)
{
  //FIXME: Here we should call Clear() to save time. For 
  //       really large events we could call Delete() to save Memory
  fTrackIDArray->Delete();
  
  for (Int_t tt=0; tt<fTrackArray->GetEntriesFast(); tt++)
  { 
    map<Int_t, Int_t> fMCTrackList; // MC TrackId, multiplicity
    map<Int_t, Int_t>::iterator iter;
    
    PndTrack *pndTrack = (PndTrack*)fTrackArray->At(tt);
    PndTrackCand trackCand = pndTrack->GetTrackCand();
    for (Int_t ii=0; ii<trackCand.GetNHits(); ii++)
    {
      PndTrackCandHit candHit = trackCand.GetSortedHit(ii);
      Int_t trackID = -1;
      if (candHit.GetDetId()==-1)
	    {
	      cout << "-W- PndMCTrackEnumAssociator::Exec: Not identified Detector ID" << endl;
	    }
      else
	    {
	      if ((candHit.GetDetId()==kMVDHitsPixel) && fMvdMCArray &&  fMvdPixelHitArray) trackID = GetMvdHitPixels(candHit.GetHitId());
	      if ((candHit.GetDetId()==kMVDHitsStrip) && fMvdMCArray &&  fMvdStripHitArray) trackID = GetMvdHitStrips(candHit.GetHitId());
	      if ((candHit.GetDetId()==kSttHit) && fSttHitInput) trackID = GetSttHits(candHit.GetHitId());
	      if ((candHit.GetDetId()==kGemHit) && fGemMCArray && fGemInput) trackID = GetGemHits(candHit.GetHitId());
	      if ((candHit.GetDetId()==kFtsHit) && fFtsMCArray && fFtsInput) trackID = GetFtsHits(candHit.GetHitId());
	    }
      fMCTrackList[trackID]++;
    } // end of PndTrackCand loop     
    
    multimap<Int_t, Int_t> fMCRevertedList; // multiplicity, MC TrackId
    Int_t count = 0;
    for (iter=fMCTrackList.begin(); iter!=fMCTrackList.end(); ++iter)
    {
      fMCRevertedList.insert(pair<Int_t, Int_t>(-(*iter).second, (*iter).first));
      count++;
    }
    
    TArrayI trackID(count);
    TArrayI multID(count);
    
    count = 0;
    for (iter=fMCRevertedList.begin(); iter!=fMCRevertedList.end(); ++iter)
    {
      trackID[count] = (*iter).second;
      multID[count] = -(*iter).first;
      count++;
    }
    
    TClonesArray &pndtrackids = *fTrackIDArray;
    Int_t size = pndtrackids.GetEntriesFast();
    PndTrackID* pndTrackId = new(pndtrackids[size]) PndTrackID(size ,trackID, multID);
  } // end of PndTrack loop
  
}

//_________________________________________________________________
void PndMCTrackEnumAssociator::Register() {
  //---
  FairRootManager::Instance()->
  Register(fTrackOutBranchName,"Gen", fTrackIDArray, fPersistence);
  
}

ClassImp(PndMCTrackEnumAssociator)

