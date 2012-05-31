#include "glpk.h"
#include "PndTrkTracking.h"
#include "PndTrkSttConformalFilling.h"
#include "PndTrkGlpkFits.h"
#include "PndTrkCTFindTrackInXY.h"
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




// definition of constants used in this class

// integer constants

#define MAXHITSINCELL		20
// the following MAXHITSINFIT cannot be too large because the fit
// crashes 'silently' for too much memory consumption in the character arrays
// or takes too long time;
#define MAXHITSINFIT		12
#define MAXMVDMCPOINTS		2000
#define MINIMUMHITSPERTRACK	3
#define MINOUTERHITSPERTRACK	5
#define TIMEOUT			60



// floating point constants

#define APOTEMAMAXINNERPARSTRAW	23.246827
#define APOTEMAMAXSKEWSTRAW	31.517569 // delimitation of the skew area
#define APOTEMAMINOUTERPARSTRAW	31.863369
#define APOTEMAMINSKEWSTRAW	23.246827 // delimitation of the skew area
#define BFIELD			2.  // in Tesla
#define CVEL			2.99792  //  velocity of light
#define DELTAnR			2.   //range of nR in TrkAssociatedParallelHitsToHelixquater
#define DIAMETERSTRAWTUBE	1.
#define DIMENSIONSCITIL		2.85 // cm
#define ERRORPIXEL		0.02611
#define ERRORSTRIP		0.02611
#define ERRORSQPIXEL		0.00068175
#define ERRORSQSTRIP		0.00068175
#define PI			3.141592654
#define PMAX			100.
#define RSTRAWDETECTORMAX	40.73 // maximum radius of the Stt detector in  cm
#define RSTRAWDETECTORMIN	16.119 // minimum radius of the Stt detector in  cm
#define STRAWRADIUS		0.5
#define STRAWRESOLUTION		0.015
#define STRAW_SKEW_INCLINATION	3.
#define STTDRIFTVEL		0.0025	//   in cm/nsec
#define VERTICALGAP		4. // (cm) gap between Left and Right sections of detector.



using namespace std;


// -----   Default constructor   -------------------------------------------
PndTrkTracking::PndTrkTracking() : FairTask("Tracking") { 
 fPersistence = kTRUE;
 istampa = 0;
 iplotta = false;
 doMcComparison = false;
 YesClean = false;
 YesCleanMvd = true;
 YesSciTil = false ;
 MvdAloneTracking = true;
 Initialization_ClassVariables();
 sprintf(fSttBranch,"STTHit");
 sprintf(fMvdPixelBranch,"MVDHitsPixel");
 sprintf(fMvdStripBranch,"MVDHitsStrip");
}
// -------------------------------------------------------------------------

PndTrkTracking::PndTrkTracking(Int_t verbose) : FairTask("Tracking") { 
 fPersistence = kTRUE;
 istampa = verbose;
 iplotta = false;
 doMcComparison = false;
 YesClean = false;
 YesCleanMvd = true;
 YesSciTil = false ;
 MvdAloneTracking = true;

 Initialization_ClassVariables();
 sprintf(fSttBranch,"STTHit");
 sprintf(fMvdPixelBranch,"MVDHitsPixel");
 sprintf(fMvdStripBranch,"MVDHitsStrip");
}
// -------------------------------------------------------------------------

PndTrkTracking::PndTrkTracking(int istamp, bool  iplot, bool imc)
		: FairTask("Tracking") { 
 fPersistence = kTRUE;
 istampa = istamp;
 iplotta = iplot;
 doMcComparison = imc;
 YesClean = false;
 YesCleanMvd = true;
 YesSciTil = false ;
 MvdAloneTracking = true;
 Initialization_ClassVariables();
 sprintf(fSttBranch,"STTHit");
 sprintf(fMvdPixelBranch,"MVDHitsPixel");

 sprintf(fMvdStripBranch,"MVDHitsStrip");
}


// -------------------------------------------------------------------------

PndTrkTracking::PndTrkTracking(int istamp, bool  iplot, bool imc, bool doSciTil)
		: FairTask("Tracking") { 
 fPersistence = kTRUE;
 istampa = istamp;
 iplotta = iplot;
 doMcComparison = imc;
 YesClean = false;
 YesCleanMvd = true;
 YesSciTil = doSciTil ;
 MvdAloneTracking = true;
 Initialization_ClassVariables();
 sprintf(fSttBranch,"STTHit");
 sprintf(fMvdPixelBranch,"MVDHitsPixel");

  sprintf(fMvdStripBranch,"MVDHitsStrip");
}
// -----   Destructor   ----------------------------------------------------
PndTrkTracking::~PndTrkTracking() {}
// -------------------------------------------------------------------------

//--------------- begin  PndTrkTracking::Initialization_ClassVariables

void PndTrkTracking::Initialization_ClassVariables()
{
// this is only for initializing the Class Variables.

	size_t len;


// booleans :
	len = sizeof(SingleHitListStt);
	memset (SingleHitListStt,true,len);

	len = sizeof(InclusionListSciTil);
	memset (InclusionListSciTil,true,len);

	len = sizeof(InclusionListStt);
	memset (InclusionListStt,true,len);

	len = sizeof(inMvdTrackCandPixel);
	memset (inMvdTrackCandPixel,false,len);

	len = sizeof(inMvdTrackCandStrip);
	memset (inMvdTrackCandStrip,false,len);

	len = sizeof(TypeConf);
	memset (TypeConf,false,len);

// char :


	len = sizeof(fSttBranch);
	memset (fSttBranch,0,len);

	len = sizeof(fMvdPixelBranch);
	memset (fMvdPixelBranch,0,len);

	len = sizeof(fMvdStripBranch);
	memset (fMvdStripBranch,0,len);

//  Short_t :

	nMCTracks=0;
	nSciTilHits=0;

	len = sizeof(ListMvdPixelHitsinTrack);
	memset (ListMvdPixelHitsinTrack,0,len);

	len = sizeof(ListMvdStripHitsinTrack);
	memset (ListMvdStripHitsinTrack,0,len);

	len = sizeof(ListSciTilHitsinTrack);
	memset (ListSciTilHitsinTrack,0,len);

	len = sizeof(ListSttParHits);
	memset (ListSttParHits,0,len);

	len = sizeof(ListSttParHitsinTrack);
	memset (ListSttParHitsinTrack,0,len);

	len = sizeof(ListSttSkewHitsinTrack);
	memset (ListSttSkewHitsinTrack,0,len);

	len = sizeof(ListSttSkewHits);
	memset (ListSttSkewHits,0,len);

	len = sizeof(ListSttSkewHitsinTrackSolution);
	memset (ListSttSkewHitsinTrackSolution,0,len);

	len = sizeof(ListTrackCandHit);
	memset (ListTrackCandHit,0,len);

	len = sizeof(nMvdPixelHitsinTrack);
	memset (nMvdPixelHitsinTrack,0,len);

	len = sizeof(nMvdStripHitsinTrack);
	memset (nMvdStripHitsinTrack,0,len);

	len = sizeof(nTrackCandHit);
	memset (nTrackCandHit,0,len);

	len = sizeof(nSciTilHitsinTrack);
	memset (nSciTilHitsinTrack,0,len);

	len = sizeof(nSttParHitsinTrack);
	memset (nSttParHitsinTrack,0,len);

	len = sizeof(nSttSkewHitsinTrack);
	memset (nSttSkewHitsinTrack,0,len);

//  Short_t :

	nMvdDSPixelHitNotTrackCand=0;
	nMvdDSStripHitNotTrackCand=0;
	nMvdPixelHit=0;
	nMvdStripHit=0;
	nMvdTrackCand=0;
	nMvdUSPixelHitNotTrackCand=0;
	nMvdUSStripHitNotTrackCand=0;

	len = sizeof(nHitMvdTrackCand);
	memset (nHitMvdTrackCand,0,len);

	len = sizeof(ListHitMvdTrackCand);
	memset (ListHitMvdTrackCand,0,len);

	len = sizeof(ListHitTypeMvdTrackCand);
	memset (ListHitTypeMvdTrackCand,0,len);

	len = sizeof(ListMvdDSPixelHitNotTrackCand);
	memset (ListMvdDSPixelHitNotTrackCand,0,len);

	len = sizeof(ListMvdUSPixelHitNotTrackCand);
	memset (ListMvdUSPixelHitNotTrackCand,0,len);

	len = sizeof(ListMvdDSStripHitNotTrackCand);
	memset (ListMvdDSStripHitNotTrackCand,0,len);

	len = sizeof(ListMvdUSStripHitNotTrackCand);
	memset (ListMvdUSStripHitNotTrackCand,0,len);

	len = sizeof(ListTrackCandHitType);
	memset (ListTrackCandHitType,0,len);

//  Double_t :

	Fimin=0.;
	SEMILENGTH_STRAIGHT=75.;
	ZCENTER_STRAIGHT=35.;

	len = sizeof(ALFA);
	memset (ALFA,0,len);

	len = sizeof(BETA);
	memset (BETA,0,len);

	len = sizeof(GAMMA);
	memset (GAMMA,0,len);

	len = sizeof(CxMC);
	memset (CxMC,0,len);

	len = sizeof(CyMC);
	memset (CyMC,0,len);

	len = sizeof(R_MC);
	memset (R_MC,0,len);

	len = sizeof(MCtruthTrkInfo);
	memset (MCtruthTrkInfo,0,len);

	len = sizeof(MCSkewAloneX);
	memset (MCSkewAloneX,0,len);

	len = sizeof(MCSkewAloneY);
	memset (MCSkewAloneY,0,len);

	len = sizeof(radiaConf);
	memset (radiaConf,0,len);

	len = sizeof(Ox);
	memset (Ox,0,len);

	len = sizeof(Oy);
	memset (Oy,0,len);

	len = sizeof(R);
	memset (R,0,len);

	len = sizeof(refindexMvdPixel);
	memset (refindexMvdPixel,0,len);

	len = sizeof(sigmaXMvdPixel);
	memset (sigmaXMvdPixel,0,len);

	len = sizeof(sigmaYMvdPixel);
	memset (sigmaYMvdPixel,0,len);

	len = sizeof(sigmaZMvdPixel);
	memset (sigmaZMvdPixel,0,len);

	len = sizeof(XMvdPixel);
	memset (XMvdPixel,0,len);

	len = sizeof(YMvdPixel);
	memset (YMvdPixel,0,len);

	len = sizeof(ZMvdPixel);
	memset (ZMvdPixel,0,len);

	len = sizeof(XMvdStrip);
	memset (XMvdStrip,0,len);

	len = sizeof(YMvdStrip);
	memset (YMvdStrip,0,len);

	len = sizeof(ZMvdStrip);
	memset (ZMvdStrip,0,len);

	len = sizeof(sigmaXMvdStrip);
	memset (sigmaXMvdStrip,0,len);

	len = sizeof(sigmaYMvdStrip);
	memset (sigmaYMvdStrip,0,len);

	len = sizeof(sigmaZMvdStrip);
	memset (sigmaZMvdStrip,0,len);

	len = sizeof(refindexMvdStrip);
	memset (refindexMvdStrip,0,len);

	len = sizeof(posizSciTil);
	memset (posizSciTil,0,len);

	len = sizeof(SciTilHitsXwithTrack);
	memset (SciTilHitsXwithTrack,0,len);

	len = sizeof(SciTilHitsYwithTrack);
	memset (SciTilHitsYwithTrack,0,len);


//  pointers :

	HANDLE=NULL;
	HANDLE2=NULL;
	HANDLEXYZ=NULL;
	PHANDLEX=NULL;
	PHANDLEY=NULL;
	PHANDLEZ=NULL;
	SHANDLEX=NULL;
	SHANDLEY=NULL;
	SHANDLEZ=NULL;

	hdeltaRPixel=NULL;
	hdeltaRStrip=NULL;
	hdeltaRPixel2=NULL;
	hdeltaRStrip2=NULL;
	fMCTrackArray=NULL;
	fSttTubeArray=NULL;
	fSttPointArray=NULL;
	fSttHitArray=NULL;
	fSttTrackArray=NULL;
	fSttTrackCandArray=NULL;
	fMvdPixelHitArray=NULL;
	fMvdStripHitArray=NULL;
	fMvdTrackCandArray=NULL;
	fSciTHitArray=NULL;
	fMvdMCPointArray=NULL;
	fSttMvdPndTrackCandArray=NULL;
	fSttMvdPndTrackArray=NULL;
	fSttParameters=NULL;

}

//--------------- end  PndTrkTracking::Initialization_ClassVariables


// -----   Public method Init   --------------------------------------------

InitStatus PndTrkTracking::Init() {


 IVOLTE=-1;

 SEMILENGTH_STRAIGHT = 75.;
 ZCENTER_STRAIGHT = 35.;


 if(iplotta){
  hdeltaRPixel = new TH1F("hdeltaRPixel", "distance MC Pixel point from trajectory in XY plane", 100, -1, 1);
  hdeltaRStrip = new TH1F("hdeltaRStrip", "distance MC Strip point from trajectory in XY plane", 100, -1, 1);
  hdeltaRPixel2 = new TH1F(
  "hdeltaRPixel2", "distance MC point from trajectory in XY plane (Pixels)", 100, -10, 10);
  hdeltaRStrip2 = new TH1F(
  "hdeltaRStrip2", "distance MC point from trajectory in XY plane (Strips)", 100, -10,10);
}


//  --------------------------- opening files for special purposes


if(istampa >=1 ){
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
    cout << "-E- PndTrkTracking::Init: "
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
	cout << "-E- PndTrkTracking::Init: No MCTrack array, return!"
	   << endl;
	return kERROR;
 }

//  -------------------------   get the SciTil hits
 if(YesSciTil) {
	fSciTHitArray = (TClonesArray*) ioman->GetObject("SciTHit");
 } else {
	fSciTHitArray = NULL;
 }
//---------------------------

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
	cout << "-W- PndTrkTracking::Init: "
	 << "No STTHit array, return!" << endl;
	return kERROR;
 }

//  -------------------------   get the Mvd hits
 fMvdPixelHitArray = (TClonesArray*) ioman->GetObject(fMvdPixelBranch);
//  fMvdPixelHitArray = (TClonesArray*) ioman->GetObject("MVDHitsPixel");
 if ( !fMvdPixelHitArray){
	cout << "-W- PndTrkTracking::Init: " << "No MVD Pixel hitArray, return!" <<endl;
	return kERROR;
 }
 fMvdStripHitArray = (TClonesArray*) ioman->GetObject(fMvdStripBranch);
//  fMvdStripHitArray = (TClonesArray*) ioman->GetObject("MVDHitsStrip");

 if ( !fMvdStripHitArray){
	cout << "-W- PndTrkTracking::Init: " << "No MVD Strip hitArray, return!" <<endl;
	return kERROR;
 }

//  -------------------------   get the Mvd track candidates

 fMvdTrackCandArray = (TClonesArray*) ioman->GetObject("MVDRiemannTrackCand");
 if ( !fMvdTrackCandArray){
	cout << "-W- PndTrkTracking::Init: " << "No MVD TrackCand Array, return!" <<endl;
	return kERROR;
 }

 cout << "-I- PndTrkTracking: Initialization successfull" << endl;

//  -------------------------   get the Mvd MC points

 fMvdMCPointArray = (TClonesArray*) ioman->GetObject("MVDPoint");
 if ( !fMvdMCPointArray){
	cout << "-W- PndTrkTracking::Init: " << "No MVD MC Point Array, return!" <<endl;
	return kERROR;
 }
 cout << "-I- PndTrkTracking: Initialization successfull" << endl;


//--------------------------------  output TClonesArrays ------------------------

 // Create and register output array for PndTrackCand of Stt+Mvd combined

 fSttMvdPndTrackCandArray = new TClonesArray("PndTrackCand");
 ioman->Register("SttMvdTrackCand","SttMvd",fSttMvdPndTrackCandArray, kTRUE);


 // Create and register output array for PndTrack of Stt+Mvd combined

 fSttMvdPndTrackArray = new TClonesArray("PndTrack");
 ioman->Register("SttMvdTrack","SttMvd",fSttMvdPndTrackArray, kTRUE);

//-----------------------


//   calculate the boundaries of the Box in Conformal Space, see Gianluigi logbook on pag. 210-211
 Short_t i;
 Double_t
	A,
	r1,
	r2;

 radiaConf[0] = 1./RSTRAWDETECTORMAX;
 r1 = RSTRAWDETECTORMIN;
 A = (RSTRAWDETECTORMAX - r1)/NRDIVCONFORMAL;
 if ( NRDIVCONFORMAL > 1 ) {
	for(i = 1; i< NRDIVCONFORMAL ; i++){
		r2 = r1 + A;
		radiaConf[NRDIVCONFORMAL-i] = 1./r2;
		r1=r2;
	}
 }




 return kSUCCESS;

}

// -------------------------------------------------------------------------




void PndTrkTracking::SetParContainers() {
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}




void PndTrkTracking::WriteHistograms(){

  TFile* file = FairRootManager::Instance()->GetOutFile();
  file->cd();
  file->mkdir("PndTrkTracking");
  file->cd("PndTrkTracking");
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
void PndTrkTracking::Exec(Option_t* opt) {

 bool
	flag,
	intersect,
	keepit[MAXTRACKSPEREVENT],
	Mvdhits[MAXTRACKSPEREVENT],
	outcome,
	status[MAXTRACKSPEREVENT],
	SttSZfit[MAXTRACKSPEREVENT];

 Short_t
	nalone,
	ncand,
	nhitsinfit,
	Nint,
	NNN,
	nRemainingCandidates,
	nXYZhits,
	nHitsInMCTrack[MAXTRACKSPEREVENT],
	nMCParalAlone[MAXTRACKSPEREVENT],
	nMCSkewAlone[MAXTRACKSPEREVENT],
	nParalCommon[MAXTRACKSPEREVENT],
	nSkewCommon[MAXTRACKSPEREVENT],
	nSkewHitsInMCTrack[MAXTRACKSPEREVENT],
	nSpuriParinTrack[MAXTRACKSPEREVENT],
	nSpuriSkewinTrack[MAXTRACKSPEREVENT],
	//  given a Hit number it gives its radial box number
	RConformalIndex[MAXSTTHITS],
	//  given a Hit number it gives its azimuthal box number
	FiConformalIndex[MAXSTTHITS],
	nTotalCandidates,
	tempore[MAXSTTHITS],
	TemporarySkewList[2*MAXSTTHITS][2],
	BigList[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	MCParalAloneList[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	MCSkewAloneList[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	// nBoxConformal,  first index -> radial divisions,
	// 2nd index -> azimuthal divisions; n. of hits falling in this cell.
	nBoxConformal[NRDIVCONFORMAL*NFIDIVCONFORMAL],
	ParalCommonList[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	ParSpuriList[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	SkewCommonList[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	SkewSpuriList[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	HitsinBoxConformal[MAXHITSINCELL*NRDIVCONFORMAL*NFIDIVCONFORMAL];

 Short_t
	i,
	iParHit,
	ipunto,
	j,
	k,
	kall,
	l,
	nFicell,
	nRcell,
	tubeID,
	Charge[MAXTRACKSPEREVENT],
	daTrackFoundaTrackMC[MAXTRACKSPEREVENT],
	resultFitSZagain[MAXTRACKSPEREVENT],
	statusflag[MAXTRACKSPEREVENT]
	;

 Int_t
	iaccept,
	nSttHit,
	nSttMCPoint,
	nSttParHit,
	nSttSkewHit,
	nSttTrackCand
	;

 Int_t	nrounds0,
	nrounds1,
	ListHits[MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK];

 Double_t
	Distance,
	Fi,
	Ptras,
	ddd,
	delta,
	dis,
	dista,
	dista0,
	dista1,
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
//
	AloneX[MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK],
	AloneY[MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK],
	DriftRadiusbis[2*MAXSTTHITSINTRACK+MAXMVDPIXELHITSINTRACK+
		MAXMVDSTRIPHITSINTRACK+MAXSCITILHITSINTRACK],// all skew hits have double
	ErrorchosenPixel[MAXMVDPIXELHITS],
	ErrorchosenStrip[MAXMVDSTRIPHITS],
	ErrorchosenSkew[MAXSTTHITS],
	ErrorDriftRadiusbis[2*MAXSTTHITSINTRACK+MAXMVDPIXELHITSINTRACK+
				MAXMVDSTRIPHITSINTRACK+2],// solutions
	FI0[MAXTRACKSPEREVENT],
	Fi_final_helix_referenceframe[MAXTRACKSPEREVENT],
	Fi_initial_helix_referenceframe[MAXTRACKSPEREVENT],
	Fi_low_limit[MAXTRACKSPEREVENT],
	Fi_up_limit[MAXTRACKSPEREVENT],
	KAPPA[MAXTRACKSPEREVENT],
	Posiz1[3],
	primoangolo[MAXTRACKSPEREVENT],
	s[2],
	Sbis[2*MAXSTTHITSINTRACK+MAXMVDPIXELHITSINTRACK
		+MAXMVDSTRIPHITSINTRACK+MAXSCITILHITSINTRACK], // multiplication by 2 in the
	SchosenPixel[MAXTRACKSPEREVENT][MAXMVDPIXELHITS],
	SchosenStrip[MAXTRACKSPEREVENT][MAXMVDSTRIPHITS],
	SchosenSkew[MAXTRACKSPEREVENT][MAXSTTHITS], // NO multiplication by 2 here because for the
					// skew hits only one
					// solution is selected.
	Start[3],
	TemporaryS[2*MAXSTTHITS],
	TemporaryZ[2*MAXSTTHITS],
	TemporaryZDrift[2*MAXSTTHITS],
	TemporaryZErrorafterTilt[2*MAXSTTHITS],
	temporeZErrorafterTilt[MAXSTTHITSINTRACK],
	temporeS[MAXSTTHITSINTRACK],
	temporeZ[MAXSTTHITSINTRACK],
	temporeZDrift[MAXSTTHITSINTRACK],
	tmpErrorZDrift[MAXSTTHITSINTRACK+MAXSCITILHITSINTRACK],
	tmpS[MAXSTTHITSINTRACK+MAXSCITILHITSINTRACK],
	tmpZ[MAXSTTHITSINTRACK+MAXSCITILHITSINTRACK],
	tmpZDrift[MAXSTTHITSINTRACK+MAXSCITILHITSINTRACK],
	trajectory_vertex[2],
	Trajectory_Start[MAXTRACKSPEREVENT][2],
	ultimoangolo[MAXTRACKSPEREVENT],
	versor[2],
	WDX[MAXSTTHITS],
	WDY[MAXSTTHITS],
	WDZ[MAXSTTHITS],
	X[MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK+MAXSCITILHITSINTRACK],
	XintersectionList[2],
	Y[MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK+MAXSCITILHITSINTRACK],
	YintersectionList[2],
	z[2],
	ZEDbis[2*MAXSTTHITSINTRACK+MAXMVDPIXELHITSINTRACK
		+MAXMVDSTRIPHITSINTRACK+MAXSCITILHITSINTRACK], // rather improbable chance that
	zdrift[2],
	zerror[2],
	ZDrift[2*MAXSTTHITS],
	ZErrorafterTilt[2*MAXSTTHITS],
	zeta0,
	zeta1,
//
	info[MAXSTTHITS][7],
	infoparalConformal[MAXSTTHITS][5],
	Sfinal[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	XY[MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK][2],
	ZchosenPixel[MAXTRACKSPEREVENT][MAXMVDPIXELHITS],
	ZchosenStrip[MAXTRACKSPEREVENT][MAXMVDSTRIPHITS],
	ZchosenSkew[MAXTRACKSPEREVENT][MAXSTTHITS],
	Zfinal[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	ZDriftfinal[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	ZErrorafterTiltfinal[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK];

 TVector3
	ErrMomentum,
	ErrPosition,
	Momentum,
	Position;

 FairMCPoint *puntator;

 PndMCTrack* pMCtr;

 PndSttHit *pSttHit;

 PndSttTube *pSttTube;

 PndSdsHit
	*pMvdPixelHit,
	*pMvdStripHit;


//------------------------------------

 IVOLTE++;

 if(istampa>0)
	cout<<endl<<"Entering in PndTrack : evt (starting from 0)  n. "<<IVOLTE<<endl;

//------------------------------------

// -------------------------------------  fetch info from MVD

 nMvdPixelHit = fMvdPixelHitArray->GetEntriesFast();
 nMvdStripHit = fMvdStripHitArray->GetEntriesFast();
 if(nMvdPixelHit>MAXMVDPIXELHITS){
	cout<<"from PndTrkTracking, nMvdPixelHit is > maximum allowed ("
	<<MAXMVDPIXELHITS<<") and therefore is set to "
	<<MAXMVDPIXELHITS<<endl;
	nMvdPixelHit=MAXMVDPIXELHITS;
 }
 if(nMvdStripHit>MAXMVDSTRIPHITS){
	cout<<"from PndTrkTracking, nMvdStripHit is > maximum allowed ("
	<<MAXMVDSTRIPHITS<<") and therefore is set to "
	<<MAXMVDSTRIPHITS<<endl;
	nMvdStripHit=MAXMVDSTRIPHITS;
 }

// ---------------------------------------------  extract info from HITS Pixel MVD

 for( i= 0; i< nMvdPixelHit; i++){
	pMvdPixelHit = (PndSdsHit *) fMvdPixelHitArray->At(i);
	TVector3 temp = pMvdPixelHit->GetPosition();
	XMvdPixel[i] = temp.X();
	YMvdPixel[i] = temp.Y();
	ZMvdPixel[i] = temp.Z();
	sigmaXMvdPixel[i] = pMvdPixelHit->GetDx();
	sigmaYMvdPixel[i] = pMvdPixelHit->GetDy();
	sigmaZMvdPixel[i] = pMvdPixelHit->GetDz();
	refindexMvdPixel[i] = pMvdPixelHit->GetRefIndex();
 }

// -------------------------------------------  extract info from HITS Strip MVD

 for( i= 0; i< nMvdStripHit; i++){
	pMvdStripHit = (PndSdsHit *) fMvdStripHitArray->At(i);
	TVector3 temp = pMvdStripHit->GetPosition();
	XMvdStrip[i] = temp.X();
	YMvdStrip[i] = temp.Y();
	ZMvdStrip[i] = temp.Z();
	sigmaXMvdStrip[i] = pMvdStripHit->GetDx();
	sigmaYMvdStrip[i] = pMvdStripHit->GetDy();
	sigmaZMvdStrip[i] = pMvdStripHit->GetDz();
	refindexMvdStrip[i] = pMvdStripHit->GetRefIndex();
 }



// ------------------------------------------ get info from trackcand  of MVD

  nMvdTrackCand = fMvdTrackCandArray->GetEntriesFast();


 if (nMvdTrackCand> MAXMVDTRACKSPEREVENT) {
	cout<<"da PndTrkTracking  :  N. of MvdTrackCand = "<<
	nMvdTrackCand<<" and it is > MAXMVDTRACKSPEREVENT (="<<MAXMVDTRACKSPEREVENT
	<<"),  therefore it is set to "<<MAXMVDTRACKSPEREVENT<<endl;
	nMvdTrackCand= MAXMVDTRACKSPEREVENT;
 }


 if(istampa>=3)cout<<"da PndTrkTracking  :  n. totale di Mvd track cand =    "
		<<nMvdTrackCand<<"  --------------"<<endl;


// -----------------------more info from Mvd trackcand.


 ExtractInfoFromMvdTrackCand();

// -----------------------------------



//---------------------------------------------   fetching the STT  MC points
 nSttMCPoint = fSttPointArray->GetEntriesFast();
 if (nSttMCPoint ==0){
	cout<<"da PndTrkTracking  :  N. di Stt MC points = 0"<<endl<<endl;
//	return;
 } else  if( nSttMCPoint>MAXSTTHITS){
	cout<<"da PndTrkTracking  :  N. di Stt MC points = "<<nSttMCPoint
	<<" and it is > MAXSTTHITS ("<<MAXSTTHITS<<")."<<endl;
 }



//----------   fetching the STT  hits

 nSttHit = fSttHitArray->GetEntriesFast();
 if (nSttHit ==0){
	cout<<"da PndTrkTracking  :  N. di Stt Hits = 0, return!"<<endl<<endl;
	return;
 } else if (nSttHit> MAXSTTHITS) {
	cout<<"da PndTrkTracking  :  N. di Stt Hits = "<<nSttHit
	     <<" and it is > MAXSTTHITS (="<<MAXSTTHITS
	     <<"), therefore consider only the first "<<MAXSTTHITS<<" hits"<<endl<<endl;
	nSttHit= MAXSTTHITS;
 }

 if (istampa >= 1) {
	cout<<"da PndTrkTracking  : evento (partendo da 0)  N. "<<
		IVOLTE<<endl<< "    N. totale Hits in STT  : "<<nSttHit<<endl;
 }




 nSttParHit=0; 
 nSttSkewHit=0;

 for( i= 0; i< nSttHit; i++){
	pSttHit = (PndSttHit *) fSttHitArray->At(i);
	// right way to extract the corrisponding MC point.
	ipunto= pSttHit->GetRefIndex();
	tubeID = pSttHit->GetTubeID();
	pSttTube = (PndSttTube *) fSttTubeArray->At(tubeID);
	TVector3 center = pSttTube->GetPosition();
	// drift radius
	Double_t dradius = pSttHit->GetIsochrone();
	// wire direction
	TVector3 wiredirection = pSttTube->GetWireDirection();

	if(wiredirection.Z() >=0.) {
		WDX[i] = wiredirection.X();
		WDY[i] = wiredirection.Y();
		WDZ[i] = wiredirection.Z();
	} else {
		WDX[i] = -wiredirection.X();
		WDY[i] = -wiredirection.Y();
		WDZ[i] = -wiredirection.Z();
	}
	info[i][0]= pSttTube->GetPosition().X();
	info[i][1]= pSttTube->GetPosition().Y();
	info[i][2]= pSttTube->GetPosition().Z();
	info[i][3]= dradius;
	info[i][4]= pSttTube->GetHalfLength();

//-----------------------------------------


	if(ipunto>=0) {
		puntator = (FairMCPoint*) fSttPointArray->At(ipunto);
		info[i][6]= puntator->GetTrackID();
	} else {
		info[i][6]= -10.;
	}

	if( fabs( WDX[i] )< 0.00001 && fabs( WDY[i] )< 0.00001 ){
		info[i][5]= 1.;
		ListSttParHits[nSttParHit]=i;
		nSttParHit++;
	} else {
		info[i][5]= 99.;// to signal that it is a skew straw.
		ListSttSkewHits[nSttSkewHit]=i;
		nSttSkewHit++;
	}




  }  //   end of for( i= 0; i< nSttHit; i++)


//	fill the inclusion list for Stt, include only first hit for those straws with
//	multiple hits.
 MakeInclusionListStt(nSttHit, info);

//-----------------------------------  end of exclusion of straws with multiple hits



//-------------------------------------------- fetch the SciTil hits

 if( fSciTHitArray != NULL){
	// number SciTil hits/event
	nSciTilHits = fSciTHitArray->GetEntriesFast();
	if(nSciTilHits>MAXSCITILHITS){
		cout<<"da PndTrkTracking  :  N. of SciTil Hits = "<<nSciTilHits
	     <<" and it is > MAXSCITILHITS (="<<MAXSCITILHITS
	     <<"), therefore consider only the first "<<MAXSCITILHITS <<" hits"<<endl<<endl;
		nSttHit= MAXSCITILHITS;
	}

	if(istampa>0)
		cout<<"da PndTrkTracking, event "<<IVOLTE<<", "<<nSciTilHits
		<<" SciTil hits presenti inizialmente."<<endl;
	if( nSciTilHits>0){
	 PndSciTHit *pPndSciTHit;
	 TVector3  posiz;
	 // the first SciTil hit; this cannot be duplicate hit by definition.
		pPndSciTHit = (PndSciTHit*) fSciTHitArray->At(0);
		posiz = pPndSciTHit->GetPosition();
		if(istampa>0)
			cout<<"da PndTrkTracking SciTil non purgati, Xpos "<<
			posiz.X()<<", Ypos "<<
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
		cout<<"da PndTrkTracking SciTil non purgati, Xpos "
			<<posiz.X()<<", Ypos "<<
		posiz.Y()<<", Zpos "<<posiz.Z()<<endl;
		// purging the duplicate SciTil hits.

	    flag = true;
	    for(k=0; k<iaccept; k++){
		if(
			(fabs(posiz.X() - posizSciTil[k][0])< 1.e-20)
					&&
			(fabs(posiz.Y() - posizSciTil[k][1])< 1.e-20)
					&&
			(fabs(posiz.Z() - posizSciTil[k][2])< 1.e-20)
		  ){
			flag=false;
			break;
		}  // end of if((fabs(posiz.X() - old...
	    } // end of for(k=0; k<iaccept; k++)
	    if(flag){
		posizSciTil[iaccept][0]=posiz.X();
		posizSciTil[iaccept][1]=posiz.Y();
		posizSciTil[iaccept][2]=posiz.Z();
		iaccept++;
	    }
	 }  // end of for(j=0; j<nSciTilHits; j++)
	 nSciTilHits=iaccept;

	 //  set the inclusion list of the SciTils to true.

	for(i=0;i<nSciTilHits;i++){
		InclusionListSciTil[i]=true;
	}

//	 memset(InclusionListSciTil,true,nSciTilHits);

	}  // end of if( nSciTilHits>0){



 }  // end of if( fSciTHitArray != NULL)


//-----------------------------------end fetching SciTil hits.



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

//        controllato finoaqui,tuto ok.

 trajectory_vertex[0]=trajectory_vertex[1]=0.;

 PndTrkSttConformalFilling fill;

 fill.FromXYtoConformal(
	trajectory_vertex,
	info,
	ListSttParHits,
	nSttParHit,
	infoparalConformal,
	STRAWRADIUS
		);

 fill.BoxConformalFilling(
	FiConformalIndex,
	HitsinBoxConformal,
	InclusionListStt,
	infoparalConformal,
	ListSttParHits,
	MAXHITSINCELL,
	nBoxConformal,
	NFIDIVCONFORMAL,
	nSttParHit,
	NRDIVCONFORMAL,
	radiaConf,
	RConformalIndex
			);


 nSttTrackCand=0;    // # tracks found



//----- use the SciTil hits first;


 //  U and V only for the parallel Stt hits.

 Double_t
	U[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	V[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK];

// tolta momentaneamente la parte che che usa gli scitil per primi.


//----- loop over the parallel hits

 // class that finds the track (Stt hits only) in XY projection
 PndTrkCTFindTrackInXY SttTrackXYFinder;

 //  struct  necessary to pass all the parametrs to the PndTrkCTFindTrackInXY::FindTrackInXYProjection
 //  method. Since these parameters are > 60, cint does NOT accept to pass them in the usual
 //  way (parameters in the calling sequence) to PndTrkCTFindTrackInXY::FindTrackInXYProjection.

 FindTrackInXYProjection_InputData input;

 // loading the struct

 input.apotemamaxskewstraw = APOTEMAMAXSKEWSTRAW;
 input.deltanr = DELTAnR;
 input.dimensionscitil = DIMENSIONSCITIL;
 input.FiConformalIndex = FiConformalIndex;
 input.HitsinBoxConf = HitsinBoxConformal;
 input.InclusionListStt = InclusionListStt;
 input.InclusionListSciTil = InclusionListSciTil;
 input.info = info;
 input.infoparalConformal = infoparalConformal;
 input.ListSttParHits = ListSttParHits;
 input.maxhitsinfit = MAXHITSINFIT;
 input.maxscitilhitsintrack = MAXSCITILHITSINTRACK;
 input.maxstthits = MAXSTTHITS;
 input.maxstthitsintrack = MAXSTTHITSINTRACK;
 input.minimumhitspertrack = MINIMUMHITSPERTRACK;
 input.minouterhitspertrack = MINOUTERHITSPERTRACK;
 input.nBoxConf = nBoxConformal;
 input.nFicell = nFicell; // Fi cell of the seed hit;
 input.nfidivconformal = NFIDIVCONFORMAL;
 input.nrdivconformal = NRDIVCONFORMAL;
 input.nSciTilHits = nSciTilHits;
 input.nsttparhit = nSttParHit;
 input.posizSciT = posizSciTil;
 input.radiaConf = radiaConf;
 input.RConformalIndex = RConformalIndex;
 input.rstrawdetectormax = RSTRAWDETECTORMAX;
 input.rstrawdetectormin = RSTRAWDETECTORMIN;
 input.strawradius = STRAWRADIUS;
 input.trajectory_vertex = trajectory_vertex;
 input.YesSciTil = YesSciTil;

//----------------------



//   begins the first iteration with more severe cuts on the # hits in track candidate
 for(iParHit=0; iParHit<nSttParHit + 1 -  MINIMUMHITSPERTRACK ; iParHit++) {

	if( nSttTrackCand > MAXTRACKSPEREVENT-1) break;
	if( ! InclusionListStt[ListSttParHits[iParHit]] )  continue;

	// inputs for the FindTrackInXYProjection class;
	nRcell = RConformalIndex[ListSttParHits[iParHit]];
	nFicell = FiConformalIndex[ListSttParHits[iParHit]];

	// output results from FindTrackInXYProjection will be stored here;

	input.ALFA = &ALFA[nSttTrackCand];
	input.BETA = &BETA[nSttTrackCand];
	input.Charge = &Charge[nSttTrackCand];
	input.Fi_final_helix_referenceframe = &Fi_final_helix_referenceframe[nSttTrackCand];
	input.Fi_initial_helix_referenceframe = &Fi_initial_helix_referenceframe[nSttTrackCand];
	input.Fi_low_limit = &Fi_low_limit[nSttTrackCand];
	input.Fi_up_limit = &Fi_up_limit[nSttTrackCand];
	input.GAMMA = &GAMMA[nSttTrackCand];
	input.iHit = iParHit;	// seed hit; it is negative for SciTil Hits.
	input.ListHitsinTrack = &ListSttParHitsinTrack[nSttTrackCand][0];
	input.ListSciTilHitsinTrack = &ListSciTilHitsinTrack[nSttTrackCand][0];
	input.nFicell = nFicell;  // Fi cell of the seed hit;
	input.nHitsinTrack = &nSttParHitsinTrack[nSttTrackCand];
	input.nRcell = nRcell;  // R cell of the seed hit;
	input.nSciTilHitsinTrack = &nSciTilHitsinTrack[nSttTrackCand];
	input.Oxx = &Ox[nSttTrackCand];
	input.Oyy = &Oy[nSttTrackCand];
	input.Rr = &R[nSttTrackCand];
	input.S_SciTilHitsinTrack = &S_SciTilHitsinTrack[nSttTrackCand][0];
	input.TypeConf = &TypeConf[nSttTrackCand];
	input.U = &U[nSttTrackCand][0];
	input.V = &V[nSttTrackCand][0];

	outcome = SttTrackXYFinder.FindTrackInXYProjection(&input);

	if(!outcome)  continue;


// --------  here the track and its hits were found, filling the Inclusion list

 for(j=0; j<nSttParHitsinTrack[nSttTrackCand]; j++){
   InclusionListStt[ListSttParHitsinTrack[nSttTrackCand][j]] = false;
 }
 keepit[nSttTrackCand]=true;




 nSttTrackCand++;

  }      // end  of   for(iParHit=0; iParHit<nSttParHit+1-MINIMUMHITSPERTRACK; iParHit++)






//------------------------------------------------------  end of skew hits section





 return;

}


//---------------------- end of   PndTrkTracking::Exec



//----------------  begin of function PndTrkTracking::ExtractInfoFromMvdTrackCand

void PndTrkTracking::ExtractInfoFromMvdTrackCand()
{

  Short_t
	i,
	j,
	k;

  Double_t qop;

  PndTrackCand* pMvdTrackCand;

  PndTrackCandHit pndtrackcandhit; 

  for( i= 0; i< nMvdTrackCand ; i++){

	pMvdTrackCand= (PndTrackCand*) fMvdTrackCandArray->At(i);
	TVector3 dirSeed=pMvdTrackCand->getDirSeed();
	TVector3 posSeed=pMvdTrackCand->getPosSeed();
	qop = pMvdTrackCand->getQoverPseed();
	// n. hits in this track cand
	nHitMvdTrackCand[i] = pMvdTrackCand->GetNHits();
	if( nHitMvdTrackCand[i]>MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK){
		cout<<"from PndTrkTracking, nHitMvdTrackCand[i] = "<<nHitMvdTrackCand[i]
		<<" and it is > MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK (="
		<<MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK
		<<"); setting nHitMvdTrackCand[i] to "
		<<MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK<<endl;
	nHitMvdTrackCand[i]=MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK;
	}

	Short_t kPixel,kStrip;

	for(j=0, k=0, kPixel=0, kStrip=0; j<nHitMvdTrackCand[i]; j++){
		pndtrackcandhit = pMvdTrackCand->GetSortedHit(j);

	// the following case should never happen (in principle), but, just to be on
	// the safe side ....
		if(pndtrackcandhit.GetHitId()<0||pndtrackcandhit.GetDetId()<0)continue;

	// this is a Pixel.
	if( pndtrackcandhit.GetDetId()==
		FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)
			&& kPixel < MAXMVDPIXELHITSINTRACK){

		// the following is a protection because the maximum Mvd hit n.
		// cannot exceed MAXMVDPIXELHITS .
		if( pndtrackcandhit.GetHitId()>MAXMVDPIXELHITS ){
			cout<<"from PndTrkTracking, this Pixel Mvd hit has a number = "
			<<pndtrackcandhit.GetHitId()<<
			" that is > MAXMVDPIXELHITS (="<<
				MAXMVDPIXELHITS<<"), rejected!"<<endl;
			continue;
		}


		inMvdTrackCandPixel[ pndtrackcandhit.GetHitId() ]= true;
		ListHitTypeMvdTrackCand[i][k] = pndtrackcandhit.GetDetId();
					  // this in reality is the Branch name of
					  // Hit; this Branch name is used to identify
					  // a Pixel.
					  // If it is -1 is supposed to be noise but
					  // a in which Pixel or Strip?? Mistery.
		kPixel++;

	// this is a Strip.
	} else if( pndtrackcandhit.GetDetId()==
		FairRootManager::Instance()->GetBranchId(fMvdStripBranch)
			&& kStrip < MAXMVDSTRIPHITSINTRACK){

		// the following is a protection because the maximum Mvd hit n. cannot
		// exceed MAXMVDSTRIPHITS .
		if( pndtrackcandhit.GetHitId()>MAXMVDSTRIPHITS ){
			cout<<"from PndTrkTracking, this Strip Mvd hit has a number = "
			<<pndtrackcandhit.GetHitId()<<
			" that is > MAXMVDSTRIPHITS (="<<
			MAXMVDSTRIPHITS<<"), rejected!"<<endl;
			continue;
		}

		inMvdTrackCandStrip[ pndtrackcandhit.GetHitId() ]= true;
		ListHitTypeMvdTrackCand[i][k] = pndtrackcandhit.GetDetId();
					  // this in reality is the Branch name of
					  // Hit; this Branch name is used to identify
					  // a Pixel.
					  // If it is -1 is supposed to be noise but
					  // a in which Pixel or Strip?? Mistery.
		kStrip++;
	} else {	// this is the case should (in principle) never happen.
		continue;	// ignore this hit.
	}

	// the following is the native Hit number that one can use
	// to extract all the info. If it is -1 I won't consider it (noise).
	ListHitMvdTrackCand[i][k] = pndtrackcandhit.GetHitId();

	k++;

     }    //   end of    for(j=0; j<nHitMvdTrackCand; j++)

     nHitMvdTrackCand[i]=k; // if the case, readjust the # of hits in this candidate.

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

 
 }
//-----------------------  end of function   PndTrkTracking::ExtractInfoFromMvdTrackCand


//--------------------------------  begin function   PndTrkTracking::MakeInclusionListStt


void PndTrkTracking::MakeInclusionListStt(
	Int_t nSttHit,
	Double_t info[][7]
	)
{

 int i,j;

// it needs to be initialized for each event !
 for(i=0;i<nSttHit;i++){
	InclusionListStt[i] = true;
	SingleHitListStt[i] = true;
 }



//	fill the inclusion list for Stt, include only first hit for those straws with
//	multiple hits.


 for(i=0; i< nSttHit-1; i++){
	if( !InclusionListStt[ i ] ) continue;
	for(j=i+1; j< nSttHit; j++){
		if(InclusionListStt[ j ] &&
			fabs(info[i][0] - info[j][0])<1.e-20 &&
			fabs(info[i][1] - info[j][1])<1.e-20  )
		{
			SingleHitListStt[j]=InclusionListStt[j]= false ;
		}
	} //  end of  for(j=i+1; j< Nhits;; j++)


 }   //   end of for(i=0; i< Nhits-1; i++)




}
//--------------------------------  end of function   PndTrkTracking::MakeInclusionListStt




ClassImp(PndTrkTracking)

