#include "PndMCTrackAssociator.h"

#include "PndTrack.h"
#include "PndTrackID.h"

#include "FairRunAna.h"
#include "PndTpcPoint.h"
#include "PndTpcCluster.h"
#include "PndSttPoint.h"
#include "PndSttHit.h"
#include "PndSttHelixHit.h"
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
#include "FairMCApplication.h"
#include "FairRootManager.h"
#include "FairTask.h"


#include <iomanip>

//_________________________________________________________________
PndMCTrackAssociator::PndMCTrackAssociator() 
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
PndMCTrackAssociator::PndMCTrackAssociator(const char *name,
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
PndMCTrackAssociator::~PndMCTrackAssociator()
{
}

//_________________________________________________________________
InitStatus PndMCTrackAssociator::Init() {
  
  //  cout << "InitStatus PndMCTrackAssociator::Init\n\n";
  cout << "-I- PndMCTrackAssociator::Init" << endl;
  
  FairRootManager *fManager =FairRootManager::Instance();	
  
  fTrackArray = (TClonesArray *)fManager->GetTClonesArray(fTrackInBranchName);
  if ( ! fTrackArray ) {
    cout << "-I-  PndMCTrackAssociator::Init: No PndTrack array!" << endl;
    return kERROR;
  }
  
  fTpcInput   = (TClonesArray *)fManager->GetObject("PndTpcCluster");
  if ( ! fTpcInput ) 
  {
    cout << "-W- PndMCTrackAssociator::Init: No TpcCluster array" << endl;
  }
  
  fSttHitInput   = (TClonesArray *)fManager->GetObject("STTHit");
  if ( ! fSttHitInput ) 
  {
    cout << "-W- PndMCTrackAssociator::Init: No STTHit array" << endl;
  }
  
  fMvdMCArray = (TClonesArray*) fManager->GetObject("MVDPoint");
  if ( ! fMvdMCArray )
  {
    cout << "-W-  PndMCTrackAssociator::Init: No MVDPoint array" << endl;
  }
  
  fMvdStripHitArray = (TClonesArray*) fManager->GetObject("MVDHitsStrip");
  if ( !fMvdStripHitArray)
  {
    cout << "-W- PndMCTrackAssociator::Init: No MVDHitsStrip array" << endl;
  }
  
  fMvdPixelHitArray = (TClonesArray*) fManager->GetObject("MVDHitsPixel");
  if ( !fMvdPixelHitArray)
  {
    cout << "-W- PndMCTrackAssociator::Init: " << "No fMvdPixelHitArray" << endl;
  }
  
  fGemInput   = (TClonesArray *)fManager->GetObject("GEMHit");
  if ( ! fGemInput ) 
  {
    cout << "-W- PndMCTrackAssociator::Init: No GEMHit array" << endl;
  }
  
  fGemMCArray = (TClonesArray*) fManager->GetObject("GEMPoint");
  if ( ! fGemMCArray )
  {
    cout << "-W- PndMCTrackAssociator::Init: No GEMPoint array" << endl;
  }
  
  fFtsInput   = (TClonesArray *)fManager->GetObject("FTSHit");
  if ( ! fFtsInput ) 
  {
    cout << "-W- PndMCTrackAssociator::Init: No FTSHit array" << endl;
  }
  
  fFtsMCArray = (TClonesArray*) fManager->GetObject("FTSPoint");
  if ( ! fFtsMCArray )
  {
    cout << "-W- PndMCTrackAssociator::Init: No FTSPoint array" << endl;
  }
  
  if (fVerbose)   cout << "-I- PndMCTrackAssociator::Init: Verbose Mode ON" << endl;
  
  Register();
  
  return kSUCCESS;
}


//_________________________________________________________________
Int_t PndMCTrackAssociator::GetMvdHitPixels(Int_t index) {
  // Taking points from PndMvdHitPixel
  
  PndSdsHit* hit = (PndSdsHit*) fMvdPixelHitArray->At(index);
  if (hit->GetRefIndex()!=-1)
  {
    PndSdsMCPoint* myPoint = (PndSdsMCPoint*)(fMvdMCArray->At(hit->GetRefIndex()));
    return (myPoint->GetTrackID());
  }
  else
  {
    return -1;
  }
}

//_________________________________________________________________
Int_t PndMCTrackAssociator::GetMvdHitStrips(Int_t index)
{
  // Taking points from PndMvdHitStrip
  
  PndSdsHit* hit = (PndSdsHit*) fMvdStripHitArray->At(index);
  if (hit->GetRefIndex()!=-1)
  {
    PndSdsMCPoint* myPoint = (PndSdsMCPoint*)(fMvdMCArray->At(hit->GetRefIndex()));
    return (myPoint->GetTrackID());
  }
  else
  {
    return -1;
  }
}

//_________________________________________________________________
Int_t PndMCTrackAssociator::GetTpcClusters(Int_t index)
{
  // Taking points from PndTpcCluster
  
  PndTpcCluster* clu = (PndTpcCluster*) fTpcInput->At(index);
  return (clu->mcId().DominantID().mctrackID());
} 

//_________________________________________________________________
Int_t PndMCTrackAssociator::GetSttHits(Int_t index)
{
  // Taking points from PndSttHit
  
  PndSttHit* sttHit = (PndSttHit*) fSttHitInput->At(index);
  return (sttHit->GetRefIndex());
} 

//_________________________________________________________________
Int_t PndMCTrackAssociator::GetGemHits(Int_t index)
{
  // Taking points from PndGemHits
  
  PndGemHit* gemHit = (PndGemHit*) fGemInput->At(index);
  if (gemHit->GetRefIndex()!=-1) 
  {
    PndGemMCPoint* myPoint = (PndGemMCPoint*)(fGemMCArray->At(gemHit->GetRefIndex()));
    return(myPoint->GetTrackID());
  }
  else
  {
    return -1;
  }
} 

//_________________________________________________________________
Int_t PndMCTrackAssociator::GetFtsHits(Int_t index)
{
  // Taking points from PndGemHits
  
  PndFtsHit* ftsHit = (PndFtsHit*) fFtsInput->At(index);
  if (ftsHit->GetRefIndex()!=-1) 
  {
    PndFtsPoint* myPoint = (PndFtsPoint*)(fFtsMCArray->At(ftsHit->GetRefIndex()));
    return(myPoint->GetTrackID());
  }
  else
  {
    return -1;
  }
} 

//_________________________________________________________________
void PndMCTrackAssociator::Exec(Option_t * option)
{
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
	      cout << "-W- PndMCTrackAssociator::Exec: Not identified Detector ID" << endl;
	    }
      else
	    {
	      if ((candHit.GetDetId()==FairRootManager::Instance()->GetBranchId("MVDHitsPixel")) && fMvdMCArray &&  fMvdPixelHitArray) trackID = GetMvdHitPixels(candHit.GetHitId());
	      if ((candHit.GetDetId()==FairRootManager::Instance()->GetBranchId("MVDHitsStrip")) && fMvdMCArray &&  fMvdStripHitArray) trackID = GetMvdHitStrips(candHit.GetHitId());
	      if ((candHit.GetDetId()==FairRootManager::Instance()->GetBranchId("PndTpcCluster")) && fTpcInput) trackID = GetTpcClusters(candHit.GetHitId());
	      if ((candHit.GetDetId()==FairRootManager::Instance()->GetBranchId("STTHit")) && fSttHitInput) trackID = GetSttHits(candHit.GetHitId());
	      if ((candHit.GetDetId()==FairRootManager::Instance()->GetBranchId("GEMHit")) && fGemMCArray && fGemInput) trackID = GetGemHits(candHit.GetHitId());
	      if ((candHit.GetDetId()==FairRootManager::Instance()->GetBranchId("FTSHit")) && fFtsMCArray && fFtsInput) trackID = GetFtsHits(candHit.GetHitId());
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
void PndMCTrackAssociator::Register() {
  //---
  FairRootManager::Instance()->
  Register(fTrackOutBranchName,"Gen", fTrackIDArray, fPersistence);
  
}

ClassImp(PndMCTrackAssociator)

