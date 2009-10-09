#include "PndSttPatternRecoTask.h"
#include "PndSttTrackMatch.h"
#include "PndSttTrack.h"
#include "GFTrack.h"
#include "GFTrackCand.h"
#include "FairRootManager.h"
#include "TClonesArray.h"
#include <iostream>
#include "PndSttHelixTrackFitter.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "PndSttRecoHit.h"
#include "PndSttHit.h"
#include "GFKalman.h"
#include "GFException.h"
#include "FairGeanePro.h"
#include "FairTrackParP.h"
#include "PndMCTrack.h"


using std::cout;
using std::endl;
using std::string;

// -----   Default constructor   -------------------------------------------
PndSttPatternRecoTask::PndSttPatternRecoTask() : FairTask("STT Pattern Reco Task") 
{
  
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSttPatternRecoTask::~PndSttPatternRecoTask() 
{
  /*
    fTrackArray->Delete();
    fMatchArray->Delete();
    fSttTrackArray->Delete();
  */
}
// -------------------------------------------------------------------------

// -----   Public method Init (abstract in base class)  --------------------
InitStatus PndSttPatternRecoTask::Init() 
{
  //   cout << "SttPRTask::Init()" << endl;
  
  FairRootManager *ioman = FairRootManager::Instance();
  
  if (!ioman) 
    {
      cout << "-E- PndSttPatternRecoTask: "
	   << "RootManager not instantised!" << endl;
      return kFATAL;
    }

  // open MCTrack array
  fMCTrackArray=(TClonesArray*) ioman->GetObject("MCTrack");
  if (fMCTrackArray==0){
    Error("SttPRTask::Init","MC track-array not found!");
    return kERROR;
  }
  // open SttTrackMatch array
  fMatchArray=(TClonesArray*) ioman->GetObject("STTTrackMatch");
  if(fMatchArray==0){
    Error("SttPRTask::Init","match-array not found!");
    return kERROR;
  }
  // open STTTrack array
  fSttTrackArray=(TClonesArray*) ioman->GetObject("STTTrack");
  if(fSttTrackArray==0){
    Error("SttPRTask::Init","stt track-array not found!");
    return kERROR;
  }
  // open STTTrack array
  fSttHitArray=(TClonesArray*) ioman->GetObject("STTHit");
  if(fSttHitArray==0){
    Error("SttPRTask::Init","stt hit-array not found!");
    return kERROR;
  }
  
  // Create and register GFTrack array
  fTrackArray = new TClonesArray("GFTrack",100);
  ioman->Register("Track", "GenFit", fTrackArray, kTRUE);
  
  fPro = new FairGeanePro();
  return kSUCCESS;
}

// ------------------------------------------------------------------

void PndSttPatternRecoTask:: Exec(Option_t* opt) 
{ 
  //   cout << "SttPRTask::Exec()" << endl;
  fTrackArray->Delete();

  PndMCTrack*       mctrack = NULL; 
  PndSttTrack*      track = NULL;
  PndSttTrackMatch* match = NULL;
  
  Int_t nTracks = fSttTrackArray->GetEntriesFast();
  Int_t nMatch = fMatchArray->GetEntriesFast();
   
  //  cout << "SttPRTask: match " << nMatch << " stttrack " << nTracks  << endl;
  if(nMatch != nTracks) cout << "SttPRTask: match stttrack has different number!" << endl;

  for (Int_t iTrack=0; iTrack<nTracks; iTrack++) 
    {
      mctrack = (PndMCTrack*) fMCTrackArray->At(iTrack);
      track = (PndSttTrack*) fSttTrackArray->At(iTrack);
      match = (PndSttTrackMatch*) fMatchArray->At(iTrack);
 
      if (!mctrack) 
	{
	  cout << "-W- SttPRTask::Exec: Empty PndMCTrack at " << iTrack << endl;
	  continue;
	}
     
      if (!track) 
	{
	  cout << "-W- SttPRTask::Exec: Empty PndSttTrack at " << iTrack << endl;
	  continue;
	}

      if(!match)
	{
	  cout << "-W- SttPRTask::Exec: Empty PndSttTrackMatch at " << iTrack << endl;
	  continue;
	}

      if(track->GetFlag() < 3) continue;

      PndSttHelixTrackFitter *fFitter;
      TVector3 *vertex = new TVector3(0.,0.,0.);            // CHECK 
      TVector3 *recovtx = fFitter->PCAToPoint(track, vertex);
      TVector3 *momeatvtx;
      momeatvtx = fFitter->MomentumAtPoint(track, recovtx);

      // STARTING VERTEX ERRORS
      TVector3 StartPos    = *recovtx;
      Double_t pterr, plerr;
      Double_t errxp, erryp, errzp;
      errxp = 10 * 0.02;                      // 200 micron * 10
      erryp = errxp;
      errzp = 10 * 0.15;                      // 1.5 mm     * 10
      TVector3 StartPosErr = TVector3(errxp, erryp, errzp);
      TVector3 StartMom    = *momeatvtx;
      TVector3 mcStartMom = mctrack->GetMomentum();
      Double_t momerr;                        // = 4% of mc mom 
      momerr = 0.04 *  mcStartMom.Mag();
      pterr = momerr;
      plerr = momerr;
      TVector3 StartMomErr = TVector3(pterr, pterr, plerr); 
      Int_t pdg = mctrack->GetPdgCode();
      TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
      TParticlePDG *fParticle= fdbPDG->GetParticle(pdg);
      Double_t  fCharge= fParticle->Charge()/3.;
      
      // cout << "======================================================================================" << endl;
      // cout << "position " << StartPos.X() << " " << StartPos.Y() << " " << StartPos.Z() << endl;
      // cout << "momentum " << StartMom.Mag() << " " << StartMom.X() << " " << StartMom.Y() << " " << StartMom.Z() << endl;
      // cout << "mcStartMom " << mcStartMom.Mag() << "\t" << mcStartMom.X() << "\t" << mcStartMom.Y() << "\t" << mcStartMom.Z() << endl;
      // cout << "pdg " << pdg << " fcharge " << fCharge << endl;
     
      TVector3 u(0.,1.,0.);
      TVector3 v(0.,0.,1.);
      GFDetPlane pl(StartPos,u,v);
      
      GFAbsTrackRep* rep = 0;
      GeaneTrackRep *grep = new GeaneTrackRep(fPro,pl,StartMom,StartPosErr,StartMomErr,fCharge,pdg);
      grep->setPropDir(1); // propagate in flight direction!
      rep=grep;


      GFTrack* trk = new((*fTrackArray)[fTrackArray->GetEntriesFast()]) GFTrack(rep);
      TMatrixT<double> status = trk->getTrackRep(0)->getState();

      GFTrackCand *cand = new GFTrackCand();

      for(int iPoint = 0; iPoint < track->GetNofHits(); iPoint++)
 	{

	  Int_t iHit = track->GetHitIndex(iPoint);
 	  PndSttHit *currenthit = (PndSttHit*) fSttHitArray->At(iHit);
	  if(!currenthit) continue;
	  if(currenthit->GetDetectorID() != 3) continue;  // to be well defined CHECK!!
	  
	  double sigx, sigy, sigz, sigradius;
       	  sigx = 0.0150;
       	  sigy = sigx;
       	  sigz = 0.15;
       	  sigradius = 0.0150;
  
	  int detId = currenthit->GetDetectorID() ;

	  cand->addHit(detId, iHit);
      	}
	
      trk->setCandidate(*cand); // here the candidate is copied! 
      
      //      cout <<"end of track " << iTrack << " " << cand->getNHits() << " " << trk->getNumHits() << endl;
      //      cout << fTrackArray->GetEntriesFast()<<" tracks created"<< endl;
      
    }
  
}
ClassImp(PndSttPatternRecoTask)
