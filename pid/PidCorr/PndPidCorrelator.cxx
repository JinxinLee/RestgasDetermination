#include "PndDetectorList.h"
#include "PndPidCorrelator.h"
#include "PndPidCandidate.h"
#include "PndTrack.h"
#include "PndTrackID.h"

#include "PndTofPoint.h"
#include "PndTofHit.h"
#include "PndEmcBump.h"
#include "PndEmcDigi.h"
#include "PndEmcStructure.h"
#include "PndEmcXtal.h"
#include "PndMdtPoint.h"
#include "PndMdtHit.h"
#include "PndMdtTrk.h"
#include "PndDrcBarPoint.h"
#include "PndDrcHit.h"
#include "PndDskParticle.h"
#include "FairTrackParH.h"
#include "FairMCApplication.h"
#include "FairRunAna.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"

#include "TObjArray.h"
#include "TVector3.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"

#include <cmath>

// ----------------------------------------------------------
// --- Interface with PidMaker and output ---

//___________________________________________________________
PndPidCorrelator::~PndPidCorrelator() {
  //
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}

//___________________________________________________________
PndPidCorrelator::PndPidCorrelator() {
  //---
  fTrack = new TClonesArray("PndTrack");
  fTrackID = new TClonesArray("PndTrackID");
  fPidChargedCand = new TClonesArray("PndPidCandidate");
  fPidNeutralCand = new TClonesArray("PndPidCandidate");
  fMdtTrack = new TClonesArray("PndTrack");
  fDebugMode = kFALSE;
  fGeanePro = kTRUE;
  fMdtRefit = kFALSE;
  fMvdMode = 0;
  fSttMode = 0;
  fTofMode = 0;
  fEmcMode = 0;
  fMdtMode = 0; 
  fDrcMode = 0;
  fDskMode = 0;
  fVerbose = kFALSE;
  fSimulation = kFALSE;
  fIdeal = kFALSE;
  tofCorr = 0;
  emcCorr = 0; 
  drcCorr = 0;
  dskCorr = 0;
  fTrackBranch = "";
  fTrackIDBranch = "";
  sDir = "./";
  sFile = "./pidcorrelator.root";
  fGeoH = PndGeoHandling::Instance();
  Reset();
}

//___________________________________________________________
PndPidCorrelator::PndPidCorrelator(const char *name, const char *title)
  :FairTask(name) {
  //---
  fTrack = new TClonesArray("PndTrack");
  fTrackID = new TClonesArray("PndTrackID");
  fPidChargedCand = new TClonesArray("PndPidCandidate"); 
  fPidNeutralCand = new TClonesArray("PndPidCandidate");
  fMdtTrack = new TClonesArray("PndTrack");
  fDebugMode = kFALSE;
  fGeanePro = kTRUE;
  fMdtRefit = kFALSE;
  fMvdMode = 0;  
  fSttMode = 0;
  fTofMode = 0;
  fEmcMode = 0;
  fMdtMode = 0;
  fDrcMode = 0;
  fDskMode = 0;
  fVerbose = kFALSE;
  fSimulation = kFALSE;
  fIdeal = kFALSE;
  tofCorr = 0;
  emcCorr = 0;
  drcCorr = 0;
  dskCorr = 0;
  fTrackBranch = "";
  fTrackIDBranch = "";
  sDir = "./";
  sFile = "./pidcorrelator.root";
  Reset(); 
}

//___________________________________________________________
InitStatus PndPidCorrelator::Init() {

  //  cout << "InitStatus PndPidCorrelator::Init()" << endl;

  FairRootManager *fManager =FairRootManager::Instance();	

  fTrack = (TClonesArray *)fManager->GetObject(fTrackBranch);
  if ( ! fTrack ) {
    cout << "-I- PndPidCorrelator::Init: No PndTrack array!" << endl;
    return kERROR;
  }

  if (fTrackIDBranch!="")
    {
      fTrackID = (TClonesArray *)fManager->GetObject(fTrackIDBranch);
      if ( ! fTrackID ) {
	cout << "-I- PndPidCorrelator::Init: No PndTrackID array! Switching MC propagation OFF" << endl;
	fTrackIDBranch = "";
      }
    }
  
  // *** STT ***
  fSttHit = (TClonesArray*) fManager->GetObject("SttHelixHit");
  if ( fSttHit ) 
    {
      cout << "-I- PndPidCorrelator::Init: Using SttHelixHit" << endl;
      fSttMode = 3;
    }
  else
    {
      fSttHit = (TClonesArray*) fManager->GetObject("SttHit");
      if ( fSttHit ) 
	{
	  cout << "-I- PndPidCorrelator::Init: Using SttHit" << endl;
	  fSttMode = 2;
	}
    }
  if (fSttMode ==0)
    {
      cout << "-W- PndPidCorrelator::Init: No STT hits array! Switching STT OFF" << endl;
    }
  
  // *** MVD ***
  fMvdHitsStrip = (TClonesArray*) fManager->GetObject("MVDHitsStrip");
  if ( ! fMvdHitsStrip ) 
    {
      cout << "-W- PndPidCorrelator::Init: No MVDHitsStrip array!" << endl;
    }
  else fMvdMode = 2;
  
  fMvdHitsPixel = (TClonesArray*) fManager->GetObject("MVDHitsPixel");
  if ( ! fMvdHitsPixel ) 
    {
      cout << "-W- PndPidCorrelator::Init: No MVDHitsPixel array!" << endl;
    }
  else fMvdMode = 2;
  if (( ! fMvdHitsStrip ) &&  ( ! fMvdHitsPixel ))
    {
      cout << "-W- PndPidCorrelator::Init: No MDC hits array! Switching MVD OFF" << endl;
      fMvdMode = 0;
    }
  else
    {
      cout << "-I- PndPidCorrelator::Init: Using MVDHit" << endl;
    }
  
  // *** TOF ***
  fTofHit = (TClonesArray*) fManager->GetObject("TofHit");
  if ( ! fTofHit ) 
    {
      cout << "-W- PndPidCorrelator::Init: No TofHit array!" << endl;
      fTofMode = 0;
    }
  else  
    {
      cout << "-I- PndPidCorrelator::Init: Using TofHit" << endl;
      fTofMode = 2;
    }
  
  // *** EMC ***
  
  fEmcCluster = (TClonesArray*) fManager->GetObject("EmcCluster");
  if ( ! fEmcCluster ) 
    {
      cout << "-W- PndPidCorrelator::Init: No EmcCluster array!" << endl;
      fEmcMode = 0;
    }
  else 
    {
      cout << "-I- PndPidCorrelator::Init: Using EmcCluster" << endl;
      fEmcMode = 2;
    }
  
  fEmcBump = (TClonesArray*) fManager->GetObject("EmcBump");
  if ( ! fEmcBump ) 
    {
      cout << "-W- PndPidCorrelator::Init: No EmcBump array!" << endl;
    }
  else fEmcMode = 3;
  
  // *** DRC ***
  fDrcHit = (TClonesArray*) fManager->GetObject("DrcHit");
  if ( ! fDrcHit ) 
    {
      cout << "-W- PndPidCorrelator::Init: No DrcHit array!" << endl;
      fDrcMode = 0;
    }
  else  
    {
      cout << "-I- PndPidCorrelator::Init: Using DrcHit" << endl;
      fDrcMode = 2;
    }
  
  // *** DSK ***
  fDskParticle = (TClonesArray*) fManager->GetObject("DskParticle");
  if ( ! fDskParticle )
    {
      cout << "-W- PndPidCorrelator::Init: No DskParticle array!" << endl;
      fDskMode = 0;
    }
  else
    {
      cout << "-I- PndPidCorrelator::Init: Using DskParticle" << endl;
      fDskMode = 2;
    }
 
 
  // *** MDT ***
  fMdtHit = (TClonesArray*) fManager->GetObject("MdtHit");
  if ( ! fMdtHit ) 
    {
      cout << "-W- PndPidCorrelator::Init: No MdtHit array!" << endl;
      fMdtMode = 0;
    }
  else  
    {
      cout << "-I- PndPidCorrelator::Init: Using MdtHit" << endl;
      fMdtMode = 2;
    }
  fMdtTrk = (TClonesArray*) fManager->GetObject("MdtTrk");
  if ( ! fMdtTrk ) 
    {
      cout << "-W- PndPidCorrelator::Init: No MdtTrk array!" << endl;
    }
  else  
    {
      cout << "-I- PndPidCorrelator::Init: Using MdtTrk" << endl;
      fMdtMode = 3;
    }
  
  if (fIdeal)
    {
      cout << "-I- PndPidCorrelator::Init: Using MonteCarlo correlation" << endl;
      fTofPoint = (TClonesArray*) fManager->GetObject("TofPoint");
      if ( ! fTofPoint ) 
	{
	  cout << "-W- PndPidCorrelator::Init: No TofPoint array!" << endl;
	  fTofMode = 0;
	}
      else  
	{
	  cout << "-I- PndPidCorrelator::Init: Using TofPoint" << endl;
	}
      fDrcPoint = (TClonesArray*) fManager->GetObject("DrcBarPoint");
      if ( ! fDrcPoint ) 
	{
	  cout << "-W- PndPidCorrelator::Init: No DrcBarPoint array!" << endl;
	  fDrcMode = 0;
	}
      else  
	{
	  cout << "-I- PndPidCorrelator::Init: Using DrcPoint" << endl;
	}
      fMdtPoint = (TClonesArray*) fManager->GetObject("MdtPoint");
      if ( ! fMdtPoint ) 
	{
	  cout << "-W- PndPidCorrelator::Init: No MdtPoint array!" << endl;
	  fMdtMode = 0;
	}
      else  
	{
	  cout << "-I- PndPidCorrelator::Init: Using MdtPoint" << endl;
	}
    }
  
  Register();
   
  fCorrPar->printParams();

  if (fGeanePro)
    {      cout << "-I- PndPidCorrelator::Init: Using Geane for Track propagation" << endl;
    }
  
  if   (fMdtRefit)
    {
      fFitter = new PndRecoKalmanFit();
      fFitter->SetGeane(fGeanePro);
      fFitter->SetNumIterations(1);
      if (!fFitter->Init()) return kFATAL;
    } 
  
  if (fDebugMode)
    {
      r = TFile::Open(sDir+sFile,"RECREATE");
      
      tofCorr = new TNtuple("tofCorr","TRACK-TOF Correlation",
			    "track_x:track_y:track_z:track_phi:track_p:track_charge:track_theta:track_z0:tof_x:tof_y:tof_z:tof_phi:chi2:dphi:len:glen");
      emcCorr = new TNtuple("emcCorr","TRACK-EMC Correlation",
			    "track_x:track_y:track_z:track_phi:track_p:track_charge:track_theta:track_z0:emc_x:emc_y:emc_z:emc_phi:chi2:dphi:emc_ene:glen:emc_mod");
      mdtCorr = new TNtuple("mdtCorr","TRACK-MDT Correlation",
			    "track_x:track_y:track_z:track_phi:track_p:track_charge:track_theta:track_z0:mdt_x:mdt_y:mdt_z:mdt_phi:chi2:mdt_mod:dphi:glen:mdt_count");
      drcCorr = new TNtuple("drcCorr","TRACK-DRC Correlation",
			    "track_x:track_y:track_z:track_phi:track_p:track_charge:track_theta:track_z0:drc_x:drc_y:drc_phi:chi2:drc_thetac:drc_nphot:dphi:glen");
      dskCorr = new TNtuple("dskCorr","TRACK-DSK Correlation",
                            "track_x:track_y:track_z:track_phi:track_p:track_charge:track_theta:track_z0:dsk_x:dsk_y:dsk_phi:chi2:dsk_thetac:dsk_nphot:dphi:glen");
      cout << "-I- PndPidCorrelator::Init: Filling Debug histograms" << endl;
      
    }
  cout << "-I- PndPidCorrelator::Init: Success!" << endl;
  fEventCounter = 1;
  return kSUCCESS;
}

//______________________________________________________
void PndPidCorrelator::SetParContainers() {

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("PndPidCorrelator:: SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("PndPidCorrelator:: SetParContainers", "No runtime database");

  // Get LHE Correlation parameter container
  fCorrPar = (PndPidCorrPar*) db->getContainer("PndPidCorrPar");
  
}
//______________________________________________________
void PndPidCorrelator::Exec(Option_t * option) {
  //-
  cout << " =====   PndPidCorrelator - Event: " << fEventCounter;
  if (fTrack)     cout << " - Number of tracks for pid " << fTrack->GetEntriesFast();
  if (fEmcMode>0) cout << " - Number of EMC Cluster for pid " << fEmcCluster->GetEntriesFast();
  cout << endl;
  Reset();
  
  if (fTrack)     ConstructChargedCandidate();
  if (fEmcMode>0) ConstructNeutralCandidate();
  fEventCounter++;
}

//______________________________________________________
void PndPidCorrelator::ConstructChargedCandidate() {
  //-
  fPidChargedCand->Delete();
  if (fMdtRefit) fMdtTrack->Delete();

  Int_t nTracks = fTrack->GetEntriesFast();
  for (Int_t i = 0; i < nTracks; i++) {
    PndTrack* track = (PndTrack*) fTrack->At(i);
    PndTrackID* trackID = (PndTrackID*) fTrackID->At(i);
    Int_t ierr = 0;
    FairTrackParP par = track->GetParamLast();
    if ((par.GetMomentum().Mag()<0.1) || (par.GetMomentum().Mag()>15.) )continue;
    FairTrackParH *helix = new FairTrackParH(&par, ierr);
    
    PndPidCandidate* pidCand = 	new PndPidCandidate();
    if (fTrackIDBranch!="")
	{
	if (trackID->GetNCorrTrackId()>0)
	  {
		pidCand->SetMcIndex(trackID->GetCorrTrackID());
	  }
	}
    pidCand->SetTrackIndex(i);
    pidCand->AddLink(FairLink("PndTrack", i));
    if (!GetTrackInfo(track, pidCand)) continue;
    GetMvdInfo(track, pidCand); 
    //GetTpcInfo(track, pidCand); 
    if ( (fSttMode==3) && (fSttHit    ->GetEntriesFast()>0) ) GetSttInfo(track, pidCand);
    if ( (fTofMode==2) && (fTofHit    ->GetEntriesFast()>0) ) GetTofInfo(helix, pidCand);
    if ( (fEmcMode>0)  && (fEmcCluster->GetEntriesFast()>0) ) GetEmcInfo(helix, pidCand);
    if ( (fMdtMode>0)  && (fMdtHit    ->GetEntriesFast()>0) ) GetMdtInfo(track, pidCand);  
    if ( (fDrcMode>0)  && (fDrcHit    ->GetEntriesFast()>0) ) GetDrcInfo(helix, pidCand);
    if ( (fDskMode>0)  && (fDskParticle->GetEntriesFast()>0) ) GetDskInfo(helix, pidCand); 
    AddChargedCandidate(pidCand);
  } 
}

//______________________________________________________
void PndPidCorrelator::ConstructNeutralCandidate() {
  //-
  fPidNeutralCand->Delete();
  TString emcType;
  
  Int_t nBumps;
  if (fEmcMode==2) 
    {
      nBumps = fEmcCluster->GetEntriesFast();
      emcType = "EmcCluster";
    }
  else
    {
      nBumps = fEmcBump->GetEntriesFast();
      emcType = "EmcBump";
    }
  
  for (Int_t i = 0; i < nBumps; i++)
    {
      PndEmcBump* bump;
      PndEmcCluster *clu;
      if (fEmcMode==2) 
	{ 
	  if (fClusterList[i]) continue;
	  bump = (PndEmcBump*) fEmcCluster->At(i);
	  clu  = (PndEmcBump*) fEmcCluster->At(i);
	}
      else
	{
	  bump = (PndEmcBump*) fEmcBump->At(i);
	  if (fClusterList[bump->GetClusterIndex()]) continue; // skip correlated clusters
	  clu = (PndEmcCluster*)fEmcCluster->At(bump->GetClusterIndex());
	  
	}

      TVector3 vtx(0,0,0);
      TVector3 v1=bump->where();
      TVector3 p3;
      TMatrixD covP4=bump->Get4MomentumErrorMatrix();
      p3.SetMagThetaPhi(bump->GetEnergyCorrected(), v1.Theta(), v1.Phi());
      TLorentzVector lv(p3,p3.Mag());
      
      PndPidCandidate* pidCand = new PndPidCandidate(0, vtx, lv);
      pidCand->SetP4Cov(covP4);
      pidCand->SetEmcRawEnergy(bump->energy());
      pidCand->SetEmcCalEnergy(bump->GetEnergyCorrected());
      pidCand->SetEmcIndex(i);
      pidCand->SetEmcModule(bump->GetModule());
      pidCand->SetEmcNumberOfCrystals(bump->NumberOfDigis());

      pidCand->SetLink(FairLink(emcType, i));

      std::vector<Int_t> mclist = clu->GetMcList();
      if (mclist.size()>0)
	{
	  pidCand->SetMcIndex(mclist[0]);
	}
      AddNeutralCandidate(pidCand);
    }
}

//_________________________________________________________________
Bool_t PndPidCorrelator::GetTofInfo(FairTrackParH* helix, PndPidCandidate* pidCand) {

  if (!fIdeal)
    {
      if ((helix->GetMomentum().Theta()*TMath::RadToDeg())<20.) return kFALSE; 
      if ((helix->GetMomentum().Theta()*TMath::RadToDeg())>150.) return kFALSE;
    }
  //---
  PndTofHit *tofHit = NULL; 
  Int_t tofEntries = fTofHit->GetEntriesFast();
  Int_t tofIndex = -1;
  Float_t tofTof = 0., tofLength = -1000, tofGLength = -1000;
  Float_t tofQuality = 1000000;
  
  Float_t chi2 = 0;
  TVector3 vertex(0., 0., 0.);
  TVector3 tofPos(0., 0., 0.);
  TVector3 momentum(0., 0., 0.);
  for (Int_t tt = 0; tt<tofEntries; tt++)
    {
      tofHit = (PndTofHit*)fTofHit->At(tt);
      if ( fIdeal && ( ((PndTofPoint*)fTofPoint->At(tofHit->GetRefIndex()))->GetTrackID() !=pidCand->GetMcIndex()) ) continue;
      tofHit->Position(tofPos);
  
      if (fGeanePro) // Overwrites vertex if Geane is used
	{ 
	  FairGeanePro *fProTof = new FairGeanePro();
	  fProTof->SetPoint(tofPos);
	  fProTof->PropagateToPCA(1, 1);
	  FairTrackParH *fRes= new FairTrackParH();
	  Bool_t rc =  fProTof->Propagate(helix, fRes, -13*pidCand->GetCharge());	
	  if (!rc) continue;
	  
	  vertex.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
	  FairGeanePro *fProVertex = new FairGeanePro();
	  fProVertex->SetPoint(TVector3(0,0,0));
	  fProVertex->PropagateToPCA(1, -1);
	  FairTrackParH *fRes2= new FairTrackParH();
	  Bool_t rc2 =  fProVertex->Propagate(fRes, fRes2, -13*pidCand->GetCharge());
	  if (rc2) tofLength = fProVertex->GetLengthAtPCA();
	}
      
      Float_t dist = (tofPos-vertex).Mag2();
      
      if ( tofQuality > dist)
	{
	  tofIndex = tt;
	  tofQuality = dist;
	  tofTof = tofHit->GetTime();
	  //tofLength = fabs(phi * track->GetRadius() / TMath::Sin(track->GetMomentum().Theta()));
	}
      if (fDebugMode)
	{
	  Float_t ntuple[] = {vertex.X(), vertex.Y(), vertex.Z(), vertex.Phi(),
			      helix->GetMomentum().Mag(), helix->GetQ(), helix->GetMomentum().Theta(), helix->GetZ(),
			      tofPos.X(), tofPos.Y(), tofPos.Z(), tofPos.Phi(),
			      dist, vertex.DeltaPhi(tofPos), tofLength, tofGLength};
	  tofCorr->Fill(ntuple);
	}
    }
  
  if ( (tofQuality<fCorrPar->GetTofCut()) || (fIdeal && tofIndex!=-1) )
    {
      pidCand->SetTofQuality(tofQuality);
      pidCand->SetTofStopTime(tofTof);
      pidCand->SetTofTrackLength(tofLength);
      pidCand->SetTofIndex(tofIndex);
    }

  return kTRUE;
}

//_________________________________________________________________
Bool_t PndPidCorrelator::GetEmcInfo(FairTrackParH* helix, PndPidCandidate* pidCand) { 
  if(! helix){
    std::cerr << "<Error> PndPidCorrelator EMCINFO: FairTrackParH NULL pointer parameter."
	      <<std::endl;
    return kFALSE;
  }
  if(! pidCand){
    std::cerr << "<Error> PndPidCorrelator EMCINFO: pidCand NULL pointer parameter."
	      <<std::endl;
    return kFALSE;
  }
  //---
  Float_t trackTheta = helix->GetMomentum().Theta()*TMath::RadToDeg();
  // PndEmcCluster *emcHit = NULL;
  Int_t emcEntries = fEmcCluster->GetEntriesFast();
  Int_t emcIndex = -1, emcModuleCorr = -1, emcNCrystals = -1;
  Float_t emcEloss = 0., emcElossCorr = 0., emcGLength = -1000;
  Float_t emcQuality = 1000000;
   Float_t chi2 = 0;
  TVector3 vertex(0., 0., 0.); TVector3 emcPos(0., 0., 0.);// TVector3 momentum(0., 0., 0.);

  // Cluster zenike moments
  double Z20 = 0.0; double Z53 = 0.0; double secLatM = 0.00;

  for (Int_t ee = 0; ee<emcEntries; ee++){
    //emcHit = (PndEmcCluster*)fEmcCluster->At(ee);
    PndEmcCluster *emcHit = (PndEmcCluster*)fEmcCluster->At(ee);
    
    if ( fIdeal ){
      std::vector<Int_t> mclist = emcHit->GetMcList();
      if (mclist.size()==0) continue;
      if (mclist[0]!=pidCand->GetMcIndex()) continue;
    }
    
    //if (emcHit->energy() < fCorrPar->GetEmc12Thr()) continue;
    Int_t emcModule = emcHit->GetModule();
    if (emcModule>4) continue;
    
    emcPos = emcHit->where();
    if (fGeanePro){ // Overwrites vertex if Geane is used
      FairGeanePro *fProEmc = new FairGeanePro();
      fProEmc->SetPoint(emcPos);
      fProEmc->PropagateToPCA(1, 1);
      vertex.SetXYZ(-10000, -10000, -10000); // reset vertex
      FairTrackParH *fRes= new FairTrackParH();
      Bool_t rc =  fProEmc->Propagate(helix, fRes, -13*pidCand->GetCharge()); // First propagation at module
      if (!rc) continue;
      
      emcGLength = fProEmc->GetLengthAtPCA();
      vertex.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
      //std::map<PndEmcTwoCoordIndex*, PndEmcXtal*> tciXtalMap=PndEmcStructure::Instance()->GetTciXtalMap();
      //PndEmcDigi *lDigi= (PndEmcDigi*)emcHit->Maxima();
      //PndEmcXtal* xtal = tciXtalMap[lDigi->GetTCI()];
      //emcPos = xtal->frontCentre();
    }

    Float_t dist = (emcPos-vertex).Mag2();
    if ( emcQuality > dist ){
      emcIndex = ee;
      emcQuality = dist;
      emcEloss = emcHit->energy();
      emcElossCorr = emcHit->GetEnergyCorrected();
      emcModuleCorr = emcModule;
      emcNCrystals = emcHit->NumberOfDigis();
      Z20 = emcHit->Z20();// Z_{n = 2}^{m = 0}
      Z53 = emcHit->Z53();// Z_{n = 5}^{m = 3}
      secLatM = emcHit->LatMom();
    }
    
    if (fDebugMode){
      Float_t ntuple[] = {vertex.X(), vertex.Y(), vertex.Z(), vertex.Phi(),
			  helix->GetMomentum().Mag(), helix->GetQ(), helix->GetMomentum().Theta(), helix->GetZ(),
			  emcPos.X(), emcPos.Y(), emcPos.Z(), emcPos.Phi(),
			  dist, vertex.DeltaPhi(emcPos), emcHit->energy(), emcGLength, emcModule};
      emcCorr->Fill(ntuple);
    }
  }// End for(ee = 0;)

  if ( (emcQuality < fCorrPar->GetEmc12Cut()) || ( fIdeal && emcIndex!=-1) ){
    fClusterList[emcIndex] = kTRUE;
    pidCand->SetEmcQuality(emcQuality);
    pidCand->SetEmcRawEnergy(emcEloss);
    pidCand->SetEmcCalEnergy(emcElossCorr);
    pidCand->SetEmcIndex(emcIndex);
    pidCand->SetEmcModule(emcModuleCorr);
    pidCand->SetEmcNumberOfCrystals(emcNCrystals);
    //======= 
    pidCand->SetEmcClusterZ20(Z20);
    pidCand->SetEmcClusterZ53(Z53);
    pidCand->SetEmcClusterLat(secLatM);
    //=====
  }
  
  return kTRUE;
}

//_________________________________________________________________
Bool_t PndPidCorrelator::GetMdtInfo(PndTrack* track, PndPidCandidate* pidCand) {
  //--- 
  FairTrackParP par = track->GetParamLast();
  Int_t ierr = 0;
  FairTrackParH *helix = new FairTrackParH(&par, ierr);
  
  map<Int_t, Int_t>mapMdtTrk;
  
  if (fMdtMode == 3)
    { 
      for (Int_t tt = 0; tt<fMdtTrk->GetEntriesFast(); tt++)
	{
	  PndMdtTrk *mdtTrk = (PndMdtTrk*)fMdtTrk->At(tt);
	  mapMdtTrk[mdtTrk->GetHitIndex(0)] = tt;
	}
    }
  PndMdtHit *mdtHit = NULL;
  Int_t mdtEntries = fMdtHit->GetEntriesFast();
  Int_t mdtIndex = -1, mdtMod = 0, mdtLayer = 0;
  Float_t mdtGLength = -1000;
  Float_t mdtQuality = 1000000;
  Float_t mdtIron = 0., mdtMom = 0, mdtTempMom = 0;
  
  Float_t chi2 = 0;
  TVector3 vertex(0., 0., 0.);
  TVector3 mdtPos(0., 0., 0.);
  TVector3 momentum(0., 0., 0.);
  for (Int_t mm = 0; mm<mdtEntries; mm++)
    {
      mdtHit = (PndMdtHit*)fMdtHit->At(mm);
      if ( fIdeal && ( ((PndMdtPoint*)fMdtPoint->At(mdtHit->GetRefIndex()))->GetTrackID() !=pidCand->GetMcIndex()) ) continue;
      if (mdtHit->GetLayerID()!=0) continue;
      if (mdtHit->GetModule()>2) continue;
      mdtHit->Position(mdtPos);
      if (fGeanePro) // Overwrites vertex if Geane is used
	{ 
	  FairGeanePro *fProMdt = new FairGeanePro();
	  fProMdt->SetPoint(mdtPos);
	  fProMdt->PropagateToPCA(1, 1);
          vertex.SetXYZ(-10000, -10000, -10000); // reset vertex
	  FairTrackParH *fRes= new FairTrackParH();
	  Bool_t rc =  fProMdt->Propagate(helix, fRes, -13*pidCand->GetCharge()); 
	  if (!rc) continue;
	  mdtTempMom = fRes->GetMomentum().Mag();  
	  vertex.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
	  mdtGLength = fProMdt->GetLengthAtPCA();
	}
      
      Float_t dist;
      if (mdtHit->GetModule()==1) 
	{
	  dist = (mdtPos-vertex).Mag2();
	}
      else
	{
	  dist = (vertex.X()-mdtPos.X())*(vertex.X()-mdtPos.X())+(vertex.Y()-mdtPos.Y())*(vertex.Y()-mdtPos.Y());
	}
      
      if ( mdtQuality > dist)
	{
	  mdtIndex = mm;
	  mdtQuality = dist;
	  mdtMod = mdtHit->GetModule();
	  mdtMom = mdtTempMom;
	  mdtLayer = 1;
	  if (fMdtMode==3)
	    {
	      PndMdtTrk *mdtTrk = (PndMdtTrk*)fMdtTrk->At(mapMdtTrk[mdtIndex]);
	      mdtIndex = mapMdtTrk[mm];
	      mdtLayer = mdtTrk->GetLayerCount();
	      mdtIron = mdtTrk->GetIronDist();
	      mdtMod = mdtTrk->GetModule();
	    }
	}
      if (fDebugMode)
	{
	  Float_t ntuple[] = {vertex.X(), vertex.Y(), vertex.Z(), vertex.Phi(), 
			      helix->GetMomentum().Mag(), helix->GetQ(), helix->GetMomentum().Theta(), helix->GetZ(),
			      mdtPos.X(), mdtPos.Y(), mdtPos.Z(), mdtPos.Phi(),
			      dist, mdtHit->GetModule(), vertex.DeltaPhi(mdtPos), mdtGLength, mdtLayer};
	  mdtCorr->Fill(ntuple);
	}
    }
  
  if ((mdtQuality<fCorrPar->GetMdtCut()) || ( fIdeal && mdtIndex!=-1))
    {
      pidCand->SetMuoIndex(mdtIndex);
      pidCand->SetMuoQuality(mdtQuality);
      pidCand->SetMuoIron(mdtIron);
      pidCand->SetMuoMomentumIn(mdtMom);
      pidCand->SetMuoModule(mdtMod);
      pidCand->SetMuoNumberOfLayers(mdtLayer);
    }
  
  if (fMdtRefit && (mdtIndex!=-1) && (mdtMom>0.)  )
    {
      PndMdtTrk *mdtTrk = (PndMdtTrk*)fMdtTrk->At(mdtIndex); 
      PndTrack *mdtTrack = new PndTrack(*track);
      PndTrackCand *oldCand = track->GetTrackCandPtr();
      PndTrackCand *newCand = mdtTrk->AddTrackCand(oldCand);
      mdtTrack->SetTrackCand(*newCand);
      Int_t fCharge= mdtTrack->GetParamFirst().GetQ();
      Int_t PDGCode = -13*fCharge;
      
      PndTrack *fitTrack = new PndTrack();
      fitTrack = fFitter->Fit(mdtTrack, PDGCode);
      PndTrack* pndTrack = new PndTrack(fitTrack->GetParamFirst(), fitTrack->GetParamLast(), fitTrack->GetTrackCand(),
					fitTrack->GetFlag(), fitTrack->GetChi2(), fitTrack->GetNDF(), fitTrack->GetPidHypo(), fitTrack->GetRefIndex(), kLheTrack);
      AddMdtTrack(pndTrack);
    }
  return kTRUE;
}

//_________________________________________________________________
Bool_t PndPidCorrelator::GetDrcInfo(FairTrackParH* helix, PndPidCandidate* pidCand) {
  if ((helix->GetMomentum().Theta()*TMath::RadToDeg())<20.) return kFALSE;
 
  //---
  PndDrcHit *drcHit = NULL;
  Int_t drcEntries = fDrcHit->GetEntriesFast();
  Int_t drcIndex = -1, drcPhot = 0;
  Float_t drcThetaC = -1000, drcThetaCErr = 0, drcGLength = -1000;
  Float_t drcQuality = 1000000;
  
  TVector3 vertex(0., 0., 0.);
  TVector3 drcPos(0., 0., 0.);
  TVector3 momentum(0., 0., 0.);
  for (Int_t dd = 0; dd<drcEntries; dd++)
    {
      drcHit = (PndDrcHit*)fDrcHit->At(dd); 
      if ( fIdeal && ( ((PndDrcBarPoint*)fDrcPoint->At(drcHit->GetRefIndex()))->GetTrackID() !=pidCand->GetMcIndex()) ) continue;
      drcHit->Position(drcPos);
      
      if (fGeanePro) // Overwrites vertex if Geane is used
	{
	  FairGeanePro *fProDrc = new FairGeanePro();
	  fProDrc->PropagateToVolume("DrcBase",0,1);
	  vertex.SetXYZ(-10000, -10000, -10000); // reset vertex
	  FairTrackParH *fRes= new FairTrackParH();
	  Bool_t rc =  fProDrc->Propagate(helix, fRes, -13*pidCand->GetCharge()); 	
	  if (!rc) continue;
	  vertex.SetXYZ(fRes->GetX(), fRes->GetY(), 0.);
	  drcGLength = fProDrc->GetLengthAtPCA();
	}
      
      Float_t dphi = vertex.DeltaPhi(drcPos);
      Float_t dist = dphi * dphi;
      
      if ( drcQuality > dist)
	{
	  drcIndex = dd;
	  drcQuality = dist;
	  drcThetaC = drcHit->GetThetaC();
	  drcThetaCErr = drcHit->GetErrThetaC();
	  drcPhot = 0; // ** to be filled **
	}
      if (fDebugMode)
	{
	  Float_t ntuple[] = {vertex.X(), vertex.Y(), vertex.Z(), vertex.Phi(),  
			      helix->GetMomentum().Mag(), helix->GetQ(), helix->GetMomentum().Theta(), helix->GetZ(),
			      drcPos.X(), drcPos.Y(), drcPos.Phi(), dist, drcHit->GetThetaC(), 0., vertex.DeltaPhi(drcPos), drcGLength};
	  drcCorr->Fill(ntuple);
	}
    }
  
  if ((drcQuality<fCorrPar->GetDrcCut()) || (fIdeal && drcIndex!=-1))
    {
      pidCand->SetDrcQuality(drcQuality);
      pidCand->SetDrcThetaC(drcThetaC);
      pidCand->SetDrcThetaCErr(drcThetaCErr);
      pidCand->SetDrcNumberOfPhotons(drcPhot);
      pidCand->SetDrcIndex(drcIndex);
    }
  return kTRUE;
}

//_________________________________________________________________
Bool_t PndPidCorrelator::GetDskInfo(FairTrackParH* helix, PndPidCandidate* pidCand) {
  if ((helix->GetMomentum().Theta()*TMath::RadToDeg())<1.) return kFALSE;

  //---
  PndDskParticle *dskParticle = NULL;
  Int_t dskEntries = fDskParticle->GetEntriesFast();
  Int_t dskIndex = -1, dskPhot = 0;
  Float_t dskThetaC = -1000, dskThetaCErr = 0, dskGLength = -1000;
  Float_t dskQuality = 1000000;


  TVector3 vertex(0., 0., 0.);
  TVector3 dskPos(0., 0., 0.);
  TVector3 momentum(0., 0., 0.);
  for (Int_t dd = 0; dd<dskEntries; dd++)
    {
      dskParticle = (PndDskParticle*)fDskParticle->At(dd);
      //if ( fIdeal && ( ((PndDskParticle*)fDrcPoint->At(drcHit->GetRefIndex()))->GetTrackID() !=pidCand->GetMcIndex()) ) continue;
      dskParticle->Position(dskPos);

      if (fGeanePro) // Overwrites vertex if Geane is used
        {
          FairGeanePro *fProDsk = new FairGeanePro();
          fProDsk->PropagateToVolume("DskBase",0,1);
          vertex.SetXYZ(-10000, -10000, -10000); // reset vertex
          FairTrackParH *fRes= new FairTrackParH();
          Bool_t rc =  fProDsk->Propagate(helix, fRes, -13*pidCand->GetCharge());
          if (!rc) continue;
          vertex.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
          dskGLength = fProDsk->GetLengthAtPCA();
        }

      
      Float_t dist = (vertex-dskPos).Mag();

      if ( dskQuality > dist)
        {
          dskIndex = dd;
          dskQuality = dist;
          dskThetaC = dskParticle->GetThetaC();
          //dskThetaCErr = dskParticle->GetErrThetaC();
          dskPhot = 0; // ** to be filled **
        }
      if (fDebugMode)
        {
          Float_t ntuple[] = {vertex.X(), vertex.Y(), vertex.Z(), vertex.Phi(),
                              helix->GetMomentum().Mag(), helix->GetQ(), helix->GetMomentum().Theta(), helix->GetZ(),
                              dskPos.X(), dskPos.Y(), dskPos.Phi(), dist, dskParticle->GetThetaC(), 0., vertex.DeltaPhi(dskPos), dskGLength};
          dskCorr->Fill(ntuple);
        }
    }

  //if ((dskQuality<fCorrPar->GetDskCut()) || (fIdeal && dskIndex!=-1))
    if ((dskQuality<1000) || (fIdeal && dskIndex!=-1))
    {
      pidCand->SetDiscQuality(dskQuality);
      pidCand->SetDiscThetaC(dskThetaC);
      //pidCand->SetDskThetaCErr(dskThetaCErr);
      pidCand->SetDiscNumberOfPhotons(dskPhot);
      pidCand->SetDiscIndex(dskIndex);
    }
  return kTRUE;
}



//_________________________________________________________________
Float_t PndPidCorrelator::ExtrapolateToR(FairTrackParH* helix, TVector3 *mom, TVector3 *vertex, const Float_t R)
{
  // Function to extrapolate the momentum and vertex coordinates to a well determined R value
  if (TMath::Tan(helix->GetMomentum().Theta()==0)) {
    cout << "-W-  PndPidCorrelator ::ExtrapolateToR: theta==0 - skipped track" << endl;
    return kFALSE;
  }
  
  Float_t zproj = (R - helix->GetPosition().Perp())/ TMath::Tan(helix->GetMomentum().Theta()) + helix->GetZ();
  return ExtrapolateToZ(helix, mom, vertex, zproj);
}

//_________________________________________________________________
Float_t PndPidCorrelator::ExtrapolateToZ(FairTrackParH* helix, TVector3 *mom, TVector3 *vertex, const Float_t z)
{
  // Function to extrapolate the momentum and vertex coordinates to a well determined z value
  Double_t B      = 2.;
  Double_t pt     = helix->GetMomentum().Pt();
  Double_t lambda = helix->GetLambda();
  Double_t lam = TMath::Tan(helix->GetLambda());
  Int_t Q = TMath::Sign(1, helix->GetQ());

  if ( (lam==0) || (pt==0) || (Q==0) ) {
    cout << "-W-  PndPidCorrelator ::ExtrapolateToZ: lambda/pt/Q==0 - skipped track" << endl;
    return -10000.;
  }

  Double_t alpha = .2998 * .02 ;
  Double_t rad = helix->GetMomentum().Pt() / alpha; // helix radius
  Double_t sign = helix->GetMomentum().Z() / (Q*alpha*rad*lam);
  Double_t m, xc, yc; // find the centre of the helix
  if (helix->GetPy()!=0)
    {
      m = -helix->GetPx() / helix-> GetPy();
      xc = helix->GetX() + Q * rad / TMath::Sqrt(m*m+1);
      yc = helix->GetY() + m * Q * rad / TMath::Sqrt(m*m+1);
    }
  else
    {
      xc = helix->GetX();
      yc = helix->GetY() + Q* rad;;
    }
  TVector2 centre(xc, yc);
  Double_t fi0 = centre.Phi();    // 0 - 2Pi
  Double_t d0 = Q*(TMath::Sqrt(xc*xc + yc*yc) - rad);
  TVector2 pivot(-xc, -yc);

  Double_t z0 = helix->GetZ() - helix->GetPosition().Perp()/TMath::Tan(helix->GetMomentum().Theta());
  TVector2 pnt(helix->GetX() - xc, helix->GetY() - yc);
  Double_t phi = - (z - z0) / (rad*lam*sign); // calulates phi from Z
  
  Double_t x = d0*TMath::Cos(fi0) + rad *(TMath::Cos(fi0) - TMath::Cos(fi0 + phi));
  Double_t y = d0*TMath::Sin(fi0) + rad *(TMath::Sin(fi0) - TMath::Sin(fi0 + phi));
 
  Double_t px = -Q/helix->GetMomentum().Pt()*TMath::Sin(fi0 + phi);
  Double_t py =  Q/helix->GetMomentum().Pt()*TMath::Cos(fi0 + phi);
  Double_t pz =  Q/helix->GetMomentum().Pt()*lam; // STE

  vertex->SetX(x);
  vertex->SetY(y);
  vertex->SetZ(z);

  mom->SetX(px);
  mom->SetY(py);
  mom->SetZ(pz);

  return phi;
}

//_________________________________________________________________
void PndPidCorrelator::Register() {
  //---
  FairRootManager::Instance()->
    Register("PidChargedCand","Pid", fPidChargedCand, kTRUE); 
  FairRootManager::Instance()->
    Register("PidNeutralCand","Pid", fPidNeutralCand, kTRUE);
  if (fMdtRefit)
    {
      FairRootManager::Instance()->
	Register("MdtTrack","Pid", fMdtTrack, kTRUE);
    }
}

//_________________________________________________________________
void PndPidCorrelator::Finish() {
  //---
  if (fDebugMode)
    {
      //TFile *r = TFile::Open(sDir+sFile,"RECREATE");
      r->cd();
      tofCorr->Write();
      emcCorr->Write(); 
      mdtCorr->Write();  
      drcCorr->Write();

      r->Save();
      
      tofCorr->Delete();
      emcCorr->Delete(); 
      mdtCorr->Delete();
      drcCorr->Delete();
      
      tofCorr = 0;
      emcCorr = 0; 
      mdtCorr = 0;
      drcCorr = 0;

      r->Close();
      r->Delete();
    }
  
}
//_________________________________________________________________
void PndPidCorrelator::Reset() {
  //---
  fMvdPath = 0.;
  fMvdELoss = 0.;
  fMvdHitCount = 0;
  fClusterList.clear();
}

//_________________________________________________________________
PndPidCandidate* PndPidCorrelator::AddChargedCandidate(PndPidCandidate* cand) {
  // Creates a new hit in the TClonesArray.
  
  TClonesArray& pidRef = *fPidChargedCand;
  Int_t size = pidRef.GetEntriesFast();
  return new(pidRef[size]) PndPidCandidate(*cand);
}

//_________________________________________________________________
PndPidCandidate* PndPidCorrelator::AddNeutralCandidate(PndPidCandidate* cand) {
  // Creates a new hit in the TClonesArray.
  
  TClonesArray& pidRef = *fPidNeutralCand;
  Int_t size = pidRef.GetEntriesFast();
  return new(pidRef[size]) PndPidCandidate(*cand);
}

//_________________________________________________________________
PndTrack* PndPidCorrelator::AddMdtTrack(PndTrack* track) {
  // Creates a new hit in the TClonesArray.
  
  TClonesArray& pidRef = *fMdtTrack;
  Int_t size = pidRef.GetEntriesFast();
  return new(pidRef[size]) PndTrack(*track);
}

ClassImp(PndPidCorrelator)
