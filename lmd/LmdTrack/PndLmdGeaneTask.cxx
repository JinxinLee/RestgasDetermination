// -------------------------------------------------------------------------
// -----                   PndLmdGeaneTask source file                 -----
// -----                 Created 18/07/08  by T.Stockmanns             -----
// -----           modified for Lmd by M. Michel & A.Karavdina         -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include <TMatrixDSym.h>


// framework includes
#include "FairRootManager.h"
#include "PndLmdGeaneTask.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "PndMCTrack.h"
#include "FairBaseParSet.h"
#include "TGeant3.h"
#include "FairTrackParH.h"
#include "FairTrackParP.h"
#include "TDatabasePDG.h"
#include "PndTrack.h"

// PndSds includes
#include "PndSdsMCPoint.h"
#include "PndSdsHit.h"
//PndLmd includes
#include "PndLinTrack.h"

#include <vector>
#include <map>


// -----   Default constructor   -------------------------------------------
PndLmdGeaneTask::PndLmdGeaneTask() : FairTask("Geane Task for PANDA Lmd"), fEventNr(0), fUseMVDPoint(false)
{
}
// -------------------------------------------------------------------------


PndLmdGeaneTask::PndLmdGeaneTask(Double_t pBeam,TVector3 IP): FairTask("Geane Task for PANDA Lmd"), fEventNr(0), fUseMVDPoint(false)
{
  fPbeam = pBeam;
  cout<<"Beam Momentum for particle with PDGid#"<<fPDGid<<" this run is "<<fPbeam<<endl;
  vtx = IP;
  cout<<"Interaction Point:"<<endl;
  vtx.Print();
}


// -----   Destructor   ----------------------------------------------------
PndLmdGeaneTask::~PndLmdGeaneTask()
{
}

// -----   Public method Init   --------------------------------------------
InitStatus PndLmdGeaneTask::Init()
{
 
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( !ioman){
    std::cout << "-E- PndLmdGeaneTask::Init: "<< "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }


  fMCHits = (TClonesArray*) ioman->GetObject("LMDPoint");
  if ( !fMCHits)	{
    std::cout << "-W- PndLmdGeaneTask::Init: "<< "No LMDPoint"<<" array!" << std::endl;
    return kERROR;
  }

  //  fTracks = (TClonesArray*) ioman->GetObject("LMDTrack");
  fTracks = (TClonesArray*) ioman->GetObject("LMDPndTrack");
  if (!fTracks){
    std::cout << "-W- PndLmdGeaneTask::Init: "<< "No Track" << " array!" << std::endl;
    return kERROR;
  }


  fTrackParGeane = new TClonesArray("FairTrackParH");
  ioman->Register("GeaneTrackPar","Geane", fTrackParGeane, kTRUE);

  fTrackParIni = new TClonesArray("FairTrackParH");
  ioman->Register("GeaneTrackIni","Geane", fTrackParIni, kTRUE);

  fTrackParFinal = new TClonesArray("FairTrackParH");
  ioman->Register("GeaneTrackFinal","Geane", fTrackParFinal, kTRUE);


  fDetName = new TClonesArray("TObjString");
  ioman->Register("DetName", "Geane", fDetName, kTRUE);

  fPro = new FairGeanePro();
  fGeoH = PndGeoHandling::Instance();
  FairRun* fRun = FairRun::Instance();
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
 
  return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndLmdGeaneTask::SetParContainers()
{
  // Get Base Container
 /// FairRun* ana = FairRun::Instance();
 // FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}


// -----   Public method Exec   --------------------------------------------
void PndLmdGeaneTask::Exec(Option_t* opt)
{
  // cout<<"PndLmdGeaneTask::Exec starts!"<<endl;
  std::map<int, std::vector<int> > mcHitMap;//Track ->  MCHits
  fTrackParGeane->Delete();
  fTrackParIni->Delete();
  fTrackParFinal->Delete();
  fDetName->Delete();

  mcHitMap = AssignHitsToTracks();
  fEventNr++;

  //Charge & mass of particle
  Int_t PDGCode = -2212; //antiproton
  //  Int_t PDGCode = fPDGid;
  TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
  TParticlePDG *fParticle = fdbPDG->GetParticle(PDGCode);
  Double_t  fCharge = fParticle->Charge()/3.;
  //  cout<<"fCharge = "<<fCharge<<endl;

  //go through all tracks
  int glI = fTracks->GetEntriesFast();
  // cout<<"glI = "<<glI<<endl;
  Int_t counterGeaneTrk = 0;
  for (Int_t i = 0; i<glI;i++){ 
    //  cout<<"PndLmdGeaneTask::Exec for track#"<<i<<endl;
      TVector3 StartPos, StartPosErr, StartMom, StartMomErr, StartO, StartU, StartV;
      int p = 0;
    
      PndTrack* recTrack = (PndTrack*)(fTracks->At(i));
      FairTrackParP fFittedTrkP = recTrack->GetParamFirst();
      TVector3 PosRecLMD(fFittedTrkP.GetX(),fFittedTrkP.GetY(),fFittedTrkP.GetZ());
      TVector3 MomRecLMD(fFittedTrkP.GetPx(),fFittedTrkP.GetPy(),fFittedTrkP.GetPz());
      MomRecLMD *=fPbeam/MomRecLMD.Mag();
      double covMARS[6][6];
      fFittedTrkP.GetMARSCov(covMARS);
      TVector3 errMomRecLMD(sqrt(covMARS[0][0]),sqrt(covMARS[1][1]),sqrt(covMARS[2][2]));
      TVector3 errPosRecLMD(sqrt(covMARS[3][3]),sqrt(covMARS[4][4]),sqrt(covMARS[5][5]));

      StartPos = PosRecLMD;
      StartPosErr = errPosRecLMD;
      StartMom = MomRecLMD;
      StartMomErr = errMomRecLMD;

       if(fVerbose>2){
	 cout<<"------------------------------------------"<<endl;      
	 cout<<"StartPos:"<<endl;
	 StartPos.Print();
	 cout<<"StartPosErr:"<<endl;
	 StartPosErr.Print();
	 cout<<""<<endl;
	 cout<<"StartMom: "<<StartMom.Mag()<<endl;
	 StartMom.Print();
	 cout<<"StartMomErr: "<<StartMomErr.Mag()<<endl;
	 StartMomErr.Print();     
       }
      TClonesArray& clref1 = *fTrackParIni;
      Int_t size1 = clref1.GetEntriesFast();

      ///Propagate to the PCA (a space point) in 7 steps ---------------------------------
      //Comment: seems back propagation in one step (for 11 m) is too much
      //try smoothing it by small steps, which follow mag.field
      const int nstep=7;
      double zbend[nstep]={661, 660.5, 660., 659, 319, 316, 220};//entarance and exit mag.field
      FairTrackParP *fStartPst = new FairTrackParP(fFittedTrkP);
      for(int js=0;js<nstep;js++){
	TVector3 stStartPos(fStartPst->GetX(),fStartPst->GetY(),fStartPst->GetZ());
	// //propagate to virtual plane@PCA
	// TVector3 spacePoint(0,0,zbend[js]);
	// fPro->SetPoint(spacePoint);
	// fPro->BackTrackToVirtualPlaneAtPCA(1); //1 = pca to point
	// FairTrackParP *fResPst = new FairTrackParP();
	// Bool_t isProp = fPro->Propagate(fStartPst, fResPst, PDGCode);

	//propagate plane-to-plane
	TVector3 ist(fStartPst->GetIVer());
	TVector3 jst(fStartPst->GetJVer());
	TVector3 kst(fStartPst->GetKVer());
	// if(fVerbose>2){
	//   cout<<"current : step#"<<js<<endl;
	//   stStartPos.Print();
	// }
	TVector3 oc(0,0,zbend[js]);
	TVector3 dj(1.,0.,0.);
	TVector3 dk(0.,1.,0.);
	dj.SetMag(1);
	dk.SetMag(1);
	//   	fPro->PropagateFromPlane(dj, dk);
	fPro->PropagateFromPlane(jst, kst);//1st detector plane
	fPro->PropagateToPlane(oc,dj,dk);//virtual plane at fixed z
	fPro->setBackProp();
	FairTrackParP *fResPst = new FairTrackParP();
	Bool_t isProp = fPro->Propagate(fStartPst, fResPst, PDGCode);

	if(fVerbose>2){
	  if(isProp) cout<<"Propagation is OK"<<endl;
	  else cout<<"Propagation failed!"<<endl;
	  cout<<"RESULT:"<<endl;
	  //	  fResPst->Print();
	  TVector3 finPos(fResPst->GetX(),fResPst->GetY(),fResPst->GetZ());
	  finPos.Print();
	}
	fStartPst = fResPst;
      }
      ///and now Propagate to the PCA (a space point) in one step ---------------------------------
      int ierr=0;
      FairTrackParH *fStart = new (clref1[size1]) FairTrackParH(fStartPst,ierr);
      TClonesArray& clref = *fTrackParGeane;
      Int_t size = clref.GetEntriesFast();
      FairTrackParH *fRes = new(clref[size]) FairTrackParH();
      fPro->SetPoint(vtx);
      fPro->PropagateToPCA(1,-1);// back-propagate to point
      Bool_t isProp = fPro->Propagate(fStart, fRes, PDGCode);
      // ///----------------------------------------------------------------------
      delete  fStartPst;
      ///----------------------------------------------------------------------

      // ///Propagate to the PCA (a space point) in one step ---------------------------------
      // int ierr=0;
      // FairTrackParH *fStart = new (clref1[size1]) FairTrackParH(&fFittedTrkP,ierr);
      // // FairTrackParH *fStart = 
      // //  	 new (clref1[size1]) FairTrackParH(StartPos, StartMom, StartPosErr, StartMomErr, fCharge);
      // TClonesArray& clref = *fTrackParGeane;
      // Int_t size = clref.GetEntriesFast();
      // FairTrackParH *fRes = new(clref[size]) FairTrackParH();
      // fPro->SetPoint(vtx);
      // fPro->PropagateToPCA(1,-1);// back-propagate to point
      // Bool_t isProp = fPro->Propagate(fStart, fRes, PDGCode);
      // ///----------------------------------------------------------------------

    


      // ///Forwars propagate to the 1st plane to a space point---------------------------------
      // PndMCTrack* mctrk = (PndMCTrack*)(fMCTracks->At(i));
      // TVector3 MomMC = mctrk->GetMomentum();
      // TVector3 PosMC = mctrk->GetStartVertex();
      // FairTrackParH *fStart = 
      // 	new (clref1[size1]) FairTrackParH(PosMC, MomMC, StartPosErr, StartMomErr, fCharge);
      // TClonesArray& clref = *fTrackParGeane;
      // Int_t size = clref.GetEntriesFast();
      // FairTrackParH *fRes = new(clref[size]) FairTrackParH();
      // fPro->SetPoint(StartPos);
      // //fPro->PropagateToPCA(1,-1);
      // fPro->PropagateToPCA(1,1);
      // Bool_t isProp = fPro->Propagate(fStart, fRes, PDGCode);
      // ///----------------------------------------------------------------------


      // ///Propagate to virtual plane at PCA ------------------------------------
      // TVector3 oc = (0,0,0);
      // TVector3 dj(0,1,0);
      // TVector3 dk(-1,0,0);
      // FairTrackParP *fStart = 
      // 	new (clref1[size1]) FairTrackParP(StartPos, StartMom, StartPosErr, StartMomErr, fCharge, oc, dj, dk);
      // TClonesArray& clref = *fTrackParGeane;
      // Int_t size = clref.GetEntriesFast();
      // FairTrackParP *fRes = new(clref[size]) FairTrackParP();
      // fPro->SetPoint(vtx);
      // fPro->BackTrackToVirtualPlaneAtPCA(1);
      // Bool_t isProp =	fPro->Propagate(fStart, fRes, PDGCode);
      // cout<<"================= %%%% ===================="<<endl;
      // ///----------------------------------------------------------------------

     // ///Propagate to plane  ------------------------------------
     //  TVector3 oc = vtx;
     //  TVector3 dj(0,1,0);
     //  TVector3 dk(-1,0,0);
     //  TVector3 v1s(StartPos.X(),StartPos.Y(),0);
     //  TVector3 v2s(-StartPos.Y(),StartPos.X(),0); 
     //  FairTrackParP *fStart = 
     //  	new (clref1[size1]) FairTrackParP(StartPos, StartMom, StartPosErr, StartMomErr, fCharge, StartPos,  v1s,  v2s);
     //  TClonesArray& clref = *fTrackParGeane;
     //  Int_t size = clref.GetEntriesFast();
     //  FairTrackParP *fRes = new(clref[size]) FairTrackParP();
     
     //  fPro->PropagateFromPlane(v1s, v2s);
     //  fPro->PropagateToPlane(oc,dj,dk);
     //  fPro->setBackProp();
     //  Bool_t isProp = fPro->Propagate(fStart, fRes, PDGCode);
     //  cout<<"================= %%%% ===================="<<endl;
     //  ///----------------------------------------------------------------------
    
     

      //------- TEST of calculation errors -------
      TVector3 gPos(fRes->GetX(),fRes->GetY(),fRes->GetZ());
      TVector3 gMom(fRes->GetPx(),fRes->GetPy(),fRes->GetPz());
      TVector3 gErrPos(fRes->GetDX(),fRes->GetDY(),fRes->GetDZ());
      TVector3 gErrMom(fRes->GetDPx(),fRes->GetDPy(),fRes->GetDPz());
      // cout<<" "<<endl;
      if(fVerbose>2){
	//      cout<<"================= %%%% ===================="<<endl;
      
      cout<<"gPos:"<<endl;
      gPos.Print();
      // cout<<"difference between final and initial point = "<<(-StartPos.Mag()+gPos.Mag())<<endl;
      cout<<"gErrPos:"<<endl;
      gErrPos.Print();
      //  cout<<"difference between final and initial point error = "<<(-StartPosErr.Mag()+gErrPos.Mag())<<endl;

      cout<<"gMom: "<<gMom.Mag()<<endl;
      gMom.Print();   
      // cout<<"difference between initial and final momentum = "<<(StartMom.Mag()-gMom.Mag())<<endl;
      cout<<"gErrMom: "<<gErrMom.Mag()<<endl;
      // cout<<"difference between initial and final momentum error= "<<(StartMomErr.Mag()-gErrMom.Mag())<<endl;
      gErrMom.Print();
      cout<<"================= %%%% ===================="<<endl;
      }
      //------------------------------------------
      
      if(isProp == kTRUE){
	new((*fTrackParFinal)[counterGeaneTrk]) FairTrackParH(*(fRes)); //save Track
	//	new((*fTrackParFinal)[counterGeaneTrk]) FairTrackParP(*(fRes)); //save Track
	counterGeaneTrk++;
	//	cout<<"***** isProp TRUE *****"<<endl;
      }
      else{
	if(fVerbose>2){
	cout<<"!!! Back-propagation with GEANE didn't return result !!!"<<endl;
	cout<<"StartPos:"<<endl;
	StartPos.Print();
	cout<<"StartPosErr:"<<endl;
	StartPosErr.Print();
	
	cout<<"StartMom: "<<StartMom.Mag()<<endl;
	StartMom.Print();
	cout<<"StartMomErr: "<<StartMomErr.Mag()<<endl;
	StartMomErr.Print();  
	}
	new((*fTrackParFinal)[counterGeaneTrk]) FairTrackParH(); //save NULL
	//	new((*fTrackParFinal)[counterGeaneTrk]) FairTrackParP(); //save NULL
	counterGeaneTrk++;
      }
  }
  // fMCTracks->Delete();
  // fMCHits->Delete();
  if(fVerbose>2) cout<<"PndLmdGeaneTask::Exec END!"<<endl;
}

void PndLmdGeaneTask::Finish()
{
}

std::map<int, std::vector<int> > PndLmdGeaneTask::AssignHitsToTracks()
{
	std::map<int, std::vector<int> > result;
	for (int i = 0; i < fMCHits->GetEntriesFast(); i++){											//get all MC Hits
		PndSdsMCPoint* myPoint = (PndSdsMCPoint*)(fMCHits->At(i));									//sort MCHits with Tracks
		//PndMCTrack* myTrack = (PndMCTrack*)(fMCTracks->At(myPoint->GetTrackID()));
		result[myPoint->GetTrackID()].push_back(i);

	}
	return result;
}

ClassImp(PndLmdGeaneTask);
