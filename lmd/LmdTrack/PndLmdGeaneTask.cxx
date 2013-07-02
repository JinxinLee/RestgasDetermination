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
#include "TTree.h"
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
#include "FairRunAna.h"
#include "PndMultiField.h"
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
  //tprop = new TNtuple();
}
// -------------------------------------------------------------------------


PndLmdGeaneTask::PndLmdGeaneTask(Double_t pBeam,TVector3 IP): FairTask("Geane Task for PANDA Lmd"), fEventNr(0), fUseMVDPoint(false)
{
  fPbeam = pBeam;
  cout<<"Beam Momentum for particle with PDGid#"<<fPDGid<<" this run is "<<fPbeam<<endl;
  vtx = IP;
  cout<<"Interaction Point:"<<endl;
  vtx.Print();
  // tprop = new TNtuple("tprop","forward MC vs. backward rec","xrec:yrec:zrec:prec:thetarec:phirec:xmc:ymc:zmc:pmc:thetamc:phimc") ;
  tprop = new TTree("tprop","forward MC vs. backward rec with Mag.Field comp@MC point");
  tprop->Branch("xrec",&fxrec);
  tprop->Branch("yrec",&fyrec);
  tprop->Branch("zrec",&fzrec);
  tprop->Branch("xmc",&fxmc);
  tprop->Branch("ymc",&fymc);
  tprop->Branch("zmc",&fzmc);
  tprop->Branch("thetarec",&fthetarec);
  tprop->Branch("phirec",&fphirec);
  tprop->Branch("prec",&fprec);
  tprop->Branch("thetamc",&fthetamc);
  tprop->Branch("phimc",&fphimc);
  tprop->Branch("pmc",&fpmc);
  tprop->Branch("Bx",&fbx);
  tprop->Branch("By",&fby);
  tprop->Branch("Bz",&fbz);
 

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

 //Get MC tracks
  fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( !fMCTracks)	{
    std::cout << "-W- PndLmdTrkQTask::Init: "<< "No MCTrack"<<" array!" << std::endl;
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
 
  pndField = FairRunAna::Instance()->GetField();

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
  // if(fVerbose>5){
  //   if((fTracks->GetEntries())!=(fMCTracks->GetEntries()))
  //     return;
  //     }
  // cout<<"PndLmdGeaneTask::Exec starts!"<<endl;
  std::map<int, std::vector<int> > mcHitMap;//Track ->  MCHits
  fTrackParGeane->Delete();
  fTrackParIni->Delete();
  fTrackParFinal->Delete();
  fDetName->Delete();

  mcHitMap = AssignHitsToTracks();


  if(fVerbose>2){
    cout<<" ---- Info: "<<  fEventNr<<endl;
  }
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
      // //trk fit results with misaligned sensors can be really wierd
      // bool fitResOK = true;
      // if(abs(fFittedTrkP.GetX())>1000 || abs(fFittedTrkP.GetY())>1000 || abs(fFittedTrkP.GetZ())>1000){
      // 	new((*fTrackParFinal)[counterGeaneTrk]) FairTrackParH(); //save NULL
      // 	//	new((*fTrackParFinal)[counterGeaneTrk]) FairTrackParP(); //save NULL
      // 	counterGeaneTrk++;
      // 	fitResOK = false;
      // }
      // if(!fitResOK)  continue;
      TVector3 PosRecLMD(fFittedTrkP.GetX(),fFittedTrkP.GetY(),fFittedTrkP.GetZ());
      TVector3 MomRecLMD(fFittedTrkP.GetPx(),fFittedTrkP.GetPy(),fFittedTrkP.GetPz());
      MomRecLMD *=fPbeam/MomRecLMD.Mag();//external assumption about mom magnitude
      fFittedTrkP.SetPx(MomRecLMD.X()); fFittedTrkP.SetPy(MomRecLMD.Y()); fFittedTrkP.SetPz(MomRecLMD.Z());
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

	 cout<<"StartMom: "<<StartMom.Mag()<<endl;
	 StartMom.Print();
	 cout<<"StartMomErr: "<<StartMomErr.Mag()<<endl;
	 StartMomErr.Print();     
	 cout<<""<<endl;
	 if(fVerbose>5 &&  (fTracks->GetEntries()==fMCTracks->GetEntries())){ //check results for MC track propagation to LMD plane
	   FairTrackParP *fStartPst = new FairTrackParP(fFittedTrkP);
	   TVector3 istLMD(fStartPst->GetIVer());
	   TVector3 jstLMD(fStartPst->GetJVer());
	   TVector3 kstLMD(fStartPst->GetKVer());
	   //	   cout<<"i,j,k of LMD trk:"<<endl;
	   //	   istLMD+=StartPos;
	   istLMD=StartPos;
	   // istLMD.Print();
	   // jstLMD.Print();
	   // kstLMD.Print();
	  TVector3 ocMC(0,0,0); //define plane perpendicular to z-axis in IP
	  TVector3 djMC(1.,0.,0.);
	  TVector3 dkMC(0.,1.,0.);
	  djMC.SetMag(1);
	  dkMC.SetMag(1);

	  
	   PndMCTrack* mctrk = (PndMCTrack*)(fMCTracks->At(i));
	   TVector3 MomMC = mctrk->GetMomentum();
	   TVector3 PosMC = mctrk->GetStartVertex();
	   if(PosMC.Z()>0) cout<<"!! Ahtung: "<<PosMC.Z()<<endl;
	   FairTrackParP *fStartMC = 
	     new FairTrackParP(PosMC, MomMC, StartPosErr, StartMomErr, fCharge,ocMC,djMC,dkMC);
	   FairTrackParP *fResMC = new FairTrackParP();
	   fPro->PropagateFromPlane(djMC,dkMC);//1st detector plane
	   fPro->PropagateToPlane(istLMD,jstLMD,kstLMD);//virtual plane at fixed z
	   Bool_t isPropMC = fPro->Propagate(fStartMC, fResMC, PDGCode);
	   //   cout<<" MC-LMD (POINT):"<<endl;
	   TVector3 finPosMC(fResMC->GetX(),fResMC->GetY(),fResMC->GetZ());
	   //finPosMC.Print();
	   // cout<<" MC-LMD (DIR):"<<endl;
	   TVector3 finDirMC(fResMC->GetPx(),fResMC->GetPy(),fResMC->GetPz());
	   //finDirMC.Print();
	   if(isPropMC){
	     double pnt[3]={finPosMC.X(),finPosMC.Y(),finPosMC.Z()}; //Position where to get field strength
	     double Bf[3]; //result goes here
	     // retrieve the field from the framework
	     //    FairField* pndField = FairRunAna::Instance()->GetField();
	     pndField->Field(pnt, Bf);  //[kGs]
	     //   cout<<"^^^^ Bf = ("<<Bf[0]<<", "<<Bf[1]<<", "<<Bf[2]<<")"<<endl;
	     fbx = Bf[0];
	     fby = Bf[1];
	     fbz = Bf[2];
	     fxrec = StartPos.X();
	     fyrec = StartPos.Y();
	     fzrec = StartPos.Z();
	     fprec = StartMom.Mag();
	     fthetarec = StartMom.Theta();
	     fphirec = StartMom.Phi();
	     fxmc = finPosMC.X();
	     fymc = finPosMC.Y();
	     fzmc = finPosMC.Z();
	     fpmc = finDirMC.Mag();
	     fphimc = finDirMC.Phi();
	     fthetamc = finDirMC.Theta();
 //  double fxrec, fyrec, fzrec, fprec, fthetarec, fphirec, fxmc, fymc, fzmc, fpmc, fthetamc, fphimc;
  // double fbx,fby,fbz;

	     //	     tprop->Fill(StartPos.X(),StartPos.Y(),StartPos.Z(),StartMom.Mag(),StartMom.Theta(),StartMom.Phi(),finPosMC.X(),finPosMC.Y(),finPosMC.Z(),finDirMC.Mag(),finDirMC.Theta(),finDirMC.Phi());
	     tprop->Fill();
	   }
	 }
       }
      TClonesArray& clref1 = *fTrackParIni;
      Int_t size1 = clref1.GetEntriesFast();

      ///Propagate to the PCA (a space point) in 7 steps ---------------------------------
      //Comment: seems back propagation in one step (for 11 m) is too much
      //try smoothing it by small steps, which follow mag.field
      //      const int nstep=7;
      //      double zbend[nstep]={661, 660.5, 660., 659, 319, 316, 220};//entarance and exit mag.field
      //TEST for 
      const int nstep=550;
      double zbend[nstep];
      //      const double z0=661;
      const double z0=1110.;
      const double z1=11;
      const double zstep=(z0-z1)/nstep;
      for(int js=0;js<nstep;js++){
	zbend[js]=z0-zstep*js;
      }
      FairTrackParP *fStartPst = new FairTrackParP(fFittedTrkP);
      for(int js=0;js<nstep;js++){
	TVector3 stStartPos(fStartPst->GetX(),fStartPst->GetY(),fStartPst->GetZ());
	TVector3 MomStartPos(fStartPst->GetPx(),fStartPst->GetPy(),fStartPst->GetPz());
	MomStartPos *=fPbeam/MomStartPos.Mag();//external assumption about mom magnitude
	fStartPst->SetPx(MomStartPos.X()); fStartPst->SetPy(MomStartPos.Y()); fStartPst->SetPz(MomStartPos.Z());//correct mom.magnitude
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
	  // if(isProp) cout<<"Propagation is OK"<<endl;
	  // else cout<<"Propagation failed!"<<endl;
	  //	  cout<<"RESULT (POINT):"<<endl;
	  //	  fResPst->Print();
	  TVector3 finPos(fResPst->GetX(),fResPst->GetY(),fResPst->GetZ());
	  //  finPos.Print();
	  //  cout<<"RESULT (DIR):"<<endl;
	  TVector3 finDir(fResPst->GetPx(),fResPst->GetPy(),fResPst->GetPz());
	  //	  finDir.Print();


	if(fVerbose>5 &&  (fTracks->GetEntries()==fMCTracks->GetEntries())){ //check results for MC track propagation
	  TVector3 ocMC(0,0,0); //define plane perpendicular to z-axis in IP
	  TVector3 djMC(1.,0.,0.);
	  TVector3 dkMC(0.,1.,0.);
	  djMC.SetMag(1);
	  dkMC.SetMag(1);

      PndMCTrack* mctrk = (PndMCTrack*)(fMCTracks->At(i));
      TVector3 MomMC = mctrk->GetMomentum();
      TVector3 PosMC = mctrk->GetStartVertex();
      FairTrackParP *fStartMC = 
      	new FairTrackParP(PosMC, MomMC, StartPosErr, StartMomErr, fCharge,ocMC,djMC,dkMC);
      FairTrackParP *fResMC = new FairTrackParP();
      fPro->PropagateFromPlane(jst, kst);//1st detector plane
      fPro->PropagateToPlane(oc,dj,dk);//virtual plane at fixed z
      Bool_t isPropMC = fPro->Propagate(fStartMC, fResMC, PDGCode);
      if(isProp && isPropMC){
	//	cout<<"RESULT MC (POINT):"<<endl;
	TVector3 finPosMC(fResMC->GetX(),fResMC->GetY(),fResMC->GetZ());
	//	finPosMC.Print();
	//	cout<<"RESULT MC (DIR):"<<endl;
	TVector3 finDirMC(fResMC->GetPx(),fResMC->GetPy(),fResMC->GetPz());
	//	finDirMC.Print();
	double pnt[3]={finPosMC.X(),finPosMC.Y(),finPosMC.Z()}; //Position where to get field strength
	double Bf[3]; //result goes here
	// retrieve the field from the framework

	pndField->Field(pnt, Bf);  //[kGs]
	//cout<<"^^^^ Bf = ("<<Bf[0]<<", "<<Bf[1]<<", "<<Bf[2]<<")"<<endl;
	fbx = Bf[0];
	fby = Bf[1];
	fbz = Bf[2];
	fxrec = finPos.X();
	fyrec = finPos.Y();
	fzrec = finPos.Z();
	fprec = finDir.Mag();
	fthetarec = finDir.Theta();
	fphirec = finDir.Phi();
	fxmc = finPosMC.X();
	fymc = finPosMC.Y();
	fzmc = finPosMC.Z();
	fpmc = finDirMC.Mag();
	fthetamc = finDirMC.Theta();
	fphimc = finDirMC.Phi();
	tprop->Fill();
 //  double fxrec, fyrec, fzrec, fprec, fthetarec, fphirec, fxmc, fymc, fzmc, fpmc, fthetamc, fphimc;
  // double fbx,fby,fbz;
	//	tprop->Fill(finPos.X(),finPos.Y(),finPos.Z(),finDir.Mag(),finDir.Theta(),finDir.Phi(),finPosMC.X(),finPosMC.Y(),finPosMC.Z(),finDirMC.Mag(),finDirMC.Theta(),finDirMC.Phi());
      }
      else{
	cout<<"Forward propagation for MC failed"<<endl;
      }
	}
	}
	if(isProp)
	  fStartPst = fResPst;
	else
	  break;
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
      // FairTrackParH *fStartMC = 
      // 	new FairTrackParH(PosMC, MomMC, StartPosErr, StartMomErr, fCharge);
      // FairTrackParH *fResMC = new FairTrackParH();
      // fPro->SetPoint(StartPos);
      // //fPro->PropagateToPCA(1,-1);
      // fPro->PropagateToPCA(1,1);
      // Bool_t isProp = fPro->Propagate(fStartMC, fResMC, PDGCode);
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
      if(fVerbose>5 &&  (fTracks->GetEntries()==fMCTracks->GetEntries())){
	PndMCTrack* mctrk = (PndMCTrack*)(fMCTracks->At(i));
	TVector3 MomMC = mctrk->GetMomentum();
	TVector3 PosMC = mctrk->GetStartVertex();
	// cout<<"mcPos:"<<endl;
	// PosMC.Print();
	// cout<<"mcMom: "<<MomMC.Mag()<<endl;
	// MomMC.Print();   

	double pnt[3]={PosMC.X(),PosMC.Y(),PosMC.Z()}; //Position where to get field strength
	double Bf[3]; //result goes here
	// retrieve the field from the framework

	pndField->Field(pnt, Bf);  //[kGs]
	//cout<<"^^^^ Bf = ("<<Bf[0]<<", "<<Bf[1]<<", "<<Bf[2]<<")"<<endl;
	fbx = Bf[0];
	fby = Bf[1];
	fbz = Bf[2];
	fxrec = gPos.X();
	fyrec = gPos.Y();
	fzrec = gPos.Z();
	fprec = gMom.Mag();
	fthetarec = gMom.Theta();
	fphirec = gMom.Phi();
	fxmc = PosMC.X();
	fymc = PosMC.Y();
	fzmc = PosMC.Z();
	fpmc = MomMC.Mag();
	fthetamc = MomMC.Theta();
	fphimc = MomMC.Phi();
	tprop->Fill();
	   //  tprop = new TNtuple("tprop","xrec:yrec:zrec:prec:thetarec:phirec:xmc:ymc:zmc:pmc:thetamc:thetamc") ;
	//	tprop->Fill(gPos.X(),gPos.Y(),gPos.Z(),gMom.Mag(),gMom.Theta(),gMom.Phi(),PosMC.X(),PosMC.Y(),PosMC.Z(),MomMC.Mag(),MomMC.Theta(),MomMC.Phi());
      }
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
  if(fVerbose>5){
    TTree *nout1 = tprop->CloneTree();
    nout1->Write();
  }
    //  tprop->Write();
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
