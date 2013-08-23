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
#include "PndSdsMergedHit.h"
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
  fPDGid=-2212;
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
  tprop->Branch("xmclmd",&fxmclmd);
  tprop->Branch("ymclmd",&fymclmd);
   tprop->Branch("zmclmd",&fzmclmd);
  tprop->Branch("thetarec",&fthetarec);
  tprop->Branch("phirec",&fphirec);
  tprop->Branch("prec",&fprec);
  tprop->Branch("thetamc",&fthetamc);
  tprop->Branch("phimc",&fphimc);
  tprop->Branch("pmc",&fpmc);
  tprop->Branch("thetamclmd",&fthetamclmd);
  tprop->Branch("phimclmd",&fphimclmd);
  tprop->Branch("pmclmd",&fpmclmd);
  tprop->Branch("xrec_err",&fxrec_err);
  tprop->Branch("yrec_err",&fyrec_err);
  tprop->Branch("xmc_err",&fxmc_err);
  tprop->Branch("ymc_err",&fymc_err);
  tprop->Branch("xmclmd_err",&fxmclmd_err);
  tprop->Branch("ymclmd_err",&fymclmd_err);
 
  tprop->Branch("vrec",&fvrec);
  tprop->Branch("wrec",&fwrec);
  tprop->Branch("tvrec",&ftvrec);
  tprop->Branch("twrec",&ftwrec);
  tprop->Branch("vrec_err",&fvrec_err);
  tprop->Branch("wrec_err",&fwrec_err);
  tprop->Branch("tvrec_err",&ftvrec_err);
  tprop->Branch("twrec_err",&ftwrec_err);
  tprop->Branch("vmc",&fvmc);
  tprop->Branch("wmc",&fwmc);
  tprop->Branch("tvmc",&ftvmc);
  tprop->Branch("twmc",&ftwmc);
  tprop->Branch("vmc_err",&fvmc_err);
  tprop->Branch("wmc_err",&fwmc_err);
  tprop->Branch("tvmc_err",&ftvmc_err);
  tprop->Branch("twmc_err",&ftwmc_err);
  tprop->Branch("twmclmd_err",&ftwmclmd_err);
  tprop->Branch("vmclmd",&fvmclmd);
  tprop->Branch("wmclmd",&fwmclmd);
  tprop->Branch("tvmclmd",&ftvmclmd);
  tprop->Branch("twmclmd",&ftwmclmd);
  tprop->Branch("vmclmd_err",&fvmclmd_err);
  tprop->Branch("wmclmd_err",&fwmclmd_err);
  tprop->Branch("tvmclmd_err",&ftvmclmd_err);
  tprop->Branch("twmclmd_err",&ftwmclmd_err);

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

  //Get trk cand [needed only for tests!!!]
  fRecCandTracks = (TClonesArray*) ioman->GetObject("LMDTrackCand");
  if ( !fRecCandTracks)	{
    std::cout << "-W- PndLmdGeaneTask::Init: "<< "No LMDTrackCand [needed only for tests!!!]"<<" array!" << std::endl;
    return kERROR;
 }

  //Get rec. hits  [needed only for tests!!!]
  fRecHits = (TClonesArray*) ioman->GetObject("LMDHitsMerged");
  if ( !fRecHits)	{
    std::cout << "-W- PndLmdGeaneTask::Init: "<< "No LMDHitsMerged [needed only for tests!!!]"<<" array!" << std::endl;
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
     
      TVector3 PosRecLMD(fFittedTrkP.GetX(),fFittedTrkP.GetY(),fFittedTrkP.GetZ());
      if(fFittedTrkP.GetZ()>1130) continue;// TEST: skip trks from 2nd plane. TODO: check are they fine???
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

      //Make true2 track from MC info on 1st plane of LMD-----------
      FairTrackParP *fStartMCLMD = new FairTrackParP();
      if(fVerbose>5 &&  (fTracks->GetEntries()==fMCTracks->GetEntries())){
      int candID = recTrack->GetRefIndex();
      PndTrackCand *trkcand = (PndTrackCand*)fRecCandTracks->At(candID);    
      PndTrackCandHit candhit = (PndTrackCandHit)(trkcand->GetSortedHit(0));//1st hit info
      Int_t hitID = candhit.GetHitId();
      PndSdsMergedHit* myHit = (PndSdsMergedHit*)(fRecHits->At(hitID));
      int mcrefbot = myHit->GetSecondMCHit();
      int mcreftop = myHit->GetRefIndex();
      PndSdsMCPoint* MCPointHit;
      if(mcreftop>=0){
	MCPointHit = (PndSdsMCPoint*)(fMCHits->At(mcreftop));
      }
      else{
	MCPointHit = (PndSdsMCPoint*)(fMCHits->At(mcrefbot));
      }
      
      TVector3 PosMClmd =  MCPointHit->GetPosition();
      double pxTrue =  MCPointHit->GetPx();
      double pyTrue =  MCPointHit->GetPy();
      double pzTrue =  MCPointHit->GetPz();
      TVector3 MomMClmd(pxTrue,pyTrue,pzTrue);
      TVector3 dirMClmd = MomMClmd;
      dirMClmd *=1./(MomMClmd.Mag());
      MomMClmd *=fPbeam/(MomMClmd.Mag());
      double xMClmdNew =  PosMClmd.X()-(dirMClmd.X()*0.010);
      double yMClmdNew =  PosMClmd.Y()-(dirMClmd.Y()*0.010);
      double zMClmdNew  = PosMClmd.Z()-(dirMClmd.Z()*0.010);
      PosMClmd.SetXYZ(xMClmdNew, yMClmdNew,zMClmdNew);  //shift 100 mkm to adoid adding MC errors during back propagation;
      TVector3 ocMC(0,0,0); //define plane perpendicular to z-axis in IP
      TVector3 djMC(1.,0.,0.);
      TVector3 dkMC(0.,1.,0.);
      TVector3 PosMClmderr(0.,0.,0.);
      TVector3 MomMClmderr(0.,0.,0.);
      fStartMCLMD = new FairTrackParP(PosMClmd, MomMClmd, PosMClmderr, MomMClmderr, fCharge,ocMC,djMC,dkMC);
      }
      //-------------------------------------------------------------------------------

      ///Propagate to the PCA (a space point) in 7 steps ---------------------------------
      //Comment: seems back propagation in one step (for 11 m) is too much
      //try smoothing it by small steps, which follow mag.field
       const int nstep=7;
       double zbend[nstep]={661, 660.5, 660., 659, 319, 316, 220};//entarance and exit mag.field
      //TEST for backward and forward propagation: more steps!
      //      const int nstep=50;
       // double zbend[nstep];
      // const double z0=fFittedTrkP.GetZ();
      // const double z1=1;
      // const double zstep=(z0-z1)/nstep;
      // for(int js=0;js<nstep;js++){
      // 	zbend[js]=z0-zstep*js;
      // }

 //      const int nstep0=10;
 //      //      double zbend0[nstep0]={fFittedTrkP.GetZ(), 661, 660.5, 660., 659, 319, 316, 220,10};//entarance and exit mag.field
 //      double zbend0[nstep0]={fFittedTrkP.GetZ(), 660, 602, 450, 342, 283, 248, 180, 100, 1};//entarance and exit mag.field
 //       //      TEST for backward and forward propagation: more steps!
 //      const int nintstep = 100;
 //      const int nstep=nstep0*nintstep;
 //      double zbend[nstep];
 //       for(int is=1;is<=nstep0;is++){
 // 	 const double z0=zbend0[is-1];
 // 	 const double z1=zbend0[is];
 // 	 const double zstep=(z0-z1)/nintstep;
 // 	 //	 cout<<"is = "<<is<<": z0 = "<<z0<<" z1 = "<<z1<<" zstep = "<<zstep<<endl;
 // 	 for(int js=0;js<nintstep;js++){
 // 	   int curint = (is-1)*nintstep+js;
 // 	   zbend[curint]=z0-zstep*js;
 // 	   //   cout<<"zbend["<<curint<<"]="<<zbend[curint]<<endl;
 // 	 }
 //       }
 vector<double> vxmc(nstep),vymc(nstep),vxmc_err(nstep),vymc_err(nstep),vzmc(nstep),vthetamc(nstep),vphimc(nstep),vpmc(nstep),vvmc(nstep),vwmc(nstep),vtvmc(nstep),vtwmc(nstep),vvmc_err(nstep),vwmc_err(nstep),vtvmc_err(nstep),vtwmc_err(nstep);
      FairTrackParP *fStartMC = new FairTrackParP();
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
 	   TVector3 MomMCerr(0.,0.,0.);
 	   TVector3 PosMCerr(0.,0.,0.);
 	   if(PosMC.Z()>0){
 	     cout<<"!!! Achtung: "<<PosMC.Z()<<endl;
 	     break;
 	   }

 	   fStartMC = new FairTrackParP(PosMC, MomMC, PosMCerr, MomMCerr, fCharge,ocMC,djMC,dkMC);
 	   for(int sj=nstep-1;sj>-1;sj--){
 	     //   cout<<"MC forward: to z="<<zbend[sj]<<endl;
 	     bool isPropMC;
 	     FairTrackParP *fResMC = PropToPlane(fStartMC,zbend[sj],+1,isPropMC);//forward propagation
 	     if(isPropMC){
 	       delete fStartMC;
 	       fStartMC=fResMC;
 	       vxmc[sj]= fResMC->GetX();
 	       vxmc_err[sj] = fResMC->GetDX();
 	       vymc_err[sj] = fResMC->GetDY();
 	       vymc[sj]= fResMC->GetY();
 	       vzmc[sj]= fResMC->GetZ();
 	       TVector3 finDirMC(fResMC->GetPx(),fResMC->GetPy(),fResMC->GetPz());
 	       vpmc[sj]= finDirMC.Mag();
 	       vthetamc[sj]= finDirMC.Theta();
 	       vphimc[sj]= finDirMC.Phi();

 	       vvmc[sj] = fResMC->GetV();
 	       vwmc[sj] = fResMC->GetW();
 	       vtvmc[sj] = fResMC->GetTV();
 	       vtwmc[sj] = fResMC->GetTW();
 	       vvmc_err[sj] = fResMC->GetDV();
 	       vwmc_err[sj] = fResMC->GetDW();
 	       vtvmc_err[sj] = fResMC->GetDTV();
 	       vtwmc_err[sj] = fResMC->GetDTW();
 	       //   cout<<"Next step;)"<<endl;
 	     }
 	     else break;
 	   }
 	 }
       }
       if(abs(fStartMC->GetZ()-fFittedTrkP.GetZ())>10 && fVerbose>5) break;//MC particle wasn't propagated to LMD plane ????
      TClonesArray& clref1 = *fTrackParIni;
      Int_t size1 = clref1.GetEntriesFast();

     
      FairTrackParP *fStartPst = new FairTrackParP(fFittedTrkP);//REC
      FairTrackParP *fStartPstMCLMD= new FairTrackParP(*fStartMCLMD);//MC in LMD, should be treated as REC
      for(int js=1;js<nstep;js++){
 	//	cout<<"step#"<<js<<endl;
 	bool isProp;
 	//	cout<<"REC backward "<<endl;
 	FairTrackParP*  fResPst  = PropToPlane(fStartPst,zbend[js],-1,isProp);//back propagation
 	if(isProp){
 	  delete fStartPst;
 	  fStartPst = fResPst;
 	  if(fVerbose>5){
 	    bool isPropMClmd;
 	    FairTrackParP*  fResPstMCLMD  = PropToPlane(fStartPstMCLMD,zbend[js],-1,isPropMClmd);//back propagation
 	    if(isPropMClmd){

 	      TVector3 finPosMCLMD(fResPstMCLMD->GetX(),fResPstMCLMD->GetY(),fResPstMCLMD->GetZ());
 	      TVector3 finDirMCLMD(fResPstMCLMD->GetPx(),fResPstMCLMD->GetPy(),fResPstMCLMD->GetPz());

 	      TVector3 finPos(fResPst->GetX(),fResPst->GetY(),fResPst->GetZ());
 	      TVector3 finDir(fResPst->GetPx(),fResPst->GetPy(),fResPst->GetPz());

 	      //     double pnt[3]={finPosMC.X(),finPosMC.Y(),finPosMC.Z()}; //Position where to get field strength
 	      double pnt[3]={vxmc[js],vymc[js],vzmc[js]};
 	      double Bf[3]; //result goes here
 	      // retrieve the field from the framework
 	      pndField->Field(pnt, Bf);  //[kGs]
 	      fbx = Bf[0];
 	      fby = Bf[1];
 	      fbz = Bf[2];
 	      fxrec = finPos.X();
 	      fyrec = finPos.Y();
 	      fxrec_err = fResPst->GetDX();
 	      fyrec_err = fResPst->GetDY();
 	      fzrec = finPos.Z();
 	      fprec = finDir.Mag();
 	      fthetarec = finDir.Theta();
 	      fphirec = finDir.Phi();
 	      fxmc = vxmc[js];
 	      fymc = vymc[js];
 	      fzmc = vzmc[js];
 	      fxmc_err = vxmc_err[js];
 	      fymc_err = vymc_err[js];
 	      fpmc = vpmc[js];
 	      fthetamc = vthetamc[js];
 	      fphimc = vphimc[js];
	    

 	      fxmclmd = finPosMCLMD.X();
 	      fymclmd = finPosMCLMD.Y();
 	      fxmclmd_err = fResPstMCLMD->GetDX();
 	      fymclmd_err = fResPstMCLMD->GetDY();
 	      fzmclmd = finPosMCLMD.Z();
 	      fpmclmd = finDirMCLMD.Mag();
 	      fthetamclmd = finDirMCLMD.Theta();
 	      fphimclmd = finDirMCLMD.Phi();

 	      fvmc = vvmc[js];
 	      fwmc = vwmc[js];
 	      ftvmc = vtvmc[js];
 	      ftwmc = vtwmc[js];
 	      fvmc_err = vvmc_err[js];
 	      fwmc_err = vwmc_err[js];
 	      ftvmc_err = vtvmc_err[js];
 	      ftwmc_err = vtwmc_err[js];

 	      fvrec = fResPst->GetV();
 	      fwrec = fResPst->GetW();
 	      ftvrec = fResPst->GetTV();
 	      ftwrec = fResPst->GetTW();
 	      fvrec_err = fResPst->GetDV();
 	      fwrec_err = fResPst->GetDW();
 	      ftvrec_err = fResPst->GetDTV();
 	      ftwrec_err = fResPst->GetDTW();

 	      fvmclmd = fResPstMCLMD->GetV();
 	      fwmclmd = fResPstMCLMD->GetW();
 	      ftvmclmd = fResPstMCLMD->GetTV();
 	      ftwmclmd = fResPstMCLMD->GetTW();
 	      fvmclmd_err = fResPstMCLMD->GetDV();
 	      fwmclmd_err = fResPstMCLMD->GetDW();
 	      ftvmclmd_err = fResPstMCLMD->GetDTV();
 	      ftwmclmd_err = fResPstMCLMD->GetDTW();

 	      tprop->Fill();
 	      delete fStartPstMCLMD;
 	      fStartPstMCLMD = fResPstMCLMD;
     	    }
 	    else
 	      break;
 	  }
 	}
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
	int motherID = mctrk->GetMotherID();
	if(motherID>=0) break;
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

	FairTrackParH *fStartMClmd = new FairTrackParH(fStartPstMCLMD,ierr);
	fPro->SetPoint(vtx);
	fPro->PropagateToPCA(1,-1);// back-propagate to point
	FairTrackParH *fResMCLMD = new FairTrackParH();
	Bool_t isPropMC = fPro->Propagate(fStartMClmd, fResMCLMD, PDGCode);
	TVector3 gPosMC(fResMCLMD->GetX(),fResMCLMD->GetY(),fResMCLMD->GetZ());
	TVector3 gMomMC(fResMCLMD->GetPx(),fResMCLMD->GetPy(),fResMCLMD->GetPz());
	fxmclmd = gPosMC.X();
	fymclmd = gPosMC.Y();
	fzmclmd = gPosMC.Z();
	fpmclmd = gMomMC.Mag();
	fthetamclmd = gMomMC.Theta();
	fphimclmd = gMomMC.Phi();
	tprop->Fill();
	delete  fStartPstMCLMD;
      }
      cout<<"================= %%%% ===================="<<endl;
      }
      //------------------------------------------
      
      if(isProp == kTRUE){
	new((*fTrackParFinal)[counterGeaneTrk]) FairTrackParH(*(fRes)); //save Track
	//	new((*fTrackParFinal)[counterGeaneTrk]) FairTrackParP(*(fRes)); //save Track
	counterGeaneTrk++;
	if(fVerbose>2) cout<<"***** isProp TRUE *****"<<endl;
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

FairTrackParP* PndLmdGeaneTask::PropToPlane(FairTrackParP* fStartPst, double zpos,int dir, bool& isProp){
	TVector3 stStartPos(fStartPst->GetX(),fStartPst->GetY(),fStartPst->GetZ());
	if(zpos>1e3){//external assumption about mom magnitude [use it while in BOX and we know mom should be diff]
	  TVector3 MomStartPos(fStartPst->GetPx(),fStartPst->GetPy(),fStartPst->GetPz());
	  MomStartPos *=fPbeam/MomStartPos.Mag();
	  fStartPst->SetPx(MomStartPos.X()); fStartPst->SetPy(MomStartPos.Y()); fStartPst->SetPz(MomStartPos.Z());//correct mom.magnitude
	}
	//propagate plane-to-plane
	TVector3 ist(fStartPst->GetIVer());
	TVector3 jst(fStartPst->GetJVer());
	TVector3 kst(fStartPst->GetKVer());
	TVector3 oc(0,0,zpos);
	TVector3 dj(1.,0.,0.);
	TVector3 dk(0.,1.,0.);
	dj.SetMag(1);
	dk.SetMag(1);
	fPro->PropagateFromPlane(jst, kst);//1st detector plane
	fPro->PropagateToPlane(oc,dj,dk);//virtual plane at fixed z
	if(dir<0) fPro->setBackProp();
	FairTrackParP *fResPst = new FairTrackParP();
	isProp = fPro->Propagate(fStartPst, fResPst,fPDGid);
	return fResPst;
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
