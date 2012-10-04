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
  // if(fPDGid!=-2212){ //calculate momentum if particle is not antiproton
  //     }
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

  // // Get input arrays
  // fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
  // if (!fMCTracks){
  //   std::cout << "-W- PndLmdGeaneTask::Init: "<< "No MCTrack" << " array!" << std::endl;
  //   return kERROR;
  // }

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

  // fHits = (TClonesArray*) ioman->GetObject("LMDHitsStrip");
  // //fHits = (TClonesArray*) ioman->GetObject("LmdHits");
  // if (!fHits){
  //   std::cout << "-W- PndLmdGeaneTask::Init: "<< "No Hits" << " array!" << std::endl;
  //   return kERROR;
  // }

  fTrackParGeane = new TClonesArray("FairTrackParH");
  ioman->Register("GeaneTrackPar","Geane", fTrackParGeane, kTRUE);

  fTrackParIni = new TClonesArray("FairTrackParH");
  ioman->Register("GeaneTrackIni","Geane", fTrackParIni, kTRUE);

  fTrackParFinal = new TClonesArray("FairTrackParH");
  ioman->Register("GeaneTrackFinal","Geane", fTrackParFinal, kTRUE);

  // fTrackParGeane = new TClonesArray("FairTrackParP");
  // ioman->Register("GeaneTrackPar","Geane", fTrackParGeane, kTRUE);

  // fTrackParIni = new TClonesArray("FairTrackParP");
  // ioman->Register("GeaneTrackIni","Geane", fTrackParIni, kTRUE);

  // fTrackParFinal = new TClonesArray("FairTrackParP");
  // ioman->Register("GeaneTrackFinal","Geane", fTrackParFinal, kTRUE);

  fDetName = new TClonesArray("TObjString");
  ioman->Register("DetName", "Geane", fDetName, kTRUE);

  fPro = new FairGeanePro();
  fGeoH = PndGeoHandling::Instance();
  FairRun* fRun = FairRun::Instance();
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  // FairBaseParSet* par=(FairBaseParSet*)
  //   (rtdb->findContainer("FairBaseParSet"));
  // fPbeam = par->GetBeamMom();
  //  cout<<"Beam Momentum for this run is "<<fPbeam<<endl;
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
  Double_t  fCharge = fParticle->Charge();

  //go through all tracks
  int glI = fTracks->GetEntriesFast();
  // cout<<"glI = "<<glI<<endl;
  Int_t counterGeaneTrk = 0;
  for (Int_t i = 0; i<glI;i++){ 
    //  cout<<"PndLmdGeaneTask::Exec for track#"<<i<<endl;
      TVector3 StartPos, StartPosErr, StartMom, StartMomErr, StartO, StartU, StartV;
      int p = 0;

      ///Get parameters of real track
      PndLinTrack* recTrack = (PndLinTrack*)(fTracks->At(i));
      TString 	DecName = recTrack->GetDetName();
      if(DecName!="Lumi") continue;

      //Vector of particle momentum and starting point
      TVector3 DirVec =  recTrack->GetDirectionVec();
      // double dz = -0.008;
      // double dx = DirVec.X()*dz;
      // double dy = DirVec.Y()*dz;
      // //do the transformation from LUMI frame (with z-axis perp. to lumi planes) to lab frame
      rotateFromLumiFrame(DirVec,false);
      //DirVec*=1./DirVec.Mag();
      StartMom = TVector3(DirVec.X()*fPbeam,DirVec.Y()*fPbeam,DirVec.Z()*fPbeam); 
      StartPos = recTrack->GetStartVec();
      // StartPos +=TVector3(dx,dy,dz);
      // cout<<"In LUMI frame:"<<endl;
      // cout<<"StartPos:"<<endl;
      // StartPos.Print();
      // cout<<"StartMom:"<<endl;
      // StartMom.Print();
      // //do the transformation from LUMI frame (with z-axis perp. to lumi planes) to lab frame
      combitransFromLumiFrame(StartPos);
      StartPosErr = recTrack->GetStartErrVec();
      //cout<<"StartPosErr:"<<endl;
      // StartPosErr.Print();
      // //do the transformation from LUMI frame (with z-axis perp. to lumi planes) to lab frame
      rotateFromLumiFrame(StartPosErr, true);
      StartMomErr = (recTrack->GetDirectionErrVec());
      // cout<<"Before: StartMomErr: "<<StartMomErr.Mag()<<endl;
      // StartMomErr.Print();
      
      // //do the transformation from LUMI frame (with z-axis perp. to lumi planes) to lab frame
      rotateFromLumiFrame(StartMomErr, true);
      //     StartMomErr = TVector3(fabs(StartMomErr.X()),fabs(StartMomErr.Y()),fabs(StartMomErr.Z()));
      StartMomErr *=fPbeam;
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

      ///Propagate to the PCA to a space point---------------------------------
      FairTrackParH *fStart = 
       	 new (clref1[size1]) FairTrackParH(StartPos, StartMom, StartPosErr, StartMomErr, fCharge);
      TClonesArray& clref = *fTrackParGeane;
      Int_t size = clref.GetEntriesFast();
      FairTrackParH *fRes = new(clref[size]) FairTrackParH();

      // //test for beam emittance----------------------
      // PndMCTrack* mctrk = (PndMCTrack*)(fMCTracks->At(i));
      // TVector3 PosMC = mctrk->GetStartVertex();
      // vtx = PosMC;
      // //------------------------------------------------

      fPro->SetPoint(vtx);
      fPro->PropagateToPCA(1,-1);// back-propagate to point

      // //back-propagate to line-------------------------
      // TVector3 extremity1(vtx.X(),vtx.Y(),vtx.Z()-0.1);
      // TVector3 extremity2(vtx.X(),vtx.Y(),vtx.Z()+0.1);
      // fPro->SetWire(extremity1, extremity2);
      // fPro->PropagateToPCA(2,-1);//back-propagate to line
      // //-----------------------------------------------
     
      Bool_t isProp = fPro->Propagate(fStart, fRes, PDGCode);
      ///----------------------------------------------------------------------

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

void PndLmdGeaneTask::combitransFromLumiFrame(TVector3& hitPos){
  //do the transformation from lab frame to LUMI frame (with z-axis perp. to lumi planes)
  const Double_t  kHalfFoilThickness  = 0.0075; // Thickness of sensitive foil (cm)
  const Double_t  kTransZ = 1100.; //(cm) //move at z-position
  const Double_t  kRotUmZ = 476.03; //(cm) //z-point to rotate
  const Double_t  kTransX = 25; //(cm) //move at x-position
  const Double_t  kRot =  0.040596358401388; // 2.326 degree  = 4.05963584013881024e-02 rad
  TVector3 LumiTrans(0,0,kTransZ-kRotUmZ);
  hitPos +=LumiTrans;
  hitPos.RotateY(kRot);
  LumiTrans = TVector3(0,0,kRotUmZ);
  hitPos +=LumiTrans;
  // cout<<"!!! NEW HIT position in LUMI frame!!! "<<endl;
  //  hitPos.Print();
}

// void PndLmdGeaneTask::rotateFromLumiFrame(TVector3& hitPos){
//   TMatrixD hitMtx(3,3);
//   hitMtx[0][0] = hitPos[0];
//   hitMtx[1][0] = hitPos[1];
//   hitMtx[2][0] = hitPos[2];
//   TMatrixD res = rotateFromLumiFrame(hitMtx);
//   hitPos = TVector3(hitMtx(0,0),hitMtx(1,0),hitMtx(2,0));
//   // hitPos = TVector3(res(0,0),res(1,0),res(2,0));
// }

//bool err = true - rotation of errors vector
void PndLmdGeaneTask::rotateFromLumiFrame(TVector3& hitPos, bool err){
  TMatrixD hitMtx(3,3);
  if(err){
    hitMtx[0][0] = hitPos[0]*hitPos[0];
    hitMtx[1][1] = hitPos[1]*hitPos[1];
    hitMtx[2][2] = hitPos[2]*hitPos[2];
    TMatrixD res = rotateFromLumiFrame(hitMtx);
    hitPos = TVector3(sqrt(res(0,0)),sqrt(res(1,1)),sqrt(res(2,2)));
  }
  else{
    hitMtx[0][0] = hitPos[0];
    hitMtx[1][0] = hitPos[1];
    hitMtx[2][0] = hitPos[2];
    TMatrixD res = rotateFromLumiFrame(hitMtx);
    hitPos = TVector3(hitMtx(0,0),hitMtx(1,0),hitMtx(2,0));
  }
}

TMatrixD PndLmdGeaneTask::rotateFromLumiFrame(TMatrixD& hitCov){
   Double_t theta=-2.326;
  Double_t degrad = TMath::Pi()/180.;
  Double_t sintheta = TMath::Sin(degrad*theta);
  Double_t costheta = TMath::Cos(degrad*theta);
  TMatrixD rot(3,3);// Rotation around Y axis
  rot[0][0]= costheta;
  rot[0][1]= 0;
  rot[0][2]= sintheta;
  rot[1][0]= 0;
  rot[1][1]= 1;
  rot[1][2]= 0;
  rot[2][0]= -sintheta;
  rot[2][1]= 0;
  rot[2][2]= costheta;
  TMatrixD result = rot;
  result.T();
  result*=hitCov;
  hitCov = result;
  result*=rot;
  // cout<<"hitCov:"<<endl;
  // hitCov.Print();
  // cout<<"result:"<<endl;
  // result.Print();
  return result;
}

ClassImp(PndLmdGeaneTask);
