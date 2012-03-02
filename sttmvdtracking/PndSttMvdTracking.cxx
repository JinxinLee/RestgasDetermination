#include "glpk.h"
#include "PndSttMvdTracking.h"

#include "PndSttHit.h"
#include "PndSciTHit.h"
#include "PndSttPoint.h"
#include "PndSttTrack.h"
#include "PndSttPoint.h"
#include "PndSttHelixHit.h"
#include "PndSttSingleStraw.h"
#include "PndSttTube.h"
#include "PndSttMapCreator.h"

#include "PndSdsHit.h"
#include "PndSdsMCPoint.h"

#include "PndTrackCand.h"
#include "PndTrackCandHit.h"
#include "PndTrack.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairTrackParP.h"

#include "TGeoManager.h"
#include "TClonesArray.h"
#include "TGeoVolume.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TH1F.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TGeoTube.h"

#include <iostream>
#include <cmath>



using namespace std;


// -----   Default constructor   -------------------------------------------
PndSttMvdTracking::PndSttMvdTracking() : FairTask("STT Stt-Mvd Tracking") { 
 fPersistence = kTRUE;
 fVerbose = 0;

 istampa = 0;
 iplotta = false;
 doMcComparison = false;
 YesClean = false;
 YesSciTil = false ;
 MvdAloneTracking = true;
 sprintf(fSttBranch,"STTHit");
 sprintf(fMvdPixelBranch,"MVDHitsPixel");
 sprintf(fMvdStripBranch,"MVDHitsStrip");
}
// -------------------------------------------------------------------------

PndSttMvdTracking::PndSttMvdTracking(Int_t verbose) : FairTask("STT Stt-Mvd Tracking") { 
 fPersistence = kTRUE;
 fVerbose = verbose;
 istampa = verbose;
 iplotta = false;
 doMcComparison = false;
 YesClean = false;
 YesSciTil = false ;
 MvdAloneTracking = true;

 sprintf(fSttBranch,"STTHit");
 sprintf(fMvdPixelBranch,"MVDHitsPixel");
 sprintf(fMvdStripBranch,"MVDHitsStrip");
}
// -------------------------------------------------------------------------

PndSttMvdTracking::PndSttMvdTracking(int istamp, bool  iplot, bool imc)
		: FairTask("STT Stt-Mvd Tracking") { 
 fPersistence = kTRUE;
 istampa = istamp;
 iplotta = iplot;
 doMcComparison = imc;
 YesClean = false;
 YesSciTil = false ;
 MvdAloneTracking = true;
 sprintf(fSttBranch,"STTHit");
 sprintf(fMvdPixelBranch,"MVDHitsPixel");

 sprintf(fMvdStripBranch,"MVDHitsStrip");
}


// -------------------------------------------------------------------------

PndSttMvdTracking::PndSttMvdTracking(int istamp, bool  iplot, bool imc, bool doSciTil)
		: FairTask("STT Stt-Mvd Tracking") { 
 fPersistence = kTRUE;
 istampa = istamp;
 iplotta = iplot;
 doMcComparison = imc;
 YesClean = false;
 YesSciTil = doSciTil ;
 MvdAloneTracking = true;
 sprintf(fSttBranch,"STTHit");
 sprintf(fMvdPixelBranch,"MVDHitsPixel");

  sprintf(fMvdStripBranch,"MVDHitsStrip");
}
// -----   Destructor   ----------------------------------------------------
PndSttMvdTracking::~PndSttMvdTracking() { 

}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndSttMvdTracking::Init() {


 ZCENTER_STRAIGHT = 35.;
 SEMILENGTH_STRAIGHT = 75.;


 if(iplotta){
  hdeltaRPixel = new TH1F("hdeltaRPixel", "distance MC Pixel point from trajectory in XY plane", 100, -1, 1);
  hdeltaRStrip = new TH1F("hdeltaRStrip", "distance MC Strip point from trajectory in XY plane", 100, -1, 1);
  hdeltaRPixel2 = new TH1F(
  "hdeltaRPixel2", "distance MC point from trajectory in XY plane (Pixels)", 100, -10, 10);
  hdeltaRStrip2 = new TH1F(
  "hdeltaRStrip2", "distance MC point from trajectory in XY plane (Strips)", 100, -10,10);
}


 IVOLTE=-1;




//  --------------------------- opening files for special purposes

int N_INTENDED=0;

if(istampa >=1 ){
//---- fetch the n. of tracks MC that were intended to be generated
//   HANDLE = fopen("n_intended_tracks.txt","r");
//   fscanf(HANDLE,"%d",&N_INTENDED);
//   fclose(HANDLE);
//---- apertura file con info su Found tracce su cui si fa Helix fit dopo
   HANDLE2 = fopen("info_da_PndTrackFinderReal.txt","w");

//  ---- open filehandle per statistica sugli hits etc.
   HANDLE = fopen("statistiche.txt","w");
//  ---------------
//   HANDLEXYZ = fopen("infoPndTrackFinderRealXYZ.txt","w");

//  ---------------  open file delle info su deltaX, Y, Z  degli hits in comune tra tracce trovate e MC
   PHANDLEX = fopen("deltaParXmio.txt","w");
   PHANDLEY = fopen("deltaParYmio.txt","w");
   PHANDLEZ = fopen("deltaParZmio.txt","w");
   SHANDLEX = fopen("deltaSkewXmio.txt","w");
   SHANDLEY = fopen("deltaSkewYmio.txt","w");
   SHANDLEZ = fopen("deltaSkewZmio.txt","w");

}  //  end of if(istampa >=1)


// -------------------------



  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndSttMvdTracking::Init: "
	 << "RootManager not instantiated, return!" << endl;
    return kFATAL;
  }
//  -----   maps of STT tubes
  // CHECK added 
  PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  fSttTubeArray = mapper->FillTubeArray();
 //----------------------------------------------------  end map




//    get   the MCTrack  array
  fMCTrackArray = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCTrackArray) 
    {
      cout << "-E- PndSttMvdTracking::Init: No MCTrack array, return!"
	   << endl;
      return kERROR;
    }


  // open STTFoundTrack array 
  fSttTrackArray = (TClonesArray*) ioman->GetObject("STTFoundTrack");
  if(!fSttTrackArray) {
     cout << "-E- PndSttMvdTracking::Init: No STTFoundTrack array, return!"
	   << endl;
    return kERROR;
  }

 // Get SttTrackCand array  dal pattern recognition di STT
  fSttTrackCandArray  = (TClonesArray*) ioman->GetObject("STTTrackCand"); 
  if ( ! fSttTrackCandArray) 
    {
      cout << "-E- PndSttMvdTracking::Init: No SttTrack Cand  array, return!"
	   << endl;
      return kERROR;
    }
	
  // Get input array   questi sono i MC point di STT
  fSttPointArray = (TClonesArray*) ioman->GetObject("STTPoint");
  if ( ! fSttPointArray ) {
    cout << "-W- PndSttHelixHitProducer::Init: "
	 << "No STTPoint array, return!" << endl;
    return kERROR;
  }

  // Get input array   hit di STT after digi
  fSttHitArray = (TClonesArray*) ioman->GetObject(fSttBranch);
//  fSttHitArray = (TClonesArray*) ioman->GetObject("STTHit");
  if ( ! fSttHitArray ) {
    cout << "-W- PndSttMvdTracking::Init: "
	 << "No STTHit array, return!" << endl;
    return kERROR;
  }
  // Create and register output array for PndTrackCand of Stt+Mvd combined

  fSttMvdPndTrackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("SttMvdTrackCand","SttMvd",fSttMvdPndTrackCandArray, kTRUE);


  // Create and register output array for PndTrack of Stt+Mvd combined

  fSttMvdPndTrackArray = new TClonesArray("PndTrack");
  ioman->Register("SttMvdTrack","SttMvd",fSttMvdPndTrackArray, kTRUE);



//  -------------------------   get the Mvd hits
  fMvdPixelHitArray = (TClonesArray*) ioman->GetObject(fMvdPixelBranch);
//  fMvdPixelHitArray = (TClonesArray*) ioman->GetObject("MVDHitsPixel");
  if ( !fMvdPixelHitArray){
    std::cout << "-W- PndSttMvdTracking::Init: " << "No MVD Pixel hitArray, return!" << std::endl;
    return kERROR;
  }
  fMvdStripHitArray = (TClonesArray*) ioman->GetObject(fMvdStripBranch);
//  fMvdStripHitArray = (TClonesArray*) ioman->GetObject("MVDHitsStrip");

  if ( !fMvdStripHitArray){
    std::cout << "-W- PndSttMvdTracking::Init: " << "No MVD Strip hitArray, return!" << std::endl;
    return kERROR;
  }

//  -------------------------   get the Mvd track candidates

  fMvdTrackCandArray = (TClonesArray*) ioman->GetObject("MVDRiemannTrackCand");
  if ( !fMvdTrackCandArray){
    std::cout << "-W- PndSttMvdTracking::Init: " << "No MVD TrackCand Array, return!" << std::endl;
    return kERROR;
  }

  cout << "-I- PndSttMvdTracking: Initialization successfull" << endl;
  
//  -------------------------   get the Mvd MC points

  fMvdMCPointArray = (TClonesArray*) ioman->GetObject("MVDPoint");
  if ( !fMvdMCPointArray){
    std::cout << "-W- PndSttMvdTracking::Init: " << "No MVD MC Point Array, return!" << std::endl;
    return kERROR;
  }

//  -------------------------   get the SciTil hits
  if(YesSciTil) {
	fSciTHitArray = (TClonesArray*) ioman->GetObject("SciTHit");
  } else {
	fSciTHitArray = NULL;
  }
//---------------------------

  cout << "-I- PndSttMvdTracking: Initialization successfull" << endl;
  



  return kSUCCESS;

}

// -------------------------------------------------------------------------




void PndSttMvdTracking::SetParContainers() {
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}




void PndSttMvdTracking::WriteHistograms(){

/*
  TFile* file = FairRootManager::Instance()->GetOutFile();
  file->cd();
  file->mkdir("PndSttHelixHit");
  file->cd("PndSttHelixHit");
  
  hx->Write();
  delete hx;
*/
  TFile* file = FairRootManager::Instance()->GetOutFile();
  file->cd();
  file->mkdir("PndSttMvdTracking");
  file->cd("PndSttMvdTracking");
  hdeltaRPixel->Write();
  hdeltaRStrip->Write();
  hdeltaRPixel2->Write();
  hdeltaRStrip2->Write();
  delete hdeltaRPixel;
  delete hdeltaRStrip;
  delete hdeltaRPixel2;
  delete hdeltaRStrip2;

}

// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
void PndSttMvdTracking::Exec(Option_t* opt) {


 bool	intersect,
	Status,
	keepit[MAXTRACKSPEREVENT],
	status[MAXTRACKSPEREVENT],
	SttSZfit[MAXTRACKSPEREVENT],
	Mvdhits[MAXTRACKSPEREVENT];


 Short_t Candidato,
         Charge,
	 statusflag[MAXTRACKSPEREVENT],
         iHit,
	 i,
	 j,
	 k,
	 m,
	 resultFitSZagain[MAXTRACKSPEREVENT],
	 tubeID
         ;

  Short_t  daTrackFoundaTrackMC[MAXTRACKSPEREVENT],
	   CHARGE[MAXTRACKSPEREVENT];


 UShort_t
	ipinco,
	kall,
	l,
	n,
	nalone,
	ncand,
	nhitsinfit,
	Nint,
	nMvdMCPoint,
	nRemainingCandidates,
	nSttHit,
	nSttParHit,
	nSttSkewHit,
	nSttHelixTrack,
	nSttMCPoint,
	nSttTrackCand,
	nTotalCandidates,
	nXYZhits,
//	TemporarynSkewHitsinTrack,
	FromHitToMCTrack[nmaxSttHits],
	ListAllParHits[nmaxSttHits],
	ListAllSkewHits[nmaxSttHits],
	nHitsInMCTrack[MAXTRACKSPEREVENT],
	nMCParalAlone[MAXTRACKSPEREVENT],
	nMCSkewAlone[MAXTRACKSPEREVENT],
	nParalCommon[MAXTRACKSPEREVENT],
	nSkewCommon[MAXTRACKSPEREVENT],
	nSkewHitsInMCTrack[MAXTRACKSPEREVENT],
	nSpuriParinTrack[MAXTRACKSPEREVENT],
	nSpuriSkewinTrack[MAXTRACKSPEREVENT],
	nSttHitsinTrack[MAXTRACKSPEREVENT],
	TemporarySkewList[2*nmaxSttHits][2],
	ListSttHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],
	ListSttHitsinTrackType[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],
	MCParalAloneList[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],
	MCSkewAloneList[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],
	ParalCommonList[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],
	ParSpuriList[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],
	SkewCommonList[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],
	SkewSpuriList[MAXTRACKSPEREVENT][nmaxSttHitsInTrack];

 Int_t	iaccept,
	nrounds0,
	nrounds1,
	ipunto,
	ListHits[nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack];

 Double_t
	Dist,
	Distance,
	Fi_final_helix_referenceframe,
	Ntras,
	posx,
	posy,
	Phi,
	Ptras,
	Rad,
	RR,
	SIGN,
	sqrtRR,
	TanL,
	Z,
	ddd,
	dis,
	dista,
	dista0,
	dista0_1,
	dista1,
	dista1_1,
	delta,
	emme,
	gap,
	highqualitycut,
	Pxini,
	px,
	Pyini,
	py,
	Pzini,
	qop,
	x,
	y,
	AloneX[nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack],
	AloneY[nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack],
	HoughFi[MAXTRACKSPEREVENT],
	KAPPA[MAXTRACKSPEREVENT],
	primoangolo[MAXTRACKSPEREVENT],
	s[2],
	Start[3],
	Trajectory_Start[MAXTRACKSPEREVENT][2],
	ultimoangolo[MAXTRACKSPEREVENT],
	versor[2],
	XintersectionList[2],
	YintersectionList[2],
	z[2],
	zeta0,
	zeta1,
	zdrift[2],
	zerror[2],
	WDX[nmaxSttHits],
	WDY[nmaxSttHits],
	WDZ[nmaxSttHits],
	Ox[MAXTRACKSPEREVENT],
	Oy[MAXTRACKSPEREVENT],
	R[MAXTRACKSPEREVENT],
	FI0[MAXTRACKSPEREVENT],
	Fifirst[MAXTRACKSPEREVENT],
	Fi_low_limit[MAXTRACKSPEREVENT],
	Fi_up_limit[MAXTRACKSPEREVENT],
	Posiz1[3],
	Posiz2[3],
	Px[MAXTRACKSPEREVENT],
	Py[MAXTRACKSPEREVENT],
	Pz[MAXTRACKSPEREVENT],
	X[nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack+nmaxSciTilHitsinTrack],
	Y[nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack+nmaxSciTilHitsinTrack],
	Sbis[2*nmaxSttHitsInTrack+nmaxMvdPixelHitsInTrack
		+nmaxMvdStripHitsInTrack+nmaxSciTilHitsinTrack], // multiplication by 2 in the
	ZEDbis[2*nmaxSttHitsInTrack+nmaxMvdPixelHitsInTrack
		+nmaxMvdStripHitsInTrack+nmaxSciTilHitsinTrack], // rather improbable chance that
	DriftRadiusbis[2*nmaxSttHitsInTrack+nmaxMvdPixelHitsInTrack+
		nmaxMvdStripHitsInTrack+nmaxSciTilHitsinTrack],// all skew hits have double
ErrorDriftRadiusbis[2*nmaxSttHitsInTrack+nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack+2],// solutions
	   SchosenPixel[MAXTRACKSPEREVENT][nmaxMvdPixelHits],
	   SchosenStrip[MAXTRACKSPEREVENT][nmaxMvdStripHits],
	   SchosenSkew[MAXTRACKSPEREVENT][nmaxSttHits], // NO multiplication by 2 here because for the
	   				// skew hits only one
	   				// solution is selected.
	   XY[nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack][2],
	   ZchosenPixel[MAXTRACKSPEREVENT][nmaxMvdPixelHits],
	   ZchosenStrip[MAXTRACKSPEREVENT][nmaxMvdStripHits],
	   ZchosenSkew[MAXTRACKSPEREVENT][nmaxSttHits],
	   ErrorchosenPixel[nmaxMvdPixelHits],
	   ErrorchosenStrip[nmaxMvdStripHits],
	   ErrorchosenSkew[nmaxSttHits],
	   info[nmaxSttHits][7],
	   TemporaryS[2*nmaxSttHits],
	   TemporaryZ[2*nmaxSttHits],
	   TemporaryZDrift[2*nmaxSttHits],
	   TemporaryZErrorafterTilt[2*nmaxSttHits];

  TVector3   Momentum,ErrMomentum,Position,ErrPosition;


//  FairMCPoint *  pSttMCPoint[nmaxSttHits];
  FairMCPoint *  puntator;

  PndTrackCand * pMvdTrackCand,
               * pSttTrackCand;
  PndTrack * pSttTrack;

  PndSttTrack * pSttHelixTrack;

  PndSttTube * pSttTube;

  PndSttHit * pSttHit;

  PndSdsHit * pMvdPixelHit,
            * pMvdStripHit;
//  PndSdsMCPoint * pMvdMCPoint;

  PndMCTrack* pMCtr;

  PndTrackCandHit pndtrackcandhit; 

  fSttMvdPndTrackCandArray->Delete();

  fSttMvdPndTrackArray->Delete();

  IVOLTE++;

if(istampa>0) {cout<<"\n\nEntering in PndSttMvdTracking, IVOLTE = "<<IVOLTE<<endl;}

//----------   fetching the MC truth tracks
	nMCTracks = fMCTrackArray->GetEntriesFast(); // num. tracce/evento
	if (istampa > 1  && IVOLTE<20) {
	     cout<<"da PndSttMvdTracking  : evento (partendo da 0)  N. "<<
	       IVOLTE<< "\n       N. di MC truth tracks  : "<<nMCTracks<<endl;
	}
	if (nMCTracks ==0){
		cout<<"da PndSttMvdTracking  :  N. di MC truth tracks = 0, return!\n"<<endl;
		return;
	} else if(nMCTracks> MAXMCTRACKS){
		cout<<"da PndSttMvdTracking  :  N. di MC truth tracks = "<<nMCTracks
		<<" and it is > MAXMCTRACKS = "<<MAXMCTRACKS<<", setting it equal to MAXMCTRACKS.\n";
		nMCTracks = MAXMCTRACKS;
	}

//------- stampe.
if(istampa>1) {cout<<"from PndSttMvdTracking, IVOLTE = "<<IVOLTE<<", n. MC Tracks "
		<<nMCTracks<<" e lista solo di quelle che vengono da (0,0,0) :\n";
		for(int ic=0;ic<nMCTracks;ic++){
			PndMCTrack* pMC = (PndMCTrack*) fMCTrackArray->At(ic);
			if ( !( fabs(pMC->GetStartVertex().X())<0.5 &&
			     fabs(pMC->GetStartVertex().Y())<0.5 &&
			     fabs(pMC->GetStartVertex().Z())<0.5 )) continue;
			double carica;
			int icode  = pMC->GetPdgCode() ;    //   PDG code of track
			double Pxx = pMC->GetMomentum().X();
			double Pyy = pMC->GetMomentum().Y();
			double aaa = sqrt( Pxx*Pxx + Pyy*Pyy);
			double Rr =   aaa*1000./(BFIELD*CVEL);// R (cm) of Helix of track projected
						//  in XY plane; B = 2 Tesla
			TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
			TParticlePDG *fParticle= fdbPDG->GetParticle(icode);
			if (icode>1000000000) carica = 1.;
			else  carica = fParticle->Charge()/3. ;    //   charge of track

			cout<<"\tTraccia n. "<<ic<<", Px "<<pMC->GetMomentum().X()
			<<", Py "<<pMC->GetMomentum().Y()
			<<", Pz "<<pMC->GetMomentum().Z()
			<<", carica = "<<carica
			<<"\n\t\tRaggio "<<Rr<<", Xvert "<<pMC->GetStartVertex().X()
			<<", Yvert "<<pMC->GetStartVertex().Y()
			<<", Zvert "<<pMC->GetStartVertex().Z()<<endl;
		}
}
//-------fine stampe.

//------------------------------

//----------   fetching the STT  MC points
 nSttMCPoint = fSttPointArray->GetEntriesFast();
 if (nSttMCPoint ==0){
	cout<<"da PndSttMvdTracking  :  N. di Stt MC points = 0\n"<<endl;
//	return;
 } else  if( nSttMCPoint>nmaxSttHits){
	cout<<"da PndSttMvdTracking  :  N. di Stt MC points = "<<nSttMCPoint
	<<" and it is > nmaxSttHits ("<<nmaxSttHits<<").\n";
 }

 if(istampa>1){
 	cout<<"from PndSttMvdTracking, printout dei "<<nSttMCPoint<<" punti MC :\n";
	for( i= 0; i< nSttMCPoint; i++){
		FairMCPoint * pSttMC = (FairMCPoint*) fSttPointArray->At(i);
		cout<<"\tpoint n. "<<i<<", Xmc "<<pSttMC->GetX()<<
		", Ymc "<<pSttMC->GetY()<<", Zmc "<<pSttMC->GetZ()
		<<", appartiene a MC Track "<< pSttMC->GetTrackID()<<endl;
	}
 }

//------------------------------




//----------   fetching the STT  hits

 nSttHit = fSttHitArray->GetEntriesFast();
 if (nSttHit ==0){
	cout<<"da PndSttMvdTracking  :  N. di Stt Hits = 0, return!\n"<<endl;
	return;
 } else if (nSttHit> nmaxSttHits) {
	cout<<"da PndSttMvdTracking  :  N. di Stt Hits = "<<nSttHit
	     <<" and it is > nmaxSttHits (="<<nmaxSttHits
	     <<"), therefore consider only the first "<<nmaxSttHits<<" hits\n"<<endl;
	nSttHit= nmaxSttHits;
 }

 if (istampa > 1  && IVOLTE<20) {
     cout<<"da PndSttMvdTracking  : evento (partendo da 0)  N. "<<
       IVOLTE<< "\n\tN. totale Hits in STT  : "<<nSttHit<<endl;
 }
// ---------------------------------------------  estraggo le info da MVD

 nMvdPixelHit = fMvdPixelHitArray->GetEntriesFast();
 nMvdStripHit = fMvdStripHitArray->GetEntriesFast();
 if(nMvdPixelHit>nmaxMvdPixelHits){
	cout<<"from PndSttMvdTracking, nMvdPixelHit is > maximum allowed ("
	<<nmaxMvdPixelHits<<") and therefore is set to "
	<<nmaxMvdPixelHits<<endl;
	nMvdPixelHit=nmaxMvdPixelHits;
 }
 if(nMvdStripHit>nmaxMvdStripHits){
	cout<<"from PndSttMvdTracking, nMvdStripHit is > maximum allowed ("
	<<nmaxMvdStripHits<<") and therefore is set to "
	<<nmaxMvdStripHits<<endl;
	nMvdStripHit=nmaxMvdStripHits;
 }


// ---------------------------------------------  get MC Points of  MVD

   nMvdMCPoint = fMvdMCPointArray->GetEntriesFast();
   if(nMvdMCPoint>nmaxMvdMCPoints) {
	cout<<"from PndSttMvdTracking, nMvdMCPoint = "<<nMvdMCPoint
	<<" and it is > the maximum number allowed ("<<nmaxMvdMCPoints<<
	")"<<
	", setting nMvdMCPoint to "<<nmaxMvdMCPoints<<endl;
	nMvdMCPoint=nmaxMvdMCPoints;
   }
   if(istampa>2&& IVOLTE<20) cout<<"N. MC Points delle Mvd = "<<nMvdMCPoint<<endl;

// ------------------------------------------ get info from trackcand  of MVD

  nMvdTrackCand = fMvdTrackCandArray->GetEntriesFast();


 if (nMvdTrackCand> MAXMVDTRACKSPEREVENT) {
	cout<<"da PndSttMvdTracking  :  N. of MvdTrackCand = "<<
	nMvdTrackCand<<" and it is > MAXMVDTRACKSPEREVENT (="<<MAXMVDTRACKSPEREVENT
	<<"),  therefore it is set to "<<MAXMVDTRACKSPEREVENT<<endl;
	nMvdTrackCand= MAXMVDTRACKSPEREVENT;
 }


  if(istampa>=3  && IVOLTE<20)cout<<"da PndSttMvdTracking  :  n. totale di Mvd track cand =    "
                     <<nMvdTrackCand<<"  --------------\n";
//---------------  fetch the Stt PndTrackCand from PR of the STT

 nSttTrackCand = fSttTrackCandArray->GetEntriesFast();
 if (nSttTrackCand ==0){
	cout<<"da PndSttMvdTracking  :  N. of SttTrackCand = 0, return!\n"<<endl;
	return;
 } else if (nSttTrackCand > MAXTRACKSPEREVENT) {
 	cout<<"da PndSttMvdTracking  :  N. of nSttTrackCand = "<<
	nSttTrackCand <<" and it is > MAXTRACKSPEREVENT (="<<MAXTRACKSPEREVENT
	<<"), analyzing only the first MAXTRACKSPEREVENT SttTrackCand!\n";
	nSttTrackCand=MAXTRACKSPEREVENT;
 }





//-------------------------------------------- fetch the SciTil hits

 if( fSciTHitArray != NULL){
	// number SciTil hits/event
	nSciTilHits = fSciTHitArray->GetEntriesFast();
	if(istampa>0)
cout<<"da PndSttMvdTracking, event "<<IVOLTE<<", "<<nSciTilHits
		<<" SciTil hits presenti inizialmente.\n";
	if( nSciTilHits>0){
	 PndSciTHit *pPndSciTHit;
	 TVector3  posiz;
	 // the first SciTil hit; this cannot be duplicate hit by definition.
		pPndSciTHit = (PndSciTHit*) fSciTHitArray->At(0);
		posiz = pPndSciTHit->GetPosition();
		if(istampa>0)
cout<<"da PndSttMvdTracking SciTil non purgati, Xpos "<<posiz.X()<<", Ypos "<<
posiz.Y()<<", Zpos "<<posiz.Z()<<endl;
		posizSciTil[0][0]=posiz.X();
		posizSciTil[0][1]=posiz.Y();
		posizSciTil[0][2]=posiz.Z();
		iaccept=1;
	// the other SciTil hits; purge them if they are duplicate.
	 for(j=1; j<nSciTilHits; j++){
		pPndSciTHit = (PndSciTHit*) fSciTHitArray->At(j);
		posiz = pPndSciTHit->GetPosition();
		if(istampa>0)
		cout<<"da PndSttMvdTracking SciTil non purgati, Xpos "
			<<posiz.X()<<", Ypos "<<
		posiz.Y()<<", Zpos "<<posiz.Z()<<endl;
		// purging the duplicate SciTil hits.
		
	    for(k=0; k<iaccept; k++){
		if(
			(fabs(posiz.X() - posizSciTil[k][0])< 1.e-20)
					&&
			(fabs(posiz.Y() - posizSciTil[k][1])< 1.e-20)
					&&
			(fabs(posiz.Z() - posizSciTil[k][2])< 1.e-20)
		  ){
			goto finish ;
		}  // end of if((fabs(posiz.X() - old...
	    } // end of for(k=0; k<iaccept; k++)
	    posizSciTil[iaccept][0]=posiz.X();
	    posizSciTil[iaccept][1]=posiz.Y();
	    posizSciTil[iaccept][2]=posiz.Z();
	    iaccept++;
	    finish: ;
	 }  // end of for(j=0; j<nSciTilHits; j++)
	 nSciTilHits=iaccept;
	 for(j=0; j<nSciTilHits; j++){
		InclusionListSciTil[j]=true;
	 }
	}  // end of if( nSciTilHits>0){




//-----------stampe.
if(istampa>0){
  cout<<"da PndSttMvdTracking, dopo purga di SciTil; n. hits = "<<nSciTilHits<<endl;
  for(j=0; j<nSciTilHits; j++){
	cout<<"da PndSttMvdTracking SciTil Xpos "<<posizSciTil[j][0]<<", Ypos "<<
	posizSciTil[j][1]<<", Zpos "<<posizSciTil[j][2]<<endl;
  }
}
//---------- fine stampe.


 }  // end of if( fSciTHitArray != NULL)


//------------------------------------------------------------------



 nSttParHit=0; 
 nSttSkewHit=0;
 for( i= 0; i< nSttHit; i++){
	InclusionListStt[i]= true ;


  pSttHit = (PndSttHit *) fSttHitArray->At(i);
  ipunto= pSttHit->GetRefIndex();// right way to extract the corrisponding MC point.
  tubeID = pSttHit->GetTubeID();
  pSttTube = (PndSttTube *) fSttTubeArray->At(tubeID);
      TVector3 center = pSttTube->GetPosition();

      // drift radius
      Double_t dradius = pSttHit->GetIsochrone();

      // wire direction
      TVector3 wiredirection = pSttTube->GetWireDirection();

      if(wiredirection.Z() >=0.) {
       WDX[i] = wiredirection.X();     WDY[i] = wiredirection.Y(); WDZ[i] = wiredirection.Z();
      }  else {
       WDX[i] = -wiredirection.X();     WDY[i] = -wiredirection.Y(); WDZ[i] = -wiredirection.Z();
      }
      info[i][0]= pSttTube->GetPosition().X();
      info[i][1]= pSttTube->GetPosition().Y();
      info[i][2]= pSttTube->GetPosition().Z();
      info[i][3]= dradius;
      info[i][4]= pSttTube->GetHalfLength();
      if(ipunto>=0) {
	puntator = (FairMCPoint*) fSttPointArray->At(ipunto);
	info[i][6]= puntator->GetTrackID();
      } else {
	info[i][6]= -10.;
      }

      if( fabs( WDX[i] )< 0.00001 && fabs( WDY[i] )< 0.00001 ){
          info[i][5]= 1.;
	  ListAllParHits[nSttParHit]=i;
	  nSttParHit++;
          ZCENTER_STRAIGHT = info[i][2];      //    this works because just few lines below there is the
          SEMILENGTH_STRAIGHT = info[i][4];   //    requirement that Minclinations[0] > 2 (= at least 3 parallel straws)
       } else {
          info[i][5]= 99.;	// to signal that it is a skew straw.
	  ListAllSkewHits[nSttSkewHit]=i;
	  nSttSkewHit++;
       }



//--------------- inizio stampaggi,  stampe di controllo
  if (istampa >= 2) {
      cout <<"da PndSttMvdTracking, Stt iHit "<< i << " e n. punto MC ottenuto con RefIndex = "
      	<<ipunto<<endl;
      if(ipunto<0) {
      	cout<<"hit di background da mixing!\n";
      	cout <<"             hit wire pos. in middle "   << pSttTube->GetPosition().X() << " " <<
             pSttTube->GetPosition().Y() << " " << pSttTube->GetPosition().Z() 
           << "; R = "<<sqrt(pSttTube->GetPosition().X()*pSttTube->GetPosition().X()+
	   pSttTube->GetPosition().Y()*pSttTube->GetPosition().Y())<<
	   ", suo drift radius = "<<dradius <<endl;
      	cout <<"             wire direction, X, Y, Z (Z direction set always positive)"
      	<< WDX[i]<<"  "<<WDY[i]<<"  "<<WDZ[i] <<endl;
      } else{
      cout <<"             hit X, Y, Z space position "   << puntator->GetX() << " " <<
                       puntator->GetY() << " " << puntator->GetZ()<<endl; 
      cout <<"             hit wire pos. in middle "   << pSttTube->GetPosition().X() << " " <<
             pSttTube->GetPosition().Y() << " " << pSttTube->GetPosition().Z() 
           << "; R = "<<sqrt(pSttTube->GetPosition().X()*pSttTube->GetPosition().X()+
	   pSttTube->GetPosition().Y()*pSttTube->GetPosition().Y())<<
	   ", suo drift radius = "<<dradius <<endl;
      cout <<"             wire direction, X, Y, Z (Z direction set always positive)"
      << WDX[i]<<"  "<<WDY[i]<<"  "<<WDZ[i] <<endl
           <<"             this hit belongs to MC track n. "<<puntator->GetTrackID()<<endl;
      }
  }  //  end of   if(istampa >= 

//--------  fine stampaggi



  }  //   end of for( i= 0; i< nSttHit; i++)


//	fill the exclusion list for those straws with multiple hits


      //   first the parallel straws
      for(i=0; i< nSttHit-1; i++){
	if( !InclusionListStt[ i ] ) continue;
			for(j=i+1; j< nSttHit; j++){
				if(InclusionListStt[ j ] &&
					fabs(info[i][0] - info[j][0])<1.e-20 &&
					fabs(info[i][1] - info[j][1])<1.e-20  )
				{
					InclusionListStt[j]= false ;
				}
			} //  end of  for(j=i+1; j< Nhits;; j++)


      }   //   end of for(i=0; i< Nhits-1; i++)



//-----------------------------------  end of exclusion of straws with multiple hits




//-------------------------------------------------------------  fine hits STT


// ---------------------------------------------  estraggo gli HITS Pixel MVD
 for( i= 0; i< nMvdPixelHit; i++){
    pMvdPixelHit = (PndSdsHit *) fMvdPixelHitArray->At(i);
//    ipunto = pMvdPixelHit->GetRefIndex();
    TVector3 temp = pMvdPixelHit->GetPosition();
    XMvdPixel[i] = temp.X();
    YMvdPixel[i] = temp.Y();
    ZMvdPixel[i] = temp.Z();
    sigmaXMvdPixel[i] = pMvdPixelHit->GetDx();
    sigmaYMvdPixel[i] = pMvdPixelHit->GetDy();
    sigmaZMvdPixel[i] = pMvdPixelHit->GetDz();
    refindexMvdPixel[i] = pMvdPixelHit->GetRefIndex();
//	cout<<"\tPixel n. "<<i<<" and n. MC point obtained with RefIndex = "<<ipunto<<endl;
 }

// ------------------------------------- fine di estraggo gli HITS Pixel MVD


// -------------------------------------------  estraggo gli HITS Strip MVD

  for( i= 0; i< nMvdStripHit; i++){
    pMvdStripHit = (PndSdsHit *) fMvdStripHitArray->At(i);
//    ipunto = pMvdStripHit->GetRefIndex();
    TVector3 temp = pMvdStripHit->GetPosition();
    XMvdStrip[i] = temp.X();
    YMvdStrip[i] = temp.Y();
    ZMvdStrip[i] = temp.Z();
    sigmaXMvdStrip[i] = pMvdStripHit->GetDx();
    sigmaYMvdStrip[i] = pMvdStripHit->GetDy();
    sigmaZMvdStrip[i] = pMvdStripHit->GetDz();
    refindexMvdStrip[i] = pMvdStripHit->GetRefIndex();
//	cout<<"\tStrip n. "<<i<<" and n. MC point obtained with RefIndex = "<<ipunto<<endl;
  }
// ------------------------------------ fine di estraggo gli HITS Strip MVD

//------------------------------------------ stampaggi  hits MVD
if(istampa>=2){
  cout<<"da PndSttMvdTraking  :  n. Mvd Pixel Hits = "<<nMvdPixelHit<<endl;
  cout<<"da PndSttMvdTracking  :  n. Mvd Strip Hits = "<<nMvdStripHit<<endl;
}
if(istampa>=2){
  cout<<"da PndSttMvdTracking  :  info hits di Mvd pixels ------------------\n";
  for(i= 0; i< nMvdPixelHit; i++){
      cout<<"      Pixel hit n. "<<i<<" Info : X  = "<<XMvdPixel[i]<<";  Y  = "<<YMvdPixel[i]<<
            ";  Z  = "<<ZMvdPixel[i]<<", R=sqrt(X**2+Y**2) = "<<
	    sqrt(XMvdPixel[i]*XMvdPixel[i]+YMvdPixel[i]*YMvdPixel[i])<<endl;
      cout<<"\t\tPixel Info : sigmaX  = "<<sigmaXMvdPixel[i]<<";  sigmaY  = "<<sigmaYMvdPixel[i]<<
            ";  sigmaZ  = "<<sigmaZMvdPixel[i]<<endl<<"\t suo RefIndex = "<<refindexMvdPixel[i]<<
	    ", suo FairRootManager::Instance()->GetBranchId(fMvdPixelBranch) = "<<
	    FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)<<endl;
  }
  cout<<"        ------------------\n";

  cout<<"da PndSttMvdTracking  :  info hits di Mvd strips ------------------\n";
  for(i= 0; i< nMvdStripHit; i++){
      cout<<"      Strip hit n. "<<i<<" Info : X  = "<<XMvdStrip[i]<<";  Y  = "<<YMvdStrip[i]<<
            ";  Z  = "<<ZMvdStrip[i]<<", R=sqrt(X**2+Y**2) = "<<
	    sqrt(XMvdStrip[i]*XMvdStrip[i]+YMvdStrip[i]*YMvdStrip[i])<<endl;
      cout<<"\t\tStrip Info : sigmaX  = "<<sigmaXMvdStrip[i]<<";  sigmaY  = "<<sigmaYMvdStrip[i]<<
            ";  sigmaZ  = "<<sigmaZMvdStrip[i]<<endl<<"\t suo RefIndex = "<<refindexMvdStrip[i]<<
	    ", suo FairRootManager::Instance()->GetBranchId(fMvdStripBranch) = "<<
	    FairRootManager::Instance()->GetBranchId(fMvdStripBranch)<<endl;

  }
  cout<<"        ------------------\n";

  cout<<"-----------------------------------------------------\n";
}   //   fine    if(istampa>=2  && IVOLTE<20)

//------------------------------------------ stampaggi


// ------------------------------------------ estraggo le altre info della trackcand  del MVD

  bool inMvdTrackCandPixel[nmaxMvdPixelHits],
       inMvdTrackCandStrip[nmaxMvdStripHits];


  for(j=0; j<nMvdPixelHit; j++){
	inMvdTrackCandPixel[j]=false;
  }
  for(j=0; j<nMvdStripHit; j++){
	inMvdTrackCandStrip[j]=false;
  }

  for( i= 0; i< nMvdTrackCand ; i++){

     pMvdTrackCand= (PndTrackCand*) fMvdTrackCandArray->At(i);
     TVector3 dirSeed=pMvdTrackCand->getDirSeed();
     TVector3 posSeed=pMvdTrackCand->getPosSeed();
     qop = pMvdTrackCand->getQoverPseed();
     nHitMvdTrackCand[i] = pMvdTrackCand->GetNHits();  // n. hits in questa track cand
     if( nHitMvdTrackCand[i]>nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack){
	cout<<"from PndSttMvdTracking, nHitMvdTrackCand[i] = "<<nHitMvdTrackCand[i]
		<<" and it is > nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack (="
		<<nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack
		<<"); setting nHitMvdTrackCand[i] to "
		<<nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack<<endl;
	nHitMvdTrackCand[i]=nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack;
     }

     UShort_t kPixel,kStrip;

     for(j=0, k=0, kPixel=0, kStrip=0; j<nHitMvdTrackCand[i]; j++){
	pndtrackcandhit = pMvdTrackCand->GetSortedHit(j);

	// the following case should never happen (in principle), but, just to be on
	// the safe side ....
	if(pndtrackcandhit.GetHitId()<0 || pndtrackcandhit.GetDetId()<0)continue;

	// this is a Pixel.
	if( pndtrackcandhit.GetDetId()==
		FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)
			&& kPixel < nmaxMvdPixelHitsInTrack){

		// the following is a protection because the maximum Mvd hit n. cannot exceed
		// nmaxMvdPixelHits .
		if( pndtrackcandhit.GetHitId()>nmaxMvdPixelHits ){
			cout<<"from PndSttMvdTracking, this Pixel Mvd hit has a number = "
			<<pndtrackcandhit.GetHitId()<<
			" that is > nmaxMvdPixelHits (="<<nmaxMvdPixelHits<<"), rejected!\n";
			continue;
		}

		inMvdTrackCandPixel[ pndtrackcandhit.GetHitId() ]= true;
		ListHitTypeMvdTrackCand[i][k] = pndtrackcandhit.GetDetId();
					  // questo in realta' e' il Branch dello
					  // Hit che viene usato - stupidamente - per dire che e'
					  // un Pixel. Roba da matti.
					  // Se e' -1 dovrebbe essere noise ma a in quale Pixel
					  // o Strip?? Mistero.
		kPixel++;

	// this is a Strip.
	} else if( pndtrackcandhit.GetDetId()==
		FairRootManager::Instance()->GetBranchId(fMvdStripBranch)
			&& kStrip < nmaxMvdStripHitsInTrack){

		// the following is a protection because the maximum Mvd hit n. cannot exceed
		// nmaxMvdStripHits .
		if( pndtrackcandhit.GetHitId()>nmaxMvdStripHits ){
			cout<<"from PndSttMvdTracking, this Strip Mvd hit has a number = "
			<<pndtrackcandhit.GetHitId()<<
			" that is > nmaxMvdStripHits (="<<nmaxMvdStripHits<<"), rejected!\n";
			continue;
		}

		inMvdTrackCandStrip[ pndtrackcandhit.GetHitId() ]= true;
		ListHitTypeMvdTrackCand[i][k] = pndtrackcandhit.GetDetId();
					  // questo in realta' e' il Branch dello
					  // Hit che viene usato - stupidamente - per dire che e'
					  // un Pixel. Roba da matti.
					  // Se e' -1 dovrebbe essere noise ma a in quale Pixel
					  // o Strip?? Mistero.
		kStrip++;
	} else {	// this is the case should (in principle) never happen.
		continue;	// ignore this hit.
	}

	// questo e' il n. Hit nativo che posso usare
	// per estrarre tutte le info che voglio. Se il n. e' -1
	// dopo non lo considero; chiedere a Tobias cos'e' perche'
	// anche il suo detID e' -1 : ne' Pixel ne' Strip...!
	ListHitMvdTrackCand[i][k] = pndtrackcandhit.GetHitId();

	k++;

     }    //   end of    for(j=0; j<nHitMvdTrackCand; j++)

     nHitMvdTrackCand[i]=k;	// if the case, readjust the # of hits in this candidate.

  }   //   end of        for( i= 0; i< nMvdTrackCand ; i++)


//  now load the arrays containing the Pixel Hits and Strip Hits
// not belonging to any Mvd Track Cand

  nMvdUSPixelHitNotTrackCand=0;
  nMvdDSPixelHitNotTrackCand=0;
  nMvdUSStripHitNotTrackCand=0;
  nMvdDSStripHitNotTrackCand=0;
  for( i= 0; i< nMvdPixelHit ; i++){
	if( ! inMvdTrackCandPixel[i] ){
		if( ZMvdPixel[i]>=0.){
			ListMvdDSPixelHitNotTrackCand[nMvdDSPixelHitNotTrackCand] = i;
			nMvdDSPixelHitNotTrackCand++;
		} else {
			ListMvdUSPixelHitNotTrackCand[nMvdUSPixelHitNotTrackCand] = i;
			nMvdUSPixelHitNotTrackCand++;
		}
	}
  }

  for( i= 0; i< nMvdStripHit ; i++){
	if( ! inMvdTrackCandStrip[i] ){
		if( ZMvdStrip[i]>=0.){
			ListMvdDSStripHitNotTrackCand[nMvdDSStripHitNotTrackCand] = i;
			nMvdDSStripHitNotTrackCand++;
		} else {
			ListMvdUSStripHitNotTrackCand[nMvdUSStripHitNotTrackCand] = i;
			nMvdUSStripHitNotTrackCand++;
		}
	}
  }

 if(istampa>=2){
	for( i= 0; i< nMvdTrackCand ; i++){
		cout<<" PndSttMvdTracking, MvdTrackCand n. "<<i<<" ha "<<nHitMvdTrackCand[i]
		<<" hits Mvd. Ecco la loro lista :\n";
		for(j=0;j<nHitMvdTrackCand[i];j++){
			cout<<"\thit n. "<< ListHitMvdTrackCand[i][j]<<" tipo "<<
			ListHitTypeMvdTrackCand[i][j]<<endl;
		}
	}

	cout<<"--------------------\nOra lista hits Upstream Pixel in nessuna Mvd track Cand : "<<
		nMvdUSPixelHitNotTrackCand<<" hits in tutto;\n";
	for(j=0;j<nMvdUSPixelHitNotTrackCand;j++){
		cout<<"\tHit n. "<<ListMvdUSPixelHitNotTrackCand[j]<<endl;
	}
	cout<<"--------------------\nOra lista hits Downstream Pixel in nessuna Mvd track Cand : "<<
		nMvdDSPixelHitNotTrackCand<<" hits in tutto;\n";
	for(j=0;j<nMvdDSPixelHitNotTrackCand;j++){
		cout<<"\tHit n. "<<ListMvdDSPixelHitNotTrackCand[j]<<endl;
	}
	cout<<"--------------------\nOra lista hits Upstream Strip in nessuna Mvd track Cand : "<<
		nMvdUSStripHitNotTrackCand<<" hits in tutto;\n";
	for(j=0;j<nMvdUSStripHitNotTrackCand;j++){
		cout<<"\tHit n. "<<ListMvdUSStripHitNotTrackCand[j]<<endl;
	}

	cout<<"--------------------\nOra lista hits Downstream Strip in nessuna Mvd track Cand : "<<
		nMvdDSStripHitNotTrackCand<<" hits in tutto;\n";
	for(j=0;j<nMvdDSStripHitNotTrackCand;j++){
		cout<<"\tHit n. "<<ListMvdDSStripHitNotTrackCand[j]<<endl;
	}
	cout<<"-----------------------------\n";
}
// ------------------------------------- fine estraggo le altre info della traccand  del MVD




//---------------  fetch the Stt PndTrackCand from PR of the STT alone

 if(istampa>0) {
	cout<<"PndSttMvdTracking  : evt. "<<IVOLTE<<
	", n. PndTrackCand passed to PndSttMvdTracking = "
	<<nSttTrackCand<<endl;
 }

 for(  i= 0; i< nSttTrackCand; i++){
   pSttTrack = (PndTrack*) fSttTrackArray->At(i);

// ----  estraggo gli hits appartenenti a questa TrackCand
  pSttTrackCand = pSttTrack->GetTrackCandPtr();
  nSttHitsinTrack[i] = pSttTrackCand->GetNHits();  // n. hits in questa track cand
  if( nSttHitsinTrack[i]> nmaxSttHits) {
	cout<<"da PndSttMvdTracking  :  N. di Stt Hits in trackcand "<<i<<" is "
	<<nSttHitsinTrack[i]
	     <<" and it is > nmaxSttHits  (="<<nmaxSttHits
	     <<"), therefore consider only the first "<<nmaxSttHits<<" hits\n"<<endl;
	nSttHitsinTrack[i]= nmaxSttHits;
  }

  Int_t igoodStt=-1;

 for(	j=0,
	nSciTilHitsinTrack[i]=0,
	nSttParHitsinTrack[i]=0,
	nSttSkewHitsinTrack[i]=0;
	j<nSttHitsinTrack[i];j++){
	pndtrackcandhit = pSttTrackCand->GetSortedHit(j);
    // necessary check to exclude hits whose number is > nmaxSttHits.


    if(pndtrackcandhit.GetDetId()==1001){// my SciTil detector ID.
	if( nSciTilHitsinTrack[i]>0) {
		cout<<"PndSttMvdTracking, more than 1 SciTilHit in this track,;"
		<<" consider only the first!\n";
	}else{
		nSciTilHitsinTrack[i] ++;
		ListSciTilHitsinTrack[i][0]=pndtrackcandhit.GetHitId();
		InclusionListSciTil[ ListSciTilHitsinTrack[i][0] ] = false;
	}
    } else {	// this is a Stt Det Id
	if( pndtrackcandhit.GetHitId() >= nmaxSttHits ) continue;
	igoodStt++;
	if(igoodStt==0) {
	//  for the calculation of the approximate Fi of the first hit in those track
		x = info[ pndtrackcandhit.GetHitId() ][0];//in the middle of the tube
		y = info[ pndtrackcandhit.GetHitId() ][1];//in the middle of the tube
	}

	ListSttHitsinTrack[i][nSttParHitsinTrack[i]+nSttSkewHitsinTrack[i]] =
		pndtrackcandhit.GetHitId(); // # hit of Stt
	if ( fabs(info[ pndtrackcandhit.GetHitId() ][5]- 1.) < 0.0001) {
		//  axial Stt hits
		ListSttHitsinTrackType[i][nSttParHitsinTrack[i]+nSttSkewHitsinTrack[i]]
			= 2; 
		ListSttParHitsinTrack[i][nSttParHitsinTrack[i]]
			= pndtrackcandhit.GetHitId(); // # hit of Stt
		nSttParHitsinTrack[i]++;
	} else {
		//  skew Stt hits
		ListSttHitsinTrackType[i][nSttParHitsinTrack[i]+nSttSkewHitsinTrack[i]]
			= 3; 
		ListSttSkewHitsinTrack[i][nSttSkewHitsinTrack[i]] =
			pndtrackcandhit.GetHitId(); // # hit of Stt
		nSttSkewHitsinTrack[i]++;
	}

    }  //  end of  if(pndtrackcandhit.GetDetId()==1001)

  }    //   end of    for(j=0; j<nSttHitsinTrack[i]; j++)


//-------------stampa.
 if(istampa>=2){
           cout<<"da PndSttMvdTracking, evt. "<<IVOLTE<<
	   " at the beginning, from the TrackCand TClonesarray :\n"<<
	   "	SttTrackCand n.  "<<i<<";  n. || Hits in Stt in Track = "
	   <<nSttParHitsinTrack[i]<<"   e loro lista \n";
	   for(j=0; j<nSttParHitsinTrack[i];j++){
	        cout<<"\t\t|| Stt Hit n. "<<
		     ListSttParHitsinTrack[i][j]<<endl;
	   }
           cout<<"da PndSttMvdTracking --------------------------------------\n"<<
	   "	SttTrackCand n.  "<<i<<";  n. skew Hits in Stt Track = "
	   <<nSttSkewHitsinTrack[i]<<"   e loro lista \n";
	   for(j=0; j<nSttSkewHitsinTrack[i];j++){
	        cout<<"\t\tskew Stt Hit n. "<<
		     ListSttSkewHitsinTrack[i][j]<<endl;
	   }
           cout<<"da PndSttMvdTracking --------------------------------------\n"<<
	   "	SttTrackCand n.  "<<i<<";  n. scitil Hits in Stt Track = "
	   <<nSciTilHitsinTrack[i]<<"   e loro lista \n";
	   for(j=0; j<nSciTilHitsinTrack[i];j++){
	        cout<<"\t\tscitil Hit n. "<<
		     ListSciTilHitsinTrack[i][0]<<endl;
	   }

 }
//-------fine stampa.

  // redefinition of the total number of Stt hits.
  nSttHitsinTrack[i] = nSttParHitsinTrack[i]+nSttSkewHitsinTrack[i];

//  assume the trajectory comes from (0,0).
  Trajectory_Start[0][i]=0.;
  Trajectory_Start[1][i]=0.;


  TVector3	dirSeed,
		posSeed;
	dirSeed=pSttTrackCand->getDirSeed();
	posSeed=pSttTrackCand->getPosSeed();
	qop = pSttTrackCand->getQoverPseed();
	daTrackFoundaTrackMC[i]=pSttTrackCand->getMcTrackId();
	Px[i] = dirSeed.X();
	Py[i] = dirSeed.Y();
	Pz[i] = dirSeed.Z();
	if( dirSeed.Z() < 9990.) {    // normal case with skew hits information existing
	  SttSZfit[i]=true;
	  Ntras = sqrt(dirSeed.X()*dirSeed.X()+dirSeed.Y()*dirSeed.Y() ) ;
	  R[i] =  Ntras /(0.003*BFIELD*fabs(qop));
	  if(qop <0.){
	    Ox[i] = -R[i]*dirSeed.Y()/Ntras ;
	    Oy[i] =  R[i]*dirSeed.X()/Ntras ;
	    KAPPA[i]=0.003*BFIELD*fabs(qop)/dirSeed.Z();
	    CHARGE[i]=-1;
	  } else {
	    Ox[i] =  R[i]*dirSeed.Y()/Ntras ;
	    Oy[i] = -R[i]*dirSeed.X()/Ntras ;
            KAPPA[i]=-0.003*BFIELD*fabs(qop)/dirSeed.Z();
	    CHARGE[i]=1;
	  }
        }  else if (dirSeed.Z()-10000.<0.) {     // case without skew straws info present
	  SttSZfit[i]=false;
          Ptras = sqrt(dirSeed.X()*dirSeed.X()+dirSeed.Y()*dirSeed.Y() ) ;
	  R[i] =  Ptras /(0.003*BFIELD);
          if(qop <0.){
	     Ox[i] = -R[i]*dirSeed.Y()/Ptras;
	     Oy[i] = R[i]*dirSeed.X()/Ptras;
	    CHARGE[i]=-1;
	  } else {
	     Ox[i] = R[i]*dirSeed.Y()/Ptras;  
	     Oy[i] = -R[i]*dirSeed.X()/Ptras;
	    CHARGE[i]=1;
	  }
	} else {     // case in which  KAPPA = 0
	  SttSZfit[i]=true;
          if(qop <0.){
	    Ox[i] = -R[i]*dirSeed.Y()/Ntras;
	    Oy[i] = R[i]*dirSeed.X()/Ntras;
	    CHARGE[i]=-1;
	  } else {
	     Ox[i] = R[i]*dirSeed.Y()/Ptras;  
	     Oy[i] = -R[i]*dirSeed.X()/Ptras;
	    CHARGE[i]=1;
	  }
             KAPPA[i]=0.;
	}

	FI0[i] = atan2(-Oy[i], -Ox[i]);
	Ox[i] +=  Trajectory_Start[0][i];
	Oy[i] +=  Trajectory_Start[1][i];


	Fifirst[i] = atan2( y-Oy[i], x-Ox[i]);
	HoughFi[i] = FI0[i] + PI;
	if(HoughFi[i]<0.) HoughFi[i]=0.;
	if( FI0[i] < 0. )  FI0[i]+= 2.*PI; if( FI0[i] < 0. ) FI0[i]=0.;
	if( Fifirst[i] < 0. )  Fifirst[i]+= 2.*PI;if( Fifirst[i] < 0. )  Fifirst[i]=0.;



 }            //   end of   for(  i= 0; i< nSttTrackCand; i++)

//---------------  end fetching of the Stt PndTrackCand from PR of the STT

//----------- treat here the special pathological case when there are no Mvd hits at all.
//----------- Copy simply the quantities from the STT alone PndTrackCand.
 if(nMvdPixelHit+nMvdStripHit==0){
	for( ncand= 0; ncand< nSttTrackCand; ncand++){
		int nbuoni, nparbuoni, nskewbuoni;
		for(j=0, nbuoni=0,nparbuoni=0, nskewbuoni=0;j<nSttHitsinTrack[ncand]; j++){

		  if(ListSttHitsinTrackType[ncand][j]==3){ // skew stt.

			k=ListSttHitsinTrack[ncand][j];
			CalculateSandZ(
					Ox[ncand],
					Oy[ncand],
					R[ncand],
					k,
					info,
					WDX,
					WDY,
					WDZ,
					s,
					z,
					zdrift,
					zerror
					);
			if( z[0]<999998. && z[1]<999998.){

			  dista0 = Dist_SZ(R[ncand],KAPPA[ncand],FI0[ncand],
							z[0]+zdrift[0],s[0],&nrounds0);
			  ddd = Dist_SZ(R[ncand],KAPPA[ncand],FI0[ncand],
							z[0]-zdrift[0],s[0],&nrounds0);
			  if( fabs(dista0)> fabs(ddd) ){
			   dista0=ddd;
			   zeta0 = z[0]-zdrift[0];
			  } else {
			   zeta0 = z[0]+zdrift[0];
			  }

			  dista1 = Dist_SZ(R[ncand],KAPPA[ncand],FI0[ncand],
							z[1]+zdrift[1],s[1],&nrounds1);
			  ddd = Dist_SZ(R[ncand],KAPPA[ncand],FI0[ncand],
							z[1]-zdrift[1],s[1],&nrounds1);
			  if( fabs(dista1)> fabs(ddd) ){
					   dista1=ddd;
					   zeta1 = z[1]-zdrift[1];
			  } else {
				   zeta1 = z[1]+zdrift[1];
				  }
				  if( fabs(dista1)<fabs(dista0)){
					SchosenSkew[ncand][k]=s[1];
					ZchosenSkew[ncand][k]=zeta1;
				  }else{
					SchosenSkew[ncand][k]=s[0];
					ZchosenSkew[ncand][k]=zeta0;
				  }
				  ListTrackCandHit[ncand][nbuoni]=
				  ListSttSkewHitsinTrack[ncand][nskewbuoni]=
					ListSttHitsinTrack[ncand][j];
				  ListTrackCandHitType[ncand][nbuoni]=
					ListSttHitsinTrackType[ncand][j];

				  nskewbuoni++;
				  nbuoni++;

			}else if( z[0]<999998.){

					  dista = Dist_SZ(R[ncand],KAPPA[ncand],FI0[ncand],
							z[0]+zdrift[0],s[0],&nrounds0);
					  ddd = Dist_SZ(R[ncand],KAPPA[ncand],FI0[ncand],
							z[0]-zdrift[0],s[0],&nrounds0);
					  SchosenSkew[ncand][k]=s[0];
					  if( fabs(dista)> fabs(ddd) ){
					   ZchosenSkew[ncand][k]=z[0]-zdrift[0];
					  } else {
					   ZchosenSkew[ncand][k]=z[0]+zdrift[0];
					  }
					  ListTrackCandHit[ncand][nbuoni]=
					  ListSttSkewHitsinTrack[ncand][nskewbuoni]=
						ListSttHitsinTrack[ncand][j];
					  ListTrackCandHitType[ncand][nbuoni]=
						ListSttHitsinTrackType[ncand][j];
					  nskewbuoni++;
					  nbuoni++;
			} else if( z[1]<999998.){

					  dista = Dist_SZ(R[ncand],KAPPA[ncand],FI0[ncand]
							,z[1]+zdrift[1],s[1],&nrounds0);
					  ddd = Dist_SZ(R[ncand],KAPPA[ncand],FI0[ncand],
							z[1]-zdrift[1],s[1],&nrounds1);
					  SchosenSkew[ncand][k]=s[1];

					  if( fabs(dista)> fabs(ddd) ){
					   ZchosenSkew[ncand][k]=z[1]-zdrift[1];
					  } else {
					   ZchosenSkew[ncand][k]=z[1]+zdrift[1];
					  }
					  ListTrackCandHit[ncand][nbuoni]=
					  ListSttSkewHitsinTrack[ncand][nskewbuoni]=
						ListSttHitsinTrack[ncand][j];
					  ListTrackCandHitType[ncand][nbuoni]=
						ListSttHitsinTrackType[ncand][j];
					  nskewbuoni++;
					  nbuoni++;

			} // end of  if( z[0]<999998. && z[1]<999998.)

		  } else { // continuation of  if(ListSttHitsinTrackType[ncand][j]==3)
				// so, here it must be a parallel hit.
					  ListTrackCandHit[ncand][nbuoni]=
					  ListSttParHitsinTrack[ncand][nparbuoni]=
						ListSttHitsinTrack[ncand][j];
					  ListTrackCandHitType[ncand][nbuoni]=
						ListSttHitsinTrackType[ncand][j];
					  nparbuoni++;
					  nbuoni++;
		  }  // end of  if(ListSttHitsinTrackType[ncand][j]==3)

		} // end of  for(j=0; j<nSttHitsinTrack[i]; j++)
		nSttHitsinTrack[ncand]=nbuoni;
		nSttParHitsinTrack[ncand]=nparbuoni;
		nSttSkewHitsinTrack[ncand]=nskewbuoni;
		nTrackCandHit[ncand]=nbuoni;
		nMvdPixelHitsinTrack[ncand]=0;
		nMvdStripHitsinTrack[ncand]=0;
		keepit[ncand]=true;
		Mvdhits[ncand]=false;
		// gli SciTil sono gia' caricati, non c'e' bisogno d'altro.
	}  // end of for(  ncand= 0; ncand< nSttTrackCand; ncand++)
	//  now skip and go directly to the upload of the MC comparison and
	//  the new (= to the old) PndTrackCand.
	nTotalCandidates=nSttTrackCand;
	goto skipping ;
 } // end of  if(nMvdPixelHit+nMvdStripHit)

//------------------ end treatment special pathological case when there are no Mvd hits at all.


//-------------------
//-------------------
//-------------------
//-------------------
//-------------------
//-------------------
//-------------------  start the combined Mvd-Stt  PR
//-------------------
//-------------------
//-------------------
//-------------------
//-------------------

 nTotalCandidates = nSttTrackCand;  // nSttTrackCand is already <= MAXTRACKSPEREVENT.

//----   find the angular range (in Fi) allowed for the STT hits, with the present Ox,Oy and R
//	of the track candidates, and for the Mvd hits (FI0 and Fi_low_limit ).

 for(  ncand= 0; ncand< nSttTrackCand; ncand++){

	keepit[ncand]=true;
	PndSttFindingParallelTrackAngularRange(
		Ox[ncand],
		Oy[ncand],
		R[ncand],
		CHARGE[ncand],
		&Fi_low_limit[ncand],	// Fi (in XY Helix frame) lower limit using
					// the Stt detector minimum/maximum radius
					// Fi_low_limit is ALWAYS between 0. and 2PI
		&Fi_up_limit[ncand],	// Fi (in XY Helix frame) upper limit using
					// the Stt detector maximum/minimum radius
					// Fi_up_limit is ALWAYS > Fi_low_limit and
					// possibly > 2PI.
		&statusflag[ncand],//it is a vector; =0, all well; =1, track contained completely between RMin
			   // and RMax; = -1 track contained within RMin; =-2 track outside RMax.
		RStrawDetectorMin,
		RStrawDetectorMax
		);

	if( statusflag[ncand] == -1) { Fi_low_limit[ncand] = -99999.; }
	else if (statusflag[ncand] == -2){ keepit[ncand] = false; };

 }	// end of for(  ncand= 0; ncand< nSttTrackCand; ncand++)




//---------------------   here call to the function that matches Mvd hits with Stt tracks
   delta=0.5; //  parameter of proximity for associating Mvd hits to Stt tracks
   highqualitycut=0.2; //  parameter of proximity for associating Mvd hits to Stt tracks


   //  This method matches the Mvd hits to the found tracks.

   MatchMvdHitsToSttTracks2(
			keepit,// input and output.
			delta,
			highqualitycut,
			nSttTrackCand,
			Ox,
			Oy,
			R,
			FI0,
			Fi_low_limit,
			CHARGE,
			nMvdPixelHitsinTrack, // output
			ListMvdPixelHitsinTrack, // output
			nMvdStripHitsinTrack, // output
			ListMvdStripHitsinTrack // output
			);

//---------inizio stampe.
if(istampa>=2&& IVOLTE<20){
           cout<<"da PndSttMvdTracking, dopo MatchMvd..2 :\n";
	stampetta(nSttTrackCand,keepit);
}   //end of if(istampa>=0)
//---------------------------------- fine stampe.

//-----------------  end of section with match Mvd hits with Stt hits


	for(  ncand= 0; ncand< nSttTrackCand; ncand++){

		if(nMvdPixelHitsinTrack[ncand]+nmaxMvdStripHitsInTrack==0){

		   Mvdhits[ncand]=false;
		   ALFA[ncand]=-2.*Ox[ncand];
		   BETA[ncand]=-2.*Oy[ncand];
		   GAMMA[ncand]= Ox[ncand]*Ox[ncand]+Oy[ncand]*Oy[ncand]-R[ncand]*R[ncand];



		   // just copy and go to the next track cand.
		   int nbuoni, nparbuoni, nskewbuoni;
		   for(j=0, nbuoni=0,nparbuoni=0, nskewbuoni=0;j<nSttHitsinTrack[ncand]; j++){
			if(istampa>=2) cout<<"da PndSttMvdTracking, IVOLTE = "<<
					IVOLTE<<", ncand = "
					<<ncand<<
					" non rifittato perche' non ha Mvd hits associati;"
					<<endl;

			  if(ListSttHitsinTrackType[ncand][j]==3){ // skew stt.

				k=ListSttHitsinTrack[ncand][j];
				CalculateSandZ(
					Ox[ncand],
					Oy[ncand],
					R[ncand],
					k,
					info,
					WDX,
					WDY,
					WDZ,
					s,
					z,
					zdrift,
					zerror
					);
				if( z[0]<999998. && z[1]<999998.){

					  dista0 = Dist_SZ(R[ncand],KAPPA[ncand],FI0[ncand],
							z[0]+zdrift[0],s[0],&nrounds0);
					  ddd = Dist_SZ(R[ncand],KAPPA[ncand],FI0[ncand],
							z[0]-zdrift[0],s[0],&nrounds0);
					  if( fabs(dista0)> fabs(ddd) ){
					   dista0=ddd;
					   zeta0 = z[0]-zdrift[0];
					  } else {
					   zeta0 = z[0]+zdrift[0];
					  }

					  dista1 = Dist_SZ(R[ncand],KAPPA[ncand],FI0[ncand],
							z[1]+zdrift[1],s[1],&nrounds1);
					  ddd = Dist_SZ(R[ncand],KAPPA[ncand],FI0[ncand],
							z[1]-zdrift[1],s[1],&nrounds1);
					  if( fabs(dista1)> fabs(ddd) ){
					   dista1=ddd;
					   zeta1 = z[1]-zdrift[1];
					  } else {
					   zeta1 = z[1]+zdrift[1];
					  }
					  if( fabs(dista1)<fabs(dista0)){
						SchosenSkew[ncand][k]=s[1];
						ZchosenSkew[ncand][k]=zeta1;
					  }else{
						SchosenSkew[ncand][k]=s[0];
						ZchosenSkew[ncand][k]=zeta0;
					  }
					  ListTrackCandHit[ncand][nbuoni]=
					  ListSttSkewHitsinTrack[ncand][nskewbuoni]=
						ListSttHitsinTrack[ncand][j];
					  ListTrackCandHitType[ncand][nbuoni]=
						ListSttHitsinTrackType[ncand][j];
					  
					  nskewbuoni++;
					  nbuoni++;

				}else if( z[0]<999998.){

					  dista = Dist_SZ(R[ncand],KAPPA[ncand],FI0[ncand],
							z[0]+zdrift[0],s[0],&nrounds0);
					  ddd = Dist_SZ(R[ncand],KAPPA[ncand],FI0[ncand],
							z[0]-zdrift[0],s[0],&nrounds0);
					  SchosenSkew[ncand][k]=s[0];
					  if( fabs(dista)> fabs(ddd) ){
					   ZchosenSkew[ncand][k]=z[0]-zdrift[0];
					  } else {
					   ZchosenSkew[ncand][k]=z[0]+zdrift[0];
					  }
					  ListTrackCandHit[ncand][nbuoni]=
					  ListSttSkewHitsinTrack[ncand][nskewbuoni]=
						ListSttHitsinTrack[ncand][j];
					  ListTrackCandHitType[ncand][nbuoni]=
						ListSttHitsinTrackType[ncand][j];
					  nskewbuoni++;
					  nbuoni++;
				} else if( z[1]<999998.){

					  dista = Dist_SZ(R[ncand],KAPPA[ncand],FI0[ncand]
							,z[1]+zdrift[1],s[1],&nrounds0);
					  ddd = Dist_SZ(R[ncand],KAPPA[ncand],FI0[ncand],
							z[1]-zdrift[1],s[1],&nrounds1);
					  SchosenSkew[ncand][k]=s[1];

					  if( fabs(dista)> fabs(ddd) ){
					   ZchosenSkew[ncand][k]=z[1]-zdrift[1];
					  } else {
					   ZchosenSkew[ncand][k]=z[1]+zdrift[1];
					  }
					  ListTrackCandHit[ncand][nbuoni]=
					  ListSttSkewHitsinTrack[ncand][nskewbuoni]=
						ListSttHitsinTrack[ncand][j];
					  ListTrackCandHitType[ncand][nbuoni]=
						ListSttHitsinTrackType[ncand][j];
					  nskewbuoni++;
					  nbuoni++;

				} // end of  if( z[0]<999998. && z[1]<999998.)

			  } else { // continuation of  if(ListSttHitsinTrackType[ncand][j]==3)
				// so, here it must be a parallel hit.
					  ListTrackCandHit[ncand][nbuoni]=
					  ListSttParHitsinTrack[ncand][nparbuoni]=
						ListSttHitsinTrack[ncand][j];
					  ListTrackCandHitType[ncand][nbuoni]=
						ListSttHitsinTrackType[ncand][j];
					  nparbuoni++;
					  nbuoni++;
			  }  // end of  if(ListSttHitsinTrackType[ncand][j]==3)

			} // end of  for(j=0; j<nSttHitsinTrack[i]; j++)
			nSttHitsinTrack[ncand]=nbuoni;
			nSttParHitsinTrack[ncand]=nparbuoni;
			nSttSkewHitsinTrack[ncand]=nskewbuoni;
			nTrackCandHit[ncand]=nbuoni;
			nMvdPixelHitsinTrack[ncand]=0;
			nMvdStripHitsinTrack[ncand]=0;

		   continue;
		} // end of if(nMvdPixelHitsinTrack[ncand]+....


		if( nMvdPixelHitsinTrack[ncand] > nmaxMvdPixelHitsInTrack){
			cout<<"from PndSttMvdTracking, nMvdPixelHitsinTrack["<<ncand
			<<"] is "<<nMvdPixelHitsinTrack[ncand]
			<<"and it is > nmaxMvdPixelHitsInTrack ("
			<<nmaxMvdPixelHitsInTrack
			<<"); setting nMvdPixelHitsinTrack[ncand] to "<<
			nmaxMvdPixelHitsInTrack<<endl;
			nMvdPixelHitsinTrack[ncand]=nmaxMvdPixelHitsInTrack;
		}

		if( nMvdStripHitsinTrack[ncand] > nmaxMvdStripHitsInTrack){
			cout<<"from PndSttMvdTracking, nMvdStripHitsinTrack["<<ncand
			<<"] is "<<nMvdStripHitsinTrack[ncand]
			<<"and it is > nmaxMvdStripHitsInTrack ("
			<<nmaxMvdStripHitsInTrack
			<<"); setting nMvdStripHitsinTrack[ncand] to "<<
			nmaxMvdStripHitsInTrack<<endl;
			nMvdStripHitsinTrack[ncand]=nmaxMvdStripHitsInTrack;
		}


		Mvdhits[ncand] = true;



//----------------------------------------------------------
//     ordering all the hits belonging to the candidate track, by increasing R;
//     forming the new track with Mvd+Stt hits

	// arrays used to store temporarily the info of Mvd hits to be ordered.

		nTrackCandHit[ncand] =nSttHitsinTrack[ncand]+
					nMvdPixelHitsinTrack[ncand]+
					nMvdStripHitsinTrack[ncand];
		// adding the Mvd hits (Pixel and Strips)
			for(i=0; i< nMvdPixelHitsinTrack[ncand]; i++){
			  XY[i][0] = XMvdPixel[ ListMvdPixelHitsinTrack[ncand][i] ];
			  XY[i][1] = YMvdPixel[ ListMvdPixelHitsinTrack[ncand][i] ];
			  ListHits[i] = ListMvdPixelHitsinTrack[ncand][i];
			}
			for(i=0; i< nMvdStripHitsinTrack[ncand]; i++){
			  XY[i+nMvdPixelHitsinTrack[ncand]][0] =
			    XMvdStrip[ ListMvdStripHitsinTrack[ncand][i] ];
			  XY[i+nMvdPixelHitsinTrack[ncand]][1] =
			    YMvdStrip[ ListMvdStripHitsinTrack[ncand][i] ];
			  // to distinguish between Pixels and Strips, add a number
			  // to the original Strip hit number.
			  ListHits[i+nMvdPixelHitsinTrack[ncand]] =
				ListMvdStripHitsinTrack[ncand][i]+
				(nmaxMvdPixelHits+nmaxMvdStripHits)*10 ;
			}

			//  ordering the Mvd Hits
			OrderingUsingConformal(
			   Ox[ncand],
			   Oy[ncand],
			   &Trajectory_Start[ncand][0],
			   nMvdPixelHitsinTrack[ncand]+nMvdStripHitsinTrack[ncand],
			   XY, // XY[*][0] = X position, XY[*][0] = Y position.
			   CHARGE[ncand],  // input
			   ListHits  // output
						);
			//  constructing the ordered new Track  Candidate now and loading
			//  the (now ordered) X and Y lists for the hits in this track.
			for(i=0; i< nMvdPixelHitsinTrack[ncand]+
					nMvdStripHitsinTrack[ncand]; i++){
				if(ListHits[i]<(nmaxMvdPixelHits+nmaxMvdStripHits)*10){//Pixel.
					ListTrackCandHit[ncand][i] = ListHits[i];
					ListTrackCandHitType[ncand][i] = 0;
				} else { // Strip hits.
					ListTrackCandHit[ncand][i] = ListHits[i]-
						(nmaxMvdPixelHits+nmaxMvdStripHits)*10;
					ListTrackCandHitType[ncand][i] = 1;
				}
			}  // end of  for(i=0; i< nMvdPixelHitsinTrack[ncand]+


		for(i=0; i<nSttHitsinTrack[ncand]; i++){
			ListTrackCandHit[ncand][i+
				nMvdPixelHitsinTrack[ncand]+
				nMvdStripHitsinTrack[ncand]
					] = ListSttHitsinTrack[ncand][i];
			ListTrackCandHitType[ncand][i+
				nMvdPixelHitsinTrack[ncand]+
				nMvdStripHitsinTrack[ncand]
					] = ListSttHitsinTrackType[ncand][i];
		}



	} //   end of  for(ncand=0; ncand< nTotalCandidates; ncand++)


//-------------------- end of ordering

//---------inizio stampe.
if(istampa>=2&& IVOLTE<20){
           cout<<"da PndSttMvdTracking, dopo riordinamento :\n";
	stampetta(nSttTrackCand,keepit);


}   //end of if(istampa>=0)
//---------------------------------- fine stampe.



//---------------------   refit the Helix in XY plane using Stt + Mvd associated hits


	Short_t	iexcl;
	Double_t
		d,
		diff,
		rotationangle,
		trajectory_vertex[2];

 for(ncand=0; ncand< nTotalCandidates; ncand++){
	if(!keepit[ncand]) continue;

	if( !Mvdhits[ncand]) continue;


	rotationangle= atan2( Py[ncand], Px[ncand]);	// Py/Px = m in
								//  v = m*u + q formula
	if( rotationangle<0. ) rotationangle += 2.*PI;

//---------- translation of the reference system in the best Mvd hit position

	for(i=0, diff = 1.e20;i<nTrackCandHit[ncand];i++){
// ListTrackCandHitType[ncand][i]=0 --> Pixel; = 1 --> Strip; = 2 Stt parallel; 3 = Stt Skew.
// There are no -1 type hits at this point.
		if(!(ListTrackCandHitType[ncand][i]==1
				||
				ListTrackCandHitType[ncand][i]==0) ) continue;
		if( ListTrackCandHitType[ncand][i] == 0 ){
				d = fabs( sqrt(
				(Ox[ncand]-XMvdPixel[ListTrackCandHit[ncand][i]])
				*(Ox[ncand]-XMvdPixel[ListTrackCandHit[ncand][i]])
				+(Oy[ncand]-YMvdPixel[ListTrackCandHit[ncand][i]])
				*(Oy[ncand]-YMvdPixel[ListTrackCandHit[ncand][i]])
					) - R[ncand]);
			if( d  <  diff ){
					diff = d;
					iexcl=i;
					trajectory_vertex[0]=
						XMvdPixel[ListTrackCandHit[ncand][i]];
					trajectory_vertex[1]=
						YMvdPixel[ListTrackCandHit[ncand][i]];
			}
		} else {
			d = fabs( sqrt(
				(Ox[ncand]-XMvdStrip[ListTrackCandHit[ncand][i]])
			       *(Ox[ncand]-XMvdStrip[ListTrackCandHit[ncand][i]])
			       +(Oy[ncand]-YMvdStrip[ListTrackCandHit[ncand][i]])
			       *(Oy[ncand]-YMvdStrip[ListTrackCandHit[ncand][i]])
					) - R[ncand]);
			if( d  <  diff ){
				diff = d;
				iexcl=i;
				trajectory_vertex[0]=
						XMvdStrip[ListTrackCandHit[ncand][i]];
				trajectory_vertex[1]=
						YMvdStrip[ListTrackCandHit[ncand][i]];
			}
		}

	} // end of  for(i=0, diff = 1.e20;i<.....

	if( diff > 0.5 ) {
		trajectory_vertex[0]=trajectory_vertex[1]=0.;
		iexcl=-1;
	}

	RefitMvdStt(
			nTrackCandHit[ncand], //this is  input
			&ListTrackCandHit[ncand][0],//this is both input and output
			&ListTrackCandHitType[ncand][0],//this is both input and output
			info,
			rotationangle,
			trajectory_vertex,
			iexcl,

			&ALFA[ncand], // output of the fit
			&BETA[ncand], // output of the fit
			&GAMMA[ncand],// output of the fit
			&status[ncand]    // fit status; true = successful
			);

	if(status[ncand]){
			Ox[ncand] = -ALFA[ncand]/2.;
			Oy[ncand] = -BETA[ncand]/2.;
			R[ncand] = Ox[ncand]*Ox[ncand]+Oy[ncand]*Oy[ncand]-GAMMA[ncand];
			if( R[ncand] > 0. ) R[ncand]=sqrt(R[ncand]) ;
			else  R[ncand]=0.;
			FI0[ncand] = atan2(Trajectory_Start[1][ncand]-Oy[ncand], Trajectory_Start[0][ncand]-Ox[ncand]);
			if( FI0[ncand] < 0. )  FI0[ncand]+= 2.*PI;
	}

	if(R[ncand]<RStrawDetectorMin/2.) {
		keepit[ncand]=false;
		continue;
	}

//--------------

//---------inizio stampe.
if(istampa>=2&& IVOLTE<20){
	cout<<"da PndSttMvdTracking, dopo XY Rfit :\n";
	stampetta(nSttTrackCand,keepit);
}   //end of if(istampa>=0)
//---------------------------------- fine stampe.





//  find out if the SciTil hit associated with this track is still acceptable after the last
//  XY refit.

	for(i=0, iaccept=0;i<nSciTilHitsinTrack[ncand];i++){

		intersect=IntersectionSciTil_Circle(
			posizSciTil[ListSciTilHitsinTrack[ncand][0]][0],
			posizSciTil[ListSciTilHitsinTrack[ncand][0]][1],
			Ox[ncand], // center of circle.
			Oy[ncand],
			R[ncand], // Radius of circle.
			&Nint,
			XintersectionList,
			YintersectionList
						);

		// reject case with no intersection of the SciTil with the circle trajectory.
		if(intersect){
		// calculate S on the lateral face of the Helix.
			if ( Nint==2){// (it should be a very rare case).
				// do an average of the two positions; put it in XintersectionList[0]
				// and in  YintersectionList[0] so that it is transparent to
				// the subsequent code.
				YintersectionList[0]=
					(YintersectionList[0]+YintersectionList[1])/2.;
				XintersectionList[0]=
					(XintersectionList[0]+XintersectionList[1])/2.;
			} // end of  if ( Nint==2)
			ListSciTilHitsinTrack[ncand][iaccept]=
					ListSciTilHitsinTrack[ncand][i];
			SciTilHitsXwithTrack[ncand][iaccept]=XintersectionList[0];
			SciTilHitsYwithTrack[ncand][iaccept]=YintersectionList[0];
			iaccept++;
		}   //  end of  if(intersect)

	} // end of  for(i=0;i<nSciTilHitsinTrack[ncand];i++)

	nSciTilHitsinTrack[ncand]=iaccept;


// finding again the Charge of the track (the last fit may have changed the concavity of the track).

// first load again the (now ordered) X and Y position of the hits.


	for(i=0;i<nMvdPixelHitsinTrack[ncand]+nMvdStripHitsinTrack[ncand];i++){
		// these are not ordered, but for charge finding ordering is not
		// necessary.
		X[i]=XY[i][0];
		Y[i]=XY[i][1];
	}

	j = nMvdPixelHitsinTrack[ncand]+nMvdStripHitsinTrack[ncand];
	for(i=0;i<nSttParHitsinTrack[ncand];i++){
		X[j]=info[ ListSttParHitsinTrack[ncand][i] ][0];
		Y[j]=info[ ListSttParHitsinTrack[ncand][i] ][1];
		j++;
	}
	for(i=0;i<nSciTilHitsinTrack[ncand];i++){
		X[j]=posizSciTil[ ListSciTilHitsinTrack[ncand][i] ][0];
		Y[j]=posizSciTil[ ListSciTilHitsinTrack[ncand][i] ][1];
	}

	FindCharge(
		Ox[ncand],
		Oy[ncand],
		nMvdPixelHitsinTrack[ncand]+nMvdStripHitsinTrack[ncand]
		   +nSttParHitsinTrack[ncand]+nSciTilHitsinTrack[ncand],
		X,
		Y,
		&CHARGE[ncand]
		);

//------------------------------


	//   finding the FI angular range (in the laboratory frame) spanned by this parallel track
	//   taking into account the Rmax and Rmin of the straw detector. The track NOT NECESSARILY
	//   has to originate at (0,0).

	PndSttFindingParallelTrackAngularRange(
		Ox[ncand],
		Oy[ncand],
		R[ncand],
		CHARGE[ncand],  // this is an input.
		&Fi_low_limit[ncand],	// Fi (in XY Helix frame) lower limit using
					// the Stt detector minimum/maximum radius
					// Fi_low_limit is ALWAYS between 0. and 2PI
		&Fi_up_limit[ncand],	// Fi (in XY Helix frame) upper limit using
					// the Stt detector maximum/minimum radius
					// Fi_up_limit is ALWAYS > Fi_low_limit and
					// possibly > 2PI.
					// Fi_low_limit and Fi_up_limit are undefined
					// when statusflag is negative.
		&statusflag[ncand],//it is a vector; =0, all well; =1, track contained completely between RMin
			// and RMax; = -1 track contained within RMin; =-2 track outside RMax.
		RStrawDetectorMin,
		RStrawDetectorMax
						);
	// Fi_low_limit set  to  -99999. when it is contained in Mvd
	// region completely.
	if( statusflag[ncand] == -1 ) Fi_low_limit[ncand] = -99999.;
	if( statusflag[ncand] == -2) keepit[ncand] = false;


	}	// end of for(ncand=0; ncand< nTotalCandidates; ncand++)

//--------------------- end of  refit the Helix in XY plane using Stt + Mvd associated hits

//---------inizio stampe.
if(istampa>=2&& IVOLTE<20){
           cout<<"\n\nda PndSttMvdTracking, prima di MatchMvdHitsToSttTracksagain :\n";
	stampetta(nSttTrackCand,keepit);
}   //end of if(istampa>=0)
//---------------------------------- fine stampe.


//---------------------   here call to the function that matches Mvd hits with the new
//  circular trajectory in XY found  for the second time, after first refit

   delta=0.5; //  parameter of proximity for associating Mvd hits to Stt tracks
//   highqualitycut=0.3; //  parameter of proximity for associating Mvd hits to Stt tracks
   highqualitycut=0.5; //  parameter of proximity for associating Mvd hits to Stt tracks


   MatchMvdHitsToSttTracksagain(
			keepit,
			Mvdhits,
			delta,
			highqualitycut,
			nSttTrackCand,
			Ox,
			Oy,
			R,
			FI0,
			Fi_low_limit, // because here we deal with hits in Mvd region
			CHARGE,
			nMvdPixelHitsinTrack, // input and output
			ListMvdPixelHitsinTrack, // input and output
			nMvdStripHitsinTrack, // input and output
			ListMvdStripHitsinTrack // input and output
			);


 if(istampa>=2){
	cout<<"\n\nda PndSttMvdTracking, dopo MatchMvdHitsToSttTracksagain :"<<endl;
		stampetta(nSttTrackCand,keepit);
 }

//---------------------   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// use the risult just obtained from the fit in XY to redo the association of the Skew Straw hits

  for(ncand=0; ncand< nTotalCandidates; ncand++)
  {
	if(!keepit[ncand]) continue;
	if( ! Mvdhits[ncand]) continue;

	if( statusflag[ncand] == -1 ) continue; // this is when the XY circle is contained in the
						// the Mvd region completely; skip the association of
						// the Skews.

	nSttSkewHitsinTrack[ncand]= AssociateSkewHitsToXYTrack(
		InclusionListStt, // hit is excluded only if it multiple hit.
		nSttSkewHit,
		ListAllSkewHits,
		Ox[ncand],   //  input : X of center of XY plane circle
		Oy[ncand],   //  input : Y of center of XY plane circle
		R[ncand],   //  input : Radius of XY plane circle
		info,
		WDX,
		WDY,
		WDZ,
		Fi_low_limit[ncand],	// Fi (in Helix XY frame) lower limit using the Stt detector minimum/maximum radius
		Fi_up_limit[ncand],	// Fi (in Helix XY frame) upper limit using the Stt detector maximum/minimum radius
		CHARGE[ncand],
		TemporarySkewList, // output,  list of selected skew hits (in skew numbering)
		TemporaryS,       //  output,  S coordinate of selected Skew hit
		TemporaryZ,       //  output,  Z coordinate of selected Skew hit (center wire)
		TemporaryZDrift,   //  output,  drift distance IN Z DIRECTION only, of selected Skew hit
		TemporaryZErrorafterTilt   //  output,  Radius taking into account the tilt, IN Z DIRECTION only, of selected Skew hit
			);
	for(j=0;j<nSttSkewHitsinTrack[ncand];j++)
	{
		ListSttSkewHitsinTrack[ncand][j]=TemporarySkewList[j][0];
		ListSttSkewHitsinTrackSolution[ncand][j]=TemporarySkewList[j][1];
	}

 if(istampa>=2){
	cout<<"\n\nda PndSttMvdTracking, dopo AssociateSkewHitsToXYTrack :"<<endl;
		stampetta(nSttTrackCand,keepit);
 }

//-------------------------------------------  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



//---------------------   here calculate the S and Z values of Mvd Pixels, Mvd Strips,
//	 Stt Skew hits and SciTil hits (if present).

//  the difference between S and Sbis, ZED and ZEDbis, DriftRadius and DriftRadiusbis,
//  ErrorDriftRadius and ErrorDriftRadiusbis, is that S, ZED etc. contain the list of
//  Pixel+Strips+SciTil + other Skew Stt hits in case Pixel+Strips+SciTil are <= 2; instead
//  Sbis, ZEDbis etc. contain Pixel+Strips+all Skew Stt hits.

	//  nXYZhits = n. of Mvd hits + SciTil hits.
	nXYZhits = nMvdPixelHitsinTrack[ncand]+nMvdStripHitsinTrack[ncand]+nSciTilHitsinTrack[ncand];
	// calculate if there is the need of using some skew hits in the subsequent SZ fit;
	// put in  nhitsinfit  the number of hits used in the subsequent  SZ  fit.
	if( nXYZhits <=2){
		nSttSkewHitsinTrack[ncand]<5 ?
			nhitsinfit = nXYZhits + nSttSkewHitsinTrack[ncand] :
			nhitsinfit = nXYZhits+5 ; // 1 0 2 XYZ hit + 5 Skew hits.
	} else {
		nhitsinfit= nXYZhits;
	}

	Double_t
	DriftRadius[nhitsinfit],
	ErrorDriftRadius[nhitsinfit],
	S[nhitsinfit],
	ZED[nhitsinfit];


	// the Mvd Pixels hit
	for(i=0; i< nMvdPixelHitsinTrack[ncand]; i++){
		k=ListMvdPixelHitsinTrack[ncand][i];
		ZEDbis[i] = ZED[i] = ZMvdPixel[k];
		S[i] = atan2( YMvdPixel[k]-Oy[ncand],XMvdPixel[k]-Ox[ncand]);
		if(S[i]<0.) S[i] +=2.*PI;
		Sbis[i] = S[i];
		// DriftRadius is set conventionally at -1, for later use in the SZ fit;
		// the error on the point used in the fit is ErrorDriftRadius and this
		// is overestimated to be  1cm.
		DriftRadiusbis[i]=DriftRadius[i]=-1.;
		ErrorDriftRadiusbis[i]=ErrorDriftRadius[i]= 1. ;
	}
	// the Mvd Strips hit
	for(j=0, i = nMvdPixelHitsinTrack[ncand]; j< nMvdStripHitsinTrack[ncand]; j++){
		k=ListMvdStripHitsinTrack[ncand][j];
		ZEDbis[i] = ZED[i] = ZMvdStrip[k];
		S[i] = atan2( YMvdStrip[k]-Oy[ncand],XMvdStrip[k]-Ox[ncand]);
		if(S[i]<0.) S[i] +=2.*PI;
		Sbis[i] = S[i] ;
		// DriftRadius is set conventionally at -1, for later use in the SZ fit;
		// the error on the point used in the fit is ErrorDriftRadius and this
		// is overestimated to be  1cm.
		DriftRadiusbis[i]=DriftRadius[i]=-1.;
		ErrorDriftRadiusbis[i]=ErrorDriftRadius[i]= 1. ;
		i ++;
	}


	// the SciTil hits ( can be only up to 2).
	for(j=0, i = nMvdPixelHitsinTrack[ncand]+nMvdStripHitsinTrack[ncand];
			j<nSciTilHitsinTrack[ncand];j++){
		// calculate S on the lateral face of the Helix.
		S[i] = atan2(YintersectionList[0]-Oy[ncand],XintersectionList[0]-Ox[ncand]);

		if ( S[i]<0.) S[i] += 2.*PI;

		ZED[i]=posizSciTil[ListSciTilHitsinTrack[ncand][j]][2];
		// DriftRadius is set conventionally at -2, for later use in the SZ fit;
		// the error on the point used in the fit is ErrorDriftRadius and this
		// is overestimated to be DIMENSIONSCITIL/2.
		DriftRadius[i]=-2.;
		ErrorDriftRadius[i]= DIMENSIONSCITIL/2.; ;

	} // end of  for(j=0, i = nMvdPi.....



	// the Skew Stt hits
	for(j=0;j<nSttSkewHitsinTrack[ncand]; j++){

		k=ListSttSkewHitsinTrack[ncand][j];
		kall = nMvdPixelHitsinTrack[ncand]+
			nMvdStripHitsinTrack[ncand]+j;
		i = nMvdPixelHitsinTrack[ncand]+nMvdStripHitsinTrack[ncand]+
			nSciTilHitsinTrack[ncand] +j ;


		// calculate the quantities used for the SZ fit only.
		if(i<nhitsinfit){
			S[i] = TemporaryS[j];
			ZED[i]=TemporaryZ[j];
			ErrorDriftRadius[i]=2.*TemporaryZDrift[j];
		}
		//-----------
		ZEDbis[kall]=TemporaryZ[j];
		Sbis[kall]= TemporaryS[j];
		DriftRadiusbis[kall]=DriftRadius[i]=TemporaryZDrift[j];
		// overestimate the error on the Drift Radius used in the SZ  fit.
		if( fabs(TemporaryZDrift[j]) >1.e-10) {
		   ErrorDriftRadiusbis[kall]=TemporaryZErrorafterTilt[j];
		} else {
		   ErrorDriftRadiusbis[kall]=0.5;
		}


	}	//   end of  for(j=0;j<nSttSkewHitsinTrack[ncand]; j++)


// ---------------  fit in SZ  with Mvd + SciTil only



//  finding if there are discontinuity at 0 for fi value of the Mvd Hit.
//  In case of discontinuity at 0, add 2*PI to fi of those hits with fi in the 1st quadrant.
//  This is necessary because the discontinuities would make the fit
//  in the SZ plane fail.
//  In this discontinuity fixing, the value FI0 of the vertex (0,0) is also included.
//  If there is discontinuity fixing, the values of S[i] might be modified (+2.*PI).
		FixDiscontinuitiesFiangleinSZplane(
			nhitsinfit,
			S,	// S can be modified by +-2*PI if necessary.
			&FI0[ncand],	// this remains unchanged.
			CHARGE[ncand]	// this remains unchanged.
					);

//---------------------   here do the fit again in the SZ space if there are Mvd hits.
//			  For this, reordering of the  Mvd hits is not necessary.

 if(istampa>=2){
	cout<<"\n\nda PndSttMvdTracking, prima di FitSZspace :"<<endl;
		stampetta(nSttTrackCand,keepit);
 }
		resultFitSZagain[ncand] = FitSZspace(
					nhitsinfit,	// n. hits to be fitted
					S,
					ZED,
					DriftRadius,
					ErrorDriftRadius,
					FI0[ncand],
					20,	// maximum number allowed in the fit
						// deve essere meno di 30+30+60
					&emme
						);

		if( resultFitSZagain[ncand]==1){
			KAPPA[ncand] = emme;
			// in case it was a TrackCand from Stt alone, originally with no Skew
			// hit and therefore without KAPPA information, signal that now the
			// SZ fit worked (thanks to the Mvd hits) by setting SttSZfit[ncand]=true.
			if( ncand<= nSttTrackCand ) SttSZfit[ncand]=true;
		} else {
			keepit[ncand]=false;
		}


 if(istampa>=2){
	cout<<"\n\nda PndSttMvdTracking, dopo FitSZspace :"<<endl;
		stampetta(nSttTrackCand,keepit);
 }


//-------------------------------------------

//	use the risult just obtained from the fit in SZ to reject the spurious Skew Straw hits
//	and the Mvd spurious hits; also in this function there is the calculation of the Z position
//	of the SKEW hits and the MVD hits, for a given track candidate (ie for a given Helix
//	circle in the XY plane)

	UShort_t MaxTurns;
	Double_t Turns;


	  if(keepit[ncand]){
	    if(R[ncand] < RStrawDetectorMax/2.){
		if(-CHARGE[ncand]*KAPPA[ncand]>0.){	// this means Pz>0.
		  Turns= 0.5*fabs((ZCENTER_STRAIGHT+SEMILENGTH_STRAIGHT)
					*KAPPA[ncand])/PI;
		  if( fabs(Turns)<10.)  MaxTurns=(UShort_t) Turns ;
		  else  MaxTurns=10;
		} else {
		  Turns= 0.5*fabs((ZCENTER_STRAIGHT-SEMILENGTH_STRAIGHT)
					*KAPPA[ncand])/PI;
		  if( fabs(Turns)<10.)  MaxTurns=(UShort_t) Turns ;
		  else  MaxTurns=10;
		}
	    } else {
		MaxTurns=0;
	    }


	    EliminateSpuriousSZ(
				MaxTurns,
				&nMvdPixelHitsinTrack[ncand],	// input and output
				&ListMvdPixelHitsinTrack[ncand][0],// input and output
				&nMvdStripHitsinTrack[ncand],	// input and output
				&ListMvdStripHitsinTrack[ncand][0],// input and output
				&nSttSkewHitsinTrack[ncand],	// input and output
				&ListSttSkewHitsinTrack[ncand][0],// input and output
				Sbis,	// input, position of the central wire on the Helix cylinder;
				ZEDbis,	// input, position of the central wire on the Helix cylinder.
				DriftRadiusbis,	// input
				ErrorDriftRadiusbis,	// input
				&SchosenPixel[ncand][0], // this value from now on
				&SchosenStrip[ncand][0], // can also be > 2PI or < 2PI when
				&SchosenSkew[ncand][0],  // the particle makes more than 1 turn.
				&ZchosenPixel[ncand][0],
				&ZchosenStrip[ncand][0],
				&ZchosenSkew[ncand][0],
				ErrorchosenPixel,
				ErrorchosenStrip,
				ErrorchosenSkew,
				KAPPA[ncand],
				FI0[ncand],
				R[ncand]
				    );



	  }  // end of  if(keepit[ncand])

//------------------------

 if(istampa>=2){
	cout<<"\n\nda PndSttMvdTracking, dopo EliminatespuriousSZ :"<<endl;
		stampetta(nSttTrackCand,keepit);
 }


//	First cleanup based on the absence of Mvd hits


	// reject the candidate if it is NOT contained in the pipe and
	// therefore it should have at least 1 Mvd hit but it has none.
	if( (!IsInTargetPipe(	Ox[ncand],
				Oy[ncand],
				R[ncand],
				FI0[ncand],
				KAPPA[ncand],
				CHARGE[ncand],
				VERTICALGAP/2.) )
				 &&
		nMvdStripHitsinTrack[ncand]+nMvdPixelHitsinTrack[ncand]==0)
		{
			keepit[ncand]=false;
		}
 if(istampa>=2){
	cout<<"\n\nda PndSttMvdTracking, dopo isintargetpipe (Ox="<<Ox[ncand]
	<<", Oy="<<Oy[ncand]<<", R= "<<R[ncand]<<" :"<<endl;
		stampetta(nSttTrackCand,keepit);

 }


    }	//  end of for(ncand=0; ncand< nTotalCandidates; ncand++)




//---  redo association of parallel Stt  straw  hits to this track, after better refit.

//---------inizio stampe.
if(istampa>=2&& IVOLTE<20){
           cout<<"da PndSttMvdTracking, prima di CollectParSttHitsagain :\n";
		stampetta(nSttTrackCand,keepit);

}   //end of if(istampa>=0)
//---------------------------------- fine stampe.

	CollectParSttHitsagain(
			keepit,
			Mvdhits,
			info,
			nSttParHit,
			ListAllParHits,
			0,	// starting candidate n. (included).
			nTotalCandidates, // ending candidate n. (excluded).
			Ox,
			Oy,
			R,
			KAPPA,
			FI0,
			Fi_low_limit,
			Fi_up_limit,
			nSttParHitsinTrack, // input and output
			ListSttParHitsinTrack // input and output
			);

//---------inizio stampe.
if(istampa>=2&& IVOLTE<20){
           cout<<"da PndSttMvdTracking, dopo CollectParSttHitsagain :\n";
		stampetta(nSttTrackCand,keepit);

}   //end of if(istampa>=0)
//---------------------------------- fine stampe.

//	ordering all the hits belonging to the candidate track, by increasing R (large
//	trajectories)  or Conformal variables (better for small trajectories);
//	from candidate n. 0 to candidate n. nTotalCandidates-1; loading ListTrackCandHit.
//	the array ordered are :
//	ListTrackCandHit, ListTrackCandHitType, ListSttParHitsinTrack, ListSttSkewHitsinTrack
//	and also at the end the SciTil hit (if present) is added.
	Ordering_Loading_ListTrackCandHit(
		keepit,
		0,
		nTotalCandidates,
		info,
		Ox,
		Oy,
		R,
		Trajectory_Start,
		CHARGE,
		SchosenSkew
		);

	// adding at the end the SciTil hits (if present).

	for(ncand=0; ncand< nTotalCandidates; ncand++){
		i=nMvdPixelHitsinTrack[ncand]+nMvdStripHitsinTrack[ncand]+
			nSttParHitsinTrack[ncand]+nSttSkewHitsinTrack[ncand];
		for(j=0;j<nSciTilHitsinTrack[ncand];j++) {
			ListTrackCandHit[ncand][i+j]=ListSciTilHitsinTrack[ncand][j];
			ListTrackCandHitType[ncand][i+j] = 1001;
		}
	}  // end of for(ncand=0; ncand< nTotalCandidates; ncand++)


//------------- cleanup section.


	Start[0]=0.;
	Start[1]=0.;
	Start[2]=0.;
	gap = (Double_t) (VERTICALGAP);


    for(ncand=0, nRemainingCandidates=0; ncand< nTotalCandidates; ncand++){
	if(!keepit[ncand]) continue;
	UShort_t &nHitsPar = nSttParHitsinTrack[ncand];
	UShort_t &nHitsSkew = nSttSkewHitsinTrack[ncand];
	Double_t auxS[nHitsSkew];

	for(i=0;i<nHitsSkew;i++){
		auxS[i] = SchosenSkew[ncand][ListSttSkewHitsinTrack[ncand][i]];
	}
//-----------------------------------stampaggi
if(istampa>=2&&IVOLTE<20){
cout<<"\n------------------------------------------------\n"
<<"\tIVOLTE = "<<IVOLTE<<", cand n. "<<ncand<<", prima di TrackCleanup :\n";

	stampetta(nSttTrackCand,keepit);
}
//-------------------------fine stampaggi.


    if(YesClean){
if(istampa>1) cout<<"PndSttMvdTracking, entra in TrackCleanup tracce normali, IVOLTE "<<IVOLTE
	<<" e track cand. "<<ncand<<endl;
	if ( !TrackCleanup(
			gap,
			Ox[ncand],
			Oy[ncand],
			R[ncand],
			KAPPA[ncand],
			FI0[ncand],
			CHARGE[ncand],
			Start,
			nHitsPar,
			&ListSttParHitsinTrack[ncand][0],  // this is already ordered.
			nHitsSkew,
			&ListSttSkewHitsinTrack[ncand][0],  // this is already ordered.
			auxS,
			info,
			RStrawDetectorMin,
			ApotemaMaxInnerParStraw,
			ApotemaMinSkewStraw,
			ApotemaMaxSkewStraw,
			ApotemaMinOuterParStraw,
			RStrawDetectorMax
				) ) {
		keepit[ncand]=false;
		continue;
	}

    }  // end of if(YesClean)

	nRemainingCandidates++;

    }	//  end of for(ncand=0; ncand< nTotalCandidates; ncand++)

//------------- end of cleanup section.


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//   this section collects more tracks starting from the remaining Mvd hits
//   not associated to any Stt track.


	if(!MvdAloneTracking) goto dontdoit ;
//	if( nRemainingCandidates >= MAXTRACKSPEREVENT ) goto dontdoit ;
								// if there are already
								// too many candidates
								// don't do anything.


//  load the UsedPixel and UsedStrip  vectors.
	bool	UsedPixel[nmaxMvdPixelHits],
		UsedStrip[nmaxMvdStripHits];

	UShort_t List[nmaxMvdPixelHits+nmaxMvdStripHits],
		 ListType[nmaxMvdPixelHits+nmaxMvdStripHits];

	Double_t centrex,
		 centrey,
		 radius;

	for(i=0;i<nMvdPixelHit; i++){
		UsedPixel[i]=false;
	}
	for(i=0;i<nMvdStripHit; i++){
		UsedStrip[i]=false;
	}

	for(ncand=0; ncand< nTotalCandidates; ncand++){
//		if(!keepit[ncand]) continue;

		// here don't use the  keepit  information because
		// I don't want to risk to find the wrong tracks again.
		for(i=0; i< nMvdPixelHitsinTrack[ncand]; i++){
			UsedPixel[ ListMvdPixelHitsinTrack[ncand][i] ] = true;
		}// end of for(i=0; i< nMvdPixelHitsinTrack[ncand]; i++)

		for(i=0; i< nMvdStripHitsinTrack[ncand]; i++){
			UsedStrip[ ListMvdStripHitsinTrack[ncand][i] ] = true;
		}// end of for(i=0; i< nMvdStripHitsinTrack[ncand]; i++)

	}//  end of for(ncand=0; ncand< nTotalCandidates; ncand++)

//------------------- 

	//  start from those Mvd CANDIDATES found by Riemann PR, whose MAJORITY of
	//  Pixel or Strip hits were not used previously in this code.

	for(i=0; i<nMvdTrackCand; i++){
		if( nTotalCandidates == MAXTRACKSPEREVENT ) break; // protection for the
						// length of many of my arrays.
//		if( nRemainingCandidates == MAXTRACKSPEREVENT ) break; // protection for the
						// length of many of my arrays.
		nalone=0;

		for(j=0;j<nHitMvdTrackCand[i];j++){

		  if(ListHitTypeMvdTrackCand[i][j] == 
			FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)){
		   if(UsedPixel[ ListHitMvdTrackCand[i][j] ])   continue;
		   AloneX[nalone] = XMvdPixel[ ListHitMvdTrackCand[i][j] ];
		   AloneY[nalone] = YMvdPixel[ ListHitMvdTrackCand[i][j] ];
		   List[nalone] = ListHitMvdTrackCand[i][j];
		   ListType[nalone] = 0;
		   nalone++;
		  }else {// at this point this is a Strip hit; already made sure
			// earlier in the code that there is no third possibility.

//		   if (ListHitTypeMvdTrackCand[i][j] ==	// this condition in principle at this
//								// point is always satisfied.
//			FairRootManager::Instance()->GetBranchId(fMvdStripBranch)){


		   if(UsedStrip[ ListHitMvdTrackCand[i][j] ])   continue;
		   AloneX[nalone] = XMvdStrip[ ListHitMvdTrackCand[i][j] ];
		   AloneY[nalone] = YMvdStrip[ ListHitMvdTrackCand[i][j] ];
		   List[nalone] = ListHitMvdTrackCand[i][j];
		   ListType[nalone] = 1;
		   nalone++;
		  }
		}	// end of   for(j=0;j<nHitMvdTrackCand[i];j++)

		if(nalone < 3 ) continue;

		// here find possibly new tracks

		if( nalone == 3 ){
			status[nTotalCandidates] = CalculateCircleThru3Points(
							AloneX[0],
							AloneY[0],
							AloneX[1],
							AloneY[1],
							AloneX[2],
							AloneY[2],
							&centrex,
							&centrey,
							&radius
							   );
			// the election on the radius is justified by the assumption that
			// the track originates from (0,0,0).
			if( status[nTotalCandidates] && radius > RStrawDetectorMin/2.){
				Ox[nTotalCandidates] = centrex;
				Oy[nTotalCandidates] = centrey;
				R[nTotalCandidates] = radius;
				FI0[nTotalCandidates] =
				    atan2(-Oy[nTotalCandidates], -Ox[nTotalCandidates]);
				if( FI0[nTotalCandidates] < 0. )
					FI0[nTotalCandidates]+= 2.*PI;
				nSttParHitsinTrack[nTotalCandidates]=0;
				nSttSkewHitsinTrack[nTotalCandidates]=0;
				nMvdPixelHitsinTrack[nTotalCandidates]=0;
				nMvdStripHitsinTrack[nTotalCandidates]=0;
				for(j=0; j<nalone;j++){
				   if(ListType[j]==0){
					ListMvdPixelHitsinTrack[nTotalCandidates][
						nMvdPixelHitsinTrack[nTotalCandidates]
										]=List[j];
						nMvdPixelHitsinTrack[nTotalCandidates]++;
				   }else{
					ListMvdStripHitsinTrack[nTotalCandidates][
						nMvdStripHitsinTrack[nTotalCandidates]
										]=List[j];
					nMvdStripHitsinTrack[nTotalCandidates]++;
				   }
				}  //   end of  for(j=1; j<nalone;j++)

				nTrackCandHit[nTotalCandidates]=nalone;
				keepit[nTotalCandidates]=true;
				Mvdhits[nTotalCandidates]=true;
				nTotalCandidates++;
				nRemainingCandidates++;
			} // end  of  if( status[nTotalCandidates] )


		} else { // continuation of  if( nalone == 3 )
			iexcl = 0;	// exclude first point in List from fit
					//  because it is taken as center of new
					//  reference frame in the fit.
			trajectory_vertex[0] = AloneX[0];
			trajectory_vertex[1] = AloneY[0];

			for(j=0; j<nalone;j++){
				ListTrackCandHit[nTotalCandidates][j]= List[j];
				ListTrackCandHitType[nTotalCandidates][j]= ListType[j];
			}
			// here at this point there are no Stt hits associated.
			RefitMvdStt(
				nalone,
				&ListTrackCandHit[nTotalCandidates][0],
				&ListTrackCandHitType[nTotalCandidates][0],
				info,
				rotationangle,
				trajectory_vertex,
				iexcl,

				&ALFA[nTotalCandidates], // output of the fit
				&BETA[nTotalCandidates], // output of the fit
				&GAMMA[nTotalCandidates],// output of the fit
				&status[nTotalCandidates]// fit status; true = success.
					);



				Ox[nTotalCandidates] = -ALFA[nTotalCandidates]/2.;
				Oy[nTotalCandidates] = -BETA[nTotalCandidates]/2.;
				R[nTotalCandidates] =
					Ox[nTotalCandidates]*Ox[nTotalCandidates]
					+Oy[nTotalCandidates]*Oy[nTotalCandidates]
					-GAMMA[nTotalCandidates];
				if( R[nTotalCandidates] > 0. ) R[nTotalCandidates]=
					sqrt(R[nTotalCandidates]) ;
				else  R[nTotalCandidates]=0.;
			if(status[nTotalCandidates]&& R[nTotalCandidates] > RStrawDetectorMin/2.){
				FI0[nTotalCandidates] =
				    atan2(-Oy[nTotalCandidates], -Ox[nTotalCandidates]);
				if( FI0[nTotalCandidates] < 0. )
					FI0[nTotalCandidates]+= 2.*PI;

				nSttParHitsinTrack[nTotalCandidates]=0;
				nSttSkewHitsinTrack[nTotalCandidates]=0;
				nMvdPixelHitsinTrack[nTotalCandidates]=0;
				nMvdStripHitsinTrack[nTotalCandidates]=0;
				for(j=0; j<nalone;j++){
					if(ListType[j]==0){
						ListMvdPixelHitsinTrack[nTotalCandidates][
							nMvdPixelHitsinTrack[nTotalCandidates]
										]=List[j];
						nMvdPixelHitsinTrack[nTotalCandidates]++;
					}else{
						ListMvdStripHitsinTrack[nTotalCandidates][
							nMvdStripHitsinTrack[nTotalCandidates]
										]=List[j];
						nMvdStripHitsinTrack[nTotalCandidates]++;
					}
				}  //   end of  for(j=1; j<nalone;j++)

				nTrackCandHit[nTotalCandidates]=nalone;

				keepit[nTotalCandidates]=true;

				Mvdhits[nTotalCandidates]=true;
				nTotalCandidates++;
				nRemainingCandidates++;

			} // end of  if(status[nTotalCandidates])

		} // end of  if( nalone == 3 )

	} //end of  for(i=0; i<nMvdTrackCand; i++)


//------------------ trying to attach a SciTil hit to the track.

	for(ncand=nSttTrackCand; ncand< nTotalCandidates; ncand++){
//		if(!keepit[ncand]) continue;
		nSciTilHitsinTrack[ncand]=0;	/// for the time being.
	}

//------------------ end trying to attach a SciTil hit to the track.



//     ordering all the hits belonging to the new candidate tracks, by increasing R;
//     loading    ListTrackCandHit. The ordering is necessary here because the charge
//     has to be calculated.

	Ordering_Loading_ListTrackCandHit(
		keepit,
		nSttTrackCand,
		nTotalCandidates,
		info,
		Ox,
		Oy,
		R,
		Trajectory_Start,
		CHARGE,
		SchosenSkew  // here this array is irrelevant since there are not yet
			// any Stt hits in Track Cand.
		);

//-------------------- end of ordering

// -----------------  finding the charge of those track found starting from Mvd

	bool flag;
	int nMvdOnly;
	Double_t Sini, Slast,
		esse[nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack+nmaxSttHitsInTrack];

	Double_t
	DriftRadius[nTrackCandHit[ncand]],// all skew hits have double
	ErrorDriftRadius[nTrackCandHit[ncand]],// solutions
	S[nTrackCandHit[ncand]], // multiplication by 2 in the
	ZED[nTrackCandHit[ncand]]; // rather improbable chance that

	for(ncand=nSttTrackCand; ncand< nTotalCandidates; ncand++){
	    if(!keepit[ncand]) continue;


	// S[]  is used later in the SZ fit.

	    for(i=0, nMvdOnly=0; i<nTrackCandHit[ncand]; i++){

		if( ListTrackCandHitType[ncand][i] == 0){ //  Pixel
			S[nMvdOnly] = atan2(
					YMvdPixel[ListTrackCandHit[ncand][i]]-Oy[ncand],
					XMvdPixel[ListTrackCandHit[ncand][i]]-Ox[ncand]
					);
			if(S[nMvdOnly]<0.) S[nMvdOnly] +=2.*PI;
			if(S[nMvdOnly]<0.) S[nMvdOnly] =0.;
			esse[i]=S[nMvdOnly];
			ZED[nMvdOnly] = ZMvdPixel[ListTrackCandHit[ncand][i]];
			// DriftRadius is set conventionally at -1, for later use in the SZ fit;
			// the error on the point used in the fit is ErrorDriftRadius and this
			// is overestimated to be  1cm.
			DriftRadius[nMvdOnly]= -1. ;
			ErrorDriftRadius[nMvdOnly]= 1. ;
			nMvdOnly++;
		} else if (ListTrackCandHitType[ncand][i] == 1) {  //  Strip
			S[nMvdOnly] = atan2(
					YMvdStrip[ListTrackCandHit[ncand][i]]-Oy[ncand],
					XMvdStrip[ListTrackCandHit[ncand][i]]-Ox[ncand]
					);
			if(S[nMvdOnly]<0.) S[nMvdOnly] +=2.*PI;
			if(S[nMvdOnly]<0.) S[nMvdOnly] =0.;
			esse[i]=S[nMvdOnly];
			ZED[nMvdOnly] = ZMvdStrip[ListTrackCandHit[ncand][i]];
			// DriftRadius is set conventionally at -1, for later use in the SZ fit;
			// the error on the point used in the fit is ErrorDriftRadius and this
			// is overestimated to be  1cm.
			DriftRadius[nMvdOnly]= -1. ;
			ErrorDriftRadius[nMvdOnly]= 1. ;
			nMvdOnly++;
		}   //   end of    if( ListTrackCandHitType[ncand][0] == 0)


	    } // end of  for(i=0; i<nTrackCandHit[ncand]; i++)

	//  fixing possible discontinuity between fi<2*PI and fi>0.

	    flag = false;
	    for(i=1;i<nTrackCandHit[ncand]; i++){
		if(fabs(esse[i-1] - esse[i]) > 1.5*PI ){
			flag=true;
			break;
		}
	    }
	    if( flag ){
		if( esse[0] < PI) Sini = esse[0]+2.*PI;
		if( esse[nTrackCandHit[ncand]-1] < PI) Slast=esse[nTrackCandHit[ncand]-1]+2.*PI;
	    } else {
		Sini = esse[0];
		Slast= esse[nTrackCandHit[ncand]-1];
	    }

	    if( Sini > Slast) {
		CHARGE[ncand] =  1;
	    } else {
		CHARGE[ncand] =  -1;
	    }



	// it is necessary first to calculate Fi_low_limit and Fi_up_limit.

	PndSttFindingParallelTrackAngularRange(
		Ox[ncand],
		Oy[ncand],
		R[ncand],
		CHARGE[ncand],
		&Fi_low_limit[ncand],	// Fi (in XY Helix frame) lower limit using
					// the Stt detector minimum/maximum radius
					// Fi_low_limit is ALWAYS between 0. and 2PI
		&Fi_up_limit[ncand],	// Fi (in XY Helix frame) upper limit using
					// the Stt detector maximum/minimum radius
					// Fi_up_limit is ALWAYS > Fi_low_limit and
					// possibly > 2PI.
		&statusflag[ncand],// vector; =0, all well; =1, track contained completely between RMin
			   // and RMax; = -1 track contained within RMin; =-2 track outside RMax.
		RStrawDetectorMin,
		RStrawDetectorMax
						);
		// discard tracks completely outside RMin (since they are formed by Mvd hits).
		if(statusflag[ncand]== -2 || statusflag[ncand]== 1){
			keepit[ncand]=false;
			continue;
		}
//  take care of possible discontinuities at 0.

	FixDiscontinuitiesFiangleinSZplane(
		nTrackCandHit[ncand],	//  n hits in track. 
		S,	// S can be modified by +-2*PI if necessary.
		&FI0[ncand],	// this remains unchanged.
		CHARGE[ncand]	// this remains unchanged.
				);




// ----------------------------- fit in SZ with the Mvd tracks
		resultFitSZagain[ncand] = FitSZspace(
					nMvdOnly,	// n. hits to be fitted
					S,
					ZED,
					DriftRadius,
					ErrorDriftRadius,
					FI0[ncand],
					20,	// maximum number allowed in the fit
						// deve essere meno di 30+30+60.
					&emme
						);
		if( resultFitSZagain[ncand]==1){
			KAPPA[ncand] = emme;
		} else {
			keepit[ncand]=false;
		}

	} //   end of  for(ncand=nSttTrackCand; ncand< nTotalCandidates; ncand++)

// ----------------------------- end fit in SZ with the Mvd tracks




//---  try to associate of parallel Stt  straw  hits to this new tracks.


	CollectParSttHitsagain(
			keepit,
			Mvdhits,
			info,
			nSttParHit,
			ListAllParHits,
			nSttTrackCand,	// starting candidate n. (included).
			nTotalCandidates, // ending candidate n. (excluded).
			Ox,
			Oy,
			R,
			KAPPA,
			FI0,
			Fi_low_limit,
			Fi_up_limit,
			nSttParHitsinTrack, // input and output
			ListSttParHitsinTrack // input and output
			);


   for(ncand=nSttTrackCand; ncand< nTotalCandidates; ncand++){

   //     try to attach skew hits to the new tracks (the result can also be 0).

	if(!keepit[ncand]) continue;
	// when statusflag[ncand]<0 the track does not intersect Stt region.
	if( statusflag[ncand]<0) continue;


	   nSttSkewHitsinTrack[ncand] = AssociateSkewHitsToXYTrack(
		InclusionListStt,
		nSttSkewHit,
		ListAllSkewHits,
		Ox[ncand],   //  input : X of center of XY plane circle
		Oy[ncand],   //  input : Y of center of XY plane circle
		R[ncand],   //  input : Radius of XY plane circle
		info,
		WDX,
		WDY,
		WDZ,
		Fi_low_limit[ncand],	// Fi (in Helix XY frame) lower limit using the Stt detector minimum/maximum radius
		Fi_up_limit[ncand],	// Fi (in Helix XY frame) upper limit using the Stt detector maximum/minimum radius
		CHARGE[ncand],
		TemporarySkewList, // output,  list of selected skew hits (in skew numbering)
		TemporaryS,       //  output,  S coordinate of selected Skew hit
		TemporaryZ,       //  output,  Z coordinate of selected Skew hit (center wire)
		TemporaryZDrift,   //  output,  drift distance IN Z DIRECTION only, of selected Skew hit
		TemporaryZErrorafterTilt   //  output,  Radius taking into account the tilt, IN Z DIRECTION only, of selected Skew hit
								);

	   for(j=0;j<nSttSkewHitsinTrack[ncand];j++)
	   {
		ListSttSkewHitsinTrack[ncand][j]=TemporarySkewList[j][0];
		SchosenSkew[ncand][ListSttSkewHitsinTrack[ncand][j]] = TemporaryS[j];

//------------- inizio stampe.
if(istampa>=2){
	cout<<"PndSttMvdTracking, partendo da Mvd evt. "<<IVOLTE
	<<", cand. n. "<<ncand<<", hit skew (original) "<<ListSttSkewHitsinTrack[ncand][j]
	<<", Schosen "<<TemporaryS[j]<<endl;
}
//------------------------fine stampe.
	   }


  //  ------------------- end of attachment of skew hits to the new tracks.



   } // end of   for(ncand=nSttTrackCand; ncand< nTotalCandidates; ncand++)



//     ordering again all the hits belonging to the new candidate tracks, by increasing R;
//     loading    ListTrackCandHit.

	Ordering_Loading_ListTrackCandHit(
		keepit,
		nSttTrackCand,
		nTotalCandidates,
		info,
		Ox,
		Oy,
		R,
		Trajectory_Start,
		CHARGE,
		SchosenSkew
		);

//-------------------- end of ordering

	// adding at the end the SciTil hit (if present).

	for(ncand=nSttTrackCand; ncand< nTotalCandidates; ncand++){
		if( nSciTilHitsinTrack[ncand]==1) {
			i=nMvdPixelHitsinTrack[ncand]+nMvdStripHitsinTrack[ncand]+
			  nSttParHitsinTrack[ncand]+nSttSkewHitsinTrack[ncand];
			ListTrackCandHit[ncand][i] = ListSciTilHitsinTrack[ncand][0];
			ListTrackCandHitType[ncand][i] = 1001;
		}
	}  // end of for(ncand=0; ncand< nTotalCandidates; ncand++)


//-----------  cleanup of the new tracks found

  if(YesClean){
     for(ncand=nSttTrackCand; ncand< nTotalCandidates; ncand++){

if(istampa>1){ cout<<"PndSttMvdTracking, entra in TrackCleanup tracce Mvd, IVOLTE "<<IVOLTE
	<<" e track cand. "<<ncand<<endl<<"\tN. || stt = "
	<<nSttParHitsinTrack[ncand]<<", n Skew "<<nSttSkewHitsinTrack[ncand]
	<<endl;
}

	if ( !TrackCleanup(
			gap,
			Ox[ncand],
			Oy[ncand],
			R[ncand],
			KAPPA[ncand],
			FI0[ncand],
			CHARGE[ncand],
			Start,
			nSttParHitsinTrack[ncand],
			&ListSttParHitsinTrack[ncand][0],  // this is already ordered.
			nSttSkewHitsinTrack[ncand],
			&ListSttSkewHitsinTrack[ncand][0],  // this is already ordered.
			&SchosenSkew[ncand][0],
			info,
			RStrawDetectorMin,
			ApotemaMaxInnerParStraw,
			ApotemaMinSkewStraw,
			ApotemaMaxSkewStraw,
			ApotemaMinOuterParStraw,
			RStrawDetectorMax
				) ) {
		keepit[ncand]=false;
	}  // end of if ( !TrackCleanup

     } // end of   for(ncand=nSttTrackCand; ncand< nTotalCandidates; ncand++)
if(istampa>1){ cout<<"\tPndSttMvdTracking, uscito da TrackCleanup tracce Mvd, keepit = ";
	if(keepit[ncand]) cout<<" true!\n"; else  cout<<" false!\n";
}

  }  // end of if(YesClean)

//-----------  end of cleanup of new tracks found.



dontdoit:  ;	//  this is the label where the computer jumps if there are already
		//  MAXTRACKSPEREVENT candidates.


// ----------------------------- end of finding Tracks starting with the Mvd tracks
// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------


skipping: ;

//----------stampaggi
if(istampa>=2){
cout<<"Evt. n. "<<IVOLTE<<",fine del trackfinding; n. Track Cand totali = "<<nTotalCandidates<<endl;

for(int it=0; it<nTotalCandidates;it++){
cout<<"-------------------\n";
cout<<"Evt. n. "<<IVOLTE<<",  Track Cand n. "<<it<<", keepit = "<<keepit[it];
cout<<"\n\t nMvdPixelHitsinTrack "<<nMvdPixelHitsinTrack[it]
	<<" e loro elenco :\n"<<endl;
for(l =0;l<nMvdPixelHitsinTrack[it];l++){
	cout<<"\tMvd Pixel hit n. "<<ListMvdPixelHitsinTrack[it][l]<<endl;
};
cout<<"\n\t nMvdStripHitsinTrack "<<nMvdStripHitsinTrack[it]
	<<" e loro elenco :\n"<<endl;
for(l =0;l<nMvdStripHitsinTrack[it];l++){
	cout<<"\tMvd Strip hit n. "<<ListMvdStripHitsinTrack[it][l]<<endl;
};
cout<<"\n\t nSttParHitsinTrack "<<nSttParHitsinTrack[it]<<" e loro elenco :\n"<<endl;
for(l =0;l<nSttParHitsinTrack[it];l++){
	cout<<"\tstt hit || n. "<<ListSttParHitsinTrack[it][l]<<endl;
};
cout<<"\n\t nSttSkewHitsinTrack "<<nSttSkewHitsinTrack[it]<<" e loro elenco :\n"<<endl;
for(l =0;l<nSttSkewHitsinTrack[it];l++){
	cout<<"\tstt hit skew n. "<<ListSttSkewHitsinTrack[it][l]<<endl;
};
cout<<"\n\t nSciTilHitsinTrack "<<nSciTilHitsinTrack[it]<<" e loro elenco :\n"<<endl;
if(nSciTilHitsinTrack[it]>0){
	cout<<"\tscitil hit n. "<<ListSciTilHitsinTrack[it][0]<<endl;
};

cout<<"\tand now the list of hits as it is in ListTrackCandHit :\n";
for(l =0;l<nSttSkewHitsinTrack[it]+nSttParHitsinTrack[it]+nMvdStripHitsinTrack[it]
	+nMvdPixelHitsinTrack[it]+nSciTilHitsinTrack[it];l++){
	cout<<"\tstt hit n. "<<ListTrackCandHit[it][l]<<
	", hit type = "<< ListTrackCandHitType[it][l]<<endl;
};
cout<<"-------------------\n";

}
}
//----- end stampaggi



//------------ section with comparison MC Mvd hits - associated hits to a certain track

//--------------------  comparison of the PndTrackCand from PR of the STT to the MC truth
//    associate the tracks found with Pattern Recognition to the MC tracks



//  the following method associates the Mvd hits to corresponding MC tracks

   Int_t	FromPixeltoMCTrack[nMvdPixelHit],
		FromStriptoMCTrack[nMvdStripHit];

  if( doMcComparison){
	MvdMatchtoMC(
		nMvdMCPoint,
		FromPixeltoMCTrack,	// output
		FromStriptoMCTrack	// output
		);
  }

//------------------------------------------------------------------------------------


//  this section associates the found tracks  to the
//  MC tracks, creating a bilinear correspondence between MC tracks and PR Found tracks

	UShort_t	nMvdPixelCommon[nTotalCandidates],
			MvdPixelCommonList[nTotalCandidates][nMvdPixelHit],
			nMvdPixelSpuriinTrack[nTotalCandidates],
			MvdPixelSpuriList[nTotalCandidates][nMvdPixelHit],
			nMCMvdPixelAlone[nTotalCandidates],
			MCMvdPixelAloneList[nTotalCandidates][nMvdPixelHit],

			nMvdStripCommon[nTotalCandidates],
			MvdStripCommonList[nTotalCandidates][nMvdStripHit],
			nMvdStripSpuriinTrack[nTotalCandidates],
			MvdStripSpuriList[nTotalCandidates][nMvdStripHit],
			nMCMvdStripAlone[nTotalCandidates],
			MCMvdStripAloneList[nTotalCandidates][nMvdStripHit];


   if( nMCTracks >0 && nTotalCandidates > 0 && doMcComparison ){


	int nmid,nn;

	Double_t X1[MAXTRACKSPEREVENT],
		 Y1[MAXTRACKSPEREVENT],
		 X2[MAXTRACKSPEREVENT],
		 Y2[MAXTRACKSPEREVENT],
		 X3[MAXTRACKSPEREVENT],
		 Y3[MAXTRACKSPEREVENT];
	
	for(i=0; i<nTotalCandidates;i++){
		if(!keepit[i]) continue;

		nn = nSttParHitsinTrack[i]+nSttSkewHitsinTrack[i]+
			nMvdPixelHitsinTrack[i]+nMvdStripHitsinTrack[i];
		// the first point on trajectory is point closest approach to (0,0)
		X1[i] = Ox[i] + R[i]*cos( FI0[i]);
		Y1[i] = Oy[i] + R[i]*sin( FI0[i]);

/*
		//  the first hit
		switch ( ListTrackCandHitType[i][0]){
			case 0 :	// Pixel
				X1[i] = XMvdPixel[ ListTrackCandHit[i][0] ] ;
				Y1[i] = YMvdPixel[ ListTrackCandHit[i][0] ] ;
			break;
			case 1 :	// Strip
				X1[i] = XMvdStrip[ ListTrackCandHit[i][0] ] ;
				Y1[i] = YMvdStrip[ ListTrackCandHit[i][0] ] ;
			break;
			case 2 :	// Straw parallel
				X1[i] = info[ ListTrackCandHit[i][0] ][0] ;
				Y1[i] = info[ ListTrackCandHit[i][0] ][1] ;
			break;
			case 3 :	// Straw skew
				X1[i] = info[ ListTrackCandHit[i][0] ][0] ;
				Y1[i] = info[ ListTrackCandHit[i][0] ][1] ;
			break;
		};
		//  the middle point hit
		nn = nSttParHitsinTrack[i]+nMvdPixelHitsinTrack[i]+nMvdStripHitsinTrack[i];
		nmid= (int) (nn/2) ;
		switch ( ListTrackCandHitType[i][nmid-1]){
			case 0 :	// Pixel
				X2[i] = XMvdPixel[ ListTrackCandHit[i][nmid-1] ] ;
				Y2[i] = YMvdPixel[ ListTrackCandHit[i][nmid-1] ] ;
			break;
			case 1 :	// Strip
				X2[i] = XMvdStrip[ ListTrackCandHit[i][nmid-1] ] ;
				Y2[i] = YMvdStrip[ ListTrackCandHit[i][nmid-1] ] ;
			break;
			case 2 :	// Straw parallel
				X2[i] = info[ ListTrackCandHit[i][nmid-1] ][0] ;
				Y2[i] = info[ ListTrackCandHit[i][nmid-1] ][1] ;
			break;
			case 3 :	// Straw skew
				X2[i] = info[ ListTrackCandHit[i][nmid-1] ][0] ;
				Y2[i] = info[ ListTrackCandHit[i][nmid-1] ][1] ;
			break;
		};

*/

		//  the third point on trajectory is given by the last hit

			switch ( ListTrackCandHitType[i][nn-1]){
			case 0 :	// Pixel
				X3[i] = XMvdPixel[ ListTrackCandHit[i][nn-1] ] ;
				Y3[i] = YMvdPixel[ ListTrackCandHit[i][nn-1] ] ;
			break;
			case 1 :	// Strip
				X3[i] = XMvdStrip[ ListTrackCandHit[i][nn-1] ] ;
				Y3[i] = YMvdStrip[ ListTrackCandHit[i][nn-1] ] ;
			break;
			case 2 :	// Straw parallel
				X3[i] = info[ ListTrackCandHit[i][nn-1] ][0] ;
				Y3[i] = info[ ListTrackCandHit[i][nn-1] ][1] ;
			break;
			case 3 :	// Straw skew
				X3[i] = info[ ListTrackCandHit[i][nn-1] ][0] ;
				Y3[i] = info[ ListTrackCandHit[i][nn-1] ][1] ;
			break;
			};


		//  the 2nd point on the trajectory is given by the middle between
		//  first and third point.

		Double_t angle, middle;
		angle = atan2(Y3[i]-Oy[i], X3[i]-Ox[i]);
		if(angle <0.) angle+= 2.*PI;
		if(angle <0.) angle = 0.;

		if( CHARGE[i]>0 ){ //  clockwise.
			if( FI0[i] < angle ) angle -= 2.*PI;
			if( FI0[i] < angle ) angle =0.;
		} else{  // counterclockwise.
			if( FI0[i] > angle ) angle += 2.*PI;
			if( FI0[i] > angle ) angle = FI0[i];
		}
			middle = (FI0[i]+angle)/2.;

		X2[i] = Ox[i] + R[i]*cos( middle );
		Y2[i] = Oy[i] + R[i]*sin( middle );


	}	// end of  for(i=0; i<nTotalCandidates;i++)

	AssociateFoundTrackstoMCquater(
		keepit,
		  info,
		  Ox,
		  Oy,
		  R,
		  X1,
		  Y1,
		  X2,
		  Y2,
		  X3,
		  Y3,
                  nTotalCandidates,
                  nSttParHitsinTrack,
                  ListSttParHitsinTrack,
                  nSttSkewHitsinTrack,
                  ListSttSkewHitsinTrack,

		nMvdPixelHitsinTrack,
		ListMvdPixelHitsinTrack,
		FromPixeltoMCTrack,
		nMvdStripHitsinTrack,
		ListMvdStripHitsinTrack,
		FromStriptoMCTrack,

                  daTrackFoundaTrackMC
                                   );



if(istampa>=2){
//	for(i=0;i<nSttTrackCand;i++){
	for(i=0;i<nTotalCandidates;i++){
		cout<<"from PndSttMvdTracking : Track candidate n. "<<i;
		if(keepit[i]){
		  cout<<"  associated to MC Track n. "<<daTrackFoundaTrackMC[i]<<endl;
		} else {
		  cout<<"  has keepit  false!\n";
		}
	}
}

//  the following method counts the matched to MC and spurious hits for all STT  Found tracks and
//  also the ghost tracks
   SttMatchedSpurious(
			keepit,
			nSttHit,
			info,
			nTotalCandidates,
			nSttParHitsinTrack, // n. hits paralleli, dal PR
			ListSttParHitsinTrack, // dal PR
			nSttSkewHitsinTrack, // n. hits skew, dal PR
			ListSttSkewHitsinTrack, // dal PR

			nParalCommon,
                        ParalCommonList,
                        nSpuriParinTrack,
                        ParSpuriList,

			nSkewCommon,
                        SkewCommonList,
                        nSpuriSkewinTrack,
                        SkewSpuriList,

                        nHitsInMCTrack,
                        nSkewHitsInMCTrack,

			nMCParalAlone,
                        MCParalAloneList,
			nMCSkewAlone,
                        MCSkewAloneList,

			daTrackFoundaTrackMC
			);


//--------------------  end comparison of the PndTrackCand from PR of the STT to the MC truth


//	assumo che la traccia MC associata alla traccia trovata dal Pattern Recognition
//	sia quella giusta e di
//	conseguenza calcolo gli hits Mvd spuri e comuni


   MvdMatchedSpurioustoTrackCand(
			nTotalCandidates,		// input
			keepit,				// input
			&daTrackFoundaTrackMC[0],	// input

			&FromPixeltoMCTrack[0],		// input
			&FromStriptoMCTrack[0],		// input

			&nMvdPixelHitsinTrack[0],	// input
			ListMvdPixelHitsinTrack,	// input
			&nMvdStripHitsinTrack[0],	// input
			ListMvdStripHitsinTrack,	// input

			&nMvdPixelCommon[0],		// output
			&MvdPixelCommonList[0][0],	// output
			&nMvdPixelSpuriinTrack[0],	// output
			&MvdPixelSpuriList[0][0],	// output
			&nMCMvdPixelAlone[0],		// output
			&MCMvdPixelAloneList[0][0],	// output

			&nMvdStripCommon[0],		// output
			&MvdStripCommonList[0][0],	// output
			&nMvdStripSpuriinTrack[0],	// output
			&MvdStripSpuriList[0][0],	// output
			&nMCMvdStripAlone[0],		// output
			&MCMvdStripAloneList[0][0]	// output
			);



//---------- inizio stampaggi
if(istampa>=3){

//     for(ncand=0; ncand< nSttTrackCand; ncand++){
	 int nbuone=-1;
     for(ncand=0; ncand< nTotalCandidates; ncand++){
	if(!keepit[ncand]) continue;
	nbuone++;
//----------------- ora la traccia MC corrispondente a questa traccia Stt
//		cout<<"\n\nda PndSttMvdTracking : la PndTrackCand n. "<<ncand;
		cout<<"\n\nda PndSttMvdTracking : la PndTrackCand n. "<<nbuone;
       if(daTrackFoundaTrackMC[ncand] >-1 ) {

		Int_t icode, im;
		Double_t aaa,Rr, Dd, Fifi, Oxx, Oyy, Cx, Cy, Pxx, Pyy, carica  ;
		PndMCTrack* pMC;
		im=daTrackFoundaTrackMC[ncand];
		pMC = (PndMCTrack*) fMCTrackArray->At(im);
	   if ( pMC ) {
         	icode  = pMC->GetPdgCode() ;    //   PDG code of track
         	Oxx = pMC->GetStartVertex().X();    //   X of starting point track
         	Oyy = pMC->GetStartVertex().Y();    //   Y of starting point track
         	Pxx = pMC->GetMomentum().X();
         	Pyy = pMC->GetMomentum().Y();
         	aaa = sqrt( Pxx*Pxx + Pyy*Pyy);
         	Rr =   aaa*1000./(BFIELD*CVEL);    //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla
         	TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
         	TParticlePDG *fParticle= fdbPDG->GetParticle(icode);
       		if (icode>1000000000) carica = 1.;
       		else  carica = fParticle->Charge()/3. ;    //   charge of track
		if(fabs(carica)<1.e-5) {
			cout<<"...associata ad una traccia neutra, assurdo!\n";
			continue ;}
           	Cx = Oxx + Pyy*1000./(BFIELD*CVEL*carica);
           	Cy = Oyy - Pxx*1000./(BFIELD*CVEL*carica);
		cout<<"  e' associata alla traccia MC n. "<<im<<" che ha :\n"
		<<"\tR = "<<Rr<<" ed ha centro in Cx = "<<Cx<<" e Cy = "<<Cy<<endl;
	  } else {
		cout<<"  ha daTrackFoundaTrackMC[ncand] = "<<daTrackFoundaTrackMC[ncand]
		<<"   pero' non ha puntatore alla sua classe PndMCTrack\n";
	  }
	}  else {
		cout<<"  ha daTrackFoundaTrackMC[ncand] = "<<daTrackFoundaTrackMC[ncand]<<endl;
	}
		cout<<"Track Cand n. "<<nbuone<<", associato a Traccia MC n. "<<
		daTrackFoundaTrackMC[ncand]<<", ha "<<
		nMvdPixelHitsinTrack[ncand]<<
		" Pixel Hits associati, di cui ecco la lista :\n";
		for(j=0;j<nMvdPixelHitsinTrack[ncand];j++){
			cout<<"\tHit n. "<<
			ListMvdPixelHitsinTrack[ncand][j]<<
			" che fu associato a MC track "
			<<FromPixeltoMCTrack[
			  ListMvdPixelHitsinTrack[ncand][j]
			  ]<<endl;
		}

		cout<<"dalla mia funzione risulta che ci sono "<<
		nMvdPixelCommon[ncand]<<
		" Pixel hits mecciati alla giusta traccia MC e la loro lista e' :\n";
		if( nMvdPixelCommon[ncand]==0) cout<<"\t-----------nessuna lista\n";
		for(j=0;j<nMvdPixelCommon[ncand];j++){
			cout<<"\tHit n. "<<
			MvdPixelCommonList[ncand][j]<<
			" che fu associato a MC track "
			<<FromPixeltoMCTrack[
			  MvdPixelCommonList[ncand][j]
			  ]<<endl;
		}

		cout<<"  ... inoltre ci sono  "<<nMvdPixelSpuriinTrack[ncand]
		<<"  Pixel hits spuri, elenco e' :\n";
		if( nMvdPixelSpuriinTrack[ncand]==0) cout<<"\t-----------nessuna lista\n";
		for(j=0;j<nMvdPixelSpuriinTrack[ncand];j++){
			cout<<"\tHit n. "<<
			MvdPixelSpuriList[ncand][j]<<
			" che fu associato a MC track "
			<<FromPixeltoMCTrack[
			  MvdPixelSpuriList[ncand][j]
			  ]<<endl;
		}
		cout<<"  ... inoltre ci sono  "<<nMCMvdPixelAlone[ncand]
		<<"  Pixel hits non inclusi, il cui elenco e' :\n";
		if( nMCMvdPixelAlone==0) cout<<"\t-----------nessuna lista\n";
		for(j=0;j<nMCMvdPixelAlone[ncand];j++){
			cout<<"\tHit n. "<<
			MCMvdPixelAloneList[ncand][j]<<
			" che fu associato a MC track "
			<<FromPixeltoMCTrack[
			  MCMvdPixelAloneList[ncand][j]
			  ]<<endl;
		}


		cout<<"inoltre, sempre Track Cand n. "<<nbuone<<", associato a Traccia MC n. "<<
		daTrackFoundaTrackMC[ncand]<<", ha "<<
		nMvdStripHitsinTrack[ncand]<<
		" Strip Hits associati, di cui ecco la lista :\n";
		if(nMvdStripHitsinTrack[ncand]==0) cout<<"\t-----------nessuna lista\n";
		for(j=0;j<nMvdStripHitsinTrack[ncand];j++){
			cout<<"\tHit n. "<<
			ListMvdStripHitsinTrack[ncand][j]<<
			" che fu associato a MC track "
			<<FromStriptoMCTrack[
			  ListMvdStripHitsinTrack[ncand][j]
			  ]<<endl;
		}
		cout<<"dalla mia funzione risulta che ci sono "<<
		nMvdStripCommon[ncand]<<
		" Strip hits mecciati alla giusta traccia MC e la loro lista e' :\n";
		if(nMvdStripCommon[ncand]==0) cout<<"\t-----------nessuna lista\n";
		for(j=0;j<nMvdStripCommon[ncand];j++){
			cout<<"\tHit n. "<<
			MvdStripCommonList[ncand][j]<<
			" che fu associato a MC track "
			<<FromStriptoMCTrack[
			  MvdStripCommonList[ncand][j]
			  ]<<endl;
		}
		cout<<"  ... inoltre ci sono  "<<nMvdStripSpuriinTrack[ncand]
		<<"  Strip hits spuri, il cui elenco e' :\n";
		if(nMvdStripSpuriinTrack[ncand]==0) cout<<"\t-----------nessuna lista\n";
		for(j=0;j<nMvdStripSpuriinTrack[ncand];j++){
			cout<<"\tHit n. "<<
			MvdStripSpuriList[ncand][j]<<
			" che fu associato a MC track "
			<<FromStriptoMCTrack[
			  MvdStripSpuriList[ncand][j]
			  ]<<endl;
		}
		cout<<"  ... inoltre ci sono  "<<nMCMvdStripAlone[ncand]
		<<"  Strip hits non inclusi, il cui elenco e' :\n";
		if(nMCMvdStripAlone[ncand]==0) cout<<"\t-----------nessuna lista\n";
		for(j=0;j<nMCMvdStripAlone[ncand];j++){
			cout<<"\tHit n. "<<
			MCMvdStripAloneList[ncand][j]<<
			" che fu associato a MC track "
			<<FromStriptoMCTrack[
			  MCMvdStripAloneList[ncand][j]
			  ]<<endl;
		}




	}	// end of for(ncand=0; ncand< nSttTrackCand; ncand++)

}	// end of if(istampa>=2)
//-------------fine stampaggi




// ora il confronto per il meeting di  Groningen

//---------- conteggio delle tracce MC accettabili!!
int citata;
int nMCTracksaccettabili=0;
int ListaMCTracksaccettabili[nMCTracks];
for (i=0;i<nMCTracks;i++){
	citata=0;
   	pMCtr = (PndMCTrack*) fMCTrackArray->At(i);
   	if ( ! pMCtr ) continue;
         Double_t aaa, carica, Rr, Dd, Fifi, Oxx, Oyy, Cx, Cy, Pxx, Pyy  ;
         Int_t icode;
         icode  = pMCtr->GetPdgCode() ;    //   PDG code of track
         Oxx = pMCtr->GetStartVertex().X();    //   X of starting point track
         Oyy = pMCtr->GetStartVertex().Y();    //   Y of starting point track
         Pxx = pMCtr->GetMomentum().X();
         Pyy = pMCtr->GetMomentum().Y();
         aaa = sqrt( Pxx*Pxx + Pyy*Pyy);
         Rr =   aaa*1000./(BFIELD*CVEL);    //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla



	if(istampa>2){
		TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
		TParticlePDG *fParticle= fdbPDG->GetParticle(icode);
		if (icode>1000000000) carica = 1.;
		else  carica = fParticle->Charge()/3. ;    //   charge of track
		if(fabs(carica)<1.e-5) continue;
		Cx = Oxx + Pyy*1000./(BFIELD*CVEL*carica);
		Cy = Oyy - Pxx*1000./(BFIELD*CVEL*carica);
		cout<<"da PndSttMvdTracking, evento (cominciando da 0) n. "<<IVOLTE<<
		",  traccia MC n. "<<i<<",  R MC = "<<Rr<<", Centro X = "<<Cx
		<<", Centro Y = "<<Cy<<endl;
	}



	for(int ic=0;ic<nSttHit;ic++){
		if( ( (int) (info[ic][6]+0.1) ) == i   && info[ic][5]<2.){
			citata++;
		}
	}
	if( citata>2 && fabs(Oxx)<1. && fabs(Oyy) < 1. ) {
		ListaMCTracksaccettabili[nMCTracksaccettabili]=i;
		nMCTracksaccettabili++;
	}


}





if(istampa>=1){cout<<"da PndSttMvdTracking, MC comparison; evt. "<<IVOLTE<<", nMCTracks "<<nMCTracks
<<", n. MC tracce accettabili "<<nMCTracksaccettabili
<<" e loro lista :\n";
	for(int g=0; g<nMCTracksaccettabili;g++){
		cout<<"\ttraccia MC n. "<<ListaMCTracksaccettabili[g]<<endl;
	}
cout<<"Total track trovate "<<nTotalCandidates<<endl;
}

//----------- fine conteggio delle tracce MC accettabili

//  if(istampa>=1 )  fprintf(HANDLE, "\n Evento %d  NTotaleTracceMC %d ------\n",IVOLTE, nMCTracks);
  if(istampa>=1 ) {

   fprintf(HANDLE, "\n Evento %d  NTotaleTracceMC %d ------\n",IVOLTE,
  	nMCTracksaccettabili);
	int ibene=0;
	if(nMCTracksaccettabili>0){
		for(int ii=0; ii<nTotalCandidates;ii++){
			for(i=0;i<nMCTracksaccettabili;i++){
				if( daTrackFoundaTrackMC[ii]==ListaMCTracksaccettabili[i]){
					ibene++;
				}
			}
		}
	}
	if(ibene>0) fprintf(HANDLE,"\tn. volte almeno 1 traccia MC accettabile e' ricostruita %d\n"
		,ibene);

int ii, ibuone=-1;
Double_t HoughFiii;



for (ii=0; ii<nTotalCandidates  ;ii++){
   if(!keepit[ii]) { if(istampa>1) {cout<<"\tevt. n "<<IVOLTE<<", cand. "<<ii<<" ha keepit false."<<
   	endl;}; continue;}
   ibuone++;
   fprintf(HANDLE,"----------------------------------------------------------\n");
   i=daTrackFoundaTrackMC[ii];



   if( i <0  ) {
    fprintf(HANDLE,
"   No TracciaMC associated to found track n. %d in pattern recognition, with %d Hits ||, %d skew hits, %f Radius \n "
         ,ibuone,nSttParHitsinTrack[ii], nSttSkewHitsinTrack[ii],  R[ii] );
            continue;
         }
   if( ( !SttSZfit[ii] )&&(resultFitSZagain[ii] !=1 )) {
    fprintf(HANDLE,
"       TracciaMC %d; sua FoundTrack associata (n. %d) NONsoddisfaRequisitiMinimi perche' in Z-S e' fallito il fit \n"
	,i,ibuone);
            continue;
         }
   if(fabs(KAPPA[ii])<1.e-20 ){
    fprintf(HANDLE,
"       TracciaMC %d; sua FoundTrack associata (n. %d) NONsoddisfaRequisitiMinimi perche' KAPPA troppo piccolo; KAPPA = %g\n",
          i,ibuone,KAPPA[ii]);
           continue;
   }
   	dista=sqrt( Ox[ii]*Ox[ii]+Oy[ii]*Oy[ii] );
   if(fabs(dista)<1.e-20 ){
    fprintf(HANDLE,
"       TracciaMC %d; sua FoundTrack associata (n. %d) NONsoddisfaRequisitiMinimi perche' centro Helix Cilinder trovato dista solo %g da (0,0)\n",
           i,ibuone,dista);
           continue;
   }
   pMCtr = (PndMCTrack*) fMCTrackArray->At(i);
   if ( ! pMCtr ){
		fprintf(HANDLE,
		"       MC track n. %d doesn't have pointer to MC Track TClones Array\n",
		i);
	  continue;
   }

//   controllo che la traccia associata MC sia una delle tracce MC 'ragionevoli'.

	for(int g=0; g<nMCTracksaccettabili;g++){
		if( i==ListaMCTracksaccettabili[g]) goto difuori;
	}
	continue;


difuori: ;

    fprintf(HANDLE,
"       TracciaMC %d ParHitsMC %d ParMecc %d ParMeccSpuri %d SkewHitsMC %d  SkewMecc %d SkewMeccSpuri %d\n",
             i,
             nHitsInMCTrack[ii],
             nParalCommon[ii],
             nSpuriParinTrack[ii],
             nSkewHitsInMCTrack[ii],
             nSkewCommon[ii],
             nSpuriSkewinTrack[ii]

           )  ;
    fprintf(HANDLE,
"       e corrisponde a track found n. %d\n", ibuone );
    fprintf(HANDLE,
"       AVENDO %d hits paralleli e %d hits skew non mecciati dalla corrisponde track found\n"
       , nMCParalAlone[ii],nMCSkewAlone[ii]  );

    HoughFiii =  atan2(Oy[ii],Ox[ii]);
    if(HoughFiii<0.)  HoughFiii += 2.*PI;

         Double_t aaa, carica, Rr, Dd, Fifi, Oxx, Oyy, Cx, Cy, Pxx, Pyy  ;
         Int_t icode;
         icode  = pMCtr->GetPdgCode() ;    //   PDG code of track
         Oxx = pMCtr->GetStartVertex().X();    //   X of starting point track
         Oyy = pMCtr->GetStartVertex().Y();    //   Y of starting point track
         Pxx = pMCtr->GetMomentum().X();
         Pyy = pMCtr->GetMomentum().Y();
         aaa = sqrt( Pxx*Pxx + Pyy*Pyy);
         Rr =   aaa*1000./(BFIELD*CVEL);    //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla
         TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
         TParticlePDG *fParticle= fdbPDG->GetParticle(icode);
         if (icode>1000000000) carica = 1.;
         else  carica = fParticle->Charge()/3. ;    //   charge of track
	 if(fabs(carica)<1.e-5) fprintf(HANDLE,"       MC track n. %d e' neutra, assurdo!\n",i);
         Cx = Oxx + Pyy*1000./(BFIELD*CVEL*carica);
         Cy = Oyy - Pxx*1000./(BFIELD*CVEL*carica);
         Fifi = atan2(Cy, Cx);       // MC truth Fifi angle of circle of Helix trajectory
         if(Fifi<0.)  Fifi += 2.*PI;
         Double_t Kakka ;
         if( fabs( pMCtr->GetMomentum().Z() )< 1.e-20) Kakka = 99999999.;
         else  Kakka = -carica*0.001*BFIELD*CVEL/pMCtr->GetMomentum().Z();

    fprintf(HANDLE,
"       R_MC %g R %g Fi_MC %g Fi %g KAPPA_MC %g KAPPA %g FI0_MC %g FI0 %g\n",
     Rr,
     R[ ii ],
     Fifi,
     HoughFiii,
     Kakka,
     KAPPA[ ii ],
     fmod(Fifi+ PI, 2.*PI),  //  FI0  da MC truth
     FI0[ ii ]
           );
     
//------------------------


//    HoughFi = atan2(Oy[i],Ox[i]);
//    if(HoughFi<0.)  HoughFi += 2.*PI;
    fprintf(HANDLE2,"Evento n. %d Found track %d messa in PndTrackCand",IVOLTE, ii);
    fprintf(HANDLE2,
"       R_MC %g R %g Fi_MC %g Fi %g KAPPA_MC %g KAPPA %g FI0_MC %g FI0 %g\n",
     Rr,
     R[ ii ],
     Fifi,
     HoughFiii,
     Kakka,
     KAPPA[ ii ],
     fmod(Fifi+ PI, 2.*PI),
     FI0[ ii ]
           );

//----------------------------------


  }   //   end of  for (ii=0; ii<nSttTrackCand  ;ii++)
}  // end of if(istampa>=1)

//--------------ghosts

// fa il conto delle ghost solo sugli eventi che hanno almeno 1 traccia MC accettabile.
int NParghost=0, NParhitsghost=0,icc,ibuone=-1;
if( istampa>=1 && nMCTracksaccettabili>0){

    for(icc=0; icc<nTotalCandidates;icc++){
	if(!keepit[icc]) continue;
	ibuone++;
       if( daTrackFoundaTrackMC[icc] == -1){
          NParghost++;
          NParhitsghost += nSttParHitsinTrack[icc]+nSttSkewHitsinTrack[icc];

          fprintf(HANDLE,"          tracce Trovata n. %d e' Ghost\n",ibuone);
       }
    }
    fprintf(HANDLE,
"          tracceGhostTrovate %d TotaleHitsGhost %d  ----\n",
            NParghost,
            NParhitsghost
           );

    fprintf(HANDLE,"----------------------------------------------------------\n");


}  //   end of    if( istampa>=1)

   }	// end of if( nMCTracks >0 && nSttTrackCand > 0  && doMcComparison )

//------------------  end section with comparison MC Mvd hits - associated hits to a certain track



//-------  load the new PndTrackCand ; each track has the STT and the Mvd hits associated
//-------  also load the new PndTrack ; each track has the STT and the Mvd hits associated


	Double_t Oxx, Oyy;
//	Int_t iflaggo;
	for(ncand=0, ipinco = 0; ncand< nTotalCandidates; ncand++){
		if(!keepit[ncand]) continue;
		// case in which there was no Skew hits and no KAPPA info and that
		// candidate could not be associated to any Mvd hits --> no KAPPA information!
		if(ncand<nSttTrackCand && ! SttSZfit[ncand]) continue;
		Oxx = Ox[ncand];
		Oyy = Oy[ncand];
		dis=sqrt( Oxx*Oxx+Oyy*Oyy );
		if( dis < 1.e-20)  continue;
		Ptras = R[ncand]*0.003*BFIELD;
		Pxini = -CHARGE[ncand]*Ptras*Oyy/dis;
		Pyini = CHARGE[ncand]*Ptras*Oxx/dis;

//   starting point not necessarily at x=0., y=0.

		x= Ox[ncand] + R[ncand]*cos(FI0[ncand]);
		y= Oy[ncand] + R[ncand]*sin(FI0[ncand]);


		TVector3 posSeed(x,y,0.);  //  the starting point

		if(fabs(KAPPA[ncand])>1.e-20  ){
			Pzini = -CHARGE[ncand]*0.003*BFIELD/KAPPA[ncand];
			if(fabs(Pzini) > PMAX)  continue;
		} else {
			continue;
		}
		// PndTrackCand Array loading
		new((*fSttMvdPndTrackCandArray)[ipinco])  PndTrackCand;
		PndTrackCand *pTrckCand = (PndTrackCand*) fSttMvdPndTrackCandArray->At(ipinco);
		TVector3 dirSeed(Pxini,Pyini,Pzini); // momentum direction in starting point
		qop = CHARGE[ncand]/dirSeed.Mag();
		dirSeed.SetMag(1.);
		pTrckCand->setTrackSeed(posSeed, dirSeed, qop);
		if(doMcComparison){
			pTrckCand->setMcTrackId(  daTrackFoundaTrackMC[ncand]   );
		}else{
			pTrckCand->setMcTrackId(-1);
		}
//		pTrckCand->sorted=true;


		for(j=0; j< nTrackCandHit[ncand]; j++){
		     switch (ListTrackCandHitType[ncand][j]){
			case 0:
pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId(fMvdPixelBranch),
		(Int_t)ListTrackCandHit[ncand][j],j);
			break;
			case 1:
pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId(fMvdStripBranch),
		(Int_t)ListTrackCandHit[ncand][j],j);
			break;
			case 2:
pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId(fSttBranch),
		(Int_t)ListTrackCandHit[ncand][j],j);
			break;
			case 3:
pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId(fSttBranch),
		(Int_t)ListTrackCandHit[ncand][j],j);
			break;
		     }
		}

		// PndTrack Array loading
		//  last = last hit in found track; first = first hit in found track;
		//  pTrckCand =  pointer to the corresponding  PndTrackCand.

		//  the first hit

		if (ListTrackCandHitType[ncand][0] == 0){  //  Mvd Pixel
			Posiz1[0] = XMvdPixel[ ListTrackCandHit[ncand][0] ];
			Posiz1[1] = YMvdPixel[ ListTrackCandHit[ncand][0] ];
			Posiz1[2] = ZMvdPixel[ ListTrackCandHit[ncand][0] ];
			ErrPosition.SetX(sigmaXMvdPixel[ ListTrackCandHit[ncand][0] ]/sqrt(12.));
			ErrPosition.SetY(sigmaXMvdPixel[ ListTrackCandHit[ncand][0] ]/sqrt(12.));
			ErrPosition.SetZ(sigmaXMvdPixel[ ListTrackCandHit[ncand][0] ]/sqrt(12.));
		} else if (ListTrackCandHitType[ncand][0] == 1){  //  Mvd Strip
			Posiz1[0] = XMvdStrip[ ListTrackCandHit[ncand][0] ];
			Posiz1[1] = YMvdStrip[ ListTrackCandHit[ncand][0] ];
			Posiz1[2] = ZMvdStrip[ ListTrackCandHit[ncand][0] ];
			ErrPosition.SetX(sigmaXMvdStrip[ ListTrackCandHit[ncand][0] ]/sqrt(12.));
			ErrPosition.SetY(sigmaXMvdStrip[ ListTrackCandHit[ncand][0] ]/sqrt(12.));
			ErrPosition.SetZ(sigmaXMvdStrip[ ListTrackCandHit[ncand][0] ]/sqrt(12.));
		} else if( ListTrackCandHitType[ncand][0] == 2 ){  // it is a parallel straw hit
			PndSttInfoXYZParal(
				info,
				ListTrackCandHit[ncand][0],
				Ox[ncand],
				Oy[ncand],
				R[ncand],
				KAPPA[ncand],
				FI0[ncand],
				CHARGE[ncand],
				Posiz1
				);

			// cases in which the calculation of the position failed, see
			// PndSttInfoXYZParal  method.
			if(Posiz1[2]<-888888887. || Posiz1[0] < -999999998.)
				continue;

			ErrPosition.SetX(0.02);	// 200 microns
			ErrPosition.SetY(0.02);	// 200 microns
			ErrPosition.SetZ(1.);		// 1 cm


	} else if ( ListTrackCandHitType[ncand][0] == 3 ){  // it is a skew straw hit

		Posiz1[0] = Ox[ncand]+R[ncand]*cos(SchosenSkew[ncand][ ListTrackCandHit[ncand][0] ]);
		Posiz1[1] = Oy[ncand]+R[ncand]*sin(SchosenSkew[ncand][ ListTrackCandHit[ncand][0] ]);
		Posiz1[2] = ZchosenSkew[ncand][ ListTrackCandHit[ncand][0] ];
		ErrPosition.SetX(0.02);	// 200 microns
		ErrPosition.SetY(0.02);	// 200 microns
		ErrPosition.SetZ(1.);		// 1 cm

		}

		Position.SetX( Posiz1[0] );
		Position.SetY( Posiz1[1] );
		Position.SetZ( Posiz1[2] );
		versor[0] = Ox[ncand]-Posiz1[0];
		versor[1] = Oy[ncand]-Posiz1[1];
		Distance = sqrt(versor[0]*versor[0]+versor[1]*versor[1]);
		versor[0] /= Distance;
		versor[1] /= Distance;
		px = -CHARGE[ncand]*Ptras*versor[1];
		py = CHARGE[ncand]*Ptras*versor[0];
		Momentum.SetX(px);
		Momentum.SetY(py);
		Momentum.SetZ(Pzini);
		ErrMomentum.SetX(0.05*Ptras); //  set at 5% all the times.
		ErrMomentum.SetY(0.05*Ptras); //  set at 5% all the times.
		ErrMomentum.SetZ(0.05*Pzini); //  set at 5% all the times.
		//  the plane of this FairTrackParP better is perpendicular to
		//  the momentum direction
		ddd = Ptras*sqrt(Ptras*Ptras+Pzini*Pzini);

if(istampa>=3){ cout<<" evento = "<<IVOLTE<<", track cand n. "<<ncand<<endl<<
	"\tfirst hit, tipo hit = "<<ListTrackCandHitType[ncand][0]<<", X = "<<Posiz1[0]
	<<", Y = "<<Posiz1[1]<<", Z = "<<Posiz1[2]<<
	"\n\tPx = "<<px<<", Py = "<<py<<", Pz = "<<Pzini<<", Ptras "<<Ptras<<endl
	<<"\t ErrX = "<<ErrPosition[0]
	<<", ErrY = "<<ErrPosition[1]<<", ErrZ = "<<ErrPosition[2]<<endl<<
	"\tErrPx = "<<ErrMomentum.X()<<", ErrPy = "<<ErrMomentum.Y()<<", ErrPz = "
	<<ErrMomentum.Z()<<endl;
	if( ncand<nSttTrackCand ) {if(SttSZfit[ncand])cout<<"SttSZfit = true";
		else cout<<"SttSZfit = false";};
	cout<<",  KAPPA = "<<KAPPA[ncand]<<", FI0 = "<<FI0[ncand]<<", n. hit (original) = "<<
	ListTrackCandHit[ncand][0]<<endl;

}


		FairTrackParP first( Position,  Momentum,
		   ErrPosition, ErrMomentum, CHARGE[ncand],
		   Position,
		   TVector3(py/Ptras, -px/Ptras, 0.), //  first vector defining the plane
		   TVector3(Pzini*px/ddd,Pzini*py/ddd,-Ptras*Ptras/ddd) //second vector defining the plane
						);
		//  the last hit

		k = nTrackCandHit[ncand]-1;
		if (ListTrackCandHitType[ncand][k] == 0){  //  Mvd Pixel
			Posiz1[0] = XMvdPixel[ ListTrackCandHit[ncand][k] ];
			Posiz1[1] = YMvdPixel[ ListTrackCandHit[ncand][k] ];
			Posiz1[2] = ZMvdPixel[ ListTrackCandHit[ncand][k] ];
			ErrPosition.SetX(sigmaXMvdPixel[ ListTrackCandHit[ncand][k] ]/sqrt(12.));
			ErrPosition.SetY(sigmaXMvdPixel[ ListTrackCandHit[ncand][k] ]/sqrt(12.));
			ErrPosition.SetZ(sigmaXMvdPixel[ ListTrackCandHit[ncand][k] ]/sqrt(12.));
		} else if (ListTrackCandHitType[ncand][k] == 1){  //  Mvd Strip
			Posiz1[0] = XMvdStrip[ ListTrackCandHit[ncand][k] ];
			Posiz1[1] = YMvdStrip[ ListTrackCandHit[ncand][k] ];
			Posiz1[2] = ZMvdStrip[ ListTrackCandHit[ncand][k] ];
			ErrPosition.SetX(sigmaXMvdStrip[ ListTrackCandHit[ncand][k] ]/sqrt(12.));
			ErrPosition.SetY(sigmaXMvdStrip[ ListTrackCandHit[ncand][k] ]/sqrt(12.));
			ErrPosition.SetZ(sigmaXMvdStrip[ ListTrackCandHit[ncand][k] ]/sqrt(12.));
		} else if( ListTrackCandHitType[ncand][k] == 2 ){  // it is a parallel straw hit
			PndSttInfoXYZParal (
				info,
				ListTrackCandHit[ncand][k],
				Ox[ncand],
				Oy[ncand],
				R[ncand],
				KAPPA[ncand],
				FI0[ncand],
				CHARGE[ncand],
				Posiz1
				);
			if(Posiz1[2]<-888888887. || Posiz1[0] < -999999998.)
				continue;

			ErrPosition.SetX(0.02);	// 200 microns
			ErrPosition.SetY(0.02);	// 200 microns
			ErrPosition.SetZ(1.);		// 1 cm

		} else if ( ListTrackCandHitType[ncand][k] == 3 ){  // it is a skew straw hit

			Posiz1[0] = Ox[ncand]+R[ncand]*cos(SchosenSkew[ncand][ ListTrackCandHit[ncand][k] ]);
			Posiz1[1] = Oy[ncand]+R[ncand]*sin(SchosenSkew[ncand][ ListTrackCandHit[ncand][k] ]);
			Posiz1[2] = ZchosenSkew[ncand][ ListTrackCandHit[ncand][k] ];
			ErrPosition.SetX(0.02);	// 200 microns
			ErrPosition.SetY(0.02);	// 200 microns
			ErrPosition.SetZ(1.);		// 1 cm

		}

		Position.SetX( Posiz1[0] );
		Position.SetY( Posiz1[1] );
		Position.SetZ( Posiz1[2] );
		versor[0] = Ox[ncand]-Posiz1[0];
		versor[1] = Oy[ncand]-Posiz1[1];
		Distance = sqrt(versor[0]*versor[0]+versor[1]*versor[1]);
		versor[0] /= Distance;
		versor[1] /= Distance;
		px = -CHARGE[ncand]*Ptras*versor[1];
		py = CHARGE[ncand]*Ptras*versor[0];
		Momentum.SetX(px);
		Momentum.SetY(py);
		Momentum.SetZ(Pzini);
		//  the plane of this FairTrackParP better is perpendicular to
		//  the momentum direction
		ddd = Ptras*sqrt(Ptras*Ptras+Pzini*Pzini);

if(istampa>=3){ cout<<" evento = "<<IVOLTE<<", track cand n. "<<ncand<<endl<<
	"\tlast hit, tipo hit = "<<ListTrackCandHitType[ncand][k]
	<<", X = "<<Posiz1[0]
	<<", Y = "<<Posiz1[1]<<", Z = "<<Posiz1[2]<<
	"\n\tPx = "<<px<<", Py = "<<py<<", Pz = "<<Pzini<<", Ptras "<<Ptras<<endl
	<<"\t ErrX = "<<ErrPosition[0]
	<<", ErrY = "<<ErrPosition[1]<<", ErrZ = "<<ErrPosition[2]<<endl<<
	"\tErrPx = "<<ErrMomentum.X()<<", ErrPy = "<<ErrMomentum.Y()<<", ErrPz = "<<ErrMomentum.Z()
	<<endl;
	if( ncand<nSttTrackCand ) {if(SttSZfit[ncand])cout<<"SttSZfit = true";
		else cout<<"SttSZfit = false";};
	cout<<", KAPPA = "<<KAPPA[ncand]<<", FI0 = "<<FI0[ncand]<<", n. hit (original) = "<<
	ListTrackCandHit[ncand][k]<<endl;
}


		FairTrackParP last( Position,  Momentum,
		   ErrPosition, ErrMomentum, CHARGE[ncand],
		   Position,
		   TVector3(py/Ptras, -px/Ptras, 0.), //  first vector defining the plane
		   TVector3(Pzini*px/ddd,Pzini*py/ddd,-Ptras*Ptras/ddd) //second vector defining the plane
						);



		// loading actually the PndTrack
		PndTrack *pTrck = new((*fSttMvdPndTrackArray)[ipinco]) PndTrack(first,last,*pTrckCand);
//		PndTrack *pTrck = (PndTrack*) fSttMvdPndTrackArray->At(ipinco);
		pTrck->SetRefIndex(ipinco);
		pTrck->SetFlag(0);

		ipinco++;
	}	// end of     for(ncand=0, ipinco = 0; ncand< nTotalCandidates; ncand++)




//--------------------- end of load the new PndTrackCand




if(istampa>2){
//for(int it=0; it<nSttTrackCand;it++){
for(int it=0; it<nTotalCandidates;it++){
cout<<"da PndSttMvdTracking, IVOLTE = "<<IVOLTE<<", traccia n. "<<it<<", elenco ordinato degli hits :\n";
for(l =0;l<nMvdPixelHitsinTrack[it]+nMvdStripHitsinTrack[it]
	+nSttParHitsinTrack[it]+nSttSkewHitsinTrack[it];l++){

	if( ListTrackCandHitType[it][l]==0){
		cout<<"\t hit tipo Pixel, X = "<<
			 XMvdPixel[ ListTrackCandHit[it][l] ]<<
			 ", Y = "<<YMvdPixel[ ListTrackCandHit[it][l] ]<<
			 ", dist = "<<sqrt(
			 XMvdPixel[ ListTrackCandHit[it][l] ]*
			 XMvdPixel[ ListTrackCandHit[it][l] ]+
			 YMvdPixel[ ListTrackCandHit[it][l] ]*
			 YMvdPixel[ ListTrackCandHit[it][l] ])<<endl;
	}else if (ListTrackCandHitType[it][l]==1){
		cout<<"\t hit tipo Strip , X = "<<
			 XMvdStrip[ ListTrackCandHit[it][l] ]<<
			 ", Y = "<<YMvdStrip[ ListTrackCandHit[it][l] ]<<
			 ", dist = "<<sqrt(
			 XMvdStrip[ ListTrackCandHit[it][l] ]*
			 XMvdStrip[ ListTrackCandHit[it][l] ]+
			 YMvdStrip[ ListTrackCandHit[it][l] ]*
			 YMvdStrip[ ListTrackCandHit[it][l] ])<<endl;
	}else if (ListTrackCandHitType[it][l]==2){
		cout<<"\t hit tipo Parallelo , X = "<<
			 info[ ListTrackCandHit[it][l] ][0]<<
			 ", Y = "<<info[ ListTrackCandHit[it][l] ][1]<<
			 ", dist = "<<sqrt(
			 info[ ListTrackCandHit[it][l] ][0]*
			 info[ ListTrackCandHit[it][l] ][0]+
			 info[ ListTrackCandHit[it][l] ][1]*
			 info[ ListTrackCandHit[it][l] ][1])<<endl;
	}else if(ListTrackCandHitType[it][l]==3){
		cout<<"\t hit tipo skew , X approssimato = "<<
			 info[ ListTrackCandHit[it][l] ][0]<<
			 ", Y approssimato = "<<info[ ListTrackCandHit[it][l] ][1]<<
			 ", dist = "<<sqrt(
			 info[ ListTrackCandHit[it][l] ][0]*
			 info[ ListTrackCandHit[it][l] ][0]+
			 info[ ListTrackCandHit[it][l] ][1]*
			 info[ ListTrackCandHit[it][l] ][1])<<endl;
	} else {
		cout<<"\tHit di nessun tipo ????\n";
	}
     }
  }
}	// end of    if(istampa>=2)



//------------------------------   plottamenti --------------------------------------------


 if(iplotta && IVOLTE<nmassimo){



// calcolo di S degli eventuali hits SciTil presenti nelle tracce trovate.

	Double_t esseSciTil[MAXTRACKSPEREVENT][nmaxSciTilHits];

	for(  i= 0; i< nTotalCandidates; i++){
		for(j=0;j< nSciTilHitsinTrack[i];j++){
			posx=posizSciTil[ListSciTilHitsinTrack[i][j]][0];
			posy=posizSciTil[ListSciTilHitsinTrack[i][j]][1];
			if( -posx*posy <0. )  SIGN=-1.;
			else  SIGN=1.;
			RR = posx*posx+posy*posy;
			sqrtRR=sqrt(RR);
			intersect = IntersectionCircle_Segment(
			posx,
			posy,
			-RR,
			posx-fabs(0.5*DIMENSIONSCITIL*posy/sqrtRR),
			posx+fabs(0.5*DIMENSIONSCITIL*posy/sqrtRR),
			posy-SIGN*fabs(0.5*DIMENSIONSCITIL*posx/sqrtRR),
			posy+SIGN*fabs(0.5*DIMENSIONSCITIL*posx/sqrtRR),
			Ox[i],
			Oy[i],
			R[i],
			&Nint,  // output
			XintersectionList,  // output
			YintersectionList,  // output
			&ddd  // output
						);
// reject case with no intersection of the SciTil with the circle trajectory.
	if(intersect){
	// calculate S on the lateral face of the Helix.
	if ( Nint==1){	// the majority of the cases
		esseSciTil[i][j] = atan2(YintersectionList[0]-Oy[i],XintersectionList[0]-Ox[i]);
	} else {  // in this case Nint=2 (it should be a very rare case).
		// do an average of the two positions.
		esseSciTil[i][j] = atan2( 0.5*(YintersectionList[0]+YintersectionList[1])-Oy[i],
			0.5*(XintersectionList[0]+XintersectionList[1])-Ox[i]);
	} // end of  if ( Nint==1)
	if ( esseSciTil[i][j]<0.) esseSciTil[i][j] += 2.*PI;



	}  // continuation of if(intersect)

		} // end of for(j....
	}  // end of  for(  i= 0; i< nTotalCandidates; i++)

//------------------

    for(  i= 0, k=-1; i< nTotalCandidates; i++){


	//-------stampe.
if(istampa>=2&&IVOLTE<20){
		cout<<"\n\n---------------------------------------------\n";
		cout<<" evt. n. "<<IVOLTE<<", candidato "<<i
		<<", n hits || "<<nSttParHitsinTrack[i]<<" e loro lista :\n";
		for(int iz=0;iz<nSttParHitsinTrack[i];iz++){
			cout<<"\thit || n. "<<ListSttParHitsinTrack[i][iz]<<endl;
		}
		cout<<"\tn hits // "<<nSttSkewHitsinTrack[i]<<" e loro lista :\n";
		for(int iz=0;iz<nSttSkewHitsinTrack[i];iz++){
			cout<<"\thit || n. "<<ListSttSkewHitsinTrack[i][iz]<<endl;
		}
		if(keepit[i])cout<<"\tkeepit = true\n";
		else cout<<"\tkeepit = false\n";
		cout<<"---------------------------------------------\n";
}
	//--------fine stampe.

	if(!keepit[i]) continue;
	k++;
	int npunti=-1+nSttParHitsinTrack[i]+nSttSkewHitsinTrack[i]+
		nMvdPixelHitsinTrack[i]+nMvdStripHitsinTrack[i]+nSciTilHitsinTrack[i];

		if (ListTrackCandHitType[i][npunti] == 0){  //  Mvd Pixel
			ultimoangolo[i] = atan2( YMvdPixel[ ListTrackCandHit[i][npunti] ]-Oy[i],
						 XMvdPixel[ ListTrackCandHit[i][npunti] ]-Ox[i]);
		} else if (ListTrackCandHitType[i][npunti] == 1){  //  Mvd Strip
			ultimoangolo[i] = atan2( YMvdStrip[ ListTrackCandHit[i][npunti] ]-Oy[i],
						 XMvdStrip[ ListTrackCandHit[i][npunti] ]-Ox[i]);
		} else if( ListTrackCandHitType[i][npunti] == 2 ){  // it is a parallel straw hit
			PndSttInfoXYZParal (
				info,
				ListTrackCandHit[i][npunti],
				Ox[i],
				Oy[i],
				R[i],
				KAPPA[i],
				FI0[i],
				CHARGE[i],
				Posiz1
				);
			ultimoangolo[i] = atan2( Posiz1[1]-Oy[i],Posiz1[0]-Ox[i]);
		} else if ( ListTrackCandHitType[i][npunti] == 3 ){  // it is a skew straw hit

			ultimoangolo[i] = SchosenSkew[i][ ListTrackCandHit[i][npunti] ];
		} else  if ( ListTrackCandHitType[i][npunti] == 1001){  // SciTil hit.
			ultimoangolo[i] =
				atan2(posizSciTil[ListTrackCandHit[i][npunti]][1]-Oy[i],
				posizSciTil[ListTrackCandHit[i][npunti]][0]-Ox[i]);
		}
		if( ultimoangolo[i]<0.) ultimoangolo[i]+= 2.*PI;



	double primo;
	primoangolo[i] = fmod(FI0[i],2.*PI);
	if(CHARGE[i]>0.){
		if( ultimoangolo[i]> primoangolo[i]) ultimoangolo[i]-=2.*PI;
		primo=ultimoangolo[i]*180./PI;
		ultimoangolo[i]=primoangolo[i]*180./PI;
		primoangolo[i]=primo;
		
	}else{
		if( ultimoangolo[i]<primoangolo[i]) ultimoangolo[i]+=2.*PI;
		ultimoangolo[i]=ultimoangolo[i]*180./PI;
		primoangolo[i]=primoangolo[i]*180./PI;
	}


	if( nSttParHitsinTrack[i]+nMvdPixelHitsinTrack[i]+
		nMvdStripHitsinTrack[i]>0 &&  doMcComparison) {

	   for( j=0;j<nMCSkewAlone[i];j++){
		puntator  = (FairMCPoint*) fSttPointArray->At(MCSkewAloneList[i][j]);
	   	MCSkewAloneX[ MCSkewAloneList[i][j] ]=puntator->GetX();
	   	MCSkewAloneY[ MCSkewAloneList[i][j] ]=puntator->GetY();
	   }


if(istampa>1) cout<<"PndSttMvdTracking, evt. "<<IVOLTE<<
";\n\tprima di WriteMacro, n. vero della trackcand "
	<<i<<", n. sequenziale per le macro = "<<k<<endl;
		WriteMacroSttParallelAssociatedHitsandMvdwithMC(
                   Ox[i], Oy[i], R[i],
			primoangolo[i],ultimoangolo[i],
                   nSttParHitsinTrack[i],
		   ListSttParHitsinTrack,
                   info,
                   i,
		k,	// questo si usa solo per il nome della Macro.
		daTrackFoundaTrackMC[i],
		nParalCommon,
		ParalCommonList,
		nSpuriParinTrack,
		ParSpuriList,
		nMCParalAlone,
		MCParalAloneList,

		nMvdPixelHitsinTrack[i],
		ListMvdPixelHitsinTrack,
		nMvdStripHitsinTrack[i],
		ListMvdStripHitsinTrack,

		nMvdPixelCommon[i],
		&MvdPixelCommonList[i][0],
		nMvdPixelSpuriinTrack[i],
		&MvdPixelSpuriList[i][0],
		nMCMvdPixelAlone[i],
		&MCMvdPixelAloneList[i][0],

		nMvdStripCommon[i],
		&MvdStripCommonList[i][0],
		nMvdStripSpuriinTrack[i],
		&MvdStripSpuriList[i][0],
		nMCMvdStripAlone[i],
		&MCMvdStripAloneList[i][0],

		nSttSkewHitsinTrack[i],
		ListSttSkewHitsinTrack,
		&SchosenSkew[i][0],
		nSkewCommon,
		SkewCommonList,
		nMCSkewAlone,
		MCSkewAloneList
						);
	}	// end of  if( nSttParHitsinTrack[i]+nMvdPixelHitsinTrack[i]+
		//			nMvdStripHitsinTrack[i]>0 &&  doMcComparison)

      if(  nSttSkewHitsinTrack[i]+nMvdPixelHitsinTrack[i]+
      	nMvdStripHitsinTrack[i]>0 &&  doMcComparison){
             WriteMacroSkewAssociatedHitswithMC(
                   KAPPA[i],
		   FI0[i],
		    Ox[i],
		     Oy[i],
		      R[i],
		   CHARGE[i],
                   info,
		   WDX,WDY,WDZ,
                   i,
		k,	// questo si usa solo per il nome della Macro.
                   nSttSkewHitsinTrack[i],
                   ListSttSkewHitsinTrack,
                   nSkewCommon[i],
                   SkewCommonList,
                   daTrackFoundaTrackMC[i],
                   nMCSkewAlone[i],
                   MCSkewAloneList,
		   nMvdPixelHitsinTrack,
		   ListMvdPixelHitsinTrack,
		   nMvdStripHitsinTrack,
		   ListMvdStripHitsinTrack,


		nMvdPixelCommon[i],
		&MvdPixelCommonList[i][0],
		nMvdPixelSpuriinTrack[i],
		&MvdPixelSpuriList[i][0],
		nMCMvdPixelAlone[i],
		&MCMvdPixelAloneList[i][0],

		nMvdStripCommon[i],
		&MvdStripCommonList[i][0],
		nMvdStripSpuriinTrack[i],
		&MvdStripSpuriList[i][0],
		nMCMvdStripAlone[i],
		&MCMvdStripAloneList[i][0],
		&esseSciTil[i][0]
			);
      }  //  end of	if(  nSttSkewHitsinTrack[i]+nMvdPixelHitsinTrack[i]+
	//	nMvdStripHitsinTrack[i]>0 &&  doMcComparison)
    }            //   end of   for(  i= 0; i< nSttTrackCand; i++)
i=0;



        WriteMacroParallelHitsGeneral(
                   nSttHit, info,
		   nTotalCandidates,
		   keepit,
		   Ox,Oy,R,
		   FI0,
		   ultimoangolo,
		   primoangolo
					);

/*
	for(int kk=-200; kk<= 150; kk += 50){
		float  time = (float) kk;
		WriteMacroParallelHitsGeneralspecial(
		   time,	// backgound time
                   nSttHit, info,
		   nTotalCandidates,
		   Ox,Oy,R,
		   FI0,
		   ultimoangolo,
		   primoangolo
					);
	}

*/


//     la seguente e' da modificare per includere eventuali hits SciTil mai usati.
        WriteMacroAllHitsRestanti(
		nSttHit,
		nSttParHit,
		nSttSkewHit,
		info,
		keepit,
		nTotalCandidates,
		nTrackCandHit,
		ListTrackCandHit,
		ListTrackCandHitType
					);


    }   //    end of   if(iplotta)

//---------------------  fine plottamenti --------------------------------------------


return;

}


//---------------------- end of   PndSttMvdTracking::Exec






















//--------------------------------  begin of function   PndSttMvdTracking::calculateintersections

 void PndSttMvdTracking::calculateintersections(Double_t Ox,Double_t Oy,
                   Double_t R,Double_t C0x,Double_t C0y,
                   Double_t C0z,Double_t r,Double_t vx,Double_t vy,Double_t vz,
                   Int_t *STATUS, Double_t* POINTS)
{


//-------------------------------------------


  Double_t P1x, P1y, P1z, P2x, P2y, P2z;

  Double_t AAA, DELTA, ax, ay, aaa;




/*
 INPUTS :

  Ox, Oy        = abscissa and ordinate of the center of the circular trajectory of
                  the particle;
  R             = radius of such trajectory;
  C0x, C0y, Coz = x, y, z coordinates of a point belonging to the axis of the
                  skewed straw;
  r  = radius of equidrift of such skewed straw;
  vx, vy, vz    =  versor of the direction along which the skewed straw lies.


 OUTPUTS :

  P1x, P1y, P1z  =  x, y, z coordinates of the point intersection between the
                    particle trajectory circle and the equidrift cylinder of
                    the skewed straw calculated as a function of theta (first
                    solution);
  P2x, P2y, P2z  =  x, y, z coordinates of the point intersection between the
                    particle trajectory circle and the equidrift cylinder of
                    the skewed straw calculated as a function of theta (second
                    solution);

 P1x, P1y, .... , P2z  are stored in the array POINTS[0-5];  the status is stored
 in *STATUS.

*/




//--------------------------------------------------------------------------


// from the resolving formula on page 23 of Gianluigi's notes, setting  r=0.


  ax = C0x - Ox;
  ay = C0y - Oy;
  DELTA = R*R*(vx*vx+vy*vy) - (vx*ay - vy*ax)*(vx*ay - vy*ax);
  AAA = vx*vx+vy*vy;


  if ( DELTA < 0. ) {
      *STATUS=-2;
  } else if (AAA == 0.){
      *STATUS=-3;
  } else if (DELTA == 0.){
      *STATUS=1;
      POINTS[0] = C0x - vx*(vx*ax + vy*ay)/AAA;
      POINTS[1] = C0y - vy*(vx*ax + vy*ay)/AAA;
      POINTS[2] = C0z - vz*(vx*ax + vy*ay)/AAA;
  }else {
      *STATUS=0;
      DELTA = sqrt(DELTA);
      POINTS[0] = C0x - vx*(vx*ax + vy*ay - DELTA)/AAA;
      POINTS[1] = C0y - vy*(vx*ax + vy*ay - DELTA)/AAA;
      POINTS[2] = C0z - vz*(vx*ax + vy*ay - DELTA)/AAA;
      POINTS[3] = C0x - vx*(vx*ax + vy*ay + DELTA)/AAA;
      POINTS[4] = C0y - vy*(vx*ax + vy*ay + DELTA)/AAA;
      POINTS[5] = C0z - vz*(vx*ax + vy*ay + DELTA)/AAA;
  }

   return;


}



//----------end of function PndSttMvdTracking::calculateintersections





//--start of function PndSttMvdTracking::WriteMacroSttParallelAssociatedHitsandMvdwithMC

  void PndSttMvdTracking::WriteMacroSttParallelAssociatedHitsandMvdwithMC(
                   Double_t Ox,Double_t Oy,Double_t R,
		   Double_t primoangolo,
		   Double_t ultimoangolo,
                   UShort_t Nhits,
UShort_t ListHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],
                   Double_t info[][7],
                   UShort_t iTrack,
		Int_t iNome,
		Short_t daSttTrackaMCTrack,
		UShort_t nParalCommon[MAXTRACKSPEREVENT],
		UShort_t ParalCommonList[MAXMCTRACKS][nmaxSttHitsInTrack],
		UShort_t nSpuriParinTrack[MAXTRACKSPEREVENT],
		UShort_t ParSpuriList[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],

		UShort_t nMCParalAlone[MAXTRACKSPEREVENT],
		UShort_t MCParalAloneList[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],

	UShort_t nMvdPixelHitsAssociatedToSttTra,
UShort_t ListPixelHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHitsInTrack],
	UShort_t nMvdStripHitsAssociatedToSttTra,
UShort_t ListStripHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdStripHitsInTrack],

		UShort_t nMvdPixelCommon,
		UShort_t *MvdPixelCommonList,
		UShort_t nMvdPixelSpuriinTrack,
		UShort_t *MvdPixelSpuriList,
		UShort_t nMCMvdPixelAlone,
		UShort_t *MCMvdPixelAloneList,

		UShort_t nMvdStripCommon,
		UShort_t *MvdStripCommonList,
		UShort_t nMvdStripSpuriinTrack,
		UShort_t *MvdStripSpuriList,
		UShort_t nMCMvdStripAlone,
		UShort_t *MCMvdStripAloneList,

		UShort_t nSkewHitsinTrack,
UShort_t ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],
		Double_t *SchosenSkew,
		UShort_t nSkewCommon[MAXTRACKSPEREVENT],
		UShort_t SkewCommonList[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],
		UShort_t nMCSkewAlone[MAXTRACKSPEREVENT],
		UShort_t MCSkewAloneList[MAXMCTRACKS][nmaxSttHitsInTrack]
					)
{

    Int_t i, j, i1, ii, index, Kincl, nlow, nup, STATUS;

    Double_t xmin , xmax, ymin, ymax,
           dx, dy, diff, d1, d2,
           delta, deltax, deltay, deltaz, deltaS,
           factor,
           zmin, zmax, Smin, Smax, S1, S2,
           z1, z2, y1, y2,x1,x2,
           vx1, vy1, vz1, C0x1, C0y1, C0z1,
           aaa, bbb, ccc, angle, minor, major,
           distance, Rx, Ry, LL,
           Aellipsis1, Bellipsis1,fi1,
           fmin, fmax, offset, step,
           SkewInclWithRespectToS, zpos, zpos1, zpos2,
           Tiltdirection1[2],
           zl[200],zu[200],
           POINTS1[6];







//---------- parallel straws Macro now
      char nome[300], nome2[300];
      sprintf(nome,"MacroSttMvdXYwithMCEvent%dT%d", IVOLTE,iNome);
      sprintf(nome2,"%s.C",nome);
      FILE * MACRO = fopen(nome2,"w");
//      fprintf(MACRO,"void %s()\n{\n",nome);
      fprintf(MACRO,"{\n");
      xmin=1.e20;
      xmax=-1.e20;
      ymin=1.e20;
      ymax=-1.e20;


//---- Scitil hits.
       for( ii=0; ii< nSciTilHitsinTrack[iTrack]; ii++) {
            i = ListSciTilHitsinTrack[iTrack][0] ;
            if (posizSciTil[i][0] < xmin)   xmin = posizSciTil[i][0];
            if (posizSciTil[i][0] > xmax)   xmax = posizSciTil[i][0];
            if (posizSciTil[i][1] < ymin)   ymin = posizSciTil[i][1];
            if (posizSciTil[i][1] > ymax)   ymax = posizSciTil[i][1];
       }
//-------------

       for( ii=0; ii< Nhits; ii++) {
            i = ListHitsinTrack[iTrack][ii] ;
            if (info[i][0]-info[i][3] < xmin)   xmin = info[i][0]-info[i][3];
            if (info[i][0]+info[i][3] > xmax)   xmax = info[i][0]+info[i][3];
            if (info[i][1]-info[i][3] < ymin)   ymin = info[i][1]-info[i][3];
            if (info[i][1]+info[i][3] > ymax)   ymax = info[i][1]+info[i][3];
       }
       for( i=0; i< nSkewHitsinTrack; i++) {
            ii = ListSkewHitsinTrack[iTrack][i] ;
	    aaa=Ox+R*cos(SchosenSkew[ii]);
	    bbb=Oy+R*sin(SchosenSkew[ii]);
            if (aaa < xmin)   xmin = aaa;
            if (aaa > xmax)   xmax = aaa;
            if (bbb < ymin)   ymin = bbb;
            if (bbb > ymax)   ymax = bbb;

       }
//       for( ii=0; ii< nMvdStripHit; ii++) {
       for( i=0; i< nMvdStripHitsAssociatedToSttTra; i++) {
        ii = ListStripHitsinTrack[iTrack][i];
            if (XMvdStrip[ii] < xmin)   xmin = XMvdStrip[ii];
            if (XMvdStrip[ii] > xmax)   xmax = XMvdStrip[ii] ;
            if (YMvdStrip[ii] < ymin)   ymin = YMvdStrip[ii];
            if (YMvdStrip[ii] > ymax)   ymax = YMvdStrip[ii];
       }
//       for( ii=0; ii< nMvdPixelHit; ii++) {
       for( i=0; i< nMvdPixelHitsAssociatedToSttTra; i++) {
        ii = ListPixelHitsinTrack[iTrack][i];
            if (XMvdPixel[ii] < xmin)   xmin = XMvdPixel[ii];
            if (XMvdPixel[ii] > xmax)   xmax = XMvdPixel[ii] ;
            if (YMvdPixel[ii] < ymin)   ymin = YMvdPixel[ii];
            if (YMvdPixel[ii] > ymax)   ymax = YMvdPixel[ii];
       }

       for( i=0; i< nMCMvdPixelAlone; i++) {
        ii = MCMvdPixelAloneList[i];
            if (XMvdPixel[ii] < xmin)   xmin = XMvdPixel[ii];
            if (XMvdPixel[ii] > xmax)   xmax = XMvdPixel[ii] ;
            if (YMvdPixel[ii] < ymin)   ymin = YMvdPixel[ii];
            if (YMvdPixel[ii] > ymax)   ymax = YMvdPixel[ii];
       }

       for( i=0; i< nMCMvdStripAlone; i++) {
        ii = MCMvdStripAloneList[i];
            if (XMvdStrip[ii] < xmin)   xmin = XMvdStrip[ii];
            if (XMvdStrip[ii] > xmax)   xmax = XMvdStrip[ii] ;
            if (YMvdStrip[ii] < ymin)   ymin = YMvdStrip[ii];
            if (YMvdStrip[ii] > ymax)   ymax = YMvdStrip[ii];
       }


       for( ii=0; ii< nMCParalAlone[iTrack]; ii++) {
            i = MCParalAloneList[iTrack][ii] ;
	    if( info[i][0] < xmin)   xmin = info[i][0];
	    if( info[i][0] > xmax)   xmax = info[i][0];
	    if( info[i][1] < ymin)   ymin = info[i][1];
	    if( info[i][1] > ymax)   ymax = info[i][1];
       }

       for( ii=0; ii< nMCSkewAlone[iTrack]; ii++) {
            i = MCSkewAloneList[iTrack][ii] ;
	    if( info[i][0] < xmin)   xmin = info[i][0];
	    if( info[i][0] > xmax)   xmax = info[i][0];
	    if( info[i][1] < ymin)   ymin = info[i][1];
	    if( info[i][1] > ymax)   ymax = info[i][1];
       }


       if( xmin > 0. ) xmin = 0.;
       if( xmax < 0.)  xmax = 0.;
       if( ymin > 0. ) ymin = 0.;
       if( ymax < 0.)  ymax = 0.;

       deltax = xmax-xmin;
       deltay = ymax - ymin;

       if( deltax > deltay) {
         ymin -=  0.5*(deltax-deltay);
         ymax = ymin+ deltax;
         delta = deltax;
       }  else  {
         xmin -=  0.5*(deltay-deltax);
         xmax = xmin+ deltay;
         delta= deltay;
       }

       xmax = xmax + delta*0.05;
       xmin = xmin - delta*0.05;

       ymax = ymax + delta*0.05;
       ymin = ymin - delta*0.05;


       fprintf(MACRO,"TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n",xmin,ymin,xmax,ymax);


//	disegna il BiHexagon destro e sinistro delle inner parallel straws.

	char myname[100];

	sprintf(myname, "InnerPar");
	DrawBiHexagonInMacro(
				VERTICALGAP,
				MACRO,
				RStrawDetectorMin,
				ApotemaMaxInnerParStraw,
				4,  // color code, 4= blue.
				myname
				);
//--------------
//	disegna il BiHexagon destro e sinistro delle skew straws.
	sprintf(myname, "Skew");
	DrawBiHexagonInMacro(
				VERTICALGAP,
				MACRO,
				ApotemaMinSkewStraw,
				ApotemaMaxSkewStraw,
				2,  // color code.
				myname
				);
//--------------
//	disegna il BiHexagon destro e sinistro delle skew straws.
	sprintf(myname, "OuterPar");
	DrawHexagonCircleInMacro(
				VERTICALGAP,
				MACRO,
				ApotemaMinOuterParStraw,
				RStrawDetectorMax,
				4,  // color code.
				myname
				);
//--------------

//---- disegna gli Scitil.

	for( ii=0; ii< nSciTilHitsinTrack[iTrack]; ii++) {
		i = ListSciTilHitsinTrack[iTrack][ii] ;
		fprintf(MACRO,"TMarker* SciT%d = new TMarker(%f,%f,%d);\n",
			i,posizSciTil[i][0],posizSciTil[i][1],30);
		fprintf(MACRO,"SciT%d->SetMarkerSize(1.5);\n",i);
		fprintf(MACRO,"SciT%d->SetMarkerColor(1);\nSciT%d->Draw();\n"
				,i,i);
	}
//------------------------
       fprintf(MACRO,"TEllipse* FoundTrack = new TEllipse(%f,%f,%f,%f,%f,%f);\n"
			,Ox,Oy,R,R,primoangolo,ultimoangolo);

       fprintf(MACRO,
       "FoundTrack->SetLineColor(2);\nFoundTrack->SetFillStyle(0);\nFoundTrack->Draw(\"only\");\n");

       fprintf(MACRO,"TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n",xmin,0.,xmax,0.,xmin,xmax);
       fprintf(MACRO,"Assex->Draw();\n");
       fprintf(MACRO,"TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", 0.,ymin,0.,ymax,ymin,ymax);
       fprintf(MACRO,"Assey->Draw();\n");


//------------- hits paralleli in comune con traccia MC
       for( ii=0; ii< nParalCommon[iTrack]; ii++) {
            i = ParalCommonList[iTrack][ii] ;
            fprintf(MACRO,
  "TEllipse* CommonParalHit%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nCommonParalHit%d->SetFillStyle(0);\nCommonParalHit%d->Draw();\n",
                     i,info[i][0],info[i][1],info[i][3],info[i][3],i,i);
       }
//------------- hits paralleli spuri
       for( ii=0; ii< nSpuriParinTrack[iTrack]; ii++) {
            i = ParSpuriList[iTrack][ii] ;
            fprintf(MACRO,
   "TEllipse* SpurParalHit%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nSpurParalHit%d->SetFillStyle(0);\nSpurParalHit%d->SetLineColor(2);\nSpurParalHit%d->Draw();\n",
                     i,info[i][0],info[i][1],info[i][3],info[i][3],i,i,i);
       }
//------------- hits paralleli MC 'alone'
       for( ii=0; ii< nMCParalAlone[iTrack]; ii++) {
            i = MCParalAloneList[iTrack][ii] ;
            fprintf(MACRO,
   "TEllipse* AloneParalHit%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nAloneParalHit%d->SetFillStyle(0);\nAloneParalHit%d->SetLineColor(4);\nAloneParalHit%d->Draw();\n",
                     i,info[i][0],info[i][1],info[i][3],info[i][3],i,i,i);
       }

//-------------








//------------- hits skew in comune e spuri di traccia MC

       for( i=0; i< nSkewHitsinTrack; i++) {
        ii = ListSkewHitsinTrack[iTrack][i];
	    aaa=Ox+R*cos(SchosenSkew[ii]);
	    bbb=Oy+R*sin(SchosenSkew[ii]);

		for( int k=0; k<nSkewCommon[iTrack];k++){
			if( SkewCommonList[iTrack][k]== ii){
	fprintf(MACRO,"TMarker* CommonSkewHit%d = new TMarker(%f,%f,%d);\n",
				ii,aaa,bbb,28);
	fprintf(MACRO,"CommonSkewHit%d->SetMarkerColor(1);\nCommonSkewHit%d->Draw();\n"
				,ii,ii);
				goto punco ;
			}
		}
		fprintf(MACRO,"TMarker* SpurSkewHit%d = new TMarker(%f,%f,%d);\n",
				ii,aaa,bbb,28);
		fprintf(MACRO,"SpurSkewHit%d->SetMarkerColor(2);\nSpurSkewHit%d->Draw();\n",ii,ii);
punco: ;

       }
//------------- hits paralleli MC 'alone'
       for( ii=0; ii< nMCSkewAlone[iTrack]; ii++) {
            i = MCSkewAloneList[iTrack][ii] ;
           fprintf(MACRO,
   "TMarker* AloneSkewHit%d = new TMarker(%f,%f,%d);\nAloneSkewHit%d->SetMarkerColor(4);\nAloneSkewHit%d->Draw();\n",
                     i,MCSkewAloneX[i],MCSkewAloneY[i],28,i,i);
       }

//------------- now the Strips

       for( i=0; i< nMvdStripHitsAssociatedToSttTra; i++) {
        ii = ListStripHitsinTrack[iTrack][i];
            x1= XMvdStrip[ii]-sigmaXMvdStrip[ii];
            x2= XMvdStrip[ii]+sigmaXMvdStrip[ii];
            y1= YMvdStrip[ii]-sigmaYMvdStrip[ii];
            y2= YMvdStrip[ii]+sigmaYMvdStrip[ii];

		for( int k=0; k<nMvdStripCommon;k++){
			if( MvdStripCommonList[k]== ii){
            fprintf(MACRO,"TMarker* CommonMvdStrip%d = new TMarker(%f,%f,%d);\n",
                    ii,XMvdStrip[ii],YMvdStrip[ii],25);
		fprintf(MACRO,"CommonMvdStrip%d->SetMarkerColor(1);\nCommonMvdStrip%d->Draw();\n",
                    ii,ii);
				goto pinco ;
			}
		}
            fprintf(MACRO,"TMarker* SpurMvdStrip%d = new TMarker(%f,%f,%d);\n",
                    ii,XMvdStrip[ii],YMvdStrip[ii],25);
		fprintf(MACRO,"SpurMvdStrip%d->SetMarkerColor(2);\nSpurMvdStrip%d->Draw();\n",ii,ii);
pinco: ;

       }

//-------------- hit Mvd Strips 'Alone'
       for( ii=0; ii< nMCMvdStripAlone; ii++) {
            i = MCMvdStripAloneList[ii] ;
            fprintf(MACRO,
   "TMarker* AloneMvdStrip%d = new TMarker(%f,%f,%d);\nAloneMvdStrip%d->SetMarkerColor(4);\nAloneMvdStrip%d->Draw();\n",
                     i,XMvdStrip[i],YMvdStrip[i],25,i,i);
       }
//-------------


//       for( ii=0; ii< nMvdPixelHit; ii++) {
       for( i=0; i< nMvdPixelHitsAssociatedToSttTra; i++) {
        ii = ListPixelHitsinTrack[iTrack][i];
            x1= XMvdPixel[ii]-sigmaXMvdPixel[ii];
            x2= XMvdPixel[ii]+sigmaXMvdPixel[ii];
            y1= YMvdPixel[ii]-sigmaYMvdPixel[ii];
            y2= YMvdPixel[ii]+sigmaYMvdPixel[ii];
//            fprintf(MACRO,"TBox* BP%d = new TBox(%f,%f,%f,%f);\nBP%d->SetFillColor(4);\nBP%d->Draw();\n",
//                     ii,x1,y1,x2,y2,ii,ii);

		for( int k=0; k<nMvdPixelCommon;k++){
			if( MvdPixelCommonList[k]== ii){
            fprintf(MACRO,"TMarker* CommonMvdPixel%d = new TMarker(%f,%f,%d);\n",
                    ii,XMvdPixel[ii],YMvdPixel[ii],26);
 fprintf(MACRO,"CommonMvdPixel%d->SetMarkerColor(1);\nCommonMvdPixel%d->Draw();\n",
                    ii,ii);
				goto panco ;
			}
		}
            fprintf(MACRO,"TMarker* SpurMvdPixel%d = new TMarker(%f,%f,%d);\n",
                    ii,XMvdPixel[ii],YMvdPixel[ii],26);
 fprintf(MACRO,"SpurMvdPixel%d->SetMarkerColor(2);\nSpurMvdPixel%d->Draw();\n",ii,ii);
panco: ;		

       }

//-------------- hit Mvd Pixel 'Alone'
       for( ii=0; ii< nMCMvdPixelAlone; ii++) {
            i = MCMvdPixelAloneList[ii] ;
            fprintf(MACRO,
   "TMarker* AloneMvdPixel%d = new TMarker(%f,%f,%d);\nAloneMvdPixel%d->SetMarkerColor(4);\nAloneMvdPixel%d->Draw();\n",
                     i,XMvdPixel[i],YMvdPixel[i],26,i,i);
       }
//-------------






//----------------- ora la traccia MC corrispondente a questa traccia Stt
       if( daSttTrackaMCTrack>-1 ) {
	Int_t icode, im;
         Double_t Rr, Dd, Fifi, Oxx, Oyy, Cx, Cy, Px, Py, carica  ;
		PndMCTrack* pMC;
		im=daSttTrackaMCTrack;
		pMC = (PndMCTrack*) fMCTrackArray->At(im);
	if ( pMC ) {
         	icode  = pMC->GetPdgCode() ;    //   PDG code of track
         	Oxx = pMC->GetStartVertex().X();    //   X of starting point track
         	Oyy = pMC->GetStartVertex().Y();    //   Y of starting point track
         	Px = pMC->GetMomentum().X();
         	Py = pMC->GetMomentum().Y();
         	aaa = sqrt( Px*Px + Py*Py);
         	Rr =   aaa*1000./(BFIELD*CVEL);    //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla
         	TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
         	TParticlePDG *fParticle= fdbPDG->GetParticle(icode);
       		if (icode>1000000000) carica = 1.;
       		else  carica = fParticle->Charge()/3. ;    //   charge of track
       if (fabs(carica)<0.1 ) goto fuori ;
           	Cx = Oxx + Py*1000./(BFIELD*CVEL*carica);
           	Cy = Oyy - Px*1000./(BFIELD*CVEL*carica);
            	fprintf(MACRO,
//"TEllipse* MC%d = new TEllipse(%f,%f,%f,%f,%f,%f);\nMC%d->SetFillStyle(0);\nMC%d->SetLineColor(3);\nMC%d->Draw(\"only\");\n",
//                     im,Cx,Cy,Rr,Rr,primoangolo,ultimoangolo,im,im,im);
"TEllipse* MC%d = new TEllipse(%f,%f,%f,%f,%f,%f);\nMC%d->SetFillStyle(0);\nMC%d->SetLineColor(3);\nMC%d->Draw(\"only\");\n",
                     im,Cx,Cy,Rr,Rr,0.,360.,im,im,im);
	}
       };
//----------- fine parte del MC
fuori: ;
      fprintf(MACRO,"}\n");
      fclose(MACRO);


    return ;

}


//--end of function PndSttMvdTracking::WriteMacroSttParallelAssociatedHitsandMvdwithMC



//----------start of function PndSttMvdTracking::WriteMacroParallelHitsGeneral

  void PndSttMvdTracking::WriteMacroParallelHitsGeneral(
                   Int_t Nhits, Double_t info[][7],
                   UShort_t nTracksFoundSoFar,
		bool *keepit,
                   Double_t *Ox, Double_t *Oy, Double_t *Radius,
		   Double_t *FI0,
		   Double_t *ultimoangolo,
		   Double_t *primoangolo



                                                      )
{

    Int_t i, j, i1, ii, index, Kincl, nlow, nup, STATUS;

    Double_t xmin , xmax, ymin, ymax, xl, xu, yl, yu,
           gamma,
           dx, dy, diff, d1, d2,
           delta, deltax, deltay, deltaz, deltaS,
           factor,ff,
           zmin, zmax, Smin, Smax, S1, S2,
           z1, z2, y1, y2,x1,x2,
           vx1, vy1, vz1, C0x1, C0y1, C0z1,
           aaa, bbb, ccc, rrr, angle, minor, major,
           distance, Rx, Ry, LL,
           Aellipsis1, Bellipsis1,fi1,
           fmin, fmax, offset, step,
           SkewInclWithRespectToS, zpos, zpos1, zpos2,
           Tiltdirection1[2],
           zl[200],zu[200],
           POINTS1[6];

      char nome[300], nome2[300];





//---------- parallel straws Macro now
      sprintf(nome,"MacroSttMvdAllHitsEvent%d", IVOLTE);
      sprintf(nome2,"%s.C",nome);
      FILE * MACRO = fopen(nome2,"w");
      fprintf(MACRO,"void %s()\n{\n",nome);
      xmin=1.e20;
      xmax=-1.e20;
      ymin=1.e20;
      ymax=-1.e20;
       for( i=0; i< Nhits; i++) {	// all straws, anche le skew
            if (info[i][0]-info[i][3] < xmin)   xmin = info[i][0]-info[i][3];
            if (info[i][0]+info[i][3] > xmax)   xmax = info[i][0]+info[i][3];
            if (info[i][1]-info[i][3] < ymin)   ymin = info[i][1]-info[i][3];
            if (info[i][1]+info[i][3] > ymax)   ymax = info[i][1]+info[i][3];
       }
       for( ii=0; ii< nMvdPixelHit; ii++) {
            if (XMvdPixel[ii] < xmin)   xmin = XMvdPixel[ii];
            if (XMvdPixel[ii] > xmax)   xmax = XMvdPixel[ii] ;
            if (YMvdPixel[ii] < ymin)   ymin = YMvdPixel[ii];
            if (YMvdPixel[ii] > ymax)   ymax = YMvdPixel[ii];
       }

       for( ii=0; ii< nMvdStripHit; ii++) {
            if (XMvdStrip[ii] < xmin)   xmin = XMvdStrip[ii];
            if (XMvdStrip[ii] > xmax)   xmax = XMvdStrip[ii] ;
            if (YMvdStrip[ii] < ymin)   ymin = YMvdStrip[ii];
            if (YMvdStrip[ii] > ymax)   ymax = YMvdStrip[ii];
       }

	// SciTil hits.
       for( ii=0; ii< nSciTilHits; ii++) {
            if (posizSciTil[ii][0] < xmin)   xmin = posizSciTil[ii][0];
            if (posizSciTil[ii][0] > xmax)   xmax = posizSciTil[ii][0] ;
            if (posizSciTil[ii][1] < ymin)   ymin = posizSciTil[ii][1];
            if (posizSciTil[ii][1] > ymax)   ymax = posizSciTil[ii][1];
       }

//       if( xmin > 0. ) xmin = 0.;
//       if( xmax < 0.)  xmax = 0.;
//       if( ymin > 0. ) ymin = 0.;
//       if( ymax < 0.)  ymax = 0.;

       deltax = xmax-xmin;
       deltay = ymax - ymin;

       if( deltax > deltay) {
         ymin -=  0.5*(deltax-deltay);
         ymax = ymin+ deltax;
         delta = deltax;
       }  else  {
         xmin -=  0.5*(deltay-deltax);
         xmax = xmin+ deltay;
         delta= deltay;
       }

       xmax = xmax + delta*0.05;
       xmin = xmin - delta*0.05;

       ymax = ymax + delta*0.05;
       ymin = ymin - delta*0.05;


	if( xmin>-1.05*RStrawDetectorMax) xmin=-1.05*RStrawDetectorMax;
	if( ymin>-1.05*RStrawDetectorMax) ymin=-1.05*RStrawDetectorMax;
	if( xmax<1.05*RStrawDetectorMax) xmax=1.05*RStrawDetectorMax;
	if( ymax<1.05*RStrawDetectorMax) ymax=1.05*RStrawDetectorMax;

//	ymin=xmin=-1.05*RStrawDetectorMax;
//	ymax=xmax= 1.05*RStrawDetectorMax;


       fprintf(MACRO,"TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n",xmin,ymin,xmax,ymax);


       fprintf(MACRO,"TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n",xmin,0.,xmax,0.,xmin,xmax);
       fprintf(MACRO,"Assex->Draw();\n");
       fprintf(MACRO,"TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", 0.,ymin,0.,ymax,ymin,ymax);
       fprintf(MACRO,"Assey->Draw();\n");

//	disegna il BiHexagon destro e sinistro delle inner parallel straws.
	char myname[100];

	sprintf(myname,"InnerPar");
	DrawBiHexagonInMacro(
				VERTICALGAP,
				MACRO,
				RStrawDetectorMin,
				ApotemaMaxInnerParStraw,
				4,  // color code, 4= blue.
				myname
				);
//--------------
//	disegna il BiHexagon destro e sinistro delle skew straws.
	sprintf(myname,"Skew");
	DrawBiHexagonInMacro(
				VERTICALGAP,
				MACRO,
				ApotemaMinSkewStraw,
				ApotemaMaxSkewStraw,
				2,  // color code.
				myname
				);
//--------------
//	disegna il BiHexagon destro e sinistro delle skew straws.
	sprintf(myname,"OuterPar");
	DrawHexagonCircleInMacro(
				VERTICALGAP,
				MACRO,
				ApotemaMinOuterParStraw,
				RStrawDetectorMax,
				4,  // color code.
				myname
				);
//--------------

       for( i=0; i< Nhits; i++) {
         if( info[i][5] == 1 ) {     // parallel straws
fprintf(MACRO,"TEllipse* Paral%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nParal%d->SetFillStyle(0);\nParal%d->Draw();\n",
                     i,info[i][0],info[i][1],info[i][3],info[i][3],i,i);
          } else {	// skew straws.
fprintf(MACRO,"TMarker* Skew%d = new TMarker(%f,%f,%d);\nSkew%d->SetMarkerColor(1);\nSkew%d->Draw();\n",
		i,info[i][0],info[i][1],28,i,i,i);
	  }
       }

       for( ii=0; ii< nMvdStripHit; ii++) {
            x1= XMvdStrip[ii]-sigmaXMvdStrip[ii];
            x2= XMvdStrip[ii]+sigmaXMvdStrip[ii];
            y1= YMvdStrip[ii]-sigmaYMvdStrip[ii];
            y2= YMvdStrip[ii]+sigmaYMvdStrip[ii];
fprintf(MACRO,
 "TMarker* Strip%d = new TMarker(%f,%f,%d);\nStrip%d->SetMarkerColor(1);\nStrip%d->Draw();\n",
                    ii,XMvdStrip[ii],YMvdStrip[ii],25,ii,ii,ii);


       }
       for( ii=0; ii< nMvdPixelHit; ii++) {
            x1= XMvdPixel[ii]-sigmaXMvdPixel[ii];
            x2= XMvdPixel[ii]+sigmaXMvdPixel[ii];
            y1= YMvdPixel[ii]-sigmaYMvdPixel[ii];
            y2= YMvdPixel[ii]+sigmaYMvdPixel[ii];
fprintf(MACRO,
"TMarker* Pixel%d = new TMarker(%f,%f,%d);\nPixel%d->SetMarkerColor(1);\nPixel%d->Draw();\n",
                    ii,XMvdPixel[ii],YMvdPixel[ii],26,ii,ii);
       }


//---- disegna gli Scitil.


	for( i=0; i< nSciTilHits; i++) {
		fprintf(MACRO,"TMarker* SciT%d = new TMarker(%f,%f,%d);\n",
			i,posizSciTil[i][0],posizSciTil[i][1],30);
		fprintf(MACRO,"SciT%d->SetMarkerSize(1.5);\n",i);
		fprintf(MACRO,"SciT%d->SetMarkerColor(1);\nSciT%d->Draw();\n"
				,i,i);
	}
//------------------------

//-------------------------------   plotting all the tracks found

    for(i=0, ii=0; i<nTracksFoundSoFar; i++){

	if(!keepit[i]) continue;
	ii++;
       aaa = Ox[i];
       bbb = Oy[i];
       rrr = Radius[i];
          fprintf(MACRO,
//"TEllipse* ris%d=new TEllipse(%f,%f,%f,%f,0.,360.);\nris%d->SetFillStyle(0);\nris%d->SetLineColor(2);\nris%d->Draw();\n",
//                     i,aaa,bbb,rrr,rrr,i,i,i);
"TEllipse* ris%d=new TEllipse(%f,%f,%f,%f,%f,%f);\nris%d->SetFillStyle(0);\nris%d->SetLineColor(2);\nris%d->Draw(\"only\");\n",
		ii,aaa,bbb,rrr,rrr,primoangolo[i],ultimoangolo[i],ii,ii,ii);


    }

// -----------


      fprintf(MACRO,"}\n");
      fclose(MACRO);
       
//------------------------------------------------------------------------------------------------------------


//---------- parallel straws Macro now con anche le tracce MC

      sprintf(nome,"MacroSttMvdAllHitswithMCEvent%d", IVOLTE);
      sprintf(nome2,"%s.C",nome);
      MACRO = fopen(nome2,"w");
      fprintf(MACRO,"void %s()\n{\n",nome);


       fprintf(MACRO,"TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n",xmin,ymin,xmax,ymax);


       fprintf(MACRO,"TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n",xmin,0.,xmax,0.,xmin,xmax);
       fprintf(MACRO,"Assex->Draw();\n");
       fprintf(MACRO,"TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", 0.,ymin,0.,ymax,ymin,ymax);
       fprintf(MACRO,"Assey->Draw();\n");

//	disegna il BiHexagon destro e sinistro delle inner parallel straws.
	sprintf(myname,"InnerPar");
	DrawBiHexagonInMacro(
				VERTICALGAP,
				MACRO,
				RStrawDetectorMin,
				ApotemaMaxInnerParStraw,
				4,  // color code, 4= blue.
				myname
				);
//--------------
//	disegna il BiHexagon destro e sinistro delle skew straws.
	sprintf(myname,"Skew");
	DrawBiHexagonInMacro(
				VERTICALGAP,
				MACRO,
				ApotemaMinSkewStraw,
				ApotemaMaxSkewStraw,
				2,  // color code.
				myname
				);
//--------------
//	disegna il BiHexagon destro e sinistro delle skew straws.
	sprintf(myname,"OuterPar");
	DrawHexagonCircleInMacro(
				VERTICALGAP,
				MACRO,
				ApotemaMinOuterParStraw,
				RStrawDetectorMax,
				4,  // color code.
				myname
				);
//--------------

       for( i=0; i< Nhits; i++) {
         if( info[i][5] == 1 ) {     // parallel straws
fprintf(MACRO,"TEllipse* Paral%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nParal%d->SetFillStyle(0);\nParal%d->Draw();\n",
                     i,info[i][0],info[i][1],info[i][3],info[i][3],i,i);
          } else {	// skew straws.
fprintf(MACRO,"TMarker* Skew%d = new TMarker(%f,%f,%d);\nSkew%d->SetMarkerColor(1);\nSkew%d->Draw();\n",
		i,info[i][0],info[i][1],28,i,i,i);
	  }
       }

       for( ii=0; ii< nMvdStripHit; ii++) {
            x1= XMvdStrip[ii]-sigmaXMvdStrip[ii];
            x2= XMvdStrip[ii]+sigmaXMvdStrip[ii];
            y1= YMvdStrip[ii]-sigmaYMvdStrip[ii];
            y2= YMvdStrip[ii]+sigmaYMvdStrip[ii];
fprintf(MACRO,
 "TMarker* Strip%d = new TMarker(%f,%f,%d);\nStrip%d->SetMarkerColor(1);\nStrip%d->Draw();\n",
                    ii,XMvdStrip[ii],YMvdStrip[ii],25,ii,ii,ii);


       }
       for( ii=0; ii< nMvdPixelHit; ii++) {
            x1= XMvdPixel[ii]-sigmaXMvdPixel[ii];
            x2= XMvdPixel[ii]+sigmaXMvdPixel[ii];
            y1= YMvdPixel[ii]-sigmaYMvdPixel[ii];
            y2= YMvdPixel[ii]+sigmaYMvdPixel[ii];
//            fprintf(MACRO,"TBox* BP%d = new TBox(%f,%f,%f,%f);\nBP%d->SetFillColor(4);\nBP%d->Draw();\n",
//                     ii,x1,y1,x2,y2,ii,ii);
fprintf(MACRO,
"TMarker* Pixel%d = new TMarker(%f,%f,%d);\nPixel%d->SetMarkerColor(1);\nPixel%d->Draw();\n",
                    ii,XMvdPixel[ii],YMvdPixel[ii],26,ii,ii);
       }

//---- disegna gli Scitil.


	for( i=0; i< nSciTilHits; i++) {
		fprintf(MACRO,"TMarker* SciT%d = new TMarker(%f,%f,%d);\n",
			i,posizSciTil[i][0],posizSciTil[i][1],30);
		fprintf(MACRO,"SciT%d->SetMarkerSize(1.5);\n",i);
		fprintf(MACRO,"SciT%d->SetMarkerColor(1);\nSciT%d->Draw();\n"
				,i,i);
	}
//------------------------




//-------------------------------   plotting all the tracks found
    for(i=0, ii=0; i<nTracksFoundSoFar; i++){

	if(!keepit[i]) continue;
	ii++;
       aaa = Ox[i];
       bbb = Oy[i];
       rrr = Radius[i];
          fprintf(MACRO,
//"TEllipse* ris%d=new TEllipse(%f,%f,%f,%f,0.,360.);\nris%d->SetFillStyle(0);\nris%d->SetLineColor(2);\nris%d->Draw();\n",
//                     i,aaa,bbb,rrr,rrr,i,i,i);
"TEllipse* ris%d=new TEllipse(%f,%f,%f,%f,%f,%f);\nris%d->SetFillStyle(0);\nris%d->SetLineColor(2);\nris%d->Draw(\"only\");\n",
		ii,aaa,bbb,rrr,rrr,primoangolo[i],ultimoangolo[i],ii,ii,ii);


    }


// -----------
//----------------- ora le traccia MC
  for(i=0; i<nMCTracks;i++) {
	Int_t icode;
		Double_t alfa0, newalfa, newx, newy,primo, ultimo ;
         Double_t Rr, Dd, Fifi, Oxx, Oyy, Cx, Cy, Px, Py, carica  ;
		PndMCTrack* pMC;
		pMC = (PndMCTrack*) fMCTrackArray->At(i);
         	TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
	if ( pMC ) {
         	icode  = pMC->GetPdgCode() ;    //   PDG code of track
         	TParticlePDG *fParticle= fdbPDG->GetParticle(icode);
       		if (icode>1000000000) carica = 1.;
       		else  carica = fParticle->Charge()/3. ;    //   charge of track
         		Oxx = pMC->GetStartVertex().X();    //   X of starting point track
         		Oyy = pMC->GetStartVertex().Y();    //   Y of starting point track
         		Px = pMC->GetMomentum().X();
         		Py = pMC->GetMomentum().Y();
         		aaa = sqrt( Px*Px + Py*Py);
		if( fabs(carica) > 0.1 ){
         		Rr =   aaa*1000./(BFIELD*CVEL); //R(cm) of Helix of track projected in XY plane;
							// B = 2 Tesla
           		Cx = Oxx + Py*1000./(BFIELD*CVEL*carica);
           		Cy = Oyy - Px*1000./(BFIELD*CVEL*carica);

	//  calcolo per plottare  solo la parte rilevante della traccia MC.
		primo=alfa0 = atan2(Oyy-Cy, Oxx-Cx);
		for(j=0;j<90;j++){
			newalfa = alfa0 - carica*j*PI/45;
			newx = Cx + Rr*cos(newalfa);
			newy = Cy + Rr*sin(newalfa);
			if(newx > xmax || newx < xmin || newy>ymax||newy<ymin){
				ultimo = newalfa;
				if(primo > ultimo ) { primo = ultimo; ultimo = alfa0;};
				goto pippo ;
			}
		}
	primo   = 0.;
	ultimo = 2.*PI;
pippo:	;

			fprintf(MACRO,
"TEllipse* MC%d = new TEllipse(%f,%f,%f,%f,%f,%f);\nMC%d->SetFillStyle(0);\nMC%d->SetLineColor(3);\nMC%d->Draw(\"only\");\n",
			i,Cx,Cy,Rr,Rr,primo*180./PI,ultimo*180./PI,i,i,i);


		} else { // continuation of  if( fabs(carica) > 0.1 )
			continue;   //  per ora non plotto le neutre.
			// this is a neutral particle (green dashed straight line).
			if( Oxx < xmin || Oxx > xmax || Oyy < ymin || Oyy > ymax) continue;
			double time, time1, time2;
			if(fabs(Px) >1.e-10 ){
				time2 = (xmax-Oxx)/Px;
				time1 = (xmin-Oxx)/Px;
				if(time1<0. && time2<0.) {
					continue;
				}else if(time2>0.&&time1<0.){
					time = time2;
					x2 = xmax;
				}else if (time1>0. && time2<0.) {
					time = time1;
					x2 = xmin;
				} else{
					if( time2>time1 ) {
						time=time2;
						x2 = xmax;
					} else {
						time=time1;
						x2 = xmin;
					}
				}
				y2 = time*Py + Oyy;
				fprintf(MACRO,"TLine* MCneut%d = new TLine(%f,%f,%f,%f);\n",
					i,Oxx,Oyy,x2,y2);
				fprintf(MACRO,"MCneut%d->SetLineStyle(2);\n",i);
				fprintf(MACRO,"MCneut%d->SetLineColor(3);\nMCneut%d->Draw(\"only\");\n"
						,i,i);
			} else if( fabs(Px) >1.e-10 ) {
				if( fabs(Py) >1.e-10 ) continue;
				time2 = (ymax-Oyy)/Py;
				time1 = (ymin-Oyy)/Py;
				if(time1<0. && time2<0.) {
					continue;
				}else if(time2>0.&&time1<0.){
					y2 = ymax;
				}else if (time1>0. && time2<0.) {
					y2 = ymin;
				} else{
					y2= time2>time1 ? ymax : ymin ;
				}
				fprintf(MACRO,"TLine* MCneut%d = new TLine(%f,%f,%f,%f);\n",
					i,Oxx,Oyy,Oxx,y2);
				fprintf(MACRO,"MCneut%d->SetLineStyle(2);\n",i);
				fprintf(MACRO,"MCneut%d->SetLineColor(3);\nMCneut%d->Draw(\"only\");\n"
						,i,i);
			}


		}  // end of  if( fabs(carica) > 0.1 )




	}	// end of  if ( pMC )



  }   //    end of for(i=0; i<nMCTracks;i++)
//----------- fine parte del MC

      fprintf(MACRO,"}\n");
      fclose(MACRO);
       
//------------------------------------------------------------------------------------------------------------




    return ;

}


//----------end of function PndSttMvdTracking::WriteMacroParallelHitsGeneral












//----------start of function PndSttMvdTracking::WriteMacroParallelHitsGeneralspecial

  void PndSttMvdTracking::WriteMacroParallelHitsGeneralspecial(
		   Double_t time,
                   Int_t Nhits, Double_t info[][7],
                   UShort_t nTracksFoundSoFar,
                   Double_t *Ox, Double_t *Oy, Double_t *Radius,
		   Double_t *FI0,
		   Double_t *ultimoangolo,
		   Double_t *primoangolo



                                                      )
{

    Int_t i, j, i1, ii, index, Kincl, nlow, nup, STATUS;

    Double_t xmin , xmax, ymin, ymax, xl, xu, yl, yu,
           gamma,
           dx, dy, diff, d1, d2,
           delta, deltax, deltay, deltaz, deltaS,
           factor,ff,
           zmin, zmax, Smin, Smax, S1, S2,
           z1, z2, y1, y2,x1,x2,
           vx1, vy1, vz1, C0x1, C0y1, C0z1,
           aaa, bbb, ccc, rrr, angle, minor, major,
           distance, Rx, Ry, LL, dR,
           Aellipsis1, Bellipsis1,fi1,
           fmin, fmax, offset, step,
           SkewInclWithRespectToS, zpos, zpos1, zpos2,
           Tiltdirection1[2],
           zl[200],zu[200],
           POINTS1[6];

      char nome[300], nome2[300];



	dR = time*STTdriftVEL; // in cm.

//---------- parallel straws Macro now
	if( time>=0.){
		i = (Int_t) time;
		sprintf(nome,"MacroSttMvdAllHitsTime%dEvent%d",i, IVOLTE);
	} else {
		i = (Int_t) -time;
		sprintf(nome,"MacroSttMvdAllHitsTime-%dEvent%d",i, IVOLTE);
	}
      sprintf(nome2,"%s.C",nome);
      FILE * MACRO = fopen(nome2,"w");
//      fprintf(MACRO,"void %s()\n{\n",nome);
      fprintf(MACRO,"{\n");
      xmin=1.e20;
      xmax=-1.e20;
      ymin=1.e20;
      ymax=-1.e20;
       for( i=0; i< Nhits; i++) {	// all straws, anche le skew
            if (info[i][0]-info[i][3] < xmin)   xmin = info[i][0]-info[i][3];
            if (info[i][0]+info[i][3] > xmax)   xmax = info[i][0]+info[i][3];
            if (info[i][1]-info[i][3] < ymin)   ymin = info[i][1]-info[i][3];
            if (info[i][1]+info[i][3] > ymax)   ymax = info[i][1]+info[i][3];
       }
       for( ii=0; ii< nMvdPixelHit; ii++) {
            if (XMvdPixel[ii] < xmin)   xmin = XMvdPixel[ii];
            if (XMvdPixel[ii] > xmax)   xmax = XMvdPixel[ii] ;
            if (YMvdPixel[ii] < ymin)   ymin = YMvdPixel[ii];
            if (YMvdPixel[ii] > ymax)   ymax = YMvdPixel[ii];
       }

       for( ii=0; ii< nMvdStripHit; ii++) {
            if (XMvdStrip[ii] < xmin)   xmin = XMvdStrip[ii];
            if (XMvdStrip[ii] > xmax)   xmax = XMvdStrip[ii] ;
            if (YMvdStrip[ii] < ymin)   ymin = YMvdStrip[ii];
            if (YMvdStrip[ii] > ymax)   ymax = YMvdStrip[ii];
       }

       if( xmin > 0. ) xmin = 0.;
       if( xmax < 0.)  xmax = 0.;
       if( ymin > 0. ) ymin = 0.;
       if( ymax < 0.)  ymax = 0.;

       deltax = xmax-xmin;
       deltay = ymax - ymin;

       if( deltax > deltay) {
         ymin -=  0.5*(deltax-deltay);
         ymax = ymin+ deltax;
         delta = deltax;
       }  else  {
         xmin -=  0.5*(deltay-deltax);
         xmax = xmin+ deltay;
         delta= deltay;
       }

       xmax = xmax + delta*0.05;
       xmin = xmin - delta*0.05;

       ymax = ymax + delta*0.05;
       ymin = ymin - delta*0.05;


       fprintf(MACRO,"TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n",xmin,ymin,xmax,ymax);
	if( time>=0.){
		i = (Int_t) time;
		fprintf(MACRO,"TPaveLabel* pt= new TPaveLabel(%f,%f,%f,%f,\"Bkg; Time shift = %d nsec\");\n",
		xmin+delta*0.05,ymax-delta*0.15,xmin+delta*0.35,ymax- delta*0.05,i);
	} else {
		i = (Int_t) -time;
		fprintf(MACRO,"TPaveLabel* pt= new TPaveLabel(%f,%f,%f,%f,\"Bkg; Time shift = -%d nsec\");\n",
		xmin+delta*0.05,ymax-delta*0.15,xmin+delta*0.35,ymax- delta*0.05,i);
	}
       fprintf(MACRO,"pt->Draw();\n");

       fprintf(MACRO,"TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n",xmin,0.,xmax,0.,xmin,xmax);
       fprintf(MACRO,"Assex->Draw();\n");
       fprintf(MACRO,"TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", 0.,ymin,0.,ymax,ymin,ymax);
       fprintf(MACRO,"Assey->Draw();\n");


       for( i=0; i< Nhits; i++) {
	 if( info[i][3]+dR >STRAWRADIUS || info[i][3]+dR <0.) continue;
         if( info[i][5] == 1 ) {     // parallel straws
fprintf(MACRO,
"TEllipse* Paral%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nParal%d->SetFillStyle(0);\nParal%d->Draw();\n",
                     i,info[i][0],info[i][1],info[i][3]+dR,info[i][3]+dR,i,i);
          }
       }

      if(fabs(time)<5.){
       for( ii=0; ii< nMvdStripHit; ii++) {
            x1= XMvdStrip[ii]-sigmaXMvdStrip[ii];
            x2= XMvdStrip[ii]+sigmaXMvdStrip[ii];
            y1= YMvdStrip[ii]-sigmaYMvdStrip[ii];
            y2= YMvdStrip[ii]+sigmaYMvdStrip[ii];
fprintf(MACRO,
"TMarker* Strip%d = new TMarker(%f,%f,%d);\nStrip%d->SetMarkerColor(1);\nStrip%d->Draw();\n",
                    ii,XMvdStrip[ii],YMvdStrip[ii],25,ii,ii,ii);


       }
       for( ii=0; ii< nMvdPixelHit; ii++) {
            x1= XMvdPixel[ii]-sigmaXMvdPixel[ii];
            x2= XMvdPixel[ii]+sigmaXMvdPixel[ii];
            y1= YMvdPixel[ii]-sigmaYMvdPixel[ii];
            y2= YMvdPixel[ii]+sigmaYMvdPixel[ii];
            fprintf(MACRO,
"TMarker* Pixel%d = new TMarker(%f,%f,%d);\nPixel%d->SetMarkerColor(1);\nPixel%d->Draw();\n",
                    ii,XMvdPixel[ii],YMvdPixel[ii],26,ii,ii);
       }

      } // end of if(fabs(time)>5.)



      fprintf(MACRO,"}\n");
      fclose(MACRO);
       
//------------------------------------------------------------------------------------------------------------





    return ;

}


//----------end of function PndSttMvdTracking::WriteMacroParallelHitsGeneralspecial








//----------start of function PndSttMvdTracking::WriteMacroSkewAssociatedHitswithMC


  void PndSttMvdTracking::WriteMacroSkewAssociatedHitswithMC(
                   Double_t KAPPA,
                   Double_t FI0,
                   Double_t Ox,
                   Double_t Oy,
                   Double_t R,
		   Short_t charge,
                   Double_t info[][7],

                   Double_t WDX[nmaxSttHits],
                   Double_t WDY[nmaxSttHits],
                   Double_t WDZ[nmaxSttHits],

                   UShort_t iTrack,
		    Int_t iNome, // questo e' per il nome delle Macro solamente.
                   UShort_t nSkewHitsinTrack,
UShort_t ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],
                   UShort_t nSkewCommon,
                   UShort_t SkewCommonList[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],
                   Short_t daTrackFoundaTrackMC,
                   UShort_t nMCSkewAlone,
                   UShort_t MCSkewAloneList[MAXMCTRACKS][nmaxSttHitsInTrack],
		   UShort_t nPixelHitsinTrack[MAXTRACKSPEREVENT], // output
UShort_t ListPixelHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHitsInTrack], // output
		   UShort_t nStripHitsinTrack[MAXTRACKSPEREVENT], // output
UShort_t ListStripHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdStripHitsInTrack], // output



		UShort_t nMvdPixelCommon,
		UShort_t *MvdPixelCommonList,
		UShort_t nMvdPixelSpuriinTrack,
		UShort_t *MvdPixelSpuriList,
		UShort_t nMCMvdPixelAlone,
		UShort_t *MCMvdPixelAloneList,

		UShort_t nMvdStripCommon,
		UShort_t *MvdStripCommonList,
		UShort_t nMvdStripSpuriinTrack,
		UShort_t *MvdStripSpuriList,
		UShort_t nMCMvdStripAlone,
		UShort_t *MCMvdStripAloneList,

		Double_t *ESSE

						)
 {


	TDatabasePDG *fdbPDG= TDatabasePDG::Instance();

    Int_t i, j, i1, ii, iii, index, Kincl, nlow, nup, STATUS, imc, Nmin, Nmax;

    Double_t xmin , xmax, ymin, ymax,
           dx, dy, diff, d1, d2,
           delta, deltax, deltay, deltaz, deltaS,
           esse,factor,
           zmin, zmax, zmin2, zmax2, Smin, Smax, S1, S2,
           z1, z2, y1, y2,
           vx1, vy1, vz1, C0x1, C0y1, C0z1,
           aaa, bbb, ccc, angle, minor, major,
           distance, Rx, Ry, LL,
           Aellipsis1, Bellipsis1,fi1,
           fmin, fmax, offset, step,
           SkewInclWithRespectToS, zpos, zpos1, zpos2,
	   dist[2],
           Tiltdirection1[2],
           zl[200],zu[200],
           POINTS1[6];


//-------------------  skew straws hits Macro now

      char  nome2[300],nome[300];
      FILE *MACRO;
      sprintf(nome,  "MacroSttMvdSZwithMCEvent%dT%d", IVOLTE,iNome);
      sprintf(nome2,  "%s.C",nome);
      MACRO = fopen(nome2,"w");
      fprintf(MACRO,"{\n");

//KAPPA = 1./166.67 ;  FI0 = 1.5*PI;

      index=0;

//--------------- ricerca del minimo e massimo.

 if( nSciTilHitsinTrack[iTrack]+nSkewHitsinTrack+
	nPixelHitsinTrack[iTrack]+
	nStripHitsinTrack[iTrack] == 1) // solo 1 punto da disegnare, in questo caso aggiunge
 {					// un punto finto in  (0, FI0 ).
	Smax=Smin= FI0;
	zmax =zmin =0.;
 } else {
      Smin=zmin = 1.e10;
      Smax=zmax = -zmin;
 }

//---------------------




// prima lo (gli) hits SciTil; assumo al massimo 1 SciTil per traccia;
//  quindi nSciTilHits[iTrack] puo' essere 0 o 1.

 for(i=0; i<nSciTilHitsinTrack[iTrack];i++){
		if( ESSE[i]>Smax ) Smax=ESSE[i];
		if( ESSE[i]<Smin ) Smin=ESSE[i];
		if( posizSciTil[iTrack][2]>zmax ) zmax=posizSciTil[iTrack][2];
		if( posizSciTil[iTrack][2]<zmin ) zmin=posizSciTil[iTrack][2];
 fprintf(MACRO,"TMarker* SciT%d = new TMarker(%f,%f,30);\n",
	i,posizSciTil[iTrack][2],ESSE[i]*R);
 fprintf(MACRO,"SciT%d->SetMarkerSize(1.5);\n",i);
 fprintf(MACRO,"SciT%d->SetMarkerColor(1);\n",i);
	}
//-------------------------


       for( iii=0; iii< nSkewHitsinTrack; iii++) {
         i = ListSkewHitsinTrack[iTrack][iii] ;
         aaa = sqrt(WDX[i]*WDX[i]+WDY[i]*WDY[i]+ WDZ[i]*WDZ[i]);
         vx1 = WDX[i]/aaa;
         vy1 = WDY[i]/aaa;
         vz1 = WDZ[i]/aaa;
         C0x1 = info[i][0];
         C0y1 = info[i][1];
         C0z1 = info[i][2];

       calculateintersections(Ox,Oy,R,C0x1,C0y1,C0z1,info[i][3],
                              vx1,vy1,vz1,
                              &STATUS,POINTS1);
       if(STATUS < 0 ) continue ;
       for( ii=0; ii<2; ii++){
        j=3*ii;
        distance = sqrt(
                  (POINTS1[j]-C0x1)*(POINTS1[j]-C0x1) + 
                  (POINTS1[1+j]-C0y1)*(POINTS1[1+j]-C0y1) + 
                  (POINTS1[2+j]-C0z1)*(POINTS1[2+j]-C0z1) 
                            );
        Rx = POINTS1[j]-Ox ;   //  x component Radial vector of cylinder of trajectory
        Ry = POINTS1[1+j]-Oy ;   //  y direction Radial vector of cylinder of trajectory

        aaa = sqrt(Rx*Rx+Ry*Ry);
        SkewInclWithRespectToS = (-Ry*vx1 + Rx*vy1)/aaa ;
        SkewInclWithRespectToS /= R;
        bbb = sqrt( SkewInclWithRespectToS*SkewInclWithRespectToS + vz1*vz1);
        //  the tilt direction of this ellipse is (1,0)  when major axis along Z direction

        LL = fabs(vx1*Rx + vy1*Ry);
        if( LL < 1.e-10) continue;
        Aellipsis1 = info[i][3]*aaa/LL;

        Bellipsis1 = info[i][3]/R;

	if( distance >= info[i][4] + Aellipsis1) continue;


//--------------------------


        fi1 = atan2(POINTS1[j+1]-Oy, POINTS1[j]-Ox) ;  // atan2 returns radians in (-pi and +pi]
        if( fi1 < 0.) fi1 += 2.*PI;

        if( zmin > POINTS1[j+2] - Aellipsis1 ) zmin = POINTS1[j+2] - Aellipsis1;
        if( zmax < POINTS1[j+2] + Aellipsis1 ) zmax = POINTS1[j+2] + Aellipsis1;

        if( Smin > fi1 - Bellipsis1 ) Smin = fi1 - Bellipsis1;
        if( Smax < fi1 + Bellipsis1 ) Smax = fi1 + Bellipsis1;



   }    //  end of    for( ii=0; ii<2; ii++)

  }   //   end of  for( iii=0; iii< nSkewHitsinTrack; iii++)

//------ aggiungo in blu eventuali punti della traccia MC che sono non mecciati

       for( iii=0; iii< nMCSkewAlone; iii++) {
         i = MCSkewAloneList[iTrack][iii];
         aaa = sqrt(WDX[i]*WDX[i]+WDY[i]*WDY[i]+ WDZ[i]*WDZ[i]);
         vx1 = WDX[i]/aaa;
         vy1 = WDY[i]/aaa;
         vz1 = WDZ[i]/aaa;
         C0x1 = info[i][0];
         C0y1 = info[i][1];
         C0z1 = info[i][2];
       calculateintersections(Ox,Oy,R,C0x1,C0y1,C0z1,info[i][3],
                              vx1,vy1,vz1,
                              &STATUS,POINTS1);

       if(STATUS < 0 ) continue ;

       for( ii=0; ii<2; ii++){
        j=3*ii;
        dist[ii] = sqrt(
                  (POINTS1[j]-C0x1)*(POINTS1[j]-C0x1) + 
                  (POINTS1[1+j]-C0y1)*(POINTS1[1+j]-C0y1) + 
                  (POINTS1[2+j]-C0z1)*(POINTS1[2+j]-C0z1) 
                            );
	}
	if(dist[0]>dist[1]) ii=1; else ii=0;

	  distance = dist[ii];

//       for( ii=0; ii<2; ii++){
        j=3*ii;
        distance = sqrt(
                  (POINTS1[j]-C0x1)*(POINTS1[j]-C0x1) + 
                  (POINTS1[1+j]-C0y1)*(POINTS1[1+j]-C0y1) + 
                  (POINTS1[2+j]-C0z1)*(POINTS1[2+j]-C0z1) 
                            );

//        if( distance >= info[i][4]*1.2 ) continue;


        Rx = POINTS1[j]-Ox ;   //  x component Radial vector of cylinder of trajectory
        Ry = POINTS1[1+j]-Oy ;   //  y direction Radial vector of cylinder of trajectory

        aaa = sqrt(Rx*Rx+Ry*Ry);
        SkewInclWithRespectToS = (-Ry*vx1 + Rx*vy1)/aaa ;
        SkewInclWithRespectToS /= R;
        bbb = sqrt( SkewInclWithRespectToS*SkewInclWithRespectToS + vz1*vz1);
        //  the tilt direction of this ellipse is (1,0)  when major axis along Z direction

        LL = fabs(vx1*Rx + vy1*Ry);
        if( LL < 1.e-10) continue;
        Aellipsis1 = info[i][3]*aaa/LL;

        Bellipsis1 = info[i][3]/R;
//--------------------------
        fi1 = atan2(POINTS1[j+1]-Oy, POINTS1[j]-Ox) ;  // atan2 returns radians in (-pi and +pi]
        if( fi1 < 0.) fi1 += 2.*PI;

        if( zmin > POINTS1[j+2] - Aellipsis1 ) zmin = POINTS1[j+2] - Aellipsis1;
        if( zmax < POINTS1[j+2] + Aellipsis1 ) zmax = POINTS1[j+2] + Aellipsis1;

        if( Smin > fi1 - Bellipsis1 ) Smin = fi1 - Bellipsis1;
        if( Smax < fi1 + Bellipsis1 ) Smax = fi1 + Bellipsis1;


//   }    //  end of    for( ii=0; ii<2; ii++)

  }   //   end of  for( iii=0; iii< nMCSkewAlone[imaxima]; iii++)

//-------------------------------
//------ fine aggiunta in blu eventuali punti della traccia MC che sono non mecciati

//   ora la parte delle Mvd

//   prima i pixel

  for(i=0; i<nPixelHitsinTrack[iTrack];i++){
        ii=ListPixelHitsinTrack[iTrack][i];

	if( zmin > ZMvdPixel[ ii ] )
	    zmin = ZMvdPixel[ ii ];
        if( zmax <  ZMvdPixel[ii ] )
	    zmax = ZMvdPixel[ii ];

	esse = atan2( YMvdPixel[ ii ]-Oy,
	              XMvdPixel[ ii ]-Ox);
	if(esse<0.) esse +=2.*PI;



        if( Smin > esse ) Smin = esse;
        if( Smax < esse ) Smax = esse;
  }

//   poi le strip

  for(i=0; i<nStripHitsinTrack[iTrack];i++){
        ii=ListStripHitsinTrack[iTrack][i];
	if( zmin > ZMvdStrip[ ii ] )
	    zmin = ZMvdStrip[ ii ];
        if( zmax <  ZMvdStrip[ii ] )
	    zmax = ZMvdStrip[ii ];

	esse = atan2( YMvdStrip[ ii ]-Oy,
	              XMvdStrip[ ii ]-Ox);
	if(esse<0.) esse +=2.*PI;



        if( Smin > esse ) Smin = esse;
        if( Smax < esse ) Smax = esse;
  }


//   ora i pixel 'Alone'
  for(i=0; i<nMCMvdPixelAlone;i++){

        ii=MCMvdPixelAloneList[i];
	if( zmin > ZMvdPixel[ ii ] )
	    zmin = ZMvdPixel[ ii ];
        if( zmax <  ZMvdPixel[ii ] )
	    zmax = ZMvdPixel[ii ];

	esse = atan2( YMvdPixel[ ii ]-Oy,
	              XMvdPixel[ ii ]-Ox);
	if(esse<0.) esse +=2.*PI;
        if( Smin > esse ) Smin = esse;
        if( Smax < esse ) Smax = esse;
  }


//   ora le strip 'Alone'

  for(i=0; i<nMCMvdStripAlone;i++){
        ii=MCMvdStripAloneList[i];
	if( zmin > ZMvdStrip[ ii ] )
	    zmin = ZMvdStrip[ ii ];
        if( zmax <  ZMvdStrip[ii ] )
	    zmax = ZMvdStrip[ii ];

	esse = atan2( YMvdStrip[ ii ]-Oy,
	              XMvdStrip[ ii ]-Ox);
	if(esse<0.) esse +=2.*PI;
        if( Smin > esse ) Smin = esse;
        if( Smax < esse ) Smax = esse;
  }






  if( zmax < zmin ) goto nohits ;
  if( Smax < Smin ) goto nohits;
  aaa = Smax-Smin;
  Smin -= aaa*0.2;
  Smax += aaa*0.2;

  aaa = zmax-zmin;
  zmin -= aaa*0.05;
  zmax += aaa*0.05;

  if(Smax > 2.*PI) Smax = 2.*PI;
  if( Smin < 0.) Smin = 0.;

//  Smin -= 10.;
//  Smax += 10.;

   deltaz = zmax-zmin;
   deltaS = Smax-Smin;


  fprintf(MACRO,"TCanvas* my= new  TCanvas();\nmy->Range(%f,%f,%f,%f);\n",
  	zmin-0.1*deltaz,R*(Smin-.1*deltaS),zmax+0.1*deltaz,R*(Smax+0.1*deltaS));
//  fprintf(MACRO,"TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n",zmin,Smin,zmax,Smax);
  for( ii=0; ii< index; ii++) {
       fprintf(MACRO,"E%d->Draw();\n",ii);
  }
   fprintf(MACRO,"TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n",
//        zmin+0.05*deltaz,Smin+0.05*deltaS,zmax-0.05*deltaz,Smin+0.05*deltaS,zmin+0.05*deltaz,zmax-0.05*deltaz);
        zmin-0.01*deltaz,R*(Smin+0.05*deltaS),zmax+0.01*deltaz,R*(Smin+0.05*deltaS),
	zmin-0.01*deltaz,zmax+0.01*deltaz);
   fprintf(MACRO,"Assex->SetTitle(\"Z (cm)\");\n");
   fprintf(MACRO,"Assex->Draw();\n");
   fprintf(MACRO,"TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n",
       zmin+0.05*deltaz,R*(Smin-0.01*deltaS),zmin+0.05*deltaz,R*(Smax+0.01*deltaS),
		R*(Smin-0.01*deltaS),R*(Smax+0.01*deltaS));
   fprintf(MACRO,"Assey->SetTitle(\"Helix crf (cm)\");\n");
   fprintf(MACRO,"Assey->Draw();\n");


//------------
//  plot di eventuali hits  SciTil;
	for(i=0;i<nSciTilHitsinTrack[iTrack];i++){
		fprintf(MACRO,"SciT%d->Draw();\n",i);
	}
//------------
// --------------------------------



       for( iii=0; iii< nSkewHitsinTrack; iii++) {
         i = ListSkewHitsinTrack[iTrack][iii] ;

         aaa = sqrt(WDX[i]*WDX[i]+WDY[i]*WDY[i]+ WDZ[i]*WDZ[i]);
         vx1 = WDX[i]/aaa;
         vy1 = WDY[i]/aaa;
         vz1 = WDZ[i]/aaa;
         C0x1 = info[i][0];
         C0y1 = info[i][1];
         C0z1 = info[i][2];

       calculateintersections(Ox,Oy,R,C0x1,C0y1,C0z1,info[i][3],
                              vx1,vy1,vz1,
                              &STATUS,POINTS1);
       if(STATUS < 0 ) continue ;
       for( ii=0; ii<2; ii++){
        j=3*ii;
        distance = sqrt(
                  (POINTS1[j]-C0x1)*(POINTS1[j]-C0x1) + 
                  (POINTS1[1+j]-C0y1)*(POINTS1[1+j]-C0y1) + 
                  (POINTS1[2+j]-C0z1)*(POINTS1[2+j]-C0z1) 
                            );
        Rx = POINTS1[j]-Ox ;   //  x component Radial vector of cylinder of trajectory
        Ry = POINTS1[1+j]-Oy ;   //  y direction Radial vector of cylinder of trajectory

        aaa = sqrt(Rx*Rx+Ry*Ry);
        SkewInclWithRespectToS = (-Ry*vx1 + Rx*vy1)/aaa ;
        SkewInclWithRespectToS /= R;
        bbb = sqrt( SkewInclWithRespectToS*SkewInclWithRespectToS + vz1*vz1);
        //  the tilt direction of this ellipse is (1,0)  when major axis along Z direction
        if( bbb > 1.e-10){
           Tiltdirection1[0] = vz1/bbb;
           Tiltdirection1[1] = SkewInclWithRespectToS/bbb;
        } else {
           Tiltdirection1[0] = 1.;
           Tiltdirection1[1] = 0.;
        }

        LL = fabs(vx1*Rx + vy1*Ry);
        if( LL < 1.e-10) continue;
        Aellipsis1 = info[i][3]*aaa/LL;

        Bellipsis1 = info[i][3]/R;

        if( distance >= info[i][4]  + Aellipsis1) continue;


//--------------------------


        fi1 = atan2(POINTS1[j+1]-Oy, POINTS1[j]-Ox) ;  // atan2 returns radians in (-pi and +pi]
        if( fi1 < 0.) fi1 += 2.*PI;

        if( zmin > POINTS1[j+2] - Aellipsis1 ) zmin = POINTS1[j+2] - Aellipsis1;
        if( zmax < POINTS1[j+2] + Aellipsis1 ) zmax = POINTS1[j+2] + Aellipsis1;

        if( Smin > fi1 - Bellipsis1 ) Smin = fi1 - Bellipsis1;
        if( Smax < fi1 + Bellipsis1 ) Smax = fi1 + Bellipsis1;


        Double_t rotation1 = 180.*atan2(Tiltdirection1[1],Tiltdirection1[0])/PI;
        fprintf(MACRO,"TEllipse* Skew%d_%d = new TEllipse(%f,%f,%f,%f,0.,360.,%f);\nSkew%d_%d->SetFillStyle(0);\n",
//                    i,ii,POINTS1[j+2],fi1,Aellipsis1,Bellipsis1,rotation1,i,ii);
                    i,ii,POINTS1[j+2],R*fi1,Aellipsis1,R*Bellipsis1,rotation1,i,ii);





// ------ se lo hit e' spurio marcalo in rosso
        for( i1=0; i1<nSkewCommon; i1++){
          if ( SkewCommonList[   iTrack   ][i1] == i ){

                goto fuori ;
          }

        }
        fprintf(MACRO,"Skew%d_%d->SetLineColor(2);\n",i,ii);
fuori: ;
        fprintf(MACRO,"Skew%d_%d->Draw();\n",i,ii);






        index++;

   }    //  end of    for( ii=0; ii<2; ii++)

  }   //   end of  for( iii=0; iii< nSkewHitsinTrack; iii++)








//------ aggiungo in blu eventuali punti della traccia MC che sono non mecciati

       for( iii=0; iii< nMCSkewAlone; iii++) {
         i = MCSkewAloneList[iTrack][iii];
         aaa = sqrt(WDX[i]*WDX[i]+WDY[i]*WDY[i]+ WDZ[i]*WDZ[i]);
         vx1 = WDX[i]/aaa;
         vy1 = WDY[i]/aaa;
         vz1 = WDZ[i]/aaa;


         C0x1 = info[i][0];
         C0y1 = info[i][1];
         C0z1 = info[i][2];
       calculateintersections(Ox,Oy,R,C0x1,C0y1,C0z1,info[i][3],
                              vx1,vy1,vz1,
                              &STATUS,POINTS1);

       if(STATUS < 0 ) continue ;


       for( ii=0; ii<2; ii++){
        j=3*ii;
        dist[ii] = sqrt(
                  (POINTS1[j]-C0x1)*(POINTS1[j]-C0x1) + 
                  (POINTS1[1+j]-C0y1)*(POINTS1[1+j]-C0y1) + 
                  (POINTS1[2+j]-C0z1)*(POINTS1[2+j]-C0z1) 
                            );
	}
	if(dist[0]>dist[1]) ii=1; else ii=0;

	  distance = dist[ii];


        j=3*ii;

        Rx = POINTS1[j]-Ox ;   //  x component Radial vector of cylinder of trajectory
        Ry = POINTS1[1+j]-Oy ;   //  y direction Radial vector of cylinder of trajectory

        aaa = sqrt(Rx*Rx+Ry*Ry);
        SkewInclWithRespectToS = (-Ry*vx1 + Rx*vy1)/aaa ;
        SkewInclWithRespectToS /= R;
        bbb = sqrt( SkewInclWithRespectToS*SkewInclWithRespectToS + vz1*vz1);
        //  the tilt direction of this ellipse is (1,0)  when major axis along Z direction
        if( bbb > 1.e-10){
           Tiltdirection1[0] = vz1/bbb;
           Tiltdirection1[1] = SkewInclWithRespectToS/bbb;
        } else {
           Tiltdirection1[0] = 1.;
           Tiltdirection1[1] = 0.;
        }

        LL = fabs(vx1*Rx + vy1*Ry);
        if( LL < 1.e-10) continue;
        Aellipsis1 = info[i][3]*aaa/LL;

        Bellipsis1 = info[i][3]/R;
//--------------------------
        fi1 = atan2(POINTS1[j+1]-Oy, POINTS1[j]-Ox) ;  // atan2 returns radians in (-pi and +pi]
        if( fi1 < 0.) fi1 += 2.*PI;

        if( zmin > POINTS1[j+2] - Aellipsis1 ) zmin = POINTS1[j+2] - Aellipsis1;
        if( zmax < POINTS1[j+2] + Aellipsis1 ) zmax = POINTS1[j+2] + Aellipsis1;

        if( Smin > fi1 - Bellipsis1 ) Smin = fi1 - Bellipsis1;
        if( Smax < fi1 + Bellipsis1 ) Smax = fi1 + Bellipsis1;


        Double_t rotation1 = 180.*atan2(Tiltdirection1[1],Tiltdirection1[0])/PI;
        fprintf(MACRO,"TEllipse* AloneSkew%d_%d = new TEllipse(%f,%f,%f,%f,0.,360.,%f);\nAloneSkew%d_%d->SetFillStyle(0);\n",
//                     i,ii,POINTS1[j+2],fi1,Aellipsis1,Bellipsis1,rotation1,i,ii);
                     i,ii,POINTS1[j+2],R*fi1,Aellipsis1,R*Bellipsis1,rotation1,i,ii);

// ------  marca lo hit in blu
        fprintf(MACRO,"AloneSkew%d_%d->SetLineColor(4);\n",i,ii);
        fprintf(MACRO,"AloneSkew%d_%d->Draw();\n",i,ii);

        index++;


  }   //   end of  for( iii=0; iii< nMCSkewAlone; iii++)

//-------------------------------
//------ fine aggiunta in blu eventuali punti della traccia MC che sono non mecciati

//   ora la parte delle Mvd

//   prima i pixel

  for(i=0; i<nPixelHitsinTrack[iTrack];i++){
        ii=ListPixelHitsinTrack[iTrack][i];

	if( zmin > ZMvdPixel[ ii ] )
	    zmin = ZMvdPixel[ ii ];
        if( zmax <  ZMvdPixel[ii ] )
	    zmax = ZMvdPixel[ii ];

	esse = atan2( YMvdPixel[ ii ]-Oy,
	              XMvdPixel[ ii ]-Ox);
	if(esse<0.) esse +=2.*PI;



        if( Smin > esse ) Smin = esse;
        if( Smax < esse ) Smax = esse;
 

		for( int k=0; k<nMvdPixelCommon;k++){
			if( MvdPixelCommonList[k]== ii){
				fprintf(MACRO,
		"TMarker* CommonPixel%d = new TMarker(%f,%f,%d);\nCommonPixel%d->SetMarkerColor(1);\n",
//				ii,ZMvdPixel[ii],esse,26,ii);
				ii,ZMvdPixel[ii],R*esse,26,ii);
		fprintf(MACRO,"CommonPixel%d->Draw();\n",ii);
				goto punco ;
			}
		}
            fprintf(MACRO,"TMarker* SpuriousPixel%d = new TMarker(%f,%f,%d);\nSpuriousPixel%d->SetMarkerColor(2);\n",
//                    ii,ZMvdPixel[ii],esse,26,ii);
                    ii,ZMvdPixel[ii],R*esse,26,ii);
		fprintf(MACRO,"SpuriousPixel%d->Draw();\n",ii);
punco: ;
  }
//   poi le strip

  for(i=0; i<nStripHitsinTrack[iTrack];i++){
        ii=ListStripHitsinTrack[iTrack][i];
	if( zmin > ZMvdStrip[ ii ] )
	    zmin = ZMvdStrip[ ii ];
        if( zmax <  ZMvdStrip[ii ] )
	    zmax = ZMvdStrip[ii ];

	esse = atan2( YMvdStrip[ ii ]-Oy,
	              XMvdStrip[ ii ]-Ox);
	if(esse<0.) esse +=2.*PI;



        if( Smin > esse ) Smin = esse;
        if( Smax < esse ) Smax = esse;
 
		for( int k=0; k<nMvdStripCommon;k++){
			if( MvdStripCommonList[k]== ii){
            fprintf(MACRO,"TMarker* CommonStrip%d = new TMarker(%f,%f,%d);\nCommonStrip%d->SetMarkerColor(1);\n",
//                    ii,ZMvdStrip[ii],esse,25,ii);
                    ii,ZMvdStrip[ii],R*esse,25,ii);
		fprintf(MACRO,"CommonStrip%d->Draw();\n",ii);
				goto ponco ;
			}
		}

            fprintf(MACRO,"TMarker* SpuriousStrip%d = new TMarker(%f,%f,%d);\nSpuriousStrip%d->SetMarkerColor(2);\n",
//                    ii,ZMvdStrip[ii],esse,25,ii);
                    ii,ZMvdStrip[ii],R*esse,25,ii);
		fprintf(MACRO,"SpuriousStrip%d->Draw();\n",ii);


ponco: ;

  }


//   ora i pixel 'Alone'
  for(i=0; i<nMCMvdPixelAlone;i++){

        ii=MCMvdPixelAloneList[i];
	if( zmin > ZMvdPixel[ ii ] )
	    zmin = ZMvdPixel[ ii ];
        if( zmax <  ZMvdPixel[ii ] )
	    zmax = ZMvdPixel[ii ];

	esse = atan2( YMvdPixel[ ii ]-Oy,
	              XMvdPixel[ ii ]-Ox);
	if(esse<0.) esse +=2.*PI;
        if( Smin > esse ) Smin = esse;
        if( Smax < esse ) Smax = esse;
           fprintf(MACRO,"TMarker* AlonePixel%d = new TMarker(%f,%f,%d);\nAlonePixel%d->SetMarkerColor(4);\n",
//                    ii,ZMvdPixel[ii],esse,26,ii);
                    ii,ZMvdPixel[ii],R*esse,26,ii);
		fprintf(MACRO,"AlonePixel%d->Draw();\n",ii);
  }


//   ora le strip 'Alone'

  for(i=0; i<nMCMvdStripAlone;i++){
        ii=MCMvdStripAloneList[i];
	if( zmin > ZMvdStrip[ ii ] )
	    zmin = ZMvdStrip[ ii ];
        if( zmax <  ZMvdStrip[ii ] )
	    zmax = ZMvdStrip[ii ];

	esse = atan2( YMvdStrip[ ii ]-Oy,
	              XMvdStrip[ ii ]-Ox);
	if(esse<0.) esse +=2.*PI;
        if( Smin > esse ) Smin = esse;
        if( Smax < esse ) Smax = esse;
 
            fprintf(MACRO,"TMarker* AloneStrip%d = new TMarker(%f,%f,%d);\nAloneStrip%d->SetMarkerColor(4);\n",
//                    ii,ZMvdStrip[ii],esse,25,ii);
                    ii,ZMvdStrip[ii],R*esse,25,ii);
		fprintf(MACRO,"AloneStrip%d->Draw();\n",ii);

  }




//  plot della traccia trovata dal finder


	zmin2=zmin;
	zmax2=zmax;

	if( -KAPPA*charge>0.) {	// Pz>0.
		if( zmax <0.) {
			cout<<"da WriteMacroSkewAssociatedHitswithMC, questa traccia"
			<<" e' inconsistente col proprio Pz, non plottata!\n";
			goto dopp ;
		}
		zmin = 0.;
	} else {  // Pz<0.
		if( zmin >0.) {
			cout<<"da WriteMacroSkewAssociatedHitswithMC, questa traccia"
			<<" e' inconsistente col proprio Pz, non plottata!\n";
			goto dopp ;
		}
		zmax = 0.;

	}


  if ( KAPPA >= 0.) {
     fmin = KAPPA*zmin + FI0;
     fmax = KAPPA*zmax + FI0;
  }  else {
     fmax = KAPPA*zmin + FI0;
     fmin = KAPPA*zmax + FI0;
  }
  if( fmax>=0.) {
    Nmax = (int) (0.5*fmax/ PI);
  }  else  {
    Nmax = ( (int) (0.5*fmax/ PI) ) -1;
  }
  if( fmin>=0.) {
    Nmin = (int) (0.5*fmin/ PI);
  } else {
    Nmin = ((int) (0.5*fmin/ PI) )-1;
  }
   if(fabs(KAPPA)<1.e-10) {
   	cout<<"da WriteMacroSkewAssociatedHitswithMC, questa traccia Found da PR non plottata"
	<<" perche' ha fabs(KAPPA)<1.e-10.\n";
	goto dopp ;
   }
  for(i=Nmin; i<= Nmax;i++){
   offset = 2.*PI*i;
   z1 = (i*2.*PI-FI0)/KAPPA;
   z2 = ((i+1)*2.*PI-FI0)/KAPPA;
   fprintf(MACRO,"TLine* FOUND%d = new TLine(%f,%f,%f,%f);\nFOUND%d->SetLineColor(2);\nFOUND%d->Draw();\n",
//                 i-Nmin,z1,0.,z2, 2.*PI,i-Nmin,i-Nmin);
                 i-Nmin,z1,0.,z2, R*2.*PI,i-Nmin,i-Nmin);

  }   //  end of  for(i=Nmin; i<= Nmax;++)

dopp: ;

	zmin=zmin2;
	zmax=zmax2;


//----------------- ora la traccia MC corrispondente a questa traccia Stt

// for(imc=0; imc<nMCTracks ; imc++){
//            if(imc !=    daTrackFoundaTrackMC[ iTrack ]) continue;
     imc=    daTrackFoundaTrackMC ;

       if( imc>-1 ) {
	Int_t icode ;
         Double_t Rr, Dd, Fifi, Kakka, Oxx, Oyy, Cx, Cy, Px, Py, carica  ;
		PndMCTrack* pMC;
		pMC = (PndMCTrack*) fMCTrackArray->At(imc);
	if ( pMC ) {
         	icode  = pMC->GetPdgCode() ;    //   PDG code of track
         	Oxx = pMC->GetStartVertex().X();    //   X of starting point track
         	Oyy = pMC->GetStartVertex().Y();    //   Y of starting point track
         	Px = pMC->GetMomentum().X();
         	Py = pMC->GetMomentum().Y();
         	aaa = sqrt( Px*Px + Py*Py);
         	Rr =   aaa*1000./(BFIELD*CVEL);    //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla
         	TParticlePDG *fParticle= fdbPDG->GetParticle(icode);
       		if (icode>1000000000) carica = 1.;
       		else  carica = fParticle->Charge()/3. ;    //   charge of track
       if (fabs(carica)<0.1 ) goto pinco ;
           	Cx = Oxx + Py*1000./(BFIELD*CVEL*carica);
           	Cy = Oyy - Px*1000./(BFIELD*CVEL*carica);
		Fifi = atan2(Cy, Cx);       // MC truth Fifi angle of circle of Helix trajectory
		if(Fifi<0.)  Fifi += 2.*PI;
		if( fabs( pMC->GetMomentum().Z() )< 1.e-20) Kakka = 99999999.;
		else  Kakka = -carica*0.001*BFIELD*CVEL/pMC->GetMomentum().Z();
     KAPPA=Kakka;
     FI0 = fmod(Fifi+ PI, 2.*PI);
  if ( KAPPA >= 0.) {
     fmin = KAPPA*zmin + FI0;
     fmax = KAPPA*zmax + FI0;
  }  else {
     fmax = KAPPA*zmin + FI0;
     fmin = KAPPA*zmax + FI0;
  }
  if( fmax>=0.) {
    Nmax = (int) (0.5*fmax/ PI);
  }  else  {
    Nmax = ( (int) (0.5*fmax/ PI) ) -1;
  }
  if( fmin>=0.) {
    Nmin = (int) (0.5*fmin/ PI);
  } else {
    Nmin = ((int) (0.5*fmin/ PI) )-1;
  }

  for(i=Nmin; i<= Nmax;i++){
   offset = 2.*PI*i;
   z1 = (i*2.*PI-FI0)/KAPPA;
   z2 = ((i+1)*2.*PI-FI0)/KAPPA;
   fprintf(MACRO,"TLine* MC%d_%d = new TLine(%f,%f,%f,%f);\nMC%d_%d->SetLineColor(3);\nMC%d_%d->Draw();\n",
//                 imc,i-Nmin,z1,0.,z2, 2.*PI,imc,i-Nmin,imc,i-Nmin);
                 imc,i-Nmin,z1,0.,z2,R* 2.*PI,imc,i-Nmin,imc,i-Nmin);
  }   //  end of  for(i=Nmin; i<= Nmax;++)
	}  // end of if ( pMC )
       }  // end of if( imc>-1 )







pinco: ;
nohits: ;

      fprintf(MACRO,"}\n");
      fclose(MACRO);




 }


//----------end of function PndSttMvdTracking::WriteMacroSkewAssociatedHitswithMC



//----------begin of function PndSttMvdTracking::WriteMacroAllHitsRestanti


    void PndSttMvdTracking::WriteMacroAllHitsRestanti(
		UShort_t nSttHit,
		UShort_t nSttParHit,
		UShort_t nSttSkewHit,
		Double_t info[][7],
		bool *keepit,
		UShort_t nSttTrackCand,
		UShort_t nCandHit[MAXTRACKSPEREVENT],
		UShort_t ListCandHit[MAXTRACKSPEREVENT][nmaxSttHitsInTrack+
	                           nmaxMvdPixelHitsInTrack+
				   nmaxMvdStripHitsInTrack+nmaxSciTilHitsinTrack],
	Short_t ListCandHitType[MAXTRACKSPEREVENT][nmaxSttHitsInTrack+
	                           nmaxMvdPixelHitsInTrack+
				   nmaxMvdStripHitsInTrack+nmaxSciTilHitsinTrack]
					)
 {


//	nSttHit = parallel+skew.

	bool	exclusionStt[nmaxSttHits],
		exclusionPixel[nmaxMvdPixelHits],
		exclusionStrip[nmaxMvdStripHits];

	char	nome[300],
		nome2[300];

	int i,j,k;


	Double_t	delta,
			deltax,
			deltay,
			xmin,
			xmax,
			ymin,
			ymax;

	for(i=0;i<nSttHit;i++){
		exclusionStt[i]=false;
	}
	for(i=0;i<nMvdPixelHit;i++){
		exclusionPixel[i]=false;
	}
	for(i=0;i<nMvdStripHit;i++){
		exclusionStrip[i]=false;
	}



	for(i=0;i<nSttTrackCand;i++){
		if(!keepit[i]) continue;

		for(j=0;j<nCandHit[i];j++){

		     switch (ListCandHitType[i][j]){
			case 0:
			   exclusionPixel[ ListCandHit[i][j] ] = true;
			   break;
			case 1:
			   exclusionStrip[ ListCandHit[i][j] ] = true;
			   break;
			default:
			   exclusionStt[ ListCandHit[i][j] ] = true;
			   break;
		     }
		}	// end of  for(j=0;j<nCandHit[i];j++)
	}	//  end of  for(i=0;i<nSttTrackCand;i++)



//	determina il boundary del plot tenendo conto di TUTTI gli hits.
      xmin=1.e20;
      xmax=-1.e20;
      ymin=1.e20;
      ymax=-1.e20;
       for( i=0; i< nSttHit; i++) {
            if (info[i][0]-info[i][3] < xmin)   xmin = info[i][0]-info[i][3];
            if (info[i][0]+info[i][3] > xmax)   xmax = info[i][0]+info[i][3];
            if (info[i][1]-info[i][3] < ymin)   ymin = info[i][1]-info[i][3];
            if (info[i][1]+info[i][3] > ymax)   ymax = info[i][1]+info[i][3];
          }
       for( i=0; i< nMvdPixelHit; i++) {
            if (XMvdPixel[i] < xmin)   xmin = XMvdPixel[i];
            if (XMvdPixel[i] > xmax)   xmax = XMvdPixel[i];
            if (YMvdPixel[i] < ymin)   ymin = YMvdPixel[i];
            if (YMvdPixel[i] > ymax)   ymax = YMvdPixel[i];
          }
       for( i=0; i< nMvdStripHit; i++) {
            if (XMvdStrip[i] < xmin)   xmin = XMvdStrip[i];
            if (XMvdStrip[i] > xmax)   xmax = XMvdStrip[i];
            if (YMvdStrip[i] < ymin)   ymin = YMvdStrip[i];
            if (YMvdStrip[i] > ymax)   ymax = YMvdStrip[i];
          }
       for( i=0; i< nSciTilHits; i++) {
            if (posizSciTil[i][0] < xmin)   xmin = posizSciTil[i][0];
            if (posizSciTil[i][0] > xmax)   xmax = posizSciTil[i][0];
            if (posizSciTil[i][1] < ymin)   ymin = posizSciTil[i][1];
            if (posizSciTil[i][1] > ymax)   ymax = posizSciTil[i][1];
          }

       if( xmin > 0. ) xmin = 0.;
       if( xmax < 0.)  xmax = 0.;
       if( ymin > 0. ) ymin = 0.;
       if( ymax < 0.)  ymax = 0.;

       deltax = xmax-xmin;
       deltay = ymax - ymin;


       if( deltax > deltay) {
         ymin -=  0.5*(deltax-deltay);
         ymax = ymin+ deltax;
         delta = deltax;
       }  else  {
         xmin -=  0.5*(deltay-deltax);
         xmax = xmin+ deltay;
         delta= deltay;
       }

       xmax = xmax + delta*0.05;
       xmin = xmin - delta*0.05;

       ymax = ymax + delta*0.05;
       ymin = ymin - delta*0.05;


      sprintf(nome,"MacroSttMvdHitsRestantiEvent%d", IVOLTE);
      sprintf(nome2,"%s.C",nome);
      FILE * MACRO = fopen(nome2,"w");
      fprintf(MACRO,"void %s()\n{\n",nome);
      fprintf(MACRO,"TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n",xmin,ymin,xmax,ymax);
       fprintf(MACRO,"TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n",xmin,0.,xmax,0.,xmin,xmax);
       fprintf(MACRO,"Assex->Draw();\n");
       fprintf(MACRO,"TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", 0.,ymin,0.,ymax,ymin,ymax);
       fprintf(MACRO,"Assey->Draw();\n");


//	disegna il BiHexagon destro e sinistro delle inner parallel straws.
	char myname[100];

	sprintf(myname,"InnerPar");
	DrawBiHexagonInMacro(
				VERTICALGAP,
				MACRO,
				RStrawDetectorMin,
				ApotemaMaxInnerParStraw,
				4,  // color code, 4= blue.
				myname
				);
//--------------
//	disegna il BiHexagon destro e sinistro delle skew straws.
	sprintf(myname,"Skew");
	DrawBiHexagonInMacro(
				VERTICALGAP,
				MACRO,
				ApotemaMinSkewStraw,
				ApotemaMaxSkewStraw,
				2,  // color code.
				myname
				);
//--------------
//	disegna il BiHexagon destro e sinistro delle skew straws.
	sprintf(myname,"OuterPar");
	DrawHexagonCircleInMacro(
				VERTICALGAP,
				MACRO,
				ApotemaMinOuterParStraw,
				RStrawDetectorMax,
				4,  // color code.
				myname
				);
//--------------

       for( i=0; i< nSttHit; i++) {
         if( !exclusionStt[i]) {     // all straws
         if( info[i][5] == 1 ) {     // parallel straws
            fprintf(MACRO,"TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nE%d->SetFillStyle(0);\nE%d->Draw();\n",
                     i,info[i][0],info[i][1],info[i][3],info[i][3],i,i);
	 }  else  {	//  skew straws.
	    fprintf(MACRO,"TMarker* SS%d = new TMarker(%f,%f,%d);\nSS%d->SetMarkerColor(1);\nSS%d->Draw();\n",
		i,info[i][0],info[i][1],28,i,i,i);
	 }
          }
       }
       for( i=0; i< nMvdPixelHit; i++) {
         if( !exclusionPixel[i]) {     // all Pixels
            fprintf(MACRO,
"TMarker* Pixel%d = new TMarker(%f,%f,%d);\nPixel%d->SetMarkerColor(1);\nPixel%d->Draw();\n",
                    i,XMvdPixel[i],YMvdPixel[i],26,i,i);
          }
       }
       for( i=0; i< nMvdStripHit; i++) {
         if( !exclusionStrip[i]) {     // all Pixels
            fprintf(MACRO,
"TMarker* Strip%d = new TMarker(%f,%f,%d);\nStrip%d->SetMarkerColor(1);\nStrip%d->Draw();\n",
                    i,XMvdStrip[i],YMvdStrip[i],25,i,i,i);
          }
       }

       for( i=0; i< nSciTilHits; i++) {
         if( InclusionListSciTil[i]) {     // all SciTil hit never used.
            fprintf(MACRO,
"TMarker* Strip%d = new TMarker(%f,%f,%d);\nStrip%d->SetMarkerColor(1);\nStrip%d->Draw();\n",
                    i,posizSciTil[i][0],posizSciTil[i][1],30,i,i,i);
          }
       }
      fprintf(MACRO,"}\n");
      fclose(MACRO);



	return;
 }







//----------end of function PndSttMvdTracking::WriteMacroAllHitsRestanti




//----------begin of function PndSttMvdTracking::DrawBiHexagonInMacro

    void PndSttMvdTracking::DrawBiHexagonInMacro(
					Double_t vgap,
					FILE * MACRO,
					Double_t Ami,
					Double_t Ama,
					UShort_t color,
					char *name
					)
{
	UShort_t iside;

	// these are the points defining the sides of the biexhagon on the left(inner).
	Double_t
		side_x[] = { -vgap/2.,	-Ama ,	-Ama,	-vgap/2.,	-vgap/2.,	-Ami,
					-Ami,	-vgap/2.,	-vgap/2.},
		side_y[] = {(-0.5*vgap+2.*Ama)/sqrt(3.),	Ama/sqrt(3.),	-Ama/sqrt(3.),
			    -(-0.5*vgap+2.*Ama)/sqrt(3.),	-(-0.5*vgap+2.*Ami)/sqrt(3.),
			    -Ami/sqrt(3.),	Ami/sqrt(3.),	(-0.5*vgap+2.*Ami)/sqrt(3.),
			    (-0.5*vgap+2.*Ama)/sqrt(3.)	};

	//  Inner straws left
	for(iside=0;iside<8;iside++){
		fprintf(MACRO,
		"TLine* %sL%d = new TLine(%f,%f,%f,%f);\n",name,
			iside,side_x[iside],side_y[iside],side_x[iside+1],side_y[iside+1]);
		fprintf(MACRO,"%sL%d->SetLineColor(%d);\n",name,iside,color);
		fprintf(MACRO,"%sL%d->SetLineStyle(2);\n",name,iside);
		fprintf(MACRO,"%sL%d->Draw();\n",name,iside);
	}

	//  Inner straws right
	for(iside=0;iside<8;iside++){
		fprintf(MACRO,
		"TLine* %sR%d = new TLine(%f,%f,%f,%f);\n",name,
			iside,-side_x[iside],side_y[iside],-side_x[iside+1],side_y[iside+1]);
		fprintf(MACRO,"%sR%d->SetLineColor(%d);\n",name,iside,color);
		fprintf(MACRO,"%sR%d->SetLineStyle(2);\n",name,iside);
		fprintf(MACRO,"%sR%d->Draw();\n",name,iside);
	}

}
//----------end of function PndSttMvdTracking::DrawBiHexagonInMacro






//----------begin of function PndSttMvdTracking::DrawHexagonCircleInMacro

    void PndSttMvdTracking::DrawHexagonCircleInMacro(
					Double_t GAP,
					FILE * MACRO,
					Double_t ApotemaMin,
					Double_t Rma,
					UShort_t color,
					char *name
					)
{
	UShort_t iside;

	Double_t angle1, angle2;

	// these are the points defining the sides of the exhagon on the left(outer).
	Double_t side_x[] = {	-GAP/2., -GAP/2. , -ApotemaMin, -ApotemaMin, -GAP/2., -GAP/2. },
		 side_y[] = {	sqrt(Rma*Rma-GAP*GAP/4.),  (2.*ApotemaMin-0.5*GAP)/sqrt(3.),
				ApotemaMin/sqrt(3.),
				-ApotemaMin/sqrt(3.),
				-(2.*ApotemaMin-0.5*GAP)/sqrt(3.), -sqrt(Rma*Rma-GAP*GAP/4.)};


	//  Outer straws left
	for(iside=0;iside<5;iside++){
		fprintf(MACRO,
		"TLine* %sL%d = new TLine(%f,%f,%f,%f);\n",name,
			iside,side_x[iside],side_y[iside],side_x[iside+1],side_y[iside+1]);
		fprintf(MACRO,"%sL%d->SetLineColor(%d);\n",name,iside,color);
		fprintf(MACRO,"%sL%d->SetLineStyle(2);\n",name,iside);
		fprintf(MACRO,"%sL%d->Draw();\n",name,iside);
	}

	//  Outer straws right
	for(iside=0;iside<5;iside++){
		fprintf(MACRO,
		"TLine* %sR%d = new TLine(%f,%f,%f,%f);\n",name,
			iside,-side_x[iside],side_y[iside],-side_x[iside+1],side_y[iside+1]);
		fprintf(MACRO,"%sR%d->SetLineColor(%d);\n",name,iside,color);
		fprintf(MACRO,"%sR%d->SetLineStyle(2);\n",name,iside);
		fprintf(MACRO,"%sR%d->Draw();\n",name,iside);
	}


	// drawing the left circle.
	angle1 = atan2 ( side_y[0], side_x[0])*180./PI;
	angle2 = 360. + atan2 ( side_y[4], side_x[4])*180./PI;
	fprintf(MACRO,"TEllipse* %sCircleL = new TEllipse(0.,0.,%f,%f,%f,%f);\n",
			name,Rma,Rma,angle1,angle2);
	fprintf(MACRO,"%sCircleL->SetFillStyle(0);\n",name);
	fprintf(MACRO,"%sCircleL->SetLineColor(%d);\n",name,color);
	fprintf(MACRO,"%sCircleL->Draw(\"only\");\n",name,color);


	// drawing the right circle.
	angle2 = atan2 ( side_y[0], -side_x[0])*180./PI;
	angle1 = atan2 ( side_y[4], -side_x[4])*180./PI;
		fprintf(MACRO,"TEllipse* %sCircleR = new TEllipse(0.,0.,%f,%f,%f,%f);\n",
			name,Rma,Rma,angle1,angle2);
	fprintf(MACRO,"%sCircleR->SetFillStyle(0);\n",name);
	fprintf(MACRO,"%sCircleR->SetLineColor(%d);\n",name,color);
	fprintf(MACRO,"%sCircleR->Draw(\"only\");\n",name,color);


}
//----------end of function PndSttMvdTracking::DrawHexagonCircleInMacro







//----------begin of function PndSttMvdTracking::DrawSttDetectorInMacro

    void PndSttMvdTracking::AssociateFoundTrackstoMC(
		  Double_t info[][7],
                  UShort_t nTracksFoundSoFar,
                  UShort_t nHitsinTrack[MAXTRACKSPEREVENT],
                  UShort_t  ListHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits],
                  UShort_t nSkewHitsinTrack[MAXTRACKSPEREVENT],
                  UShort_t  ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits],
                  Short_t daTrackFoundaTrackMC[MAXTRACKSPEREVENT]
                                                        )
{

   bool	       inclusionMC[MAXTRACKSPEREVENT][nmaxSttHits],
		inclusionExp[MAXTRACKSPEREVENT];

   UShort_t	ntoMCtrack[MAXTRACKSPEREVENT],
		toMCtracklist[MAXTRACKSPEREVENT][nmaxSttHits],
		toMCtrackfrequency[MAXTRACKSPEREVENT][nmaxSttHits];

   UShort_t  i, j, jtemp,jexp;

   Short_t  itemp, massimo;

   Int_t  enne;


   for(i=0; i<nTracksFoundSoFar;i++){



     daTrackFoundaTrackMC[i]=-1;
     inclusionExp[i]=true;
	for(j=0; j<nHitsinTrack[i]+nSkewHitsinTrack[i];j++){
		inclusionMC[i][j]=true;
	}
   }



     for(jexp=0; jexp< nTracksFoundSoFar ;jexp++){
	ntoMCtrack[jexp]=1;
	toMCtracklist[jexp][0]=(UShort_t)( info[ ListHitsinTrack[jexp][0]  ][6]+0.01);
	toMCtrackfrequency[jexp][0]=1;

// prima  gli hits paralleli ---------------------
	for(i=1; i<nHitsinTrack[jexp]; i++){
		enne = (Int_t)( info[  ListHitsinTrack[jexp][i] ][6]+0.01 );
		for(j=0; j<ntoMCtrack[jexp]; j++){
			if( enne == toMCtracklist[jexp][j] ) {
				toMCtrackfrequency[jexp][j]++;
				goto out1 ;
			}
		}
		toMCtracklist[jexp][ ntoMCtrack[jexp] ] = enne;
		toMCtrackfrequency[jexp][ ntoMCtrack[jexp] ] = 1;
		ntoMCtrack[jexp]++;
out1:  ;
	}   //  end of for(i=0; i<nHitsinTrack[jexp]; i++)



// poi  gli hits skew ---------------------
	for(i=0; i<nSkewHitsinTrack[jexp]; i++){
		enne = (Int_t)( info[ ListSkewHitsinTrack[jexp][i]  ][6]+0.01 );
		for(j=0; j<ntoMCtrack[jexp]; j++){
			if( enne == toMCtracklist[jexp][j] ) {
				toMCtrackfrequency[jexp][j]++;
				goto out2 ;
			}
		}
		toMCtracklist[jexp][ ntoMCtrack[jexp] ] = enne;
		toMCtrackfrequency[jexp][ ntoMCtrack[jexp] ] = 1;
		ntoMCtrack[jexp]++;
out2:  ;
	}   //  end of for(i=0; j<nHitsinTrack[jexp]; i++)



     }  // end of  for(jexp=0; jexp< nTracksFoundSoFar ;jexp++)


     itemp=0;
     while ( itemp > -1){
	itemp=-1;
	massimo = -1;
	for(jexp=0; jexp< nTracksFoundSoFar ;jexp++){
		if( !inclusionExp[jexp])  continue;
		for(i=0; i< ntoMCtrack[jexp]; i++){
			if( !inclusionMC[jexp][i])  continue;
			if( toMCtrackfrequency[jexp][i]>massimo){
				massimo=toMCtrackfrequency[jexp][i];
				itemp = toMCtracklist[jexp][i];
				jtemp = jexp;
			}
		}
	}
	if( itemp>-1 ){
		daTrackFoundaTrackMC[jtemp]=itemp;
		inclusionExp[jtemp]=false;
		for(jexp=0; jexp<nTracksFoundSoFar;jexp++){
			for(int jk=0;jk<ntoMCtrack[jexp];jk++){
				if( itemp==toMCtracklist[jexp][jk]){
					inclusionMC[jexp][jk]=false;
				}
			}
		}
	}
     }    //    end while ( itemp > -1)

  return;


}




//----------end of function PndSttMvdTracking::AssociateFoundTrackstoMC











//----------begin of function PndSttMvdTracking::AssociateFoundTrackstoMCbis

    void PndSttMvdTracking::AssociateFoundTrackstoMCbis(
		  Double_t info[][7],
                  UShort_t nTracksFoundSoFar,
                  UShort_t nHitsinTrack[MAXTRACKSPEREVENT],
                  UShort_t  ListHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits],
                  UShort_t nSkewHitsinTrack[MAXTRACKSPEREVENT],
                  UShort_t  ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits],
                  Short_t daTrackFoundaTrackMC[MAXTRACKSPEREVENT]
                                                        )
{

   bool	       inclusionMC[MAXTRACKSPEREVENT][nmaxSttHits],
		inclusionExp[MAXTRACKSPEREVENT];

   UShort_t	ntoMCtrack[MAXTRACKSPEREVENT],
		toMCtracklist[MAXTRACKSPEREVENT][nmaxSttHits],
		toMCtrackfrequency[MAXTRACKSPEREVENT][nmaxSttHits];

   UShort_t  i, j, enne, jtemp,jexp;

   Short_t  itemp, massimo;



   for(i=0; i<nTracksFoundSoFar;i++){



     daTrackFoundaTrackMC[i]=-1;
     inclusionExp[i]=true;
	for(j=0; j<nHitsinTrack[i]+nSkewHitsinTrack[i];j++){
		inclusionMC[i][j]=true;
	}
   }



     for(jexp=0; jexp< nTracksFoundSoFar ;jexp++){
	ntoMCtrack[jexp]=1;
	toMCtracklist[jexp][0]=(UShort_t)( info[ ListHitsinTrack[jexp][0]  ][6]+0.01);
	toMCtrackfrequency[jexp][0]=1;

// prima  gli hits paralleli ---------------------
	for(i=1; i<nHitsinTrack[jexp]; i++){
		enne = (UShort_t)( info[  ListHitsinTrack[jexp][i] ][6]+0.01 );
		for(j=0; j<ntoMCtrack[jexp]; j++){
			if( enne == toMCtracklist[jexp][j] ) {
				toMCtrackfrequency[jexp][j]++;
				goto out1 ;
			}
		}
		toMCtracklist[jexp][ ntoMCtrack[jexp] ] = enne;
		toMCtrackfrequency[jexp][ ntoMCtrack[jexp] ] = 1;
		ntoMCtrack[jexp]++;
out1:  ;
	}   //  end of for(i=0; i<nHitsinTrack[jexp]; i++)


/*
// poi  gli hits skew ---------------------
	for(i=0; i<nSkewHitsinTrack[jexp]; i++){
		enne = (UShort_t)( info[ ListSkewHitsinTrack[jexp][i]  ][6]+0.01 );
		for(j=0; j<ntoMCtrack[jexp]; j++){
			if( enne == toMCtracklist[jexp][j] ) {
				toMCtrackfrequency[jexp][j]++;
				goto out2 ;
			}
		}
		toMCtracklist[jexp][ ntoMCtrack[jexp] ] = enne;
		toMCtrackfrequency[jexp][ ntoMCtrack[jexp] ] = 1;
		ntoMCtrack[jexp]++;
out2:  ;
	}   //  end of for(i=0; j<nHitsinTrack[jexp]; i++)

*/

     }  // end of  for(jexp=0; jexp< nTracksFoundSoFar ;jexp++)


     itemp=0;
     while ( itemp > -1){
	itemp=-1;
	massimo = -1;
	for(jexp=0; jexp< nTracksFoundSoFar ;jexp++){
		if( !inclusionExp[jexp])  continue;
		for(i=0; i< ntoMCtrack[jexp]; i++){
			if( !inclusionMC[jexp][i])  continue;
			if( toMCtrackfrequency[jexp][i]>massimo){
				massimo=toMCtrackfrequency[jexp][i];
				itemp = toMCtracklist[jexp][i];
				jtemp = jexp;
			}
		}
	}
	if( itemp>-1 ){
		daTrackFoundaTrackMC[jtemp]=itemp;
		inclusionExp[jtemp]=false;
		for(jexp=0; jexp<nTracksFoundSoFar;jexp++){
			for(int jk=0;jk<ntoMCtrack[jexp];jk++){
				if( itemp==toMCtracklist[jexp][jk]){
					inclusionMC[jexp][jk]=false;
				}
			}
		}
	}
     }    //    end while ( itemp > -1)

  return;


}




//----------end of function PndSttMvdTracking::AssociateFoundTrackstoMCbis







//----------begin of function PndSttMvdTracking::AssociateFoundTrackstoMCtris

    void PndSttMvdTracking::AssociateFoundTrackstoMCtris(
		Double_t info[][7],
		Double_t Ox[MAXTRACKSPEREVENT],
		Double_t Oy[MAXTRACKSPEREVENT],
		Double_t R[MAXTRACKSPEREVENT],
                   UShort_t nTracksFoundSoFar,
                  UShort_t nHitsinTrack[MAXTRACKSPEREVENT],
                  UShort_t  ListHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits],
                  UShort_t nSkewHitsinTrack[MAXTRACKSPEREVENT],
                  UShort_t  ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits],
                  Short_t daTrackFoundaTrackMC[MAXTRACKSPEREVENT]
                                                        )
{

   bool	firstime,
	inclusionMC[MAXTRACKSPEREVENT][nmaxSttHits],
		inclusionExp[MAXTRACKSPEREVENT];

   UShort_t	ntoMCtrack[MAXTRACKSPEREVENT],
		toMCtrackfrequency[MAXTRACKSPEREVENT][nmaxSttHits];

   UShort_t  i, j, jtemp,jexp , nmid;

   Short_t   itemp, massimo,
		toMCtracklist[MAXTRACKSPEREVENT][nmaxSttHits];

   Int_t enne;

	Double_t dx,
		 Cx,
		 Cy,
		 Rr,
		 alfa,
		 beta,
		 gamma,
		 minimo,
		 tanlow[MAXTRACKSPEREVENT],
		 tanmid[MAXTRACKSPEREVENT],
		 tanup[MAXTRACKSPEREVENT],
		 toMCtrackdistance[MAXTRACKSPEREVENT][nmaxSttHits];


int nevento=4;


   for(i=0; i<nTracksFoundSoFar;i++){



     daTrackFoundaTrackMC[i]=-1;
     inclusionExp[i]=true;
	for(j=0; j<nHitsinTrack[i]+nSkewHitsinTrack[i];j++){
		inclusionMC[i][j]=true;
	}

//--- find the minimum, middle, end Tan(angle) of this track, for the comparison later

	tanlow[i]=0.;
	tanmid[i]=0.;
	tanup [i]=0.;
	if( nHitsinTrack[i] > 2) {

		dx = info[ ListHitsinTrack[i][0] ][0] - Ox[i];
		if(fabs(dx)> 1.e-10){
			tanlow[i] = ( info[ ListHitsinTrack[i][0] ][1] - Oy[i] )/dx;
		} else {
			tanlow[i] = 999999.;
		}

		nmid = (UShort_t) (nHitsinTrack[i]+1)/2;
		dx = info[ ListHitsinTrack[i][nmid] ][0] - Ox[i];
		if(fabs(dx)> 1.e-10){
			tanmid[i] = ( info[ ListHitsinTrack[i][nmid] ][1] - Oy[i] )/dx;
		} else {
			tanmid[i] = 999999.;
		}

		dx = info[ ListHitsinTrack[i][nHitsinTrack[i]-1] ][0] - Ox[i];
		if(fabs(dx)> 1.e-10){
			tanup[i] = ( info[ ListHitsinTrack[i][nHitsinTrack[i]-1] ][1] - Oy[i] )/dx;
		} else {
			tanup[i] = 999999.;
		}

	}	// end of  if( nHitsinTrack[i] > 2)

//-----------------------------

   }	// end of for(i=0; i<nTracksFoundSoFar;i++)





     for(jexp=0; jexp< nTracksFoundSoFar ;jexp++){

	firstime=true;
	ntoMCtrack[jexp]=0;


// solo  gli hits paralleli ---------------------
	for(i=0; i<nHitsinTrack[jexp]; i++){
		enne = (Int_t)( info[  ListHitsinTrack[jexp][i] ][6]+0.01 );
		if(enne<0) continue;   //  hit not associated to any MC track; noise hit.

		if(firstime) {
			toMCtracklist[jexp][0]= enne;
			toMCtrackfrequency[jexp][0]=1;
			firstime = false;

			getMCInfo( enne, &Cx, &Cy, &Rr);
			if( Rr<0.) {
				toMCtrackdistance[jexp][0]=-1.;
			} else {
				alfa = -2.*Cx;
				beta = -2.*Cy;
				gamma = Cx*Cx+Cy*Cy-Rr*Rr;
				toMCtrackdistance[jexp][0]= FindDistance(Ox[jexp],Oy[jexp],
					R[jexp],tanlow[jexp],tanmid[jexp],tanup[jexp],alfa,beta,gamma);
			}
			ntoMCtrack[jexp]=1;

		} else {	// continuation of  if(firstime)

			for(j=0; j<ntoMCtrack[jexp]; j++){
				if( enne == toMCtracklist[jexp][j] ) {
					toMCtrackfrequency[jexp][j]++;
					goto out1 ;
				}
			}
			toMCtracklist[jexp][ ntoMCtrack[jexp] ] = enne;
			toMCtrackfrequency[jexp][ ntoMCtrack[jexp] ] = 1;
			getMCInfo( enne, &Cx, &Cy, &Rr);
			if( Rr<0.) {
				toMCtrackdistance[jexp][ntoMCtrack[jexp]]=-1.;
			} else {
				alfa = -2.*Cx;
				beta = -2.*Cy;
				gamma = Cx*Cx+Cy*Cy-Rr*Rr;
				toMCtrackdistance[jexp][ntoMCtrack[jexp]]= FindDistance(Ox[jexp],Oy[jexp],
					R[jexp],tanlow[jexp],tanmid[jexp],tanup[jexp],alfa,beta,gamma);
			}
			ntoMCtrack[jexp]++;
out1:  ;
		}

	}   //  end of for(i=0; i<nHitsinTrack[jexp]; i++)


     }  // end of  for(jexp=0; jexp< nTracksFoundSoFar ;jexp++)


     itemp=0;
     while ( itemp > -1){
	itemp=-1;
	massimo = -1;
	minimo = 999999999999.;
	for(jexp=0; jexp< nTracksFoundSoFar ;jexp++){
		if( !inclusionExp[jexp])  continue;
		for(i=0; i< ntoMCtrack[jexp]; i++){
			if( !inclusionMC[jexp][i])  continue;
			// if( toMCtrackfrequency[jexp][i]>massimo){
			if( toMCtrackdistance[jexp][i]<-0.5) continue;
			if( toMCtrackdistance[jexp][i]<minimo){
				massimo=toMCtrackfrequency[jexp][i];
				minimo=toMCtrackdistance[jexp][i];
				itemp = toMCtracklist[jexp][i];
				jtemp = jexp;
			}
		}
	}
	if( itemp>-1 ){
		daTrackFoundaTrackMC[jtemp]=itemp;
		inclusionExp[jtemp]=false;
		for(jexp=0; jexp<nTracksFoundSoFar;jexp++){
			for(int jk=0;jk<ntoMCtrack[jexp];jk++){
				if( itemp==toMCtracklist[jexp][jk]){
					inclusionMC[jexp][jk]=false;
				}
			}
		}
	}
     }    //    end while ( itemp > -1)

  return;


}




//----------end of function PndSttMvdTracking::AssociateFoundTrackstoMCtris






//----------begin of function PndSttMvdTracking::AssociateFoundTrackstoMCquater

    void PndSttMvdTracking::AssociateFoundTrackstoMCquater(
		bool *keepit,
		Double_t info[][7],
		Double_t Ox[MAXTRACKSPEREVENT],
		Double_t Oy[MAXTRACKSPEREVENT],
		Double_t R[MAXTRACKSPEREVENT],
		Double_t X1[MAXTRACKSPEREVENT],
		Double_t Y1[MAXTRACKSPEREVENT],
		Double_t X2[MAXTRACKSPEREVENT],
		Double_t Y2[MAXTRACKSPEREVENT],
		Double_t X3[MAXTRACKSPEREVENT],
		Double_t Y3[MAXTRACKSPEREVENT],

                   UShort_t nTracksFoundSoFar,
                  UShort_t nHitsinTrack[MAXTRACKSPEREVENT],
UShort_t  ListHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],
                  UShort_t nSkewHitsinTrack[MAXTRACKSPEREVENT],
UShort_t  ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],

		UShort_t nPixelHitsinTrack[MAXTRACKSPEREVENT],
UShort_t ListPixelHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHitsInTrack],
		Int_t *FromPixeltoMCTrack,
		UShort_t nStripHitsinTrack[MAXTRACKSPEREVENT],
UShort_t ListStripHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdStripHitsInTrack],
		Int_t *FromStriptoMCTrack,

		Short_t daTrackFoundaTrackMC[MAXTRACKSPEREVENT]
						)
{

   bool	firstime,
 inclusionMC[nTracksFoundSoFar][nmaxSttHitsInTrack+nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack],
		inclusionExp[nTracksFoundSoFar];
//	inclusionMC[MAXTRACKSPEREVENT][nmaxSttHits+nmaxMvdPixelHits+nmaxMvdStripHits],
//		inclusionExp[MAXTRACKSPEREVENT];

//   UShort_t	ntoMCtrack[MAXTRACKSPEREVENT],
//		toMCtrackfrequency[MAXTRACKSPEREVENT][nmaxSttHits];
   UShort_t	ntoMCtrack[nTracksFoundSoFar],
		toMCtrackfrequency[nTracksFoundSoFar][nmaxSttHitsInTrack];

   UShort_t  i, j, jtemp,jexp , nmid;

   Short_t  itemp, massimo,
		toMCtracklist[nTracksFoundSoFar][nmaxSttHitsInTrack];
//		toMCtracklist[MAXTRACKSPEREVENT][nmaxSttHits];

   Int_t enne;

	Double_t dx,
		 Cx,
		 Cy,
		 Rr,
		 alfa,
		 beta,
		 gamma,
		 minimo,
		 tanlow[nTracksFoundSoFar],
		 tanmid[nTracksFoundSoFar],
		 tanup[nTracksFoundSoFar],
		 toMCtrackdistance[nTracksFoundSoFar][nmaxSttHitsInTrack];
//		 tanlow[MAXTRACKSPEREVENT],
//		 tanmid[MAXTRACKSPEREVENT],
//		 tanup[MAXTRACKSPEREVENT],
//		 toMCtrackdistance[MAXTRACKSPEREVENT][nmaxSttHits];




   for(i=0; i<nTracksFoundSoFar;i++){
	if(!keepit[i]) continue;
     daTrackFoundaTrackMC[i]=-1;
     inclusionExp[i]=true;
//	for(j=0; j<nHitsinTrack[i]+nSkewHitsinTrack[i];j++){
	for(j=0; j<nHitsinTrack[i]+nSkewHitsinTrack[i]+nPixelHitsinTrack[i]
		+nStripHitsinTrack[i];j++){
		inclusionMC[i][j]=true;
	}

//--- find the minimum, middle, end Tan(angle) of this track, for the comparison later

	tanlow[i]=0.;
	tanmid[i]=0.;
	tanup [i]=0.;
	int nn = nHitsinTrack[i]+nPixelHitsinTrack[i]+nStripHitsinTrack[i];
	if( nn > 2 || nn == 1) {
		dx = X1[i] - Ox[i];
		if(fabs(dx)> 1.e-10){
			tanlow[i] = (Y1[i] - Oy[i] )/dx;
		} else {
			tanlow[i] = 999999.;
		}

		dx = X2[i] - Ox[i];
		if(fabs(dx)> 1.e-10){
			tanmid[i] = ( Y2[i] - Oy[i] )/dx;
		} else {
			tanmid[i] = 999999.;
		}

		dx = X3[i] - Ox[i];
		if(fabs(dx)> 1.e-10){
			tanup[i] = (Y3[i] - Oy[i] )/dx;
		} else {
			tanup[i] = 999999.;
		}

	} else if (nn==2) {	// continuation  of  if( nn > 2 || nn == 1)
		dx = X1[i] - Ox[i];
		if(fabs(dx)> 1.e-10){
			tanlow[i] = (Y1[i] - Oy[i] )/dx;
		} else {
			tanlow[i] = 999999.;
		}

		dx = X2[i] - Ox[i];
		if(fabs(dx)> 1.e-10){
			tanmid[i] = ( Y2[i] - Oy[i] )/dx;
		} else {
			tanmid[i] = 999999.;
		}

		tanup[i] = tanmid[i];

	}

/*
	 else{  // continuation of   if( nn > 2), case in which there is only 1 hit in the Track cand.

		dx = X1[i] - Ox[i];
		if(fabs(dx)> 1.e-10){
			tanlow[i] = (Y1[i] - Oy[i] )/dx;
		} else {
			tanlow[i] = 999999.;
		}


		tanmid[i] = tanlow[i];
		tanup[i] = tanlow[i];

	}	// end of  if( nn > 2)
*/
//-----------------------------

   }	// end of for(i=0; i<nTracksFoundSoFar;i++)





     for(jexp=0; jexp< nTracksFoundSoFar ;jexp++){
	if(!keepit[jexp]) continue;

	firstime=true;
	ntoMCtrack[jexp]=0;


// prima  gli hits paralleli ---------------------
	for(i=0; i<nHitsinTrack[jexp]; i++){
		enne = (Int_t)( info[  ListHitsinTrack[jexp][i] ][6]+0.01 );
		if(enne<0) continue;   //  hit not associated to any MC track; noise hit.

		if(firstime) {
			toMCtracklist[jexp][0]= enne;
			toMCtrackfrequency[jexp][0]=1;
			firstime = false;

			getMCInfo( enne, &Cx, &Cy, &Rr);
			if( Rr<0.) {
				toMCtrackdistance[jexp][0]=-1.;
			} else {
				alfa = -2.*Cx;
				beta = -2.*Cy;
				gamma = Cx*Cx+Cy*Cy-Rr*Rr;
				toMCtrackdistance[jexp][0]= FindDistance(Ox[jexp],Oy[jexp],
				R[jexp],tanlow[jexp],tanmid[jexp],tanup[jexp],alfa,beta,gamma);
			}
			ntoMCtrack[jexp]=1;

		} else {	// continuation of  if(firstime)

			for(j=0; j<ntoMCtrack[jexp]; j++){
				if( enne == toMCtracklist[jexp][j] ) {
					toMCtrackfrequency[jexp][j]++;
					goto out1 ;
				}
			}
			toMCtracklist[jexp][ ntoMCtrack[jexp] ] = enne;
			toMCtrackfrequency[jexp][ ntoMCtrack[jexp] ] = 1;
			getMCInfo( enne, &Cx, &Cy, &Rr);
			if( Rr<0.) {
				toMCtrackdistance[jexp][ntoMCtrack[jexp]]=-1.;
			} else {
				alfa = -2.*Cx;
				beta = -2.*Cy;
				gamma = Cx*Cx+Cy*Cy-Rr*Rr;
				toMCtrackdistance[jexp][ntoMCtrack[jexp]]= FindDistance(Ox[jexp],Oy[jexp],
				R[jexp],tanlow[jexp],tanmid[jexp],tanup[jexp],alfa,beta,gamma);
			}
			ntoMCtrack[jexp]++;
out1:  ;
		}

	}   //  end of for(i=0; i<nHitsinTrack[jexp]; i++)



// poi i pixel -------------------------------------------

	for(i=0; i<nPixelHitsinTrack[jexp]; i++){
		enne = FromPixeltoMCTrack[ ListPixelHitsinTrack[jexp][i] ] ;
		if(enne<0) continue;   //  hit not associated to any MC track; noise hit.

		if(firstime) {
			toMCtracklist[jexp][0]= enne;
			toMCtrackfrequency[jexp][0]=1;
			firstime = false;

			getMCInfo( enne, &Cx, &Cy, &Rr);
			if( Rr<0.) {
				toMCtrackdistance[jexp][0]=-1.;
			} else {
				alfa = -2.*Cx;
				beta = -2.*Cy;
				gamma = Cx*Cx+Cy*Cy-Rr*Rr;
				toMCtrackdistance[jexp][0]= FindDistance(Ox[jexp],Oy[jexp],
				R[jexp],tanlow[jexp],tanmid[jexp],tanup[jexp],alfa,beta,gamma);
			}
			ntoMCtrack[jexp]=1;

		} else {	// continuation of  if(firstime)

			for(j=0; j<ntoMCtrack[jexp]; j++){
				if( enne == toMCtracklist[jexp][j] ) {
					toMCtrackfrequency[jexp][j]++;
					goto out2 ;
				}
			}
			toMCtracklist[jexp][ ntoMCtrack[jexp] ] = enne;
			toMCtrackfrequency[jexp][ ntoMCtrack[jexp] ] = 1;
			getMCInfo( enne, &Cx, &Cy, &Rr);
			if( Rr<0.) {
				toMCtrackdistance[jexp][ntoMCtrack[jexp]]=-1.;
			} else {
				alfa = -2.*Cx;
				beta = -2.*Cy;
				gamma = Cx*Cx+Cy*Cy-Rr*Rr;
				toMCtrackdistance[jexp][ntoMCtrack[jexp]]= FindDistance(Ox[jexp],Oy[jexp],
				R[jexp],tanlow[jexp],tanmid[jexp],tanup[jexp],alfa,beta,gamma);
			}
			ntoMCtrack[jexp]++;
out2:  ;
		}

	}   //  end of for(i=0; i<nHitsinTrack[jexp]; i++)

// infine le strip -------------------------------------------

	for(i=0; i<nStripHitsinTrack[jexp]; i++){

		enne = FromStriptoMCTrack[ ListStripHitsinTrack[jexp][i] ] ;
		if(enne<0) continue;   //  hit not associated to any MC track; noise hit.

		if(firstime) {
			toMCtracklist[jexp][0]= enne;
			toMCtrackfrequency[jexp][0]=1;
			firstime = false;

			getMCInfo( enne, &Cx, &Cy, &Rr);
			if( Rr<0.) {
				toMCtrackdistance[jexp][0]=-1.;
			} else {
				alfa = -2.*Cx;
				beta = -2.*Cy;
				gamma = Cx*Cx+Cy*Cy-Rr*Rr;
				toMCtrackdistance[jexp][0]= FindDistance(Ox[jexp],Oy[jexp],
				R[jexp],tanlow[jexp],tanmid[jexp],tanup[jexp],alfa,beta,gamma);
			}
			ntoMCtrack[jexp]=1;

		} else {	// continuation of  if(firstime)

			for(j=0; j<ntoMCtrack[jexp]; j++){
				if( enne == toMCtracklist[jexp][j] ) {
					toMCtrackfrequency[jexp][j]++;
					goto out3 ;
				}
			}
			toMCtracklist[jexp][ ntoMCtrack[jexp] ] = enne;
			toMCtrackfrequency[jexp][ ntoMCtrack[jexp] ] = 1;
			getMCInfo( enne, &Cx, &Cy, &Rr);
			if( Rr<0.) {
				toMCtrackdistance[jexp][ntoMCtrack[jexp]]=-1.;
			} else {
				alfa = -2.*Cx;
				beta = -2.*Cy;
				gamma = Cx*Cx+Cy*Cy-Rr*Rr;
				toMCtrackdistance[jexp][ntoMCtrack[jexp]]= FindDistance(Ox[jexp],Oy[jexp],
				R[jexp],tanlow[jexp],tanmid[jexp],tanup[jexp],alfa,beta,gamma);
			}
			ntoMCtrack[jexp]++;
out3:  ;
		}

	}   //  end of for(i=0; i<nHitsinTrack[jexp]; i++)

     }  // end of  for(jexp=0; jexp< nTracksFoundSoFar ;jexp++)

//--------------------------------------------------------------------
     itemp=0;
     while ( itemp > -1){
	itemp=-1;
	massimo = -1;
	minimo = 999999999999.;
	for(jexp=0; jexp< nTracksFoundSoFar ;jexp++){
		if(!keepit[jexp]) continue;
		if( !inclusionExp[jexp])  continue;
		for(i=0; i< ntoMCtrack[jexp]; i++){
			if( !inclusionMC[jexp][i])  continue;


			// if( toMCtrackfrequency[jexp][i]>massimo){
			if( toMCtrackdistance[jexp][i]<-0.5) continue;
			if( toMCtrackdistance[jexp][i]<minimo){
				massimo=toMCtrackfrequency[jexp][i];
				minimo=toMCtrackdistance[jexp][i];
				itemp = toMCtracklist[jexp][i];
				jtemp = jexp;
			}
		}
	}  // end of   for(jexp=0; jexp< nTracksFoundSoFar ;jexp++)
	if( itemp>-1 ){
		daTrackFoundaTrackMC[jtemp]=itemp;
		inclusionExp[jtemp]=false;
		for(jexp=0; jexp<nTracksFoundSoFar;jexp++){
			if(!keepit[jexp]) continue;
			for(int jk=0;jk<ntoMCtrack[jexp];jk++){
				if( itemp==toMCtracklist[jexp][jk]){
					inclusionMC[jexp][jk]=false;
				}
			}
		}
	}
     }    //    end while ( itemp > -1)

  return;


}




//----------end of function PndSttMvdTracking::AssociateFoundTrackstoMCquater



//------------------------- begin of function  PndSttMvdTracking::SttMatchedSpurious


void PndSttMvdTracking::SttMatchedSpurious(
			bool *keepit,
			UShort_t ntotalHits,
			Double_t info[][7],
                        UShort_t nTracksFoundSoFar, //  quelle trovate dal PR
  			UShort_t nHitsinTrack[], // n. hits PARALLELI+SKEW, dal PR
UShort_t ListHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHitsInTrack], // dal PR
  			UShort_t nSkewHitsinTrack[MAXTRACKSPEREVENT], // n. hits skew, dal PR
UShort_t ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHitsInTrack], // dal PR

			UShort_t nParalCommon[MAXTRACKSPEREVENT],
                        UShort_t ParalCommonList[MAXMCTRACKS][nmaxSttHitsInTrack],
                        UShort_t nSpuriParinTrack[MAXTRACKSPEREVENT],
                        UShort_t ParSpuriList[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],

			UShort_t nSkewCommon[MAXTRACKSPEREVENT],
                        UShort_t SkewCommonList[MAXMCTRACKS][nmaxSttHitsInTrack],
                        UShort_t nSpuriSkewinTrack[MAXTRACKSPEREVENT],
                        UShort_t SkewSpuriList[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],

                        UShort_t nHitsInMCTrack[MAXTRACKSPEREVENT],
                        UShort_t nSkewHitsInMCTrack[MAXTRACKSPEREVENT],

			UShort_t nMCParalAlone[MAXTRACKSPEREVENT],
                        UShort_t MCParalAloneList[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],
			UShort_t nMCSkewAlone[MAXTRACKSPEREVENT],
                        UShort_t MCSkewAloneList[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],

			Short_t  daTrackFoundaTrackMC[MAXTRACKSPEREVENT]
                                               )
{

    UShort_t	i, jexp, exphit, iHit,
		enne[MAXTRACKSPEREVENT][nmaxSttHits];
  Short_t	emme;


  for(jexp=0; jexp<nTracksFoundSoFar;jexp++){
	if(!keepit[jexp]) continue;
	nParalCommon[jexp]=0;
	nSkewCommon[jexp]=0;
	nMCParalAlone[jexp]=0;
	nMCSkewAlone[jexp]=0;
	nSpuriParinTrack[jexp]=0;
	nSpuriSkewinTrack[jexp]=0;

// --- parallel hits
	for(exphit=0; exphit<nHitsinTrack[jexp]; exphit++){
		iHit = ListHitsinTrack[jexp][exphit] ;
		enne[jexp][exphit] = (UShort_t) ( info[iHit][6] + 0.01);
		if( enne[jexp][exphit] ==   daTrackFoundaTrackMC[jexp] ){
			ParalCommonList[jexp][ nParalCommon[jexp] ] = iHit;
			nParalCommon[jexp]++;
		} else {
			ParSpuriList[jexp][ nSpuriParinTrack[jexp] ] = iHit;
			nSpuriParinTrack[jexp]++;			
		}

	}
//--- ricerca degli hits non mecciati, della traccia MC associata a questa traccia trovata.
	for(i=0; i<ntotalHits; i++){
		emme = (UShort_t) ( info[ i ][6] + 0.01);
		// escludo gli hits non paralleli oppure che non appartengono alla giusta
		// traccia MC
		if( info[i][5] > 2. || (emme != daTrackFoundaTrackMC[jexp]) ) continue;
		if( !InclusionListStt[i]) continue; // escludo gli hits con multiple hits
			for(exphit=0; exphit<nHitsinTrack[jexp]; exphit++){
				if(ListHitsinTrack[jexp][exphit] == i) goto pinco ;
			}
			MCParalAloneList[jexp][ nMCParalAlone[jexp] ] = i;
			nMCParalAlone[jexp]++;
			pinco:  ;
	}  //  end of  for(i=0; i<ntotalHits; i++)

	nHitsInMCTrack[jexp] = nMCParalAlone[jexp]+nParalCommon[jexp];
// --- skew hits

	for(exphit=0; exphit<nSkewHitsinTrack[jexp]; exphit++){
		iHit =  ListSkewHitsinTrack[jexp][exphit];
		enne[jexp][exphit] = (UShort_t) ( info[iHit][6] + 0.01);
		if( enne[jexp][exphit] ==   daTrackFoundaTrackMC[jexp] ){
			SkewCommonList[jexp][ nSkewCommon[jexp] ] = iHit;
			nSkewCommon[jexp]++;
		} else {
			SkewSpuriList[jexp][ nSpuriSkewinTrack[jexp] ] = iHit;
			nSpuriSkewinTrack[jexp]++;			
		}

	}
//--- ricerca degli hits non mecciati, della traccia MC associata a questa traccia trovata.
	for(i=0; i<ntotalHits; i++){
		emme = (Short_t) ( info[ i ][6] + 0.01);

		// considero solo le skew ( info[i][5]=99.) ed escludo quelle che
		//  non appartengono alla giusta traccia MC
		if( info[i][5] < 98. || (emme != daTrackFoundaTrackMC[jexp]) ) continue;
		if( !InclusionListStt[i]) continue; // escludo gli hits con multiple hits
			for(exphit=0; exphit<nSkewHitsinTrack[jexp]; exphit++){
				if(i == ListSkewHitsinTrack[jexp][exphit] ) goto pinco2 ;
			}
			MCSkewAloneList[jexp][ nMCSkewAlone[jexp] ] = i;
			nMCSkewAlone[jexp]++;
			pinco2:  ;
	}

	nSkewHitsInMCTrack[jexp] = nMCSkewAlone[jexp]+nSkewCommon[jexp];


   }   //   end of  for(jexp=0; jexp<nTracksFoundSoFar;jexp++)










    return;
}


//------------------------- end of function  PndSttMvdTracking::SttMatchedSpurious





//------------------------- begin of function  PndSttMvdTracking::MatchMvdHitsToSttTracks

	void	PndSttMvdTracking::MatchMvdHitsToSttTracks(
			Double_t delta,
			UShort_t nSttTrackCand,
			Double_t Ox[MAXTRACKSPEREVENT],
			Double_t Oy[MAXTRACKSPEREVENT],
			Double_t R[MAXTRACKSPEREVENT],
			Double_t FI0[MAXTRACKSPEREVENT],
			Double_t Fifirst[MAXTRACKSPEREVENT],
			Short_t CHARGE[MAXTRACKSPEREVENT],

			UShort_t nPixelHitsinTrack[MAXTRACKSPEREVENT], // output
			UShort_t ListPixelHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHits], // output
			UShort_t nStripHitsinTrack[MAXTRACKSPEREVENT], // output
			UShort_t ListStripHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdStripHits] // output
                    )
{
	bool specialcase;

	UShort_t i,j;

	Double_t angle,
		anglemax,
		anglemin;



	for(i=0; i<nSttTrackCand; i++){
if(istampa>=3 && IVOLTE<20) cout<<"MatchMvdHitsToSttTracks, i= "<<i<<", FI0[i] = "<<
                    FI0[i] <<", Fifirst[i] = "<<Fifirst[i]<<endl;
//   handle those cases when FI0 and Fifirst are just around 0.
	if( fabs( FI0[i] - Fifirst[i]) > PI ) {     //  this is the special case
		specialcase = true;
		if( FI0[i] < Fifirst[i] ){
			FI0[i] += 2.*PI;
		} else {
			Fifirst[i] += 2.*PI;
if(istampa>=3 && IVOLTE<20) cout<<"MatchMvdHitsToSttTracks, track candidate n. = "<<i
                        <<", c'e' stata correzione!!!! FI0[i] dopo correzione = "<<
                    FI0[i] <<", Fifirst[i] dopo correzione = "<<Fifirst[i]<<endl;
		}
	} else {
		specialcase = false;
	}

	if( FI0[i] < Fifirst[i] ){
		anglemax = Fifirst[i];
		anglemin = FI0[i];
	} else {
		anglemax = FI0[i];
		anglemin = Fifirst[i];
	}

//--------------------
	   nPixelHitsinTrack[i]=0;
	   nStripHitsinTrack[i]=0;
	   for(j=0; j<nMvdPixelHit; j++){


//------- stampaggi
if(istampa>=3){
cout<<" pixel hit n. "<<j<<",  XMvdPixel[j] = "<<XMvdPixel[j]
    <<",  YMvdPixel[j] = "<<YMvdPixel[j]<<",  ZMvdPixel[j] = "<<ZMvdPixel[j]<<endl;

cout<<" dist = "<<
  sqrt((Ox[i]-XMvdPixel[j])*(Ox[i]-XMvdPixel[j])+(Oy[i]-YMvdPixel[j])*(Oy[i]-YMvdPixel[j]))
 <<",   ed  R = "<<R[i]<<endl;
}
//----- fine stampaggi

		angle = atan2(YMvdPixel[j]-Oy[i],XMvdPixel[j]-Ox[i]);
		if( angle<0. ) angle += 2.*PI;
		if(specialcase){
			if (angle < PI )   angle += 2.*PI;
		}
if(istampa>=3 && IVOLTE<20) cout<<"MatchMvdHitsToSttTracks, trackcandidate n. = "<<i
		<<",  angolo hit pixel (dopo eventuali correzioni) = "<<angle<<endl;


	      if( fabs( sqrt((Ox[i]-XMvdPixel[j])*(Ox[i]-XMvdPixel[j])
	               +(Oy[i]-YMvdPixel[j])*(Oy[i]-YMvdPixel[j]))
		        - R[i])  <  delta
					&&
			angle > anglemin && angle < anglemax
		)
	      {
		ListPixelHitsinTrack[i][nPixelHitsinTrack[i]]=j;
		nPixelHitsinTrack[i]++;
	      }
	   }
	   for(j=0; j<nMvdStripHit; j++){
//-----------stampaggi
if(istampa>=3 && IVOLTE<20){
cout<<"strip hit n. "<<j<<",  XMvdStrip[j] = "<<XMvdStrip[j]
    <<",  YMvdStrip[j] = "<<YMvdStrip[j]<<",  ZMvdStrip[j] = "<<ZMvdStrip[j]<<endl;
cout<<" dist = "<<
 sqrt((Ox[i]-XMvdStrip[j])*(Ox[i]-XMvdStrip[j])
	               +(Oy[i]-YMvdStrip[j])*(Oy[i]-YMvdStrip[j]))
		
 <<",   ed  R = "<<R[i]<<endl;
}
//----------------------fine stampaggi

		angle = atan2(YMvdStrip[j]-Oy[i],XMvdStrip[j]-Ox[i]);
		if( angle<0. ) angle += 2.*PI;
		if(specialcase){
			if (angle < PI )   angle += 2.*PI;
		}
if(istampa>=3 && IVOLTE<20) cout<<"MatchMvdHitsToSttTracks, trackcandidate n. = "<<i
		<<",  angolo hit strip (dopo eventuali correzioni) = "<<angle<<endl;
	      if( fabs( sqrt((Ox[i]-XMvdStrip[j])*(Ox[i]-XMvdStrip[j])
	               +(Oy[i]-YMvdStrip[j])*(Oy[i]-YMvdStrip[j]))
		       - R[i]) <  delta
					&&
			angle > anglemin && angle < anglemax
		)
	      {
		ListStripHitsinTrack[i][nStripHitsinTrack[i]]=j;
		nMvdStripHitsinTrack[i]++;
	      }
	   }
	}


 return;
}

//------------------------- end of function  PndSttMvdTracking::MatchMvdHitsToSttTracks


//----------begin of function PndSttMvdTracking::Merge_Sort



void PndSttMvdTracking::Merge_Sort(
	UShort_t n_ele,
	Double_t *array,
	Int_t *ind)
{

  Int_t nr, nl, middle, i,
	ind_left[n_ele], ind_right[n_ele];

  Double_t left[n_ele], right[n_ele], result[n_ele];

   if( n_ele <= 1)  return;

   middle = n_ele/2 ;
   for(i=0; i<middle; i++){
     left[i]=array[i];
     ind_left[i]= ind[i];
   }
   for(i=middle; i<n_ele; i++){
     right[i-middle]=array[i];
     ind_right[i-middle]= ind[i];
   }

   Merge_Sort( middle,  left, ind_left);
   Merge_Sort(n_ele-middle, right, ind_right);

   if( left[middle-1] > right[0]) {
     Merge(middle, left,ind_left, n_ele-middle, right, ind_right, array, ind);
   }  else {
     //  do the appending
     for(i=0; i<middle; i++){
       array[i]=left[i];
       ind[i]=ind_left[i];
       
     }
     for(i=middle; i<n_ele; i++){
       array[i]=right[i-middle];
       ind[i]=ind_right[i-middle];
     }
   }


}


//----------end of function PndSttMvdTracking::Merge_Sort



//----------begin of function PndSttMvdTracking::Merge




void PndSttMvdTracking::Merge(UShort_t nl, Double_t *left, Int_t *ind_left, UShort_t nr,
                                         Double_t *right, Int_t *ind_right,  Double_t *result, Int_t *ind)
{
   UShort_t i =0, j, nl_curr=0, nr_curr=0;

   while( nl > 0 && nr >0){
     if( left[nl_curr] <= right[nr_curr]){
      result[i] =  left[nl_curr];
      ind[i] = ind_left[nl_curr];
      nl--;
      nl_curr++;
     } else {
      result[i] =  right [nr_curr];
      ind[i] =  ind_right [nr_curr];
      nr--;
      nr_curr++;
     }
    i++;
   }
//--------------------
   if( nl ==0) {
     for(j=0; j<nr; j++){
      result[i+j]= right[nr_curr+j];
      ind[i+j]= ind_right[nr_curr+j];
     }
   }   else {
     for(j=0; j<nl; j++){
      result[i+j]= left[nl_curr+j];
      ind[i+j]= ind_left[nl_curr+j];
     }
   }





}

//----------end of function PndSttMvdTracking::Merge


//----------begin of function PndSttMvdTracking::FitHelixCylinder

 Short_t PndSttMvdTracking::FitHelixCylinder(
		UShort_t nHitsinTrack,
		Double_t *Xconformal,
		Double_t *Yconformal,
		Double_t *DriftRadiusconformal,
		Double_t *ErrorDriftRadiusconformal,
		Double_t rotationangle,
		Double_t * trajectory_vertex,
		UShort_t NMAX,
		Double_t *emme,
		Double_t *qu,
		Double_t *pAlfa,
		Double_t *pBeta,
		Double_t *pGamma,
		bool *Type
					)
{


    //   definition of variables for the glpsol  solver
   //    ROWS (for read_rows  function)
   //
   UShort_t  NpointsInFit = nHitsinTrack-NMAX <0 ?  nHitsinTrack :  NMAX;
//   if (NpointsInFit>nmaxSttHitsInTrack+nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack)
//	NpointsInFit=nmaxSttHitsInTrack+nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack;
//   bool mvdhit[nmaxSttHitsInTrack+nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack];
   bool mvdhit[NpointsInFit];




     Double_t M = 1.,
              m_result,
              q_result,
              A,
              alfetta,
              angle,
              offsety,
              Delta[NpointsInFit],
              Ox[NpointsInFit],
              Oy[NpointsInFit];

     UShort_t  i, j, ii, iii, nSttHits, nMvdHits;
     Short_t Status;

     char nome[300], stringa[300], stringa2[300];
     float m1_result,m2_result, q1_result,q2_result, A1_result, A2_result;

// --

//-------------- stampaggi
if(istampa>=3){
	cout<<"from FitHelixCylinder,prima di rotazione,  Evento "<<IVOLTE<<", nHitsinTrack = "<<nHitsinTrack
	<<"\nfrom FitHelixCylinder, nPointsinFit = "<<NpointsInFit<<endl;
	for(i=0 ; i< NpointsInFit ; i++) {
		cout<<"  Xconformal["<<i<<"] = "<<Xconformal[ i ]<<
		";   Yconformal["<<i<<"] = "<<Yconformal[ i ]<<",  drift radius conformal "<<
		DriftRadiusconformal[i]<<endl<<"\tErrordiriftradiusconformal = "
		<<ErrorDriftRadiusconformal[i]<<endl;
	}
}
//------------ end stampaggi



     if( nHitsinTrack < 2) {
        return -1;
     }
//  use the trick of increasing the rotation angle by 10 degrees in order to obtain always a positive m
      rotationangle -= PI/18.;

      Double_t cose = cos(rotationangle), sine = sin(rotationangle);



	nSttHits = nMvdHits = 0;
      for(i=0;i<NpointsInFit; i++){
       Ox[i] = Xconformal[ i ] *cose +
               Yconformal[ i ]*sine;
       Oy[i] = -Xconformal[ i ] *sine +
               Yconformal[ i ]*cose;
          Delta[i] = 3.*ErrorDriftRadiusconformal[ i ];   //   3 times the Drift Radius

	if( DriftRadiusconformal[ i ]<0. )
	{
		mvdhit[i]=true;
		nMvdHits++;
	} else {
		mvdhit[i]=false;
		nSttHits++;
	}
      }

//-------------- stampaggi
if(istampa>=3){
	cout<<"from FitHelixCylinder, dopo rotazione, Evento "<<IVOLTE<<", nHitsinTrack = "<<nHitsinTrack
	<<"\nfrom FitHelixCylinder, nPointsinFit = "<<NpointsInFit<<endl;
	for(i=0 ; i< NpointsInFit ; i++) {
		cout<<"  Ox["<<i<<"] = "<<Ox[ i ]<<
		";   Oy["<<i<<"] = "<<Oy[ i ]<<",  Delta "<<
		Delta[i]<<endl;
	}
}
//------------ end stampaggi
//--------- calculation of # structural variables (see Gianluigi's logbook pag. 236) etc.etc.

	int NStructVar =     4           +    1       +  nMvdHits * 2     +                nSttHits *4 ;
//               m1,m2,q1,q2     DUMMY     lam & sigma      lamp & lamm & sigmap & sigmam


	int nRows =     1    +  nMvdHits * 4  +               nSttHits * 9;
//	         OBJECT     A,B,C,D        Ap,Bp,Cp,Dp,Am,Bm,Cm,Dm,LAMBDA

//----  creating the various service arrays
	int typeRows[nRows];
	char * nameRows[nRows];
	char  auxnameRows[nRows][20];

	int  NStructRowsMax = 8*NpointsInFit ;  //  maximum number of ROWS in which a
				//  structural variable (for instance M ) can be found
	double final_values[NStructVar];
	int  NRowsInWhichStructVarArePresent[NStructVar];
	char *StructVarName[NStructVar];
	char auxStructVarName[NStructVar][20];
	char *NameRowsInWhichStructVarArePresent[NStructVar*NStructRowsMax];
	char aux[NStructVar*NStructRowsMax][20];
	double Coefficients[NStructVar*NStructRowsMax];

	//--------for RHS information
	double ValueB[nRows-1]; // -1 because OBJECT dowsn't have a RHS boundary.
	//--------for RANGES information
	int nRanges = nSttHits;
	double ValueRanges[nRanges];
	char *NameRanges[nRanges];
	char auxNameRanges[nRanges][20];

	//--------for BOUNDS information
	int nBounds=NpointsInFit+nSttHits+1;
	double BoundValue[nBounds];
	char *BoundStructVarName[nBounds];
	char auxBoundStructVarName[nBounds][20];
	char *TypeofBound[nBounds];
	char auxTypeofBound[nBounds][20];
	//--------end BOUNDS information

//---------------------------------------------------




//--- calculate array      NRowsInWhichStructVarArePresent
        NRowsInWhichStructVarArePresent[0] =	//  this is for m1
        NRowsInWhichStructVarArePresent[1] = 	//  this is for m2
        NRowsInWhichStructVarArePresent[2] =	//  this is for q1
        NRowsInWhichStructVarArePresent[3] = nMvdHits*2 + nSttHits *4;	//  this is for q2
	//--- the following is for the  lam* (Mvd hits) or lamp* (Stt hits) structural variables
      for(i=0,ii=0; i< NpointsInFit ; i++) {
		if( mvdhit[i]){
			NRowsInWhichStructVarArePresent[4+ii]= 4;
		} else {
			NRowsInWhichStructVarArePresent[4+ii]= 5;
		}
		ii++;
	}
	//--- the following is for the  lamm* (Mvd Hits, if any)  structural variables
      for(i=0; i< NpointsInFit ; i++) {
		if( !mvdhit[i]){
			NRowsInWhichStructVarArePresent[4+ii]= 5;
			ii++;
		}
	}

	//--- the following is for the  sigma   structural variables
	for(i=0 ; i< nMvdHits+2*nSttHits ; i++) {
		NRowsInWhichStructVarArePresent[4+ii+i]= 5;
	}
//--- the following is for the    DUMMY    structural variable
	NRowsInWhichStructVarArePresent[4+ii+nMvdHits+2*nSttHits]= nMvdHits*4 + nSttHits*8;




//-----------------  write the ROWS  section






      sprintf(&(auxnameRows[0][0]),"OBJECT",i);  nameRows[0]=&auxnameRows[0][0];
      typeRows[0]=GLP_FR;
      for(i=0 , ii=0 ; i< NpointsInFit ; i++) {

	if( mvdhit[i]){
       typeRows[1+ii]=GLP_UP;typeRows[2+ii]=GLP_UP;typeRows[3+ii]=GLP_UP;typeRows[4+ii]=GLP_UP;
       typeRows[5+ii]=GLP_LO;

       sprintf(&(auxnameRows[1+ii][0]),"A%d",i);  nameRows[1+ii]=&auxnameRows[1+ii][0];
       sprintf(&(auxnameRows[2+ii][0]),"B%d",i);  nameRows[2+ii]=&auxnameRows[2+ii][0];
       sprintf(&(auxnameRows[3+ii][0]),"C%d",i);  nameRows[3+ii]=&auxnameRows[3+ii][0];
       sprintf(&(auxnameRows[4+ii][0]),"D%d",i);  nameRows[4+ii]=&auxnameRows[4+ii][0];
         ii+=4;

	} else {
         typeRows[1+ii]=GLP_UP;typeRows[2+ii]=GLP_UP;typeRows[3+ii]=GLP_UP;typeRows[4+ii]=GLP_UP;
         typeRows[5+ii]=GLP_UP;typeRows[6+ii]=GLP_UP;typeRows[7+ii]=GLP_UP;typeRows[8+ii]=GLP_UP;
         typeRows[9+ii]=GLP_LO;

         sprintf(&(auxnameRows[1+ii][0]),"Ap%d",i);  nameRows[1+ii]=&auxnameRows[1+ii][0];
         sprintf(&(auxnameRows[2+ii][0]),"Bp%d",i);  nameRows[2+ii]=&auxnameRows[2+ii][0];
         sprintf(&(auxnameRows[3+ii][0]),"Cp%d",i);  nameRows[3+ii]=&auxnameRows[3+ii][0];
         sprintf(&(auxnameRows[4+ii][0]),"Dp%d",i);  nameRows[4+ii]=&auxnameRows[4+ii][0];
         sprintf(&(auxnameRows[5+ii][0]),"Am%d",i);  nameRows[5+ii]=&auxnameRows[5+ii][0];
         sprintf(&(auxnameRows[6+ii][0]),"Bm%d",i);  nameRows[6+ii]=&auxnameRows[6+ii][0];
         sprintf(&(auxnameRows[7+ii][0]),"Cm%d",i);  nameRows[7+ii]=&auxnameRows[7+ii][0];
         sprintf(&(auxnameRows[8+ii][0]),"Dm%d",i);  nameRows[8+ii]=&auxnameRows[8+ii][0];
         sprintf(&(auxnameRows[9+ii][0]),"LAMBDA%d",i);  nameRows[9+ii]=&auxnameRows[9+ii][0];
         ii+=9;
	}
      }




//-----------------  write the COLUMNS  section

//      fprintf(FMCS,"COLUMNS\n");  //--------stampaggi

//  Column variable  m1

      ii=0;
      for(i=0  ; i< NpointsInFit ; i++) {

	if( mvdhit[i]){
//---stampaggi
//          fprintf(FMCS,"  m1 A%d   %g\n  m1 B%d  %g\n",i,Ox[i],i,-Ox[i]);
//-----stampaggi, fine

         Coefficients[ii]=    Ox[i];
         Coefficients[ii+1]= -Ox[i];
	 ii +=2;
	} else {
//---stampaggi
//          fprintf(FMCS,"  m1 Ap%d  %g  Am%d  %g\n  m1 Bp%d  %g   Bm%d  %g\n",
//                                  i,Ox[i],i,Ox[i],i,-Ox[i],i,-Ox[i]);
//-----stampaggi, fine

         Coefficients[ii]=    Ox[i];
         Coefficients[ii+1]=  Ox[i];
         Coefficients[ii+2]= -Ox[i];
         Coefficients[ii+3]= -Ox[i];
	 ii += 4;
	}
      }



//  Column variable  m2
      for(i=0, ii=0; i< NpointsInFit ; i++) {
	if( mvdhit[i]){
//          fprintf(FMCS,"  m2 A%d  %g\n  m2 B%d  %g\n",//---stampaggi
//                                  i,-Ox[i],i,Ox[i]);//---stampaggi
         Coefficients[NStructRowsMax+ii]=   -Ox[i];
         Coefficients[NStructRowsMax+ii+1]= Ox[i];
	 ii += 2;
	} else {
//          fprintf(FMCS,"  m2 Ap%d  %g  Am%d  %g\n  m2 Bp%d  %g   Bm%d  %g\n",//---stampaggi
//                                  i,-Ox[i],i,-Ox[i],i,Ox[i],i,Ox[i]);//---stampaggi
         Coefficients[NStructRowsMax+ii]=   -Ox[i];
         Coefficients[NStructRowsMax+ii+1]= -Ox[i];
         Coefficients[NStructRowsMax+ii+2]= Ox[i];
         Coefficients[NStructRowsMax+ii+3]= Ox[i];
	 ii += 4;
	}
      }

//  Column variable  q1
      for(i=0, ii=0 ; i< NpointsInFit ; i++) {
	if( mvdhit[i]){
//          fprintf(FMCS,"  q1 A%d   1.\n  q1 B%d  -1.\n",//---stampaggi
//                                  i,i);//---stampaggi
         Coefficients[2*NStructRowsMax+ii]=    1.;
         Coefficients[2*NStructRowsMax+ii+1]= -1.;
	 ii +=2;
	} else {
//          fprintf(FMCS,"  q1 Ap%d   1.  Am%d   1.\n  q1 Bp%d  -1.  Bm%d  -1.\n",//---stampaggi
//                                  i,i,i,i);//---stampaggi
         Coefficients[2*NStructRowsMax+ii]=    1.;
         Coefficients[2*NStructRowsMax+ii+1]=  1.;
         Coefficients[2*NStructRowsMax+ii+2]= -1.;
         Coefficients[2*NStructRowsMax+ii+3]= -1.;
	 ii += 4;
	}
      }

//  Column variable  q2
      for(i=0, ii=0 ; i< NpointsInFit ; i++) {
	if( mvdhit[i]){
//          fprintf(FMCS,"  q2 A%d   -1.\n  q2 B%d   1.\n",//---stampaggi
//                                  i,i);//---stampaggi
          Coefficients[3*NStructRowsMax+ii]=   -1.;
          Coefficients[3*NStructRowsMax+ii+1]=  1.;
	  ii += 2;
	} else {
//          fprintf(FMCS,"  q2 Ap%d   -1.  Am%d   -1.\n  q2 Bp%d   1.   Bm%d   1.\n",//---stampaggi
//                                  i,i,i,i);//---stampaggi
          Coefficients[3*NStructRowsMax+ii]=   -1.;
          Coefficients[3*NStructRowsMax+ii+1]= -1.;
          Coefficients[3*NStructRowsMax+ii+2]=  1.;
          Coefficients[3*NStructRowsMax+ii+3]=  1.;
	  ii += 4;
	}

      }

//  Column variable  lambdap(i)
      for(i=0 ; i< NpointsInFit ; i++) {
         ii=(4+i)*NStructRowsMax;
         Coefficients[ii]= -M;
         Coefficients[ii+1]= -M;
         Coefficients[ii+2]= -M;
         Coefficients[ii+3]=  M;
//	if( mvdhit[i]){
//  fprintf(FMCS,"  lam%d  A%d  %g  B%d  %g\n  lam%d  C%d  %g  D%d   %g\n",//---stampaggi
//                      i,i,-M,i,-M, i , i,-M, i, M);//---stampaggi
//	} else {
//  fprintf(FMCS,"  lamp%d  Ap%d  %g  Bp%d  %g\n  lamp%d  Cp%d  %g  Dp%d   %g\n  lamp%d  LAMBDA%d  1.\n",//---stampaggi
//                      i,i,-M,i,-M, i , i,-M, i, M, i,i);//---stampaggi
//	}

	if(! mvdhit[i]) Coefficients[ii+4]=  1.;

      }



//  Column variable  lambdam(i)
      for(i=0 ; i< NpointsInFit ; i++) {
	if( mvdhit[i]) continue;
	 ii+= NStructRowsMax;
//         fprintf(FMCS,"  lamm%d  Am%d  %g  Bm%d  %g\n  lamm%d  Cm%d  %g  Dm%d %g\n  lamm%d  LAMBDA%d  1.\n",//---stampaggi
//                                                i,i,-M,i,-M, i,i , -M, i, M, i,i);//---stampaggi
         Coefficients[ii]= -M;
         Coefficients[ii+1]= -M;
         Coefficients[ii+2]= -M;
         Coefficients[ii+3]=  M;
         Coefficients[ii+4]=  1.;
      }
//  Column variable  sigmap(i)
      for(i=0; i< NpointsInFit ; i++) {
	ii+= NStructRowsMax;

/*
	if( mvdhit[i]){
      fprintf(FMCS,"  sigma%d  OBJECT  %g  A%d  -1.\n  sigma%d  B%d    -1. C%d  1.\n  sigma%d  D%d -1.\n",//---stampaggi
                                                i,1./Delta[i],i,i,i,i,i,i);//---stampaggi

	} else {
      fprintf(FMCS,"  sigmap%d  OBJECT  %g  Ap%d  -1.\n  sigmap%d  Bp%d    -1. Cp%d  1.\n  sigmap%d  Dp%d -1.\n",//---stampaggi
                                                i,1./Delta[i],i,i,i,i,i,i);//---stampaggi
	}
*/

        Coefficients[ii]=  1./Delta[i];
        Coefficients[ii+1]= -1.;
        Coefficients[ii+2]= -1.;
        Coefficients[ii+3]=  1.;
        Coefficients[ii+4]= -1.;


      }
//  Column variable  sigmam(i)
      for(i=0 ; i< NpointsInFit ; i++) {
	if( mvdhit[i])continue;
	 ii+= NStructRowsMax;
//         fprintf(FMCS,"  sigmam%d  OBJECT %g  Am%d  -1.\n  sigmam%d  Bm%d   -1. Cm%d   1.\n  sigmam%d  Dm%d  -1.\n",//---stampaggi
//                                                i,1./Delta[i],i,i,i,i,i,i);//---stampaggi
        Coefficients[ii]=  1./Delta[i];
        Coefficients[ii+1]= -1.;
        Coefficients[ii+2]= -1.;
        Coefficients[ii+3]=  1.;
        Coefficients[ii+4]= -1.;
      }

//  Column variable  DUMMY
//-----------stampaggi
/*
      for(i=0 ; i< NpointsInFit ; i++) {
	if( mvdhit[i]){
         fprintf(FMCS,"  DUMMY     A%d  1.\n  DUMMY   B%d   1.\n  DUMMY    C%d   1.\n",i,i,i);
         fprintf(FMCS,"  DUMMY     D%d  1.\n",i);
	} else {
         fprintf(FMCS,"  DUMMY     Ap%d  1.      Am%d       1.\n  DUMMY   Bp%d   1.    Bm%d   1.\n  DUMMY    Cp%d   1.   Cm%d   1\n",
                                                i,i,i,i,i,i);
         fprintf(FMCS,"  DUMMY     Dp%d  1.      Dm%d       1.\n",i,i);
	}
      }
*/
//---fine stampaggi
	ii+= NStructRowsMax;
      for(i=0, iii=0 ; i< NpointsInFit ; i++) {
	if( mvdhit[i]){
	 Coefficients[ii+iii]= 1.;
	 Coefficients[ii+iii+1]= 1.;
	 Coefficients[ii+iii+2]= 1.;
 	 Coefficients[ii+iii+3]= 1.;
	 iii +=4;
	} else {
	 Coefficients[ii+iii]= 1.;
	 Coefficients[ii+iii+1]= 1.;
	 Coefficients[ii+iii+2]= 1.;
 	 Coefficients[ii+iii+3]= 1.;
	 Coefficients[ii+iii+4]= 1.;
	 Coefficients[ii+iii+5]= 1.;
	 Coefficients[ii+iii+6]= 1.;
 	 Coefficients[ii+iii+7]= 1.;
	 iii +=8;
	}
      }

//-----------

//--- give the names to the Structural Variables

      sprintf(&auxStructVarName[0][0],"m1",i);
      StructVarName[0] = &auxStructVarName[0][0];
      sprintf(&auxStructVarName[1][0],"m2",i);
      StructVarName[1] = &auxStructVarName[1][0];

      sprintf(&auxStructVarName[2][0],"q1",i);
      StructVarName[2] = &auxStructVarName[2][0];

      sprintf(&auxStructVarName[3][0],"q2",i);
      StructVarName[3] = &auxStructVarName[3][0];
      for(i=0, ii=0; i< NpointsInFit ; i++) {
	if( mvdhit[i]){
          sprintf(&auxStructVarName[4+i][0],"lam%d",i);
          StructVarName[4+i] = &auxStructVarName[4+i][0];

          sprintf(&auxStructVarName[4+nMvdHits+2*nSttHits+i][0],"sigma%d",i);
          StructVarName[4+nMvdHits+2*nSttHits+i] = &auxStructVarName[4+nMvdHits+2*nSttHits+i][0];
	} else {
          sprintf(&auxStructVarName[4+i][0],"lamp%d",i);
          StructVarName[4+i] = &auxStructVarName[4+i][0];

          sprintf(&auxStructVarName[4+NpointsInFit+ii][0],"lamm%d",i);
          StructVarName[4+NpointsInFit+ii] = &auxStructVarName[4+NpointsInFit+ii][0];

          sprintf(&auxStructVarName[4+nMvdHits+2*nSttHits+i][0],"sigmap%d",i);
          StructVarName[4+nMvdHits+2*nSttHits+i] = &auxStructVarName[4+nMvdHits+2*nSttHits+i][0];

          sprintf(&auxStructVarName[4+NpointsInFit+nMvdHits+2*nSttHits+ii][0],"sigmam%d",i);
          StructVarName[4+NpointsInFit+nMvdHits+2*nSttHits+ii] =
		 &auxStructVarName[4+NpointsInFit+nMvdHits+2*nSttHits+ii][0];
	  ii++;
	}
      }


      sprintf(&auxStructVarName[NStructVar-1][0],"DUMMY",i);
      StructVarName[NStructVar-1] = &auxStructVarName[NStructVar-1][0];



//--- give the names of those Rows in which the Structural Variables are present

//  for m1, m2, q1, q2
      for(i=0; i< 4; i++){
        for(j=0, ii=0; j< NpointsInFit;j++){
		if( mvdhit[j]){
         sprintf(&aux[i*NStructRowsMax+ii][0],"A%d",j);
         NameRowsInWhichStructVarArePresent[i*NStructRowsMax+ii]=&aux[i*NStructRowsMax+ii][0];
         sprintf(&aux[i*NStructRowsMax+ii+1][0],"B%d",j);
         NameRowsInWhichStructVarArePresent[i*NStructRowsMax+ii+1]=&aux[i*NStructRowsMax+ii+1][0];
	 ii += 2;
		} else {
         sprintf(&aux[i*NStructRowsMax+ii][0],"Ap%d",j);
         NameRowsInWhichStructVarArePresent[i*NStructRowsMax+ii]=&aux[i*NStructRowsMax+ii][0];
         sprintf(&aux[i*NStructRowsMax+ii+1][0],"Am%d",j);
         NameRowsInWhichStructVarArePresent[i*NStructRowsMax+ii+1]=&aux[i*NStructRowsMax+ii+1][0];
         sprintf(&aux[i*NStructRowsMax+ii+2][0],"Bp%d",j);
         NameRowsInWhichStructVarArePresent[i*NStructRowsMax+ii+2]=&aux[i*NStructRowsMax+ii+2][0];
         sprintf(&aux[i*NStructRowsMax+ii+3][0],"Bm%d",j);
         NameRowsInWhichStructVarArePresent[i*NStructRowsMax+ii+3]=&aux[i*NStructRowsMax+ii+3][0];
	 ii += 4;
		}
        }
      }

//  now for the    lamp*   variables
      for(i=0; i< NpointsInFit;i++){
		if( mvdhit[i]){
         sprintf(&aux[(i+4)*NStructRowsMax+0][0],"A%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRowsMax+0]= &aux[(i+4)*NStructRowsMax+0][0];
         sprintf(&aux[(i+4)*NStructRowsMax+1][0],"B%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRowsMax+1]= &aux[(i+4)*NStructRowsMax+1][0];
         sprintf(&aux[(i+4)*NStructRowsMax+2][0],"C%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRowsMax+2]= &aux[(i+4)*NStructRowsMax+2][0];
         sprintf(&aux[(i+4)*NStructRowsMax+3][0],"D%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRowsMax+3]= &aux[(i+4)*NStructRowsMax+3][0];
		} else {
         sprintf(&aux[(i+4)*NStructRowsMax+0][0],"Ap%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRowsMax+0]= &aux[(i+4)*NStructRowsMax+0][0];
         sprintf(&aux[(i+4)*NStructRowsMax+1][0],"Bp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRowsMax+1]= &aux[(i+4)*NStructRowsMax+1][0];
         sprintf(&aux[(i+4)*NStructRowsMax+2][0],"Cp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRowsMax+2]= &aux[(i+4)*NStructRowsMax+2][0];
         sprintf(&aux[(i+4)*NStructRowsMax+3][0],"Dp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRowsMax+3]= &aux[(i+4)*NStructRowsMax+3][0];
         sprintf(&aux[(i+4)*NStructRowsMax+4][0],"LAMBDA%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRowsMax+4]= &aux[(i+4)*NStructRowsMax+4][0];
		}
      }

//  now for the    lamm*   variables
      for(i=0, ii=0; i< NpointsInFit;i++){
	if( mvdhit[i]) continue;
         sprintf(&aux[(ii+4+NpointsInFit)*NStructRowsMax][0],"Am%d",i);
         NameRowsInWhichStructVarArePresent[(ii+4+NpointsInFit)*NStructRowsMax]=
		&aux[(ii+4+NpointsInFit)*NStructRowsMax][0];
         sprintf(&aux[(ii+4+NpointsInFit)*NStructRowsMax+1][0],"Bm%d",i);
         NameRowsInWhichStructVarArePresent[(ii+4+NpointsInFit)*NStructRowsMax+1]=
		&aux[(ii+4+NpointsInFit)*NStructRowsMax+1][0];
         sprintf(&aux[(ii+4+NpointsInFit)*NStructRowsMax+2][0],"Cm%d",i);
         NameRowsInWhichStructVarArePresent[(ii+4+NpointsInFit)*NStructRowsMax+2]=
		&aux[(ii+4+NpointsInFit)*NStructRowsMax+2][0];
         sprintf(&aux[(ii+4+NpointsInFit)*NStructRowsMax+3][0],"Dm%d",i);
         NameRowsInWhichStructVarArePresent[(ii+4+NpointsInFit)*NStructRowsMax+3]=
		&aux[(ii+4+NpointsInFit)*NStructRowsMax+3][0];
         sprintf(&aux[(ii+4+NpointsInFit)*NStructRowsMax+4][0],"LAMBDA%d",i);
         NameRowsInWhichStructVarArePresent[(ii+4+NpointsInFit)*NStructRowsMax+4]=
		&aux[(ii+4+NpointsInFit)*NStructRowsMax+4][0];
	 ii++; 
     }

//  now for the    sigmap*   variables
      for(i=0; i< NpointsInFit;i++){
	if( mvdhit[i]) {
         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax][0],"OBJECT",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit+nSttHits)*NStructRowsMax]=
		 &aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax][0];
         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+1][0],"A%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+1]=
		 &aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+1][0];
         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+2][0],"B%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+2]=
		 &aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+2][0];
         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+3][0],"C%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+3]=
		 &aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+3][0];
         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+4][0],"D%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+4]=
		 &aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+4][0];
	} else {
         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax][0],"OBJECT",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit+nSttHits)*NStructRowsMax]=
		 &aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax][0];
         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+1][0],"Ap%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+1]=
		 &aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+1][0];
         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+2][0],"Bp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+2]=
		 &aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+2][0];
         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+3][0],"Cp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+3]=
		 &aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+3][0];
         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+4][0],"Dp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+4]=
		 &aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+4][0];
	}
      }

//  now for the    sigmam*   variables
      for(i=0, ii=0; i< NpointsInFit;i++){
	if( mvdhit[i]) continue;
         sprintf(&aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax][0],"OBJECT",i);
         NameRowsInWhichStructVarArePresent[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax]=
		 &aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax][0];

         sprintf(&aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+1][0],"Am%d",i);
         NameRowsInWhichStructVarArePresent[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+1]=
		 &aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+1][0];

         sprintf(&aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+2][0],"Bm%d",i);
         NameRowsInWhichStructVarArePresent[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+2]=
		 &aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+2][0];

         sprintf(&aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+3][0],"Cm%d",i);
         NameRowsInWhichStructVarArePresent[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+3]=
		 &aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+3][0];

         sprintf(&aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+4][0],"Dm%d",i);
         NameRowsInWhichStructVarArePresent[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+4]=
		 &aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+4][0];
	 ii++; 
      }

//  now for the    DUMMY   variable
      for(i=0, ii=0; i< NpointsInFit;i++){
	if( mvdhit[i]) {
         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii][0],"A%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii][0];

         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii+1][0],"B%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii+1]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii+1][0];

         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii+2][0],"C%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii+2]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii+2][0];

         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii+3][0],"D%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii+3]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii+3][0];
	 ii += 4;
	} else {
         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii][0],"Ap%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii][0];

         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii+1][0],"Am%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii+1]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii+1][0];

         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii+2][0],"Bp%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii+2]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii+2][0];

         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii+3][0],"Bm%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii+3]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii+3][0];
         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii+4][0],"Cp%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii+4]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii+4][0];

         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii+5][0],"Cm%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii+5]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii+5][0];

         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii+6][0],"Dp%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii+6]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii+6][0];

         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii+7][0],"Dm%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii+7]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii+7][0];
	 ii += 8;
	}
      }



//-----------------  write the RHS  section

//      fprintf(FMCS,"RHS\n");//---stampaggi
      for(i=0, ii=0 ; i< NpointsInFit ; i++) {
	if( mvdhit[i]){
//---stampaggi
/*
          fprintf(FMCS,"  BOUND  A%d  %g  B%d  %g\n  BOUND  C%d  %g  D%d  %g\n",
              i, Oy[i]+2.*M,i,
                -Oy[i]+2.*M,i,
                 Delta[i]+2.*M,i,M-Delta[i]+2.*M);
*/
//---fine stampaggi
          ValueB[ii]  =  Oy[i]+2.*M;
          ValueB[ii+1]= -Oy[i]+2.*M;
          ValueB[ii+2]= Delta[i]+2.*M;
          ValueB[ii+3]= M-Delta[i]+2.*M;
	  ii += 4;
	} else {
//---stampaggi
/*
          fprintf(FMCS,"  BOUND  Ap%d  %g  Bp%d  %g\n  BOUND  Cp%d  %g  Dp%d  %g\n",
              i, Oy[i]+DriftRadiusconformal[ i ]+2.*M,i,
                -Oy[i]-DriftRadiusconformal[ i ]+2.*M,i,
                 Delta[i]+2.*M,i,M-Delta[i]+2.*M);
*/
//---fine stampaggi
          ValueB[ii]  =  Oy[i]+DriftRadiusconformal[ i ]+2.*M;
          ValueB[ii+1]= -Oy[i]-DriftRadiusconformal[ i ]+2.*M;
          ValueB[ii+2]= Delta[i]+2.*M;
          ValueB[ii+3]= M-Delta[i]+2.*M;


//---stampaggi
/*
          fprintf(FMCS,"  BOUND  Am%d  %g  Bm%d  %g\n  BOUND  Cm%d  %g  Dm%d %g\n",
              i, Oy[i]-DriftRadiusconformal[ i ]+2.*M,i,
                -Oy[i]+DriftRadiusconformal[ i ]+2.*M,i,
                 Delta[i]+2.*M,i,M-Delta[i]+2.*M);
          fprintf(FMCS,"  BOUND  LAMBDA%d   1.\n",i);
*/
//---fine stampaggi
          ValueB[ii+4]=  Oy[i]-DriftRadiusconformal[ i ]+2.*M;
          ValueB[ii+5]= -Oy[i]+DriftRadiusconformal[ i ]+2.*M;
          ValueB[ii+6]= Delta[i]+2.*M;
          ValueB[ii+7]= M-Delta[i]+2.*M;
          ValueB[ii+8]= 1.;
	  ii +=9;
	}

      }


//-----------------  write the RANGES  section

//      fprintf(FMCS,"RANGES\n");//---stampaggi
      for(i=0 , ii=0; i< NpointsInFit ; i++) {
	if( mvdhit[i]) continue;
//          fprintf(FMCS,"  RANGE  LAMBDA%d  1.\n",i);//---stampaggi
//---
        ValueRanges[ii]=1.;
        sprintf(&auxNameRanges[ii][0],"LAMBDA%d",i);
        NameRanges[ii]=&auxNameRanges[ii][0];
	ii++;
      }

//-----------------  write the BOUNDS  section
//      fprintf(FMCS,"BOUNDS\n");//---stampaggi


      for(i=0 ; i< NpointsInFit ; i++) {
	if( mvdhit[i]){
//          fprintf(FMCS," BV  Bounds  lam%d\n",  i);//---stampaggi
          sprintf(&auxTypeofBound[i][0],"BV");   TypeofBound[i]= &auxTypeofBound[i][0];
          sprintf(&auxBoundStructVarName[i][0],"lam%d",i);
	} else {
//          fprintf(FMCS," BV  Bounds  lamp%d\n",  i);//---stampaggi
          sprintf(&auxTypeofBound[i][0],"BV");   TypeofBound[i]= &auxTypeofBound[i][0];
          sprintf(&auxBoundStructVarName[i][0],"lamp%d",i);
	}

        BoundStructVarName[i]=&auxBoundStructVarName[i][0];
        BoundValue[i]=0.;

      }

      for(i=0, ii=0 ; i< NpointsInFit ; i++) {
	if( mvdhit[i]) continue;
//          fprintf(FMCS," BV  Bounds  lamm%d\n", i);//---stampaggi
          sprintf(&auxTypeofBound[ii+NpointsInFit][0],"BV");
          TypeofBound[ii+NpointsInFit]= &auxTypeofBound[ii+NpointsInFit][0];
          sprintf(&auxBoundStructVarName[ii+NpointsInFit][0],"lamm%d",i);
          BoundStructVarName[ii+NpointsInFit]=&auxBoundStructVarName[ii+NpointsInFit][0];
          BoundValue[ii+NpointsInFit]=0.;
	  ii++;
      }

//          fprintf(FMCS," FX  Bounds  DUMMY  %g\n",2.*M);//--stampaggi
          sprintf(&auxTypeofBound[NpointsInFit+nSttHits][0],"FX");
          TypeofBound[NpointsInFit+nSttHits]= &auxTypeofBound[NpointsInFit+nSttHits][0];

          sprintf(&auxTypeofBound[NpointsInFit+nSttHits][0],"FX");
          TypeofBound[NpointsInFit+nSttHits]= &auxTypeofBound[NpointsInFit+nSttHits][0];

          sprintf(&auxBoundStructVarName[NpointsInFit+nSttHits][0],"DUMMY");
          BoundStructVarName[NpointsInFit+nSttHits]=
		&auxBoundStructVarName[NpointsInFit+nSttHits][0];
          BoundValue[NpointsInFit+nSttHits]=2.*M;
//-----

//------------------------------------------------------------stampaggi
/*
      fprintf(FMCS,"ENDATA\n");
      fclose(FMCS);
*/

if(istampa>=4){

cout<<"n.  punti nel fit "<<NpointsInFit<<endl;


cout<<"nRows "<<nRows<<endl;
for(int ic =0;ic<nRows; ic++){
   cout<<"n.  Row  "<<ic<<", nameRows "<<nameRows[ic]<<",  typeRows "<<typeRows[ic]<<endl;
}

cout<<"NStructRowsMax, NStructVar "<<NStructRowsMax<<", "<<NStructVar<<endl;
for(int ic =0;ic<NStructVar; ic++){
   cout<<"NRowsInWhichStructVarArePresent  "<<NRowsInWhichStructVarArePresent[ic]
     <<", nome var. strut. n."<<ic<<"  = "
     <<StructVarName[ic]<<endl;

  for(int jc=0; jc<NRowsInWhichStructVarArePresent[ic];jc++){
   cout<<"n. "<<jc<<"  NameRowsInWhichStructVarArePresent  "
	<<NameRowsInWhichStructVarArePresent[ic*NStructRowsMax+jc]<<endl;
  }
}


cout<<"n Coefficient "<<21*nMvdHits+24*nSttHits<<endl;
iii=0;
for(int ic =0;ic<NStructVar; ic++){
   cout<<"Struct. Var."<< StructVarName[ic] <<" e' presente in "
	<< NRowsInWhichStructVarArePresent[ic]<<"  Rows;"<<endl;
  for(ii=0;ii<NRowsInWhichStructVarArePresent[ic];ii++){

   cout<<"\tin Row "<<NameRowsInWhichStructVarArePresent[ic*NStructRowsMax+ii]
         <<", ha  Coefficient   "<<Coefficients[ic*NStructRowsMax+ii]<<
	" (n. sequenziale = "<<iii<<")"<<endl;
	iii++;
  }
}

cout<<"n valuesB "<<nRows-1<<endl;
for(int ic =0;ic<nRows-1; ic++){
   cout<<"n. "<<ic<<",  valuesB   "<<ValueB[ic]<<endl;
}
cout<<"n ranges "<<nRanges<<endl;
for(int ic =0;ic<nRanges; ic++){
   cout<<"n. "<<ic<<",  RANGES   "<<ValueRanges[ic]<<endl;
}
cout<<"n Bounds "<<nBounds<<endl;
for(int ic =0;ic<nBounds; ic++){
   cout<<"n. "<<ic<<",  Bounds   "<<BoundValue[ic]<<endl;
   cout<<"n. "<<ic<<",  Bound Type   "<<TypeofBound[ic]<<endl;
   cout<<"n. "<<ic<<",  Bound Name   "<<BoundStructVarName[ic]<<endl;
}
 }	// end of if(istampa

//-------fine stampaggi

//-----------------------  funzioni chiamate direttamente
/*
cout<<"cavolo, da sttmvdtracking : nRows = "<<nRows<<", NStructVar = "<<
	NStructVar<<", NStructRowsMax = "<<NStructRowsMax<<
	", NRowsInWhichStructVarArePresent = "<<
	NRowsInWhichStructVarArePresent<<", nRanges = "<<nRanges
	<<", nBounds = "<<nBounds<<endl;
*/
      int status= glp_main(
            nRows,nameRows,typeRows, //  ROWS info
            NStructVar, NStructRowsMax, NRowsInWhichStructVarArePresent,  //  COLUMNS info
      StructVarName, NameRowsInWhichStructVarArePresent,  //  COLUMNS info
      Coefficients,  //  COLUMNS info
      ValueB,  // RHS  info
      nRanges, ValueRanges, NameRanges, //  RANGES  info
      nBounds, BoundValue, BoundStructVarName, TypeofBound //  BOUNDS info
//      ,final_values,TIMEOUT
      ,final_values
       );
	if(status != 0) return -5 ;


//--------stampaggi
if(istampa>=3){
printf("from FitHelixCylinder  printout dopo glp_main -------------------------------\n");
printf("      number of structural variables %d\n",NStructVar);
int ica;
for(ica=0;ica<NStructVar;ica++){
    printf("name of structural variable %s and its final value %g\n",
           StructVarName[ica], final_values[ica]);
}
printf("from FitHelixCylinder  printout dopo glp_main  -------------------------------\n");
}	// end of if(istampa
//--------fine stampaggi



//-----------------------  fine funzioni chiamate direttamente


/*
   if(istampa>=3 && IVOLTE <= 20){
     sprintf(stringa,
"/home/boca/panda/glpk/glpk-4.39/examples/glpsol --min -o soluztrack%dEvent%dstep%d    GeneralParallelHitsConformeTraccia%dEvent%d.mcs",
                                0,IVOLTE,1,0,IVOLTE);
   }  else {
     sprintf(stringa,
"/home/boca/panda/glpk/glpk-4.39/examples/glpsol --min -o soluztrack%dEvent%dstep%d    GeneralParallelHitsConformeTraccia%dEvent%d.mcs >& /dev/null",
                                0, IVOLTE,1,0,IVOLTE,1);
   }
*/

     m1_result = final_values[0];
     m2_result = final_values[1];
     q1_result = final_values[2];
     q2_result = final_values[3];

if(istampa>2) cout<<"Results : m1 = "<<m1_result<<", m2= "<<m2_result<<", q1 = "<<q1_result<<
	", q2 = "<<q2_result<<endl;

//---------  case in which the fit failed
	if( final_values[0]==0. && final_values[1]==0. && final_values[2]==0. &&final_values[3]==0. )
		return -10;

//------------------------  transformation of the result in terms of ALFA, BETA, GAMMA


     *qu = q1_result - q2_result;
     *emme = m1_result-m2_result ;

    *pGamma = 0.;
    if( fabs( *qu ) > 1.e-10) {    //  trajectory is a circle in XY space
     *pAlfa = *emme/(*qu);
     *pBeta = -1./(*qu);
     *Type=true;
//  now take into account the rotation and correct; the only affected quantities are ALFA and BETA
      alfetta = *pAlfa;
      *pAlfa = *pAlfa*cose - *pBeta*sine;
      *pBeta = alfetta*sine + *pBeta*cose;


    }  else if(fabs(*emme)> 1.e-10)  {//  trajectory is a straight line in XY space of equation y= m*x
       //  the rotation first
       angle = atan(*emme) + rotationangle;
       if( fabs(cos(angle)) > 1.e-10 ) {
         *pAlfa = 999999.;
         *pBeta = -(*pAlfa)/tan(angle);
         *Type=false;

       } else {  //  in this case the equation in XY plane is y = 0.
         *pAlfa = 0.;
         *pBeta = 999999.;
         *Type=false;
       }
    }  else {   //  in this case also the equation in XY plane is  y = 0.
         *pAlfa = 0.;
         *pBeta = 999999.;
         *Type=false;
    }	// end of 	if( fabs( *qu ) > 1.e-10)

//------------------


// now take into account the displacement and correct
      *pGamma += (trajectory_vertex[0]*trajectory_vertex[0]+ trajectory_vertex[1]*trajectory_vertex[1]
                                  -*pAlfa*trajectory_vertex[0]-*pBeta*trajectory_vertex[1]);
      *pAlfa -=  2.*trajectory_vertex[0];
      *pBeta -=  2.*trajectory_vertex[1];


      if(fabs(cose-*emme*sine)> 1.e-10) {
        *qu=*qu/(cose-*emme*sine);
        *emme=(*emme*cose+sine)/(cose-*emme*sine);
        return 1;
      } else {    //  in this case the equation is   0 = x+*qu .
        if(fabs(sine+*emme*cose) < 1.e-10)  {
  cout<<" From FitHelixCylinder, equation of XY circle : X**2 + Y**2 =0,"
	<<" situation impossible in principle! Returning -1"
                    <<endl;
           return -1;
        }

        *emme=1.;
        *qu = *qu/(sine+*emme*cose);
        return 99;    //  in this case the equation is   0 = x+*qu .
      }




}


//----------end of function PndSttMvdTracking::FitHelixCylinder




//----------begin of function PndSttMvdTracking::FitSZspace




     Short_t PndSttMvdTracking::FitSZspace(
			UShort_t nSkewHitsinTrack,
			Double_t *S,
			Double_t *Z, //
			Double_t *DriftRadius,
			Double_t *ErrorDriftRadius, //
			Double_t FInot,
			UShort_t NMAX,
			Double_t *emme
						)
{


    //   definition of variables for the glpsol  solver
   //    ROWS (for read_rows  function)
   //
   UShort_t  NpointsInFit = nSkewHitsinTrack-NMAX <0 ?  nSkewHitsinTrack :  NMAX;


   bool mvdhit[NpointsInFit];




   Double_t	ave,
		avex,
		avey,
		cose,
		sine,
		M = 50.,
		m_result,
		q_result,
		A,
		alfetta,
		angle,
		offsety,
		rotationangle,
		Ox[NpointsInFit],
		Oy[NpointsInFit],
		Delta[NpointsInFit];

     UShort_t  i, j, ii, iii, n, nSttHits, nMvdHits;
     Short_t Status;

     char nome[300], stringa[300], stringa2[300];
     float m1_result,m2_result, q1_result,q2_result, A1_result, A2_result;

// --

	if(nSkewHitsinTrack==0) {
	cout<<"from FitSZspace, Evento "<<IVOLTE<<endl;
		cout<<"from PndSttMvdTracking::FitSZspace  :  no points in fit, return!\n";
		return -10;
	}


/*
	ave=0.;
	avex=0.;
	avey=0.;
	n=0;
	for(i=0;i<nSkewHitsinTrack;i++){
		if( fabs(Z[ i ]) > 1.e-10&& DriftRadius[ i ]>0.){
			n++;
			ave += (S[ i ] - FInot)/Z[ i ];
			avex += Z[ i ];
			avey += (S[ i ] - FInot);
		}
	}

	if( n>0) {
		ave /=n;
		avex /=n;
		avey /=n;
		rotationangle = atan2(avey,avex);
	} else {
		rotationangle=PI/2.;
	}

*/



//  use the trick of increasing the rotation angle by 10 degrees in order to obtain always a positive m
//      rotationangle -= PI/18.;
      rotationangle = PI/2.;


      cose = cos(rotationangle);
      sine = sin(rotationangle);

      nSttHits = nMvdHits = 0;
 for(i=0;i<NpointsInFit; i++){
	Ox[i] =   Z[i]*cose +(S[i] - FInot)*sine;
	Oy[i] = -Z[i]*sine +(S[i] - FInot)*cose;
	Delta[i] = ErrorDriftRadius[i];
//          Delta[i] = 2.*DriftRadius[i];

	if( DriftRadius[ i ]<0. )
	{
		mvdhit[i]=true;
		nMvdHits++;
	} else {
		mvdhit[i]=false;
		nSttHits++;
	}
 }





//--------- calculation of # structural variables (see Gianluigi's logbook pag. 236) etc.etc.

	int NStructVar =     4           +    1       +  nMvdHits * 2     +                nSttHits *4 ;
//                  m1,m2,q1,q2     DUMMY      lam & sigma            lamp & lamm & sigmap & sigmam


	int nRows =     1    +  nMvdHits * 4  +               nSttHits * 9;
//	         OBJECT     A,B,C,D        Ap,Bp,Cp,Dp,Am,Bm,Cm,Dm,LAMBDA

//----  creating the various service arrays
	int typeRows[nRows];
	char * nameRows[nRows];
	char  auxnameRows[nRows][20];

	int  NStructRowsMax = 8*NpointsInFit ;  //  maximum number of ROWS in which a
				//  structural variable (for instance M ) can be found
	double final_values[NStructVar];
	int  NRowsInWhichStructVarArePresent[NStructVar];
	char *StructVarName[NStructVar];
	char auxStructVarName[NStructVar][20];
	char *NameRowsInWhichStructVarArePresent[NStructVar*NStructRowsMax];
	char aux[NStructVar*NStructRowsMax][20];
	double Coefficients[NStructVar*NStructRowsMax];

	//--------for RHS information
	double ValueB[nRows-1]; // -1 because OBJECT dowsn't have a RHS boundary.
	//--------for RANGES information
	int nRanges = nSttHits;
	double ValueRanges[nRanges];
	char *NameRanges[nRanges];
	char auxNameRanges[nRanges][20];

	//--------for BOUNDS information
	int nBounds=NpointsInFit+nSttHits+1+2;// +2  because q1 =  q2 = 0 fixed.
	double BoundValue[nBounds];
	char *BoundStructVarName[nBounds];
	char auxBoundStructVarName[nBounds][20];
	char *TypeofBound[nBounds];
	char auxTypeofBound[nBounds][20];
	//--------end BOUNDS information

//---------------------------------------------------




//--- calculate array      NRowsInWhichStructVarArePresent
        NRowsInWhichStructVarArePresent[0] =	//  this is for m1
        NRowsInWhichStructVarArePresent[1] = 	//  this is for m2
        NRowsInWhichStructVarArePresent[2] =	//  this is for q1
        NRowsInWhichStructVarArePresent[3] = nMvdHits*2 + nSttHits *4;	//  this is for q2
	//--- the following is for the  lam* (Mvd Hits) and lamp* (Stt hits) structural variables
      for(i=0,ii=0; i< NpointsInFit ; i++) {
		if( mvdhit[i]){
			NRowsInWhichStructVarArePresent[4+ii]= 4;
		} else {
			NRowsInWhichStructVarArePresent[4+ii]= 5;
		}
			ii++;
	}

	//  the lamm* (Stt hits) if any.
      for(i=0; i< NpointsInFit ; i++) {
		if( !mvdhit[i]){
			NRowsInWhichStructVarArePresent[4+ii]= 5;
			ii++;
		}
	}





	//--- the following is for the  sigma   structural variables
	for(i=0 ; i< nMvdHits+2*nSttHits ; i++) {
		NRowsInWhichStructVarArePresent[4+ii+i]= 5;
	}
//--- the following is for the    DUMMY    structural variable
	NRowsInWhichStructVarArePresent[4+ii+nMvdHits+2*nSttHits]= nMvdHits*4 + nSttHits*8;




//-----------------  write the ROWS  section


//-------stampaggi
/*
      sprintf(nome,"GeneralSkewEvent%d.mcs", IVOLTE);
      FILE * FMCS = fopen(nome,"w");
      fprintf(FMCS,"NAME    FIT\n");


      fprintf(FMCS,"ROWS\n");
      fprintf(FMCS," N OBJECT\n");
      for(i=0 ; i< NpointsInFit ; i++) {
	if( mvdhit[i]){
           fprintf(FMCS," L A%d\n L B%d\n L C%d\n L D%d\n",i,i,i,i);
	}else{
           fprintf(FMCS," L Ap%d\n L Bp%d\n L Cp%d\n L Dp%d\n",i,i,i,i);
           fprintf(FMCS," L Am%d\n L Bm%d\n L Cm%d\n L Dm%d\n G LAMBDA%d\n",i,i,i,i,i);
	}
      }
*/
//--------------------------fine stampaggi




      sprintf(&(auxnameRows[0][0]),"OBJECT",i);  nameRows[0]=&auxnameRows[0][0];
      typeRows[0]=GLP_FR;
      for(i=0 , ii=0 ; i< NpointsInFit ; i++) {

	if( mvdhit[i]){
       typeRows[1+ii]=GLP_UP;typeRows[2+ii]=GLP_UP;typeRows[3+ii]=GLP_UP;typeRows[4+ii]=GLP_UP;
       typeRows[5+ii]=GLP_LO;

       sprintf(&(auxnameRows[1+ii][0]),"A%d",i);  nameRows[1+ii]=&auxnameRows[1+ii][0];
       sprintf(&(auxnameRows[2+ii][0]),"B%d",i);  nameRows[2+ii]=&auxnameRows[2+ii][0];
       sprintf(&(auxnameRows[3+ii][0]),"C%d",i);  nameRows[3+ii]=&auxnameRows[3+ii][0];
       sprintf(&(auxnameRows[4+ii][0]),"D%d",i);  nameRows[4+ii]=&auxnameRows[4+ii][0];
         ii+=4;

	} else {
         typeRows[1+ii]=GLP_UP;typeRows[2+ii]=GLP_UP;typeRows[3+ii]=GLP_UP;typeRows[4+ii]=GLP_UP;
         typeRows[5+ii]=GLP_UP;typeRows[6+ii]=GLP_UP;typeRows[7+ii]=GLP_UP;typeRows[8+ii]=GLP_UP;
         typeRows[9+ii]=GLP_LO;

         sprintf(&(auxnameRows[1+ii][0]),"Ap%d",i);  nameRows[1+ii]=&auxnameRows[1+ii][0];
         sprintf(&(auxnameRows[2+ii][0]),"Bp%d",i);  nameRows[2+ii]=&auxnameRows[2+ii][0];
         sprintf(&(auxnameRows[3+ii][0]),"Cp%d",i);  nameRows[3+ii]=&auxnameRows[3+ii][0];
         sprintf(&(auxnameRows[4+ii][0]),"Dp%d",i);  nameRows[4+ii]=&auxnameRows[4+ii][0];
         sprintf(&(auxnameRows[5+ii][0]),"Am%d",i);  nameRows[5+ii]=&auxnameRows[5+ii][0];
         sprintf(&(auxnameRows[6+ii][0]),"Bm%d",i);  nameRows[6+ii]=&auxnameRows[6+ii][0];
         sprintf(&(auxnameRows[7+ii][0]),"Cm%d",i);  nameRows[7+ii]=&auxnameRows[7+ii][0];
         sprintf(&(auxnameRows[8+ii][0]),"Dm%d",i);  nameRows[8+ii]=&auxnameRows[8+ii][0];
         sprintf(&(auxnameRows[9+ii][0]),"LAMBDA%d",i);  nameRows[9+ii]=&auxnameRows[9+ii][0];
         ii+=9;
	}
      }




//-----------------  write the COLUMNS  section

//      fprintf(FMCS,"COLUMNS\n");  //--------stampaggi

//  Column variable  m1

      ii=0;
      for(i=0  ; i< NpointsInFit ; i++) {

	if( mvdhit[i]){
//---stampaggi
//          fprintf(FMCS,"  m1 A%d   %g\n  m1 B%d  %g\n",i,Ox[i],i,-Ox[i]);
//-----stampaggi, fine

         Coefficients[ii]=    Ox[i];
         Coefficients[ii+1]= -Ox[i];
	 ii +=2;
	} else {
//---stampaggi
//          fprintf(FMCS,"  m1 Ap%d  %g  Am%d  %g\n  m1 Bp%d  %g   Bm%d  %g\n",
//                                  i,Ox[i],i,Ox[i],i,-Ox[i],i,-Ox[i]);
//-----stampaggi, fine

         Coefficients[ii]=    Ox[i];
         Coefficients[ii+1]=  Ox[i];
         Coefficients[ii+2]= -Ox[i];
         Coefficients[ii+3]= -Ox[i];
	 ii += 4;
	}
      }



//  Column variable  m2
      for(i=0, ii=0; i< NpointsInFit ; i++) {
	if( mvdhit[i]){
//          fprintf(FMCS,"  m2 A%d  %g\n  m2 B%d  %g\n",//---stampaggi
//                                  i,-Ox[i],i,Ox[i]);//---stampaggi
         Coefficients[NStructRowsMax+ii]=   -Ox[i];
         Coefficients[NStructRowsMax+ii+1]= Ox[i];
	 ii += 2;
	} else {
//          fprintf(FMCS,"  m2 Ap%d  %g  Am%d  %g\n  m2 Bp%d  %g   Bm%d  %g\n",//---stampaggi
//                                  i,-Ox[i],i,-Ox[i],i,Ox[i],i,Ox[i]);//---stampaggi
         Coefficients[NStructRowsMax+ii]=   -Ox[i];
         Coefficients[NStructRowsMax+ii+1]= -Ox[i];
         Coefficients[NStructRowsMax+ii+2]= Ox[i];
         Coefficients[NStructRowsMax+ii+3]= Ox[i];
	 ii += 4;
	}
      }

//  Column variable  q1
      for(i=0, ii=0 ; i< NpointsInFit ; i++) {
	if( mvdhit[i]){
//          fprintf(FMCS,"  q1 A%d   1.\n  q1 B%d  -1.\n",//---stampaggi
//                                  i,i);//---stampaggi
         Coefficients[2*NStructRowsMax+ii]=    1.;
         Coefficients[2*NStructRowsMax+ii+1]= -1.;
	 ii +=2;
	} else {
//          fprintf(FMCS,"  q1 Ap%d   1.  Am%d   1.\n  q1 Bp%d  -1.  Bm%d  -1.\n",//---stampaggi
//                                  i,i,i,i);//---stampaggi
         Coefficients[2*NStructRowsMax+ii]=    1.;
         Coefficients[2*NStructRowsMax+ii+1]=  1.;
         Coefficients[2*NStructRowsMax+ii+2]= -1.;
         Coefficients[2*NStructRowsMax+ii+3]= -1.;
	 ii += 4;
	}
      }

//  Column variable  q2
      for(i=0, ii=0 ; i< NpointsInFit ; i++) {
	if( mvdhit[i]){
//          fprintf(FMCS,"  q2 A%d   -1.\n  q2 B%d   1.\n",//---stampaggi
//                                  i,i);//---stampaggi
          Coefficients[3*NStructRowsMax+ii]=   -1.;
          Coefficients[3*NStructRowsMax+ii+1]=  1.;
	  ii += 2;
	} else {
//          fprintf(FMCS,"  q2 Ap%d   -1.  Am%d   -1.\n  q2 Bp%d   1.   Bm%d   1.\n",//---stampaggi
//                                  i,i,i,i);//---stampaggi
          Coefficients[3*NStructRowsMax+ii]=   -1.;
          Coefficients[3*NStructRowsMax+ii+1]= -1.;
          Coefficients[3*NStructRowsMax+ii+2]=  1.;
          Coefficients[3*NStructRowsMax+ii+3]=  1.;
	  ii += 4;
	}

      }

//  Column variable  lambdap(i)
      for(i=0 ; i< NpointsInFit ; i++) {
         ii=(4+i)*NStructRowsMax;
         Coefficients[ii]= -M;
         Coefficients[ii+1]= -M;
         Coefficients[ii+2]= -M;
         Coefficients[ii+3]=  M;
/*
	if( mvdhit[i]){
  fprintf(FMCS,"  lam%d  A%d  %g  B%d  %g\n  lam%d  C%d  %g  D%d   %g\n",//---stampaggi
                      i,i,-M,i,-M, i , i,-M, i, M);//---stampaggi
	} else {
  fprintf(FMCS,"  lamp%d  Ap%d  %g  Bp%d  %g\n  lamp%d  Cp%d  %g  Dp%d   %g\n  lamp%d  LAMBDA%d  1.\n",//---stampaggi
                      i,i,-M,i,-M, i , i,-M, i, M, i,i);//---stampaggi
	}
*/
	if(! mvdhit[i]) Coefficients[ii+4]=  1.;

      }



//  Column variable  lambdam(i)
      for(i=0 ; i< NpointsInFit ; i++) {
	if( mvdhit[i]) continue;
	 ii+= NStructRowsMax;
//         fprintf(FMCS,"  lamm%d  Am%d  %g  Bm%d  %g\n  lamm%d  Cm%d  %g  Dm%d %g\n  lamm%d  LAMBDA%d  1.\n",//---stampaggi
//                     i,i,-M,i,-M, i,i , -M, i, M, i,i);//---stampaggi
         Coefficients[ii]= -M;
         Coefficients[ii+1]= -M;
         Coefficients[ii+2]= -M;
         Coefficients[ii+3]=  M;
         Coefficients[ii+4]=  1.;
      }
//  Column variable  sigmap(i)
      for(i=0; i< NpointsInFit ; i++) {
	ii+= NStructRowsMax;

/*
	if( mvdhit[i]){
      fprintf(FMCS,"  sigma%d  OBJECT  %g  A%d  -1.\n  sigma%d  B%d    -1. C%d  1.\n  sigma%d  D%d -1.\n",//---stampaggi
                                                i,1./Delta[i],i,i,i,i,i,i);//---stampaggi

	} else {
      fprintf(FMCS,"  sigmap%d  OBJECT  %g  Ap%d  -1.\n  sigmap%d  Bp%d    -1. Cp%d  1.\n  sigmap%d  Dp%d -1.\n",//---stampaggi
                                                i,1./Delta[i],i,i,i,i,i,i);//---stampaggi
	}
*/


        Coefficients[ii]=  1./Delta[i];
        Coefficients[ii+1]= -1.;
        Coefficients[ii+2]= -1.;
        Coefficients[ii+3]=  1.;
        Coefficients[ii+4]= -1.;


      }
//  Column variable  sigmam(i)
      for(i=0 ; i< NpointsInFit ; i++) {
	if( mvdhit[i])continue;
	 ii+= NStructRowsMax;
/*
  fprintf(FMCS,"  sigmam%d  OBJECT %g  Am%d  -1.\n  sigmam%d  Bm%d   -1. Cm%d   1.\n  sigmam%d  Dm%d  -1.\n",//---stampaggi
                                                i,1./Delta[i],i,i,i,i,i,i);//---stampaggi
*/
        Coefficients[ii]=  1./Delta[i];
        Coefficients[ii+1]= -1.;
        Coefficients[ii+2]= -1.;
        Coefficients[ii+3]=  1.;
        Coefficients[ii+4]= -1.;
      }

//  Column variable  DUMMY
//-----------stampaggi
/*
      for(i=0 ; i< NpointsInFit ; i++) {
	if( mvdhit[i]){
         fprintf(FMCS,"  DUMMY     A%d  1.\n  DUMMY   B%d   1.\n  DUMMY    C%d   1.\n",i,i,i);
         fprintf(FMCS,"  DUMMY     D%d  1.\n",i);
	} else {
         fprintf(FMCS,"  DUMMY     Ap%d  1.      Am%d       1.\n  DUMMY   Bp%d   1.    Bm%d   1.\n  DUMMY    Cp%d   1.   Cm%d   1\n",
                                                i,i,i,i,i,i);
         fprintf(FMCS,"  DUMMY     Dp%d  1.      Dm%d       1.\n",i,i);
	}
      }
*/
//---fine stampaggi
	ii+= NStructRowsMax;
      for(i=0, iii=0 ; i< NpointsInFit ; i++) {
	if( mvdhit[i]){
	 Coefficients[ii+iii]= 1.;
	 Coefficients[ii+iii+1]= 1.;
	 Coefficients[ii+iii+2]= 1.;
 	 Coefficients[ii+iii+3]= 1.;
	 iii +=4;
	} else {
	 Coefficients[ii+iii]= 1.;
	 Coefficients[ii+iii+1]= 1.;
	 Coefficients[ii+iii+2]= 1.;
 	 Coefficients[ii+iii+3]= 1.;
	 Coefficients[ii+iii+4]= 1.;
	 Coefficients[ii+iii+5]= 1.;
	 Coefficients[ii+iii+6]= 1.;
 	 Coefficients[ii+iii+7]= 1.;
	 iii +=8;
	}
      }

//-----------

//--- give the names to the Structural Variables

      sprintf(&auxStructVarName[0][0],"m1",i);
      StructVarName[0] = &auxStructVarName[0][0];
      sprintf(&auxStructVarName[1][0],"m2",i);
      StructVarName[1] = &auxStructVarName[1][0];

      sprintf(&auxStructVarName[2][0],"q1",i);
      StructVarName[2] = &auxStructVarName[2][0];

      sprintf(&auxStructVarName[3][0],"q2",i);
      StructVarName[3] = &auxStructVarName[3][0];
      for(i=0, ii=0; i< NpointsInFit ; i++) {
	if( mvdhit[i]){
          sprintf(&auxStructVarName[4+i][0],"lam%d",i);
          StructVarName[4+i] = &auxStructVarName[4+i][0];

          sprintf(&auxStructVarName[4+nMvdHits+2*nSttHits+i][0],"sigma%d",i);
          StructVarName[4+nMvdHits+2*nSttHits+i] = &auxStructVarName[4+nMvdHits+2*nSttHits+i][0];
	} else {
          sprintf(&auxStructVarName[4+i][0],"lamp%d",i);
          StructVarName[4+i] = &auxStructVarName[4+i][0];

          sprintf(&auxStructVarName[4+NpointsInFit+ii][0],"lamm%d",i);
          StructVarName[4+NpointsInFit+ii] = &auxStructVarName[4+NpointsInFit+ii][0];

          sprintf(&auxStructVarName[4+nMvdHits+2*nSttHits+i][0],"sigmap%d",i);
          StructVarName[4+nMvdHits+2*nSttHits+i] = &auxStructVarName[4+nMvdHits+2*nSttHits+i][0];

          sprintf(&auxStructVarName[4+NpointsInFit+nMvdHits+2*nSttHits+ii][0],"sigmam%d",i);
          StructVarName[4+NpointsInFit+nMvdHits+2*nSttHits+ii] =
		 &auxStructVarName[4+NpointsInFit+nMvdHits+2*nSttHits+ii][0];
	  ii++;
	}
      }


      sprintf(&auxStructVarName[NStructVar-1][0],"DUMMY",i);
      StructVarName[NStructVar-1] = &auxStructVarName[NStructVar-1][0];



//--- give the names of those Rows in which the Structural Variables are present

//  for m1, m2, q1, q2
      for(i=0; i< 4; i++){
        for(j=0, ii=0; j< NpointsInFit;j++){
		if( mvdhit[j]){
         sprintf(&aux[i*NStructRowsMax+ii][0],"A%d",j);
         NameRowsInWhichStructVarArePresent[i*NStructRowsMax+ii]=&aux[i*NStructRowsMax+ii][0];
         sprintf(&aux[i*NStructRowsMax+ii+1][0],"B%d",j);
         NameRowsInWhichStructVarArePresent[i*NStructRowsMax+ii+1]=&aux[i*NStructRowsMax+ii+1][0];
	 ii += 2;
		} else {
         sprintf(&aux[i*NStructRowsMax+ii][0],"Ap%d",j);
         NameRowsInWhichStructVarArePresent[i*NStructRowsMax+ii]=&aux[i*NStructRowsMax+ii][0];
         sprintf(&aux[i*NStructRowsMax+ii+1][0],"Am%d",j);
         NameRowsInWhichStructVarArePresent[i*NStructRowsMax+ii+1]=&aux[i*NStructRowsMax+ii+1][0];
         sprintf(&aux[i*NStructRowsMax+ii+2][0],"Bp%d",j);
         NameRowsInWhichStructVarArePresent[i*NStructRowsMax+ii+2]=&aux[i*NStructRowsMax+ii+2][0];
         sprintf(&aux[i*NStructRowsMax+ii+3][0],"Bm%d",j);
         NameRowsInWhichStructVarArePresent[i*NStructRowsMax+ii+3]=&aux[i*NStructRowsMax+ii+3][0];
	 ii += 4;
		}
        }
      }

//  now for the    lamp*   variables
      for(i=0; i< NpointsInFit;i++){
		if( mvdhit[i]){
         sprintf(&aux[(i+4)*NStructRowsMax+0][0],"A%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRowsMax+0]= &aux[(i+4)*NStructRowsMax+0][0];
         sprintf(&aux[(i+4)*NStructRowsMax+1][0],"B%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRowsMax+1]= &aux[(i+4)*NStructRowsMax+1][0];
         sprintf(&aux[(i+4)*NStructRowsMax+2][0],"C%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRowsMax+2]= &aux[(i+4)*NStructRowsMax+2][0];
         sprintf(&aux[(i+4)*NStructRowsMax+3][0],"D%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRowsMax+3]= &aux[(i+4)*NStructRowsMax+3][0];
		} else {
         sprintf(&aux[(i+4)*NStructRowsMax+0][0],"Ap%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRowsMax+0]= &aux[(i+4)*NStructRowsMax+0][0];
         sprintf(&aux[(i+4)*NStructRowsMax+1][0],"Bp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRowsMax+1]= &aux[(i+4)*NStructRowsMax+1][0];
         sprintf(&aux[(i+4)*NStructRowsMax+2][0],"Cp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRowsMax+2]= &aux[(i+4)*NStructRowsMax+2][0];
         sprintf(&aux[(i+4)*NStructRowsMax+3][0],"Dp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRowsMax+3]= &aux[(i+4)*NStructRowsMax+3][0];
         sprintf(&aux[(i+4)*NStructRowsMax+4][0],"LAMBDA%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRowsMax+4]= &aux[(i+4)*NStructRowsMax+4][0];
		}
      }

//  now for the    lamm*   variables
      for(i=0, ii=0; i< NpointsInFit;i++){
	if( mvdhit[i]) continue;
         sprintf(&aux[(ii+4+NpointsInFit)*NStructRowsMax][0],"Am%d",i);
         NameRowsInWhichStructVarArePresent[(ii+4+NpointsInFit)*NStructRowsMax]= &aux[(ii+4+NpointsInFit)*NStructRowsMax][0];
         sprintf(&aux[(ii+4+NpointsInFit)*NStructRowsMax+1][0],"Bm%d",i);
         NameRowsInWhichStructVarArePresent[(ii+4+NpointsInFit)*NStructRowsMax+1]= &aux[(ii+4+NpointsInFit)*NStructRowsMax+1][0];
         sprintf(&aux[(ii+4+NpointsInFit)*NStructRowsMax+2][0],"Cm%d",i);
         NameRowsInWhichStructVarArePresent[(ii+4+NpointsInFit)*NStructRowsMax+2]= &aux[(ii+4+NpointsInFit)*NStructRowsMax+2][0];
         sprintf(&aux[(ii+4+NpointsInFit)*NStructRowsMax+3][0],"Dm%d",i);
         NameRowsInWhichStructVarArePresent[(ii+4+NpointsInFit)*NStructRowsMax+3]= &aux[(ii+4+NpointsInFit)*NStructRowsMax+3][0];
         sprintf(&aux[(ii+4+NpointsInFit)*NStructRowsMax+4][0],"LAMBDA%d",i);
         NameRowsInWhichStructVarArePresent[(ii+4+NpointsInFit)*NStructRowsMax+4]= &aux[(ii+4+NpointsInFit)*NStructRowsMax+4][0];
	 ii++; 
     }

//  now for the    sigmap*   variables
      for(i=0; i< NpointsInFit;i++){
	if( mvdhit[i]) {
         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax][0],"OBJECT",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit+nSttHits)*NStructRowsMax]=
		 &aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax][0];
         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+1][0],"A%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+1]=
		 &aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+1][0];
         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+2][0],"B%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+2]=
		 &aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+2][0];
         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+3][0],"C%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+3]=
		 &aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+3][0];
         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+4][0],"D%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+4]=
		 &aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+4][0];
	} else {
         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax][0],"OBJECT",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit+nSttHits)*NStructRowsMax]=
		 &aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax][0];
         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+1][0],"Ap%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+1]=
		 &aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+1][0];
         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+2][0],"Bp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+2]=
		 &aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+2][0];
         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+3][0],"Cp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+3]=
		 &aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+3][0];
         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+4][0],"Dp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+4]=
		 &aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax+4][0];
	}
      }

//  now for the    sigmam*   variables
      for(i=0, ii=0; i< NpointsInFit;i++){
	if( mvdhit[i]) continue;
         sprintf(&aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax][0],"OBJECT",i);
         NameRowsInWhichStructVarArePresent[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax]=
		 &aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax][0];

         sprintf(&aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+1][0],"Am%d",i);
         NameRowsInWhichStructVarArePresent[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+1]=
		 &aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+1][0];

         sprintf(&aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+2][0],"Bm%d",i);
         NameRowsInWhichStructVarArePresent[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+2]=
		 &aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+2][0];

         sprintf(&aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+3][0],"Cm%d",i);
         NameRowsInWhichStructVarArePresent[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+3]=
		 &aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+3][0];

         sprintf(&aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+4][0],"Dm%d",i);
         NameRowsInWhichStructVarArePresent[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+4]=
		 &aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax+4][0];
	 ii++; 
      }

//  now for the    DUMMY   variable
      for(i=0, ii=0; i< NpointsInFit;i++){
	if( mvdhit[i]) {
         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii][0],"A%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii][0];

         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii+1][0],"B%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii+1]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii+1][0];

         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii+2][0],"C%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii+2]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii+2][0];

         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii+3][0],"D%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii+3]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii+3][0];
	 ii += 4;
	} else {
         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii][0],"Ap%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii][0];

         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii+1][0],"Am%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii+1]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii+1][0];

         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii+2][0],"Bp%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii+2]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii+2][0];

         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii+3][0],"Bm%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii+3]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii+3][0];
         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii+4][0],"Cp%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii+4]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii+4][0];

         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii+5][0],"Cm%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii+5]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii+5][0];

         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii+6][0],"Dp%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii+6]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii+6][0];

         sprintf(&aux[(NStructVar-1)*NStructRowsMax+ii+7][0],"Dm%d",i);
         NameRowsInWhichStructVarArePresent[(NStructVar-1)*NStructRowsMax+ii+7]=
		 &aux[(NStructVar-1)*NStructRowsMax+ii+7][0];
	 ii += 8;
	}
      }



//-----------------  write the RHS  section

//      fprintf(FMCS,"RHS\n");//---stampaggi
      for(i=0, ii=0 ; i< NpointsInFit ; i++) {
	if( mvdhit[i]){
//---stampaggi
/*
          fprintf(FMCS,"  BOUND  A%d  %g  B%d  %g\n  BOUND  C%d  %g  D%d  %g\n",
              i, Oy[i]+2.*M,i,
                -Oy[i]+2.*M,i,
                 Delta[i]+2.*M,i,M-Delta[i]+2.*M);
*/
//---fine stampaggi
          ValueB[ii]  =  Oy[i]+2.*M;
          ValueB[ii+1]= -Oy[i]+2.*M;
          ValueB[ii+2]= Delta[i]+2.*M;
          ValueB[ii+3]= M-Delta[i]+2.*M;
	  ii += 4;
	} else {
//---stampaggi
/*
          fprintf(FMCS,"  BOUND  Ap%d  %g  Bp%d  %g\n  BOUND  Cp%d  %g  Dp%d  %g\n",
              i, Oy[i]+DriftRadius[ i ]+2.*M,i,
                -Oy[i]-DriftRadius[ i ]+2.*M,i,
                 Delta[i]+2.*M,i,M-Delta[i]+2.*M);
*/
//---fine stampaggi
          ValueB[ii]  =  Oy[i]+DriftRadius[ i ]+2.*M;
          ValueB[ii+1]= -Oy[i]-DriftRadius[ i ]+2.*M;
          ValueB[ii+2]= Delta[i]+2.*M;
          ValueB[ii+3]= M-Delta[i]+2.*M;


//---stampaggi
/*
          fprintf(FMCS,"  BOUND  Am%d  %g  Bm%d  %g\n  BOUND  Cm%d  %g  Dm%d %g\n",
              i, Oy[i]-DriftRadius[ i ]+2.*M,i,
                -Oy[i]+DriftRadius[ i ]+2.*M,i,
                 Delta[i]+2.*M,i,M-Delta[i]+2.*M);f
          fprintf(FMCS,"  BOUND  LAMBDA%d   1.\n",i);
*/
//---fine stampaggi
          ValueB[ii+4]=  Oy[i]-DriftRadius[ i ]+2.*M;
          ValueB[ii+5]= -Oy[i]+DriftRadius[ i ]+2.*M;
          ValueB[ii+6]= Delta[i]+2.*M;
          ValueB[ii+7]= M-Delta[i]+2.*M;
          ValueB[ii+8]= 1.;
	  ii +=9;
	}

      }


//-----------------  write the RANGES  section

//      fprintf(FMCS,"RANGES\n");//---stampaggi
      for(i=0 , ii=0; i< NpointsInFit ; i++) {
	if( mvdhit[i]) continue;
//          fprintf(FMCS,"  RANGE  LAMBDA%d  1.\n",i);//---stampaggi
//---
        ValueRanges[ii]=1.;
        sprintf(&auxNameRanges[ii][0],"LAMBDA%d",i);
        NameRanges[ii]=&auxNameRanges[ii][0];
	ii++;
      }

//-----------------  write the BOUNDS  section
//      fprintf(FMCS,"BOUNDS\n");//---stampaggi


      for(i=0 ; i< NpointsInFit ; i++) {
	if( mvdhit[i]){
//          fprintf(FMCS," BV  Bounds  lam%d\n",  i);//---stampaggi
          sprintf(&auxTypeofBound[i][0],"BV");   TypeofBound[i]= &auxTypeofBound[i][0];
          sprintf(&auxBoundStructVarName[i][0],"lam%d",i);
	} else {
//          fprintf(FMCS," BV  Bounds  lamp%d\n",  i);//---stampaggi
          sprintf(&auxTypeofBound[i][0],"BV");   TypeofBound[i]= &auxTypeofBound[i][0];
          sprintf(&auxBoundStructVarName[i][0],"lamp%d",i);
	}

        BoundStructVarName[i]=&auxBoundStructVarName[i][0];
        BoundValue[i]=0.;

      }

      for(i=0, ii=0 ; i< NpointsInFit ; i++) {
	if( mvdhit[i]) continue;
//          fprintf(FMCS," BV  Bounds  lamm%d\n", i);//---stampaggi
          sprintf(&auxTypeofBound[ii+NpointsInFit][0],"BV");
          TypeofBound[ii+NpointsInFit]= &auxTypeofBound[ii+NpointsInFit][0];
          sprintf(&auxBoundStructVarName[ii+NpointsInFit][0],"lamm%d",i);
          BoundStructVarName[ii+NpointsInFit]=&auxBoundStructVarName[ii+NpointsInFit][0];
          BoundValue[ii+NpointsInFit]=0.;
	  ii++;
      }

//          fprintf(FMCS," FX  Bounds  DUMMY  %g\n",2.*M);//--stampaggi
          sprintf(&auxTypeofBound[NpointsInFit+nSttHits][0],"FX");
          TypeofBound[NpointsInFit+nSttHits]= &auxTypeofBound[NpointsInFit+nSttHits][0];

          sprintf(&auxTypeofBound[NpointsInFit+nSttHits][0],"FX");
          TypeofBound[NpointsInFit+nSttHits]= &auxTypeofBound[NpointsInFit+nSttHits][0];

          sprintf(&auxBoundStructVarName[NpointsInFit+nSttHits][0],"DUMMY");
          BoundStructVarName[NpointsInFit+nSttHits]=&auxBoundStructVarName[NpointsInFit+nSttHits][0];
          BoundValue[NpointsInFit+nSttHits]=2.*M;

//   fixing q1
//          fprintf(FMCS," FX  Bounds  q1  %g\n",0.);//--stampaggi
          sprintf(&auxTypeofBound[NpointsInFit+nSttHits+1][0],"FX");
          TypeofBound[NpointsInFit+nSttHits+1]=&auxTypeofBound[NpointsInFit+nSttHits+1][0];
          sprintf(&auxBoundStructVarName[NpointsInFit+nSttHits+1][0],"q1");
          BoundStructVarName[NpointsInFit+nSttHits+1]=&auxBoundStructVarName[NpointsInFit+nSttHits+1][0];
          BoundValue[NpointsInFit+nSttHits+1]= 0.;
//   fixing q2
//          fprintf(FMCS," FX  Bounds  q2  %g\n",0.);//--stampaggi
          sprintf(&auxTypeofBound[NpointsInFit+nSttHits+2][0],"FX");
          TypeofBound[NpointsInFit+nSttHits+2]=&auxTypeofBound[NpointsInFit+nSttHits+2][0];
          sprintf(&auxBoundStructVarName[NpointsInFit+nSttHits+2][0],"q2");
          BoundStructVarName[NpointsInFit+nSttHits+2]=&auxBoundStructVarName[NpointsInFit+nSttHits+2][0];
          BoundValue[NpointsInFit+nSttHits+2]= 0.;

//-----

//------------------------------------------------------------stampaggi
/*
      fprintf(FMCS,"ENDATA\n");
      fclose(FMCS);
*/

/*
cout<<"n.  punti nel fit "<<NpointsInFit<<endl;


cout<<"nRows "<<nRows<<endl;
for(int ic =0;ic<nRows; ic++){
   cout<<"n.  Row  "<<ic<<", nameRows "<<nameRows[ic]<<",  typeRows "<<typeRows[ic]<<endl;
}

cout<<"NStructRowsMax = "<<NStructRowsMax<<endl;
cout<<"NStructVar "<<NStructVar<<" e loro elenco "<<endl;
for(int ic =0;ic<NStructVar; ic++){
	cout<<"\tvar. n. "<<ic<<", nome = "<<StructVarName[ic]<<endl;
}



for(int ic =0;ic<NStructVar; ic++){
   cout<<"NRowsInWhichStructVarArePresent  "<<NRowsInWhichStructVarArePresent[ic]
     <<", nome var. strut. n."<<ic<<"  = "
     <<StructVarName[ic]<<endl;

  for(int jc=0; jc<NRowsInWhichStructVarArePresent[ic];jc++){
   cout<<"n. "<<jc<<"  NameRowsInWhichStructVarArePresent  "<<NameRowsInWhichStructVarArePresent[ic*NStructRowsMax+jc]<<endl;
  }
}


cout<<"n Coefficient "<<21*nMvdHits+24*nSttHits<<endl;
iii=0;
for(int ic =0;ic<NStructVar; ic++){
   cout<<"Struct. Var."<< StructVarName[ic] <<" e' presente in "<< NRowsInWhichStructVarArePresent[ic]
	<<"  Rows;"<<endl;
  for(ii=0;ii<NRowsInWhichStructVarArePresent[ic];ii++){

   cout<<"\tin Row "<<NameRowsInWhichStructVarArePresent[ic*NStructRowsMax+ii]
         <<", ha  Coefficient   "<<Coefficients[ic*NStructRowsMax+ii]<<
	" (n. sequenziale = "<<iii<<")"<<endl;
	iii++;
  }
}

cout<<"n valuesB "<<nRows-1<<endl;
for(int ic =0;ic<nRows-1; ic++){
   cout<<"n. "<<ic<<",  valuesB   "<<ValueB[ic]<<endl;
}
cout<<"n ranges "<<nRanges<<endl;
for(int ic =0;ic<nRanges; ic++){
   cout<<"n. "<<ic<<",  RANGES   "<<ValueRanges[ic]<<endl;
}
cout<<"n Bounds "<<nBounds<<endl;
for(int ic =0;ic<nBounds; ic++){
   cout<<"n. "<<ic<<",  Bounds   "<<BoundValue[ic]<<endl;
   cout<<"n. "<<ic<<",  Bound Type   "<<TypeofBound[ic]<<endl;
   cout<<"n. "<<ic<<",  Bound Name   "<<BoundStructVarName[ic]<<endl;
}
*/

//-------fine stampaggi

//-----------------------  funzioni chiamate direttamente
/*
cout<<"cavolo2, da sttmvdtracking : nRows = "<<nRows<<", NStructVar = "<<
	NStructVar<<", NStructRowsMax = "<<NStructRowsMax<<
	", NRowsInWhichStructVarArePresent = "<<
	NRowsInWhichStructVarArePresent<<", nRanges = "<<nRanges
	<<", nBounds = "<<nBounds<<endl;
*/
      int status= glp_main(
            nRows,nameRows,typeRows, //  ROWS info
            NStructVar, NStructRowsMax, NRowsInWhichStructVarArePresent,  //  COLUMNS info
      StructVarName, NameRowsInWhichStructVarArePresent,  //  COLUMNS info
      Coefficients,  //  COLUMNS info
      ValueB,  // RHS  info
      nRanges, ValueRanges, NameRanges, //  RANGES  info
      nBounds, BoundValue, BoundStructVarName, TypeofBound //  BOUNDS info
//      ,final_values, TIMEOUT
      ,final_values
       );

     if (status != 0) return -5;	// fit failed

//--------stampaggi
/*
printf("from main, final printout con routines chiamate direttamente -------------------------------\n");
printf("      number of structural variables %d\n",NStructVar);
int ica;
for(ica=0;ica<NStructVar;ica++){
    printf("name of structural variable %s and its final value %g\n",
           StructVarName[ica], final_values[ica]);
}
printf("from main, end of final printout  con routines chiamate direttamente -------------------------------\n");
*/
//--------fine stampaggi



//-----------------------  fine funzioni chiamate direttamente


     m1_result=final_values[0];
     m2_result=final_values[1];
//     q1_result=final_values[2];
//     q2_result=final_values[3];


     *emme = m1_result-m2_result ;
// taking into account the rotation + traslation that was performed and calculate emme and qu

      if(fabs(cose-*emme*sine)> 1.e-10) {
        *emme=((*emme)*cose+sine)/(cose-(*emme)*sine);
	return 1;
      } else {    //  in this case the equation is   0 = U in the Conformal plane --> x=0 in the XY plane.
           return -99;
      }



}










//----------end of function PndSttMvdTracking::FitSZspace








//------------------ begin function  PndSttMvdTracking::RefitMvdStt


 void	PndSttMvdTracking::RefitMvdStt(
			UShort_t nCandHit,
			UShort_t *ListCandHit,
			Short_t *ListCandHitType,
			Double_t info[][7],
			Double_t rotationangle, //  this is between 0. and 2*PI
			Double_t tv[2],
			Short_t iexcl,
			Double_t *pAlfa, // output of the fit
			Double_t *pBeta, // output of the fit
			Double_t *pGamma,// set at zero always for now
			bool *status    // fit status; true = successful
						)
{
	bool Type;

	UShort_t	i,
			iparallel;

	UShort_t MAXIMUMHITSINFIT = 20;
	if(MAXIMUMHITSINFIT>nmaxSttHitsInTrack+nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack)
	     MAXIMUMHITSINFIT =
		nmaxSttHitsInTrack+nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack;

	Short_t	exitstatus;

	Double_t dist2,
		 mindis,
		 emme,
		 factor,
		 gamma,
		 qu,
		 ErrorStraw = 0.03,
		 ErrorMvd = 0.01,
Xconformal[nmaxSttHitsInTrack+nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack],
Yconformal[nmaxSttHitsInTrack+nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack],
DriftRadiusconformal[nmaxSttHitsInTrack+nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack],
ErrorDriftRadiusconformal[nmaxSttHitsInTrack+nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack];

	*status= false;
	factor=3.;
	mindis=0.5;
//	trajectory_vertex[0]=trajectory_vertex[1]=0.;

	for(i=0, iparallel=0; i<nCandHit && iparallel< MAXIMUMHITSINFIT; i++){

		if(i==iexcl) continue;
		if( ListCandHitType[i] == 0 ){	// mvd pixels
			//----- translate the little circumference in XY representing
			//  approximately the sensitive area into the conformal space
			//  circumference

			dist2 = (XMvdPixel[ListCandHit[i]]-tv[0])*
				(XMvdPixel[ListCandHit[i]]-tv[0])+
				(YMvdPixel[ListCandHit[i]]-tv[1])*
				(YMvdPixel[ListCandHit[i]]-tv[1]);
			if(dist2<mindis) continue;	// this is to exclude Mvd hits too close to the traslated
						// center of reference frame (given by tv[0], tv[1]). This
						// would cause the subsequent fit to fail.

			gamma = dist2 - ErrorMvd*ErrorMvd; // for Pixels
				// I assume the 'drift radius' to be  the max dimension of
				//  the Pixel
			Xconformal[iparallel] = (XMvdPixel[ListCandHit[i]]-tv[0])/gamma;
			Yconformal[iparallel] = (YMvdPixel[ListCandHit[i]]-tv[1])/gamma;
			DriftRadiusconformal[iparallel]=-1.;// only to signal later this is a Mvd hit.
			ErrorDriftRadiusconformal[iparallel]=factor*ErrorMvd/gamma;
			iparallel++;
		} else if ( ListCandHitType[i] == 1 ){	// mvd strips
			//----- translate the little circumference in XY representing
			//  approximately the sensitive area into the conformal space
			//  circumference
			dist2 = (XMvdStrip[ListCandHit[i]]-tv[0])*
				(XMvdStrip[ListCandHit[i]]-tv[0])+
				(YMvdStrip[ListCandHit[i]]-tv[1])*
				(YMvdStrip[ListCandHit[i]]-tv[1]);
			if(dist2<mindis) continue;	// this is to exclude Mvd hits too close to the traslated
						// center of reference frame (given by tv[0], tv[1]). This
						// would cause the subsequent fit to fail.

			gamma = dist2 - ErrorMvd*ErrorMvd;// for Strips also
				// I assume the 'drift radius' to be  the largest error of
				//  the Strips (which is in the X dimension)
			Xconformal[iparallel] = (XMvdStrip[ListCandHit[i]]-tv[0])/gamma;
			Yconformal[iparallel] = (YMvdStrip[ListCandHit[i]]-tv[1])/gamma;
			DriftRadiusconformal[iparallel]=-1.;// only to signal later this is a Mvd hit.
			ErrorDriftRadiusconformal[iparallel]=factor* ErrorMvd/gamma;
			iparallel++;
		} else if ( ListCandHitType[i] == 2 ){	// Stt parallel hit.


			dist2 = (info[ListCandHit[i]][0]-tv[0])*
				(info[ListCandHit[i]][0]-tv[0])+
				(info[ListCandHit[i]][1]-tv[1])*
				(info[ListCandHit[i]][1]-tv[1]);
			if(dist2<mindis) continue;	// this is to exclude hits too close to the traslated
						// center of reference frame (given by tv[0], tv[1]). This
						// would cause the subsequent fit to fail.
			gamma = dist2 -
				info[ListCandHit[i]][3]*
				info[ListCandHit[i]][3];
			Xconformal[iparallel] = (info[ListCandHit[i]][0]-tv[0])/gamma;
			Yconformal[iparallel] = (info[ListCandHit[i]][1]-tv[1])/gamma;
			DriftRadiusconformal[iparallel]=info[ListCandHit[i]][3]/gamma;
			ErrorDriftRadiusconformal[iparallel]=factor*ErrorStraw/gamma;
			iparallel++;
		}
	}	// end of for(i=0, iparallel=0;

	if( nCandHit < 2)  return;
if(istampa>2){
	cout<<"PndSttMvdTracking::RefitMvdStt, n. Hits (Mvd+Stt || ) = "<<iparallel<<endl;
	cout<<"\tthe trajectory pivoting hit is n. "<<iexcl<<" in the track hit list;\n";
	cout<<"\tthe trajectory pivoting vertex is in X = "<<tv[0]<<", Y = "<<tv[1]<<endl;
	for(int ig=0;ig<iparallel;ig++){
		if( DriftRadiusconformal[ig] <0.) {
				cout<<"\tMvd Hit;   ";
			} else {
				cout<<"\tStt || Hit;   ";
			}
		cout<<"ErrorDriftRadiusconformal["<<ig<<"] = "<<
		ErrorDriftRadiusconformal[ig]<<endl;
	}
}
	exitstatus = FitHelixCylinder(  iparallel,
					Xconformal,
					Yconformal,
					DriftRadiusconformal,
					ErrorDriftRadiusconformal,
					rotationangle,  //  rotationangle, da mettere
					tv,	//  vertex in (X,Y) of this trajectory
					MAXIMUMHITSINFIT,  //  maximum n. of hits allowed in fast fit
					&emme,
					&qu,
					pAlfa,
					pBeta,
					pGamma,
					&Type
					);

	//  existatus > 0, Type= true --> fit ok, it is a Circle in XY; 
	//  existatus > 0, Type= false --> fit ok, it is a Straigh Line in XY; 
	//  existatus = -1, fit failed, equation of XY circle : X**2 + Y**2 =0, impossible in principle; 
	//  existatus < 0, fit failed. 
	if( exitstatus > 0 && Type)	*status=true;
	return;
}


//------------------ end function  PndSttMvdTracking::RefitMvdStt



//------------------ begin function  PndSttMvdTracking::MvdMatchtoMC

  void PndSttMvdTracking::MvdMatchtoMC(
		UShort_t nMvdMCPoint,
		Int_t *FromPixeltoMCTrack,	// output
		Int_t *FromStriptoMCTrack	// output
		   )
{

	bool	inclusionMCPoint[nMvdMCPoint];


	UShort_t	i,
			j,
			jmcpoint;
	Double_t	dist,
			distance;

	Int_t	MCPointtoMCTrackID;

	Double_t	XMvdMCPoint,
			YMvdMCPoint,
			ZMvdMCPoint;

	PndSdsMCPoint * pMvdMCPoint;


//----  initializations

	for(i=0; i<nMvdMCPoint;i++){
		inclusionMCPoint[i]=true;
	}
	for(i=0; i<nMvdPixelHit;i++){
		FromPixeltoMCTrack[i]=-1;
	}
	for(i=0; i<nMvdStripHit;i++){
		FromStriptoMCTrack[i]=-1;
	}

//----------

	for(i=0; i<nMvdPixelHit;i++){
		if (refindexMvdPixel[i]<0.) continue;
		dist=errorsqPixel;
		for(j=0;j<nMvdMCPoint;j++){
			// get the MC info.
			pMvdMCPoint = (PndSdsMCPoint*) fMvdMCPointArray->At(j);
			TVector3 position;
			pMvdMCPoint->Position(position);
			XMvdMCPoint=position.X();
			YMvdMCPoint=position.Y();
			ZMvdMCPoint=position.Z();
			MCPointtoMCTrackID= pMvdMCPoint->GetTrackID();

			if( !inclusionMCPoint[j]) continue;
			distance = (XMvdMCPoint-XMvdPixel[i])*(XMvdMCPoint-XMvdPixel[i])+
				(YMvdMCPoint-YMvdPixel[i])*(YMvdMCPoint-YMvdPixel[i])+
				(ZMvdMCPoint-ZMvdPixel[i])*(ZMvdMCPoint-ZMvdPixel[i]);
if(istampa>2)	{
	cout<<"distanza**2 di Pixel hit n. "<<i
   <<" da MC Mvd Point n. "<<j<<" = "<<distance<<endl;
		}
			if( distance<dist){
				FromPixeltoMCTrack[i]=MCPointtoMCTrackID;
				jmcpoint=j;
				dist=distance;
			}
		}	// end of for(j=0;j<nMvdMCPoint;j++)

		if( FromPixeltoMCTrack[i]>= 0){
			inclusionMCPoint[jmcpoint]=false;
		}
if(istampa>=2){
	if( FromPixeltoMCTrack[i]<0 ){
cout<<"Evento n. "<<IVOLTE<<
"; il pixel hit n. "<<i<<" non e' associato ad alcun Mvd Point (FromPixeltoMCTrack=-1).\n";
	} else {
cout<<"Evento n. "<<IVOLTE<<
  "; associato pixel hit n. "<<i<<" a  MC Mvd Point n. "<<jmcpoint
		<<" e di conseguenza alla traccia MC n. "<<FromPixeltoMCTrack[i]<<endl;
	}
}

	}	// end of for(i=0; i<nMvdPixelHit;i++)






	for(i=0; i<nMvdStripHit;i++){
		if (refindexMvdStrip[i]<0.) continue;
		dist=errorsqStrip;
		for(j=0;j<nMvdMCPoint;j++){
			// get the MC info.
			pMvdMCPoint = (PndSdsMCPoint*) fMvdMCPointArray->At(j);
			TVector3 position;
			pMvdMCPoint->Position(position);
			XMvdMCPoint=position.X();
			YMvdMCPoint=position.Y();
			ZMvdMCPoint=position.Z();
			MCPointtoMCTrackID= pMvdMCPoint->GetTrackID();


			if( !inclusionMCPoint[j]) continue;
			distance = (XMvdMCPoint-XMvdStrip[i])*(XMvdMCPoint-XMvdStrip[i])+
				(YMvdMCPoint-YMvdStrip[i])*(YMvdMCPoint-YMvdStrip[i])+
				(ZMvdMCPoint-ZMvdStrip[i])*(ZMvdMCPoint-ZMvdStrip[i]);
if(istampa>2) cout<<"distanza**2 di Strip hit n. "<<i
   <<" da MC Mvd Point n. "<<j<<" = "<<distance<<endl;
			if( distance<dist){
				FromStriptoMCTrack[i]=MCPointtoMCTrackID;
				jmcpoint=j;
				dist=distance;
			}
		}	// end of for(j=0;j<nMvdMCPoint;j++)
		if( FromStriptoMCTrack[i]>= 0){
			inclusionMCPoint[jmcpoint]=false;
		}
if(istampa>=2){
	if( FromStriptoMCTrack[i]<0 ){
cout<<"Evento n. "<<IVOLTE<<
",  lo strip hit n. "<<i<<" non e' associato ad alcun Mvd Point (FromStriptoMCTrack=-1).\n";
	} else {
cout<<"Evento n. "<<IVOLTE<<
		";  associato strip hit n. "<<i<<" a  MC Mvd Point n. "<<jmcpoint
		<<" e di conseguenza alla traccia MC n. "<<FromStriptoMCTrack[i]<<endl;
	}
}

	}	// end of for(i=0; i<nMvdStripHit;i++)





	return;
}


//------------------ end function  PndSttMvdTracking::MvdMatchtoMC










//------------------ begin function  PndSttMvdTracking::MvdMatchedSpurioustoTrackCand
  void PndSttMvdTracking::MvdMatchedSpurioustoTrackCand(
			UShort_t nSttTrackCand,				// input
			bool *keepit,				// input
			Short_t *daTrackFoundaTrackMC,			// input
			Int_t *FromPixeltoMCTrack,			// input
			Int_t *FromStriptoMCTrack,			// input
			UShort_t *nPixelHitsinTrack,	// input
UShort_t ListPixelHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHitsInTrack],// input
			UShort_t *nStripHitsinTrack,	// input
UShort_t ListStripHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdStripHitsInTrack],// input

			UShort_t *nMvdPixelCommon,		// output
			UShort_t *MvdPixelCommonList,		// output
			UShort_t *nMvdPixelSpuriinTrack,	// output
			UShort_t *MvdPixelSpuriList,		// output
			UShort_t *nMCMvdPixelAlone,		// output
			UShort_t *MCMvdPixelAloneList,		// output

			UShort_t *nMvdStripCommon,		// output
			UShort_t *MvdStripCommonList,		// output
			UShort_t *nMvdStripSpuriinTrack,	// output
			UShort_t *MvdStripSpuriList,		// output
			UShort_t *nMCMvdStripAlone,		// output
			UShort_t *MCMvdStripAloneList		// output
				)
{


//	bool includePixel[MAXTRACKSPEREVENT][nMvdPixelHit],
//	     includeStrip[MAXTRACKSPEREVENT][nMvdStripHit];
	bool includePixel[nSttTrackCand][nMvdPixelHit],
	     includeStrip[nSttTrackCand][nMvdStripHit];

	UShort_t i,j;

	int index;

	for(i=0; i<nSttTrackCand;i++){
		if(!keepit[i]) continue;
		nMvdPixelCommon[i]=0;
		nMvdPixelSpuriinTrack[i]=0;
		nMCMvdPixelAlone[i]=0;
		nMvdStripCommon[i]=0;
		nMvdStripSpuriinTrack[i]=0;
		nMCMvdStripAlone[i]=0;
	}





	for(i=0; i<nSttTrackCand;i++){
		if(!keepit[i]) continue;
		for(j=0;j<nMvdPixelHit;j++){
			includePixel[i][j]=true;
		}
		for(j=0;j<nMvdStripHit;j++){
			includeStrip[i][j]=true;
		}

		for(j=0;j<nPixelHitsinTrack[i];j++){
			includePixel[i][ListPixelHitsinTrack[i][j]]=false;
		  if( daTrackFoundaTrackMC[i]> -1){
			if( daTrackFoundaTrackMC[i] ==
				FromPixeltoMCTrack[
				     ListPixelHitsinTrack[i][j]
						]
			  ){
				index = i*nMvdPixelHit+nMvdPixelCommon[i];
				*(MvdPixelCommonList+index) =
				     ListPixelHitsinTrack[i][j];
				nMvdPixelCommon[i]++;
			} else {
				index = i*nMvdPixelHit+nMvdPixelSpuriinTrack[i];
				*(MvdPixelSpuriList+index) =
				     ListPixelHitsinTrack[i][j];
				nMvdPixelSpuriinTrack[i]++;
			}
		  }
		}

		for(j=0;j<nStripHitsinTrack[i];j++){
			includeStrip[i][ListStripHitsinTrack[i][j]]=false;
		  if( daTrackFoundaTrackMC[i]> -1){
			if( daTrackFoundaTrackMC[i] ==
				FromStriptoMCTrack[
				     ListStripHitsinTrack[i][j]
						]
			  ){
			  	index = i*nMvdStripHit+nMvdStripCommon[i];
				*(MvdStripCommonList+index) =
				     ListStripHitsinTrack[i][j];
				nMvdStripCommon[i]++;
			} else {
				index = i*nMvdStripHit+nMvdStripSpuriinTrack[i];
				*(MvdStripSpuriList+index) =
				     ListStripHitsinTrack[i][j];
				nMvdStripSpuriinTrack[i]++;
			}
		  }
		}
	}	// end of for(i=0; i<nSttTrackCand;i++)





	for(j=0; j<nSttTrackCand;j++){
	    if(!keepit[j]) continue;
	    if( daTrackFoundaTrackMC[j]> -1){
		for(i=0;i<nMvdPixelHit;i++){
			if(!includePixel[j][i]) continue;
			if( daTrackFoundaTrackMC[j]==FromPixeltoMCTrack[i] ){
				index = j*nMvdPixelHit+nMCMvdPixelAlone[j];
				*(MCMvdPixelAloneList+index)=
					i;
				nMCMvdPixelAlone[j]++;
			}
		}
	    }
	}





	for(j=0; j<nSttTrackCand;j++){
	    if(!keepit[j]) continue;
	    if( daTrackFoundaTrackMC[j]> -1){
		for(i=0;i<nMvdStripHit;i++){
			if(!includeStrip[j][i]) continue;
			if( daTrackFoundaTrackMC[j]==FromStriptoMCTrack[i] ){
				index = j*nMvdStripHit+nMCMvdStripAlone[j];
				*(MCMvdStripAloneList+index)=i;
				nMCMvdStripAlone[j]++;
			}
		}
	    }
	}




	return;
}

//------------------ end function  PndSttMvdTracking::MvdMatchedSpurioustoTrackCand



//------------------------- begin of function  PndSttMvdTracking::MatchMvdHitsToSttTracks2

	void	PndSttMvdTracking::MatchMvdHitsToSttTracks2(
			bool *keepit,
			Double_t delta,
			Double_t highqualitycut,
			UShort_t nSttTrackCand,
			Double_t Ox[MAXTRACKSPEREVENT],
			Double_t Oy[MAXTRACKSPEREVENT],
			Double_t R[MAXTRACKSPEREVENT],
			Double_t FI0[MAXTRACKSPEREVENT],
			Double_t Fifirst[MAXTRACKSPEREVENT],
			Short_t CHARGE[MAXTRACKSPEREVENT],

			UShort_t nPixelHitsinTrack[MAXTRACKSPEREVENT], // output
UShort_t ListPixelHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHitsInTrack], // output
			UShort_t nStripHitsinTrack[MAXTRACKSPEREVENT], // output
UShort_t ListStripHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdStripHitsInTrack] // output
                    )
{
	bool specialcase;

	UShort_t i,j,j1, j2, imvdcand, jmvdhit, ncont,
		chosenmix,
		chosenmix2,
		ngoodmix,
		oldN,
		nn[MAXMVDTRACKSPEREVENT+2],
		nHighQuality[MAXMVDTRACKSPEREVENT+2],
		List[MAXMVDTRACKSPEREVENT+2][nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack],
		ListType[MAXMVDTRACKSPEREVENT+2][nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack];

	Double_t angle,
		anglemax,
		anglemin,
		dist,
		oldtotal,
		oldtotal2,
		total,
		Dist,
		DIST[MAXMVDTRACKSPEREVENT+1];



//int temporaneo=4;

 for(i=0; i<nSttTrackCand; i++){
	if( ! keepit[i] ) continue ;

	if( Fifirst[i] < -99998. ){  // case with Fifirst[i]=-99999.; in this
					// case there the circle is contained
					// in the Mvd region.
		anglemax = 2.*PI;
		anglemin = 0.;
		goto pippo ;
	}


	if(CHARGE[i]>0){	// track must rotate clockwise looking into the beam.
		anglemax = FI0[i];
		anglemin = Fifirst[i];
	} else {
		anglemin = FI0[i];
		anglemax = Fifirst[i];
	}
	if(anglemax < anglemin) anglemax += 2.*PI;
	if(anglemax < anglemin) anglemax==anglemin; // this is just to be super-sure.


pippo: ;


//--------------------

	ngoodmix=0;
	nn[0]=0;

 for( imvdcand=0; imvdcand<nMvdTrackCand; imvdcand++){
	Dist = 0.;
	ncont=0;
	nn[ngoodmix]=0;
	nHighQuality[ngoodmix]=0;
	for( jmvdhit=0; jmvdhit<nHitMvdTrackCand[imvdcand]; jmvdhit++){

		if(ListHitTypeMvdTrackCand[imvdcand][jmvdhit]==
		    FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)){
			ncont++;
			angle = atan2(
			YMvdPixel[ListHitMvdTrackCand[imvdcand][jmvdhit]]-Oy[i],
			XMvdPixel[ListHitMvdTrackCand[imvdcand][jmvdhit]]-Ox[i]
							);
			if(angle<0.) angle += 2.*PI;

			if( angle>anglemax){
				angle -= 2.*PI;
				if( angle>anglemax) angle = anglemax;
			} else if (angle<anglemin){
				angle += 2.*PI;
				if (angle<anglemin) angle = anglemin;
			}
			if(angle > anglemin && angle < anglemax)
			{
				dist=fabs( sqrt(
				 (Ox[i]-XMvdPixel[ListHitMvdTrackCand[imvdcand][jmvdhit]])*
				 (Ox[i]-XMvdPixel[ListHitMvdTrackCand[imvdcand][jmvdhit]])
				+(Oy[i]-YMvdPixel[ListHitMvdTrackCand[imvdcand][jmvdhit]])*
				 (Oy[i]-YMvdPixel[ListHitMvdTrackCand[imvdcand][jmvdhit]]))-R[i]);
				if(dist<delta)
				{
				     List[ngoodmix][nn[ngoodmix]]=
					ListHitMvdTrackCand[imvdcand][jmvdhit];
				     ListType[ngoodmix][nn[ngoodmix]]=
					FairRootManager::Instance()->GetBranchId(fMvdPixelBranch);
				     Dist += dist;
				     if( dist<highqualitycut) nHighQuality[ngoodmix]++;
				     nn[ngoodmix]++;
				}
			}	// end of  if(angle > anglemin)

		} else {// at this point this is a Strip hit; already made sure
			// earlier in the code that there is no third possibility.

			ncont++;
			angle = atan2(
			YMvdStrip[ListHitMvdTrackCand[imvdcand][jmvdhit]]-Oy[i],
			XMvdStrip[ListHitMvdTrackCand[imvdcand][jmvdhit]]-Ox[i]
							);
			if(angle<0.) angle += 2.*PI;

			if( angle>anglemax){
				angle -= 2.*PI;
				if( angle>anglemax) angle = anglemax;
			} else if (angle<anglemin){
				angle += 2.*PI;
				if (angle<anglemin) angle = anglemin;
			}
			if(angle > anglemin && angle < anglemax){
				dist=fabs( sqrt(
			 (Ox[i]-XMvdStrip[ListHitMvdTrackCand[imvdcand][jmvdhit]])*
			 (Ox[i]-XMvdStrip[ListHitMvdTrackCand[imvdcand][jmvdhit]])
			 +(Oy[i]-YMvdStrip[ListHitMvdTrackCand[imvdcand][jmvdhit]])*
			 (Oy[i]-YMvdStrip[ListHitMvdTrackCand[imvdcand][jmvdhit]])) -R[i]);
				if(dist<delta)
				{
				   List[ngoodmix][nn[ngoodmix]]=
					ListHitMvdTrackCand[imvdcand][jmvdhit];
				   ListType[ngoodmix][nn[ngoodmix]]=
				    FairRootManager::Instance()->GetBranchId(fMvdStripBranch);
				   Dist += dist;
				   if( dist<highqualitycut) nHighQuality[ngoodmix]++;

				   nn[ngoodmix]++;
				}
			}	// end of   if(angle > anglemin)
		} // end of    if(ListHitTypeMvdTrackCand[imvdcand][jmvdhit]

	}	// end of   for( jmvdhit=0; jmvdhit<nHitMvdTrackCand[imvdcand];


	if( nn[ngoodmix]>0) {
		DIST[ngoodmix]=Dist/nn[ngoodmix];
		ngoodmix++;


//--------- stampaggi
if(istampa>=3 ){cout<<"\tquesto Mvd candidato (n. ngoodmix = "<<ngoodmix-1<<
	") passa con i seguenti hits :"<<endl;

	for(int icc=0; icc<nn[ngoodmix-1]; icc++){
		if(ListType[ngoodmix-1][icc]==FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) {
			cout<<"\tPixel hit n. "<<List[ngoodmix-1][icc]<<endl;
		} else if(ListType[ngoodmix-1][icc]==
			FairRootManager::Instance()->GetBranchId(fMvdStripBranch)){
			cout<<"\tStrip hit n. "<<List[ngoodmix-1][icc]<<endl;
		} else{
			cout<<"\tNoise  (?) , hit tipo "<<ListType[ngoodmix-1][icc]<<endl;
		}
	}
	cout<<endl;
}
//------------fine stampaggi



	}
 }	// end of for( imvdcand=0;imvdcand<nMvdTrackCand;imvdcand++)


//-------  now use the Mvd which are in no Mvd Track Candidate


//------- first, the DS (downstream) Mvd hits
		nn[ngoodmix]=0;
		DIST[ngoodmix] = 0.;
		nHighQuality[ngoodmix]=0;
		for( jmvdhit=0; jmvdhit<nMvdDSPixelHitNotTrackCand; jmvdhit++){

			angle = atan2(
				YMvdPixel[ListMvdDSPixelHitNotTrackCand[jmvdhit]]-Oy[i],
				XMvdPixel[ListMvdDSPixelHitNotTrackCand[jmvdhit]]-Ox[i]
					);
			if(angle<0.) angle += 2.*PI;
			if( angle>anglemax){
				angle -= 2.*PI;
				if( angle>anglemax) angle = anglemax;
			} else if (angle<anglemin){
				angle += 2.*PI;
				if (angle<anglemin) angle = anglemin;
			}
			if(angle > anglemin && angle < anglemax){
				dist=fabs( sqrt(
				 (Ox[i]-XMvdPixel[ListMvdDSPixelHitNotTrackCand[jmvdhit]])*
				 (Ox[i]-XMvdPixel[ListMvdDSPixelHitNotTrackCand[jmvdhit]])
				+(Oy[i]-YMvdPixel[ListMvdDSPixelHitNotTrackCand[jmvdhit]])*
				 (Oy[i]-YMvdPixel[ListMvdDSPixelHitNotTrackCand[jmvdhit]])
						) -R[i]);
				if(dist<delta)
				{
					List[ngoodmix][nn[ngoodmix]]=
					ListMvdDSPixelHitNotTrackCand[jmvdhit];
					ListType[ngoodmix][nn[ngoodmix]]=
					 FairRootManager::Instance()->GetBranchId(fMvdPixelBranch);
					DIST[ngoodmix] += dist;
					if( dist<highqualitycut) nHighQuality[ngoodmix]++;
						nn[ngoodmix]++;
				}
			}  //  end of     if(angle > anglemin )
		}	//  end  of for( jmvdhit=0; jmvdhit<nMvdDSPixelHitNotTrackCand; jmvdhit++)

		for( jmvdhit=0; jmvdhit<nMvdDSStripHitNotTrackCand; jmvdhit++){

			angle = atan2(
				YMvdStrip[ListMvdDSStripHitNotTrackCand[jmvdhit]]-Oy[i],
				XMvdStrip[ListMvdDSStripHitNotTrackCand[jmvdhit]]-Ox[i]
				      );
			if(angle<0.) angle += 2.*PI;
			if( angle>anglemax){
				angle -= 2.*PI;
				if( angle>anglemax) angle = anglemax;
			} else if (angle<anglemin){
				angle += 2.*PI;
				if (angle<anglemin) angle = anglemin;
			}
			if(angle > anglemin && angle < anglemax){

				dist=fabs( sqrt(
				 (Ox[i]-XMvdStrip[ListMvdDSStripHitNotTrackCand[jmvdhit]])*
				 (Ox[i]-XMvdStrip[ListMvdDSStripHitNotTrackCand[jmvdhit]])
				+(Oy[i]-YMvdStrip[ListMvdDSStripHitNotTrackCand[jmvdhit]])*
				 (Oy[i]-YMvdStrip[ListMvdDSStripHitNotTrackCand[jmvdhit]])
					) -R[i]);


				if(dist<delta)
				{
					List[ngoodmix][nn[ngoodmix]]=
					ListMvdDSStripHitNotTrackCand[jmvdhit];
					ListType[ngoodmix][nn[ngoodmix]]=
					 FairRootManager::Instance()->GetBranchId(fMvdStripBranch);
					DIST[ngoodmix] += dist;
					if( dist<highqualitycut) nHighQuality[ngoodmix]++;
					nn[ngoodmix]++;
				}
			}  //  end of     if(angle > anglemin )

		}	//  end  of for( jmvdhit=0; jmvdhit<nMvdDSStripHitNotTrackCand; jmvdhit++)

			if( nn[ngoodmix]>0) {
				DIST[ngoodmix] /= nn[ngoodmix];
				ngoodmix++;
//--------- stampaggi
if(istampa>=3){cout<<"\tevento n. "<<IVOLTE<<" questi Mvd ALONE DS hits passano  :\n"<<endl;

	for(int icc=0; icc<nn[ngoodmix-1]; icc++){
		if(ListType[ngoodmix-1][icc]==FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) {
			cout<<"\tDS Pixel hit n. "<<List[ngoodmix-1][icc]<<endl;
		} else if(ListType[ngoodmix-1][icc]==
			FairRootManager::Instance()->GetBranchId(fMvdStripBranch)){
			cout<<"\tDS Strip hit n. "<<List[ngoodmix-1][icc]<<endl;
		} else{
			cout<<"\tNoise (?) , hit tipo "<<ListType[ngoodmix-1][icc]<<endl;
		}
	}

}
//------------fine stampaggi
			}	// end of if( nn[ngoodmix]>0)






//--------  now the US (upstream) Mvd hits

		nn[ngoodmix]=0;
		DIST[ngoodmix] = 0.;
		nHighQuality[ngoodmix]=0;
		for( jmvdhit=0; jmvdhit<nMvdUSPixelHitNotTrackCand; jmvdhit++){

			angle = atan2(
			YMvdPixel[ListMvdUSPixelHitNotTrackCand[jmvdhit]]-Oy[i],
					XMvdPixel[ListMvdUSPixelHitNotTrackCand[jmvdhit]]-Ox[i]
							);
			if(angle<0.) angle += 2.*PI;
			if( angle>anglemax){
				angle -= 2.*PI;
				if( angle>anglemax) angle = anglemax;
			} else if (angle<anglemin){
				angle += 2.*PI;
				if (angle<anglemin) angle = anglemin;
			}
			if(angle > anglemin && angle < anglemax){
				dist=fabs( sqrt(
				 (Ox[i]-XMvdPixel[ListMvdUSPixelHitNotTrackCand[jmvdhit]])*
				 (Ox[i]-XMvdPixel[ListMvdUSPixelHitNotTrackCand[jmvdhit]])
				+(Oy[i]-YMvdPixel[ListMvdUSPixelHitNotTrackCand[jmvdhit]])*
				 (Oy[i]-YMvdPixel[ListMvdUSPixelHitNotTrackCand[jmvdhit]])
						) -R[i]);
				if(dist<delta)
				{
					List[ngoodmix][nn[ngoodmix]]=
						ListMvdUSPixelHitNotTrackCand[jmvdhit];
					ListType[ngoodmix][nn[ngoodmix]]=
					 FairRootManager::Instance()->GetBranchId(fMvdPixelBranch);
					DIST[ngoodmix] += dist;
					if( dist<highqualitycut) nHighQuality[ngoodmix]++;
						nn[ngoodmix]++;
				}
			}  //  end of     if(angle > anglemin )

		}	//  end  of for( jmvdhit=0; jmvdhit<nMvdUSPixelHitNotTrackCand; jmvdhit++)

		for( jmvdhit=0; jmvdhit<nMvdUSStripHitNotTrackCand; jmvdhit++){

			angle = atan2(
				YMvdStrip[ListMvdUSStripHitNotTrackCand[jmvdhit]]-Oy[i],
				XMvdStrip[ListMvdUSStripHitNotTrackCand[jmvdhit]]-Ox[i]
				      );
			if(angle<0.) angle += 2.*PI;
			if( angle>anglemax){
				angle -= 2.*PI;
				if( angle>anglemax) angle = anglemax;
			} else if (angle<anglemin){
				angle += 2.*PI;
				if (angle<anglemin) angle = anglemin;
			}
			if(angle > anglemin && angle < anglemax){

				dist=fabs( sqrt(
				 (Ox[i]-XMvdStrip[ListMvdUSStripHitNotTrackCand[jmvdhit]])*
				 (Ox[i]-XMvdStrip[ListMvdUSStripHitNotTrackCand[jmvdhit]])
				+(Oy[i]-YMvdStrip[ListMvdUSStripHitNotTrackCand[jmvdhit]])*
				 (Oy[i]-YMvdStrip[ListMvdUSStripHitNotTrackCand[jmvdhit]])
					) -R[i]);


				if(dist<delta)
				{
					List[ngoodmix][nn[ngoodmix]]=
					ListMvdUSStripHitNotTrackCand[jmvdhit];
					ListType[ngoodmix][nn[ngoodmix]]=
					 FairRootManager::Instance()->GetBranchId(fMvdStripBranch);
					DIST[ngoodmix] += dist;
					if( dist<highqualitycut) nHighQuality[ngoodmix]++;
					nn[ngoodmix]++;
				}
			}  //  end of     if(angle > anglemin )

		}	//  end  of for( jmvdhit=0; jmvdhit<nMvdUSStripHitNotTrackCand; jmvdhit++)

			if( nn[ngoodmix]>0) {
				DIST[ngoodmix] /= nn[ngoodmix];
				ngoodmix++;
//--------- stampaggi
if(istampa>=3){cout<<"\tevento n. "<<IVOLTE<<" questi Mvd ALONE US hits passano  :\n"<<endl;
	if( ngoodmix>0) {
	for(int icc=0; icc<nn[ngoodmix-1]; icc++){
		if(ListType[ngoodmix-1][icc]==FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) {
			cout<<"\tUS Pixel hit n. "<<List[ngoodmix-1][icc]<<endl;
		} else if(ListType[ngoodmix-1][icc]==
			FairRootManager::Instance()->GetBranchId(fMvdStripBranch)){
			cout<<"\tUS Strip hit n. "<<List[ngoodmix-1][icc]<<endl;
		} else{
			cout<<"\tNoise (?) , hit tipo "<<ListType[ngoodmix-1][icc]<<endl;
		}
	}
	}

}
//------------fine stampaggi
			}	// end of if( nn[ngoodmix]>0)




//-------  end of using the Mvd which are in no Mvd Track Candidate


if(istampa>=3 ){cout<<"da PndSttMvdTracking : appena prima arbitration, IVOLTE = "<<
IVOLTE<<", Stt track cand = "<<i<<", ngoodmix = "<<ngoodmix<<endl;}

		if( ngoodmix==1){
			chosenmix=0;
			chosenmix2=0;
		} else if( ngoodmix>1) {
//--- here the arbitration if there are more than 1 Stt+Mvd hit combination for a given SttTrackCand
			oldtotal = DIST[0];
			oldtotal2 = DIST[0];
			oldN = nHighQuality[0];
//			oldtotal /= nTotali[0];
if(istampa>=3 ){cout<<"da PndSttMvdTracking : goodmix n. 0, total distance (che e' = total distance2) = "<<oldtotal
				<<", e nHighQuality = "<<nHighQuality[0]<<endl;}
			chosenmix=0;
			chosenmix2=0;
			for(j1=1; j1<ngoodmix;j1++){
				total = DIST[j1];
if(istampa>=3){cout<<"da PndSttMvdTracking :\t goodmix n. "<<j1<<", total distance "<<total
					<<", e nHighQuality = "<<nHighQuality[j1]<<endl;}
				if(oldN<nHighQuality[j1]){
					oldN=nHighQuality[j1];
					chosenmix2=j1;
				} else if (oldN==nHighQuality[j1]){
					if(total<oldtotal2){
						chosenmix2=j1;
						oldtotal2=total;
					}
				}
				if(total<oldtotal){
					oldtotal=total;
					chosenmix=j1;
				}
			}
		}	// end of  if( ngoodmix==1)
//--- end of arbitration
if(istampa>=3 ){cout<<"da PndSttMvdTracking : fine arbitration, IVOLTE = "<<
IVOLTE<<", Stt track cand = "<<i<<endl;}





	nPixelHitsinTrack[i]=0;
	nStripHitsinTrack[i]=0;
	if( ngoodmix>0){
		chosenmix=chosenmix2;
		for(j=0;j<nn[chosenmix];j++){
			if( ListType[chosenmix][j]==
				FairRootManager::Instance()->GetBranchId(fMvdPixelBranch) ){
				ListPixelHitsinTrack[i]
				   [nPixelHitsinTrack[i]]=List[chosenmix][j];
				nPixelHitsinTrack[i]++;
			} else if(
			     ListType[chosenmix][j]==
			     FairRootManager::Instance()->GetBranchId(fMvdStripBranch)){
				ListStripHitsinTrack[i]
				   [nStripHitsinTrack[i]]=List[chosenmix][j];
				nStripHitsinTrack[i]++;
			}
		}
	}	// end of if( ngoodmix>0)


 }	// end of for(i=0; i<nSttTrackCand; i++)


 return;
}

//------------------------- end of function  PndSttMvdTracking::MatchMvdHitsToSttTracks2








//------------------------- begin of function  PndSttMvdTracking::MatchMvdHitsToSttTracksagain

	void	PndSttMvdTracking::MatchMvdHitsToSttTracksagain(
			bool *keepit,
			bool *Mvdhits,
			Double_t delta,
			Double_t highqualitycut,
			UShort_t nSttTrackCand,
			Double_t Ox[MAXTRACKSPEREVENT],
			Double_t Oy[MAXTRACKSPEREVENT],
			Double_t R[MAXTRACKSPEREVENT],
			Double_t FI0[MAXTRACKSPEREVENT],
			Double_t Fifirst[MAXTRACKSPEREVENT],
			Short_t CHARGE[MAXTRACKSPEREVENT],

			UShort_t nPixelHitsinTrack[MAXTRACKSPEREVENT], // output
UShort_t ListPixelHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHitsInTrack], // output
			UShort_t nStripHitsinTrack[MAXTRACKSPEREVENT], // output
UShort_t ListStripHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdStripHitsInTrack] // output
                    )
{
	bool specialcase,
	     downstream,
	     determined;

	UShort_t j,
		itrack,
		ipix,
		istr,
		ndownstream,
		ntot,
		naddpix,
		naddstr,
		List[nmaxMvdPixelHits+nmaxMvdStripHits];

	Double_t angle,
		anglemax,
		anglemin,
		dist,
		oldtotal,
		oldtotal2,
		total,
		Dist,
		DIST[MAXMVDTRACKSPEREVENT+1];




	for(itrack=0; itrack<nSttTrackCand; itrack++){
		if( ! keepit[itrack] ) continue;
		if( ! Mvdhits[itrack] ) continue;
		ntot=nPixelHitsinTrack[itrack]+
			nStripHitsinTrack[itrack];
	if( Fifirst[itrack] < -99998. ){  // case with Fifirst[i]=-99999.; in this
					// case the circle is contained
					// in the Mvd region.
		anglemax = 2.*PI;
		anglemin = 0.;
		goto zippo ;
	}




	if(CHARGE[itrack]>0){	// track must rotate clockwise looking into the beam.
		anglemax = FI0[itrack];
		anglemin = Fifirst[itrack];
	} else {
		anglemin = FI0[itrack];
		anglemax = Fifirst[itrack];
	}
		if(anglemax < anglemin) anglemax += 2.*PI;
		if(anglemax < anglemin) anglemax==anglemin; // this is just to be super-sure.


zippo: ;


//  find if this track goes downstream or upstream

		ndownstream=0;
		  for(j=0; j<nPixelHitsinTrack[itrack]; j++){
			if( ZMvdPixel[ListPixelHitsinTrack[itrack][j]]>0.){
				ndownstream++ ;
			}
		  }
		  for(j=0; j<nStripHitsinTrack[itrack]; j++){
			if( ZMvdPixel[ListStripHitsinTrack[itrack][j]]>0.){
				ndownstream++ ;
			}
		  }
		  if(ndownstream>ntot-ndownstream) downstream=true; else downstream=false;



//  loop over the Mvd Pixel and try to attach new Pixels to each candidate track



		naddpix=0;
		for(ipix=0; ipix<nMvdPixelHit; ipix++){
			for(j=0; j<nPixelHitsinTrack[itrack]; j++){
			     if( ListPixelHitsinTrack[itrack][j]==ipix) goto out ;
			}
//				if (    (downstream && ZMvdPixel[ipix]>=0.) ||
//					((!downstream) && ZMvdPixel[ipix]<0.)
//				   ){
					angle = atan2(YMvdPixel[ipix]-Oy[itrack],
							XMvdPixel[ipix]-Ox[itrack]);
					if(angle<0.) angle += 2.*PI;
					if( angle>anglemax){
						angle -= 2.*PI;
						if( angle>anglemax) angle = anglemax;
					} else if (angle<anglemin){
						angle += 2.*PI;
						if (angle<anglemin) angle = anglemin;
					}
					if(angle > anglemin && angle < anglemax)
					{
						dist=fabs( sqrt(
						(Ox[itrack]-XMvdPixel[ipix])*(Ox[itrack]-XMvdPixel[ipix])
						  +(Oy[itrack]-YMvdPixel[ipix])*(Oy[itrack]-YMvdPixel[ipix])
							) -R[itrack]);


						if(dist<highqualitycut)
						{
							List[naddpix]=ipix;
							naddpix++;
						}
					}	// end of  if(angle > anglemin)
//				}	// end of  if (    (downstream &&
			out: ;
		}	//  end of   for(ipix=0; ipix<nMvdPixelHit; ipix++)

		if(naddpix>0){
			for(j=0;j<naddpix;j++){
				ListPixelHitsinTrack[itrack]
				[nPixelHitsinTrack[itrack]+j]=List[j];
			}
			nPixelHitsinTrack[itrack] += naddpix;
		}	//  end of  if(naddpix>0)




		naddstr=0;
		for(istr=0; istr<nMvdStripHit; istr++){
			for(j=0; j<nStripHitsinTrack[itrack]; j++){
			     if( ListStripHitsinTrack[itrack][j]==istr) goto out2 ;
			}
//				if (    (downstream && ZMvdStrip[istr]>=0.) ||
//					( (!downstream) && ZMvdStrip[istr]<0.)
//				   ){
					angle = atan2(YMvdStrip[istr]-Oy[itrack],
							XMvdStrip[istr]-Ox[itrack]);



					if(angle<0.) angle += 2.*PI;
					if( angle>anglemax){
						angle -= 2.*PI;
						if( angle>anglemax) angle = anglemax;
					} else if (angle<anglemin){
						angle += 2.*PI;
						if (angle<anglemin) angle = anglemin;
					}
					if(angle > anglemin && angle < anglemax)
					{
						dist=fabs( sqrt(
						  (Ox[itrack]-XMvdStrip[istr])*(Ox[itrack]-XMvdStrip[istr])
						  +(Oy[itrack]-YMvdStrip[istr])*(Oy[itrack]-YMvdStrip[istr])
							) -R[itrack]);

						if(dist<highqualitycut)
						{
						List[naddstr]=istr;
						naddstr++;
						}
					}	//  end of  if(angle > anglemin)
//				}	// end of    if (    (downstream &&



			out2: ;
		}	//  end of   for(istr=0; istr<nMvdStripHit; istr++)
		if(naddstr>0){
			for(j=0;j<naddstr;j++){
				ListStripHitsinTrack[itrack]
				[nStripHitsinTrack[itrack]+j]=List[j];
			}
			nStripHitsinTrack[itrack] += naddstr;
		}	//  end of  if(naddpix>0)





	}	// end of for(itrack=0; itrack<nSttTrackCand; itrack++)









 return;
}

//------------------------- end of function  PndSttMvdTracking::MatchMvdHitsToSttTracksagain











//------------------------- begin of function  PndSttMvdTracking::CollectParSttHitsagain

	void	PndSttMvdTracking::CollectParSttHitsagain(
			bool *keepit,
			bool *Mvdhits,
			Double_t info[][7],
			UShort_t nSttParHit,
			UShort_t ListAllParHits[nmaxSttHits],

			UShort_t StartTrackCand, // starting investigation from candidate n. StartTrackCand
			UShort_t EndTrackCand,   //  until candidate n. EndTrackCand-1 included.

			Double_t Ox[MAXTRACKSPEREVENT],
			Double_t Oy[MAXTRACKSPEREVENT],
			Double_t R[MAXTRACKSPEREVENT],
			Double_t KAPPA[MAXTRACKSPEREVENT],
			Double_t FI0[MAXTRACKSPEREVENT],
			Double_t Fi_low_limit[MAXTRACKSPEREVENT],
			Double_t Fi_up_limit[MAXTRACKSPEREVENT],
			UShort_t nParHitsinTrack[MAXTRACKSPEREVENT], // input/output
UShort_t ListParHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHitsInTrack] // input/output
						)
{

	UShort_t i,
		itrack,
		ihit,
		j,
		k,
		ntot,
		nadd;

	Double_t angle,
		deltaZ,
		dist,
		dist1,
		Zpos;

	const Double_t NTIMES=0.4;
//	const Double_t NTIMES=1.;


	for(itrack=StartTrackCand; itrack<EndTrackCand; itrack++){
		if( ! keepit[itrack] ) continue;
		if( ! Mvdhits[itrack] ) continue;
		if( Fi_low_limit[itrack] < -99998.) continue;  // case in which the track
						// is completely inside the Mvd region.
		nParHitsinTrack[itrack]=0;

//	loop over the STT parallel hits and try to attach to each candidate track; in this
//	way in one shot I collect also the previously non collected hits and I remove the
//	spurious hits.

		nadd=0;
		for(i=0; i<nSttParHit; i++){
			ihit = ListAllParHits[i];


			if( !InclusionListStt[ihit] ) continue;
			angle = atan2(info[ihit][1]-Oy[itrack],info[ihit][0]-Ox[itrack]);
			if(angle<0.) angle += 2.*PI;


			// selection on Zpos under assumption that for parallel STT
			// makes 1 turn
			if( fabs(KAPPA[itrack]) > 1.e-20){
				deltaZ = 2.*PI/KAPPA[itrack];
				Zpos = (angle - FI0[itrack])/KAPPA[itrack];
				if(
				    fabs(Zpos-info[ihit][2])>1.5*info[ihit][4] &&
				    fabs(Zpos+deltaZ-info[ihit][2])>1.5*info[ihit][4] &&
				    fabs(Zpos-deltaZ-info[ihit][2])>1.5*info[ihit][4]
				  ) continue;
			}

			if( angle>Fi_up_limit[itrack]){
				angle -= 2.*PI;
				if(angle < Fi_low_limit[itrack]) continue;
			} else if (angle < Fi_low_limit[itrack]){
				angle += 2.*PI;
				if(angle > Fi_up_limit[itrack]) continue;
			}



			dist1 = fabs(
					sqrt(
				(Ox[itrack]-info[ihit][0])*(Ox[itrack]-info[ihit][0])+
				(Oy[itrack]-info[ihit][1])*(Oy[itrack]-info[ihit][1])
					) - R[itrack]
				);
			dist = fabs( dist1 - info[ihit][3] );


			// info[ihit][3] = drift radius; if  info[ihit][3]>dist then
			// the drift circle of this straw crosses the trajectory.
			if(dist<NTIMES*STRAWRADIUS || info[ihit][3]> dist ){
				ListParHitsinTrack[itrack][ nParHitsinTrack[itrack] ]
					=ihit;
				nParHitsinTrack[itrack]++;
			}


		}	// end of     for(i=0; i<nSttParHit; i++)




	}	// end of for(itrack=StartTrackCand; itrack<EndTrackCand; itrack++)

 return;
}

//------------------------- end of function  PndSttMvdTracking::CollectParSttHitsagain









//------------------------- begin of function  PndSttMvdTracking::getMCInfo

  void PndSttMvdTracking::getMCInfo(
  					Int_t MCTrack,
					Double_t * Cx,
					Double_t * Cy,
					Double_t * Rr
				   )
{
		Int_t icode;
		Double_t aaa, Dd, Fifi, Oxx, Oyy, Pxx, Pyy, carica  ;
		PndMCTrack* pMC;


		if( MCTrack <0) {
			*Rr=-2.;
			return;
		}


		pMC = (PndMCTrack*) fMCTrackArray->At(MCTrack);
		if ( pMC ) {
			icode  = pMC->GetPdgCode() ;    //   PDG code of track
			Oxx = pMC->GetStartVertex().X();    //   X of starting point track
			Oyy = pMC->GetStartVertex().Y();    //   Y of starting point track
			Pxx = pMC->GetMomentum().X();
			Pyy = pMC->GetMomentum().Y();
			aaa = sqrt( Pxx*Pxx + Pyy*Pyy);
			*Rr = aaa*1000./(BFIELD*CVEL);    //   R (cm) of Helix of track
						      //   projected in XY plane; B = 2 Tesla
			TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
			TParticlePDG *fParticle= fdbPDG->GetParticle(icode);
			if (icode>1000000000) carica = 1.;
			else  carica = fParticle->Charge()/3. ;    //   charge of track
			if(fabs(carica)<1.e-5) { *Rr = -3.; return;}
			*Cx = Oxx + Pyy*1000./(BFIELD*CVEL*carica);
			*Cy = Oyy - Pxx*1000./(BFIELD*CVEL*carica);
		} else {
			*Rr = -1.;
		}



	return;
}

//------------------------- end of function  PndSttMvdTracking::getMCInfo




//------------------------- begin of function  PndSttMvdTracking::FindDistance

  Double_t PndSttMvdTracking::FindDistance(
			Double_t Ox,	//  center from wich distance is calculated
			Double_t Oy,	//  center from wich distance is calculated
			Double_t R,
			Double_t tanlow,
			Double_t tanmid,
			Double_t tanup,
			Double_t alfa,	//  intersection circumference parameter
			Double_t beta,	//  intersection circumference parameter
			Double_t gamma	//  intersection circumference parameter
			)
{


	UShort_t i,
		 n;

	Double_t Delta,
		 m[3],
		 q,
		 dist,
		 dist1,
		 dist2,
		 distlow,
		 distmid,
		 distup,
		 totaldist,
		 x1,
		 x2,
		 y1,
		 y2;

int nevento=1;





	m[0] = tanlow;
	m[1] = tanmid;
	m[2] = tanup;

	n=0;
	totaldist=0.;


    for(i=0;i<3;i++){

	if( tanlow<999998.) {
		q = Oy-m[i]*Ox;
		Delta = alfa*alfa + beta*beta*m[i]*m[i] - 4.*gamma*m[i]*m[i] - 4.*q*q + 4.*m[i]*q*alfa +
			+ 2.*alfa*beta*m[i] - 4.*beta*q - 4.*gamma;
		if( Delta < 0.){
			dist = -1.;
		} else if (Delta==0.){
			x1 = 0.5*(-alfa - 2.*m[i]*q - beta*m[i] )/(1.+m[i]*m[i]);
			y1 = m[i]*x1+q;
			dist = fabs( sqrt( (Ox-x1)*(Ox-x1) + (Oy-y1)*(Oy-y1) ) - R);
		} else {
			Delta = sqrt(Delta);
			x1 = 0.5*(-alfa - 2.*m[i]*q - beta*m[i] - Delta)/(1.+m[i]*m[i]);
			x2 = 0.5*(-alfa - 2.*m[i]*q - beta*m[i] + Delta)/(1.+m[i]*m[i]);
			y1 = m[i]*x1+q;
			y2 = m[i]*x2+q;
			dist1 = fabs( sqrt( (Ox-x1)*(Ox-x1) + (Oy-y1)*(Oy-y1) ) - R);
			dist2 = fabs( sqrt( (Ox-x2)*(Ox-x2) + (Oy-y2)*(Oy-y2) ) - R);
			if(dist1<dist2){
				dist = dist1;
			} else{
				dist = dist2;
			}
		}
	} else {
		Delta =  beta*beta - 4.*Ox*Ox - 4.*Ox*alfa - 4.*gamma;

		if( Delta < 0.){
			dist = -1.;
		} else if (Delta==0.){
			dist = fabs( fabs(Oy+0.5*beta) - R);
		} else {
			Delta = sqrt(Delta);
			y1 = -0.5*beta + Delta/2.;
			y2 = -0.5*beta - Delta/2.;
			dist1 = fabs( fabs(Oy+0.5*beta + Delta/2.) - R);
			dist2 = fabs( fabs(Oy+0.5*beta - Delta/2.) - R);
			if(dist1<dist2) dist = dist1;
			else  dist = dist2;
		}
	}

	if( dist>-0.5) {
		totaldist+=dist;
		n++;
	}


    }	//   end of  for(i=0;i<3;i++)


	if(n!=3)  totaldist = -1.;
	else	totaldist = totaldist / n;

	return totaldist;

}



//------------------------- end of function  PndSttMvdTracking::FindDistance


//------------------------- begin of function  PndSttMvdTracking::CalculateSandZ

	void PndSttMvdTracking::CalculateSandZ(
						Double_t Ox,
						Double_t Oy,
						Double_t R,
						Short_t skewnum,
						Double_t info[nmaxSttHits][7],
						Double_t *WDX,
						Double_t *WDY,
						Double_t *WDZ,
						Double_t S[2],
						Double_t Z[2], //  Zcoordinate of the central wire.
						Double_t Zdrift[2],// drift radius projected onto the Helix.
						Double_t Zerror[2] //  150 micron projected onto the Helix.
						)
{


	Int_t STATUS;
	Short_t i,
		j,
		ii;

	Double_t aaa,
		bbb,
		distance,
		Aellipsis1,
		Bellipsis1,
		LL,
		SkewInclWithRespectToS,
		Rx,
		Ry,
		vx1,
		vy1,
		vz1,
		C0x1,
		C0y1,
		C0z1,
		POINTS1[6];


	 Z[0]= 999999.;
	 Z[1]= 999999.;
         i = skewnum ;

         aaa = sqrt(WDX[i]*WDX[i]+WDY[i]*WDY[i]+ WDZ[i]*WDZ[i]);
         vx1 = WDX[i]/aaa;
         vy1 = WDY[i]/aaa;
         vz1 = WDZ[i]/aaa;
         C0x1 = info[i][0];
         C0y1 = info[i][1];
         C0z1 = info[i][2];


       calculateintersections(Ox,Oy,R,C0x1,C0y1,C0z1,info[i][3],
                              vx1,vy1,vz1,
                              &STATUS,POINTS1);

       if(STATUS < 0 ) return;



       for( ii=0; ii<2; ii++){
        j=3*ii;
        distance = sqrt(
                  (POINTS1[j]-C0x1)*(POINTS1[j]-C0x1) + 
                  (POINTS1[1+j]-C0y1)*(POINTS1[1+j]-C0y1) + 
                  (POINTS1[2+j]-C0z1)*(POINTS1[2+j]-C0z1) 
                            );


        Rx = POINTS1[j]-Ox ;   //  x component Radial vector of cylinder of trajectory
        Ry = POINTS1[1+j]-Oy ;   //  y direction Radial vector of cylinder of trajectory

        aaa = sqrt(Rx*Rx+Ry*Ry);
        SkewInclWithRespectToS = (-Ry*vx1 + Rx*vy1)/aaa ;
        SkewInclWithRespectToS /= R;
        bbb = sqrt( SkewInclWithRespectToS*SkewInclWithRespectToS + vz1*vz1);



        //  the tilt direction of this ellipse is (1,0)  when major axis along Z direction



        LL = fabs(vx1*Rx + vy1*Ry);
        if( LL < 1.e-10) continue;
        Aellipsis1 = info[i][3]*aaa/LL;

        Bellipsis1 = info[i][3]/R;
        if( distance >= info[i][4]  + Aellipsis1) continue;

//--------------------------
        S[ii] = atan2(POINTS1[j+1]-Oy, POINTS1[j]-Ox) ;  // atan2 returns radians in (-pi and +pi]
        if( S[ii] < 0.) S[ii] += 2.*PI;


	Z[ii] = POINTS1[j+2];
	Zdrift[ii] =  Aellipsis1;
	Zerror[ii] = STRAWRESOLUTION*aaa/LL;



//	Double_t rotation1 = 180.*atan2(Tiltdirection1[1],Tiltdirection1[0])/PI;


   }    //  end of    for( ii=0; ii<2; ii++)

	return;
}

//-------------------------  end of function  PndSttMvdTracking::CalculateSandZ




//-------------------------  begin of function  PndSttMvdTracking::EliminateSpuriousSZ

  void PndSttMvdTracking::EliminateSpuriousSZ(
			UShort_t MaxTurnofTracks,
			UShort_t *nPixelHitsinTrack,
			UShort_t *ListPixelHitsinTrack,
			UShort_t *nStripHitsinTrack,
			UShort_t *ListStripHitsinTrack,
			UShort_t *nSkewHitsinTrack,
			UShort_t *ListSkewHitsinTrack,
			Double_t *S,
			Double_t *ZED,
			Double_t *DriftRadius,
			Double_t *ErrorDriftRadius,
			Double_t *SchosenPixel,
			Double_t *SchosenStrip,
			Double_t *SchosenSkew,
			Double_t *ZchosenPixel,
			Double_t *ZchosenStrip,
			Double_t *ZchosenSkew,
			Double_t *ErrorchosenPixel,
			Double_t *ErrorchosenStrip,
			Double_t *ErrorchosenSkew,
			Double_t KAPPA,
			Double_t FI0,
			Double_t R
				    )
{

	UShort_t i,
		 j,
		 k,
		 Kmin,
		 auxnMvdPixel,
		 auxListMvdPixel[nmaxMvdPixelHits],
		 auxnMvdStrip,
		 auxListMvdStrip[nmaxMvdStripHits],
		 auxnSttSkew,
		 auxListSttSkew[nmaxSttHits];

	Int_t  nr2,
		 nrounds0,
		 nrounds1,
		 nchosen,
		 Nround[4];

//	const Double_t  MvdCut=0.8,
	const Double_t  MvdCut=0.3,
//	const Double_t  MvdCut=0.1,
			minimumSttDriftError = 1.;

	Double_t ddd,
		 dista,
		 dista1,
		 dista0,
		 d_min,
		 error,
		 zeta0,
		 zeta1,
		 Dista[4],
		 Errore[4],
		 Esse[4],
		 Zeta[4];


	auxnMvdPixel=0;
	auxnMvdStrip=0;
	auxnSttSkew=0;


	for(i=0;i<*nPixelHitsinTrack;i++){

		if( fabs(Dist_SZ(R,KAPPA,FI0,ZED[i],S[i],&nrounds0)) < MvdCut
						&&
					abs(nrounds0)<=MaxTurnofTracks){
			auxListMvdPixel[auxnMvdPixel]=ListPixelHitsinTrack[i];
			if(nrounds0>=0)SchosenPixel[ListPixelHitsinTrack[i]]=S[i]+nrounds0*2.*PI;
			else SchosenPixel[ListPixelHitsinTrack[i]]=S[i]+(nrounds0-1)*2.*PI;
			ZchosenPixel[ListPixelHitsinTrack[i]]=ZED[i];
			ErrorchosenPixel[ListPixelHitsinTrack[i]]=ERRORPIXEL;
			auxnMvdPixel++;
		}
	}	// end of  for(i=0;i<*nPixelHitsinTrack;i++)

	for(j=0;j<*nStripHitsinTrack;j++){
		i=j+(*nPixelHitsinTrack);

		if( fabs(Dist_SZ(R,KAPPA,FI0,ZED[i],S[i],&nrounds0)) < MvdCut
						&&
					abs(nrounds0)<=MaxTurnofTracks){
			auxListMvdStrip[auxnMvdStrip]=ListStripHitsinTrack[j];
			if(nrounds0>=0)SchosenStrip[ListStripHitsinTrack[j]]=S[i]+nrounds0*2.*PI;
			else SchosenStrip[ListStripHitsinTrack[j]]=S[i]+(nrounds0-1)*2.*PI;
			ZchosenStrip[ListStripHitsinTrack[j]]=ZED[i];
			ErrorchosenStrip[ListStripHitsinTrack[j]]=ERRORSTRIP;
			auxnMvdStrip++;
		}
	}	// end of  for(j=0;j<*nStripHitsinTrack;j++)

//-----------------stampe.
if(istampa>=3){
	cout<<"in EliminateSpuriousSZ : nSkew hit = "<<*nSkewHitsinTrack
	<<", K = "<< KAPPA<<", FI0 = "<< FI0 <<endl;
}
//--------------fine stampe.

	for(j=0;j<*nSkewHitsinTrack;j++){
		i=j+(*nPixelHitsinTrack)+
		    (*nStripHitsinTrack);


//-----------------stampe.
if(istampa>1){
	cout<<"in EliminateSpuriousSZ : skew hit n. "<<ListSkewHitsinTrack[j];
	cout<<"  ZED[i] = "<<ZED[i]<<", S[i] "<<S[i];
	cout<<endl;
}
//--------------fine stampe.
			dista =
		fabs(Dist_SZ(R,KAPPA,FI0,ZED[i]+DriftRadius[i],S[i],&nrounds0));
			ddd = fabs(Dist_SZ(R,KAPPA,FI0,ZED[i]-DriftRadius[i],S[i],&nr2));
			if( abs(nrounds0) > MaxTurnofTracks &&
				 abs(nr2)>MaxTurnofTracks)
			{
				continue;
			} else if ( abs(nr2)>MaxTurnofTracks)
			{
				 zeta0 = ZED[i]+DriftRadius[i];
				 nchosen=nrounds0;
			} else if ( abs(nrounds0) > MaxTurnofTracks )
			{
				 dista = ddd;
				 zeta0 = ZED[i]-DriftRadius[i];
				 nchosen=nr2;
			} else
			{
				if( fabs(dista)>fabs(ddd) ) {
				 dista = ddd;
				 zeta0 = ZED[i]-DriftRadius[i];
				 nchosen=nr2;
				} else {
				 zeta0 = ZED[i]+DriftRadius[i];
				 nchosen=nrounds0;
				}
			}
			error = DriftRadius[i];
			if(nchosen>=0)SchosenSkew[ListSkewHitsinTrack[j]]=S[i]+nchosen*2.*PI;
			else SchosenSkew[ListSkewHitsinTrack[j]]=S[i]+(nchosen-1)*2.*PI;
			ZchosenSkew[ListSkewHitsinTrack[j]]=zeta0;



//-----------------stampe.
if(istampa>=3){
	cout<<"in EliminateSpuriousSZ : insomma, dista prima della selezione = "<<
	dista<< ", ed e' da comparare\n\tcon 4*error = "<<  4.*error<<endl<<
	"\toppure con 2.*minimumSttDriftError = "<<2.*minimumSttDriftError<<endl;
}
//--------------fine stampe.

		if(
			dista < 4.*error
//			dista < 1.1*error
				||
			dista < 2.*minimumSttDriftError
			){
			auxListSttSkew[auxnSttSkew]=ListSkewHitsinTrack[j];
			ErrorchosenSkew[ListSkewHitsinTrack[j]]=error;
			auxnSttSkew++;
		}
	}	// end of  for(j=0;j<*nSkewHitsinTrack;j++)




//	reload the list of good hits

	*nPixelHitsinTrack = auxnMvdPixel;
	*nStripHitsinTrack = auxnMvdStrip;
	*nSkewHitsinTrack = auxnSttSkew;
	for(j=0;j<*nPixelHitsinTrack;j++){
		ListPixelHitsinTrack[j]=auxListMvdPixel[j];
	}	// end of  for(j=0;j<*nPixelHitsinTrack;j++)
	for(j=0;j<*nStripHitsinTrack;j++){
		ListStripHitsinTrack[j]=auxListMvdStrip[j];
	}	// end of  for(j=0;j<*nStripHitsinTrack;j++)
	for(j=0;j<*nSkewHitsinTrack;j++){
		ListSkewHitsinTrack[j]=auxListSttSkew[j];
	}	// end of  for(j=0;j<*nSkewHitsinTrack;j++)


	return;

}
//-------------------------  end of function  PndSttMvdTracking::EliminateSpuriousSZ



//-------------------------  begin of function  PndSttMvdTracking::Dist_SZ

  Double_t PndSttMvdTracking::Dist_SZ(
					Double_t R,
					Double_t KAPPA,
					Double_t FI0,
					Double_t ZED,
					Double_t S,
					Int_t *nrounds
					)
{

//	Defining :	ZZ = (S-FI0)/KAPPA
//	this method returns the distance (WITH ITS SIGN ) :  ZZ - ZED.  Therefore this number
//	can be negative.
//	Care is taken to calculate this distance properly taking into
//	account that we are dealing with the function  FI = mod(KAPPA*Z + FI0, 2*3.14). 

// the limits of an Int_t are : -2147483648 <= n <= 2147483647

	Double_t aaa,
		ABSdis1,
		dis1,
		dis2,
		dis_segments,
		gap;

	if(fabs(KAPPA) < 1.e-10){
		return -999999999.;
	} else if (fabs(KAPPA)>1.e10) {
		return -ZED;
	}

//	gap = fabs(2.*PI/KAPPA);
	
	aaa = (KAPPA*ZED)/(2.*PI);
	if( aaa<-2147483648.) {
		*nrounds = -2147483647;
	} else if (aaa > 2147483647.) {
		*nrounds = 2147483647;
	} else {
		*nrounds = (Int_t) aaa;
	}

	dis_segments = 2.*PI*R/sqrt(1.+KAPPA*KAPPA*R*R); // distance between
						// two consecutive segments
						//  of trajectory.

	dis1 = fabs( R*S -R*KAPPA*ZED - R*FI0)/sqrt( 1.+KAPPA*KAPPA*R*R);
	dis1 = fmod(dis1,dis_segments);
	dis2 = dis_segments-dis1; if(dis2<0.)dis2==0.;


	if( dis1 < dis2 )
	{
		return dis1;
	} else {
		return dis2;
	}

}

//-------------------------  end of function  PndSttMvdTracking::Dist_SZ




//-------------------------  begin of function  PndSttMvdTracking::SignedDist_SZbis

//obsolete
  Double_t PndSttMvdTracking::SignedDist_SZbis(
  					Double_t KAPPA,
					Double_t FI0,
					Double_t ZED,
					Double_t S,
					Short_t *nrounds
  					)
{

//	Defining :	ZZ = (S-FI0)/KAPPA
//	this method returns the distance (WITH ITS SIGN ) :  ZZ - ZED.  Therefore this number
//	can be negative.
//	Care is taken to calculate this distance properly taking into
//	account that we are dealing with the function  FI = mod(KAPPA*Z + FI0, 2*3.14). 

	Short_t n;

	Double_t ABSdis1,
		dis1,
		dis2,
		gap;

	if(fabs(KAPPA) < 1.e-10){
		return -999999999.;
	} else if (fabs(KAPPA)>1.e10) {
		return -ZED;
	}

	gap = fabs(2.*PI/KAPPA);
	n = (Short_t) ((KAPPA*ZED+FI0)/(2.*PI));
	*nrounds = (Short_t) (KAPPA*ZED/(2.*PI));
	if(n<0) n--;
	dis1 = ((S + n*2.*PI ) - FI0)/KAPPA  - ZED ;
	ABSdis1 = fabs(dis1);
	dis2 = gap - ABSdis1;
	if (dis2<= 0.) return 0.;	// in this case the distance is actually 0. (because of rounding
				// errors it may appear as <0.)
	if( ABSdis1 < dis2 )
	{
		return dis1;
	} else {
		if( dis1>0) return -dis2;
		else   return dis2;
	}

}

//-------------------------  end of function  PndSttMvdTracking::SignedDist_SZbis




//---------- begin of function PndSttMvdTracking::PndSttInfoXYZParal


    void PndSttMvdTracking::PndSttInfoXYZParal(
                             Double_t info[][7],
                             UShort_t infopar,
                             Double_t Ox,
                             Double_t Oy,
                             Double_t R,
                             Double_t KAPPA,
                             Double_t FI0,
                             Short_t Charge,
                             Double_t *Posiz
                            )
{

   Double_t fi, norm, vers[2];

   vers[0] = Ox - info[infopar][0];
   vers[1] = Oy - info[infopar][1];
   norm = sqrt( vers[0]*vers[0] + vers[1]*vers[1] );

   if(norm < 1.e-20) {
     Posiz[0] = -999999999.;
     return;
   }
   





   if( fabs( R - fabs( norm - info[infopar][3] ) ) // distance trajectory-drift radius
				<
		fabs( R - (norm + info[infopar][3]) )  ) {

	Posiz[0] = info[infopar][0] + info[infopar][3]*vers[0]/norm;
	Posiz[1] = info[infopar][1] + info[infopar][3]*vers[1]/norm;

   } else {

	Posiz[0] = info[infopar][0] - info[infopar][3]*vers[0]/norm;
	Posiz[1] = info[infopar][1] - info[infopar][3]*vers[1]/norm;

   }	// end of if ( fabs( R - fabs( Distance - info[infopar][3] ) ).....




//   Posiz[0] = info[infopar][0] + info[infopar][3]*vers[0]/norm;
//   Posiz[1] = info[infopar][1] + info[infopar][3]*vers[1]/norm;

   if( fabs(KAPPA)<1.e-20 ){
     Posiz[2] = -888888888.;
     return;
   }


   fi = atan2(-vers[1],-vers[0]);
   if(fi<0.)  fi += 2.*PI;

   if ( Charge > 0){
    if(fi > FI0 )  FI0 += 2.*PI;
//    Posiz[2] = (FI0-fi)/KAPPA;
   } else {
    if(fi < FI0 )  fi += 2.*PI;
   }
    Posiz[2] = (fi-FI0)/KAPPA;

   return;
}

//----------end of function PndSttMvdTracking::PndSttInfoXYZParal




//----------start of function PndSttMvdTracking::FixDiscontinuitiesFiangleinSZplane

  void PndSttMvdTracking::FixDiscontinuitiesFiangleinSZplane(
		UShort_t TemporarynSkewHitsinTrack,
		Double_t *S,
		Double_t *Fi_initial_helix_referenceframe,
		Short_t Charge
			)
{

 UShort_t i;
 Double_t max, min;

 if( Charge >0 )
 {
	for(i=0 ; i<TemporarynSkewHitsinTrack; i++){
		if( S[i] > *Fi_initial_helix_referenceframe )  S[i]-= 2.*PI;
	}
 } else {
	for(i=0 ; i<TemporarynSkewHitsinTrack; i++){
		if( S[i] < *Fi_initial_helix_referenceframe )  S[i]+= 2.*PI;
	}
 }

     return;

}
//----------end of function PndSttMvdTracking::FixDiscontinuitiesFiangleinSZplane



//----------start  function PndSttMvdTracking::PndSttFindingParallelTrackAngularRange


      void   PndSttMvdTracking::PndSttFindingParallelTrackAngularRange(
				Double_t oX,
				Double_t oY,
				Double_t R,
				Short_t  Charge,
				Double_t *Fi_low_limit,
				Double_t *Fi_up_limit,
				Short_t * status,
				Double_t Rmi,	// Rmin of cylindrical volume intersected by track;
				Double_t Rma	// Rmax of cylindrical volume intersected by track;
									)
{
// -------------- calculate the maximum fi and minimum fi spanned by this track,

// see logbook pag.270; by using the Rmin and Rmax of the straw detector.
// this function works also when circular trajectory in XY doesn't pass through (0,0).

	bool	intersection_inner,
		intersection_outer;
	Double_t	teta1,
			teta2,
			tetavertex,
			a,
			cosT,
			cost,
			cosFi,
			cosfi,
			Fi,
			fi,
			FI0,
			Px,
			Py,
			tmp;


	Rma += 1. ; // add a safety margin.
	Rmi -= 1. ; // add a safety margin.




	a = sqrt(oX*oX+oY*oY);

	//  preliminary condition
	if(a + R <= Rmi )	// in this case there might be hits at radius < Rmi.
		 { *status = -1 ;return;}
	if( a >= R + Rma || R >= a + Rma)  // in this case there can be no hits with radius < Rma.
		 { *status = -2;return;}

	if( a - R >= Rmi ) intersection_inner = false; else intersection_inner = true;

	if( a + R <= Rma || a - R >= Rma  )
		 intersection_outer = false; else intersection_outer = true;

	if( (! intersection_inner) && (! intersection_outer) ){
		*Fi_low_limit = 0.;
		*Fi_up_limit = 2.*PI;
		*status = 1;
		return;
	}

//	now the calculation

	FI0 = atan2(-oY,-oX);
	if( intersection_outer ){
		cosFi = (a*a + R*R - Rma*Rma)/(2.*R*a);
		if(cosFi<-1.) cosFi=-1.; else if(cosFi>1.) cosFi=1.;
		Fi = acos(cosFi);
	}

	if( intersection_inner ){
		cosfi = (a*a + R*R - Rmi*Rmi)/(2.*R*a);
		if(cosfi<-1.) cosfi=-1.; else if(cosfi>1.) cosfi=1.;
		fi = acos(cosfi);
	}


	if( Charge < 0.){ // this particle rotates counterclockwise when looking into the beam
		if( intersection_outer && intersection_inner){
			*Fi_low_limit=FI0 + fi;
			*Fi_up_limit= FI0 +Fi;

		} else if (intersection_inner) {
			*Fi_low_limit=FI0 + fi;
			*Fi_up_limit= FI0 - fi;
		} else {
			*Fi_low_limit=FI0 - Fi;
			*Fi_up_limit= FI0 + Fi;
		}	// end of    if( intersection_outer && intersection_inner



	} else {	// continuation of   if( Charge < 0.)

		if( intersection_outer && intersection_inner){
			*Fi_low_limit=FI0 - Fi;
			*Fi_up_limit= FI0 - fi;

		} else if (intersection_inner) {
			*Fi_low_limit=FI0 + fi;	// must invert because low limit must be < up limit
			*Fi_up_limit= FI0 - fi;
		} else {
			*Fi_low_limit=FI0 - Fi;
			*Fi_up_limit= FI0 + Fi;
		}	// end of    if( intersection_outer && intersection_inner


	}	// end of  if( Charge < 0.)




	if(*Fi_low_limit<0.) {
		*Fi_low_limit=fmod(*Fi_low_limit,2.*PI);
		*Fi_low_limit += 2.*PI;
	} else if (*Fi_low_limit>=2.*PI){
		*Fi_low_limit=fmod(*Fi_low_limit,2.*PI);
	}
	if(*Fi_up_limit<0.) {
		*Fi_up_limit=fmod(*Fi_up_limit,2.*PI);
		*Fi_up_limit += 2.*PI;
	} else if (*Fi_up_limit>=2.*PI){
		*Fi_up_limit=fmod(*Fi_up_limit,2.*PI);
	}

	//	Modify *Fi_up_limit by adding
	//	2PI if it is the case, in order to make *Fi_up_limit > *Fi_low_limit.
	if( *Fi_up_limit < *Fi_low_limit ) *Fi_up_limit += 2.*PI;
	if( *Fi_up_limit < *Fi_low_limit ) *Fi_up_limit = *Fi_low_limit;

	*status = 0;



      return;
}


//---------- end of  function PndSttMvdTracking::PndSttFindingParallelTrackAngularRange



//----------begin of function PndSttMvdTracking::AssociateSkewHitsToXYTrack

  UShort_t PndSttMvdTracking::AssociateSkewHitsToXYTrack(
                   bool *ExclusionListSkew,
		   UShort_t NSkewhits,
		   UShort_t *infoskew,
                   Double_t Ox,
                   Double_t Oy,
                   Double_t R,
                   Double_t info[][7],
                   Double_t *WDX,
                   Double_t *WDY,
                   Double_t *WDZ,
                   Double_t Fi_low_limit,
                   Double_t Fi_up_limit,
                   Short_t  Charge,
                   UShort_t SkewList[nmaxSttHits][2], // output,  list of selected skew hits (in skew numbering)
                   Double_t *S,       //  output,  S coordinate of selected Skew hit
                   Double_t *Z,       //  output,  Z coordinate of selected Skew hit
                   Double_t *ZDrift,   //  output,  drift distance IN Z DIRECTION only, of selected Skew hit
                   Double_t *ZErrorafterTilt   //  output,  Radius taking into account the tilt, IN Z DIRECTION only, of selected Skew hit
							)
 {



    Int_t i, j, i1, ii, iii, NAssociated, Kincl, nlow, nup, STATUS, Nmin, Nmax;

    Double_t xmin , xmax, ymin, ymax,
           dx, dy, diff, d1, d2,
           delta, deltax, deltay, deltaz, deltaS,
           factor,
           zmin, zmax, Smin, Smax, S1, S2,
           z1, z2, y1, y2,
           vx1, vy1, vz1, C0x1, C0y1, C0z1,
           aaa, bbb, ccc, angle, minor, major,
           distance, Rx, Ry, LL,
           Aellipsis1, Bellipsis1,fi1,
           fmin, fmax, offset, step,
           SkewInclWithRespectToS, zpos, zpos1, zpos2,
           Tiltdirection1[2],
           zl[200],zu[200],
           POINTS1[6];


//   calculate the Fi range allowed to the skew hits, with Fi calculated in the TRACK CYLINDER REFERENCE FRAME.


      NAssociated=0;

       for( iii=0; iii< NSkewhits; iii++) {
         i = infoskew[iii];
         if( !ExclusionListSkew[i]) continue;


         aaa = sqrt(WDX[i]*WDX[i]+WDY[i]*WDY[i]+WDZ[i]*WDZ[i]);
         vx1 = WDX[i]/aaa;
         vy1 = WDY[i]/aaa;
         vz1 = WDZ[i]/aaa;
         C0x1 = info[i][0];
         C0y1 = info[i][1];
         C0z1 = info[i][2];

       calculateintersections(Ox,Oy,R,C0x1,C0y1,C0z1,info[i][3],
                              vx1,vy1,vz1,
                              &STATUS,POINTS1);

       if(STATUS < 0 ) continue ;


       for( ii=0; ii<2; ii++){

        j=3*ii;
        distance = sqrt(
                  (POINTS1[j]-C0x1)*(POINTS1[j]-C0x1) + 
                  (POINTS1[1+j]-C0y1)*(POINTS1[1+j]-C0y1) + 
                  (POINTS1[2+j]-C0z1)*(POINTS1[2+j]-C0z1) 
                            );


        Rx = POINTS1[j]-Ox ;   //  x component Radial vector of cylinder of trajectory
        Ry = POINTS1[1+j]-Oy ;   //  y direction Radial vector of cylinder of trajectory

        aaa = sqrt(Rx*Rx+Ry*Ry);
        SkewInclWithRespectToS = (-Ry*vx1 + Rx*vy1)/aaa ;
        SkewInclWithRespectToS /= R;
        bbb = sqrt( SkewInclWithRespectToS*SkewInclWithRespectToS + vz1*vz1);
        //  the tilt direction of this ellipse is (1,0)  when major axis along Z direction
        if( bbb > 1.e-10){
           Tiltdirection1[0] = vz1/bbb;
           Tiltdirection1[1] = SkewInclWithRespectToS/bbb;
        } else {
           Tiltdirection1[0] = 1.;
           Tiltdirection1[1] = 0.;
        }

        LL = fabs(vx1*Rx + vy1*Ry);
        if( LL < 1.e-10) continue;

        Aellipsis1 = info[i][3]*aaa/LL;

        Bellipsis1 = info[i][3]/R;

        if( distance >= info[i][4]+Aellipsis1 ){
		 continue;
	}




        S[NAssociated] = atan2(POINTS1[j+1]-Oy, POINTS1[j]-Ox) ;  // atan2 returns radians in (-pi and +pi]
        if( S[NAssociated] < 0.) S[NAssociated] += 2.*PI;


        if(  S[NAssociated] < Fi_low_limit) {
           if(  S[NAssociated]+2.*PI > Fi_up_limit)  continue;
        }  else if(  S[NAssociated] > Fi_up_limit) {
	   if(  S[NAssociated]- 2.*PI < Fi_low_limit)  continue;
        }



//---------------------------   end check


        Z[NAssociated] = POINTS1[j+2];
        ZDrift[NAssociated] = Aellipsis1*Tiltdirection1[0];
        ZErrorafterTilt[NAssociated] = 0.02*aaa*Tiltdirection1[0]/LL;
        SkewList[NAssociated][0] = i;  // n. skew hit in original hit numbering
        SkewList[NAssociated][1] = ii;  //  solution 0 or solution 1 were accepted


// check if this skew hit doesn't "push out"  the most external parallel hit
// (see Gianluigi's logbook on page 251)

/*
        Double_t Zh1 = Z[NAssociated] - ZDrift[NAssociated];
        Double_t Zh2 = Z[NAssociated] + ZDrift[NAssociated];
        Double_t Sh1 = S[NAssociated] - Aellipsis1*Tiltdirection1[1];
        Double_t Sh2 = S[NAssociated] + Aellipsis1*Tiltdirection1[1];
        Double_t Zlast1 = (Fi_final_helix_referenceframe-Fi_initial_helix_referenceframe)*Zh1
                                  /(Sh1-Fi_initial_helix_referenceframe);
        Double_t Zlast2 = (Fi_final_helix_referenceframe-Fi_initial_helix_referenceframe)*Zh2
                                  /(Sh2-Fi_initial_helix_referenceframe);



        if( fabs(Zlast1 - ZCENTER_STRAIGHT) > SEMILENGTH_STRAIGHT
                                  &&
            fabs(Zlast2 - ZCENTER_STRAIGHT) > SEMILENGTH_STRAIGHT
           )   continue;
*/
        NAssociated++;

   }    //  end of    for( ii=0; ii<2; ii++)

  }   //   for( iii=0; iii< NSkewhits; iii++)



  return NAssociated;



 }

//----------end of function PndSttMvdTracking::AssociateSkewHitsToXYTrack

//----------begin of function PndSttMvdTracking::CalculateCircleThru3Points

  bool PndSttMvdTracking::CalculateCircleThru3Points(
			Double_t x1,
			Double_t y1,
			Double_t x2,
			Double_t y2,
			Double_t x3,
			Double_t y3,
			Double_t *Ox,
			Double_t *Oy,
			Double_t *R)
{
	// inspiration from  http://mathworld.wolfram.com/Circle.html

	Double_t a,
		 d,
		 e,
		 q1,
		 q2,
		 q3;

	a = x1*y2 + y1*x3 + x2*y3 - x3*y2 - x2*y1 - x1*y3;

	if( fabs(a)<1.e-10 ) return false;

	q1 = x1*x1 + y1*y1;
	q2 = x2*x2 + y2*y2;
	q3 = x3*x3 + y3*y3;

	d = -(q1*y2 + y1*q3 + q2*y3 - q3*y2 - q2*y1 - q1*y3);

	e = -(x1*q2 + q1*x3 + x2*q3 - x3*q2 - x2*q1 - x1*q3);

	*Ox = -0.5*d/a;
	*Oy = -0.5*e/a;
	*R = sqrt( (x1-*Ox)*(x1-*Ox) + (y1-*Oy)*(y1-*Oy) );


	return true;
}
//----------end of function PndSttMvdTracking::CalculateCircleThru3Points





//----------begin of function PndSttMvdTracking::Ordering_Loading_ListTrackCandHit
  void PndSttMvdTracking::Ordering_Loading_ListTrackCandHit(
			bool *keepit,
			UShort_t FirstCandidate,
			UShort_t LastCandidate,
			Double_t info[][7],
			Double_t * Ox,
			Double_t * Oy,
			Double_t * Rr,
			Double_t Trajectory_Start[MAXTRACKSPEREVENT][2],
			Short_t *CHARGE,
			Double_t SchosenSkew[][nmaxSttHits]
				)
{
	UShort_t ncand;

	for(ncand=FirstCandidate; ncand< LastCandidate; ncand++){
		// for small radius trajectory better the ordering with conformal.
		if( Rr[ncand]< RStrawDetectorMax/2.){
			OrderingConformal_Loading_ListTrackCandHit(
				keepit,
				ncand,
				info,
				Ox,
				Oy,
				Rr,
				Trajectory_Start,
				CHARGE,
				SchosenSkew
						);
		} else { // otherwise it is better distance from (0,0) method.
			OrderingR_Loading_ListTrackCandHit(
				keepit,
				ncand,
				info
						);
		}

	} //   end of  for(ncand=FirstCandidate; ncand< LastCandidate; ncand++)


	return;
}
//----------end of function PndSttMvdTracking::Ordering_Loading_ListTrackCandHit










//----------begin of function PndSttMvdTracking::OrderingR_Loading_ListTrackCandHit
  void PndSttMvdTracking::OrderingR_Loading_ListTrackCandHit(
			bool *keepit,
			UShort_t ncand,
			Double_t info[][7]
				)
{

	UShort_t	i,
			j,
			ipar,
			iskew;


//     ordering all the hits belonging to the candidate track, by increasing R;
//     forming the new track with Mvd+Stt hits


		if(!keepit[ncand]) return;
		nTrackCandHit[ncand] =nSttParHitsinTrack[ncand]+nSttSkewHitsinTrack[ncand]+
					nMvdPixelHitsinTrack[ncand]+
					nMvdStripHitsinTrack[ncand];
		UShort_t tempmvdindex[nMvdPixelHitsinTrack[ncand]+
					nMvdStripHitsinTrack[ncand] ],
			 tempmvdtype[nMvdPixelHitsinTrack[ncand]+
					nMvdStripHitsinTrack[ncand] ];
		Int_t auxIndex[nMvdPixelHitsinTrack[ncand]+
					nMvdStripHitsinTrack[ncand] ];
		Double_t auxR[nMvdPixelHitsinTrack[ncand]+
					nMvdStripHitsinTrack[ncand] ];
		// adding the Mvd hits (Pixel and Strips)
		for(i=0; i< nMvdPixelHitsinTrack[ncand]; i++){
			auxR[i] =
			 XMvdPixel[ ListMvdPixelHitsinTrack[ncand][i] ]*
			 XMvdPixel[ ListMvdPixelHitsinTrack[ncand][i] ]+
			 YMvdPixel[ ListMvdPixelHitsinTrack[ncand][i] ]*
			 YMvdPixel[ ListMvdPixelHitsinTrack[ncand][i] ];
			tempmvdindex[i]=ListMvdPixelHitsinTrack[ncand][i];
			tempmvdtype[i]=0;
			auxIndex[i] = i;
		}
		for(i=0; i< nMvdStripHitsinTrack[ncand]; i++){
			auxR[i+nMvdPixelHitsinTrack[ncand]] =
			 XMvdStrip[ ListMvdStripHitsinTrack[ncand][i] ]*
			 XMvdStrip[ ListMvdStripHitsinTrack[ncand][i] ]+
			 YMvdStrip[ ListMvdStripHitsinTrack[ncand][i] ]*
			 YMvdStrip[ ListMvdStripHitsinTrack[ncand][i] ];
			tempmvdindex[i+nMvdPixelHitsinTrack[ncand]]=
				ListMvdStripHitsinTrack[ncand][i];
			tempmvdtype[i+nMvdPixelHitsinTrack[ncand]]=1;
			auxIndex[i+nMvdPixelHitsinTrack[ncand]]=
				i+nMvdPixelHitsinTrack[ncand];
		}

		//  ordering the Mvd Hits
		if( nMvdPixelHitsinTrack[ncand]+
		    nMvdStripHitsinTrack[ncand] >0){
			Merge_Sort( nMvdPixelHitsinTrack[ncand]+
		                    nMvdStripHitsinTrack[ncand],
				    auxR, auxIndex);

		//  constructing the first part of the ordered new Track  Candidate
			for(i=0; i< nMvdPixelHitsinTrack[ncand]+
					nMvdStripHitsinTrack[ncand]; i++){
				ListTrackCandHit[ncand][i] = tempmvdindex[ auxIndex[i] ];
				ListTrackCandHitType[ncand][i] = tempmvdtype[ auxIndex[i] ];

			}
		}	// end of  if( nMvdPixelHitsinTrack[ncand]+

		// construction of the second part of the ordered new Track  Candidate

		UShort_t tempmvdindex2[nSttParHitsinTrack[ncand]+
					nSttSkewHitsinTrack[ncand] ],
			 tempmvdtype2[nSttParHitsinTrack[ncand]+
					nSttSkewHitsinTrack[ncand] ];
		Int_t auxIndex2[nSttParHitsinTrack[ncand]+
					nSttSkewHitsinTrack[ncand] ];
		Double_t auxR2[nSttParHitsinTrack[ncand]+
					nSttSkewHitsinTrack[ncand] ];

		for(i=0; i<nSttParHitsinTrack[ncand]; i++){
			auxR2[i] =
			 info[ ListSttParHitsinTrack[ncand][i] ][0]*
			 info[ ListSttParHitsinTrack[ncand][i] ][0]+
			 info[ ListSttParHitsinTrack[ncand][i] ][1]*
			 info[ ListSttParHitsinTrack[ncand][i] ][1];
			tempmvdindex2[i]=ListSttParHitsinTrack[ncand][i];
			tempmvdtype2[i]=2;
			auxIndex2[i] = i;
		}
		for(i=0; i<nSttSkewHitsinTrack[ncand]; i++){
			j = i+nSttParHitsinTrack[ncand];
			auxR2[j] =
			 info[ ListSttSkewHitsinTrack[ncand][i] ][0]*
			 info[ ListSttSkewHitsinTrack[ncand][i] ][0]+
			 info[ ListSttSkewHitsinTrack[ncand][i] ][1]*
			 info[ ListSttSkewHitsinTrack[ncand][i] ][1];
			tempmvdindex2[j]=ListSttSkewHitsinTrack[ncand][i];
			tempmvdtype2[j]=3;
			auxIndex2[j] = j;
		}


		//  ordering the Stt Hits
		if( nSttParHitsinTrack[ncand]+nSttSkewHitsinTrack[ncand] >0){
			Merge_Sort( nSttParHitsinTrack[ncand]+nSttSkewHitsinTrack[ncand],
				    auxR2, auxIndex2);

			for(j=0,ipar=0,iskew=0;
				  j< nSttParHitsinTrack[ncand]+nSttSkewHitsinTrack[ncand];j++){
				i = j+nMvdPixelHitsinTrack[ncand]+
				    nMvdStripHitsinTrack[ncand];
				ListTrackCandHit[ncand][i] = tempmvdindex2[ auxIndex2[j] ];
				ListTrackCandHitType[ncand][i] = tempmvdtype2[ auxIndex2[j] ];

				if( ListTrackCandHitType[ncand][i]==2) {
				  ListSttParHitsinTrack[ncand][ipar]=tempmvdindex2[auxIndex2[j]];
				  ipar++;
				} else {
				  ListSttSkewHitsinTrack[ncand][iskew]=tempmvdindex2[auxIndex2[j]];
				  iskew++;
				}
			}
		}	// end of  if( nSttParHitsinTrack[ncand]+






	return;
}

//----------end of function PndSttMvdTracking::OrderingR_Loading_ListTrackCandHit


//----------begin of function PndSttMvdTracking::OrderingConformal_Loading_ListTrackCandHit
  void PndSttMvdTracking::OrderingConformal_Loading_ListTrackCandHit(
			bool *keepit,
			UShort_t ncand,
			Double_t info[][7],
			Double_t * Ox,
			Double_t * Oy,
			Double_t * Rr,
			Double_t Trajectory_Start[MAXTRACKSPEREVENT][2],
			Short_t *CHARGE,
			Double_t SchosenSkew[][nmaxSttHits]
				)
{

	UShort_t	i,
			j,
			ipar,
			iskew;


//     ordering all the hits belonging to the candidate track, by increasing R;
//     forming the new track with Mvd+Stt hits


	// arrays used to store temporarily the info of Mvd hits to be ordered.
	Int_t ListHits[nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack],
		ListHits2[nmaxSttHitsInTrack];
	Double_t XY[nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack][2],
		XY2[nmaxSttHitsInTrack][2];

//	for(ncand=FirstCandidate; ncand< LastCandidate; ncand++){

		if(!keepit[ncand]) return;
		nTrackCandHit[ncand] =nSttParHitsinTrack[ncand]+nSttSkewHitsinTrack[ncand]+
					nMvdPixelHitsinTrack[ncand]+
					nMvdStripHitsinTrack[ncand];
		// adding the Mvd hits (Pixel and Strips)

		if( nMvdPixelHitsinTrack[ncand]+
		    nMvdStripHitsinTrack[ncand] >0){
			for(i=0; i< nMvdPixelHitsinTrack[ncand]; i++){
			  XY[i][0] = XMvdPixel[ ListMvdPixelHitsinTrack[ncand][i] ];
			  XY[i][1] = YMvdPixel[ ListMvdPixelHitsinTrack[ncand][i] ];
			  ListHits[i] = ListMvdPixelHitsinTrack[ncand][i];
			}
			for(i=0; i< nMvdStripHitsinTrack[ncand]; i++){
			  XY[i+nMvdPixelHitsinTrack[ncand]][0] =
			    XMvdStrip[ ListMvdStripHitsinTrack[ncand][i] ];
			  XY[i+nMvdPixelHitsinTrack[ncand]][1] =
			    YMvdStrip[ ListMvdStripHitsinTrack[ncand][i] ];
			  // to distinguish between Pixels and Strips, add a number
			  // to the original Strip hit number.
			  ListHits[i+nMvdPixelHitsinTrack[ncand]] =
				ListMvdStripHitsinTrack[ncand][i]+
				(nmaxMvdPixelHits+nmaxMvdStripHits)*10 ;
			}

			//  ordering the Mvd Hits
			OrderingUsingConformal(
			   Ox[ncand],
			   Oy[ncand],
			   &Trajectory_Start[ncand][0],
			   nMvdPixelHitsinTrack[ncand]+nMvdStripHitsinTrack[ncand],
			   XY, // XY[*][0] = X position, XY[*][0] = Y position.
			   CHARGE[ncand],  // input
			   ListHits  // output
						);
			//  constructing the ordered new Track  Candidate now
			for(i=0; i< nMvdPixelHitsinTrack[ncand]+
					nMvdStripHitsinTrack[ncand]; i++){
				if(ListHits[i]<(nmaxMvdPixelHits+nmaxMvdStripHits)*10){//Pixel.
					ListTrackCandHit[ncand][i] = ListHits[i];
					ListTrackCandHitType[ncand][i] = 0;
				} else { // Strip hits.
					ListTrackCandHit[ncand][i] = ListHits[i]-
						(nmaxMvdPixelHits+nmaxMvdStripHits)*10;
					ListTrackCandHitType[ncand][i] = 1;
				}
			}  // end of  for(i=0; i< nMvdPixelHitsinTrack[ncand]+

		}	// end of  if( nMvdPixelHitsinTrack[ncand]+




		// construction of the second part of the ordered new Track  Candidate

		if( nSttParHitsinTrack[ncand]+nSttSkewHitsinTrack[ncand] >0){

			for(i=0; i<nSttParHitsinTrack[ncand]; i++){
			 XY2[i][0] = info[ ListSttParHitsinTrack[ncand][i] ][0];
			 XY2[i][1] = info[ ListSttParHitsinTrack[ncand][i] ][1];
			 ListHits2[i] = ListSttParHitsinTrack[ncand][i];


			}

			for(i=0; i<nSttSkewHitsinTrack[ncand]; i++){
			 j = i+nSttParHitsinTrack[ncand];
			 XY2[j][0] = Ox[ncand]+Rr[ncand]*
				cos(SchosenSkew[ncand][ListSttSkewHitsinTrack[ncand][i]]);
			 XY2[j][1] = Oy[ncand]+Rr[ncand]*
				sin(SchosenSkew[ncand][ListSttSkewHitsinTrack[ncand][i]]);
			 ListHits2[j] = ListSttSkewHitsinTrack[ncand][i]+
					nmaxSttHits*10; // in order to distinguish
							//  the Skew hits.
			}


		//  ordering the Stt Hits

			OrderingUsingConformal(
			 Ox[ncand],
			 Oy[ncand],
			 &Trajectory_Start[ncand][0],
			 nSttParHitsinTrack[ncand]+nSttSkewHitsinTrack[ncand],
			 XY2, // XY2[*][0] = X position, XY2[*][0] = Y position.
			 CHARGE[ncand],  // input
			 ListHits2  // output
					);





			for(j=0,ipar=0,iskew=0;
				j< nSttParHitsinTrack[ncand]+nSttSkewHitsinTrack[ncand];j++){
			  i = j+nMvdPixelHitsinTrack[ncand]+ nMvdStripHitsinTrack[ncand];
			  if(ListHits2[j]<nmaxSttHits*10){  // parallel Stt hit.
			    ListTrackCandHit[ncand][i] = ListHits2[j];
			    ListTrackCandHitType[ncand][i] = 2;
			    ListSttParHitsinTrack[ncand][ipar]=ListHits2[j];
			    ipar++;
			  } else {  // skew Stt hit.
			    ListTrackCandHit[ncand][i] = ListHits2[j]-nmaxSttHits*10;
			    ListTrackCandHitType[ncand][i] = 3;
			    ListSttSkewHitsinTrack[ncand][iskew]=ListHits2[j]-nmaxSttHits*10;
			    iskew++;
			  }  // end of  if(ListHits2[j]<nmaxSttHits*10)
			} // end of for(j=0,ipar=0,iskew=0; ....

		}	// end of  if( nSttParHitsinTrack[ncand]+


if(istampa>=3) for(int ica=0; ica<nMvdPixelHitsinTrack[ncand]+nMvdStripHitsinTrack[ncand]+
	nSttParHitsinTrack[ncand]+nSttSkewHitsinTrack[ncand]; ica++){
	cout<<"from PndSttMvdTracking, hit n. "<<ListTrackCandHit[ncand][ica]<<", hit type "
	<<ListTrackCandHitType[ncand][ica]<<endl;
}


//	} //   end of  for(ncand=FirstCandidate; ncand< LastCandidate; ncand++)


	return;
}

//----------end of function PndSttMvdTracking::OrderingConformal_Loading_ListTrackCandHit






//----------begin of function PndSttMvdTracking::OrderingUsingConformal

	void   PndSttMvdTracking::OrderingUsingConformal(
		Double_t oX,
		Double_t oY,
		Double_t Traj_Sta[2],
		Int_t nHits,
		Double_t XY[][2],
		Short_t  Charge,  // input
		Int_t *ListHits
							)
{




      UShort_t	i,j, 
		tmp[nHits];
      Double_t	aaa,
		bbb,
		ccc,
		b1,
		firstR2,
		lastR2,
		aux[nHits],
		U[nHits],
		V[nHits];



//  here there is the ordering of the hits, NOT under the assumption that the circumference
//  in XY goes through  Trajectory_Start.
//  Moreover, the code before is supposed to have selected trajectories in XY with (Ox,Oy)
//  farther from (0,0) by > 0.9 * RminStrawDetector/2 and consequently Ox and Oy are not both 0.
//  The scheme for the ordering of the hit is as follows :
//  1)  order hits by increasing U or V of the conformal mapping; see Gianluigi's Logbook page 283;
//  2)  find the charge of the track by checking if it is closest to the center in XY
//	the first or the last of the ordered hits.
//  3)  in case, invert the ordering of U, V and ListHits such that the first hits in the
//	list are always those closer to the Trajectory_Start.


//   ordering of the hits

	aaa = atan2( oY-Traj_Sta[1], oX-Traj_Sta[0]);  // atan2 defined between -PI and PI.

	// the following statement is necessary since for unknown reason the root interpreter
	// gives a weird error when using PI directly in the if statement below!!!!!!! I lost
	// 2 hours trying to figure this out!
	b1 = PI/4.;

	if((aaa>b1&&aaa<3.*b1) || (aaa>-3.*b1&&aaa<-b1)){//use U as ordering variable;
							//[case 1 or 3 Gianluigi's Logbook page 285].
		for (j = 0; j< nHits; j++){
			bbb = XY[j][0]-Traj_Sta[0];
			ccc = XY[j][1]-Traj_Sta[1];
			U[j]= bbb/(bbb*bbb+ccc*ccc);
		}
		Merge_Sort( nHits, U, ListHits);

		if((aaa>b1&&aaa<3.*b1)){  //  case #1;
			if( Charge == -1){
				// inverting the order of the hits.
				for(i=0;i<nHits;i++){
					tmp[i]=ListHits[nHits-1-i];
				}
				for(i=0;i<nHits;i++){
					ListHits[i]=tmp[i];
				}
			}
		} else{  //  case # 3.
			if(Charge == 1){
				// inverting the order of the hits.
				for(i=0;i<nHits;i++){
					tmp[i]=ListHits[nHits-1-i];
				}
				for(i=0;i<nHits;i++){
					ListHits[i]=tmp[i];
				}
			}// end of  if( Charge ==1)
		}// end of  if((aaa>b1&&aaa<3.*b1))

	} else { // use V as ordering variable [case 2 or 4 Gianluigi's Logbook page 285].
		for (j = 0; j< nHits; j++){
			bbb = XY[j][0]-Traj_Sta[0];
			ccc = XY[j][1]-Traj_Sta[1];
			V[j]= ccc/(bbb*bbb+ccc*ccc);
		}
		Merge_Sort( nHits, V, ListHits);

		if((aaa<=-3.*b1 || aaa>=3.*b1)){  //  case #2;
			if( Charge == -1){
				// inverting the order of the hits.
				for(i=0;i<nHits;i++){
					tmp[i]=ListHits[nHits-1-i];
				}
				for(i=0;i<nHits;i++){
					ListHits[i]=tmp[i];
				}
			}
		} else{  //  case # 4.
			if( Charge == 1){
				// inverting the order of the hits.
				for(i=0;i<nHits;i++){
					tmp[i]=ListHits[nHits-1-i];
				}
				for(i=0;i<nHits;i++){
					ListHits[i]=tmp[i];
				}
			}
		}

	} //  end of   if((aaa>b1&& ....



 return; 


}
//----------end of function PndSttMvdTracking::OrderingUsingConformal









//----------begin of function PndSttMvdTracking::TrackCleanup



  bool PndSttMvdTracking::TrackCleanup(
				Double_t GAP,
				Double_t Oxx,
				Double_t Oyy,
				Double_t Rr,
				Double_t KAPPA,
				Double_t FI0,
				Short_t  Charge,
				Double_t Start[3],
				UShort_t &nHitsPar,  // n. hits parall Stt
				UShort_t *ListHitsPar,
				UShort_t &nHitsSkew,  // n. hits parall Stt
				UShort_t *ListHitsSkew,
				Double_t *auxS,
				Double_t info[][7],
				Double_t RStrawDetMin,
				Double_t ApotemaMaxInnerPar,
				Double_t ApotemaMinSkew,
				Double_t ApotemaMaxSkew,
				Double_t ApotemaMinOuterPar,
				Double_t RStrawDetMax
						)
{
// this method does 3 things :
//
//	1)  finds the entrance and exit points in the STT parallel and skew volumes of the current track;
//	2)  eliminates from the track hit list possible spurious hits that are not encompassed
//		by the entrance and exit point;
//	3)  eliminates the tracks if the hit sequence is not continuous enough.


	bool ConsiderLastHit;

	Short_t flagInnerStt,
		flagOuterStt;

	UShort_t	ihit,
			nInnerHits,
			nOuterHits,
			nIntersections[2],
			ListInnerHits[nmaxSttHits],
			ListOuterHits[nmaxSttHits];

	Double_t	FiLimitAdmissible,
			r,
			Xcross[2],
			Ycross[2],
			XintersectionList[12][2], // second index =0 --> inner Hexagon, =1 --> outer.
			YintersectionList[12][2]; // first index : all the possible intersections
						  // (up to 12 intersections).

//------------------------
	// calculation of the Maximum FI angle possible (if it is a +ve charge) of the Minimum
	// for this track, taking into account
	// that the maximum possible Z of a hit is ZCENTER_STRAIGHT + SEMILENGTH_STRAIGHT; the minimum
	// Z of a hit is ZCENTER_STRAIGHT - SEMILENGTH_STRAIGHT.
	if(Charge<0){
		if( KAPPA>0.){
			FiLimitAdmissible = FI0 + KAPPA*(ZCENTER_STRAIGHT + SEMILENGTH_STRAIGHT) ;
		} else {
			FiLimitAdmissible = FI0 + KAPPA*(ZCENTER_STRAIGHT - SEMILENGTH_STRAIGHT) ;
		}
	} else {
		if( KAPPA>0.){
			FiLimitAdmissible = FI0 + KAPPA*(ZCENTER_STRAIGHT - SEMILENGTH_STRAIGHT) ;
		} else {
			FiLimitAdmissible = FI0 + KAPPA*(ZCENTER_STRAIGHT + SEMILENGTH_STRAIGHT) ;
		}
	}  // end of    if(Charge<0)
//-----------------------------------------------------------------------------------------------------

	// parallel cleanup.

//----------------stampe
if(istampa>=2){
cout<<" IVOLTE = "<<IVOLTE<<", prima di paral cleanup, nHitsPar "<<nHitsPar<<
", KAPPA = "<<KAPPA <<", charge "<<Charge<<", FI0 "<<FI0
<<"\n\tFiLimitAdmissible "<<
FiLimitAdmissible<<", X limit "<<Oxx+Rr*cos(FiLimitAdmissible)<<
", Y limit "<<Oyy+Rr*sin(FiLimitAdmissible)<<", Ox "<<Oxx<<", Oy "<<Oyy<<", R "<<Rr<<endl;
}
//-------------------- fine stampe

if(istampa>1) cout<<"\tentra in SttParalCleanup\n";

//	if(nHitsPar>0 && !SttParalCleanup(
	if(!SttParalCleanup(
				GAP,
				Oxx,
				Oyy,
				Rr,
				Charge,
				Start,
				FI0,
				FiLimitAdmissible,
				nHitsPar, // it doesn't get modify for now.
				ListHitsPar, // input only for now. 
				info,
				RStrawDetMin,
				ApotemaMaxInnerPar,
				ApotemaMinOuterPar,
				RStrawDetMax
						) ){
if(istampa>1) cout<<"uscito da SttParalCleanup : false\n";
						  return false;
				}
if(istampa>1) cout<<"uscito da : SttParalCleanup true\n";




//----------------------------------------------------------------------------

	// skew cleanup.
if(istampa>1) cout<<"\tentra in SttSkewCleanup\n";
	if ( ! (SttSkewCleanup(
			GAP,
			Oxx,
			Oyy,
			Rr,
			Charge,
			Start,  // strarting point of trajectory.
			FI0,
			FiLimitAdmissible,
			nHitsSkew, // it doesn't get modify for now.
			ListHitsSkew, // it doesn't get modify for now.
			auxS,
			info,
			ApotemaMinSkew,  //distance hexagon side from (0,0)
			ApotemaMaxSkew,  //distance hexagon side from (0,0)
			3., // cut distance
			1 // max number of failures allowed.
			) ) ) {

if(istampa>1) cout<<"uscito da SttSkewCleanup false\n";
						  return false;
				}

if(istampa>1) cout<<"uscito da SttSkewCleanup true\n";


	return true;
	
	

};



//----------end of function PndSttMvdTracking::TrackCleanup



//----------begin of function PndSttMvdTracking::SttParalCleanup



  bool PndSttMvdTracking::SttParalCleanup(
				Double_t GAP,
				Double_t Oxx,
				Double_t Oyy,
				Double_t Rr,
				Short_t  Charge,
				Double_t Start[3],
				Double_t FI0,
				Double_t FiLimitAdmissible,
				UShort_t nHits,
				UShort_t *Listofhits,
				Double_t info[][7],
				Double_t RStrawDetMin,
				Double_t ApotemaInnerParMax,
				Double_t ApotemaOuterParMin,
				Double_t RStrawDetMax
						)
{
// this method does 3 things :
//
//	1)  finds the entrance and exit points in the STT parallel volumes of the current track;
//	2)  eliminates from the track hit list possible spurious hits that are not encompassed
//		by the entrance and exit point;
//	3)  eliminates the tracks if the hit sequence is not continuous enough.


//	bool ConsiderLastHit;

	Short_t flagInnerSttR,
		flagOuterSttR,
		flagInnerSttL,
		flagOuterSttL,
		flagOutStt;

	UShort_t	enne,
			i,
			ihit,
			ipurged,
			islack,
			nintersections,
			nnn,
			nInnerHits,
			nInnerHitsLeft,
			nInnerHitsRight,
			nOuter,
			nOuterHits,
			nOuterHitsLeft,
			nOuterHitsRight,
			nIntersections[2],
			ListHits[nHits],
			ListInnerHits[nHits],
			ListInnerHitsLeft[nHits],
			ListInnerHitsRight[nHits],
			ListOuterHits[nHits],
			ListOuterHitsLeft[nHits],
			ListOuterHitsRight[nHits];

	Double_t	epsilonTheta,
			fi,
			r,
			aux[2],
			Xcross[2],
			Ycross[2],
			XcrossL[2],
			YcrossL[2],
			XcrossR[2],
			YcrossR[2],
			XcrossOut[2],
			YcrossOut[2],
			XintersectionList[7], // there is also the last boundary FiLimitAdmissible
			YintersectionList[7]; // take into account and the two possible
					      // intersections with the external circle.


	islack=1;  // uncertainty allowed in the # of straws that should be hit in a given part
			// of the Stt detector.


//------------------------
//  elimination of hits outside the physical FI range (FiLimitAdmissible) due to finite length of
//  Straws.


if(istampa>1) {
	cout<<"SttParalCleanup, evento n. "<<IVOLTE<<", n. || in ingresso "<<
	nHits<<", prima di essere purgati."<<endl; }

	epsilonTheta = STRAWRADIUS/Rr;  // some extra slac for being conservative.
	for(i=0, ipurged=0; i< nHits; i++){

		fi = atan2( info[Listofhits[i]][1]-Oyy,info[Listofhits[i]][0]-Oxx);
		if (fi<0.) fi+=2.*PI;

	  if(Charge <0) {
		if( fi > FI0){
			if( fi>FiLimitAdmissible+epsilonTheta) continue;
		} else {
			fi += 2.*PI;
			if( fi >FiLimitAdmissible+epsilonTheta ) continue; 
		}  // end of  if( fi > FI0)
	  } else {  // continuation of  if(Charge <0)
		if( fi > FI0){
			fi -= 2.*PI;
		}  // end of  if( fi > FI0)
		if (fi < FiLimitAdmissible-epsilonTheta) continue;
	  } // end of if(Charge <0)

	  ListHits[ipurged]=Listofhits[i];
	  ipurged++;
	}  // end of    for(i=0, ipurged=0; i< nHits; i++)

	nHits = ipurged;
if(istampa>1) {
	cout<<"SttParalCleanup, evento n. "<<IVOLTE<<", n. || hits "<<
	nHits<<", dopo di essere purgati."<<endl; }

	if(nHits==0){
		nInnerHits=0;
		nInnerHitsRight=0;
		nInnerHitsLeft=0;
		nOuterHits=0;
		nOuterHitsRight=0;
		nOuterHitsLeft=0;
		goto jumpa ; // don't discard track yet, see if it should have parallel hits.
	}

//------------------
//   separation of inner Parallel Stt hits from outer Parallel Stt hits.

	SeparateInnerOuterParallel(

				// input
				nHits,
				ListHits,
				info,
				2.*ApotemaInnerParMax/sqrt(3.),

				// output
				&nInnerHits,
				ListInnerHits,
				&nOuterHits,
				ListOuterHits,

				&nInnerHitsLeft,
				ListInnerHitsLeft,
				&nInnerHitsRight,
				ListInnerHitsRight,

				&nOuterHitsLeft,
				ListOuterHitsLeft,
				&nOuterHitsRight,
				ListOuterHitsRight
				);

if(istampa>1) {
	cout<<"SttParalCleanup, evento n. "<<IVOLTE
	<<", dopo separate INNER/OUTER, n. || inner hits left "<<
	nInnerHitsLeft<<", right : "<< nInnerHitsRight <<endl; }
//------------------------------------------
	// find the entrance and exit of the track in the Inner Left Parallel Straw region.
	// This region is bounded by two Hexagons, and it has the target gap in the middle.

	// flag meaning :
	// -1 -->  track outside outer perimeter;
	// 0 -->  at least 1 intersection with polygon, therefore a possible entry and an exit;
	// 1 -->  track contained completely between the two polygons;

jumpa: ;

//	first of all, find possible intersection points with outer circle encompassing
//	the Stt system.

	flagOutStt = FindIntersectionsOuterCircle(
				Oxx,
				Oyy,
				Rr,
				RStrawDetectorMax,
				XcrossOut,
				YcrossOut
				);


//	intersection with Inner Section.

	flagInnerSttL=FindTrackEntranceExitbiHexagonLeft(
				GAP,
				Oxx,
				Oyy,
				Rr,
				Charge,
				Start,
				RStrawDetMin,
				ApotemaInnerParMax,
				XcrossL,
				YcrossL
				);
	// find the entrance and exit of the track in the Inner Right Parallel Straw region.
	// This region is bounded by two Hexagons, and it has the target gap in the middle.

	flagInnerSttR=FindTrackEntranceExitbiHexagonRight(
				GAP,
				Oxx,
				Oyy,
				Rr,
				Charge,
				Start,
				RStrawDetMin,
				ApotemaInnerParMax,
				XcrossR,
				YcrossR
				);



if(istampa>1) {
	cout<<"SttParalCleanup, evento n. "<<IVOLTE<<", flagInnerSttR (-1,0,1) = "<<flagInnerSttR
	<<", flagInnerSttL "<<flagInnerSttL<<", flagOutStt "<<flagOutStt<<
	", FI0  "<<FI0<<",  FiLimitAdmissible "<<FiLimitAdmissible<<endl; }

//-----------------

//	working in the hypothesis that his is a track coming from Vertex at (0,0).

	// case when track is contained either in Left of Right Inner Stt Parallel sections.
	if( flagInnerSttL == 1 || flagInnerSttR == 1 ){
		return false;
	}

	// if a track enters only marginally in the skew volumes, define the track
	// as non-entering and the corresponding flag to -1.

	if( flagInnerSttR == 0 && (XcrossR[0]-XcrossR[1])*(XcrossR[0]-XcrossR[1])+
			(YcrossR[0]-YcrossR[1])*(YcrossR[0]-YcrossR[1])
			< 9.*STRAWRADIUS*STRAWRADIUS ) flagInnerSttR=-1;

	if( flagInnerSttL == 0 && (XcrossL[0]-XcrossL[1])*(XcrossL[0]-XcrossL[1])+
			(YcrossL[0]-YcrossL[1])*(YcrossL[0]-YcrossL[1])
			< 9.*STRAWRADIUS*STRAWRADIUS ) flagInnerSttR=-1;

	// case when track is outside both Inner Stt Parallel sections.
	if( flagInnerSttL == -1 && flagInnerSttR == -1 ){
		//nInnerHits=0; // eliminate all the hits from hit list.
		//nInnerHitsLeft=0;
		//nInnerHitsRight=0;
		goto outer ;
	}



	// case when the track crosses both InnerLeft and InnerRight.
	// Decide what was crossed first and ignore the other part.
	// This may be changed in the future.
	if( flagInnerSttL == 0 && flagInnerSttR == 0 ) {
		XintersectionList[0]=XcrossL[0];
		YintersectionList[0]=YcrossL[0];
		XintersectionList[1]=XcrossL[1];
		YintersectionList[1]=YcrossL[1];
		XintersectionList[2]=XcrossR[0];
		YintersectionList[2]=YcrossR[0];
		XintersectionList[3]=XcrossR[1];
		YintersectionList[3]=YcrossR[1];
		nintersections=4;
		ChooseEntranceExitbis(
			Oxx,
			Oyy,
			Charge,
			FI0,
			nintersections,// n. intersection in input.
			XintersectionList,
			YintersectionList,
			Xcross,	// output
			Ycross	// output
				);
		// now decide which sector was crossed first.
		if( (fabs(XcrossL[0]-Xcross[0])<1.e-5&&fabs(YcrossL[0]-Ycross[0])<1.e-5)
					||
		    (fabs(XcrossL[1]-Xcross[0])<1.e-5&&fabs(YcrossL[1]-Ycross[0])<1.e-5)
		   ) {  // the Left part was entered first.
		   flagInnerSttR=-1;
		} else {  // the Right part was entered first.
		   flagInnerSttL=-1;
		} // end of  if( (fabs(XcrossL[0]-Xcross.....
	}  // end of if( (flagInnerSttL == 0 && flagInnerSttR = 0 )



//---------  the other 2 possible cases.

	if( flagInnerSttL == 0){
	   nnn=nInnerHitsLeft;
	   for(i=0;i<2;i++){
		XintersectionList[i]=XcrossL[i];
		YintersectionList[i]=YcrossL[i];
	   }
	} else {  // continuation of if( (flagInnerSttL == 0), case in which
		  // flagInnerSttR == 0.
	   nnn=nInnerHitsRight;
	   for(i=0;i<2;i++){
		XintersectionList[i]=XcrossR[i];
		YintersectionList[i]=YcrossR[i];
	   }
	}  // end of   if( (flagInnerSttL == 0)



	nintersections=2;
	if(fabs(FiLimitAdmissible-FI0) < 2.*PI){  // in this case the point
			// corresponding to FiLimitAdmissible can play a role in the
			// determination of the limiting points of the hits.
			aux[0]=Oxx+Rr*cos(FiLimitAdmissible);
			aux[1]=Oyy+Rr*sin(FiLimitAdmissible);
			XintersectionList[2]=aux[0];
			YintersectionList[2]=aux[1];
			nintersections++;
	}  // end of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)

	if( flagOutStt ==0){// 2 intersections with outer Stt circle.
				XintersectionList[nintersections]=XcrossOut[0];
				XintersectionList[nintersections+1]=XcrossOut[1];
				YintersectionList[nintersections]=YcrossOut[0];
				YintersectionList[nintersections+1]=YcrossOut[1];
				nintersections +=2;
	}


	ChooseEntranceExitbis(
				Oxx,
				Oyy,
				Charge,
				FI0,
				nintersections,// n. intersection in input.
				XintersectionList,
				YintersectionList,
				Xcross,	// output
				Ycross	// output
				);

	if(fabs(FiLimitAdmissible-FI0) < 2.*PI){
			// case when this track exit in Z before having the possibility
			// of hitting the Stt parallel inner section.
		if( fabs(aux[0]-Xcross[0])<1.e-5&& fabs(aux[1]-Ycross[0])<1.e-5 ){
			return true;
		}
		if( flagOutStt ==0){// 2 intersections with outer Stt circle.
		   // case when this track exits the Stt outer circle without
		   // hitting the Stt parallel inner section (for instance the track
		   if( (fabs(XcrossOut[0]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[0]-Ycross[0])<1.e-5 )
					||
				(fabs(XcrossOut[1]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[1]-Ycross[0])<1.e-5 )
				){
				goto outer ;
		   }
		} // end of  if( flagOutStt ==0)

		// most usual case when track crossed the Inner parallel Stt.
		if (nnn == 0) return false;

		// if the exit point is actually given by FiLimitAdmissible, then allow
		// an extra uncertainty in the # Stt hit that must be present;
		// this is done because FiLimitAdmissible is not a very precise number.
		if( fabs(aux[0]-Xcross[1])<1.e-5&& fabs(aux[1]-Ycross[1])<1.e-5 ){
			islack = 3;
		}

	} else { // continuation of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)
		if( flagOutStt ==0){// 2 intersections with outer Stt circle.
			   // case when this track exits the Stt outer circle without
			   // hitting the Stt parallel inner section (for instance the track
			   if( (fabs(XcrossOut[0]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[0]-Ycross[0])<1.e-5 )
					||
				(fabs(XcrossOut[1]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[1]-Ycross[0])<1.e-5 )
				){
				//nInnerHits=0; // eliminate all the hits from hit list.
				//nInnerHitsRight=0;
				//nInnerHitsLeft=0;
				goto outer ;
			   }
		} // end of  if( flagOutStt ==0)

		// most usual case when track crossed the Inner parallel Stt.
		if (nnn == 0) return false;

	}  // end of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)





//-------------  cleanup of the spurious tracks first using the inner parallel straws.

if(istampa>1) {
	cout<<"SttParalCleanup, evento n. "<<IVOLTE<<", prima di BadTrack_ParStt; Xin Inner "<<
	Xcross[0]<<", Yin Inner "<<Ycross[0]<<
	",  Xout Inner "<<Xcross[1]<<", Yout Inner "<<Ycross[1]<<endl; }


	// at this point the n. of inner hits cannot be 0 for a true track.
	if ( BadTrack_ParStt(
			Oxx,
			Oyy,
			Rr,
			Charge,
			Xcross,  // Xcross[0]=point of entrance; Xcross[1]=point of exit.
			Ycross,
			nInnerHits,
			ListInnerHits,
			info,
			2.*DiameterStrawTube,	//  cut of proximity between hits.
			1,	// maximum allowed # consecutive hits with distance > cut.
			islack // uncertainty allowed as far as the n. of hits that should be present.
					)
	   ){

		return false;
	}

if(istampa>1) cout<<"uscito da BadTrack_ParStt.\n";

//-----------------------------------------------------

outer: ;

	islack=1;   // reset the extra uncertainty in the # Stt.




//------------ Outer Parallel Stt hits section.
	// find the entrance and exit of the track in the Outer Parallel Straw region, Left side.
	// This region is bounded by a Hexagon (inner), a Circle (outer) and it has
	// the target gap in the middle.

	//  Returns -1 if there are 0 or 1 intersections, 0 if there are at least 2 intersections.
		flagOuterSttL=FindTrackEntranceExitHexagonCircleLeft(
				Oxx,
				Oyy,
				Rr,
				Charge,
				Start,
				ApotemaMinOuterParStraw,
				RStrawDetMax,
				GAP,
				XcrossL,
				YcrossL
				);



//------------
	// find the entrance and exit of the track in the Outer Parallel Straw region, Right side.
	// This region is bounded by a Hexagon (inner), a Circle (outer) and it has
	// the target gap in the middle.
		flagOuterSttR=FindTrackEntranceExitHexagonCircleRight(
				Oxx,
				Oyy,
				Rr,
				Charge,
				Start,
				ApotemaMinOuterParStraw,
				RStrawDetMax,
				GAP,
				XcrossR,
				YcrossR
				);


if(istampa>1) {
	cout<<"SttParalCleanup, evento n. "<<IVOLTE<<", flagOuterSttR (-1,0,1) = "<<flagOuterSttR
	<<", flagOuterSttL "<<flagOuterSttL<<endl; }


//--------------------------------

	// case when track is contained either in Left of Right Outer Stt Parallel sections.
	if( flagOuterSttL == 1 || flagOuterSttR == 1 ){
		return false;
	}


	// if a track enters only marginally in the skew volumes, define the track
	// as non-entering and the corresponding flag to -1.

	if( flagInnerSttR == 0 && (XcrossR[0]-XcrossR[1])*(XcrossR[0]-XcrossR[1])+
			(YcrossR[0]-YcrossR[1])*(YcrossR[0]-YcrossR[1])
			< 9.*STRAWRADIUS*STRAWRADIUS ) flagInnerSttR=-1;

	if( flagInnerSttL == 0 && (XcrossL[0]-XcrossL[1])*(XcrossL[0]-XcrossL[1])+
			(YcrossL[0]-YcrossL[1])*(YcrossL[0]-YcrossL[1])
			< 9.*STRAWRADIUS*STRAWRADIUS ) flagInnerSttR=-1;


	// case when track is outside both Outer Stt Parallel sections.
	if( flagOuterSttL == -1 && flagOuterSttR == -1 ){
		return true ;
	}


	// case when the track crosses both OuterLeft and OuterRight.
	// Decide what was crossed first and ignore the other part.
	// This may be changed in the future.
	if( flagOuterSttL == 0 && flagOuterSttR == 0 ) {
		XintersectionList[0]=XcrossL[0];
		YintersectionList[0]=YcrossL[0];
		XintersectionList[1]=XcrossL[1];
		YintersectionList[1]=YcrossL[1];
		XintersectionList[2]=XcrossR[0];
		YintersectionList[2]=YcrossR[0];
		XintersectionList[3]=XcrossR[1];
		YintersectionList[3]=YcrossR[1];
		nintersections=4;
		ChooseEntranceExitbis(
			Oxx,
			Oyy,
			Charge,
			FI0,
			nintersections,// n. intersection in input.
			XintersectionList,
			YintersectionList,
			Xcross,	// output
			Ycross	// output
				);
		// now decide which sector was crossed first.
		if( (fabs(XcrossL[0]-Xcross[0])<1.e-5&&fabs(YcrossL[0]-Ycross[0])<1.e-5)
					||
		    (fabs(XcrossL[1]-Xcross[0])<1.e-5&&fabs(YcrossL[1]-Ycross[0])<1.e-5)
		   ) {  // the Left part was entered first.
		   flagOuterSttR=-1;
		} else {  // the Right part was entered first.
		   flagOuterSttL=-1;
		} // end of  if( (fabs(XcrossL[0]-Xcross.....
	}  // end of if( (flagInnerSttL == 0 && flagInnerSttR = 0 )

//-------------stampe.
if(istampa>1) {
cout<<"in SttParalCleanup Outer, caso traccia entra in L and R outer. Dopo scelta, flagOuterSttR "
	<<flagOuterSttR<<", flagOuterSttL "<<flagOuterSttL<<endl;
}
//-------------fine stampe.

	if( flagOuterSttL == 0){
	   nnn=nOuterHitsLeft;
	   for(i=0;i<2;i++){
		XintersectionList[i]=XcrossL[i];
		YintersectionList[i]=YcrossL[i];
	   }
	} else {  // continuation of if( (flagOuterSttL == 0), case in which
		  // flagOuterSttR == 0.
	   nnn=nOuterHitsRight;
	   for(i=0;i<2;i++){
		XintersectionList[i]=XcrossR[i];
		YintersectionList[i]=YcrossR[i];
	   }
	}  // end of   if( (flagOuterSttL == 0)


//-------------stampe.
if(istampa>1) {
cout<<"in SttParalCleanup Outer, nhit considerati "<< nnn <<endl;
}
//-------------fine stampe.


	nintersections=2;
	if(fabs(FiLimitAdmissible-FI0) < 2.*PI){  // in this case the point
			// corresponding to FiLimitAdmissible can play a role in the
			// determination of the limiting points of the hits.
			XintersectionList[2]=aux[0];
			YintersectionList[2]=aux[1];
			nintersections++;
//-------------stampe.
if(istampa>1) {
cout<<"in SttParalCleanup Outer, caso in cui  FiLimitAdmissible = "<< FiLimitAdmissible
<<"  conta!" <<endl;
}
//-------------fine stampe.
	}  // end of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)

	if( flagOutStt ==0){// 2 intersections with outer Stt circle.
				XintersectionList[nintersections]=XcrossOut[0];
				XintersectionList[nintersections+1]=XcrossOut[1];
				YintersectionList[nintersections]=YcrossOut[0];
				YintersectionList[nintersections+1]=YcrossOut[1];
				nintersections +=2;
	}
//-------------stampe.
if(istampa>1) {
cout<<"in SttParalCleanup Outer, prima di  ChooseEntranceExitbis, nintersections "
<< nintersections<<" e loro lista :"<<endl;
	for(int ic=0;ic<nintersections;ic++){
		cout<<"\tX["<<ic<<"] = "<<XintersectionList[ic]
		<<", Y["<<ic<<"] = "<<YintersectionList[ic]<<endl;
	}
}
//-------------fine stampe.

	ChooseEntranceExitbis(
				Oxx,
				Oyy,
				Charge,
				FI0,
				nintersections,// n. intersection in input.
				XintersectionList,
				YintersectionList,
				Xcross,	// output
				Ycross	// output
				);
//-------------stampe.
if(istampa>1) {
cout<<"in SttParalCleanup Outer, dopo di  ChooseEntranceExitbis, Xin"
<< Xcross[0]<<", Yin "<< Ycross[0]<<", Xout " << Xcross[1]<<", Yout "<< Ycross[1]
<<endl;
}
//-------------fine stampe.


	if(fabs(FiLimitAdmissible-FI0) < 2.*PI){
			// case when this track exit in Z before having the possibility
			// of hitting the Stt parallel inner section.

			// case when this track exit in Z before having the possibility
			// of hitting the Stt parallel inner section.
		if( fabs(aux[0]-Xcross[0])<1.e-5&& fabs(aux[1]-Ycross[0])<1.e-5 ){
			return true;
		}

		if( flagOutStt ==0){// 2 intersections with outer Stt circle.
		   // case when this track exits the Stt outer circle without
		   // hitting the Stt parallel Outer section (for instance the track
		   if( (fabs(XcrossOut[0]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[0]-Ycross[0])<1.e-5 )
					||
				(fabs(XcrossOut[1]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[1]-Ycross[0])<1.e-5 )
				){
				//nOuterHits=0; // eliminate all the hits from hit list.
				//nOuterHitsRight=0;
				//nOuterHitsLeft=0;
				return true ;
		   }
		} // end of  if( flagOutStt ==0)


		// most usual case when track crossed the Outer parallel Stt.
		if (nnn == 0) return false;

		// if the exit point is actually given by FiLimitAdmissible, then allow
		// an extra uncertainty in the # Stt hit that must be present;
		// this is done because FiLimitAdmissible is not a very precise number.
		if( fabs(aux[0]-Xcross[1])<1.e-5&& fabs(aux[1]-Ycross[1])<1.e-5 ){
			islack = 3;
		}

	} else { // continuation of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)

		if( flagOutStt ==0){// 2 intersections with outer Stt circle.
			   // case when this track exits the Stt outer circle without
			   // hitting the Stt parallel Outer section (for instance the track
			   if( (fabs(XcrossOut[0]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[0]-Ycross[0])<1.e-5 )
					||
				(fabs(XcrossOut[1]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[1]-Ycross[0])<1.e-5 )
				){
				//nOuterHits=0; // eliminate all the hits from hit list.
				//nOuterHitsRight=0;
				//nOuterHitsLeft=0;
				return true ;
			   }
		} // end of  if( flagOutStt ==0)


		// most usual case when track crossed the Outer parallel Stt.
		if (nnn == 0) return false;

	}  // end of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)




//-------------------- stampe
if(istampa>=2&&IVOLTE<20){
cout<<"SttParalCleanup, OUTER, caso R || L true, IVOLTE = "<<IVOLTE<<"\n\t Xcross[0] "
<< Xcross[0]<<", Ycross[0] " <<Ycross[0]<<"\n\t Xcross[1] "
<< Xcross[1]<<", Ycross[1] " <<Ycross[1]<<" e charge = "<<Charge<<", FiLimitAdmissible "
<<FiLimitAdmissible<<" (X="<<Oxx+Rr*cos(FiLimitAdmissible)
  <<", Y="<< Oyy+Rr*sin(FiLimitAdmissible)<<")." <<endl;
}
//-------------------fine stampe



//  cleanup of the spurious tracks now using the outer parallel straws.


	// at this point nOuterHits cannot be 0 for a real track.
	if ( BadTrack_ParStt(
			Oxx,
			Oyy,
			Rr,
			Charge,
			Xcross,  // Xcross[0]=point of entrance; Xcross[1]=point of exit.
			Ycross,
			nOuterHits,
			ListOuterHits,
			info,
			2.*DiameterStrawTube,	//  cut of proximity between hits.
			1,	// maximum allowed # consecutive hits with distance > cut.
			islack // uncertainty allowed as far as the n. of hits that should be present.
					)
	   ) return false;

//      }  // end of  if(nOuterHits==0)

//----------------------------------------------------------------------------

// finito: ;

	// if the code comes here it means that the track is acceptable.

//	nHits = nOuterHits+nInnerHits;

	return true;

};



//----------end of function PndSttMvdTracking::SttParalCleanup







//----------begin of function PndSttMvdTracking::SttSkewCleanup



  bool PndSttMvdTracking::SttSkewCleanup(
			Double_t GAP,
			Double_t Oxx,
			Double_t Oyy,
			Double_t Rr,
			Short_t  Charge,
			Double_t Start[3],
			Double_t FI0,
			Double_t FiLimitAdmissible,
			UShort_t nHits,
			UShort_t *Listofhits,
			Double_t *S,
			Double_t info[][7],
			Double_t RminStrawSkew,
			Double_t RmaxStrawSkew,
			Double_t cut, // cut distance (in cm).
			UShort_t maxnum // max number allowed of failures to pass the cut.
						)
{

	bool ConsiderLastHit;

	Short_t flagSttL,
		flagSttR,
		flagOutStt;

	UShort_t	i,
			ipurged,
			ibad,
			islack,
			nHitsLeft,
			nHitsRight,
			nintersections,
			ninside,
			nnn,
			nIntersections[2],
			ListHits[nHits],
			ListHitsRight[nHits],
			ListHitsLeft[nHits];

	Double_t	cut2,
			epsilonTheta,
			fi,
			FiStart,
			length,
			r,
			Sprevious,
			aux[2],
			Distance[nmaxSttHits+1],
			Xcross[2],
			Ycross[2],
			XcrossL[2],
			YcrossL[2],
			XcrossR[2],
			YcrossR[2],
			XcrossOut[2],
			YcrossOut[2],
			XintersectionList[5], // second index =0 --> inner Hexagon, =1 --> outer.
			YintersectionList[5]; // first index : all the possible intersections
						  // (up to 12 intersections).


	cut2=cut*cut;
	islack=1;// uncertainty allowed as far as
		// the n. of hits that should be present in a given section of the Stt track.


//------------------------
//  elimination of hits outside the physical FI range (FiLimitAdmissible) due to finite length of
//  Straws.

	epsilonTheta = STRAWRADIUS/Rr;  // some extra slac for being conservative.

if(istampa>1&&IVOLTE<20)
cout<<"\n\nevt "<<IVOLTE<<", FI0 "<<FI0<<", Filimit "
<< FiLimitAdmissible+epsilonTheta <<", Ox "<<Oxx<<", Oy "<<Oyy<<", R "<<Rr<<endl;

	for(i=0, ipurged=0; i< nHits; i++){
	  fi = S[i];

if(istampa>1&&IVOLTE<20)cout<<"\thit // n. "<<Listofhits[i]<<", fi "<<fi<<endl;

	  if(Charge <0) {
		if(fi > FI0){
			if( fi>FiLimitAdmissible+epsilonTheta) continue;
		} else {
			fi += 2.*PI;
			if( fi > FiLimitAdmissible+epsilonTheta ) continue; 
		}  // end of  if( fi > FI0)
	  } else {  // continuation of  if(Charge <0)
		if( fi > FI0){
			fi -= 2.*PI;
		}  // end of  if( fi > FI0)
		if (fi < FiLimitAdmissible-epsilonTheta) continue;
	  } // end of if(Charge <0)
if(istampa>1&&IVOLTE<20)cout<<"in SttSkewCleanup : hit preso!"<<endl;

	  ListHits[ipurged]=Listofhits[i];
	  S[ipurged]=S[i];
	  ipurged++;
	}  // end of    for(i=0, ipurged=0; i< nHits; i++)

if(istampa>1&&IVOLTE<20)cout<<"in SttSkewCleanup : hit skew prima di purga = "
<<nHits<<", dopo purga "<<ipurged<<endl;

	nHits = ipurged;
	if(nHits==0){ // don't discard track yet, see if it should have
				// skew hits or not.
		nHitsRight=nHitsLeft=0;
		goto jampa;
	}



	// separation of Right and Left Skew hits.

	nHitsRight=nHitsLeft=0;
	for(i=0;i< nHits; i++){
		if(info[ListHits[i]][0]<0.){
			ListHitsLeft[nHitsLeft]=ListHits[i];
			nHitsLeft++;
		}else{
			ListHitsRight[nHitsRight]=ListHits[i];
			nHitsRight++;
		}
	}

if(istampa>1&&IVOLTE<20)cout<<"in SttSkewCleanup : n. hit skew Left = "
<<nHitsLeft<<", right "<< nHitsRight  <<endl;

jampa: ;
//	first of all, find possible intersection points with outer circle encompassing
//	the Stt system.

	flagOutStt = FindIntersectionsOuterCircle(
				Oxx,
				Oyy,
				Rr,
				RStrawDetectorMax,
				XcrossOut,
				YcrossOut
				);

//------------------------------------------
	// find the entrance and exit of the track in the Skew Straw region.
	// This region is bounded by two Hexagons, and it has the target gap
	// in the middle. So Left is the left looking from downstream.

	flagSttL=FindTrackEntranceExitbiHexagonLeft(
				GAP,
				Oxx,
				Oyy,
				Rr,
				Charge,
				Start,
		ApotemaMinSkewStraw,
		ApotemaMaxSkewStraw, // Apotema is the distance of a Hexagonal side from (0,0)
				XcrossL,
				YcrossL
				);


	flagSttR=FindTrackEntranceExitbiHexagonRight(
				GAP,
				Oxx,
				Oyy,
				Rr,
				Charge,
				Start,
		ApotemaMinSkewStraw,
		ApotemaMaxSkewStraw, // Apotema is the distance of a Hexagonal side from (0,0)
				XcrossR,
				YcrossR
				);

	// find the entrance and exit of the track in the Skew Straw region.
	// This region is bounded by two Hexagons, and it has the target gap in the middle.

if(istampa>1)cout<<"in SttSkewCleanup : flagLeft (-1,0,1) = "<<flagSttL
<<", right "<< flagSttR  <<endl;
	if (flagSttR == 1 || flagSttL == 1 ) { // the trajectory is contained completely
					// in  the Right or Left Skew section, reject!
		return false ;
	}

	// if a track enters only marginally in the skew volumes, define the track
	// as non-entering and the corresponding flag to -1.

	if( flagSttR == 0 && (XcrossR[0]-XcrossR[1])*(XcrossR[0]-XcrossR[1])+
			(YcrossR[0]-YcrossR[1])*(YcrossR[0]-YcrossR[1])
			< 16.*STRAWRADIUS*STRAWRADIUS ){
		flagSttR=-1;
if(istampa>1)cout<<"in SttSkewCleanup : distanza entrata-uscita<4*STRAWRADIUS,flagSttR set at -1!\n";
	}

	if( flagSttL == 0 && (XcrossL[0]-XcrossL[1])*(XcrossL[0]-XcrossL[1])+
			(YcrossL[0]-YcrossL[1])*(YcrossL[0]-YcrossL[1])
			< 16.*STRAWRADIUS*STRAWRADIUS ){
		flagSttR=-1;
if(istampa>1)cout<<"in SttSkewCleanup : distanza entrata-uscita<4*STRAWRADIUS,flagSttL set at -1!\n";
	}

	if (flagSttR != 0 && flagSttL != 0 ) {
		//nHits=0;
		if(istampa>1)cout<<"in SttSkewCleanup : flagSttR = "<<flagSttR
		<<", e  flagSttL = "<<flagSttL<<", exit con true!\n";
		return true; // don't discard track because it may have Mvd hits anyway
				// and/or they can have Inner Parallel hits.
	}


	// case when the track crosses both SkewLeft and SkewRight.
	// Decide what was crossed first and ignore the other part.
	// This may be changed in the future.



	if( flagSttL == 0 && flagSttR == 0 ) { // crosses both right and left sections.
		XintersectionList[0]=XcrossL[0];
		YintersectionList[0]=YcrossL[0];
		XintersectionList[1]=XcrossL[1];
		YintersectionList[1]=YcrossL[1];
		XintersectionList[2]=XcrossR[0];
		YintersectionList[2]=YcrossR[0];
		XintersectionList[3]=XcrossR[1];
		YintersectionList[3]=YcrossR[1];
		nintersections=4;
		ChooseEntranceExitbis(
			Oxx,
			Oyy,
			Charge,
			FI0,
			nintersections,// n. intersection in input.
			XintersectionList,
			YintersectionList,
			Xcross,	// output
			Ycross	// output
				);
		// now decide which sector was crossed first.
		if( (fabs(XcrossL[0]-Xcross[0])<1.e-5&&fabs(YcrossL[0]-Ycross[0])<1.e-5)
					||
		    (fabs(XcrossL[1]-Xcross[0])<1.e-5&&fabs(YcrossL[1]-Ycross[0])<1.e-5)
		   ) {  // the Left part was entered first.
		   flagSttR=-1;
		} else {  // the Right part was entered first.
		   flagSttL=-1;
		} // end of  if( (fabs(XcrossL[0]-Xcross.....
	}  // end of if( (flagInnerSttL == 0 && flagInnerSttR = 0 )



//---------  the other 2 possible cases.



	if( flagSttL == 0){
	   nnn=nHitsLeft;
	   for(i=0;i<2;i++){
		XintersectionList[i]=XcrossL[i];
		YintersectionList[i]=YcrossL[i];
	   }
	} else {  // continuation of if( (flagSttL == 0), case in which
		  // flagSttR == 0.
	   nnn=nHitsRight;
	   for(i=0;i<2;i++){
		XintersectionList[i]=XcrossR[i];
		YintersectionList[i]=YcrossR[i];
	   }
	}  // end of   if( (flagInnerSttL == 0)

	nintersections=2;
	if(fabs(FiLimitAdmissible-FI0) < 2.*PI){  // in this case the point
			// corresponding to FiLimitAdmissible can play a role in the
			// determination of the limiting points of the hits.
			aux[0]=Oxx+Rr*cos(FiLimitAdmissible);
			aux[1]=Oyy+Rr*sin(FiLimitAdmissible);
			XintersectionList[2]=aux[0];
			YintersectionList[2]=aux[1];
			nintersections++;
	}  // end of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)


	if( flagOutStt ==0){// 2 intersections with outer Stt circle.
				XintersectionList[nintersections]=XcrossOut[0];
				XintersectionList[nintersections+1]=XcrossOut[1];
				YintersectionList[nintersections]=YcrossOut[0];
				YintersectionList[nintersections+1]=YcrossOut[1];
				nintersections +=2;
	}

	ChooseEntranceExitbis(
				Oxx,
				Oyy,
				Charge,
				FI0,
				nintersections,// n. intersection in input.
				XintersectionList,
				YintersectionList,
				Xcross,	// output
				Ycross	// output
				);

	if(fabs(FiLimitAdmissible-FI0) < 2.*PI){
			// case when this track exit in Z before having the possibility
			// of hitting the Stt parallel inner section.
		if( fabs(aux[0]-Xcross[0])<1.e-5&& fabs(aux[1]-Ycross[0])<1.e-5 ){
			return true;
		}
		if( flagOutStt ==0){// 2 intersections with outer Stt circle.
		   // case when this track exits the Stt outer circle without
		   // hitting the Stt parallel inner section (for instance the track
		   if( (fabs(XcrossOut[0]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[0]-Ycross[0])<1.e-5 )
					||
				(fabs(XcrossOut[1]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[1]-Ycross[0])<1.e-5 )
				){
				return true ;
		   }
		} // end of  if( flagOutStt ==0)

		// most usual case when track crossed the Inner parallel Stt.
		if (nnn == 0) return false;

		// if the exit point is actually given by FiLimitAdmissible, then allow
		// an extra uncertainty in the # Stt hit that must be present;
		// this is done because FiLimitAdmissible is not a very precise number.
		if( fabs(aux[0]-Xcross[1])<1.e-5&& fabs(aux[1]-Ycross[1])<1.e-5 ){
			islack = 3;
		}


	} else { // continuation of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)
		if( flagOutStt ==0){// 2 intersections with outer Stt circle.
			   // case when this track exits the Stt outer circle without
			   // hitting the Stt parallel inner section (for instance the track
			   if( (fabs(XcrossOut[0]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[0]-Ycross[0])<1.e-5 )
					||
				(fabs(XcrossOut[1]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[1]-Ycross[0])<1.e-5 )
				){
				return true ;
			   }
		} // end of  if( flagOutStt ==0)

		// most usual case when track crossed the Skew Stt.
		if (nnn == 0) return false;

	}  // end of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)



	length=CalculateArcLength(Oxx,
				Oyy,
				Rr,
				Charge,
				Xcross,
				Ycross
				);
//-------------------- stampe
if(istampa>=2&&IVOLTE<20){
cout<<"in SttSkewCleanup,  IVOLTE = "<<IVOLTE<<"\n\t Xcross[0] "
<< Xcross[0]<<", Ycross[0] " <<Ycross[0]<<"\n\t Xcross[1] "
<< Xcross[1]<<", Ycross[1] " <<Ycross[1]<<", R = "<<Rr<<", Lungh. arco "<<length<<endl;
}
//-------------------fine stampe

//-------------------------------------------------------------------------


	Sprevious = atan2(Ycross[0]-Oyy,Xcross[0]-Oxx);
	ibad=0;
	ninside=0;

	for (i=0; i<nHits;i++){
		if( ! IsInsideArc(
			Oxx,Oyy,Charge,
			Xcross,
			Ycross,
			S[i])
			){
			continue;
if(istampa>1)cout<<"in SttSkewCleanup :hit n. "<< ListHits[i]
	<<" is NOT inside the arc between entrance and exit; hit excluded!\n";
		}

		ninside++;

		Distance[i] = 2.*Rr*Rr*(1.-cos(S[i]-Sprevious)); // this is the usual
					//  distance**2 formula: (x1-x2)**2+(y1-y2)**2;
					// it is already 'protected' against S[i] jumps
					// around 2PI/0.
		Sprevious = S[i];
		if(Distance[i]<0.) Distance[i]=0.;   // rounding errors protection.
if(istampa>=2)cout<<"in SttSkewCleanup, Hit n. "<< ListHits[i]<<" has Distance "
<<sqrt(Distance[i]) <<endl;
		if(Distance[i]>cut2){
			if(Distance[i]>16.*cut2){
if(istampa>=2)cout<<"in SttSkewCleanup, Hit n. "<< ListHits[i]<<" has Distance "
<<sqrt(Distance[i]) <<" which is >4.*cut [="<<cut<<"], discard the track!"<<endl;
				return false;
			}
if(istampa>=2)cout<<"in SttSkewCleanup, Hit n. "<< ListHits[i]<<" has Distance "
<<sqrt(Distance[i]) <<" which is > cut [="<<cut<<"]."<<endl;
			ibad++;
		}
	}	// end of do (i=1; i<nHits;i++)



	// cut on the minimum (conservative) n. hits that must have fired
	if( ninside < ((int) length/DiameterStrawTube )-islack ){
		if(istampa>1){
			cout<<"in SttSkewCleanup, n. Hits inside = "<<ninside
			<<" is < n. hits that should be inside at least = "
			<<((int)length/DiameterStrawTube)<<"-islack ("<<
			islack<<"), track rejected!\n";
			return false;
		}
	}


	// compute the distance of last hit to point at which track leaves this detector volume
	// or the last physical possible Fi (given the length of the  straw).

	if(IsInsideArc(Oxx,Oyy,Charge,
			Xcross,
			Ycross,
			S[nHits-1])
			) {
		Distance[nHits] =
		(Oxx+Rr*cos(S[nHits-1])-Xcross[1])*(Oxx+Rr*cos(S[nHits-1])-Xcross[1]) +
		(Oyy+Rr*sin(S[nHits-1])-Ycross[1])*(Oyy+Rr*sin(S[nHits-1])-Ycross[1]);

if(istampa>=2)cout<<"in SttSkewCleanup, last Hit n. "<< ListHits[nHits-1]<<" has Distance from boundary "
<<sqrt(Distance[nHits]) <<endl;
	   if( Distance[nHits]>cut2 ){
		if( Distance[nHits]>16.*cut2){
if(istampa>=2)cout<<"in SttSkewCleanup, last Hit n. "<< ListHits[nHits-1]<<" has Distance from boundary "
<<sqrt(Distance[nHits]) <<" which is >4 .*cut [="<<cut<<"], discard the track!"<<endl;
			return false;
		}
if(istampa>=2)cout<<"in SttSkewCleanup, last Hit n. "<< ListHits[nHits-1]<<" has Distance from boundary "
<<sqrt(Distance[nHits]) <<" and it is > cut [="<<cut<<"]."<<endl;
		ibad++;
	   }
	}  // end of if(IsInsideArc(



	if( ibad > maxnum){
if(istampa>=2&&IVOLTE<20)cout<<"in SttSkewCleanup, reject this track because ibad = "<< ibad
<<" and it is >  maxnum [="<<maxnum<<"].\n";
		 return false;
	}

	return true;






};



//----------end of function PndSttMvdTracking::SttSkewCleanup










//----------begin of function PndSttMvdTracking::BadTrack_ParStt
  bool PndSttMvdTracking::BadTrack_ParStt(
			Double_t Oxx,
			Double_t Oyy,
			Double_t Rr,
			Short_t Charge,
			Double_t Xcross[2],  // Xcross[0]=point of entrance;
						//  Xcross[1]=point of exit.
			Double_t Ycross[2],
//			bool  ConsiderLastHit,
			UShort_t nHits,
			UShort_t* ListHits,
			Double_t info[][7],
			Double_t cut,
			UShort_t maxnum,
			UShort_t islack// uncertainty allowed as far as
		// the n. of hits that should be present in a given section of the Stt track.
				)
{
	UShort_t	ibad,
			ihit,
			ninside;

	Double_t	cut2,
			length,
			Xprevious,
			Yprevious,
			S,
			Distance[nmaxSttHits+1];

	cut2=cut*cut;
	ibad=0;

	Xprevious=Xcross[0];
	Yprevious=Ycross[0];

	length=CalculateArcLength(Oxx,
				Oyy,
				Rr,
				Charge,
				Xcross,
				Ycross
				);
if(istampa>1) {cout<<"in BadTrack_ParStt : Xingresso "<<Xcross[0]<<", Yingresso "<<Ycross[0]
	<<", Xuscita "<<Xcross[1]<<", Yuscita "<<Ycross[1]<<", Lungh. arco "
	<<length<<", islack "<<islack<<endl;
	}


	ninside=0;
	for(ihit=0; ihit<nHits;ihit++){
		S = atan2(info[ListHits[ihit]][1]-Oyy,info[ListHits[ihit]][0]-Oxx);
		if(S<0.) S += 2.*PI;
		if(S<0.) S = 0.;
		if( ! IsInsideArc(
			Oxx,Oyy,Charge,
			Xcross,
			Ycross,
			S
		) ) {
			continue;
			}
		ninside++;
		Distance[ihit]= 
			(info[ListHits[ihit]][0]-Xprevious)*
			(info[ListHits[ihit]][0]-Xprevious)+
			(info[ListHits[ihit]][1]-Yprevious)*
			(info[ListHits[ihit]][1]-Yprevious);
if(istampa>1) {cout<<"in BadTrack_ParStt :hit || n. "<<ListHits[ihit]<<", X "<<info[ListHits[ihit]][0]
<<", Y "<<info[ListHits[ihit]][1]<<"\n\tX prima "<<Xprevious
<<", Y prima "<<Yprevious<<", Distanza "<<sqrt(Distance[ihit])<<", cut = "
<<cut<<endl;
}

		Xprevious=info[ListHits[ihit]][0];
		Yprevious=info[ListHits[ihit]][1];


		if(Distance[ihit]>cut2){
			if(Distance[ihit]>16.*cut2){
				return true;
			}
			ibad++;
		}

	}	// end of   for(ihit=0,NinDet=0 ;ihit<nHits;ihit++)


	// cut on the minimum (conservative) n. hits that must have fired
	if( ninside < ((int) length/DiameterStrawTube)-islack ){
		if(istampa>1){
			cout<<"in BadTrack_ParStt, n. Hits inside = "<<ninside
			<<" is < n. hits that should be inside at least = "
			<<((int) length/DiameterStrawTube)<<"-islack ("<<
			islack<<"), track rejected!\n";
			return true;
		}
	}



	// compute the distance of last hit to point at which track leaves this detector volume.

	// here S is already the fi of the last point.
	if( IsInsideArc(Oxx,Oyy,Charge,
			Xcross,
			Ycross,
			S )
		) {
	   Distance[nHits] =
	   (info[ListHits[nHits-1]][0]-Xcross[1])*(info[ListHits[nHits-1]][0]-Xcross[1])+
	   (info[ListHits[nHits-1]][1]-Ycross[1])*(info[ListHits[nHits-1]][1]-Ycross[1])
						;
if(istampa>1)cout<<"in BadTrack_ParStt, Stt || hit n. (original notation) "<<
	ListHits[nHits-1]<<", Distance to boundary = "<<sqrt(Distance[nHits])
	<<", 4*cut "<<4.*cut<<endl;
	   if( Distance[nHits]>cut2 ){
		if( Distance[nHits]>16.*cut2){
			return true;
		}
		ibad++;
	   }
	}	// end of if( IsInsideArc


if(istampa>1)cout<<"in BadTrack_ParStt, ibad "<<ibad<<", max bad allowed = "<< maxnum<<endl;

	if( ibad > maxnum) return true;
//	if(ninarc==0) return true;
	return false;
}


//----------end of function PndSttMvdTracking::BadTrack_ParStt


//----------start  function PndSttMvdTracking::IntersectionsWithClosedPolygon

	Short_t  PndSttMvdTracking::IntersectionsWithClosedPolygon(
		//-------- inputs
		Double_t Ox,
		Double_t Oy,
		Double_t R,
		Double_t Rmi,	// min Apotema of hexagonal  volume intersected by track;
		Double_t Rma,	// max Apotema of hexagonal volume intersected by track;
		//-------- outputs
		UShort_t nIntersections[2],
		Double_t XintersectionList[][2],
		Double_t YintersectionList[][2]
						)
{


	// return integer convention :
	// -2 -->  track outside outer polygon;
	// -1 -->  track contained completely within inner polygon;
	// 0 -->  at least 1 intersection with inner polygon, at least 1 with outer polygon;
	// 1 -->  track contained completely between the two polygons;
	// 2 -->  track contained completely by larger polygons, with intersections in the smaller;
	// 3 -->  track completely outsiede the small polygon with intersections in the bigger.


	//  inner Hexagon --> 0
	//  outer Hexagon --> 1

	bool	internal[2],
		AtLeast1[2];

	UShort_t i,
		 is,
		 j,
		 Nintersections;

	Double_t mindist[2],
		 distance,
	//-------------------
	// a,b,c == coefficients of the implicit equations of the six sides of the Hexagon
	// centered at 0 :   a*x + b*y +c =0; see Gianluigi's logbook on page 277;
	// the coefficient  c  has to be multiplied by Erre.
	// The first side is 
		a[] = { 1./sqrt(3.) , 1., -1./sqrt(3.), 1./sqrt(3.) , 1. , -1./sqrt(3.) } ,
		b[] = { 1., 0. , 1., 1., 0. , 1. },
		c[] = { -2./sqrt(3.) , -1., 2./sqrt(3.), 2./sqrt(3.), 1., -2./sqrt(3.)},
	//----------------------

		Erre[] = {Rmi, Rma},  //  this is the distance from (0,0) 
					// of the Verteces of the Hexagon delimiting the Skew area
		tempX[2],
		tempY[2];

	// both hexagon_side_xlow and hexagon_side_xup must be multiplied by appropriate Erre;
	// sides of Hexagon ordered as a, b, c ..... in Gianluigi's logbook on page 280.
	Double_t
		hexagon_side_x[] = { 0., 1. , 1., 0., -1., -1., 0. },
		hexagon_side_y[] = { 2./sqrt(3.),1./sqrt(3.),-1./sqrt(3.),
					-2./sqrt(3.),-1./sqrt(3.), 1./sqrt(3.), 2./sqrt(3.)};



//-----------------------

//   find intersection with the 6 sides of the small exhagon delimiting the skew straws zone
//   see on page 277 of Gianluigi's logbook.

	// status  =0, at least 1 intersection with inner Hexagon, at least 1 intersection
	// with the outer Hexagon; =1, track contained completely between the
	// two Hexagons; = -1 track contained within inner Hexagon;
	// =-2 track outside outer Hexagon.


	for(i=0;i<2;i++){	// i=0 --> inner Hexagon, i= 1 --> outer Hexagon.
		AtLeast1[i] = false;
		nIntersections[i]=0;
		internal[i] = true;
		mindist[i]=999999.;
		for(is=0; is<6; is++){
			if ( IntersectionCircle_Segment(a[is],
						b[is],
						c[is]*Erre[i],
						hexagon_side_x[is]*Erre[i],
						hexagon_side_x[is+1]*Erre[i],
						hexagon_side_y[is]*Erre[i],
						hexagon_side_y[is+1]*Erre[i],
						Ox,
						Oy,
						R,
						&Nintersections,
						tempX,
						tempY,
						&distance // distance of (Ox,Oy) from line
							  // defined by  a*x+b*y+c=0.
							)
			   ){
			   AtLeast1[i]=true;
			   for(j=0;j<Nintersections;j++){
				XintersectionList[ nIntersections[i] ][i] =tempX[j];
				YintersectionList[ nIntersections[i] ][i] =tempY[j];
				nIntersections[i]++;
			   }
			}	// end of if ( IntersectionCircle_Segment( .....

			if(mindist[i]>distance) mindist[i]=distance;

			// the definition of 'internal' here is when the given Point
			// stays at the same side of the origin (0,0) with respect to
			// the given line of equation   a*x+b*y+c=0.
			 internal[i] = internal[i] && IsInternal(Ox,
								Oy,
								a[is],
								b[is],
								c[is]*Erre[i]
								);

		} // end of  for(is=0; is<6; is++)
	}  // end of  for(i=0;i<2;i++)


	if( (!AtLeast1[0]) && (!AtLeast1[1]) ){
	  if (!internal[1])  return -2;	// trajectory outside outer Hexagon.
	  if( R > mindist[1]) return -2;
	  if( !internal[0]) return 1; // trajectory contained between inner and outer Hexagon.
	  if( mindist[0] >= R)  return -1; //  trajectory contained in inner Hexagon.
	  return 1;	// trajectory contained between inner and outer Hexagon.
	} else if (AtLeast1[0] && AtLeast1[1] ){ // continuation of  if( (!AtLeast1[0]) && ...
	  return  0;
	} else if (AtLeast1[0]){
		return 2;
	}

	return 3;
}

//---------- end of  function PndSttMvdTracking::IntersectionsWithClosedPolygon



//----------start  function PndSttMvdTracking::IntersectionsWithOpenPolygon

	UShort_t  PndSttMvdTracking::IntersectionsWithOpenPolygon(
		//-------- inputs
		Double_t Ox, // Track parameter
		Double_t Oy, // Track parameter
		Double_t R, // Track parameter
		UShort_t nSides, // input, n. of Sides of open Polygon.
		Double_t *a, //  coefficient of formula :  aX + bY + c = 0 defining
		Double_t *b, //  the Polygon sides.
		Double_t *c,
		Double_t *Side_x,  // X,Y coordinate of the Sides vertices (in sequence, following
		Double_t *Side_y,  // the Polygon along.
		//-------- outputs
		Double_t *XintersectionList, // XintersectionList
		Double_t *YintersectionList // YintersectionList.
						)
{

	// this methods returns the n. of intersections.



	UShort_t i,
		 is,
		 j,
		 nIntersections,
		 Nintersections;

	Double_t mindist,
		 distance,
	//-------------------
	// a,b,c == coefficients of the implicit equations of the six sides of the Hexagon
	// centered at 0 :   a*x + b*y +c =0; see Gianluigi's logbook on page 277;
	// the coefficient  c  has to be multiplied by Erre.

		tempX[2],
		tempY[2];




//-----------------------

		nIntersections=0;
		for(is=0; is<nSides; is++){
			if ( IntersectionCircle_Segment(a[is],
						b[is],
						c[is],
						Side_x[is],
						Side_x[is+1],
						Side_y[is],
						Side_y[is+1],
						Ox,
						Oy,
						R,
						&Nintersections,
						tempX,
						tempY,
						&distance // distance of (Ox,Oy) from line
							  // defined by  a*x+b*y+c=0.
							)
			   ){
			   for(j=0;j<Nintersections;j++){
				XintersectionList[ nIntersections ] =tempX[j];
				YintersectionList[ nIntersections ] =tempY[j];
				nIntersections += Nintersections;
			   }
			}	// end of if ( IntersectionCircle_Segment( .....


		} // end of  for(is=0; is<nSides; is++)
//	}  // end of  for(i=0;i<2;i++)



	return nIntersections;
}

//---------- end of  function PndSttMvdTracking::IntersectionsWithOpenPolygon


//----------start  function PndSttMvdTracking::IntersectionsWithClosedbiHexagonLeft

	Short_t  PndSttMvdTracking::IntersectionsWithClosedbiHexagonLeft(
		//-------- inputs
		Double_t vgap,
		Double_t Ox,
		Double_t Oy,
		Double_t R,
		Double_t Ami,	// min Apotema of hexagonal  volume intersected by track;
		Double_t Ama,	// max Apotema of hexagonal volume intersected by track;
		//-------- outputs
		UShort_t *nIntersections,
		Double_t *XintersectionList,
		Double_t *YintersectionList
						)
{


	// return integer convention :
	// -1 -->  track outside outer perimeter;
	// 0 -->  at least 1 intersection with polygon;
  	// 1 -->  track contained completely between the two polygons;


	//  inner Hexagon --> 0
	//  outer Hexagon --> 1

	bool	internal,
		AtLeast1;

	UShort_t i,
		 is,
		 j,
		 Nintersections;

	Double_t aaa,
		 maxdistq,
		 distance,
	//-------------------
	// a,b,c == coefficients of the implicit equations of the 3 sides of the half inner Hexagon
	// plus 3 sides of the half outer Hexagon plus 2 vertical sides corresponding to the Gap :
	//    a*x + b*y +c =0; the numbering of these 8 sides foolows the convention of Gianluigi's
	// logbook on page 286.
a[] = {-1./sqrt(3.) ,	1.,	1./sqrt(3.),	1.,	1./sqrt(3.),	1.,	-1./sqrt(3.),	1.},
b[] = {1.,		0.,	1.,		0.,	1.,		0.,	1.,		0.},
c[] = {-2.*Ama/sqrt(3.),Ama,	2.*Ama/sqrt(3.),vgap/2.,2.*Ami/sqrt(3.),Ami,	-2.*Ami/sqrt(3.),vgap/2.},
	//----------------------

		tempX[2],
		tempY[2];

	// side_x and side_y are ordered as side1, side2, ..... in Gianluigi's logbook on page 286.
	Double_t
		side_x[] = { -vgap/2.,	-Ama ,	-Ama,	-vgap/2.,	-vgap/2.,	-Ami,
					-Ami,	-vgap/2.,	-vgap/2.},
		side_y[] = {(-0.5*vgap+2.*Ama)/sqrt(3.),	Ama/sqrt(3.),	-Ama/sqrt(3.),
			    -(-0.5*vgap+2.*Ama)/sqrt(3.),	-(-0.5*vgap+2.*Ami)/sqrt(3.),
			    -Ami/sqrt(3.),	Ami/sqrt(3.),	(-0.5*vgap+2.*Ami)/sqrt(3.),
			    (-0.5*vgap+2.*Ama)/sqrt(3.)	};



//-----------------------

//   find intersections (maximum 16) with the 8 sides.



		AtLeast1 = false;
		*nIntersections =0;
		internal = true;
		maxdistq=-9999.;
		for(is=0; is<8; is++){
			aaa = (side_x[is]-Ox)*(side_x[is]-Ox)+(side_y[is]-Oy)*(side_y[is]-Oy);
			if(aaa>maxdistq) maxdistq=aaa;
			aaa = (side_x[is+1]-Ox)*(side_x[is+1]-Ox)+(side_y[is+1]-Oy)*(side_y[is+1]-Oy);
			if(aaa>maxdistq) maxdistq=aaa;
			if ( IntersectionCircle_Segment(
						a[is],
						b[is],
						c[is],
						side_x[is],
						side_x[is+1],
						side_y[is],
						side_y[is+1],
						Ox,
						Oy,
						R,
						&Nintersections,
						tempX,
						tempY,
						&distance // distance of (Ox,Oy) from line
							  // defined by  a*x+b*y+c=0.
							)
			   ){
			   AtLeast1=true;
			   for(j=0;j<Nintersections;j++){
				XintersectionList[ *nIntersections ] =tempX[j];
				YintersectionList[ *nIntersections ] =tempY[j];
				(*nIntersections)++;
			   }
			}	// end of if ( IntersectionCircle_Segment( .....


			// the definition of 'internal' here is when the given Point
			// stays at the same side of the origin (0,0) with respect to
			// the given line of equation   a*x+b*y+c=0.
			if(is<3) {
				internal = internal && IsInternal(Ox,
								Oy,
								a[is],
								b[is],
								c[is]
								);
			} else {
				internal = internal && (!IsInternal(Ox,
								Oy,
								a[is],
								b[is],
								c[is]
								) );
			}

		} // end of  for(is=0; is<8; is++)


	if( !AtLeast1){
	  if (!internal)  return -1;// trajectory outside polygon.
	  if( maxdistq < R*R ) return -1;// trajectory outside polygon.
	  return 1;	// trajectory completely contained inside this Polygon.
	}
	return 0;

}

//---------- end of  function PndSttMvdTracking::IntersectionsWithClosedbiHexagonLeft

//----------start  function PndSttMvdTracking::IntersectionsWithClosedbiHexagonRight

	Short_t  PndSttMvdTracking::IntersectionsWithClosedbiHexagonRight(
		//-------- inputs
		Double_t vgap,
		Double_t Ox,
		Double_t Oy,
		Double_t R,
		Double_t Ami,	// min Apotema of hexagonal  volume intersected by track;
		Double_t Ama,	// max Apotema of hexagonal volume intersected by track;
		//-------- outputs
		UShort_t *nIntersections,
		Double_t *XintersectionList,
		Double_t *YintersectionList
						)
{


	// return integer convention :
	// -1 -->  track outside outer perimeter;
	// 0 -->  at least 1 intersection with polygon;
  	// 1 -->  track contained completely between the two polygons;


	//  inner Hexagon --> 0
	//  outer Hexagon --> 1

	bool	internal,
		AtLeast1;

	UShort_t i,
		 is,
		 j,
		 Nintersections;

	Double_t aaa,
		 maxdistq,
		 distance,
	//-------------------
	// a,b,c == coefficients of the implicit equations of the 3 sides of the half inner Hexagon
	// plus 3 sides of the half outer Hexagon plus 2 vertical sides corresponding to the Gap :
	//    a*x + b*y +c =0; the numbering of these 8 sides foolows the convention of Gianluigi's
	// logbook on page 286.
a[] = {1./sqrt(3.) ,	1.,	-1./sqrt(3.),	1.,	-1./sqrt(3.),	1.,	1./sqrt(3.),	1.},
b[] = {1.,		0.,	1.,		0.,	1.,		0.,	1.,		0.},
c[] = {-2.*Ama/sqrt(3.),-Ama,	2.*Ama/sqrt(3.),-vgap/2.,2.*Ami/sqrt(3.),-Ami,	-2.*Ami/sqrt(3.),-vgap/2.},
	//----------------------

		tempX[2],
		tempY[2];

	// side_x and side_y are ordered as side1, side2, ..... in Gianluigi's logbook on page 286.
	Double_t
		side_x[] = { vgap/2.,	Ama ,	Ama,	vgap/2.,	vgap/2.,	Ami,
					Ami,	vgap/2.,	vgap/2.},
		side_y[] = {(-0.5*vgap+2.*Ama)/sqrt(3.),	Ama/sqrt(3.),	-Ama/sqrt(3.),
			    -(-0.5*vgap+2.*Ama)/sqrt(3.),	-(-0.5*vgap+2.*Ami)/sqrt(3.),
			    -Ami/sqrt(3.),	Ami/sqrt(3.),	(-0.5*vgap+2.*Ami)/sqrt(3.),
			    (-0.5*vgap+2.*Ama)/sqrt(3.)	};



//-----------------------

//   find intersections (maximum 16) with the 8 sides.



		AtLeast1 = false;
		*nIntersections =0;
		internal = true;
		maxdistq=-9999.;
		for(is=0; is<8; is++){
			aaa = (side_x[is]-Ox)*(side_x[is]-Ox)+(side_y[is]-Oy)*(side_y[is]-Oy);
			if(aaa>maxdistq) maxdistq=aaa;
			aaa = (side_x[is+1]-Ox)*(side_x[is+1]-Ox)+(side_y[is+1]-Oy)*(side_y[is+1]-Oy);
			if(aaa>maxdistq) maxdistq=aaa;
			if ( IntersectionCircle_Segment(
						a[is],
						b[is],
						c[is],
						side_x[is],
						side_x[is+1],
						side_y[is],
						side_y[is+1],
						Ox,
						Oy,
						R,
						&Nintersections,
						tempX,
						tempY,
						&distance // distance of (Ox,Oy) from line
							  // defined by  a*x+b*y+c=0.
							)
			   ){
			   AtLeast1=true;
			   for(j=0;j<Nintersections;j++){
				XintersectionList[ *nIntersections ] =tempX[j];
				YintersectionList[ *nIntersections ] =tempY[j];
				(*nIntersections)++;
			   }
			}	// end of if ( IntersectionCircle_Segment( .....


			// the definition of 'internal' here is when the given Point
			// stays at the same side of the origin (0,0) with respect to
			// the given line of equation   a*x+b*y+c=0.
			if(is<3) {
				internal = internal && IsInternal(Ox,
								Oy,
								a[is],
								b[is],
								c[is]
								);
			} else {
				internal = internal && (!IsInternal(Ox,
								Oy,
								a[is],
								b[is],
								c[is]
								) );
			}

		} // end of  for(is=0; is<8; is++)


	if( !AtLeast1){
	  if (!internal)  return -1;// trajectory outside polygon.
	  if( maxdistq < R*R ) return -1;// trajectory outside polygon.
	  return 1;	// trajectory completely contained inside this Polygon.
	}
	return 0;

}

//---------- end of  function PndSttMvdTracking::IntersectionsWithClosedbiHexagonRight

//----------begin of function PndSttMvdTracking::IntersectionCircle_Segment

	bool PndSttMvdTracking::IntersectionCircle_Segment(
				Double_t a, // coefficients implicit equation.
				Double_t b, // of segment : a*x + b*y + c =0.
				Double_t c,
				Double_t P1x, // point delimiting the segment.
				Double_t P2x, // point delimiting the segment.
				Double_t P1y, // point delimiting the segment.
				Double_t P2y, // point delimiting the segment.
				Double_t Ox, // center of circle.
				Double_t Oy,
				Double_t R, // Radius of circle.
				UShort_t * Nintersections,
				Double_t XintersectionList[2],
				Double_t YintersectionList[2],
				Double_t *distance
								)
{

// this method finds the intersection of a circle with a segment. If the circle
// passes through an endpoint of the segment, that is considered an intersection also.

	bool status;

	Short_t ipossibility;

	Double_t aperp,
		 bperp,
		 cperp,
		 det,
		 distq,
		 dist1,
		 dist2,
		 length,
		 length_segmentq,
		 Rq,
		 x,
		 y,
		 Xintersection,
		 Yintersection;

	*Nintersections=0;
	det = a*a+b*b ;
	if(det < 1.e-20){
		cout<<"from  PndSttMvdTracking::IntersectionCircle_Segment :"
				<<" this is not the equation of a segment, return!\n";
		return false;
	}
	//  find if intersection circle - segment is possible.

	// check if there is an intersection (with the squares, it's the same!).
	distq = ( a*Ox+ b*Oy + c )*( a*Ox+ b*Oy + c )/det;
	*distance = sqrt(distq);
	Rq=R*R;
	length = Rq - distq;
	if(length <= 0. ) return false; // no intersection between trajectory and this
						// segment.
	length = sqrt(length);
	// coefficients of line perpendicular to input segment and passing for (Ox, Oy).
	aperp = -b;
	bperp = a;
	cperp = - aperp*Ox - bperp*Oy;

	// find intersection of segment with perpendicular : no need to check if
	// det is different from 0.
	Xintersection = (-bperp*c + b*cperp)/det;
	Yintersection = (-a*cperp + aperp*c)/det;
	det = sqrt(det);
	length_segmentq = (P1x-P2x)*(P1x-P2x) + (P1y-P2y)*(P1y-P2y);

	status = false;
	for(ipossibility=-1;ipossibility<2;ipossibility +=2){
		x = Xintersection + ipossibility*length*b/det ;
		y = Yintersection - ipossibility*length*a/det ;
		if ( (x-P1x)*(x-P1x)+(y-P1y)*(y-P1y) >  length_segmentq
						||
		     (x-P2x)*(x-P2x)+(y-P2y)*(y-P2y) >  length_segmentq
				    )  continue;
		status = true;
		XintersectionList[*Nintersections] = x;
		YintersectionList[*Nintersections] = y;
		(*Nintersections)++;
	}  //  end of  for(ipossibility=-1; ...


	return status;
}

//----------end of function PndSttMvdTracking::IntersectionCircle_Segment


//----------begin of function PndSttMvdTracking::IntersectionsWithGapSemicircle


	UShort_t PndSttMvdTracking::IntersectionsWithGapSemicircle(
			Double_t Oxx,
			Double_t Oyy,
			Double_t Rr,
			Double_t GAP,
			bool left,  // if true --> Left semicircle; false --> Right semicircle.
			Double_t Rma,
			Double_t *XintersectionList,
			Double_t *YintersectionList
						)
{

	UShort_t	nIntersectionsCircle;

	Double_t	cosFi,
			theta1,
			theta2,
			Theta1,
			Theta2,
			aaa,
			Fi,
			FI0,
			x1,
			x2,
			y1,
			y2;



	nIntersectionsCircle=0;
	aaa = sqrt(Oxx*Oxx+Oyy*Oyy);

	//  preliminary condition for having intersections between trajectory and  Circle.

	if( !( aaa >= Rr + Rma || Rr >= aaa + Rma) &&
		!( aaa + Rr <= Rma || aaa - Rr >= Rma  ) )	{

//	now the calculation

		FI0 = atan2(-Oyy,-Oxx);
		cosFi = (aaa*aaa + Rr*Rr - Rma*Rma)/(2.*Rr*aaa);
		if(cosFi<-1.) cosFi=-1.; else if(cosFi>1.) cosFi=1.;
		Fi = acos(cosFi);


		//  (x1, y1) and (x2,y2) are the intersections between the trajectory
		//  and the circle, in the laboratory reference frame.
		x1 = Oxx+Rr*cos(FI0 - Fi);
		y1 = Oyy+Rr*sin(FI0 - Fi);
		theta1 = atan2(y1,x1); // in this way theta1 is between -PI and PI.
		x2 = Oxx+Rr*cos(FI0 + Fi);
		y2 = Oyy+Rr*sin(FI0 + Fi);
		theta2 = atan2(y2,x2); // in this way theta2 is between -PI and PI.
		//  Theta1, Theta2 = angle of the edges of the outer circle + Gap in the laboratory frame.
		//  Theta1, Theta2 are angles between -PI/2 and +PI/2.
		if(!left){	//  Right (looking into the beam) Semicircle.
			Theta2 = atan2( sqrt(Rma*Rma-GAP*GAP/4.),GAP/2.);
			Theta1 = atan2( -sqrt(Rma*Rma-GAP*GAP/4.),GAP/2.);
			if( Theta1<= theta1 && theta1 <= Theta2 ){
				XintersectionList[nIntersectionsCircle]=x1;
				YintersectionList[nIntersectionsCircle]=y1;
				nIntersectionsCircle++;
			}
			if( Theta1<= theta2 && theta2 <= Theta2 ){
				XintersectionList[nIntersectionsCircle]=x2;
				YintersectionList[nIntersectionsCircle]=y2;
				nIntersectionsCircle++;
			}
		} else {	//  Left (looking into the beam) Semicircle.
			Theta2 = atan2( -sqrt(Rma*Rma-GAP*GAP/4.),-GAP/2.);
			Theta1 = atan2( sqrt(Rma*Rma-GAP*GAP/4.),-GAP/2.);
			if( Theta1<= theta1 || theta1 <= Theta2 ){
				XintersectionList[nIntersectionsCircle]=x1;
				YintersectionList[nIntersectionsCircle]=y1;
				nIntersectionsCircle++;
			}
			if( Theta1<= theta2 || theta2 <= Theta2 ){
				XintersectionList[nIntersectionsCircle]=x2;
				YintersectionList[nIntersectionsCircle]=y2;
				nIntersectionsCircle++;
			}
		}

	}  // end of   if (!( a >= Rr + Rma || .....

//---------------------------- end of calculation intersection with outer circle.

	return nIntersectionsCircle;


}
//----------end of function PndSttMvdTracking::IntersectionsWithGapSemicircle




//----------star of function PndSttMvdTracking::IsInternal
	bool PndSttMvdTracking::IsInternal(
			Double_t Px,	// point
			Double_t Py,
			Double_t Xtraslation,
			Double_t Ytraslation,
			Double_t Theta
					)
{


	// for explanations see Gianluigi's logbook on page 278-280.

	if( (Xtraslation-Px)*sin(Theta) + (Py-Ytraslation)*cos(Theta) >= 0. ) return true;
	else return false;


}
//----------end of function PndSttMvdTracking::IsInternal





//----------star of function PndSttMvdTracking::ChooseEntranceExitbis
	void PndSttMvdTracking::ChooseEntranceExitbis(
			Double_t Oxx,
			Double_t Oyy,
			Short_t  Charge,
			Double_t FiStart,
			UShort_t nIntersections,
			Double_t *XintersectionList, //  second index =1 -->inner polygon;
			Double_t *YintersectionList, //  second index =2 -->outer polygon.
			Double_t Xcross[2],	// output
			Double_t Ycross[2]	// output
					)
{

	UShort_t i,
		 j;
// this method works under the hypothesis that there are at least 2 intersections.
	if (nIntersections<2) return;

	  if(Charge > 0) {
		Int_t auxIndex[100];
		Double_t fi[100];
		for( i=0;i<nIntersections;i++){
		  fi[i] = atan2(YintersectionList[i]-Oyy,
				XintersectionList[i]-Oxx);
		  if( fi[i] < 0.) fi[i]  += 2.*PI;
		  if( fi[i] > FiStart) fi[i]  -= 2.*PI;
		  if( fi[i] > FiStart) fi[i] = FiStart;
		  auxIndex[i]=i;
		} // end of for( i=0;i<nIntersections[j];i++)
		Merge_Sort( nIntersections, fi, auxIndex);
		Xcross[0] = XintersectionList[ auxIndex[nIntersections-1] ];
		Ycross[0] = YintersectionList[ auxIndex[nIntersections-1] ];
		Xcross[1] = XintersectionList[ auxIndex[nIntersections-2] ];
		Ycross[1] = YintersectionList[ auxIndex[nIntersections-2] ];

	  } else { // case in which Charge is negative.
		Int_t auxIndex[nIntersections];
		Double_t fi[nIntersections];
		for( i=0;i<nIntersections;i++){
		  fi[i] = atan2(YintersectionList[i]-Oyy,
				XintersectionList[i]-Oxx);
		  if( fi[i] < 0.) fi[i]  += 2.*PI;
		  if( fi[i] < FiStart) fi[i]  += 2.*PI;
		  if( fi[i] < FiStart) fi[i] += FiStart;
		  auxIndex[i]=i;
		} // end of for( i=0;i<nIntersections;i++)
		Merge_Sort( nIntersections, fi, auxIndex);
		Xcross[0] = XintersectionList[ auxIndex[0] ];
		Ycross[0] = YintersectionList[ auxIndex[0] ];
		Xcross[1] = XintersectionList[ auxIndex[1] ];
		Ycross[1] = YintersectionList[ auxIndex[1] ];
	  }




}
//----------end of function PndSttMvdTracking::ChooseEntranceExitbis










//----------begin of function PndSttMvdTracking::SeparateInnerOuterParallel

	void PndSttMvdTracking::SeparateInnerOuterParallel(

			// input
			UShort_t nHits,
			UShort_t *ListHits,
			Double_t info[][7],
			Double_t RStrawDetInnerParMax,

			// output
			UShort_t *nInnerHits,
			UShort_t *ListInnerHits,
			UShort_t *nOuterHits,
			UShort_t *ListOuterHits,

			UShort_t *nInnerHitsLeft,
			UShort_t *ListInnerHitsLeft,
			UShort_t *nInnerHitsRight,
			UShort_t *ListInnerHitsRight,

			UShort_t *nOuterHitsLeft,
			UShort_t *ListOuterHitsLeft,
			UShort_t *nOuterHitsRight,
			UShort_t *ListOuterHitsRight
								)
{

	UShort_t ihit;

	Double_t r;

//   separation of inner Parallel Stt hits from outer Parallel Stt hits.

	*nInnerHits=0;
	*nInnerHitsLeft=0;
	*nInnerHitsRight=0;
	*nOuterHits=0;
	*nOuterHitsLeft=0;
	*nOuterHitsRight=0;
	for(ihit=0  ;ihit<nHits;ihit++){
		r = sqrt( info[ListHits[ihit]][0]*info[ListHits[ihit]][0] +
			 info[ListHits[ihit]][1]*info[ListHits[ihit]][1]);
		// the value 2.*RStrawDetectorParMax/sqrt(3.) is because RStrawDetectorParMax
		// is an Apotema !
		if(r>2.*ApotemaMaxInnerParStraw/sqrt(3.) ){	// outer Parallel hit.
			ListOuterHits[ *nOuterHits ] = ListHits[ihit];
			(*nOuterHits)++;
			if(info[ListHits[ihit]][0]<0.){
				ListOuterHitsLeft[ *nOuterHitsLeft ] = ListHits[ihit];
				(*nOuterHitsLeft)++;
			} else {
				ListOuterHitsRight[ *nOuterHitsRight ] = ListHits[ihit];
				(*nOuterHitsRight)++;
			}
		}else{
			ListInnerHits[ *nInnerHits ] = ListHits[ihit];
			(*nInnerHits)++;
			if(info[ListHits[ihit]][0]<0.){
				ListInnerHitsLeft[ *nInnerHitsLeft ] = ListHits[ihit];
				(*nInnerHitsLeft)++;
			} else {
				ListInnerHitsRight[ *nInnerHitsRight ] = ListHits[ihit];
				(*nInnerHitsRight)++;
			}
		}
	}

}


//----------end of function PndSttMvdTracking::SeparateInnerOuterParallel





//----------begin of function PndSttMvdTracking::FindTrackEntranceExitbiHexagonLeft

	Short_t PndSttMvdTracking::FindTrackEntranceExitbiHexagonLeft(
				Double_t vgap,
				Double_t Oxx,
				Double_t Oyy,
				Double_t Rr,
				Short_t  Charge,
				Double_t Start[3],
				Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
				Double_t ApotemaMax,
				Double_t Xcross[2],
				Double_t Ycross[2]
							)
{
	Short_t flag;
	UShort_t	nIntersections;
	Double_t	FiStart,
			XintersectionList[16], // all the possible intersections
			YintersectionList[16]; // (up to 16 intersections).

// The following is the form of the Left (looking from downstream into the beam) biHexagon
// geometrical shape considered in this method :
//
/*

	      /|
	     / |
	    /  |
	   /  /
	  /  / 
	 /  /  
	/  /   
	|  |   
	|  |   
	|  |   
	|  |   
	\  \   
	 \  \  
	  \  \ 
	   \  \
	    \  |
	     \ |
	      \|

*/
// finding all possible intersections with inner parallel straw region.
// The inner parallel straw region is delimited by two hexagons.

	// flag meaning :
	// -1 -->  track outside outer perimeter;
	// 0 -->  at least 1 intersection with polygon, therefore a possible entry and an exit;
	// 1 -->  track contained completely between the two polygons;


	flag=IntersectionsWithClosedbiHexagonLeft(
		vgap,
		Oxx,
		Oyy,
		Rr,
		ApotemaMin,	// Apotema of the inner Hexagon,
		ApotemaMax,// Apotema of the outer Hexagon.
		&nIntersections,
		XintersectionList, // XintersectionList[..][0] --> inner polygon,
				   // XintersectionList[..][1] --> outer polygon.
		YintersectionList
					);

	// IMPORTANT :
	// this is true because here it is assumed that the track comes from (0,0,0)
	// otherwise the code must be changed!

	if (!(flag == 0)) return flag;
	if( nIntersections<2) return -1;

//-------  among all  possible intersection find the entrance point (Xcross[0], Ycross[0])
//	   and the exit point (Xcross[1], Ycross[1])  of this track.

//-------- the starting point of the track.
	FiStart = atan2( Start[1]-Oyy,Start[0]-Oxx);
	if(FiStart<0.) FiStart+= 2.*PI;
	if(FiStart<0.) FiStart =0.;

	// this method selects the entrance and exit points of the trajectory among all
	// geometrical intersections of the circular trajectory with the straw particular
	// volume.

	// so at this point, the intersections are at least 2.

	ChooseEntranceExitbis(
			Oxx,
			Oyy,
			Charge,
			FiStart,
			nIntersections,
			XintersectionList,
			YintersectionList,
			Xcross,	// output
			Ycross	// output
					);


//----------------------

	return flag;

}

//----------end of function PndSttMvdTracking::FindTrackEntranceExitbiHexagonLeft









//----------begin of function PndSttMvdTracking::FindTrackEntranceExitbiHexagonRight


	Short_t PndSttMvdTracking::FindTrackEntranceExitbiHexagonRight(
				Double_t vgap,
				Double_t Oxx,
				Double_t Oyy,
				Double_t Rr,
				Short_t  Charge,
				Double_t Start[3],
				Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
				Double_t ApotemaMax,
				Double_t Xcross[2],
				Double_t Ycross[2]
							)
{
	Short_t flag;
	UShort_t	nIntersections;
	Double_t	FiStart,
			XintersectionList[16], // all the possible intersections
			YintersectionList[16]; // (up to 16 intersections).

// The following is the form of the Left (looking from downstream into the beam) biHexagon
// geometrical shape considered in this method :
//
/*
	|\
	| \
	|  \
	 \  \
	  \  \
	   |  |
	   |  |
	   /  /
	  /  /
	 /  /
	|  /
	| /
	|/
*/

// finding all possible intersections with inner parallel straw region.
// The inner parallel straw region is delimited by two hexagons.

	// flag meaning :
	// -1 -->  track outside outer perimeter;
	// 0 -->  at least 1 intersection with polygon, therefore a possible entry and an exit;
	// 1 -->  track contained completely between the two polygons;


	flag=IntersectionsWithClosedbiHexagonRight(
		vgap,
		Oxx,
		Oyy,
		Rr,
		ApotemaMin,	// Apotema of the inner Hexagon,
		ApotemaMax,// Apotema of the outer Hexagon.
		&nIntersections,
		XintersectionList,
		YintersectionList
					);




	// IMPORTANT :
	// this is true because here it is assumed that the track comes from (0,0,0)
	// otherwise the code must be changed!

	if (!(flag == 0)) return flag;
	if( nIntersections<2) return -1;

//-------  among all  possible intersection find the entrance point (Xcross[0], Ycross[0])
//	   and the exit point (Xcross[1], Ycross[1])  of this track.

//-------- the starting point of the track.
	FiStart = atan2( Start[1]-Oyy,Start[0]-Oxx);
	if(FiStart<0.) FiStart+= 2.*PI;
	if(FiStart<0.) FiStart =0.;

	// this method selects the entrance and exit points of the trajectory among all
	// geometrical intersections of the circular trajectory with the straw particular
	// volume.

	// so at this point, the intersections are at least 2.

	ChooseEntranceExitbis(
			Oxx,
			Oyy,
			Charge,
			FiStart,
			nIntersections,
			XintersectionList,
			YintersectionList,
			Xcross,	// output
			Ycross	// output
					);


//----------------------

	return flag;

}


//----------end of function PndSttMvdTracking::FindTrackEntranceExitbiHexagonRight


//----------begin of function PndSttMvdTracking::FindTrackEntranceExitHexagonCircleLeft


	Short_t PndSttMvdTracking::FindTrackEntranceExitHexagonCircleLeft(
				Double_t Oxx,
				Double_t Oyy,
				Double_t Rr,
				Short_t  Charge,
				Double_t Start[3],
				Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
				Double_t Rma, // outer radius of the Circle.
				Double_t GAP,
				Double_t Xcross[2],
				Double_t Ycross[2]
							)
{

//  This methods finds the intersections between a trajectory coming from (0,0) and
//  parameters  Oxx, Oyy, Rr   with the closed geometrical figure (in XY) formed by the STT
//  external Left semicircle and the Outer STT parallel Left straw semi-Hexagon + Gap for
//  the pellet target target.
//  It returns -1 if there are 0 or 1 intersections, 0 if there are at least 2 intersections.


	Double_t	cosFi,
			theta1,
			theta2,
			Theta1,
			Theta2,
			aaa,
			Fi,
			FI0,
			x1,
			x2,
			y1,
			y2;
//------------------

	UShort_t	nIntersectionsCircle,
			nIntersections;
	Double_t	FiStart,
			XintersectionList[12],
			YintersectionList[12]; // all the possible intersections (up to 12 intersections).

// finding all possible intersections with inner parallel straw region.


	Double_t Side_x[] = {	-GAP/2., -GAP/2. , -ApotemaMin, -ApotemaMin, -GAP/2., -GAP/2. },
		 Side_y[] = {	sqrt(Rma*Rma-GAP*GAP/4.),  (2.*ApotemaMin-0.5*GAP)/sqrt(3.),
				ApotemaMin/sqrt(3.),
				-ApotemaMin/sqrt(3.),
				-(2.*ApotemaMin-0.5*GAP)/sqrt(3.), -sqrt(Rma*Rma-GAP*GAP/4.)},
		 a[] =	{1.,		-1./sqrt(3.),	1.,	1./sqrt(3.),	1.},
		 b[] =	{0.,		1.,		0.,	1.,		0.},
		 c[] =	{GAP/2., -2.*ApotemaMin/sqrt(3.),ApotemaMin, 2.*ApotemaMin/sqrt(3.), GAP/2.};

	nIntersections=IntersectionsWithOpenPolygon(
		Oxx,
		Oyy,
		Rr,
		5, //  n. Sides of open Polygon.
		a, //  coefficient of formula :  aX + bY + c = 0 defining the Polygon sides.
		b,
		c,
		Side_x,  // X,Y coordinate of the Sides vertices (in sequence, following
		Side_y,  // the Polygon along.
		XintersectionList, // XintersectionList
		YintersectionList // YintersectionList.
					);




//-------------------------------------------------------------------------
// finding intersections of trajectory [assumed to originate from (0,0) ]
// with outer semicircle, the Left part.

	nIntersectionsCircle=IntersectionsWithGapSemicircle(
		Oxx, // input from trajectory
		Oyy, // input from trajectory
		Rr, // input from trajectory
		GAP, // input, vertical gap in XY plane of STT detector.
		true, // true --> Left semicircle, false --> Right semicircle.
		Rma, // radius of external Stt containment.
		&XintersectionList[nIntersections], // output, X list of intersections (maximal 2).
		&YintersectionList[nIntersections]
						);
	nIntersections += nIntersectionsCircle;

//-------- the starting point of the track.

	if(nIntersections<2) return -1;

	FiStart = atan2( Start[1]-Oyy,Start[0]-Oxx);
	if(FiStart<0.) FiStart+= 2.*PI;
	if(FiStart<0.) FiStart =0.;

	// this method selects the entrance and exit points of the trajectory among all
	// geometrical intersections of the circular trajectory with the straw particular
	// volume.


	ChooseEntranceExitbis(
			Oxx,
			Oyy,
			Charge,
			FiStart,
			nIntersections,
			XintersectionList,
			YintersectionList,
			Xcross,	// output
			Ycross	// output
					);


	return 0;

}

//----------end of function PndSttMvdTracking::FindTrackEntranceExitHexagonCircleLeft












//----------begin of function PndSttMvdTracking::FindTrackEntranceExitHexagonCircleRight


	Short_t PndSttMvdTracking::FindTrackEntranceExitHexagonCircleRight(
				Double_t Oxx,
				Double_t Oyy,
				Double_t Rr,
				Short_t  Charge,
				Double_t Start[3],
				Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
				Double_t Rma, // outer radius of the Circle.
				Double_t GAP,
				Double_t Xcross[2],
				Double_t Ycross[2]
							)
{

//  This methods finds the intersections between a trajectory coming from (0,0) and
//  parameters  Oxx, Oyy, Rr   with the closed geometrical figure (in XY) formed by the STT
//  external Left semicircle and the Outer STT parallel Left straw semi-Hexagon + Gap for
//  the pellet target target.
//  It returns -1 if there are 0 or 1 intersections, 0 if there are at least 2 intersections.


	Double_t	cosFi,
			theta1,
			theta2,
			Theta1,
			Theta2,
			aaa,
			Fi,
			FI0,
			x1,
			x2,
			y1,
			y2;
//------------------

	UShort_t	nIntersectionsCircle,
			nIntersections;
	Double_t	FiStart,
			XintersectionList[12],
			YintersectionList[12]; // all the possible intersections (up to 10 intersections).

// finding all possible intersections with inner parallel straw region.


	Double_t Side_x[] = {	GAP/2., GAP/2. , ApotemaMin, ApotemaMin, GAP/2., GAP/2. },
		 Side_y[] = {	sqrt(Rma*Rma-GAP*GAP/4.),  (2.*ApotemaMin-GAP)/sqrt(3.),
				ApotemaMin/sqrt(3.),
				-ApotemaMin/sqrt(3.),
				-(2.*ApotemaMin-GAP)/sqrt(3.), -sqrt(Rma*Rma-GAP*GAP/4.)},
		 a[] =	{1.,		1./sqrt(3.),	1.,	-1./sqrt(3.),	1.},
		 b[] =	{0.,		1.,		0.,	1.,		0.},
		 c[] =	{-GAP/2.,-2.*ApotemaMin/sqrt(3.),-ApotemaMin, 2.*ApotemaMin/sqrt(3.), -GAP/2.};

	nIntersections=IntersectionsWithOpenPolygon(
		Oxx,
		Oyy,
		Rr,
		5, //  n. Sides of open Polygon.
		a, //  coefficient of formula :  aX + bY + c = 0 defining the Polygon sides.
		b,
		c,
		Side_x,  // X,Y coordinate of the Sides vertices (in sequence, following
		Side_y,  // the Polygon along.
		XintersectionList, // XintersectionList
		YintersectionList // YintersectionList.
					);



//-------------------------------------------------------------------------
// finding intersections of trajectory [assumed to originate from (0,0) ]
// with outer semicircle, the Left part.

	nIntersectionsCircle=IntersectionsWithGapSemicircle(
		Oxx, // input from trajectory
		Oyy, // input from trajectory
		Rr, // input from trajectory
		GAP, // input, vertical gap in XY plane of STT detector.
		false, // true --> Left semicircle, false --> Right semicircle.
		Rma, // radius of external Stt containment.
		&XintersectionList[nIntersections], // output, X list of intersections (maximal 2).
		&YintersectionList[nIntersections]
						);


	nIntersections += nIntersectionsCircle;

//-------- the starting point of the track.

	if(nIntersections<2) return -1;

	FiStart = atan2( Start[1]-Oyy,Start[0]-Oxx);
	if(FiStart<0.) FiStart+= 2.*PI;
	if(FiStart<0.) FiStart =0.;

	// this method selects the entrance and exit points of the trajectory among all
	// geometrical intersections of the circular trajectory with the straw particular
	// volume.

	ChooseEntranceExitbis(
			Oxx,
			Oyy,
			Charge,
			FiStart,
			nIntersections,
			XintersectionList,
			YintersectionList,
			Xcross,	// output
			Ycross	// output
					);


	return 0;

}

//----------end of function PndSttMvdTracking::FindTrackEntranceExitHexagonCircleRight




//----------begin of function PndSttMvdTracking::FindIntersectionsOuterCircle


	Short_t PndSttMvdTracking::FindIntersectionsOuterCircle(
				Double_t oX,
				Double_t oY,
				Double_t R,
				Double_t Rma,
				Double_t Xcross[2],
				Double_t Ycross[2]
					)
{

	// return -1 --> non intersection;
	// return 0  --> 2 intersections.

	Double_t	a,
			cosFi,
			Fi,
			FI0;
	a = sqrt(oX*oX+oY*oY);

	// case with no intersections.
	if( a >= R + Rma || R >= a + Rma ||  a + R <= Rma) return -1;


	FI0 = atan2(-oY,-oX);
	cosFi = (a*a + R*R - Rma*Rma)/(2.*R*a);
	if(cosFi<-1.) cosFi=-1.; else if(cosFi>1.) cosFi=1.;
	Fi = acos(cosFi);

	Xcross[0] = oX + R*cos(FI0+Fi);
	Ycross[0] = oY + R*sin(FI0+Fi);
	Xcross[1] = oX + R*cos(FI0-Fi);
	Ycross[1] = oY + R*sin(FI0-Fi);

	return 0;
}
//----------end of function PndSttMvdTracking::FindIntersectionsOuterCircle




//----------begin of function PndSttMvdTracking::IsInsideCircle



	bool PndSttMvdTracking::IsInsideArc(
			Double_t Oxx,
			Double_t Oyy,
			Short_t Charge,
			Double_t Xcross[2],
			Double_t Ycross[2],
			Double_t f  // f should be between 0 and 2PI.
			)
{

	Double_t f1,
		 f2;


	// Xcross[0],Ycross[0] is the point of entrance.


	f1 = atan2(Ycross[0]-Oyy, Xcross[0]-Oxx);
	if(f1<0.) f1+= 2.*PI;
	if(f1<0.) f1= 0.;
	f2 = atan2(Ycross[1]-Oyy, Xcross[1]-Oxx);
	if(f2<0.) f2+= 2.*PI;
	if(f2<0.) f2= 0.;



	if(Charge<0){
		if(f1 > f2 ) f2 +=2.*PI;
		if(f1 > f2 ) f2 = f1;
		if( f>f1){
			if(f<f2) return true; else return false;
		} else {
			f +=2.*PI;
			if(f<f1) f= f1;
			if(f<f2) return true; else return false;
		}
	} else {  // Charge > 0.
		if(f1 < f2 ) f1 +=2.*PI;
		if(f1 < f2 ) f1 = f2;
		if( f>f2){
			if(f<f1) return true; else return false;
		} else {
			f +=2.*PI;
			if(f<f2) f= f2;
			if(f<f1) return true; else return false;
		}
	}	// end of if(Charge<0)
}

//----------end of function PndSttMvdTracking::IsInsideCircle

//----------end of function PndSttMvdTracking::IsInTargetPipe
	bool PndSttMvdTracking::IsInTargetPipe(
			Double_t Oxx,
			Double_t Oyy,
			Double_t Rr,
			Double_t fi0,
			Double_t kappa,
			Short_t Charge,
			Double_t gap
						)
{
	UShort_t nintersections;

	Double_t	delta,
			XintersectionList[8],
			YintersectionList[8],
			Xcross[2],
			Ycross[2];

	//  find if this track lays in the target pipe volume
	//  for Radius < 15 (supposidly the maximu radius of the Mvd system)
	//  and consequently it cannot have Mvd hits.

	nintersections=0;

	// intersections of trajectory with plane X = gap;
	delta = Rr*Rr - (gap-Oxx)*(gap-Oxx);
	if(delta>0.) {
		XintersectionList[0] = gap;
		YintersectionList[0] = sqrt(delta) + Oyy;
		XintersectionList[1] = gap;
		YintersectionList[1] = -sqrt(delta) + Oyy;
		nintersections+=2;
	}

	// intersections of trajectory with plane X = -gap;
	delta = Rr*Rr - (-gap-Oxx)*(-gap-Oxx);
	if(delta>0.) {
		XintersectionList[nintersections] = -gap;
		YintersectionList[nintersections] = sqrt(delta) + Oyy;
		XintersectionList[nintersections+1] = -gap;
		YintersectionList[nintersections+1] = -sqrt(delta) + Oyy;
		nintersections+=2;
	}


	// intersections of trajectory with plane Z = gap;
		XintersectionList[nintersections] = Oxx + Rr*cos(fi0+kappa*gap);
		YintersectionList[nintersections] = Oyy + Rr*sin(fi0+kappa*gap);
		nintersections++;

	// intersections of trajectory with plane Z = -gap;
		XintersectionList[nintersections] = Oxx + Rr*cos(fi0-kappa*gap);
		YintersectionList[nintersections] = Oyy + Rr*sin(fi0-kappa*gap);
		nintersections++;

	ChooseEntranceExitbis(
				Oxx,
				Oyy,
				Charge,
				fi0,
				nintersections,// n. intersection in input.
				XintersectionList,
				YintersectionList,
				Xcross,	// output
				Ycross	// output
				);

	// here I suppose that the Mvd system CONSERVATIVELY ends at Rmax=5 cm.
	if( fabs(Ycross[0]) > 4. ) return true;
	else  return false;
}
//----------end of function PndSttMvdTracking::IsInTargetPipe

//----------begin of function PndSttMvdTracking::CalculateArcLength

	Double_t PndSttMvdTracking::CalculateArcLength(
			Double_t Oxx,
			Double_t Oyy,
			Double_t Rr,
			Short_t charge,
			Double_t Xcross[2], // entrance-exit point
			Double_t Ycross[2] // entrance-exit point
			)
{
	Double_t	dis,
			theta1,
			theta2;

	theta1 = atan2(Ycross[0]-Oyy,  Xcross[0]- Oxx);
	theta2 = atan2(Ycross[1]-Oyy,  Xcross[1]- Oxx);


	if(charge>0){  // the rotation was clockwise.
		dis = theta1-theta2;
	} else {  // the rotation was counterclockwise.
		dis = theta2-theta1;
	}

	if(dis<0.) dis += 2.*PI;
	if(dis<0.) dis =0.;

	dis *= Rr;

	return dis;
}
//----------end of function PndSttMvdTracking::CalculateArcLength


//----------begin of function PndSttMvdTracking::FindCharge

	void   PndSttMvdTracking::FindCharge(
		Double_t oX,
		Double_t oY,
		UShort_t nParallelHits,
		Double_t *X,
		Double_t *Y,
		Short_t  * Charge
				)
{

	UShort_t ihit,
			nleft,
			nright;

	Double_t cross,
		 disq,
		 minl,
		 minr;


	// this methods works with the hypothesis that this track comes
	//  from (0,0)

	for(ihit=0, nleft=0, nright=0, minr = 9999999., minl = 9999999.; ihit<nParallelHits; ihit++){ 
	// find the Z component of the cross product between the vector from (0,0) to center of
	// circular trajectory [namely, (oX,oY) ]  and the Position vector of the center of the
	// parallel Hits [namely, (x,y)].

		cross = oX*Y[ihit] -
			oY*X[ihit];

	// if  cross >0  hits stays 'on the left' (which means clockwise to go from the origin
	// to the hit following the smaller path) otherwise it stays 'on the right'.

		if (cross>0.) {
			disq =	X[ihit]*X[ihit]+Y[ihit]*Y[ihit];
			nleft++;
		} else {
			nright++;
		}
	}	// end of   for(ihit=0, nleft=0, nright=0;....

	if( nright> nleft) {
		*Charge = -1;
	} else if ( nleft > nright) {
		*Charge = 1;
	} else {	// then choose according the closest hit ti the center
		if( minr < minl ) *Charge = -1;
		else  *Charge = 1;
	}



}

//----------end of function PndSttMvdTracking::FindCharge



//----------begin of function PndSttMvdTracking::AssociateSciTilHit

	UShort_t PndSttMvdTracking::AssociateSciTilHit(
		Double_t Oxx,
		Double_t Oyy,
		Double_t Rr,
		UShort_t *List,  // output
		Double_t *esse  // output
				)
{

 bool intersect;

 UShort_t
	igoodScit,
	iScitHit,
	Nint;

 Double_t
	distance,
	QQ,
	sqrtRR,
	SIGN,
	XintersectionList[2],
	YintersectionList[2];


 igoodScit=0;
 for(iScitHit=0; iScitHit<nSciTilHits; iScitHit++){
	if(!InclusionListSciTil[iScitHit]) continue;

	IntersectionSciTil_Circle(
	  posizSciTil[iScitHit][0],
	  posizSciTil[iScitHit][1],
	  Oxx,
	  Oyy,
	  Rr,
	  &Nint,  // output
	  XintersectionList,  // output
	  YintersectionList  // output
	);

	if(intersect){
		List[igoodScit] = iScitHit;

		// calculate S on the lateral face of the Helix.
		if ( Nint==1){	// the majority of the cases
			esse[igoodScit] = atan2(YintersectionList[0]-Oyy,
				XintersectionList[0]-Oxx);
		} else {  // in this case Nint=2 (it should be a very rare case).
			// do an average of the two positions.
			esse[igoodScit] = atan2( 0.5*(YintersectionList[0]+
					YintersectionList[1])-Oyy,
				0.5*(XintersectionList[0]+
					XintersectionList[1])-Oxx);
		} // end of  if ( Nint==1)
		if ( esse[igoodScit]<0.) esse[igoodScit] += 2.*PI;
		igoodScit++;
		if( igoodScit == nmaxSciTilHitsinTrack) break;

	} // end of  if(intersect && distance<olddist)
 }  // end of  for(iScitHit=0; iScitHit<nScitHits; iScitHit++)

 return igoodScit;

}

//----------end of function PndSttTrackFinderReal::AssociateSciTilHit


//----------begin of function PndSttMvdTracking::IntersectionSciTil_Circle
	bool  PndSttMvdTracking::IntersectionSciTil_Circle(
			Double_t posizSciTilx,
			Double_t posizSciTily,
			Double_t Oxx, // center of circle.
			Double_t Oyy,
			Double_t Rr, // Radius of circle.
			UShort_t * Nintersections,
			Double_t XintersectionList[2],
			Double_t YintersectionList[2]
							)
{

	bool intersect;

	Double_t
		distance,
		QQ,
		sqrtRR,
		SIGN;


	QQ = posizSciTilx*posizSciTilx+posizSciTily*posizSciTily;
	sqrtRR=sqrt(QQ);

	if( posizSciTily<0.)  SIGN=-1.;
	else  SIGN=1.;


	intersect = IntersectionCircle_Segment(
	  posizSciTilx,
	  posizSciTily,
	  -QQ,
	  posizSciTilx-SIGN*0.5*DIMENSIONSCITIL*posizSciTily/sqrtRR,
	  posizSciTilx+SIGN*0.5*DIMENSIONSCITIL*posizSciTily/sqrtRR,
	  posizSciTily-SIGN*posizSciTilx*0.5*DIMENSIONSCITIL/sqrtRR,
	  posizSciTily+SIGN*posizSciTilx*0.5*DIMENSIONSCITIL/sqrtRR,
	  Oxx,
	  Oyy,
	  Rr,
	  Nintersections,  // output
	  XintersectionList,  // output
	  YintersectionList,  // output
	  &distance  // output
						);

	return intersect;
}
//----------end of function PndSttMvdTracking::IntersectionSciTil_Circle





//----------begin of function PndSttMvdTracking::stampetta

	void PndSttMvdTracking::stampetta(
			UShort_t nSttTrackCand,
			bool *keepit
					)
{
	int i,j;

           cout<<"da PndSttMvdTracking, evt. "<<IVOLTE<<",  n. SttTrackCand totali = "
	   <<nSttTrackCand<<endl;
      for(  i= 0; i< nSttTrackCand; i++){
	   cout<<"\tcandidate n. "<<i<<", keepit = ";
	   if(keepit[i]){ cout<<"true "; }else {cout<<"false ";}
	   cout<<"-----------------\n";
      	if(!keepit[i]) continue;
	   cout<<"da PndSttMvdTracking --------------------------------------\n"<<
	   "	SttTrackCand n.  "<<i<<";  n. Hits in Pixels associati = "
	   <<nMvdPixelHitsinTrack[i]<<"   e loro lista \n";
	   for(j=0; j<nMvdPixelHitsinTrack[i];j++){
		cout<<"\t\tMvd Pixel Hit n. "<<
		     ListMvdPixelHitsinTrack[i][j]<<endl;
	   }
           cout<<"da PndSttMvdTracking --------------------------------------\n"<<
	   "	SttTrackCand n.  "<<i<<";  n. Hits in Strips associati = "
	   <<nMvdStripHitsinTrack[i]<<"   e loro lista \n";
	   for(j=0; j<nMvdStripHitsinTrack[i];j++){
	        cout<<"\t\tMvd Strip Hit n. "<<
		     ListMvdStripHitsinTrack[i][j]<<endl;
	   }
           cout<<"da PndSttMvdTracking --------------------------------------\n"<<
	   "	SttTrackCand n.  "<<i<<";  n. || Hits in Stt in Track = "
	   <<nSttParHitsinTrack[i]<<"   e loro lista \n";
	   for(j=0; j<nSttParHitsinTrack[i];j++){
	        cout<<"\t\t|| Stt Hit n. "<<
		     ListSttParHitsinTrack[i][j]<<endl;
	   }
           cout<<"da PndSttMvdTracking --------------------------------------\n"<<
	   "	SttTrackCand n.  "<<i<<";  n. skew Hits in Stt Track = "
	   <<nSttSkewHitsinTrack[i]<<"   e loro lista \n";
	   for(j=0; j<nSttSkewHitsinTrack[i];j++){
	        cout<<"\t\tskew Stt Hit n. "<<
		     ListSttSkewHitsinTrack[i][j]<<endl;
	   }
           cout<<"da PndSttMvdTracking --------------------------------------\n"<<
	   "	SttTrackCand n.  "<<i<<";  n. SciTil Hits in Stt Track = "
	   <<nSciTilHitsinTrack[i]<<"   e loro lista \n";
	   for(j=0; j<nSciTilHitsinTrack[i];j++){
	        cout<<"\t\tscitil Hit n. "<<
		     ListSciTilHitsinTrack[i][j]<<endl;
	   }
      }// end of   for(  i= 0; i< nSttTrackCand; i++)

	cout<<endl<<endl;
	return;

}


//----------end of function PndSttMvdTracking::stampetta






ClassImp(PndSttMvdTracking)
