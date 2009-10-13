// -------------------------------------------------------------------------
// -----                CbmStsHitProducerIdeal source file             -----
// -----                  Created 10/01/06  by V. Friese               -----
// -------------------------------------------------------------------------


#include "TClonesArray.h"
#include "TArrayD.h"
#include "TGeoManager.h"

#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "PndMvdIdealTrackFinderTask.h"

#include "PndMvdDigiPixel.h"
#include "PndMvdDigiStrip.h"
// #include "PndMvdPixelCluster.h"


// -----   Default constructor   -------------------------------------------
PndMvdIdealTrackFinderTask::PndMvdIdealTrackFinderTask() :
  FairTask("MVD Ideal Track Finding Task")
{
   fHitBranchStrip = "MVDHitsStrip";
   fHitBranchPixel = "MVDHitsPixel";
   fClusterBranchStrip = "MVDStripClusterCand";
   fClusterBranchPixel = "MVDClusterCand";
   fDigiBranchStrip = "MVDStripDigis";
   fDigiBranchPixel = "MVDPixelDigis";
   fMcBranch = "MVDPoint";
   fTrackBranch = "MCTrack";
}


// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMvdIdealTrackFinderTask::~PndMvdIdealTrackFinderTask()
{
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndMvdIdealTrackFinderTask::SetParContainers()
{
  // Get Base Container
/*
  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));
*/
}

InitStatus PndMvdIdealTrackFinderTask::ReInit()
{

  InitStatus stat=kERROR;
  return stat;

  /*
  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar=(PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));

  return kSUCCESS;
  */
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdIdealTrackFinderTask::Init()
{

  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman )
    {
      std::cout << "-E- PndMvdIdealTrackFinderTask::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  // Get input array
  fStripHitArray = (TClonesArray*) ioman->GetObject(fHitBranchStrip);
  if ( !fStripHitArray){
    std::cout << "-W- PndMvdIdealTrackFinderTask::Init: " << "No fStripHitArray!" << std::endl;
    return kERROR;
  }

  fPixelHitArray = (TClonesArray*) ioman->GetObject(fHitBranchPixel);
  if ( !fPixelHitArray){
    std::cout << "-W- PndMvdIdealTrackFinderTask::Init: " << "No fPixelHitArray!" << std::endl;
    return kERROR;
  }

  fStripClusterArray = (TClonesArray*) ioman->GetObject(fClusterBranchStrip);
  if ( !fStripClusterArray){
    std::cout << "-W- PndMvdIdealTrackFinderTask::Init: " << "No StripclusterArray!" << std::endl;
    return kERROR;
  }
  fPixelClusterArray = (TClonesArray*) ioman->GetObject(fClusterBranchPixel);
  if ( !fPixelClusterArray){
    std::cout << "-W- PndMvdIdealTrackFinderTask::Init: " << "No PixelclusterArray!" << std::endl;
    return kERROR;
  }


  fStripDigiArray = (TClonesArray*) ioman->GetObject(fDigiBranchStrip);
  if ( !fStripDigiArray){
    std::cout << "-W- PndMvdIdealTrackFinderTask::Init: " << "No StripdigiArray!" << std::endl;
    return kERROR;
  }

  fPixelDigiArray = (TClonesArray*) ioman->GetObject(fDigiBranchPixel);
  if ( !fPixelDigiArray){
    std::cout << "-W- PndMvdIdealTrackFinderTask::Init: " << "No PixeldigiArray!" << std::endl;
    return kERROR;
  }

  fMcArray = (TClonesArray*) ioman->GetObject(fMcBranch);
  if ( !fMcArray){
    std::cout << "-W- PndMvdIdealTrackFinderTask::Init: " << "No mcArray!" << std::endl;
    return kERROR;
  }

  fTrackArray = (TClonesArray*) ioman->GetObject(fTrackBranch);
  if ( !fTrackArray){
    std::cout << "-W- PndMvdIdealTrackFinderTask::Init: " << "No trackArray!" << std::endl;
    return kERROR;
  }

  fTrackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("MVDIdealTrackCand", "MVD", fTrackCandArray, kTRUE);

  std::cout << "-I- PndMvdIdealTrackFinderTask: Initialisation successfull" << std::endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndMvdIdealTrackFinderTask::Exec(Option_t* opt)
{

  // Reset output array
  if ( ! fTrackCandArray )
    Fatal("Exec", "No trackCandArray");
  fTrackCandArray->Delete();

  Int_t nStripHits = fStripHitArray->GetEntriesFast();
  Int_t nPixelHits = fPixelHitArray->GetEntriesFast();
//TODO: Thi becomes easier now, with the FairHit::GetRefIndex()
  //pixel part
  for (Int_t iHit = 0; iHit < nPixelHits; iHit++){
    PndMvdHit* myHit = (PndMvdHit*)(fPixelHitArray->At(iHit));
    PndMvdCluster* myCluster = (PndMvdCluster*)(fPixelClusterArray->At(myHit->GetClusterIndex()));
    PndMvdDigiPixel* apixeldigi = (PndMvdDigiPixel*)fPixelDigiArray->At(myCluster->GetDigiIndex(0));
    if (apixeldigi->GetIndex(0) == -1) continue; // sort out noise
    PndMvdMCPoint* myPoint = (PndMvdMCPoint*)(fMcArray->At(apixeldigi->GetIndex(0)));

    AddAndExpand(myPoint->GetTrackID(),apixeldigi->GetDetID(),iHit, myHit);

  }
  //strip part
  for (Int_t iHit = 0; iHit < nStripHits; iHit++){
    PndMvdHit* myHit = (PndMvdHit*)(fStripHitArray->At(iHit));
    PndMvdCluster* myCluster =  (PndMvdCluster*)(fStripClusterArray->At(myHit->GetClusterIndex()));
    PndMvdDigiStrip* astripdigi = (PndMvdDigiStrip*)fStripDigiArray->At(myCluster->GetDigiIndex(0));
    if (astripdigi->GetIndex(0) == -1) continue; // sort out noise
    PndMvdMCPoint* myPoint = (PndMvdMCPoint*)(fMcArray->At(astripdigi->GetIndex(0)));

    AddAndExpand(myPoint->GetTrackID(),astripdigi->GetDetID(),iHit, myHit);
  }

  if(fVerbose>0) PrintResult();

  Int_t i = 0;
  for (std::map<Int_t,PndTrackCand*>::const_iterator kIt=fTrackCandMap.begin();
      kIt != fTrackCandMap.end(); kIt++){
    new((*fTrackCandArray)[i]) PndTrackCand(*(kIt->second));
    i++;
  }
  ClearTrackCandMap();
}

void PndMvdIdealTrackFinderTask::AddAndExpand(Int_t trackID, Int_t detnum, Int_t iHit,PndMvdHit* theHit){
  if (fTrackCandMap[trackID] == 0){
    PndTrackCand *myTCand = new PndTrackCand();
    PndMCTrack* myMCTrack = (PndMCTrack*)fTrackArray->At(trackID);
//    myTCand->setCurv(GetTrackCurvature(myMCTrack));
//    myTCand->setDip(GetTrackDip(myMCTrack));
//    myTCand->setInverted(false);
    int pdg = myMCTrack->GetPdgCode();
    double charge;
    if(pdg<100000000){
      charge =  TDatabasePDG::Instance()->GetParticle(pdg)->Charge()/3.;
    }
    else{
      charge = 0.;
    }
    myTCand->setTrackSeed(
			  myMCTrack->GetStartVertex(),
			  myMCTrack->GetMomentum(),
			  charge/myMCTrack->GetMomentum().Mag()
			  );
    myTCand->setMcTrackId(trackID);
    fTrackCandMap[trackID] = myTCand;
  }

  fTrackCandMap[trackID]->AddHit(detnum,iHit,theHit->GetPosition().Mag());
}

Double_t PndMvdIdealTrackFinderTask::GetTrackCurvature(PndMCTrack* myTrack)
{
  TVector3 p = myTrack->GetMomentum();
  return (2/TMath::Sqrt(p.Px()*p.Px() + p.Py()*p.Py()));
}

Double_t PndMvdIdealTrackFinderTask::GetTrackDip(PndMCTrack* myTrack)
{
  TVector3 p= myTrack->GetMomentum();
  return (p.Mag()/TMath::Sqrt(p.Px()*p.Px() + p.Py()*p.Py()));
}

void PndMvdIdealTrackFinderTask::PrintResult()
{
  std::cout << "**** TrackFinding *****" << std::endl;
  Int_t nStripHits = fStripHitArray->GetEntriesFast();
   for (std::map<Int_t, PndTrackCand*>::const_iterator kIt=fTrackCandMap.begin();
        kIt != fTrackCandMap.end(); kIt++){
     std::cout << "TrackID: " << kIt->first << std::endl;
     PndTrackCand* trackCand = kIt->second;
     unsigned int detId, hitId;
     for (unsigned int i = 0; i < trackCand->GetNHits(); i++){
       detId=trackCand->GetSortedHit(i).GetDetId();
       hitId=trackCand->GetSortedHit(i).GetHitId();
      PndMvdHit* myHit;
      if(hitId<nStripHits) myHit = (PndMvdHit*)(fStripHitArray->At(hitId));
      else myHit = (PndMvdHit*)(fPixelHitArray->At(hitId - nStripHits));
      std::cout << "Detector no. " << detId <<": "<< *myHit;
     }
   }
   std::cout << std::endl;
}

void PndMvdIdealTrackFinderTask::ClearTrackCandMap()
{
  for (std::map<Int_t,PndTrackCand*>::const_iterator kIt=fTrackCandMap.begin();
          kIt != fTrackCandMap.end(); kIt++){
    delete(kIt->second);
  }
  fTrackCandMap.clear();
}
ClassImp(PndMvdIdealTrackFinderTask);

