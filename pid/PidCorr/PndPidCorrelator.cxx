#include "PndDetectorList.h"
#include "PndPidCorrelator.h"
#include "PndPidCandidate.h"
#include "PndTrack.h"

#include "PndTofHit.h"
#include "PndEmcBump.h"
#include "PndEmcDigi.h"
#include "PndEmcStructure.h"
#include "PndEmcXtal.h"
#include "PndMvdHit.h"
#include "PndSttHit.h"
#include "PndSttHelixHit.h"
#include "PndMdtHit.h"
#include "PndDrcHit.h"
#include "FairTrackParH.h"
#include "FairMCApplication.h"
#include "FairRunAna.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"

#include "TObjArray.h"
#include "TVector3.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
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
  fPidChargedCand = new TClonesArray("PndPidCandidate");
  fPidNeutralCand = new TClonesArray("PndPidCandidate");
  fDebugMode = kFALSE;
  fGeanePro = kTRUE;
  fMvdMode = 0;
  fSttMode = 0;
  fTofMode = 0;
  fEmcMode = 0;
  fMdtMode = 0; 
  fDrcMode = 0;
  fVerbose = kFALSE;
  fSimulation = kFALSE;
  tofCorr = 0;
  emcCorr = 0; 
  drcCorr = 0;
  fTrackBranch = "";
  sDir = "./";
  sFile = "./pidcorrelator.root";
  Reset();
}

//___________________________________________________________
PndPidCorrelator::PndPidCorrelator(const char *name, const char *title)
  :FairTask(name) {
  //---
  fTrack = new TClonesArray("PndTrack");
  fPidChargedCand = new TClonesArray("PndPidCandidate"); 
  fPidNeutralCand = new TClonesArray("PndPidCandidate");
  fGeanePro = kTRUE;
  fMvdMode = 0;  
  fSttMode = 0;
  fTofMode = 0;
  fEmcMode = 0;
  fMdtMode = 0;
  fDrcMode = 0;
  fVerbose = kFALSE;
  fSimulation = kFALSE;
  tofCorr = 0;
  emcCorr = 0;
  drcCorr = 0;
  fTrackBranch = "";
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

  Register();
   
  fCorrPar->printParams();

  geoH = new PndMvdGeoHandling(gGeoManager);

  if (fGeanePro)
    {
      cout << "-I- PndPidCorrelator::Init: Using Geane for Track propagation" << endl;
    }
  
  if (fDebugMode)
    {
      r = TFile::Open(sDir+sFile,"RECREATE");
      
      tofCorr = new TNtuple("tofCorr","TRACK-TOF Correlation",
			    "track_x:track_y:track_z:track_phi:track_p:track_charge:track_theta:track_z0:tof_x:tof_y:tof_z:tof_phi:chi2:dphi:len:glen");
      emcCorr = new TNtuple("emcCorr","TRACK-EMC Correlation",
			    "track_x:track_y:track_z:track_phi:track_p:track_charge:track_theta:track_z0:emc_x:emc_y:emc_z:emc_phi:chi2:dphi:emc_ene:glen:emc_mod");
      mdtCorr = new TNtuple("mdtCorr","TRACK-MDT Correlation",
			    "track_x:track_y:track_z:track_phi:track_p:track_charge:track_theta:track_z0:mdt_x:mdt_y:mdt_z:mdt_phi:chi2:mdt_mod:dphi:glen");
      drcCorr = new TNtuple("drcCorr","TRACK-DRC Correlation",
			    "track_x:track_y:track_z:track_phi:track_p:track_charge:track_theta:track_z0:drc_x:drc_y:drc_phi:chi2:drc_thetac:drc_nphot:dphi:glen");
      cout << "-I- PndPidCorrelator::Init: Filling Debug histograms" << endl;
      
    }
  cout << "-I- PndPidCorrelator::Init: Success!" << endl;
  
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
  cout << " =====   PndPidCorrelator: ";
  Reset();
  
  if (fTrack) {
    cout << " Number of tracks for pid " << fTrack->GetEntriesFast();
    ConstructChargedCandidate();
  }
  
  if (fEmcMode>0) {
    cout << " - Number of EMC Cluster for pid " << fEmcCluster->GetEntriesFast();
    ConstructNeutralCandidate();
  }
  cout << endl;
}

//______________________________________________________
void PndPidCorrelator::ConstructChargedCandidate() {
  //-
  fPidChargedCand->Delete();
  
  Int_t nTracks = fTrack->GetEntriesFast();
  for (Int_t i = 0; i < nTracks; i++) {
    PndTrack* track = (PndTrack*) fTrack->At(i);
    Int_t ierr = 0;
    FairTrackParP par = track->GetParamLast();
    if ((par.GetMomentum().Mag()<0.1) || (par.GetMomentum().Mag()>15.) )continue;
    FairTrackParH *helix = new FairTrackParH(&par, ierr);
    
    PndPidCandidate* pidCand = 	new PndPidCandidate();
    
    GetTrackInfo(track, pidCand);
    GetMvdInfo(track, pidCand); 
    if ( (fSttMode==3) && (fSttHit    ->GetEntriesFast()>0) ) GetSttInfo(track, pidCand);
    if ( (fTofMode==2) && (fTofHit    ->GetEntriesFast()>0) ) GetTofInfo(helix, pidCand);
    if ( (fEmcMode>0)  && (fEmcCluster->GetEntriesFast()>0) ) GetEmcInfo(helix, pidCand);
    if ( (fMdtMode>0)  && (fMdtHit    ->GetEntriesFast()>0) ) GetMdtInfo(helix, pidCand);  
    if ( (fDrcMode>0)  && (fDrcHit    ->GetEntriesFast()>0) ) GetDrcInfo(helix, pidCand);
    
    AddChargedCandidate(pidCand);
  } 
}

//______________________________________________________
void PndPidCorrelator::ConstructNeutralCandidate() {
  //-
  fPidNeutralCand->Delete();
  
  Int_t nBumps;
  if (fEmcMode==2) 
    {
      nBumps = fEmcCluster->GetEntriesFast();
    }
  else
    {
      nBumps = fEmcBump->GetEntriesFast();
    }
  
  for (Int_t i = 0; i < nBumps; i++)
    {
      PndEmcBump* bump;
      if (fEmcMode==2) 
	{
	  bump = (PndEmcBump*) fEmcCluster->At(i);
	}
      else
	{
	  bump = (PndEmcBump*) fEmcBump->At(i);
	  if (fClusterList[bump->GetClusterIndex()]) continue; // skip correlated clusters
	}
      
      TVector3 vtx(0,0,0);
      TVector3 v1=bump->where();
      TVector3 p3;
      TMatrixD covP4=bump->Get4MomentumErrorMatrix();
      p3.SetMagThetaPhi(bump->GetEnergyCorrected(), v1.Theta(), v1.Phi());
      TLorentzVector lv(p3,0.);
      
      PndPidCandidate* pidCand = new PndPidCandidate(0, vtx, lv); 
      pidCand->SetEmcRawEnergy(bump->energy());
      pidCand->SetEmcCalEnergy(bump->GetEnergyCorrected());
      pidCand->SetEmcIndex(i);
      AddNeutralCandidate(pidCand);
    }
}
//_________________________________________________________________
void PndPidCorrelator::GetTrackInfo(PndTrack* track, PndPidCandidate* pidCand) 
{
  pidCand->SetCharge((Int_t)TMath::Sign(1., track->GetParamFirst().GetQ()));
     
  TVector3 first(track->GetParamFirst().GetX(),
		 track->GetParamFirst().GetY(),
		 track->GetParamFirst().GetZ());
  TVector3 last(track->GetParamLast().GetX(),
		track->GetParamLast().GetY(),
		track->GetParamLast().GetZ());
  FairTrackParP par = track->GetParamFirst();
  Int_t ierr = 0;
  FairTrackParH *helix = new FairTrackParH(&par, ierr);
  TVector3 momentum, vertex;
  Float_t ex = ExtrapolateToZ(helix, &momentum, &vertex); // Extrapolation to z=0a
  Float_t energy = TMath::Sqrt(momentum.Mag2()+138.*138.);
  if (fGeanePro) // Overwrites vertex if Geane is used
    {
      FairGeanePro *fPro0 = new FairGeanePro();
      FairTrackParH *fRes= new FairTrackParH();
      fPro0->SetPoint(TVector3(0,0,0));
      fPro0->PropagateToPCA(1, -1);
      Bool_t rc =  fPro0->Propagate(helix, fRes, -13*(Int_t)helix->GetQ());	
      if (rc)
	{
	  vertex.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
          Int_t ierr = 0;
          FairTrackParP *fParab = new FairTrackParP(fRes, TVector3(1.,0.,0.), TVector3(0.,1.,0.), ierr);
          Double_t globalCov[6][6];
	  fParab->GetMARSCov(globalCov);
	  TMatrixD mat(7,7);
	  Int_t ii,jj;
	  for (ii=0;ii<6;ii++) for(jj=0;jj<6;jj++) mat[ii][jj]=globalCov[ii][jj];

          energy = TMath::Sqrt(fParab->GetMomentum().Mag2()+138.*138.);
          //Extend matrix for energy (with default pion hypothesis) -> Klaus Goetzen
          Double_t invE = 1./(energy);
          mat[0+3][3+3] = mat[3+3][0+3] =
(fRes->GetX()*mat[0+3][0+3]+fRes->GetY()*mat[0+3][1+3]+fRes->GetZ()*mat[0+3][2+3])*invE;
          mat[1+3][3+3] = mat[3+3][1+3] =
(fRes->GetX()*mat[0+3][1+3]+fRes->GetY()*mat[1+3][1+3]+fRes->GetZ()*mat[1+3][2+3])*invE;
          mat[2+3][3+3] = mat[3+3][2+3] =
(fRes->GetX()*mat[0+3][2+3]+fRes->GetY()*mat[1+3][2+3]+fRes->GetZ()*mat[2+3][2+3])*invE;
          mat[3+3][3+3] =
(fRes->GetX()*fRes->GetX()*mat[0+3][0+3]+fRes->GetY()*fRes->GetY()*mat[1+3][1+3]+fRes->GetZ()*fRes->GetZ()*mat[2+3][2+3]
		  	+2.0*fRes->GetX()*fRes->GetY()*mat[0+3][1+3]
	  		+2.0*fRes->GetX()*fRes->GetZ()*mat[0+3][2+3]
  			+2.0*fRes->GetY()*fRes->GetZ()*mat[1+3][2+3])*invE*invE;
			
          mat[3+3][4-4] = mat[4-4][3+3] =
(fRes->GetX()*mat[0+3][4-4]+fRes->GetY()*mat[1+3][4-4]+fRes->GetZ()*mat[2+3][4-4])*invE;
          mat[3+3][5-4] = mat[5-4][3+3] =
(fRes->GetX()*mat[0+3][5-4]+fRes->GetY()*mat[1+3][5-4]+fRes->GetZ()*mat[2+3][5-4])*invE;
          mat[3+3][6-4] = mat[6-4][3+3] =
(fRes->GetX()*mat[0+3][6-4]+fRes->GetY()*mat[1+3][6-4]+fRes->GetZ()*mat[2+3][6-4])*invE;

          pidCand->SetCov7(mat);
	}
      else
	{
	  vertex.SetXYZ(-10000., -10000., -10000.);
	}
    }
  pidCand->SetPosition(vertex);
  pidCand->SetMomentum(momentum);
  pidCand->SetEnergy(energy);
  pidCand->SetFirstHit(first);
  pidCand->SetLastHit(last);
  //pidCand->SetMcIndex(); // ??? dummy, for the moment
  
  pidCand->SetDegreesOfFreedom(track->GetNDF());
  pidCand->SetFitStatus(track->GetFlag());
  pidCand->SetChiSquared(track->GetChi2());
}

//_________________________________________________________________
void PndPidCorrelator::GetMvdInfo(PndTrack* track, PndPidCandidate* pidCand) 
{
  Float_t fMvdELoss = 0.; // total energy lost in MVD;
  Float_t fMvdPath = 0.;  // total thickness crossed in MVD
  Int_t mvdCounts = 0;
  PndTrackCand trackCand = track->GetTrackCand();
  for (Int_t ii=0; ii<trackCand.GetNHits(); ii++)
    {
      PndMvdHit *mvdHit = NULL;
      PndTrackCandHit candHit = trackCand.GetSortedHit(ii);
      if ( (candHit.GetDetId()!=kMVDHitsPixel) || (candHit.GetDetId()!=kMVDHitsStrip) ) continue;
      
      if (candHit.GetDetId()==kMVDHitsPixel) mvdHit = (PndMvdHit*)fMvdHitsPixel->At(candHit.GetHitId());
      if (candHit.GetDetId()==kMVDHitsStrip) mvdHit = (PndMvdHit*)fMvdHitsStrip->At(candHit.GetHitId());
      TVector3 mvdPos;
      mvdHit->Position(mvdPos);
      mvdCounts++;
      TGeoNode *mvdNode = (TGeoNode*)gGeoManager->FindNode(mvdHit->GetX(), mvdHit->GetY(), mvdHit->GetZ());
      TGeoVolume *mvdVol = (TGeoVolume*)mvdNode->GetVolume();
      TGeoBBox* actBox = (TGeoBBox*)(mvdVol->GetShape()); // volume of the MVD strip/pixel
      TGeoMatrix* mvdGeoRot = (TGeoMatrix*)mvdNode->GetMatrix();
      const Double_t *rotM = mvdGeoRot->GetRotationMatrix();
      TVector3 zaxis(rotM[2], rotM[5], rotM[8]); // Z axis in the detector frame
      TVector3 vertex(0., 0., 0.);
      TVector3 momentum(0., 0., 0.);
      FairTrackParP par = track->GetParamLast();
      Int_t ierr = 0;
      FairTrackParH *helix = new FairTrackParH(&par, ierr);
      Float_t ex = ExtrapolateToZ(helix, &momentum, &vertex, mvdHit->GetZ()); // track momentum at the strip/pixel
      Double_t cos = 0.;
      if (ex) cos = TMath::Cos(momentum.Angle(zaxis)); // cos of the angle between the track and the strip/pixel normal axis
      
      if (fGeanePro) // Overwrites vertex if Geane is used
    	{
	  FairGeanePro *fProMvd = new FairGeanePro();
	  fProMvd->SetPoint(mvdPos);
	  fProMvd->PropagateToPCA(1, 1);
          vertex.SetXYZ(-10000, -10000, -10000); // reset vertex
          FairTrackParH *fRes= new FairTrackParH();
          Bool_t rc =  fProMvd->Propagate(helix, fRes, -13*(Int_t)helix->GetQ()); // First propagation at module
          if (rc)
	    {
	      cos = TMath::Cos(fRes->GetMomentum().Angle(zaxis)); 
	    }
	  else
	    {
	      cos = 0.;
	    }
	}
      Float_t thickness = 0.;
      
      if (fabs(cos)<0.000001)
	{
	  cout << "-W- PndPidCorrelator::GetMvdInfo: Track perpendicular to MVD strip/pixel! Not added to MVD eloss" << endl;     
	}
      else
	{
	  thickness = actBox->GetDZ()*2./fabs(cos);
	  fMvdELoss += mvdHit->GetEloss();
	  fMvdPath += thickness;
	  fMvdHitCount++;
	}
      if (fVerbose) cout << mvdHit->GetDetName() << "\t" << mvdHit->GetEloss() << "\t" << thickness << endl;
    }
  
  if (fMvdPath>0.) pidCand->SetMvdDEDX(fMvdELoss/fMvdPath);
  //pidCand->SetMvdHits(mvdCounts);
}

//_________________________________________________________________
void PndPidCorrelator::GetSttInfo(PndTrack* track, PndPidCandidate* pidCand) {
 
  std::vector<Double_t> dedxvec;
  dedxvec.clear();
  
  Int_t sttCounts = 0;
  PndTrackCand trackCand = track->GetTrackCand();
  for (Int_t ii=0; ii<trackCand.GetNHits(); ii++)
    {
      PndSttHelixHit *sttHit = NULL;
      PndTrackCandHit candHit = trackCand.GetSortedHit(ii);
      if (candHit.GetDetId()!=kSttHelixHit) continue;
      sttHit = (PndSttHelixHit*)fSttHit->At(candHit.GetHitId());
      if (sttHit->GetdEdx() != 0.) 
	{
	  dedxvec.push_back(sttHit->GetdEdx());
	  sttCounts++;
	}
    }
  
  if( sttCounts > 0) {
    // truncated mean
    Double_t perc = 0.60;
    // sort
    std::sort(dedxvec.begin(), dedxvec.end());
    
    //truncated mean
    Double_t sum = 0;
    Int_t endnum = int(floor(sttCounts * perc));
    
    for(Int_t m = 0; m < endnum; m++) sum += dedxvec[m];
    
    if(endnum > 0) {
      pidCand->SetSttMeanDEDX(sum/(Double_t) endnum);
      
    }
  }
  //pidCand->SetSttHits(sttCounts);
}

//_________________________________________________________________
void PndPidCorrelator::GetTofInfo(FairTrackParH* helix, PndPidCandidate* pidCand) {

  if ((helix->GetMomentum().Theta()*TMath::RadToDeg())<20.) return; 
  if ((helix->GetMomentum().Theta()*TMath::RadToDeg())>150.) return;
  
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
      tofHit->Position(tofPos);

       Float_t ex = ExtrapolateToR(helix, &momentum, &vertex, fCorrPar->GetTofRadius()); // Important even to calculate phi for path length
      
      if (fGeanePro) // Overwrites vertex if Geane is used
	{ 
	  FairGeanePro *fProTof = new FairGeanePro();
	  fProTof->SetPoint(tofPos);
	  fProTof->PropagateToPCA(1, 1);
	  FairTrackParH *fRes= new FairTrackParH();
	  Bool_t rc =  fProTof->Propagate(helix, fRes, -13*(Int_t)helix->GetQ());	
	  if (rc)
	    {
	      vertex.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
	      tofGLength = fProTof->GetLengthAtPCA();
	    }
	  else
	    {
	      vertex.SetXYZ(-10000., -10000., -10000.);
	    }
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
  
  if (tofQuality<fCorrPar->GetTofCut())
    {
      pidCand->SetTofQuality(tofQuality);
      pidCand->SetTofStopTime(tofTof);
      pidCand->SetTofTrackLength(tofLength);
      pidCand->SetTofIndex(tofIndex);
    }
  
}

//_________________________________________________________________
void PndPidCorrelator::GetEmcInfo(FairTrackParH* helix, PndPidCandidate* pidCand) { 
  //---
  Float_t trackTheta = helix->GetMomentum().Theta()*TMath::RadToDeg();
  PndEmcCluster *emcHit = NULL;
  Int_t emcEntries = fEmcCluster->GetEntriesFast();
  Int_t emcIndex = -1, emcModuleCorr = -1;
  Float_t emcEloss = 0., emcElossCorr = 0., emcGLength = -1000;
  Float_t emcQuality = 1000000;

  Float_t chi2 = 0;
  TVector3 vertex(0., 0., 0.);
  TVector3 emcPos(0., 0., 0.);
  TVector3 momentum(0., 0., 0.);
  for (Int_t ee = 0; ee<emcEntries; ee++)
    {
      emcHit = (PndEmcCluster*)fEmcCluster->At(ee);
      if (emcHit->energy() < fCorrPar->GetEmc12Thr()) continue;
      Int_t emcModule = ((PndEmcDigi*)emcHit->Maxima())->GetModule();
      if (emcModule>4) continue;
      if ( (trackTheta>130.) && ((emcModule==1)||(emcModule==3)) ) continue;
      if ( (trackTheta<130.) && (emcModule==4) ) continue;
      if ( (trackTheta<40.)  && ((emcModule==2)||(emcModule==4)) ) continue;
      
      emcPos = emcHit->where();
      Float_t ex = ExtrapolateToR(helix, &momentum, &vertex, fCorrPar->GetEmc12Radius());

      if (fGeanePro) // Overwrites vertex if Geane is used
    	{
	  FairGeanePro *fProEmc = new FairGeanePro();
	  fProEmc->SetPoint(emcPos);
	  fProEmc->PropagateToPCA(1, 1);
          vertex.SetXYZ(-10000, -10000, -10000); // reset vertex
          FairTrackParH *fRes= new FairTrackParH();
          Bool_t rc =  fProEmc->Propagate(helix, fRes, -13*pidCand->GetCharge()); // First propagation at module
          if (rc)
	    {
	      emcGLength = fProEmc->GetLengthAtPCA();
	      vertex.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
	    }
          //std::map<PndEmcTwoCoordIndex*, PndEmcXtal*> tciXtalMap=PndEmcStructure::Instance()->GetTciXtalMap();
          //PndEmcDigi *lDigi= (PndEmcDigi*)emcHit->Maxima();
          //PndEmcXtal* xtal = tciXtalMap[lDigi->GetTCI()];
          //emcPos = xtal->frontCentre();
        }
      Float_t dist = (emcPos-vertex).Mag2();
      if ( emcQuality > dist )
	{
	  emcIndex = ee;
	  emcQuality = dist;
	  emcEloss = emcHit->energy();
	  emcElossCorr = emcHit->GetEnergyCorrected();
	  emcModuleCorr == emcModule;
	}

      if (fDebugMode)
	{
	  Float_t ntuple[] = {vertex.X(), vertex.Y(), vertex.Z(), vertex.Phi(),
			      helix->GetMomentum().Mag(), helix->GetQ(), helix->GetMomentum().Theta(), helix->GetZ(),
			      emcPos.X(), emcPos.Y(), emcPos.Z(), emcPos.Phi(),
			      dist, vertex.DeltaPhi(emcPos), emcHit->energy(), emcGLength, emcModule};
	  emcCorr->Fill(ntuple);
	}
    }
  if (emcQuality < fCorrPar->GetEmc12Cut())
    {
      fClusterList[emcIndex] = kTRUE;
      pidCand->SetEmcQuality(emcQuality);
      pidCand->SetEmcRawEnergy(emcEloss);
      pidCand->SetEmcCalEnergy(emcElossCorr);
      pidCand->SetEmcIndex(emcIndex);
      pidCand->SetEmcModule(emcModuleCorr);
    }
}

//_________________________________________________________________
void PndPidCorrelator::GetMdtInfo(FairTrackParH* helix, PndPidCandidate* pidCand) {
  //---
  PndMdtHit *mdtHit = NULL;
  Int_t mdtEntries = fMdtHit->GetEntriesFast();
  Int_t mdtIndex = -1, mdtMod = 0;
  Float_t mdtGLength = -1000;
  Float_t mdtQuality = 1000000;
  
  Float_t chi2 = 0;
  TVector3 vertex(0., 0., 0.);
  TVector3 mdtPos(0., 0., 0.);
  TVector3 momentum(0., 0., 0.);
  for (Int_t mm = 0; mm<mdtEntries; mm++)
    {
      mdtHit = (PndMdtHit*)fMdtHit->At(mm);
      if (mdtHit->GetLayerID()!=0) continue;
      if (mdtHit->GetModule()>2) continue;
      
      mdtHit->Position(mdtPos);
      Float_t ex = ExtrapolateToR(helix, &momentum, &vertex, fCorrPar->GetMdtRadius());

      if (fGeanePro) // Overwrites vertex if Geane is used
	{ 
	  FairGeanePro *fProMdt = new FairGeanePro();
	  fProMdt->SetPoint(mdtPos);
	  fProMdt->PropagateToPCA(1, 1);
          vertex.SetXYZ(-10000, -10000, -10000); // reset vertex
	  FairTrackParH *fRes= new FairTrackParH();
	  Bool_t rc =  fProMdt->Propagate(helix, fRes, -13*(Int_t)helix->GetQ()); 
	  if (rc)
	    {
	      vertex.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
	      mdtGLength = fProMdt->GetLengthAtPCA();
	    }
	}
      
      Float_t dist = (mdtPos-vertex).Mag2();
      
      if ( mdtQuality > dist)
	{
	  mdtIndex = mm;
	  mdtQuality = dist;
	  mdtMod = mdtHit->GetModule();
	}
      if (fDebugMode)
	{
	  Float_t ntuple[] = {vertex.X(), vertex.Y(), vertex.Z(), vertex.Phi(), 
			      helix->GetMomentum().Mag(), helix->GetQ(), helix->GetMomentum().Theta(), helix->GetZ(),
			      mdtPos.X(), mdtPos.Y(), mdtPos.Z(), mdtPos.Phi(),
			      dist, mdtHit->GetModule(), vertex.DeltaPhi(mdtPos), mdtGLength};
	  mdtCorr->Fill(ntuple);
	}
    }
  
  if (mdtQuality<fCorrPar->GetMdtCut())
    {
      //pidCand->SetMuoIndex(mdtIndex);
      pidCand->SetMuoQuality(mdtQuality);
      //track->SetMdtModule(mdtMod);
      pidCand->SetMuoNumberOfLayers(1);
    }
  
}

//_________________________________________________________________
void PndPidCorrelator::GetDrcInfo(FairTrackParH* helix, PndPidCandidate* pidCand) {
  if ((helix->GetMomentum().Theta()*TMath::RadToDeg())<20.) return;
 
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
           
      drcHit->Position(drcPos);
      Float_t ex = ExtrapolateToR(helix, &momentum, &vertex, fCorrPar->GetDrcRadius());

      if (fGeanePro) // Overwrites vertex if Geane is used
	{
	  FairGeanePro *fProDrc = new FairGeanePro();
	  fProDrc->SetPoint(drcPos);
	  fProDrc->PropagateToPCA(1, 1);
          vertex.SetXYZ(-10000, -10000, -10000); // reset vertex
	  FairTrackParH *fRes= new FairTrackParH();
	  Bool_t rc =  fProDrc->Propagate(helix, fRes, -13*(Int_t)helix->GetQ()); 	
	  if (rc)
	    {
	      vertex.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
	      drcGLength = fProDrc->GetLengthAtPCA();
	    }
	  else
	    {
	      vertex.SetXYZ(-10000., -10000., -10000.);
	    }
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
  
  if (drcQuality<fCorrPar->GetDrcCut())
    {
      pidCand->SetDrcQuality(drcQuality);
      pidCand->SetDrcThetaC(drcThetaC);
      pidCand->SetDrcThetaCErr(drcThetaCErr);
      pidCand->SetDrcNumberOfPhotons(drcPhot);
      pidCand->SetDrcIndex(drcIndex);
    }
  
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
  Double_t Q = TMath::Sign(1., helix->GetQ());

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


ClassImp(PndPidCorrelator)
