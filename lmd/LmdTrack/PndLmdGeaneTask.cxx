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


// framework includes
#include "FairRootManager.h"
#include "PndLmdGeaneTask.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "PndMCTrack.h"
#include "FairBaseParSet.h"
// PndMvd includes
#include "PndSdsMCPoint.h"
#include "FairTrackParH.h"
#include "TGeant3.h"
#include "TDatabasePDG.h"
//PndLmd includes
#include "PndLinTrack.h"

#include <vector>
#include <map>


// -----   Default constructor   -------------------------------------------
PndLmdGeaneTask::PndLmdGeaneTask() : FairTask("Geane Task for PANDA Lmd"), fEventNr(0), fUseMVDPoint(false)
{
}
// -------------------------------------------------------------------------


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

  // Get input arrays
  fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
  if (!fMCTracks){
    std::cout << "-W- PndLmdGeaneTask::Init: "<< "No MCTrack" << " array!" << std::endl;
    return kERROR;
  }

  fMCHits = (TClonesArray*) ioman->GetObject("LMDPoint");
  if ( !fMCHits)	{
    std::cout << "-W- PndLmdGeaneTask::Init: "<< "No LMDPoint"<<" array!" << std::endl;
    return kERROR;
  }

  fTracks = (TClonesArray*) ioman->GetObject("LMDTrack");
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
  //  fGeoH = new PndGeoHandling(gGeoManager);
  fGeoH = PndGeoHandling::Instance();
  FairRun* fRun = FairRun::Instance();
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairBaseParSet* par=(FairBaseParSet*)
    (rtdb->findContainer("FairBaseParSet"));
  fPbeam = par->GetBeamMom();

  return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndLmdGeaneTask::SetParContainers()
{
  // Get Base Container
 /// FairRun* ana = FairRun::Instance();
 //s FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}


// -----   Public method Exec   --------------------------------------------
void PndLmdGeaneTask::Exec(Option_t* opt)
{
  std::map<int, std::vector<int> > mcHitMap;						//Track ->  MCHits
  fTrackParGeane->Delete();
  fTrackParIni->Delete();
  fTrackParFinal->Delete();
  fDetName->Delete();

  mcHitMap = AssignHitsToTracks();
  // cout<<"Beam Momentum = "<<fPbeam<<endl;
  // std::cout << "------------Event " << fEventNr << "-------------" << std::endl;
  fEventNr++;

  //Charge & mass of particle
  Int_t PDGCode = -2212;
  TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
  TParticlePDG *fParticle = fdbPDG->GetParticle(PDGCode);
  Double_t  fCharge = fParticle->Charge();

  //go through all tracks
  int glI = fTracks->GetEntriesFast();
  int glMC = fMCTracks->GetEntriesFast();
  // cout<<"glI = "<<glI<<" glMC = "<<glMC<<endl;
  Int_t counterGeaneTrk = 0;
  for (Int_t i = 0; i<glI;i++){ 
    //   cout<<"PndLmdGeaneTask::Exec for track#"<<i<<endl;
      TVector3 StartPos, StartPosErr, StartMom, StartMomErr, StartO, StartU, StartV;
      int p = 0;

      ///Get parameters of real track
      PndLinTrack* recTrack = (PndLinTrack*)(fTracks->At(i));
      Double_t parrecTrk[4];
      recTrack->GetPar(parrecTrk);
      Double_t p0 = parrecTrk[0];
      Double_t p1 = parrecTrk[1];
      Double_t p2 = parrecTrk[2];
      Double_t p3 = parrecTrk[3];
      Double_t parrecTrkErr[4];
      recTrack->GetParErr(parrecTrkErr);
     
      //Vector of particle momentum and starting point
      TVector3  DirVec =  recTrack->GetDirectionVec();
      // //      Double_t theta = DirVec.Theta();
      // Double_t Pz;
      // if(TMath::Hypot(p1,p3)>1) Pz = fPbeam;
      // else Pz = fPbeam*TMath::Sqrt(1-p1*p1-p3*p3);
      Double_t Pnorm = fPbeam/DirVec.Mag();
      //      Double_t Pnorm = (fPbeam-8e-2)/DirVec.Mag();
      //  StartMom = TVector3(p1*fPbeam,p3*fPbeam,Pz);
      //  cout<<"p1*p1+p3*p3 = "<<p1*p1+p3*p3<<endl;
      //StartMom = TVector3(p1*fPbeam,p3*fPbeam,fPbeam); //TEST of Pz
      StartMom = TVector3(p1*Pnorm,p3*Pnorm,Pnorm); 
      StartPos = recTrack->GetStartVec();
      // if(fabs(StartPos.X())>100 || fabs(StartPos.Y())>100) continue;
      //  StartMomErr = TVector3(parrecTrkErr[1]*fPbeam,parrecTrkErr[3]*fPbeam,errPz);
      //  StartMomErr = TVector3(parrecTrkErr[1]*Pnorm,parrecTrkErr[3]*Pnorm,errPz);
      StartPosErr = TVector3(parrecTrkErr[0],parrecTrkErr[2],0);
      Double_t errpx = 
	Pnorm*TMath::Hypot((p3*p3+1-2*p1*p1)*parrecTrkErr[1],
			   3*p3*p1*parrecTrkErr[3])/DirVec.Mag2();
      Double_t errpy = 
	Pnorm*TMath::Hypot((p1*p1+1-2*p3*p3)*parrecTrkErr[3],
			   3*p3*p1*parrecTrkErr[1])/DirVec.Mag2();
      Double_t errpz = 3*Pnorm*TMath::Hypot(p1*parrecTrkErr[1],p3*parrecTrkErr[3])/DirVec.Mag2();
      
      StartMomErr = TVector3(errpx,errpy,errpz);
      TClonesArray& clref1 = *fTrackParIni;
      Int_t size1 = clref1.GetEntriesFast();
      
      FairTrackParH *fStart = 
  	new (clref1[size1]) FairTrackParH(StartPos, StartMom, StartPosErr, StartMomErr, fCharge);
    
      TClonesArray& clref = *fTrackParGeane;
      Int_t size = clref.GetEntriesFast();
      FairTrackParH *fRes = new(clref[size]) FairTrackParH();
      TVector3 vtx(0.,0.,0.);
      fPro->SetPoint(vtx);
      fPro->PropagateToPCA(1,-1);
      Bool_t isProp =	fPro->Propagate(fStart, fRes, PDGCode);

      //------- TEST of calculation errors -------
      TVector3 gPos(fRes->GetX(),fRes->GetY(),fRes->GetZ());
      TVector3 gMom(fRes->GetPx(),fRes->GetPy(),fRes->GetPz());
      TVector3 gErrPos(fRes->GetDX(),fRes->GetDY(),fRes->GetDZ());
      TVector3 gErrMom(fRes->GetDPx(),fRes->GetDPy(),fRes->GetDPz());
      // cout<<" "<<endl;
      // cout<<"================= %%%% ===================="<<endl;
      // cout<<"StartPos:"<<endl;
      // StartPos.Print();
      // cout<<"StartPosErr:"<<endl;
      // StartPosErr.Print();

      // cout<<"StartMom: "<<StartMom.Mag()<<endl;
      // StartMom.Print();
      // cout<<"StartMomErr: "<<StartMomErr.Mag()<<endl;
      // StartMomErr.Print();     
      // cout<<"------------------------------------------"<<endl;
      // cout<<"gPos:"<<endl;
      // gPos.Print();
      // cout<<"difference between initial and final point = "<<(StartPos.Mag()-gPos.Mag())<<endl;
      // cout<<"gErrPos:"<<endl;
      // gErrPos.Print();
      // cout<<"difference between initial and final point error = "<<(StartPosErr.Mag()-gErrPos.Mag())<<endl;

      // cout<<"gMom: "<<gMom.Mag()<<endl;
      // gMom.Print();   
      // cout<<"difference between initial and final momentum = "<<(StartMom.Mag()-gMom.Mag())<<endl;
      // cout<<"gErrMom: "<<gErrMom.Mag()<<endl;
      // cout<<"difference between initial and final momentum error= "<<(StartMomErr.Mag()-gErrMom.Mag())<<endl;
      // gErrMom.Print();
      // cout<<"================= %%%% ===================="<<endl;
      //------------------------------------------
      
      if(isProp == kTRUE){
	//	new((*fTrackParFinal)[i]) FairTrackParH(*(fRes)); //save Track
	new((*fTrackParFinal)[counterGeaneTrk]) FairTrackParH(*(fRes)); //save Track
	counterGeaneTrk++;
	//	cout<<"***** isProp TRUE *****"<<endl;
      }
      else cout<<"!!! Back-propagation with GEANE didn't return result !!!"<<endl;
  }
  fMCTracks->Delete();
  fMCHits->Delete();
  //  cout<<"PndLmdGeaneTask::Exec END!"<<endl;
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
