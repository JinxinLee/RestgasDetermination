#include "PndLhePidMaker.h"
#include "PndTpcLheTrack.h"
#include "PndTpcLheHit.h"
#include "PndLhePidTrack.h"

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
#include "CbmTrackParH.h"
#include "CbmMCApplication.h"
#include "CbmRunAna.h"
#include "CbmRootManager.h"
#include "CbmRuntimeDb.h"

#include "TObjArray.h"
#include "TVector3.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "TGeoManager.h"

#include <cmath>

// ---------------------------------------------------------------
// --- Interface with PidMaker and output ---


PndLhePidMaker* PndLhePidMaker::ftInstance = NULL;

//___________________________________________________________
PndLhePidMaker* PndLhePidMaker::Instance() {
  //---
  return ftInstance;
}

//___________________________________________________________
PndLhePidMaker::~PndLhePidMaker() {
  //
  CbmRootManager *fManger =CbmRootManager::Instance();
  fManger->Write();
}

//___________________________________________________________
PndLhePidMaker::PndLhePidMaker() {
  //---
  fPidTrackCand = new TClonesArray("PndLhePidTrack");
  fDebugMode = kFALSE;
  fMvdMode = 0;
  fSttMode = 0;
  fTofMode = 0;
  fEmcMode = 0;
  fMdtMode = 0; 
  fDrcMode = 0;
  fVerbose = kFALSE;
  fSimulation = kFALSE;
  if( !ftInstance ) ftInstance = this;
  tofCorr = 0;
  emcCorr = 0; 
  drcCorr = 0;
  sDir = "./";
  sFile = "./lhepidmaker.root";
  Reset();
}

//___________________________________________________________
PndLhePidMaker::PndLhePidMaker(const char *name, const char *title)
  :CbmTask(name) {
  //---
  fPidTrackCand = new TClonesArray("PndLhePidTrack");
  fMvdMode = 0;  
  fSttMode = 0;
  fTofMode = 0;
  fEmcMode = 0;
  fMdtMode = 0;
  fDrcMode = 0;
  fVerbose = kFALSE;
  fSimulation = kFALSE;
  if( !ftInstance ) ftInstance = this; 
  tofCorr = 0;
  emcCorr = 0;
  drcCorr = 0;
  sDir = "./";
  sFile = "./lhepidmaker.root";
  Reset();
}

//___________________________________________________________
InitStatus PndLhePidMaker::Init() {

  //  cout << "InitStatus PndLhePidMaker::Init()" << endl;

  CbmRootManager *fManager =CbmRootManager::Instance();	

  fTrackCand = (TClonesArray *)fManager->GetObject("PndTpcLheTrack");
  if ( ! fTrackCand ) {
    cout << "-I- PndLhePidMaker::Init: No PndTpcLheTrack array!" << endl;
    return kERROR;
  }
  
   // *** STT ***
  fSttHit = (TClonesArray*) fManager->GetObject("SttHelixHit");
  if ( fSttHit ) 
    {
      cout << "-I- PndLhePidMaker::Init: Using SttHelixHit" << endl;
      fSttMode = 3;
    }
  else
    {
      fSttHit = (TClonesArray*) fManager->GetObject("SttHit");
      if ( fSttHit ) 
	{
	  cout << "-I- PndLhePidMaker::Init: Using SttHit" << endl;
	  fSttMode = 2;
	}
    }
  if (fSttMode ==0)
    {
      cout << "-W- PndLhePidMaker::Init: No STT hits array! Switching STT OFF" << endl;
    }
  
  fMvdHitsPixel = (TClonesArray*) fManager->GetObject("MVDHitsPixel");
  if ( ! fMvdHitsPixel ) 
    {
      cout << "-W- PndLhePidMaker::Init: No MVDHitsPixel array!" << endl;
    }
  else fMvdMode = 2;
  if (( ! fMvdHitsStrip ) &&  ( ! fMvdHitsPixel ))
    {
      cout << "-W- PndLhePidMaker::Init: No MDC hits array! Switching MVD OFF" << endl;
      fMvdMode = 0;
    }
  else
    {
	cout << "-I- PndLhePidMaker::Init: Using MVDHit" << endl;
    }
  // *** MVD ***
  fMvdHitsStrip = (TClonesArray*) fManager->GetObject("MVDHitsStrip");
  if ( ! fMvdHitsStrip ) 
    {
      cout << "-W- PndLhePidMaker::Init: No MVDHitsStrip array!" << endl;
    }
  else fMvdMode = 2;
  
  fMvdHitsPixel = (TClonesArray*) fManager->GetObject("MVDHitsPixel");
  if ( ! fMvdHitsPixel ) 
    {
      cout << "-W- PndLhePidMaker::Init: No MVDHitsPixel array!" << endl;
    }
  else fMvdMode = 2;
  if (( ! fMvdHitsStrip ) &&  ( ! fMvdHitsPixel ))
    {
      cout << "-W- PndLhePidMaker::Init: No MDC hits array! Switching MVD OFF" << endl;
      fMvdMode = 0;
    }
  else
    {
	cout << "-I- PndLhePidMaker::Init: Using MVDHit" << endl;
    }
  
  // *** TOF ***
  fTofHit = (TClonesArray*) fManager->GetObject("TofHit");
  if ( ! fTofHit ) 
    {
      cout << "-W- PndLhePidMaker::Init: No TofHit array!" << endl;
      fTofMode = 0;
    }
  else  
    {
      cout << "-I- PndLhePidMaker::Init: Using TofHit" << endl;
      fTofMode = 2;
    }
  
  // *** EMC ***
  /*
  fEmcCluster = (TClonesArray*) fManager->GetObject("EmcBump");
  if ( ! fEmcCluster ) 
    {
      cout << "-W- PndLhePidMaker::Init: No EmcBump array!" << endl;
    }
  else fEmcMode = 3;
  */
  fEmcCluster = (TClonesArray*) fManager->GetObject("EmcCluster");
  if ( ! fEmcCluster ) 
    {
      cout << "-W- PndLhePidMaker::Init: No EmcCluster array!" << endl;
      fEmcMode = 0;
    }
  else 
    {
      cout << "-I- PndLhePidMaker::Init: Using EmcCluster" << endl;
      fEmcMode = 2;
    }

  // *** MDT ***
  fMdtHit = (TClonesArray*) fManager->GetObject("MdtHit");
  if ( ! fMdtHit ) 
    {
      cout << "-W- PndLhePidMaker::Init: No MdtHit array!" << endl;
      fMdtMode = 0;
    }
  else  
    {
      cout << "-I- PndLhePidMaker::Init: Using MdtHit" << endl;
      fMdtMode = 2;
    }

  // *** DRC ***
  fDrcHit = (TClonesArray*) fManager->GetObject("DrcHit");
  if ( ! fDrcHit ) 
    {
      cout << "-W- PndLhePidMaker::Init: No DrcHit array!" << endl;
      fDrcMode = 0;
    }
  else  
    {
      cout << "-I- PndLhePidMaker::Init: Using DrcHit" << endl;
      fDrcMode = 2;
    }

  Register();
   
  fCorrPar->printParams();

  geoH = new PndMvdGeoHandling(gGeoManager);

  if (fGeanePro)
    {
      fPro = new CbmGeanePro();
      cout << "-I- PndLhePidMaker::Init: Using Geane for Track propagation" << endl;
    }
  
  if (fDebugMode)
    {
      r = TFile::Open(sDir+sFile,"RECREATE");
      
      tofCorr = new TNtuple("tofCorr","TRACK-TOF Correlation",
			    "track_x:track_y:track_z:track_phi:track_p:track_charge:track_theta:track_z0:tof_x:tof_y:tof_z:tof_phi:chi2:dphi:len:glen");
      emcCorr = new TNtuple("emcCorr","TRACK-EMC Correlation",
			    "track_x:track_y:track_z:track_phi:track_p:track_charge:track_theta:track_z0:emc_x:emc_y:emc_z:emc_phi:chi2:dphi:emc_ene:glen");
      mdtCorr = new TNtuple("mdtCorr","TRACK-MDT Correlation",
			    "track_x:track_y:track_z:track_phi:track_p:track_charge:track_theta:track_z0:mdt_x:mdt_y:mdt_z:mdt_phi:chi2:mdt_mod:dphi:glen");
      drcCorr = new TNtuple("drcCorr","TRACK-DRC Correlation",
			    "track_x:track_y:track_z:track_phi:track_p:track_charge:track_theta:track_z0:drc_x:drc_y:drc_phi:chi2:drc_thetac:drc_nphot:dphi:glen");
      cout << "-I- PndLhePidMaker::Init: Filling Debug histograms" << endl;
      
    }
  cout << "-I- PndLhePidMaker::Init: Success!" << endl;
  
  return kSUCCESS;
}

//______________________________________________________
void PndLhePidMaker::SetParContainers() {

  // Get run and runtime database
  CbmRunAna* run = CbmRunAna::Instance();
  if ( ! run ) Fatal("PndLhePidMaker:: SetParContainers", "No analysis run");

  CbmRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("PndLhePidMaker:: SetParContainers", "No runtime database");

  // Get LHE Correlation parameter container
  fCorrPar = (PndLheCorrPar*) db->getContainer("PndLheCorrPar");
  
}

//______________________________________________________
void PndLhePidMaker::Exec(Option_t * option) {
  //-
  cout << " =====   PndLhePidMaker: ";

  fPidTrackCand->Delete();
  
  if (fTrackCand) {
    cout << " Number of tracks for pid " <<
      fTrackCand->GetEntriesFast() << endl;
  }

  Int_t nTracks = fTrackCand->GetEntriesFast();

  for (Int_t i = 0; i < nTracks; i++) {
    Reset();
    
    PndTpcLheTrack* track = (PndTpcLheTrack*) fTrackCand->At(i);
    if (track->IsGood()==kFALSE) continue;
    
    PndLhePidTrack* pidTrack = 	new PndLhePidTrack(*track);
        
    if (fVerbose) cout << "\n\n Track " << i << "\n"; 
    TObjArray* lheList = track->GetRHits();
    if (fVerbose) cout << lheList->GetEntries() << " " << track->GetNumberOfHits() << endl;
    for (Int_t lh=0; lh < lheList->GetEntries(); lh++)
      {
	PndTpcLheHit* lhit = (PndTpcLheHit*)lheList->At(lh);
	if (fVerbose) cout << lhit->GetX() << "\t" << lhit->GetY() << "\t" <<lhit->GetZ() << endl;
	
	if ((fMvdMode==2) &&
	    ((lhit->GetDetectorId()==kMVDHitsStrip) || (lhit->GetDetectorId()==kMVDHitsPixel)))
	  GetMvdInfo(lhit, pidTrack);
	if ((fSttMode==3) &&
	    ((lhit->GetDetectorId()==kSttHelixHit)))
	  GetSttInfo(lhit, pidTrack);
	
      }
    
    pidTrack->SetMvdELoss(fMvdELoss);
    pidTrack->SetMvdPath(fMvdPath);
    pidTrack->SetMvdHitCounts(fMvdHitCount);
    
    pidTrack->SetSttELoss(fSttELoss);
    pidTrack->SetSttHitCounts(fSttHitCount);
    
    if (fTofMode==2) GetTofInfo(pidTrack);
    if (fEmcMode>0)  GetEmcInfo(pidTrack);
    if (fMdtMode>0)  GetMdtInfo(pidTrack);  
    if (fDrcMode>0)  GetDrcInfo(pidTrack);
    
    AddTrack(pidTrack);
    
  } 
  
#if 0
  /** Constructor with all track variables  (x,y,z in SC) **/
  CbmTrackParH(Double_t x,  Double_t y,  Double_t z,
	       Double_t lambda, Double_t phi, Double_t qp,
	       Double_t CovMatrix[15]);

  /** Constructor track parameters with position (LAB) momentum **/
  
  CbmTrackParH(TVector3 pos, TVector3 Mom, TVector3 posErr,
	       TVector3 MomErr, Double_t q);

#endif


}

//_________________________________________________________________
void PndLhePidMaker::GetMvdInfo(const PndTpcLheHit* hit, PndLhePidTrack* track) {
  //---
  PndMvdHit *mvdHit = NULL;
  if (hit->GetDetectorId()==kMVDHitsPixel) mvdHit = (PndMvdHit*)fMvdHitsPixel->At(hit->GetRefIndex());
  if (hit->GetDetectorId()==kMVDHitsStrip) mvdHit = (PndMvdHit*)fMvdHitsStrip->At(hit->GetRefIndex());
  
  TGeoNode *mvdNode = (TGeoNode*)gGeoManager->FindNode(mvdHit->GetX(), mvdHit->GetY(), mvdHit->GetZ());
  TGeoVolume *mvdVol = (TGeoVolume*)mvdNode->GetVolume();
  TGeoBBox* actBox = (TGeoBBox*)(mvdVol->GetShape()); // volume of the MVD strip/pixel
  TGeoMatrix* mvdGeoRot = (TGeoMatrix*)mvdNode->GetMatrix();
  const Double_t *rotM = mvdGeoRot->GetRotationMatrix();
  TVector3 zaxis(rotM[2], rotM[5], rotM[8]); // Z axis in the detector frame
  TVector3 vertex(0., 0., 0.);
  TVector3 momentum(0., 0., 0.);
  Float_t phi = track->ExtrapolateToZ(&momentum, &vertex, mvdHit->GetZ()); // track momentum at the strip/pixel
  Double_t cos = TMath::Cos(momentum.Angle(zaxis)); // cos of the angle between the track and the strip/pixel normal axis
  Float_t thickness = 0.;
  
  if (fabs(cos)<0.000001)
    {
      cout << "-W- PndLhePidMaker::GetMvdInfo: Track perpendicular to MVD strip/pixel! Not added to MVD eloss" << endl;     
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

//_________________________________________________________________
void PndLhePidMaker::GetSttInfo(const PndTpcLheHit* hit, const PndLhePidTrack* track) {
  //---
  PndSttHelixHit *sttHit = NULL;
  
  if (hit->GetDetectorId()==kSttHelixHit) sttHit = (PndSttHelixHit*)fSttHit->At(hit->GetRefIndex());
  
  //if (fVerbose) cout << sttHit->GetDetName() << "\t" << sttHit->GetEloss() << endl;
  
  //fSttELoss += sttHit->GetEloss();
  fSttHitCount++;
}

//_________________________________________________________________
void PndLhePidMaker::GetTofInfo(PndLhePidTrack* track) {

  if ((track->GetMomentum().Theta()*TMath::RadToDeg())<20.) return; 
  if ((track->GetMomentum().Theta()*TMath::RadToDeg())>150.) return;
  if (fGeanePro) fPro->PropagateToVolume("tofB01",0,1);
  
  //---
  PndTofHit *tofHit = NULL;
  Int_t tofEntries = fTofHit->GetEntriesFast();
  Int_t tofIndex = -1;
  Float_t tofTof = 0., tofDz = -1000, tofDphi = -1000, tofLength = -1000, tofGLength = -1000;
  Float_t tofQuality = 1000000;
  
  Float_t chi2 = 0;
  TVector3 vertex(0., 0., 0.);
  TVector3 tofPos(0., 0., 0.);
  TVector3 momentum(0., 0., 0.);
  for (Int_t tt = 0; tt<tofEntries; tt++)
    {
      tofHit = (PndTofHit*)fTofHit->At(tt);
      tofHit->Position(tofPos);

      Float_t phi = track->ExtrapolateToR(&momentum, &vertex, fCorrPar->GetTofRadius()); // Important even to calculate phi for path length
      
      if (fGeanePro) // Overwrites vertex if Geane is used
	{
	  CbmTrackParH *fStart= new CbmTrackParH(track->GetLastHit().GetCoord(), track->GetMomentum(), track->GetLastHit().GetError(), (TVector3)(track->GetMomentum()*0.), track->GetCharge());
	  CbmTrackParH *fRes= new CbmTrackParH();
	  Bool_t rc =  fPro->Propagate(fStart, fRes, 13);	
	  if (rc)
	    {
	      vertex.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
	      tofGLength = fPro->GetLengthAtPCA();
	    }
	  else
	    {
	      vertex.SetXYZ(-10000., -10000., -10000.);
	    }
	}
      
      Float_t dzs = (vertex.Z()-tofHit->GetZ()-fCorrPar->GetTofZ0()) / fCorrPar->GetTofSigmaZ();
      Float_t dphi = (vertex.DeltaPhi(tofPos)-fCorrPar->GetTofPhi0()) / fCorrPar->GetTofSigmaPhi();
 
      Float_t chi2 = dphi * dphi + dzs * dzs;
      //Float_t chi2 = dzs * dzs;
      
      if ( tofQuality > chi2)
	{
	  tofIndex = tt;
	  tofQuality = chi2;
	  tofTof = tofHit->GetTime();
	  tofDz = vertex.Z()- tofHit->GetZ() - fCorrPar->GetTofZ0();
	  tofDphi = vertex.DeltaPhi(tofPos)-fCorrPar->GetTofPhi0();
	  tofLength = fabs(phi * track->GetRadius() / TMath::Sin(track->GetMomentum().Theta()));
	}
      if (fDebugMode)
	{
	  Float_t ntuple[] = {vertex.X(), vertex.Y(), vertex.Z(), vertex.Phi(),
			      track->GetMomentum().Mag(), track->GetCharge(), track->GetMomentum().Theta(), track->GetZ0(),
			      tofPos.X(), tofPos.Y(), tofPos.Z(), tofPos.Phi(),
			      chi2, vertex.DeltaPhi(tofPos), tofLength, tofGLength};
	  tofCorr->Fill(ntuple);
	}
    }
  
  if (tofQuality<fCorrPar->GetTofCut())
    {
      track->SetTofIndex(tofIndex);
      track->SetTofDeltaPhi(tofDphi);
      track->SetTofDeltaZ(tofDz);
      track->SetTofQuality(tofQuality);
      track->SetTof(tofTof);
      track->SetTofPathLength(tofLength);
    }
  
}

//_________________________________________________________________
void PndLhePidMaker::GetEmcInfo(PndLhePidTrack* track) {  
  if ((track->GetMomentum().Theta()*TMath::RadToDeg())<20.) return;
  if ((track->GetMomentum().Theta()*TMath::RadToDeg())>145.) return;
  if (fGeanePro) fPro->PropagateToVolume("Emc12",0,1);
  
  //---
  PndEmcCluster *emcHit = NULL;
  Int_t emcEntries = fEmcCluster->GetEntriesFast();
  Int_t emcIndex = -1;
  Float_t emcEloss = 0., emcDz = -1000, emcDphi = -1000, emcGLength = -1000;
  Float_t emcQuality = 1000000;
  
  Float_t chi2 = 0;
  TVector3 vertex(0., 0., 0.);
  TVector3 emcPos(0., 0., 0.);
  TVector3 momentum(0., 0., 0.);
  for (Int_t ee = 0; ee<emcEntries; ee++)
    {
      emcHit = (PndEmcCluster*)fEmcCluster->At(ee);
      
      if (((PndEmcDigi*)emcHit->Maxima())->GetModule()>2) continue;
      if (emcHit->energy() < fCorrPar->GetEmc12Thr()) continue;
      
      emcPos = emcHit->where();
      Float_t phi = track->ExtrapolateToR(&momentum, &vertex, fCorrPar->GetEmc12Radius());
      
      if (fGeanePro) // Overwrites vertex if Geane is used
	{
	  CbmTrackParH *fStart= new CbmTrackParH(track->GetLastHit().GetCoord(), track->GetMomentum(), track->GetLastHit().GetError(), (TVector3)(track->GetMomentum()*0.), track->GetCharge());
	  CbmTrackParH *fRes= new CbmTrackParH();
	  Bool_t rc =  fPro->Propagate(fStart, fRes, 13);
	  if (rc)
	    {
	      vertex.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
	      emcGLength = fPro->GetLengthAtPCA();
	    }
	  else
	    {
	      vertex.SetXYZ(-10000, -10000, -10000);
	    }

	  std::map<PndEmcTwoCoordIndex*, PndEmcXtal*> tciXtalMap=PndEmcStructure::Instance()->GetTciXtalMap();
	  PndEmcDigi *lDigi= (PndEmcDigi*)emcHit->Maxima();
	  PndEmcXtal* xtal = tciXtalMap[lDigi->GetTCI()];
	  emcPos = xtal->frontCentre();
	}
      
      Float_t dzs = (vertex.Z()-emcPos.Z()-fCorrPar->GetEmc12Z0()) / fCorrPar->GetEmc12SigmaZ();
      Float_t dphi = (vertex.DeltaPhi(emcPos)-fCorrPar->GetEmc12Phi0()) / fCorrPar->GetEmc12SigmaPhi();
      Float_t chi2 = dphi * dphi + dzs * dzs;
      //Float_t chi2 = dzs * dzs;
      if ( emcQuality > chi2 ) 
	{
	  emcIndex = ee;
	  emcQuality = chi2;
	  emcEloss = emcHit->energy();
	  emcDphi = vertex.DeltaPhi(emcPos)-fCorrPar->GetEmc12Phi0();
	  emcDz = vertex.Z() - emcPos.Z() - fCorrPar->GetEmc12Z0();
	}

      if (fDebugMode)
	{
	  Float_t ntuple[] = {vertex.X(), vertex.Y(), vertex.Z(), vertex.Phi(),
			      track->GetMomentum().Mag(), track->GetCharge(), track->GetMomentum().Theta(), track->GetZ0(),
			      emcPos.X(), emcPos.Y(), emcPos.Z(), emcPos.Phi(),
			      chi2, vertex.DeltaPhi(emcPos), emcHit->energy(), emcGLength};
	  emcCorr->Fill(ntuple);
	}
    }
  if (emcQuality < fCorrPar->GetEmc12Cut())
    {
      track->SetEmcIndex(emcIndex);
      track->SetEmcDeltaPhi(emcDphi);
      track->SetEmcDeltaZ(emcDz);
      track->SetEmcQuality(emcQuality);
      track->SetEmcELoss(emcEloss);
    }
}

//_________________________________________________________________
void PndLhePidMaker::GetMdtInfo(PndLhePidTrack* track) {
  //---
  if (fGeanePro) fPro->PropagateToVolume("MdtBarrel",0,1);
  
  PndMdtHit *mdtHit = NULL;
  Int_t mdtEntries = fMdtHit->GetEntriesFast();
  Int_t mdtIndex = -1, mdtMod = 0;
  Float_t mdtDz = -1000, mdtDphi = -1000, mdtGLength = -1000;
  Float_t mdtQuality = 1000000;
  
  Float_t chi2 = 0;
  TVector3 vertex(0., 0., 0.);
  TVector3 mdtPos(0., 0., 0.);
  TVector3 momentum(0., 0., 0.);
  for (Int_t mm = 0; mm<mdtEntries; mm++)
    {
      mdtHit = (PndMdtHit*)fMdtHit->At(mm);
      if (mdtHit->GetLayerID()!=0) continue;
      
      if (mdtHit->GetModule()!=1) continue;
      
      mdtHit->Position(mdtPos);
      Float_t phi = track->ExtrapolateToR(&momentum, &vertex, fCorrPar->GetMdtRadius());

      if (fGeanePro) // Overwrites vertex if Geane is used
	{
	  CbmTrackParH *fStart= new CbmTrackParH(track->GetLastHit().GetCoord(), track->GetMomentum(), track->GetLastHit().GetError(), (TVector3)(track->GetMomentum()*0.), track->GetCharge());
	  CbmTrackParH *fRes= new CbmTrackParH();
	  Bool_t rc =  fPro->Propagate(fStart, fRes, 13); 
	  if (rc)
	    {
	      vertex.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
	      mdtGLength = fPro->GetLengthAtPCA();
	    }
	  else
	    {
	      vertex.SetXYZ(-10000., -10000.,-10000.);
	    }
	}
      
      Float_t dzs = (vertex.Z()-mdtHit->GetZ()-fCorrPar->GetMdtZ0()) / fCorrPar->GetMdtSigmaZ();
      Float_t dphi = (vertex.DeltaPhi(mdtPos)-fCorrPar->GetMdtPhi0()) / fCorrPar->GetMdtSigmaPhi();
 
      //Float_t chi2 = dphi * dphi + dzs * dzs;
      Float_t chi2 = dzs * dzs;
      
      if ( mdtQuality > chi2)
	{
	  mdtIndex = mm;
	  mdtQuality = chi2;
	  mdtDz = vertex.Z()- mdtHit->GetZ() - fCorrPar->GetMdtZ0();
	  mdtDphi = vertex.DeltaPhi(mdtPos)-fCorrPar->GetMdtPhi0();
	  mdtMod = mdtHit->GetModule();
	}
      if (fDebugMode)
	{
	  Float_t ntuple[] = {vertex.X(), vertex.Y(), vertex.Z(), vertex.Phi(), 
			      track->GetMomentum().Mag(), track->GetCharge(), track->GetMomentum().Theta(), track->GetZ0(),
			      mdtPos.X(), mdtPos.Y(), mdtPos.Z(), mdtPos.Phi(),
			      chi2, mdtHit->GetModule(), vertex.DeltaPhi(mdtPos), mdtGLength};
	  mdtCorr->Fill(ntuple);
	}
    }
  
  if (mdtQuality<fCorrPar->GetMdtCut())
    {
      track->SetMdtIndex(mdtIndex);
      track->SetMdtDeltaPhi(mdtDphi);
      track->SetMdtDeltaZ(mdtDz);
      track->SetMdtQuality(mdtQuality);
      track->SetMdtModule(mdtMod);
      track->SetMdtLayerCount(1);
    }
  
}

//_________________________________________________________________
void PndLhePidMaker::GetDrcInfo(PndLhePidTrack* track) {
  if ((track->GetMomentum().Theta()*TMath::RadToDeg())<20.) return;
  if (fGeanePro) fPro->PropagateToVolume("DrcBase",0,1);
  //---
  PndDrcHit *drcHit = NULL;
  Int_t drcEntries = fDrcHit->GetEntriesFast();
  Int_t drcIndex = -1, drcPhot = 0;
  Float_t drcThetaC = -1000, drcThetaCErr = 0, drcDphi = -1000, drcGLength = -1000;
  Float_t drcQuality = 1000000;
  
  TVector3 vertex(0., 0., 0.);
  TVector3 drcPos(0., 0., 0.);
  TVector3 momentum(0., 0., 0.);
  for (Int_t dd = 0; dd<drcEntries; dd++)
    {
      drcHit = (PndDrcHit*)fDrcHit->At(dd);
           
      drcHit->Position(drcPos);
      Float_t phi = track->ExtrapolateToR(&momentum, &vertex, fCorrPar->GetDrcRadius());

      if (fGeanePro) // Overwrites vertex if Geane is used
	{
	  CbmTrackParH *fStart= new CbmTrackParH(track->GetLastHit().GetCoord(), track->GetMomentum(), track->GetLastHit().GetError(), (TVector3)(track->GetMomentum()*0.), track->GetCharge());
	  CbmTrackParH *fRes= new CbmTrackParH();
	  Bool_t rc =  fPro->Propagate(fStart, fRes, 13); 	
	  if (rc)
	    {
	      vertex.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
	      drcGLength = fPro->GetLengthAtPCA();
	    }
	  else
	    {
	      vertex.SetXYZ(-10000., -10000., -10000.);
	    }
	}
      
      Float_t dphi = (vertex.DeltaPhi(drcPos)-fCorrPar->GetDrcPhi0()) / fCorrPar->GetDrcSigmaPhi();
      Float_t chi2 = dphi * dphi;
      
      if ( drcQuality > chi2)
	{
	  drcIndex = dd;
	  drcQuality = chi2;
	  drcDphi = vertex.DeltaPhi(drcPos)-fCorrPar->GetDrcPhi0();
	  drcThetaC = drcHit->GetThetaC();
	  drcThetaCErr = drcHit->GetErrThetaC();
	  drcPhot = 0; // ** to be filled **
	}
      if (fDebugMode)
	{
	  Float_t ntuple[] = {vertex.X(), vertex.Y(), vertex.Z(), vertex.Phi(),  
			      track->GetMomentum().Mag(), track->GetCharge(), track->GetMomentum().Theta(), track->GetZ0(),
			      drcPos.X(), drcPos.Y(), drcPos.Phi(), chi2, drcHit->GetThetaC(), 0., vertex.DeltaPhi(drcPos), drcGLength};
	  drcCorr->Fill(ntuple);
	}
    }
  
  if (drcQuality<fCorrPar->GetDrcCut())
    {
      track->SetDrcIndex(drcIndex);
      track->SetDrcDeltaPhi(drcDphi);
      track->SetDrcQuality(drcQuality);
      track->SetDrcThetaC(drcThetaC);
      track->SetDrcThetaCErr(drcThetaCErr);
      track->SetDrcNPhotons(drcPhot);
    }
  
}

//_________________________________________________________________
void PndLhePidMaker::Register() {
  //---
  CbmRootManager::Instance()->
    Register("LhePidTrack","Lhe", fPidTrackCand, kTRUE);
}

//_________________________________________________________________
void PndLhePidMaker::Finish() {
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
void PndLhePidMaker::Reset() {
  //---
  fMvdPath = 0.;
  fMvdELoss = 0.;
  fMvdHitCount = 0;
  fSttPath = 1.;
  fSttELoss = 0.;
  fSttHitCount = 0;
}

//_________________________________________________________________
PndLhePidTrack* PndLhePidMaker::AddTrack(PndLhePidTrack* track) {
  // Creates a new hit in the TClonesArray.
  
  TClonesArray& trkRef = *fPidTrackCand;
  Int_t size = trkRef.GetEntriesFast();
  return new(trkRef[size]) PndLhePidTrack(*track);
}


ClassImp(PndLhePidMaker)
