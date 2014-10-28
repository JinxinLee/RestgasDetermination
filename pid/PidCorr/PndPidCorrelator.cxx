#include "PndDetectorList.h"
#include "PndPidCorrelator.h"
#include "PndPidCandidate.h"
#include "PndMCTrack.h"
#include "PndTrack.h"
#include "PndTrackID.h"

#include "PndSciTPoint.h"
#include "PndSciTHit.h"
#include "PndEmcBump.h"
#include "PndEmcDigi.h"
#include "PndEmcStructure.h"
#include "PndEmcXtal.h"
#include "PndEmcErrorMatrix.h"
#include "PndEmcClusterCalibrator.h"
#include "PndEmcClusterEnergySums.h"
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
#include "TSystem.h"

#include <cmath>

// ----------------------------------------------------------
// --- Interface with PidMaker and output ---

//___________________________________________________________
PndPidCorrelator::~PndPidCorrelator() 
{
  //
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
  delete fEmcErrorMatrix; 
}

//___________________________________________________________
PndPidCorrelator::PndPidCorrelator() : 
  FairTask(), fMcTrack(0), fTrack(0), fTrackID(0), fTrack2(0), fTrackID2(0), fPidChargedCand(0), fPidNeutralCand(0), fMdtTrack(0), fMvdHitsStrip(0), fMvdHitsPixel(0), fTofHit(0), fTofPoint(0), fFtofHit(0), fFtofPoint(0), fEmcCluster(0), fEmcBump(0), fEmcDigi(0), fMdtPoint(0), fMdtHit(0), fMdtTrk(0), fDrcPoint(0), fDrcHit(0), fDskParticle(0), fSttHit(0), fFtsHit(0),
  fCorrPar(new PndPidCorrPar()), fEmcGeoPar(new PndEmcGeoPar()), fEmcErrorMatrixPar(new PndEmcErrorMatrixPar()), fEmcErrorMatrix(new PndEmcErrorMatrix()), fSttParameters(new PndGeoSttPar()), fEmcCalibrator(NULL), fEmcClstCount(0), fFscClstCount(0),
  fDebugMode(kFALSE),
  fGeanePro(kTRUE), 
  fMdtRefit(kFALSE),
  fMvdMode(-1),
  fSttMode(-1),
  fFtsMode(-1),
  fTofMode(-1), 
  fFtofMode(-1),
  fEmcMode(-1),
  fMdtMode(-1), 
  fDrcMode(-1),
  fDskMode(-1),
  fMixMode(kFALSE),
  fPidHyp(0),
  fIdealHyp(kFALSE), 
  fFast(kFALSE),
  fSimulation(kFALSE),
  fIdeal(kFALSE), 
  fCorrErrorProp(kTRUE),
  fFlagCut(kTRUE),
  fBackPropagate(kTRUE),
  tofCorr(0),
  emcCorr(0), 
  fscCorr(0),
  drcCorr(0),
  dskCorr(0),
  fTrackBranch(""),
  fTrackIDBranch(""),
  fTrackBranch2(""),
  fTrackIDBranch2(""),
  fTrackOutBranch(""),
  sDir(""),
  sFile(""),
  fGeoH(NULL),
  fClusterList(), 
  fClusterQ(),
  mapMdtBarrel(),
  mapMdtEndcap(),
  mapMdtForward()
{
  //---
  fPidChargedCand = new TClonesArray("PndPidCandidate");
  fPidNeutralCand = new TClonesArray("PndPidCandidate");
  sDir = "./";
  sFile = "./pidcorrelator.root";
  fGeoH = PndGeoHandling::Instance();
  
  // Resetting MDT geometry parameters
  for (Int_t mm=0; mm<3; mm++)
    for (Int_t ll=0; ll<20;ll++)
      {
	mdtLayerPos[mm][ll] = -1;
	mdtIronThickness[mm][ll] = -1;
      }
  
  Reset();
}

//___________________________________________________________
PndPidCorrelator::PndPidCorrelator(const char *name, const char *title) :
  FairTask(name),
  fMcTrack(0), fTrack(0), fTrackID(0), fTrack2(0), fTrackID2(0), fPidChargedCand(0), fPidNeutralCand(0), fMdtTrack(0), fMvdHitsStrip(0), fMvdHitsPixel(0), fTofHit(0), fTofPoint(0), fFtofHit(0), fFtofPoint(0), fEmcCluster(0), fEmcBump(0), fEmcDigi(0), fMdtPoint(0), fMdtHit(0), fMdtTrk(0), fDrcPoint(0), fDrcHit(0), fDskParticle(0), fSttHit(0), fFtsHit(0), 
  fCorrPar(new PndPidCorrPar()), fEmcGeoPar(new PndEmcGeoPar()), fEmcErrorMatrixPar(new PndEmcErrorMatrixPar()), fEmcErrorMatrix(new PndEmcErrorMatrix()), fSttParameters(new PndGeoSttPar()), fEmcCalibrator(NULL), fEmcClstCount(0), fFscClstCount(0),
  fDebugMode(kFALSE),
  fGeanePro(kTRUE), 
  fMdtRefit(kFALSE),
  fMvdMode(-1),
  fSttMode(-1), 
  fFtsMode(-1),
  fTofMode(-1), 
  fFtofMode(-1),
  fEmcMode(-1),
  fMdtMode(-1), 
  fDrcMode(-1),
  fDskMode(-1),
  fMixMode(kFALSE),
  fPidHyp(0),
  fIdealHyp(kFALSE), 
  fFast(kFALSE),
  fSimulation(kFALSE),
  fIdeal(kFALSE), 
  fCorrErrorProp(kTRUE), 
  fFlagCut(kTRUE),
  fBackPropagate(kTRUE),
  tofCorr(0),
  emcCorr(0), 
  fscCorr(0),
  drcCorr(0),
  dskCorr(0),
  fTrackBranch(""),
  fTrackIDBranch(""),
  fTrackBranch2(""),
  fTrackIDBranch2(""),
  fTrackOutBranch(""),
  sDir(""),
  sFile(""),
  fGeoH(NULL),
  fClusterList(), 
  fClusterQ(),
  mapMdtBarrel(),
  mapMdtEndcap(),
  mapMdtForward()
{
  //---
  fPidChargedCand = new TClonesArray("PndPidCandidate");
  fPidNeutralCand = new TClonesArray("PndPidCandidate");
  sDir = "./";
  sFile = "./pidcorrelator.root";
  fGeoH = PndGeoHandling::Instance(); 
  
  // Resetting MDT geometry parameters
  for (Int_t mm=0; mm<3; mm++)
    for (Int_t ll=0; ll<20;ll++)
      {
	mdtLayerPos[mm][ll] = -1;
	mdtIronThickness[mm][ll] = -1;
      }
  
  Reset();
}

//___________________________________________________________
InitStatus PndPidCorrelator::Init() {
  
  //  cout << "InitStatus PndPidCorrelator::Init()" << endl;
  
  FairRootManager *fManager =FairRootManager::Instance();	
  
  fTrack = dynamic_cast<TClonesArray *> (fManager->GetObject(fTrackBranch));
  if ( ! fTrack ) {
    cout << "-I- PndPidCorrelator::Init: No PndTrack array!" << endl;
    return kERROR;
  }
  
  if (fTrackIDBranch!="")
    {
      fTrackID = dynamic_cast<TClonesArray *> (fManager->GetObject(fTrackIDBranch));
      if ( ! fTrackID ) {
	cout << "-I- PndPidCorrelator::Init: No PndTrackID array! Switching MC propagation OFF" << endl;
	fTrackIDBranch = "";
      }
    }
  
  if (fTrackBranch2!="")
    {
      fTrack2 = dynamic_cast<TClonesArray *> (fManager->GetObject(fTrackBranch2));
      if ( ! fTrack2 ) {
	cout << "-I- PndPidCorrelator::Init: No 2nd PndTrack array!" << endl;
	return kERROR;
      }
    }
  
  if (fTrackIDBranch2!="")
    {
      fTrackID2 = dynamic_cast<TClonesArray *> (fManager->GetObject(fTrackIDBranch2));
      if ( ! fTrackID2 ) {
	cout << "-I- PndPidCorrelator::Init: No 2nd PndTrackID array! Switching MC propagation OFF" << endl;
	fTrackIDBranch2 = "";
      }
    }
  
  // *** STT ***
  if (fSttMode)
    {
      if (fMixMode==kFALSE)
	{
	  fSttHit = dynamic_cast<TClonesArray *> ( fManager->GetObject("STTHit"));
	  if ( fSttHit ) 
	    {
	      cout << "-I- PndPidCorrelator::Init: Using STTHit" << endl;
	      fSttMode = 2;
	    }
	  else
	    {
	      cout << "-W- PndPidCorrelator::Init: No STT hits array! Switching STT OFF" << endl;
	      fSttMode = 0;
	    } 
	}
      else
	{
	  fSttHit = dynamic_cast<TClonesArray *> (fManager->GetObject("STTHitMix"));
	  if ( fSttHit )
	    {
	      cout << "-I- PndPidCorrelator::Init: Using STTHitMix" << endl;
	      fSttMode = 2;
	    }
	  else
	    {
	      cout << "-W- PndPidCorrelator::Init: No STT hits mix array! Switching STT OFF" << endl;
	      fSttMode = 0;
	    }
	}
    }
  
  // *** FTS ***
  if (fFtsMode)
    {
      if (fMixMode==kFALSE)
	{
	  fFtsHit =dynamic_cast<TClonesArray *>( fManager->GetObject("FTSHit"));
	  if ( fFtsHit ) 
	    {
	      cout << "-I- PndPidCorrelator::Init: Using FTSHit" << endl;
	      fFtsMode = 2;
	    }
	  else
	    {
	      cout << "-W- PndPidCorrelator::Init: No FTS hits array! Switching FTS OFF" << endl;
	      fFtsMode = 0;
	    } 
	}
      else
	{
	  fFtsHit =dynamic_cast<TClonesArray *> (fManager->GetObject("FTSHitMix"));
	  if ( fFtsHit )
	    {
	      cout << "-I- PndPidCorrelator::Init: Using FTSHitMix" << endl;
	      fFtsMode = 2;
	    }
	  else
	    {
	      cout << "-W- PndPidCorrelator::Init: No FTS hits mix array! Switching FTS OFF" << endl;
	      fFtsMode = 0;
	    }
	}
    }

  // *** MVD ***
  if (fMvdMode)
    {
      if (fMixMode==kFALSE)
	{
	  fMvdHitsStrip = dynamic_cast<TClonesArray *> ( fManager->GetObject("MVDHitsStrip"));
	  if ( ! fMvdHitsStrip ) 
	    {
	      cout << "-W- PndPidCorrelator::Init: No MVDHitsStrip array!" << endl;
	    }
	  else fMvdMode = 2;
	  
	  fMvdHitsPixel = dynamic_cast<TClonesArray *> ( fManager->GetObject("MVDHitsPixel"));
	  if ( ! fMvdHitsPixel ) 
	    {
	      cout << "-W- PndPidCorrelator::Init: No MVDHitsPixel array!" << endl;
	    }
	  else fMvdMode = 2;
	}
      else
	{
	  fMvdHitsStrip = dynamic_cast<TClonesArray *> ( fManager->GetObject("MVDHitsStripMix"));
	  if ( ! fMvdHitsStrip )
	    {
	      cout << "-W- PndPidCorrelator::Init: No MVDHitsStripMix array!" << endl;
	    }
	  else fMvdMode = 2;
	  
	  fMvdHitsPixel = dynamic_cast<TClonesArray *> (fManager->GetObject("MVDHitsPixelMix"));
	  if ( ! fMvdHitsPixel )
	    {
	      cout << "-W- PndPidCorrelator::Init: No MVDHitsPixelMix array!" << endl;
	    }
	  else fMvdMode = 2;
	}
      
      if (( ! fMvdHitsStrip ) &&  ( ! fMvdHitsPixel ))
	{
	  cout << "-W- PndPidCorrelator::Init: No MVD hits array! Switching MVD OFF" << endl;
	  fMvdMode = 0;
	}
      else
	{
	  cout << "-I- PndPidCorrelator::Init: Using MVDHit" << endl;
	}
    }
  
  // *** TOF ***
  if (fTofMode)
    {
      fTofHit = dynamic_cast<TClonesArray *> (fManager->GetObject("SciTHit"));
      if ( ! fTofHit ) 
	{
	  cout << "-W- PndPidCorrelator::Init: No SciTHit array!" << endl;
	  fTofMode = 0;
	}
      else  
	{
	  cout << "-I- PndPidCorrelator::Init: Using SciTHit" << endl;
	  fTofMode = 2;
	}
    if (fIdeal)
      {
        fTofPoint = dynamic_cast<TClonesArray *> (fManager->GetObject("SciTPoint"));
        if ( ! fTofPoint )
          {
            cout << "-W- PndPidCorrelator::Init: No SciTPoint array!" << endl;
            fTofMode = 0;
          }
      }
    }
  
  // *** FTOF ***
  if (fFtofMode)
    {
      fFtofHit = dynamic_cast<TClonesArray *> ( fManager->GetObject("FtofHit"));
      if ( ! fFtofHit ) 
	{
	  cout << "-W- PndPidCorrelator::Init: No FtofHit array!" << endl;
	  fFtofMode = 0;
	}
      else  
	{
	  cout << "-I- PndPidCorrelator::Init: Using FtofHit" << endl;
	  fFtofMode = 2;
	}
    if (fIdeal)
      {
        fFtofPoint = dynamic_cast<TClonesArray *>  (fManager->GetObject("FtofPoint"));
        if ( ! fFtofPoint )
          {
            cout << "-W- PndPidCorrelator::Init: No FtofPoint array!" << endl;
            fFtofMode = 0;
          }
      }
    }
  
  // *** EMC ***
  if (fEmcMode)
    {
      fEmcCluster = dynamic_cast<TClonesArray *> ( fManager->GetObject("EmcCluster"));
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
      
      fEmcBump = dynamic_cast<TClonesArray *> (fManager->GetObject("EmcBump"));
      if ( ! fEmcBump ) 
	{
	  cout << "-W- PndPidCorrelator::Init: No EmcBump array!" << endl;
	}
      else 
	{
	  cout << "-I- PndPidCorrelator::Init: Using EmcBump" << endl;
	  fEmcMode = 3;
	}	  

      fEmcDigi = dynamic_cast<TClonesArray *> ( fManager->GetObject("EmcDigi"));
      if ( ! fEmcDigi)
        {
          cout << "-W- PndPidCorrelator::Init: No EmcDigi array! No EMC E1/E9/E25 information is propagated!" << endl;
        }
    }

  // *** DRC ***
  if (fDrcMode)
    {
      fDrcHit = dynamic_cast<TClonesArray *> (fManager->GetObject("DrcHit"));
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
    }
  
  // *** DSK ***
  if (fDskMode)
    {
      fDskParticle = dynamic_cast<TClonesArray *> ( fManager->GetObject("DskParticle"));
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
    }
  
  // *** MDT ***
  if (fMdtMode)
    {
      fMdtHit = dynamic_cast<TClonesArray *> (fManager->GetObject("MdtHit"));
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
      fMdtTrk = dynamic_cast<TClonesArray *> (fManager->GetObject("MdtTrk"));
      if ( ! fMdtTrk ) 
	{
	  cout << "-W- PndPidCorrelator::Init: No MdtTrk array!" << endl;
	}
      else  
	{
	  cout << "-I- PndPidCorrelator::Init: Using MdtTrk" << endl;
	  fMdtMode = 3;
	}
    }
  
  if (fIdeal)
    {
      cout << "-I- PndPidCorrelator::Init: Using MonteCarlo correlation" << endl;
      fTofPoint =dynamic_cast<TClonesArray *> ( fManager->GetObject("TofPoint"));
      if ( ! fTofPoint ) 
	{
	  cout << "-W- PndPidCorrelator::Init: No TofPoint array!" << endl;
	  fTofMode = 0;
	}
      else  
	{
	  cout << "-I- PndPidCorrelator::Init: Using TofPoint" << endl;
	}
      fDrcPoint = dynamic_cast<TClonesArray *> (fManager->GetObject("DrcBarPoint"));
      if ( ! fDrcPoint ) 
	{
	  cout << "-W- PndPidCorrelator::Init: No DrcBarPoint array!" << endl;
	  fDrcMode = 0;
	}
      else  
	{
	  cout << "-I- PndPidCorrelator::Init: Using DrcPoint" << endl;
	}
      fMdtPoint = dynamic_cast<TClonesArray *> ( fManager->GetObject("MdtPoint"));
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
    { 
      cout << "-I- PndPidCorrelator::Init: Using Geane for Track propagation" << endl;
      if (!fCorrErrorProp)
	{
	  cout << "-I- PndPidCorrelator::Init: Switching OFF Geane error propagation" << endl;
	}
      if (fIdealHyp)
	{
	  fMcTrack = dynamic_cast<TClonesArray *> (fManager->GetObject("MCTrack"));
	  if ( ! fMcTrack ) {
	    cout << "-I- PndPidCorrelator::Init: No PndMcTrack array! No ideal pid hypothesis is possible!" << endl;
	    return kERROR;
	  }
	  if (fTrackIDBranch=="") {
	    cout << "-I- PndPidCorrelator::Init: No TrackID Branch name! No ideal pid hypothesis is possible!" << endl;
	    return kERROR;
	  }
	}
      else
	{
	  switch (abs(fPidHyp))
	    {
	    case 0:
	      cout << "-I- PndPidCorrelator::Init: No PID set -> Using default PION hypothesis" << endl;
	      fPidHyp = 211;
	      break;
        
	    case 11:
	      cout << "-I- PndPidCorrelator::Init: Using ELECTRON hypothesis" << endl;
	      fPidHyp = -11;
	      break;
        
	    case 13:
	      cout << "-I- PndPidCorrelator::Init: Using MUON hypothesis" << endl;
	      fPidHyp = -13;
	      break;
        
	    case 211:
	      cout << "-I- PndPidCorrelator::Init: Using PION hypothesis" << endl;
	      fPidHyp = 211;
	      break;
        
	    case 321:
	      cout << "-I- PndPidCorrelator::Init: Using KAON hypothesis" << endl;
	      fPidHyp = 321;
	      break;
        
	    case 2212:
	      cout << "-I- PndPidCorrelator::Init: Using PROTON hypothesis" << endl;
	      fPidHyp = 2212;
	      break;
        
	    default:
	      cout << "-I- PndPidCorrelator::Init: Not recognised PID set -> Using default PION hypothesis" << endl;
	      fPidHyp = 211;
	      break;
	    }
	}
    }
  else
    {
      return kFATAL;
    }
  
  if (fMdtMode>0)
    {
      if (!MdtGeometry())
	{
	  cout << "-W- PndPidCorrelator::Init: No MDT geometry ???" << endl;
	  fMdtMode = 0;
	}
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
      ftofCorr = new TNtuple("ftofCorr","TRACK-FTOF Correlation",
                            "track_x:track_y:track_z:ver_x:ver_y:ver_z:ver_px:ver_py:ver_pz:track_p:track_charge:track_theta:track_z0:tof_x:tof_y:tof_z:chi2:len:glen:tlen");
      emcCorr = new TNtuple("emcCorr","TRACK-EMC Correlation",
			    "track_x:track_y:track_z:track_phi:track_p:track_charge:track_theta:track_z0:emc_x:emc_y:emc_z:emc_phi:chi2:dphi:emc_ene:glen:emc_mod");
      fscCorr = new TNtuple("fscCorr","TRACK-FSC Correlation",
			    "track_x:track_y:track_z:track_phi:track_p:track_charge:track_theta:track_z0:emc_x:emc_y:emc_z:emc_phi:chi2:dphi:emc_ene:glen:emc_mod");
      mdtCorr = new TNtuple("mdtCorr","TRACK-MDT Correlation",
			    "track_x:track_y:track_z:track_dx:track_dy:track_dz:track_phi:track_p:track_charge:track_theta:track_z0:mdt_x:mdt_y:mdt_z:mdt_phi:mdt_p:chi2:mdt_mod:dphi:glen:mdt_count:nhits");
      drcCorr = new TNtuple("drcCorr","TRACK-DRC Correlation",
			    "track_x:track_y:track_z:track_phi:track_p:track_charge:track_theta:track_z0:drc_x:drc_y:drc_phi:chi2:drc_thetac:drc_nphot:dphi:glen:flag");
      dskCorr = new TNtuple("dskCorr","TRACK-DSK Correlation",
			    "track_x:track_y:track_z:track_phi:track_p:track_charge:track_theta:track_z0:dsk_x:dsk_y:dsk_z:dsk_phi:chi2:dsk_thetac:dsk_nphot:dphi:glen:track_lx:track_ly:track_lz:track_xp:flag");
      cout << "-I- PndPidCorrelator::Init: Filling Debug histograms" << endl;
    
    }
	
  // Set Parameters for Emc error matrix 
  if (fEmcMode>0) 
  {
    if (fEmcErrorMatrixPar->IsValid())
      {
        fEmcErrorMatrix->Init(fEmcErrorMatrixPar->GetParObject());
        //std::cout<<"PndPidCorrelator: Emc error matrix is read from RTDB"<<std::endl;
      } else
      {
        Int_t emcGeomVersion=fEmcGeoPar->GetGeometryVersion();
        fEmcErrorMatrix->InitFromFile(emcGeomVersion);
        fEmcErrorMatrixPar->SetErrorMatrixObject(fEmcErrorMatrix->GetParObject());
        //std::cout<<"PndPidCorrelator: Emc error matrix is read from file"<<std::endl;
      }
    fEmcCalibrator= PndEmcClusterCalibrator::MakeEmcClusterCalibrator(1, 3);	
  }

  if (fFast)  cout << "-W- PndPidCorrelator::Init: Using fast correlator!!" << endl;
    
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
  
  // Get PID Correlation parameter container
  fCorrPar = (PndPidCorrPar*) db->getContainer("PndPidCorrPar");
  
  // Get Emc geometry parameter container
  fEmcGeoPar = (PndEmcGeoPar*) db->getContainer("PndEmcGeoPar");
  
  // Get Emc error matrix parameter container
  fEmcErrorMatrixPar = (PndEmcErrorMatrixPar*) db->getContainer("PndEmcErrorMatrixPar");

  // Get Stt parameter
  fSttParameters = (PndGeoSttPar*) db->getContainer("PndGeoSttPar");
  fFtsParameters = (PndGeoFtsPar*) db->getContainer("PndGeoFtsPar");
}
//______________________________________________________
void PndPidCorrelator::Exec(Option_t * option) {
  //-
  Reset();
  cout << " =====   PndPidCorrelator - Event: " << fEventCounter;
  Int_t nTracksTot=0;
  if (fTrack)
    {
      nTracksTot += fTrack->GetEntriesFast();
    }
  if (fTrack2)
    {
      nTracksTot += fTrack2->GetEntriesFast();
    }

  cout << " - Number of tracks for pid " << nTracksTot;
  if (fEmcMode>0)
    {
      PndEmcCluster *tmp_cluster;
      for(Int_t i=0; i < fEmcCluster->GetEntriesFast();i++)
	{
	  tmp_cluster = (PndEmcCluster*)fEmcCluster->At(i);
	  if(tmp_cluster->GetModule() < 5 && tmp_cluster->GetModule() >0)
	    {
	      fEmcClstCount++;
	    }
	  if(tmp_cluster->GetModule() == 5)
	    {
	      fFscClstCount++;
	    }
	}
      ResetEmcQ();
      cout << " - Number of Clusters for pid: ";
      cout << " EMC: " << fEmcClstCount;
      cout << " FSC: " << fFscClstCount;
    }
  cout<<endl;

  if (fTrack)     ConstructChargedCandidate();
  if ((fEmcMode>0)  && (!fFast)) ConstructNeutralCandidate();
  fEventCounter++;
}

//______________________________________________________
void PndPidCorrelator::ConstructChargedCandidate() {
  //-
  //FIXME: Use Clear() to save time. 
  //Call Delete() only for too busy events to save Memory
  fPidChargedCand->Delete();
  if (fMdtRefit) fMdtTrack->Delete();
  if (fMdtMode>0) MdtMapping();
  
  Int_t nTracks = fTrack->GetEntriesFast();
  for (Int_t i = 0; i < nTracks; i++) {
    PndTrack* track = (PndTrack*) fTrack->At(i);
    Int_t ierr = 0;
    FairTrackParP par = track->GetParamLast();
    if ((par.GetMomentum().Mag()<0.05) || (par.GetMomentum().Mag()>15.) )continue; // cut low and high momenta
    if ((fFlagCut) && (track->GetFlag()<=0)) continue; // cut flag<=0
    FairTrackParH *helix = new FairTrackParH(&par, ierr);
    
    PndPidCandidate* pidCand = 	new PndPidCandidate();
    if (fTrackIDBranch!="")
      {
	PndTrackID* trackID = (PndTrackID*) fTrackID->At(i);
	if (trackID->GetNCorrTrackId()>0)
	  {
	    pidCand->SetMcIndex(trackID->GetCorrTrackID());
	    if (fIdealHyp)
	      {
		PndMCTrack *mcTrack = (PndMCTrack*)fMcTrack->At(trackID->GetCorrTrackID());
		if ( ! mcTrack ) 
		  {
		    fPidHyp = 211;
                    cout << "-I- PndPidCorrelator::ConstructChargedCandidate: PndMCTrack does not exist!! (why?) -> let's try with pion hyp " << endl;
		  }
                else
		  {
		    fPidHyp = abs(mcTrack->GetPdgCode());
		  }
                if (fPidHyp>=100000000)
                  {
                    fPidHyp = 211;
                    std::cout << "-I- PndPidCorrelator::ConstructChargedCandidate: Track is an ion (PDGCode>100000000) -> let's try with pion hyp" << std::endl;
                  }

		if ( abs(fPidHyp)==13 ) fPidHyp = -13;
		if ( abs(fPidHyp)==11 ) fPidHyp = -11;
	      }
	  }
      } else { // added for PndAnalysis, TODO: remove after Fairlinks work with Associators
      PndTrackCand trackCand = track->GetTrackCand();
      pidCand->SetMcIndex(trackCand.getMcTrackId());
    }
    pidCand->SetTrackIndex(i);
    pidCand->SetTrackBranch(FairRootManager::Instance()->GetBranchId(fTrackBranch));
    pidCand->AddLink(FairLink(fTrackBranch, i));
    if (!GetTrackInfo(track, pidCand)) continue;
    if ( (fMvdMode==2) && ((fMvdHitsStrip->GetEntriesFast()+fMvdHitsPixel->GetEntriesFast())>0) ) GetMvdInfo(track, pidCand); 
    if ( (fSttMode == 2) && (fSttHit    ->GetEntriesFast()>0) ) GetSttInfo(track, pidCand);
    GetGemInfo(track, pidCand);
    if (!fFast)
      {
	if ( (fTofMode==2) && (fTofHit    ->GetEntriesFast()>0) ) GetTofInfo(helix, pidCand);
	if ( (fEmcMode>0)  && (fEmcClstCount>0) ) GetEmcInfo(helix, pidCand);
	if ( (fMdtMode>0)  && (fMdtHit    ->GetEntriesFast()>0) ) GetMdtInfo(track, pidCand);  
	if ( (fDrcMode>0)  && (fDrcHit    ->GetEntriesFast()>0) ) GetDrcInfo(helix, pidCand);
	if ( (fDskMode>0)  && (fDskParticle->GetEntriesFast()>0)) GetDskInfo(helix, pidCand); 
      }
    AddChargedCandidate(pidCand);
  } 
  
  if (fTrackBranch2!="")
    {
      Int_t nTracks2 = fTrack2->GetEntriesFast();
      for (Int_t i = 0; i < nTracks2; i++) {
	PndTrack* track = (PndTrack*) fTrack2->At(i);
	Int_t ierr = 0;
	FairTrackParP par = track->GetParamLast();
	if ((par.GetMomentum().Mag()<0.1) || (par.GetMomentum().Mag()>20.) )continue; // cut low and high momenta
	if ((fFlagCut) && (track->GetFlag()<=0)) continue; // cut flag<=0
	FairTrackParH *helix = new FairTrackParH(&par, ierr);
      
	PndPidCandidate* pidCand =  new PndPidCandidate();
	if (fTrackIDBranch2!="")
	  {
	    PndTrackID* trackID = (PndTrackID*) fTrackID2->At(i);
	    if (trackID->GetNCorrTrackId()>0)
	      {
		pidCand->SetMcIndex(trackID->GetCorrTrackID());
		if (fIdealHyp)
		  {
		    PndMCTrack *mcTrack = (PndMCTrack*)fMcTrack->At(trackID->GetCorrTrackID());
		    if ( ! mcTrack ) 
		      {
			fPidHyp = 211;
			cout << "-I- PndPidCorrelator::ConstructChargedCandidate: PndMCTrack does not exist!! (why?) -> let's try with pion hyp " << endl;
		      }
		    else
		      {
			fPidHyp = abs(mcTrack->GetPdgCode());
		      }
		    if (fPidHyp>=100000000)
		      {
			fPidHyp = 211;
			std::cout << "-I- PndPidCorrelator::ConstructChargedCandidate: Track is an ion (PDGCode>100000000) -> let's try with pion hyp" << std::endl;
		      }
		    
		    if ( abs(fPidHyp)==13 ) fPidHyp = -13;
		    if ( abs(fPidHyp)==11 ) fPidHyp = -11;
		  }
	      }
	  } else { // added for PndAnalysis, TODO: remove after Fairlinks work with Associators
	  PndTrackCand trackCand = track->GetTrackCand();
	  pidCand->SetMcIndex(trackCand.getMcTrackId());
	}
	pidCand->SetTrackIndex(i);
        pidCand->SetTrackBranch(FairRootManager::Instance()->GetBranchId(fTrackBranch2));
	pidCand->AddLink(FairLink("PndTrack", i));
	if (!GetTrackInfo(track, pidCand)) continue;
	if ( (fMvdMode==2) && ((fMvdHitsStrip->GetEntriesFast()+fMvdHitsPixel->GetEntriesFast())>0) ) GetMvdInfo(track, pidCand); 
	if ( (fFtsMode == 2) && (fFtsHit    ->GetEntriesFast()>0) ) GetFtsInfo(track, pidCand); 
	GetGemInfo(track, pidCand);
	if (!fFast)
	  {
	    if ( (fFtofMode==2) && (fFtofHit->GetEntriesFast()>0) ) GetFtofInfo(helix, pidCand);
	    if ( (fEmcMode>0)  && (fFscClstCount>0) ) GetFscInfo(helix, pidCand);
	    //if ( (fMdtMode>0)  && (fMdtHit    ->GetEntriesFast()>0) ) GetMdtInfo(track, pidCand);
	    if (mapMdtForward.size()>0)  GetFMdtInfo(&par, pidCand);
	  } // end of fast mode
	AddChargedCandidate(pidCand);
      }
    }
  
}

//______________________________________________________
void PndPidCorrelator::ConstructNeutralCandidate() {
  //-
  fPidNeutralCand->Delete();
  TString emcType;
  
  Int_t nBumps = 0;
  if (fEmcMode == 2)
    {
      nBumps = fEmcCluster->GetEntriesFast();
      emcType = "EmcCluster";
    }
  else if(fEmcMode == 3)
    {
      nBumps = fEmcBump->GetEntriesFast();
      emcType = "EmcBump";
    }
  
  for (Int_t i = 0; i < nBumps; i++)
    {
      PndEmcBump* bump;
      PndEmcCluster *clu;
      //Float_t quality = -1.;
      if (fEmcMode==2)
	{ 
	  //if (fClusterList[i]) continue;
	  bump = (PndEmcBump*) fEmcCluster->At(i);
	  clu  = (PndEmcBump*) fEmcCluster->At(i);
          //quality = fClusterQ[i];
	}
      else if(fEmcMode == 3)
	{
	  bump = (PndEmcBump*) fEmcBump->At(i);
	  //if (fClusterList[bump->GetClusterIndex()]) continue; // skip correlated clusters
	  clu = (PndEmcCluster*)fEmcCluster->At(bump->GetClusterIndex());
          //quality = fClusterQ[bump->GetClusterIndex()];
	}
    
      TVector3 vtx(0,0,0);
      TVector3 v1=bump->where();
      TVector3 p3;
      p3.SetMagThetaPhi(fEmcCalibrator->Energy(bump), v1.Theta(), v1.Phi());
      TLorentzVector lv(p3,p3.Mag());
      TMatrixD covP4=fEmcErrorMatrix->Get4MomentumErrorMatrix(*clu);
    
      FairGeanePro *fProEmc = new FairGeanePro(); 
      if (!fCorrErrorProp) fProEmc->PropagateOnlyParameters();
      Float_t emcQuality = 1000000;
      TVector3 vertex(0., 0., 0.); 
      
      Int_t nTracks = 0;
      if (bump->GetModule()>5) continue; // kick non valid emc modules
      if (bump->GetModule()<5) // barrel
	{
	  nTracks = fTrack->GetEntriesFast(); 
	}
      else // forward
	{
	   if (fTrackBranch2!="") nTracks = fTrack2->GetEntriesFast();
	}
      
      for (Int_t tt = 0; tt < nTracks; tt++)
	{
	  PndTrack* track;
	  if (bump->GetModule()<5) // barrel
	    {
	      track = (PndTrack*) fTrack->At(tt);
	    }
	  else // forward
	    {
	      track = (PndTrack*) fTrack2->At(tt);
	    }
	  
	  Int_t ierr = 0;
	  FairTrackParP par = track->GetParamLast(); 
	  FairTrackParH *helix = new FairTrackParH(&par, ierr);
	  
	  if (bump->GetModule()<5) // barrel
	    {
	      if ((bump->GetModule()<3) && (helix->GetZ()>150.)  ) continue; // not consider tracks after emc barrel for BARREL 
	      if ((bump->GetModule()==3) && (helix->GetZ()<165.) ) continue; // consider tracks only from last gem plane for FWD
	      if ((bump->GetModule()==4) && (helix->GetZ()>-30.) ) continue; // consider tracks only ending at the back of STT for BKW
	    }
	  else // forward
	    {
	      if (helix->GetZ() <  fCorrPar->GetZLastPlane()) continue;  // consider tracks only from last fts plane for FSC
	    }

	  if (fGeanePro)
	    { // Overwrites vertex if Geane is used
	      fProEmc->SetPoint(v1);
	      fProEmc->PropagateToPCA(1, 1);
	      vertex.SetXYZ(-10000, -10000, -10000); // reset vertex
	      FairTrackParH *fRes= new FairTrackParH();
	      Bool_t rc =  fProEmc->Propagate(helix, fRes, fPidHyp*par.GetQ()); // First propagation at module
	      if (!rc) continue;
	      vertex.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
	    }
	  
	  Float_t dist = (v1-vertex).Mag2();
	  if ( emcQuality > dist )
	    {
	      emcQuality = dist;
	    }
	} // end of track correlation
      
      if ( emcQuality > fCorrPar->GetEmcNeutralQCut() )
        { 
          PndPidCandidate* pidCand = new PndPidCandidate(0, vtx, lv);
          pidCand->SetP4Cov(covP4);
          pidCand->SetEmcRawEnergy(bump->energy());
          pidCand->SetEmcCalEnergy(fEmcCalibrator->Energy(bump));
          pidCand->SetEmcIndex(i);
          pidCand->SetEmcModule(bump->GetModule());
          pidCand->SetEmcNumberOfCrystals(bump->NumberOfDigis());
          pidCand->SetEmcNumberOfBumps(clu->NBumps());
          pidCand->SetEmcQuality(emcQuality);

          pidCand->SetEmcClusterZ20(bump->Z20());
          pidCand->SetEmcClusterZ53(bump->Z53());
          pidCand->SetEmcClusterLat(bump->LatMom()); 
          if (fEmcDigi)
	    {
	      PndEmcClusterEnergySums esum(*clu, fEmcDigi);
	      pidCand->SetEmcClusterE1(esum.E1());
	      pidCand->SetEmcClusterE9(esum.E9());
	      pidCand->SetEmcClusterE25(esum.E25());
	    }
          pidCand->SetLink(FairLink(emcType, i));
    
          std::vector<Int_t> mclist = clu->GetMcList();
          if (mclist.size()>0)
	    {
	      pidCand->SetMcIndex(mclist[0]);
	    }
          AddNeutralCandidate(pidCand);
        }
    }
}



void PndPidCorrelator::ResetEmcQ()
{
  // Fuction to reset all the quality values for emc-track correlation to -1
  fClusterQ.clear();
  fClusterList.clear();
  for (Int_t ii=0; ii<fEmcCluster->GetEntriesFast(); ii++)
    {
      fClusterQ[ii] = -1;
      fClusterList[ii] = kFALSE;
    }
}

//_________________________________________________________________
void PndPidCorrelator::Register() {
  //---
  TString chargName = "PidChargedCand" + fTrackOutBranch;
  FairRootManager::Instance()->
    Register(chargName,"Pid", fPidChargedCand, kTRUE); 
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
      ftofCorr->Write();
      emcCorr->Write(); 
      fscCorr->Write();
      mdtCorr->Write();  
      drcCorr->Write();
      dskCorr->Write();

      r->Save();
    
      tofCorr->Delete();
      ftofCorr->Delete();
      emcCorr->Delete(); 
      fscCorr->Delete();
      mdtCorr->Delete();
      drcCorr->Delete();
      dskCorr->Delete();
    
      tofCorr = 0;
      ftofCorr = 0;
      emcCorr = 0; 
      fscCorr = 0;
      mdtCorr = 0;
      drcCorr = 0;
      dskCorr = 0;
    
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
  fEmcClstCount = 0;
  fFscClstCount = 0;
  fClusterList.clear();
  fClusterQ.clear();
  mapMdtBarrel.clear();
  mapMdtEndcap.clear();
  mapMdtForward.clear();
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
