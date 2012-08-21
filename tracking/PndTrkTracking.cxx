#include "glpk.h"

#include "PndTrkTracking.h"
#include "PndTrkComparisonMCtruth.h"
#include "PndTrkSttConformalFilling.h"
#include "PndTrkGlpkFits.h"
#include "PndTrkCTFindTrackInXY.h"
#include "PndTrkCTGeometryCalculations.h"
#include "PndTrkMergeSort.h"
#include "PndTrkPlotMacros.h"
#include "PndTrkPrintouts.h"

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



//  pointers :

	HANDLE=NULL;
	HANDLE2=NULL;

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


if(doMcComparison >=1 ){
//---- apertura file con info su Found tracce su cui si fa Helix fit dopo
   HANDLE2 = fopen("info_da_PndTrackFinderReal.txt","w");

//  ---- open filehandle per statistica sugli hits etc.
   HANDLE = fopen("statistiche.txt","w");
//  ---------------


}  //  end of if(istampa >=1)


// -------------------------



 // Get RootManager
 FairRootManager* ioman = FairRootManager::Instance();
// ioman = FairRootManager::Instance();
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

//  -------------------------   get the SciTil MC Points
 if(YesSciTil && doMcComparison) {
	fSciTPointArray = (TClonesArray*) ioman->GetObject("SciTPoint");
 } else {
	fSciTPointArray = NULL;
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
	npixelhitsintrack,
	nSkewCommon[MAXTRACKSPEREVENT],
	nSkewHitsInMCTrack[MAXTRACKSPEREVENT],
	nSpuriParinTrack[MAXTRACKSPEREVENT],
	nSpuriSkewinTrack[MAXTRACKSPEREVENT],
	nstriphitsintrack,
	//  given a Hit number it gives its radial box number
	RConformalIndex[MAXSTTHITS],
	//  given a Hit number it gives its azimuthal box number
	FiConformalIndex[MAXSTTHITS],
	nTotalCandidates,
	tempore[MAXSTTHITS],
	TemporarySkewList[2*MAXSTTHITS][2],
	BigList[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	// nBoxConformal,  first index -> radial divisions,
	// 2nd index -> azimuthal divisions; n. of hits falling in this cell.
	nBoxConformal[NRDIVCONFORMAL*NFIDIVCONFORMAL],
	ParalCommonList[MAXTRACKSPEREVENT*MAXSTTHITSINTRACK],
	ParSpuriList[MAXTRACKSPEREVENT*MAXSTTHITSINTRACK],
	SkewCommonList[MAXTRACKSPEREVENT*MAXSTTHITSINTRACK],
	SkewSpuriList[MAXTRACKSPEREVENT*MAXSTTHITSINTRACK],
	HitsinBoxConformal[MAXHITSINCELL*NRDIVCONFORMAL*NFIDIVCONFORMAL];

 Short_t
	i,
	iParHit,
	ipunto,
	j,
	k,
	kall,
	l,
	tubeID,
	Charge[MAXTRACKSPEREVENT],
	daTrackFoundaTrackMC[MAXTRACKSPEREVENT],
	resultFitSZagain[MAXTRACKSPEREVENT],
	statusflag[MAXTRACKSPEREVENT]
	;

 Int_t
	iaccept,
	len,
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

 PndTrkCTGeometryCalculations GeomCalculator;

 PndTrkPrintouts fPrint;

//------------------------------------

 IVOLTE++;

 if(istampa>0)
	cout<<endl<<"Entering in PndTrkTrack : evt (starting from 0)  n. "<<IVOLTE<<endl;

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


// ---------------   printout of Mvd Hits;
 if(istampa>=1)
	fPrint.stampaMvdHits(
		fMvdPixelBranch,
		fMvdStripBranch,
		nMvdPixelHit,
		nMvdStripHit,
		refindexMvdPixel,
		refindexMvdStrip,
		sigmaXMvdPixel,
		sigmaXMvdStrip,
		sigmaYMvdPixel,
		sigmaYMvdStrip,
		sigmaZMvdPixel,
		sigmaZMvdStrip,
		XMvdPixel,
		XMvdStrip,
		YMvdPixel,
		YMvdStrip,
		ZMvdPixel,
		ZMvdStrip
			);


// ------------------------------------------ get info from trackcand  of MVD

  nMvdTrackCand = fMvdTrackCandArray->GetEntriesFast();


 if (nMvdTrackCand> MAXMVDTRACKSPEREVENT) {
	cout<<"da PndTrkTracking  :  N. of MvdTrackCand = "<<
	nMvdTrackCand<<" and it is > MAXMVDTRACKSPEREVENT (="<<MAXMVDTRACKSPEREVENT
	<<"),  therefore it is set to "<<MAXMVDTRACKSPEREVENT<<endl;
	nMvdTrackCand= MAXMVDTRACKSPEREVENT;
 }




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
	cout<<"da PndTrkTracking  :  n. totale Hits in STT  : "<<nSttHit<<endl;
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


 //  printout of the Stt hits;
 if (istampa >= 1 ) fPrint.stampaSttHits(i,ipunto,dradius,WDX,WDY,WDZ,puntator,pSttTube);

  }  //   end of for( i= 0; i< nSttHit; i++)

//	fill the inclusion list for Stt, include only first hit for those straws with
//	multiple hits.

 MakeInclusionListStt(nSttHit, info);

//-----------------------------------  end of exclusion of straws with multiple hits



//-------------------------------------------- fetch the SciTil hits
 nSciTilHits = 0;
 if( fSciTHitArray != NULL){
	// number SciTil hits/event
	nSciTilHits = fSciTHitArray->GetEntriesFast();
	if(nSciTilHits>MAXSCITILHITS){
		cout<<"da PndTrkTracking  :  N. of SciTil Hits = "<<nSciTilHits
	     <<" and it is > MAXSCITILHITS (="<<MAXSCITILHITS
	     <<"), therefore consider only the first "<<MAXSCITILHITS <<" hits"<<endl<<endl;
		nSciTilHits= MAXSCITILHITS;
	}
 }  // end of if( fSciTHitArray != NULL)

 // it is important that fSciTilMaxNumber and OriginalSciTilList have a scope
 // extending in all   exec  method; that's why they are stated here, out
 // of the if( fSciTHitArray != NULL).
 Short_t fSciTilMaxNumber;
 if( nSciTilHits > 0) fSciTilMaxNumber = nSciTilHits ; else fSciTilMaxNumber = 1;

 Short_t
	nHitsInSciTile[fSciTilMaxNumber],
	OriginalSciTilList[fSciTilMaxNumber][fSciTilMaxNumber];

//--------   initialization (to 0) of nHitsInSciTile array;
 memset (nHitsInSciTile,0,sizeof(nHitsInSciTile));
//---

 if(istampa>0) cout<<"da PndTrkTracking, event "<<IVOLTE<<", "<<nSciTilHits
		<<" SciTil hits presenti inizialmente."<<endl;
	for(j=0; j<nSciTilHits; j++){
	}

   if( nSciTilHits >0 ){
	// OriginalSciTilList is the list of original SciTil hits (not purged yet)
	// present in a given SciTil tile :
	// OriginalSciTilList[nacceptedhit][*];


	PndSciTHit *pPndSciTHit;
	TVector3  posiz;
	// the first SciTil hit; this cannot be duplicate hit by definition.
	// The Sci Tiles are numbered here according to the numbering
	// of the (first) SciTil Hit inside the Sci Tile.
	pPndSciTHit = (PndSciTHit*) fSciTHitArray->At(0);
	posiz = pPndSciTHit->GetPosition();
	if(istampa>0)cout<<"da PndTrkTracking SciTil n. "<<0<<
	" non purgato, Xpos "<<
			posiz.X()<<", Ypos "<<
			posiz.Y()<<", Zpos "<<posiz.Z()<<endl;

	posizSciTil[0][0]=posiz.X();
	posizSciTil[0][1]=posiz.Y();
	posizSciTil[0][2]=posiz.Z();
	pSciTilx[0]=posiz.X();
	pSciTily[0]=posiz.Y();
	pSciTilz[0]=posiz.Z();
	OriginalSciTilList[0][0]=0;
	nHitsInSciTile[0]=1;
	iaccept=1;
	// the other SciTil hits; purge them if they are duplicate.
	for(j=1; j<nSciTilHits; j++){
		pPndSciTHit = (PndSciTHit*) fSciTHitArray->At(j);
		posiz = pPndSciTHit->GetPosition();
		if(istampa>0)cout<<"da PndTrkTracking SciTil n. "<<j<<" non purgato, Xpos "
			<<posiz.X()<<", Ypos "<<posiz.Y()<<", Zpos "<<posiz.Z()<<endl;

	// purging the duplicate SciTil hits.
	    flag = true;
	    for(k=0; k<iaccept; k++){
		if(
			(fabs(posiz.X() - posizSciTil[k][0])< 1.e-20) &&
			(fabs(posiz.Y() - posizSciTil[k][1])< 1.e-20) &&
			(fabs(posiz.Z() - posizSciTil[k][2])< 1.e-20)
		  ){
			flag=false;
			OriginalSciTilList[k][nHitsInSciTile[k]]= j;
			nHitsInSciTile[k]++;
			break;
		}  // end of if((fabs(posiz.X() - old...
	    } // end of for(k=0; k<iaccept; k++)
	    if(flag){
		posizSciTil[iaccept][0]=posiz.X();
		posizSciTil[iaccept][1]=posiz.Y();
		posizSciTil[iaccept][2]=posiz.Z();
		pSciTilx[iaccept]=posiz.X();
		pSciTily[iaccept]=posiz.Y();
		pSciTilz[iaccept]=posiz.Z();
		OriginalSciTilList[iaccept][0]= j;
		nHitsInSciTile[iaccept]=1;
		iaccept++;
	    }
	 }  // end of for(j=0; j<nSciTilHits; j++)
	 nSciTilHits=iaccept;

	 //  set the inclusion list of the SciTils to true.

	for(i=0;i<nSciTilHits;i++){
		InclusionListSciTil[i]=true;
	}


//-----------stampe.
if(istampa>0){
  cout<<"da PndTrkTracking, dopo purga di SciTil; n. hits = "<<nSciTilHits<<endl;
  for(j=0; j<nSciTilHits; j++){
	cout<<"da PndTrkTracking SciTil Xpos "<<posizSciTil[j][0]<<", Ypos "<<
	posizSciTil[j][1]<<", Zpos "<<posizSciTil[j][2]<<endl;
  }
}
//---------- fine stampe.



   }  // end of if( nSciTilHits >0 )


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


 //  initialization of the (assumed) starting point of the tracks, at the origin;
 //  needs to be modified later;

 trajectory_vertex[0]=trajectory_vertex[1]=0.;
 len = sizeof(Trajectory_Start);
 memset (Trajectory_Start,0,len);
//--------------


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

//-----
 // class that finds the track (Stt hits only) in XY projection
 PndTrkCTFindTrackInXY SttTrackXYFinder;

 //  struct  necessary to pass all the parametrs to the PndTrkCTFindTrackInXY::FindTrackInXYProjection
 //  method. Since these parameters are > 60, cint does NOT accept to pass them in the usual
 //  way (parameters in the calling sequence) to PndTrkCTFindTrackInXY::FindTrackInXYProjection.
 FindTrackInXYProjection_InputData input;

 // loading those elements of the struct common to all the candidate tracks.
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





//----- loop over the SciTil hits first;


 //  U and V only for the parallel Stt hits.

 Double_t
	U[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	V[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK];



 if(YesSciTil) {
    for(i=0; i<nSciTilHits ; i++) {
	if( nSttTrackCand >= MAXTRACKSPEREVENT ){
		cout<<"from PndTrkTracking :  # n. Tracks found so far = "
		<<nSttTrackCand<<" and it is >= MAXTRACKSPEREVENT ( = "
		 <<MAXTRACKSPEREVENT<<"); exiting from SciTil loop.\n";
		break ;
	}

	// inputs for the FindTrackInXYProjection class;
	input.iHit = -i-1;	// seed hit; it is negative for SciTil Hits.
	input.nRcell = -1;  // R cell of the seed hit negative because SciTil hit is outside
			    // of the Stt system.;

	Fi =  atan2(posizSciTil[i][1],posizSciTil[i][0]) ;
//-----------------
	if ( Fi < 0. ) Fi += 2.*PI;
	input.nFicell =  (Short_t) (0.5*NFIDIVCONFORMAL*Fi/PI);
	if(input.nFicell > NFIDIVCONFORMAL ) {
		input.nFicell = NFIDIVCONFORMAL;
	} else if (input.nFicell<0) {
		input.nFicell = 0;
	}
	input.posizSciTilx = posizSciTil[i][0];
	input.posizSciTily = posizSciTil[i][1];

	// outputs from the FindTrackInXYProjection class are stored here;
	input.ALFA = &ALFA[nSttTrackCand];
	input.BETA = &BETA[nSttTrackCand];
	input.Charge = &Charge[nSttTrackCand];
	input.Fi_final_helix_referenceframe = &Fi_final_helix_referenceframe[nSttTrackCand];
	input.Fi_initial_helix_referenceframe = &Fi_initial_helix_referenceframe[nSttTrackCand];
	input.Fi_low_limit = &Fi_low_limit[nSttTrackCand];
	input.Fi_up_limit = &Fi_up_limit[nSttTrackCand];
	input.GAMMA = &GAMMA[nSttTrackCand];
	input.ListHitsinTrack = &ListSttParHitsinTrack[nSttTrackCand][0];
	input.ListSciTilHitsinTrack = &ListSciTilHitsinTrack[nSttTrackCand][0];
	input.nHitsinTrack = &nSttParHitsinTrack[nSttTrackCand];
	input.nSciTilHitsinTrack = &nSciTilHitsinTrack[nSttTrackCand];
	input.Oxx = &Ox[nSttTrackCand];
	input.Oyy = &Oy[nSttTrackCand];
	input.Rr = &R[nSttTrackCand];
	input.S_SciTilHitsinTrack = &S_SciTilHitsinTrack[nSttTrackCand][0];
	input.TypeConf = &TypeConf[nSttTrackCand];
	input.U = &U[nSttTrackCand][0];
	input.V = &V[nSttTrackCand][0];

	outcome = SttTrackXYFinder.FindTrackInXYProjection(&input);

	if(!outcome){
		continue;
	}
	for(j=0; j<nSttParHitsinTrack[nSttTrackCand]; j++){
	  InclusionListStt[ListSttParHitsinTrack[nSttTrackCand][j]] = false;
	}
	keepit[nSttTrackCand]=true;
	nSttTrackCand++;

    } // end of  for(i=0; i<nSciTilHits ; i++)


 }  // end of  if(YesSciTil)


//----- end use the SciTil hits first.



//----- loop over the parallel hits

//   begins the first iteration with more severe cuts on the # hits in track candidate
 for(iParHit=0; iParHit<nSttParHit + 1 -  MINIMUMHITSPERTRACK ; iParHit++) {

	if( nSttTrackCand >= MAXTRACKSPEREVENT) {
		cout<<"from PndTrkTracking :  # n. Tracks found so far = "
		<<nSttTrackCand<<" and it is >= MAXTRACKSPEREVENT ( = "
		 <<MAXTRACKSPEREVENT<<"); exiting from || hit loop.\n";
		break;
	}
	if( ! InclusionListStt[ListSttParHits[iParHit]] )  continue;

	// inputs for the FindTrackInXYProjection class;
	input.iHit = iParHit;// seed hit in the PARALLEL number scheme; it is negative for SciTil Hits.
	input.nFicell = FiConformalIndex[ListSttParHits[iParHit]];  // Fi cell of the seed hit;
	input.nRcell = RConformalIndex[ListSttParHits[iParHit]];  // R cell of the seed hit;

	// outputs from the FindTrackInXYProjection class are stored here;
	input.ALFA = &ALFA[nSttTrackCand];
	input.BETA = &BETA[nSttTrackCand];
	input.Charge = &Charge[nSttTrackCand];
	input.Fi_final_helix_referenceframe = &Fi_final_helix_referenceframe[nSttTrackCand];
	input.Fi_initial_helix_referenceframe = &Fi_initial_helix_referenceframe[nSttTrackCand];
	input.Fi_low_limit = &Fi_low_limit[nSttTrackCand];
	input.Fi_up_limit = &Fi_up_limit[nSttTrackCand];
	input.GAMMA = &GAMMA[nSttTrackCand];
	input.ListHitsinTrack = &ListSttParHitsinTrack[nSttTrackCand][0];
	input.ListSciTilHitsinTrack = &ListSciTilHitsinTrack[nSttTrackCand][0];
	input.nHitsinTrack = &nSttParHitsinTrack[nSttTrackCand];
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




//-----------------------
//-----------------------
//-----------------------
//-----------------------  doing the fit with the skew hits for each XY plane track found
//-----------------------
//-----------------------
//-----------------------





 bool	GoodSkewFit[MAXTRACKSPEREVENT];

	Double_t
	S[2*MAXSTTHITS],
	Z[2*MAXSTTHITS];

 // the class with all the fits.
 PndTrkGlpkFits fit;




 for(i=0; i<nSttTrackCand;i++){

	keepit[i]=true;	// initialization.
	//  flag indicating if the skew sector info
	//  has completed the parameter info;
	//  a priori this is set false.
	GoodSkewFit[i]=false;

	nSttSkewHitsinTrack[i]=0;

	if( Fi_low_limit[i] <-99998.) continue ;  // this is when in XY the Helix circle is not in the
						// STT region; this in principle should never happen.

//-----  finding the skew hits intersecting this XY trajectory circle


 nSttSkewHitsinTrack[i] = AssociateSkewHitsToXYTrack(
	InclusionListStt, // excluded only if it is a double hit
	nSttSkewHit,
	ListSttSkewHits,
	Ox[i],   //  input : X of center of XY plane circle
	Oy[i],   //  input : Y of center of XY plane circle
	R[i],   //  input : Radius of XY plane circle
	info,
	WDX,
	WDY,
	WDZ,
	Fi_low_limit[i],// in the Helix XY frame, taking into account the minimum/maximum
	Fi_up_limit[i], // radius of the STT  detector.
	Charge[i],
	TemporarySkewList, // output,  list of selected skew hits (in original numbering)
	S,       //  output,  S coordinate of selected Skew hit
	Z,       //  output,  Z coordinate of center wire of selected Skew hit
	ZDrift,   //  output,  drift distance IN Z DIRECTION only, of selected Skew hit
	ZErrorafterTilt   //  output,  Radius taking into account the tilt, IN Z DIRECTION only, of selected Skew hit
		);

// limit the total # Stt hits to MAXSTTHITSINTRACK
 if( nSttSkewHitsinTrack[i]+nSttParHitsinTrack[i] > MAXSTTHITSINTRACK ) {
	if(MAXSTTHITSINTRACK > nSttParHitsinTrack[i])
		nSttSkewHitsinTrack[i]=MAXSTTHITSINTRACK-nSttParHitsinTrack[i];
	else nSttSkewHitsinTrack[i]=0;
 }

// here there are up to 2 SciTil hits in track.
 if(nSciTilHitsinTrack[i]>0){
    for(j=0;j<nSciTilHitsinTrack[i];j++){
	tmpS[j]= S_SciTilHitsinTrack[i][j]; // this is already between 0 and 2PI.
	tmpZ[j]=posizSciTil[ ListSciTilHitsinTrack[i][j] ][2],
	tmpZDrift[j]=-1., // conventional, to signal that this is not a STT hit.
	// error is intentionally overestimated for later use in SZ fit.
//	tmpErrorZDrift[j] = DIMENSIONSCITIL/sqrt(12.);
	tmpErrorZDrift[j] = DIMENSIONSCITIL/2.;
    }
 }
 for(j=0;j<nSttSkewHitsinTrack[i];j++){
	ListSttSkewHitsinTrack[i][j]=TemporarySkewList[j][0];
	tmpS[j+nSciTilHitsinTrack[i]]=S[j],
	tmpZ[j+nSciTilHitsinTrack[i]]=Z[j],
	tmpZDrift[j+nSciTilHitsinTrack[i]]=ZDrift[j],
	// error is intentionally overestimated for later use in SZ fit.
	tmpErrorZDrift[j+nSciTilHitsinTrack[i]] = 3.*STRAWRADIUS;
 }

 if( nSttSkewHitsinTrack[i]+nSciTilHitsinTrack[i]< 2) {
	for(j=0;j<nSttSkewHitsinTrack[i];j++){
	   Sfinal[i][ListSttSkewHitsinTrack[i][j]]= S[j];
	}
	continue ;
 }

//  finding if there are discontinuity at 0 for fi value of the Skew Straws Hit.
//  In case of discontinuity at 0, add 2*PI to fi of those hits with fi in the 1st quadrant.
//  This is necessary because the discontinuities would make the fit
//  in the SZ plane fail.
//  In this discontinuity fixing, the value FI0 of the vertex (0,0) is also included.
//  If there is discontinuity fixing, the values of S[i] AND POSSIBLY
//  Fi_initial_helix_referenceframe[i] might be modified (+2.*PI) from  now on.


 FixDiscontinuitiesFiangleinSZplane(
	nSttSkewHitsinTrack[i],
	S,
	&Fi_initial_helix_referenceframe[i],
	Charge[i]
	);

 outcome = fit.FitSZspace( 
	nSttSkewHitsinTrack[i]+nSciTilHitsinTrack[i],
	tmpS,
	tmpZ,
	tmpZDrift,  // drift radius onto the SZ projection; if negative --> SciTil hit.
	tmpErrorZDrift,
	Fi_initial_helix_referenceframe[i],   //   this is an input;
	MAXHITSINFIT,   // maximum n. hits in fit.
	&KAPPA[i],
	0     // IVOLTE
	);
//    outcome is negative (-99) when m = 0. and (-100) as result when the fit with glpk
//		failed.
 if(outcome < 0 || fabs(KAPPA[i])>1.e10)  {
	//  necessary to load here the Sfinal  vector anyway.
	for(j=0;j<nSttSkewHitsinTrack[i];j++){
		Sfinal[i][ListSttSkewHitsinTrack[i][j]]= S[j];
	}
	continue;
 }

 FI0[i]=Fi_initial_helix_referenceframe[i];  //  therefore, FI0[i] has an extra +2*PI or -2*PI added
						  // in case of tracks
						  //  crossing the X axis

//-----  finding a better association of the skew hits intersecting this XY trajectory circle

//    this means discarding those skew hits that are too far away from the fitted straight line
//    found in the SZ fit.



 Short_t STATUS;
 NNN=AssociateBetterAfterFitSkewHitsToXYTrack(
	nSttSkewHitsinTrack[i],
	TemporarySkewList, // input, list of selected skew hits (in skew numbering)
	S,	//  input,  S coordinate of selected Skew hit
	Z,	//  input,  Z coordinate of center wire of selected Skew hit
	ZDrift,	//  input,  drift distance IN Z DIRECTION only, of selected Skew hit
	ZErrorafterTilt,   //  input,  Radius taking into account the tilt,
		// IN Z DIRECTION only, of selected Skew hit
	KAPPA[i],	// input, KAPPA result of fit
	FI0[i],	// input, FI0 result of fit,
	tempore,	//  output, associated skew hits
	temporeS,	//  output, associated skew hit  S
	temporeZ,	//  output, associated skew hits Zcoordinate of center wire
	temporeZDrift,	//  output, associated skew hit Z drift
	temporeZErrorafterTilt,  //  output, associated skew hits Z error after tilt
	&STATUS	// output status.
	);
//    out of this function  STATUS  is zero signals only that KAPPA  is zero.

 if( STATUS >=0 ){

       if (NNN < 2) continue ;
       nSttSkewHitsinTrack[i] = NNN;
    // limit the total # of hits in track to MAXSTTHITSINTRACK.
	if( nSttSkewHitsinTrack[i]+nSttParHitsinTrack[i] > MAXSTTHITSINTRACK ) {
	 if(MAXSTTHITSINTRACK > nSttParHitsinTrack[i])
		nSttSkewHitsinTrack[i]=MAXSTTHITSINTRACK-nSttParHitsinTrack[i];
	 else nSttSkewHitsinTrack[i]=0;
	}
       for(j=0;j<nSttSkewHitsinTrack[i];j++){
		ListSttSkewHitsinTrack[i][j]=tempore[j];
		Sfinal[i][ListSttSkewHitsinTrack[i][j]]= temporeS[j];
		S[j]  =  temporeS[j] ;
		Z[j]  =  temporeZ[j] ;
		ZDrift[j]  =  temporeZDrift[j] ;
		ZErrorafterTilt[j]  =  temporeZErrorafterTilt[j] ;
       }


       GoodSkewFit[i]= true;

 }   else {   //   continuation of   if( outcome >=0 )
	// in this case nSttSkewHitsinTrack[i] remains the value as before (it is not set to NNN).

    for(j=0;j<nSttSkewHitsinTrack[i];j++){ ListSttSkewHitsinTrack[i][j]=TemporarySkewList[j][0];}
    GoodSkewFit[i]= true;
 }    //  end of     if( STATUS >=0 )



      if( nSttParHitsinTrack[i]+nSttSkewHitsinTrack[i] < MINIMUMHITSPERTRACK ||
	 nSttParHitsinTrack[i]+nSttSkewHitsinTrack[i]>MAXSTTHITSINTRACK) {
	keepit[i]=false;
        continue;
      }

      // ---------    numbering according to the ORIGINAL hit number
    for(int i1=0; i1< nSttSkewHitsinTrack[i]; i1++){

	SchosenSkew[i][ListSttSkewHitsinTrack[i][i1]]=
		Sfinal[i][ListSttSkewHitsinTrack[i][i1]]  =  S[i1] ;
	ZchosenSkew[i][ListSttSkewHitsinTrack[i][i1]]=
		Zfinal[i][ListSttSkewHitsinTrack[i][i1]]  =  Z[i1] ;
             ZDriftfinal[i][ListSttSkewHitsinTrack[i][i1]]  =  ZDrift[i1] ;
             ZErrorafterTiltfinal[i][ListSttSkewHitsinTrack[i][i1]]  =  ZErrorafterTilt[i1] ;
    }
     // ---------


 }   //  end of     for(i=0; i<nSttTrackCand;i++)
//------------------------------------------------------  end of skew hits section

//-------------- stampa
 if(istampa>=2){
	cout<<"from PndTrkTracking, after AssociateBetterAfterFitSkewHitsToXYTrack.\n";
	fPrint.stampetta(
			IVOLTE,
			keepit,
			&ListMvdPixelHitsinTrack[0][0],
			&ListMvdStripHitsinTrack[0][0],
			&ListSttParHitsinTrack[0][0],
			&ListSttSkewHitsinTrack[0][0],
			&ListSciTilHitsinTrack[0][0],
			nMvdPixelHitsinTrack,
			nMvdStripHitsinTrack,
			nSttParHitsinTrack,
			nSttSkewHitsinTrack,
			nSciTilHitsinTrack,
			nSttTrackCand,
			MAXMVDPIXELHITSINTRACK,
			MAXMVDSTRIPHITSINTRACK,
			MAXSCITILHITSINTRACK,
			MAXSTTHITSINTRACK,
			R,
			Ox,
			Oy,
			FI0
			);
 }
//-------------- fine stampa


// now the ordering the parallel and skew hits.


 for(i=0; i<nSttTrackCand;i++){

	// the BigList array must be loaded also when there are no Skew hits.
	OrderingSttSkewandSttParallel(
		Ox[i],
		Oy[i],
		R[i],
		nSttSkewHitsinTrack[i],  // input
		&ListSttSkewHitsinTrack[i][0],// input, but this gets ordered
		&Sfinal[i][0], // input from Skew Straws
		Charge[i], // input
		nSttParHitsinTrack[i], // input, # parallel Hits in the current track
		&ListSttParHitsinTrack[i][0],	// this was already ordered
		&U[i][0], // U conformal parallel hits; input, this was already ordered
		&V[i][0], // V conformal parallel hits; input, this was already ordered
		&BigList[i][0] // this is the final ordered Parallel+Skew list; already
				// in NATIVE hit number.
				);


 }   //  end of     for(i=0; i<nSttTrackCand;i++)

//-----------------------------------

 fSttMvdPndTrackCandArray->Delete();

 fSttMvdPndTrackArray->Delete();

 nTotalCandidates = nSttTrackCand;  // nSttTrackCand is already <= MAXTRACKSPEREVENT.


//----   find the angular range (in Fi) allowed for the STT hits, with the present Ox,Oy and R
//	of the track candidates, and for the Mvd hits (FI0 and Fi_low_limit ).

 for(  ncand= 0; ncand< nSttTrackCand; ncand++){

	keepit[ncand]=true;
	FindingParallelTrackAngularRange(
		Ox[ncand],
		Oy[ncand],
		R[ncand],
		Charge[ncand],
		&Fi_low_limit[ncand],	// Fi (in XY Helix frame) lower limit using
					// the Stt detector minimum/maximum radius
					// Fi_low_limit is ALWAYS between 0. and 2PI
		&Fi_up_limit[ncand],	// Fi (in XY Helix frame) upper limit using
					// the Stt detector maximum/minimum radius
					// Fi_up_limit is ALWAYS > Fi_low_limit and
					// possibly > 2PI.
		&statusflag[ncand],//it is a vector; =0, all well; =1, track contained completely between RMin
			   // and RMax; = -1 track contained within RMin; =-2 track outside RMax.
		RSTRAWDETECTORMIN,
		RSTRAWDETECTORMAX
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
			FI0,
			Fi_low_limit,
			Charge,
			nMvdPixelHitsinTrack, // output
			ListMvdPixelHitsinTrack, // output
			nMvdStripHitsinTrack, // output
			ListMvdStripHitsinTrack // output
			);

//-------------- stampa
 if(istampa>=2){
	cout<<"dopo MatchMvdHitsToSttTracks2[1646], evt. "<<IVOLTE<<endl;
	fPrint.stampetta(
			IVOLTE,
			keepit,
			&ListMvdPixelHitsinTrack[0][0],
			&ListMvdStripHitsinTrack[0][0],
			&ListSttParHitsinTrack[0][0],
			&ListSttSkewHitsinTrack[0][0],
			&ListSciTilHitsinTrack[0][0],
			nMvdPixelHitsinTrack,
			nMvdStripHitsinTrack,
			nSttParHitsinTrack,
			nSttSkewHitsinTrack,
			nSciTilHitsinTrack,
			nSttTrackCand,
			MAXMVDPIXELHITSINTRACK,
			MAXMVDSTRIPHITSINTRACK,
			MAXSCITILHITSINTRACK,
			MAXSTTHITSINTRACK,
			R,
			Ox,
			Oy,
			FI0
			);
 }
//-------------- fine stampa


//-----------------  end of section with match Mvd hits with Stt hits

	for(  ncand= 0; ncand< nSttTrackCand; ncand++){

		if(nMvdPixelHitsinTrack[ncand]+
			nMvdStripHitsinTrack[ncand]==0){

		   Mvdhits[ncand]=false;
		   ALFA[ncand]=-2.*Ox[ncand];
		   BETA[ncand]=-2.*Oy[ncand];
		   GAMMA[ncand]= Ox[ncand]*Ox[ncand]+Oy[ncand]*Oy[ncand]-R[ncand]*R[ncand];
		   continue;
		} // end of  if(nMvdPixelHitsinTrack[ncand]+


		if( nMvdPixelHitsinTrack[ncand] > MAXMVDPIXELHITSINTRACK){
			cout<<"from PndTrkTracking, nMvdPixelHitsinTrack["<<ncand
			<<"] is "<<nMvdPixelHitsinTrack[ncand]
			<<"and it is > MAXMVDPIXELHITSINTRACK ("
			<<MAXMVDPIXELHITSINTRACK
			<<"); setting nMvdPixelHitsinTrack[ncand] to "<<
			MAXMVDPIXELHITSINTRACK<<endl;
			nMvdPixelHitsinTrack[ncand]=MAXMVDPIXELHITSINTRACK;
		}

		if( nMvdStripHitsinTrack[ncand] > MAXMVDSTRIPHITSINTRACK){
			cout<<"from PndTrkTracking, nMvdStripHitsinTrack["<<ncand
			<<"] is "<<nMvdStripHitsinTrack[ncand]
			<<"and it is > MAXMVDSTRIPHITSINTRACK ("
			<<MAXMVDSTRIPHITSINTRACK
			<<"); setting nMvdStripHitsinTrack[ncand] to "<<
			MAXMVDSTRIPHITSINTRACK<<endl;
			nMvdStripHitsinTrack[ncand]=MAXMVDSTRIPHITSINTRACK;
		}


		Mvdhits[ncand] = true;



//----------------------------------------------------------
//     ordering all the hits belonging to the candidate track, by increasing R;
//     forming the new track with Mvd+Stt hits


		// here nTrackCandHit[ncand] is the sum of all the hits (Mvd+Stt) belonging to the track;
		nTrackCandHit[ncand] =nSttParHitsinTrack[ncand]+
					nSttSkewHitsinTrack[ncand]+
					nMvdPixelHitsinTrack[ncand]+
					nMvdStripHitsinTrack[ncand];
	// arrays used to store temporarily the info of Mvd hits to be ordered.
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
				(MAXMVDPIXELHITS+MAXMVDSTRIPHITS)*10 ;
			}

			//  ordering the Mvd Hits
			OrderingUsingConformal(
			   Ox[ncand],
			   Oy[ncand],
			   &Trajectory_Start[ncand][0],
			   nMvdPixelHitsinTrack[ncand]+nMvdStripHitsinTrack[ncand],
			   XY, // XY[*][0] = X position, XY[*][0] = Y position.
			   Charge[ncand],  // input
			   ListHits  // output
						);
			//  constructing the ordered new Track  Candidate now and loading
			//  the (now ordered) X and Y lists for the hits in this track.
			for(i=0; i< nMvdPixelHitsinTrack[ncand]+
					nMvdStripHitsinTrack[ncand]; i++){
				if(ListHits[i]<(MAXMVDPIXELHITS+MAXMVDSTRIPHITS)*10){//Pixel.
					ListTrackCandHit[ncand][i] = ListHits[i];
					ListTrackCandHitType[ncand][i] = 0;
				} else { // Strip hits.
					ListTrackCandHit[ncand][i] = ListHits[i]-
						(MAXMVDPIXELHITS+MAXMVDSTRIPHITS)*10;
					ListTrackCandHitType[ncand][i] = 1;
				}
			}  // end of  for(i=0; i< nMvdPixelHitsinTrack[ncand]+



		for(i=0; i<nSttParHitsinTrack[ncand]+
			nSttSkewHitsinTrack[ncand];i++){
			ListTrackCandHit[ncand][i+
				nMvdPixelHitsinTrack[ncand]+
				nMvdStripHitsinTrack[ncand]
					] = BigList[ncand][i];
			if( info[BigList[ncand][i]][5] < 2.){
				// parallel Stt hit.
				ListTrackCandHitType[ncand][i+
				nMvdPixelHitsinTrack[ncand]+
				nMvdStripHitsinTrack[ncand]
					] = 2;
			} else {
				ListTrackCandHitType[ncand][i+
				nMvdPixelHitsinTrack[ncand]+
				nMvdStripHitsinTrack[ncand]
					] = 3;
			}
		}


	} //   end of  for(ncand=0; ncand< nTotalCandidates; ncand++)


//-------------------- end of ordering

//-------------- stampa
 if(istampa>=2){
	cout<<"\tstampa dopo ordering,    prima di refit.\n";
	fPrint.stampetta(
			IVOLTE,
			keepit,
			&ListMvdPixelHitsinTrack[0][0],
			&ListMvdStripHitsinTrack[0][0],
			&ListSttParHitsinTrack[0][0],
			&ListSttSkewHitsinTrack[0][0],
			&ListSciTilHitsinTrack[0][0],
			nMvdPixelHitsinTrack,
			nMvdStripHitsinTrack,
			nSttParHitsinTrack,
			nSttSkewHitsinTrack,
			nSciTilHitsinTrack,
			nSttTrackCand,
			MAXMVDPIXELHITSINTRACK,
			MAXMVDSTRIPHITSINTRACK,
			MAXSCITILHITSINTRACK,
			MAXSTTHITSINTRACK,
			R,
			Ox,
			Oy,
			FI0
			);
 }
//-------------- fine stampa


//---------------------   refit the Helix in XY plane using Stt + Mvd associated hits


	Short_t	iexcl;
	Double_t
		d,
		diff,
		rotationangle;




 for(ncand=0; ncand< nTotalCandidates; ncand++){
	if(!keepit[ncand]) continue;

	if( !Mvdhits[ncand]) continue;


	//   Py/Px = m in    v = m*u + q formula
	rotationangle= atan2( Charge[ncand]*Ox[ncand],-Charge[ncand]*Oy[ncand]);	// Py/Px = m in
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

	if(R[ncand]<RSTRAWDETECTORMIN/2.) {
		keepit[ncand]=false;
		continue;
	}
//--------------

//  find out if the SciTil hit associated with this track is still acceptable after the last
//  XY refit.


	for(i=0, iaccept=0;i<nSciTilHitsinTrack[ncand];i++){

		intersect=GeomCalculator.IntersectionSciTil_Circle(
			DIMENSIONSCITIL,
			posizSciTil[ListSciTilHitsinTrack[ncand][i]][0],
			posizSciTil[ListSciTilHitsinTrack[ncand][i]][1],
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
			S_SciTilHitsinTrack[ncand][0]=atan2(YintersectionList[0]-Oy[ncand],
				XintersectionList[0]-Ox[ncand]);
			if ( S_SciTilHitsinTrack[ncand][0]<0.)
				S_SciTilHitsinTrack[ncand][0] += 2.*PI;
			iaccept++;


		}   //  end of  if(intersect)

	} // end of  for(i=0, iaccept=0;i<nSciTilHitsinTrack[ncand];i++)



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
		&Charge[ncand]
		);

//------------------------------


	//   finding the FI angular range (in the laboratory frame) spanned by this parallel track
	//   taking into account the Rmax and Rmin of the straw detector. The track NOT NECESSARILY
	//   has to originate at (0,0).

	FindingParallelTrackAngularRange(
		Ox[ncand],
		Oy[ncand],
		R[ncand],
		Charge[ncand],  // this is an input.
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
		RSTRAWDETECTORMIN,
		RSTRAWDETECTORMAX
						);
	// Fi_low_limit set  to  -99999. when it is contained in Mvd
	// region completely.
	if( statusflag[ncand] == -1 ) Fi_low_limit[ncand] = -99999.;
	if( statusflag[ncand] == -2) keepit[ncand] = false;


	}	// end of for(ncand=0; ncand< nTotalCandidates; ncand++)

//--------------------- end of  refit the Helix in XY plane using Stt + Mvd associated hits


//-------------- stampa
 if(istampa>=2){
	cout<<"\tstampa dopo il Refit.\n";
	fPrint.stampetta(
			IVOLTE,
			keepit,
			&ListMvdPixelHitsinTrack[0][0],
			&ListMvdStripHitsinTrack[0][0],
			&ListSttParHitsinTrack[0][0],
			&ListSttSkewHitsinTrack[0][0],
			&ListSciTilHitsinTrack[0][0],
			nMvdPixelHitsinTrack,
			nMvdStripHitsinTrack,
			nSttParHitsinTrack,
			nSttSkewHitsinTrack,
			nSciTilHitsinTrack,
			nSttTrackCand,
			MAXMVDPIXELHITSINTRACK,
			MAXMVDSTRIPHITSINTRACK,
			MAXSCITILHITSINTRACK,
			MAXSTTHITSINTRACK,
			R,
			Ox,
			Oy,
			FI0
			);
 }
//-------------- fine stampa


  if(nMvdPixelHit+nMvdStripHit>0){
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
			FI0,
			Fi_low_limit, // because here we deal with hits in Mvd region
			Charge,
			nMvdPixelHitsinTrack, // input and output
			ListMvdPixelHitsinTrack, // input and output
			nMvdStripHitsinTrack, // input and output
			ListMvdStripHitsinTrack // input and output
			);



  } // end of  if(nMvdPixelHit+nMvdStripHit>0)
//---------------------   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


//-------------- stampa
 if(istampa>=2){
	cout<<"\tstampa dopo il Match Again.\n";

	fPrint.stampetta(
			IVOLTE,
			keepit,
			&ListMvdPixelHitsinTrack[0][0],
			&ListMvdStripHitsinTrack[0][0],
			&ListSttParHitsinTrack[0][0],
			&ListSttSkewHitsinTrack[0][0],
			&ListSciTilHitsinTrack[0][0],
			nMvdPixelHitsinTrack,
			nMvdStripHitsinTrack,
			nSttParHitsinTrack,
			nSttSkewHitsinTrack,
			nSciTilHitsinTrack,
			nSttTrackCand,
			MAXMVDPIXELHITSINTRACK,
			MAXMVDSTRIPHITSINTRACK,
			MAXSCITILHITSINTRACK,
			MAXSTTHITSINTRACK,
			R,
			Ox,
			Oy,
			FI0
			);
 }
//-------------- fine stampa


// use the risult just obtained from the fit in XY to redo the association of the Skew Straw hits


  for(ncand=0; ncand< nTotalCandidates; ncand++)
  {
	if(!keepit[ncand]) continue;
	if( ! Mvdhits[ncand])
	{
		if(YesCleanMvd){
	// reject the candidate if it is NOT contained in the pipe and
	// therefore it should have at least 1 Mvd hit but it has none.
			if( (!GeomCalculator.IsInTargetPipe(
				Ox[ncand],
				Oy[ncand],
				R[ncand],
				FI0[ncand],
				KAPPA[ncand],
				Charge[ncand],
				VERTICALGAP/2.) )
				)   keepit[ncand]=false;
		}  // end of  (YesCleanMvd)

		continue;
	}

	if( statusflag[ncand] == -1 ) continue; // this is when the XY circle is contained in the
						// the Mvd region completely; skip the association of
						// the Skews.

	nSttSkewHitsinTrack[ncand]= AssociateSkewHitsToXYTrack(
		InclusionListStt, // hit is excluded only if it multiple hit.
		nSttSkewHit,
		ListSttSkewHits,
		Ox[ncand],   //  input : X of center of XY plane circle
		Oy[ncand],   //  input : Y of center of XY plane circle
		R[ncand],   //  input : Radius of XY plane circle
		info,
		WDX,
		WDY,
		WDZ,
		Fi_low_limit[ncand],	// Fi (in Helix XY frame) lower limit using the Stt detector minimum/maximum radius
		Fi_up_limit[ncand],	// Fi (in Helix XY frame) upper limit using the Stt detector maximum/minimum radius
		Charge[ncand],
		TemporarySkewList, // output,  list of selected skew hits (in skew numbering)
		TemporaryS,       //  output,  S coordinate of selected Skew hit
		TemporaryZ,       //  output,  Z coordinate of selected Skew hit (center wire)
		TemporaryZDrift,   //  output,  drift distance IN Z DIRECTION only, of selected Skew hit
		TemporaryZErrorafterTilt   //  output,  Radius taking into account the tilt, IN Z DIRECTION only, of selected Skew hit
			);



	// limit the total # Stt hits to MAXSTTHITSINTRACK
	if( nSttSkewHitsinTrack[ncand]+nSttParHitsinTrack[ncand] > MAXSTTHITSINTRACK ) {
	  if(MAXSTTHITSINTRACK > nSttParHitsinTrack[ncand])
		nSttSkewHitsinTrack[ncand]=MAXSTTHITSINTRACK-nSttParHitsinTrack[ncand];
	  else nSttSkewHitsinTrack[ncand]=0;
	}
	for(j=0;j<nSttSkewHitsinTrack[ncand];j++)
	{
		ListSttSkewHitsinTrack[ncand][j]=TemporarySkewList[j][0];
		ListSttSkewHitsinTrackSolution[ncand][j]=TemporarySkewList[j][1];
	}

//-------------------------------------------  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


//---------------------   here calculate the S and Z values of Mvd Pixels, Mvd Strips,
//	 Stt Skew hits and SciTil hits (if present).

//  the difference between S and Sbis, ZED and ZEDbis, DriftRadius and DriftRadiusbis,
//  ErrorDriftRadius and ErrorDriftRadiusbis, is that S, ZED etc. contain the list of
//  Pixel+Strips+SciTil + other Skew Stt hits in case Pixel+Strips+SciTil are <= 2; instead
//  Sbis, ZEDbis etc. contain Pixel+Strips+all Skew Stt hits.

	//  nXYZhits = n. of Mvd hits + SciTil hits. However, if there are 2 SciTil
	//  hits in this track (namely two adjacent SciTil tiles have a hit
	//  caused PRESUMABLY by the same track) then count them AS ONE because below
	//  the average of their postions is considered !

	if( nSciTilHitsinTrack[ncand] == 2) {
	   nXYZhits = nMvdPixelHitsinTrack[ncand]+nMvdStripHitsinTrack[ncand]+ 1;
	}else {   // in this case nSciTilHitsinTrack[ncand] is 0 or 1;
	   nXYZhits = nMvdPixelHitsinTrack[ncand]+nMvdStripHitsinTrack[ncand]+
			nSciTilHitsinTrack[ncand];
	}

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


	// the SciTil hit ( when they are 2, the S_SciTilHitsinTrack is already a mean
	// of the two; then consider only 1 SciTil hit, the first, and make an average
	// of the two Z positions).

	i = nMvdPixelHitsinTrack[ncand]+nMvdStripHitsinTrack[ncand];
	if(nSciTilHitsinTrack[ncand]==2){
		ZED[i]=0.5*(posizSciTil[ListSciTilHitsinTrack[ncand][0]][2]+
			posizSciTil[ListSciTilHitsinTrack[ncand][1]][2]);
		S[i] = S_SciTilHitsinTrack[ncand][0];
		// DriftRadius is set conventionally at -2, for later use in the SZ fit;
		// the error on the point used in the fit is ErrorDriftRadius and this
		// is overestimated to be DIMENSIONSCITIL/2.
		DriftRadius[i]=-2.;
		ErrorDriftRadius[i]= DIMENSIONSCITIL/2.; ;
	}else if (nSciTilHitsinTrack[ncand]==1){
		ZED[i]=posizSciTil[ListSciTilHitsinTrack[ncand][0]][2];
		S[i] = S_SciTilHitsinTrack[ncand][0];
		// DriftRadius is set conventionally at -2, for later use in the SZ fit;
		// the error on the point used in the fit is ErrorDriftRadius and this
		// is overestimated to be DIMENSIONSCITIL/2.
		DriftRadius[i]=-2.;
		ErrorDriftRadius[i]= DIMENSIONSCITIL/2.; ;
	}


	// the Skew Stt hits
	for(j=0;j<nSttSkewHitsinTrack[ncand]; j++){

		k=ListSttSkewHitsinTrack[ncand][j];
		kall = nMvdPixelHitsinTrack[ncand]+
			nMvdStripHitsinTrack[ncand]+j;
		if( nSciTilHitsinTrack[ncand] ==2 ){ // in this case only 1 SciTil hit
						// has been considered above;
		  i = nMvdPixelHitsinTrack[ncand]+nMvdStripHitsinTrack[ncand]+1+j;
		} else {  // this is the case of 1 or 0 SciTil hits in track; the
			//  (impossible?) case of > 2 SciTil hits has already been
			//  prevented early in PndTrkCTFindTrackInXY.
		  i = nMvdPixelHitsinTrack[ncand]+nMvdStripHitsinTrack[ncand]+
			nSciTilHitsinTrack[ncand] +j ;
		}

		// calculate the quantities used for the SZ fit only.
		if(i<nhitsinfit){
			S[i] = TemporaryS[j];
			ZED[i]=TemporaryZ[j];
			DriftRadius[i]=TemporaryZDrift[j];
			ErrorDriftRadius[i]=2.*TemporaryZDrift[j];
		}
		//-----------
		ZEDbis[kall]=TemporaryZ[j];
		Sbis[kall]= TemporaryS[j];
		DriftRadiusbis[kall]=TemporaryZDrift[j];
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
			Charge[ncand]	// this remains unchanged.
					);

//---------------------   here do the fit again in the SZ space if there are Mvd hits.
//			  For this, reordering of the  Mvd hits is not necessary.


		resultFitSZagain[ncand] = fit.FitSZspace(
					nhitsinfit,	// n. hits to be fitted
					S,
					ZED,
					DriftRadius,
					ErrorDriftRadius,
					FI0[ncand],
					MAXHITSINFIT,	// maximum number allowed in the fit
						// deve essere meno di 30+30+60
					&emme,
					IVOLTE	// IVOLTE
						);

		if( resultFitSZagain[ncand]==1){
			KAPPA[ncand] = emme;
			if( ncand<= nSttTrackCand ) SttSZfit[ncand]=true;
		} else {
			keepit[ncand]=false;
		}

//-------------------------------------------

//	use the risult just obtained from the fit in SZ to reject the spurious Skew Straw hits
//	and the Mvd spurious hits; also in this function there is the calculation of the Z position
//	of the SKEW hits and the MVD hits, for a given track candidate (ie for a given Helix
//	circle in the XY plane)

	Short_t MaxTurns;
	Double_t Turns;


	  if(keepit[ncand]){
	    if(R[ncand] < RSTRAWDETECTORMAX/2.){
		if(-Charge[ncand]*KAPPA[ncand]>0.){	// this means Pz>0.
		  Turns= 0.5*fabs((ZCENTER_STRAIGHT+SEMILENGTH_STRAIGHT)
					*KAPPA[ncand])/PI;
		  if( fabs(Turns)<10.)  MaxTurns=(Short_t) Turns ;
		  else  MaxTurns=10;
		} else {
		  Turns= 0.5*fabs((ZCENTER_STRAIGHT-SEMILENGTH_STRAIGHT)
					*KAPPA[ncand])/PI;
		  if( fabs(Turns)<10.)  MaxTurns=(Short_t) Turns ;
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



//	First cleanup based on the absence of Mvd hits


 if(YesCleanMvd){
	// reject the candidate if it is NOT contained in the pipe and
	// therefore it should have at least 1 Mvd hit but it has none.
	if( (!GeomCalculator.IsInTargetPipe(	Ox[ncand],
			Oy[ncand],
			R[ncand],
			FI0[ncand],
			KAPPA[ncand],
			Charge[ncand],
			VERTICALGAP/2.) )
				 &&
		nMvdStripHitsinTrack[ncand]+nMvdPixelHitsinTrack[ncand]==0)
		{
			keepit[ncand]=false;
		}
 }  // end of  (YesCleanMvd)

    }	//  end of for(ncand=0; ncand< nTotalCandidates; ncand++)





//-------------- stampa
 if(istampa>=2){
	cout<<"\tstampa dopo il Cleanup piccolo\n";
	fPrint.stampetta(
			IVOLTE,
			keepit,
			&ListMvdPixelHitsinTrack[0][0],
			&ListMvdStripHitsinTrack[0][0],
			&ListSttParHitsinTrack[0][0],
			&ListSttSkewHitsinTrack[0][0],
			&ListSciTilHitsinTrack[0][0],
			nMvdPixelHitsinTrack,
			nMvdStripHitsinTrack,
			nSttParHitsinTrack,
			nSttSkewHitsinTrack,
			nSciTilHitsinTrack,
			nSttTrackCand,
			MAXMVDPIXELHITSINTRACK,
			MAXMVDSTRIPHITSINTRACK,
			MAXSCITILHITSINTRACK,
			MAXSTTHITSINTRACK,
			R,
			Ox,
			Oy,
			FI0
			);
 }
//-------------- fine stampa

//---  redo association of parallel Stt  straw  hits to this track, after better refit.
  if(nMvdPixelHit+nMvdStripHit>0){


	CollectParSttHitsagain(
			keepit,
			Mvdhits,
			info,
			nSttParHit,
			0,	// starting candidate n. (included).
			nTotalCandidates, // ending candidate n. (excluded).
			KAPPA,
			FI0,
			Fi_low_limit,
			Fi_up_limit,
			nSttParHitsinTrack, // input and output
			ListSttParHitsinTrack // input and output
			);


//	ordering all the hits belonging to the candidate track, by increasing R (large
//	trajectories)  or Conformal variables (better for small trajectories);
//	from candidate n. 0 to candidate n. nTotalCandidates-1; loading ListTrackCandHit.
//	the array ordered are :
//	ListTrackCandHit, ListTrackCandHitType, ListSttParHitsinTrack, ListSttSkewHitsinTrack
//	and also at the end the SciTil hit (if present) is added.

//-------------------------------
  for(ncand=0; ncand< nTotalCandidates; ncand++){
	// limit the total # Stt hits to MAXSTTHITSINTRACK
	if( nSttSkewHitsinTrack[ncand]+nSttParHitsinTrack[ncand] > MAXSTTHITSINTRACK ) {
	  if(MAXSTTHITSINTRACK > nSttSkewHitsinTrack[ncand])
		nSttParHitsinTrack[ncand]=MAXSTTHITSINTRACK-nSttSkewHitsinTrack[ncand];
	  else nSttParHitsinTrack[ncand]=0;
	}
  }
//---------------

	Ordering_Loading_ListTrackCandHit(
		keepit,
		0,
		nTotalCandidates,
		info,
		Trajectory_Start,
		Charge,
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

  }  // end of if(nMvdPixelHit+nMvdStripHit>0)

//------------- cleanup section.


	Start[0]=0.;
	Start[1]=0.;
	Start[2]=0.;
	gap = (Double_t) (VERTICALGAP);

//-------------- stampa
 if(istampa>=2){
	cout<<"\tstampa prima del Cleanup grosso\n";
	fPrint.stampetta(
			IVOLTE,
			keepit,
			&ListMvdPixelHitsinTrack[0][0],
			&ListMvdStripHitsinTrack[0][0],
			&ListSttParHitsinTrack[0][0],
			&ListSttSkewHitsinTrack[0][0],
			&ListSciTilHitsinTrack[0][0],
			nMvdPixelHitsinTrack,
			nMvdStripHitsinTrack,
			nSttParHitsinTrack,
			nSttSkewHitsinTrack,
			nSciTilHitsinTrack,
			nSttTrackCand,
			MAXMVDPIXELHITSINTRACK,
			MAXMVDSTRIPHITSINTRACK,
			MAXSCITILHITSINTRACK,
			MAXSTTHITSINTRACK,
			R,
			Ox,
			Oy,
			FI0
			);
 }
//-------------- fine stampa


    for(ncand=0, nRemainingCandidates=0; ncand< nTotalCandidates; ncand++){
	if(!keepit[ncand]) continue;
	Short_t &nHitsPar = nSttParHitsinTrack[ncand];
	Short_t &nHitsSkew = nSttSkewHitsinTrack[ncand];
	Double_t auxS[nHitsSkew];

	for(i=0;i<nHitsSkew;i++){
		auxS[i] = SchosenSkew[ncand][ListSttSkewHitsinTrack[ncand][i]];
	}

    if(YesClean){
if(istampa>1) cout<<"PndTrkTracking, entra in TrackCleanup tracce normali, IVOLTE "<<IVOLTE
	<<" e track cand. "<<ncand<<endl;
	if ( !TrackCleanup(
			gap,
			Ox[ncand],
			Oy[ncand],
			R[ncand],
			KAPPA[ncand],
			FI0[ncand],
			Charge[ncand],
			Start,
			nHitsPar,
			&ListSttParHitsinTrack[ncand][0],  // this is already ordered.
			nHitsSkew,
			&ListSttSkewHitsinTrack[ncand][0],  // this is already ordered.
			auxS,
			info,
			RSTRAWDETECTORMIN,
			APOTEMAMAXINNERPARSTRAW,
			APOTEMAMINSKEWSTRAW,
			APOTEMAMAXSKEWSTRAW,
			APOTEMAMINOUTERPARSTRAW,
			RSTRAWDETECTORMAX
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

	if(MvdAloneTracking && nMvdPixelHit+nMvdStripHit>0) {

//  load the UsedPixel and UsedStrip  vectors.
	bool	UsedPixel[MAXMVDPIXELHITS],
		UsedStrip[MAXMVDSTRIPHITS];

	Short_t List[MAXMVDPIXELHITS+MAXMVDSTRIPHITS],
		 ListType[MAXMVDPIXELHITS+MAXMVDSTRIPHITS];

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
		if( nTotalCandidates >= MAXTRACKSPEREVENT -1 ) break; // protection for the
						// length of many arrays.
		nalone=0;
		npixelhitsintrack=0;
		nstriphitsintrack=0;
		for(j=0;j<nHitMvdTrackCand[i];j++){

		  if(ListHitTypeMvdTrackCand[i][j] == 
			FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)){
		   if(UsedPixel[ ListHitMvdTrackCand[i][j] ])   continue;
		   if(npixelhitsintrack>MAXMVDPIXELHITSINTRACK) continue;
		   AloneX[nalone] = XMvdPixel[ ListHitMvdTrackCand[i][j] ];
		   AloneY[nalone] = YMvdPixel[ ListHitMvdTrackCand[i][j] ];
		   List[nalone] = ListHitMvdTrackCand[i][j];
		   ListType[nalone] = 0;
		   nalone++;
		   npixelhitsintrack++;
		  }else {// at this point this is a Strip hit; already made sure
			// earlier in the code that there is no third possibility.

//		   if (ListHitTypeMvdTrackCand[i][j] ==	// this condition in principle at this
//								// point is always satisfied.
//			FairRootManager::Instance()->GetBranchId(fMvdStripBranch)){


		   if(UsedStrip[ ListHitMvdTrackCand[i][j] ])   continue;
		   if(nstriphitsintrack>MAXMVDSTRIPHITSINTRACK) continue;
		   AloneX[nalone] = XMvdStrip[ ListHitMvdTrackCand[i][j] ];
		   AloneY[nalone] = YMvdStrip[ ListHitMvdTrackCand[i][j] ];
		   List[nalone] = ListHitMvdTrackCand[i][j];
		   ListType[nalone] = 1;
		   nalone++;
		   nstriphitsintrack++;
		  }
		}	// end of   for(j=0;j<nHitMvdTrackCand[i];j++)

		if(nalone < 3 ) continue;

		// here find possibly new tracks

		if( nalone == 3 ){
			status[nTotalCandidates] = GeomCalculator.CalculateCircleThru3Points(
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
			if( status[nTotalCandidates] && radius > RSTRAWDETECTORMIN/2.){
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
			if(status[nTotalCandidates]&& R[nTotalCandidates] > RSTRAWDETECTORMIN/2.){
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
		Trajectory_Start,
		Charge,
		SchosenSkew  // here this array is irrelevant since there are not yet
			// any Stt hits in Track Cand.
		);

//-------------------- end of ordering

// -----------------  finding the charge of those track found starting from Mvd

 int dim = MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK+
			MAXSTTHITSINTRACK+MAXSCITILHITSINTRACK;

 int nMvdOnly;
 Double_t
	esse[dim],
	Sini,
	Slast;

	Double_t
	DriftRadius[dim],// all skew hits have double
	ErrorDriftRadius[dim],// solutions
	ZED[dim]; // rather improbable chance that

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
		Charge[ncand] =  1;
	    } else {
		Charge[ncand] =  -1;
	    }



	// it is necessary first to calculate Fi_low_limit and Fi_up_limit.

	FindingParallelTrackAngularRange(
		Ox[ncand],
		Oy[ncand],
		R[ncand],
		Charge[ncand],
		&Fi_low_limit[ncand],	// Fi (in XY Helix frame) lower limit using
					// the Stt detector minimum/maximum radius
					// Fi_low_limit is ALWAYS between 0. and 2PI
		&Fi_up_limit[ncand],	// Fi (in XY Helix frame) upper limit using
					// the Stt detector maximum/minimum radius
					// Fi_up_limit is ALWAYS > Fi_low_limit and
					// possibly > 2PI.
		&statusflag[ncand],// vector; =0, all well; =1, track contained completely between RMin
			   // and RMax; = -1 track contained within RMin; =-2 track outside RMax.
		RSTRAWDETECTORMIN,
		RSTRAWDETECTORMAX
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
		Charge[ncand]	// this remains unchanged.
				);




// ----------------------------- fit in SZ with the Mvd tracks
		resultFitSZagain[ncand] = fit.FitSZspace(
					nMvdOnly,	// n. hits to be fitted
					S,
					ZED,
					DriftRadius,
					ErrorDriftRadius,
					FI0[ncand],
					MAXHITSINFIT,	// maximum number allowed in the fit
						// deve essere meno di 30+30+60.
					&emme,
					0   // IVOLTE
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
			nSttTrackCand,	// starting candidate n. (included).
			nTotalCandidates, // ending candidate n. (excluded).
			KAPPA,
			FI0,
			Fi_low_limit,
			Fi_up_limit,
			nSttParHitsinTrack, // input and output
			ListSttParHitsinTrack // input and output
			);
	// limit the total # Stt hits to MAXSTTHITSINTRACK
   for(ncand=nSttTrackCand; ncand< nTotalCandidates; ncand++){
	if( nSttParHitsinTrack[ncand] > MAXSTTHITSINTRACK ) {
	  nSttParHitsinTrack[ncand]=MAXSTTHITSINTRACK;
	}
   } // end of   for(ncand=nSttTrackCand; ncand< nTotalCandidates; ncand++)


   //     try to attach skew hits to the new tracks (the result can also be 0).
   for(ncand=nSttTrackCand; ncand< nTotalCandidates; ncand++){
	if(!keepit[ncand]) continue;
	// when statusflag[ncand]<0 the track does not intersect Stt region.
	if( statusflag[ncand]<0) continue;
	   nSttSkewHitsinTrack[ncand] = AssociateSkewHitsToXYTrack(
		InclusionListStt,
		nSttSkewHit,
		ListSttSkewHits,
		Ox[ncand],   //  input : X of center of XY plane circle
		Oy[ncand],   //  input : Y of center of XY plane circle
		R[ncand],   //  input : Radius of XY plane circle
		info,
		WDX,
		WDY,
		WDZ,
		Fi_low_limit[ncand],	// Fi (in Helix XY frame) lower limit using the Stt detector minimum/maximum radius
		Fi_up_limit[ncand],	// Fi (in Helix XY frame) upper limit using the Stt detector maximum/minimum radius
		Charge[ncand],
		TemporarySkewList, // output,  list of selected skew hits (in skew numbering)
		TemporaryS,       //  output,  S coordinate of selected Skew hit
		TemporaryZ,       //  output,  Z coordinate of selected Skew hit (center wire)
		TemporaryZDrift,   //  output,  drift distance IN Z DIRECTION only, of selected Skew hit
		TemporaryZErrorafterTilt   //  output,  Radius taking into account the tilt, IN Z DIRECTION only, of selected Skew hit
								);
	   // limit the total # Stt hits to MAXSTTHITSINTRACK
	   if( nSttSkewHitsinTrack[ncand]+nSttParHitsinTrack[ncand] > MAXSTTHITSINTRACK ) {
		if(MAXSTTHITSINTRACK > nSttParHitsinTrack[ncand])
		  nSttSkewHitsinTrack[ncand]=MAXSTTHITSINTRACK-nSttParHitsinTrack[ncand];
		else nSttSkewHitsinTrack[ncand]=0;
	   }

	   for(j=0;j<nSttSkewHitsinTrack[ncand];j++)
	   {
		ListSttSkewHitsinTrack[ncand][j]=TemporarySkewList[j][0];
		SchosenSkew[ncand][ListSttSkewHitsinTrack[ncand][j]] = TemporaryS[j];
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
		Trajectory_Start,
		Charge,
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


	if ( !TrackCleanup(
			gap,
			Ox[ncand],
			Oy[ncand],
			R[ncand],
			KAPPA[ncand],
			FI0[ncand],
			Charge[ncand],
			Start,
			nSttParHitsinTrack[ncand],
			&ListSttParHitsinTrack[ncand][0],  // this is already ordered.
			nSttSkewHitsinTrack[ncand],
			&ListSttSkewHitsinTrack[ncand][0],  // this is already ordered.
			&SchosenSkew[ncand][0],
			info,
			RSTRAWDETECTORMIN,
			APOTEMAMAXINNERPARSTRAW,
			APOTEMAMINSKEWSTRAW,
			APOTEMAMAXSKEWSTRAW,
			APOTEMAMINOUTERPARSTRAW,
			RSTRAWDETECTORMAX
				) ) {
		keepit[ncand]=false;
	}  // end of if ( !TrackCleanup


     } // end of   for(ncand=nSttTrackCand; ncand< nTotalCandidates; ncand++)
  }  // end of if(YesClean)

//-----------  end of cleanup of new tracks found.



	} // end of  if(MvdAloneTracking && nMvdPixelHit+nMvdStripHit>0)



// ----------------------------- end of finding Tracks starting with the Mvd tracks
// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------




//-------  load the new PndTrackCand ; each track has the STT and the Mvd hits associated
//-------  also load the new PndTrack ; each track has the STT and the Mvd hits associated

 LoadPndTrack_TrackCand(
	keepit,
	SttSZfit,
	nTotalCandidates,
	Charge,
	nSttTrackCand,
	FI0,
	KAPPA,
	info,
	SchosenSkew,
	ZchosenSkew
	);



//------------ section with comparison MC Mvd hits - associated hits to a certain track




 int dim1,  dim2  , dim3 , dim4, dim5, dim6, dim7;

 if(nSttHit == 0 ) { dim1=1; } else { dim1 = MAXTRACKSPEREVENT*nSttHit; };

 Short_t
	MCParalAloneList[dim1],
	MCSkewAloneList[dim1];

 if(nTotalCandidates == 0 ) {
	dim1 = 1;
	dim2 = 1;
	dim3 = 1;
	dim4 = 1;
	dim5 = 1;
	dim6 = 1;
	dim7 = 1;
 } else {
	dim1 = nTotalCandidates;
	dim2 = nTotalCandidates*MAXMVDPIXELHITSINTRACK;
	dim3 = nTotalCandidates*MAXMVDSTRIPHITSINTRACK;
	dim4 = nTotalCandidates*nMvdPixelHit;
	dim5 = nTotalCandidates*nMvdStripHit;
	dim6 = nTotalCandidates*MAXSCITILHITSINTRACK;
	dim7 = nTotalCandidates*nSciTilHits;
 }

 Short_t
	nMvdPixelCommon[dim1],
	MvdPixelCommonList[dim2],
	nMvdPixelSpuriinTrack[dim1],
	MvdPixelSpuriList[dim2],
	nMCMvdPixelAlone[dim1],
	MCMvdPixelAloneList[dim4],

	nMvdStripCommon[dim1],
	MvdStripCommonList[dim3],
	nMvdStripSpuriinTrack[dim1],
	MvdStripSpuriList[dim3],
	nMCMvdStripAlone[dim1],
	MCMvdStripAloneList[dim5],

	nSciTilCommon[dim1],
	SciTilCommonList[dim6],
	nSciTilSpuriinTrack[dim1],
	SciTilSpuriList[dim6],
	nMCSciTilAlone[dim1],
	MCSciTilAloneList[dim7];


 if( doMcComparison && nTotalCandidates > 0){
	// make the struct for the data to pass to the
	// method PndTrkComparisonMCtruth::ComparisonwithMC ;
	PndTrkComparisonMCtruth_io_Data ioData;
	// load the structure;

	ioData.Bfield = BFIELD;
	ioData.Charge = Charge;
	ioData.Cvel = CVEL;
	ioData.daTrackFoundaTrackMC = daTrackFoundaTrackMC;
	ioData.DIMENSIONSciTil = DIMENSIONSCITIL;
	ioData.Errorsqpixel = ERRORSQPIXEL;
	ioData.Errorsqstrip = ERRORSQSTRIP;
	ioData.FI0 = FI0;
	ioData.fMCTrackArray = fMCTrackArray;
	ioData.fMvdMCPointArray = fMvdMCPointArray;
	ioData.fSciTilMaxNumber = fSciTilMaxNumber;
	ioData.fSciTHitArray = fSciTHitArray;
	ioData.fSciTPointArray = fSciTPointArray;
	ioData.fSttPointArray = fSttPointArray;
	ioData.HANDLE = HANDLE,
	ioData.HANDLE2 = HANDLE2,
	ioData.info = &info[0][0];
	ioData.istampa = istampa;
	ioData.IVOLTE = IVOLTE;
	ioData.KAPPA = KAPPA;
	ioData.keepit = keepit;
	ioData.InclusionListStt = InclusionListStt;
	ioData.ListMvdPixelHitsinTrack = &ListMvdPixelHitsinTrack[0][0];
	ioData.ListMvdStripHitsinTrack = &ListMvdStripHitsinTrack[0][0];
	ioData.ListSciTilHitsinTrack = &ListSciTilHitsinTrack[0][0];
	ioData.ListSttParHitsinTrack = &ListSttParHitsinTrack[0][0];
	ioData.ListSttSkewHitsinTrack = &ListSttSkewHitsinTrack[0][0];
	ioData.ListTrackCandHit = &ListTrackCandHit[0][0];
	ioData.ListTrackCandHitType = &ListTrackCandHitType[0][0];
	ioData.MAXMCTRACKS = MAXMCTRACKS;
	ioData.MAXMVDPIXELHITS = MAXMVDPIXELHITS;
	ioData.MAXMVDPIXELHITSINTRACK = MAXMVDPIXELHITSINTRACK;
	ioData.Maxmvdmcpoints = MAXMVDMCPOINTS;
	ioData.MAXMVDSTRIPHITS = MAXMVDSTRIPHITS;
	ioData.MAXMVDSTRIPHITSINTRACK = MAXMVDSTRIPHITSINTRACK;
	ioData.MAXSCITILHITS = MAXSCITILHITS ;
	ioData.MAXSCITILHITSINTRACK = MAXSCITILHITSINTRACK;
	ioData.MAXSTTHITS = MAXSTTHITS;
	ioData.MAXSTTHITSINTRACK = MAXSTTHITSINTRACK;
	ioData.MAXTRACKSPEREVENT = MAXTRACKSPEREVENT;
	ioData.MCMvdPixelAloneList = MCMvdPixelAloneList;
	ioData.MCMvdStripAloneList = MCMvdStripAloneList;
	ioData.MCParalAloneList = MCParalAloneList;
	ioData.MCSciTilAloneList = MCSciTilAloneList;
	ioData.MCSkewAloneList = MCSkewAloneList;
	ioData.MCSkewAloneX = MCSkewAloneX ;
	ioData.MCSkewAloneY = MCSkewAloneY ;
	ioData.MvdPixelCommonList = MvdPixelCommonList;
	ioData.MvdPixelSpuriList = MvdPixelSpuriList;
	ioData.MvdStripCommonList = MvdStripCommonList;
	ioData.MvdStripSpuriList = MvdStripSpuriList;
	ioData.nHitsInMCTrack = nHitsInMCTrack;
	ioData.nHitsInSciTile = nHitsInSciTile;
	ioData.nMCMvdPixelAlone = nMCMvdPixelAlone;
	ioData.nMCMvdStripAlone = nMCMvdStripAlone;
	ioData.nMCParalAlone = nMCParalAlone;
	ioData.nMCSciTilAlone = nMCSciTilAlone;
	ioData.nMCSkewAlone = nMCSkewAlone;
	ioData.nMvdPixelCommon = nMvdPixelCommon;
	ioData.nMvdPixelHitsinTrack = nMvdPixelHitsinTrack;
	ioData.nMvdStripHitsinTrack = nMvdStripHitsinTrack;
	ioData.nMvdPixelHit = nMvdPixelHit;
	ioData.nMvdPixelSpuriinTrack = nMvdPixelSpuriinTrack;
	ioData.nMvdStripCommon = nMvdStripCommon;
	ioData.nMvdStripHit = nMvdStripHit;
	ioData.nMvdStripSpuriinTrack = nMvdStripSpuriinTrack;
	ioData.nParalCommon = nParalCommon;
	ioData.nSciTilCommon = nSciTilCommon;
	ioData.nSciTilHits = nSciTilHits;
	ioData.nSciTilHitsinTrack = nSciTilHitsinTrack ;
	ioData.nSciTilSpuriinTrack = nSciTilSpuriinTrack ;
	ioData.nSkewCommon = nSkewCommon;
	ioData.nSkewHitsInMCTrack = nSkewHitsInMCTrack;
	ioData.nSpuriParinTrack = nSpuriParinTrack;
	ioData.nSpuriSkewinTrack = nSpuriSkewinTrack;
	ioData.nSttHit = nSttHit;
	ioData.nSttParHitsinTrack = nSttParHitsinTrack;
	ioData.nSttSkewHitsinTrack = nSttSkewHitsinTrack;
	ioData.nTotalCandidates = nTotalCandidates;
	ioData.OriginalSciTilList = &OriginalSciTilList[0][0];
	ioData.Ox = Ox;
	ioData.Oy = Oy;
	ioData.ParalCommonList = ParalCommonList;
	ioData.ParSpuriList = ParSpuriList;
	ioData.R = R;
	ioData.refindexMvdPixel = refindexMvdPixel;
	ioData.refindexMvdStrip = refindexMvdStrip;
	ioData.resultFitSZagain = resultFitSZagain;
	ioData.SciTilCommonList = SciTilCommonList;
	ioData.SciTilSpuriList = SciTilSpuriList;
	ioData.SkewCommonList = SkewCommonList;
	ioData.SkewSpuriList = SkewSpuriList;
	ioData.SttSZfit = SttSZfit;
	ioData.XMvdPixel = XMvdPixel;
	ioData.XMvdStrip = XMvdStrip;
	ioData.XSciTilCenter = pSciTilx;
	ioData.YMvdPixel = YMvdPixel;
	ioData.YMvdStrip = YMvdStrip;
	ioData.YSciTilCenter = pSciTily;
	ioData.ZMvdPixel = ZMvdPixel;
	ioData.ZMvdStrip = ZMvdStrip;
	ioData.ZSciTilCenter = pSciTilz;

	// class for the MC comparison;
	PndTrkComparisonMCtruth cmp;
	nMCTracks = cmp.ComparisonwithMC( ioData);




 }

//----------

 // write the Macro for visualization of tracks and hits;

 if(iplotta && IVOLTE<300){

	// the following initialization is necessary when the MC comparison
	// is not done just above (when doMcComparison=false). In this case
	// in fact it is necessary to have  the arrays nParalCommon, nSpuriParinTrack  etc.etc.
	// set at 0  otherwise some WriteMacro  methods  crash;
	if(!doMcComparison){
	 for(i=0; i<nTotalCandidates;i++){
	  nParalCommon[i]=0; nSpuriParinTrack[i]=0; nMCParalAlone[i]=0;
	  nSkewCommon[i]=0; nSpuriSkewinTrack[i]=0; nMCSkewAlone[i]=0;
	  nMvdPixelCommon[i]=0; nMvdPixelSpuriinTrack[i]=0; nMCMvdPixelAlone[i]=0;
	  nMvdStripCommon[i]=0; nMvdStripSpuriinTrack[i]=0; nMCMvdStripAlone[i]=0;
	  nSciTilCommon[i]=0; nSciTilSpuriinTrack[i]=0; nMCSciTilAlone[i]=0;

	 }  // end for(i=0; i<nTotalCandidates;i++)

	}


	PndTrkPlotMacros mymacro;
	PndTrkPlotMacros_InputData In_Put;
	In_Put.apotemamaxinnerparstraw = APOTEMAMAXINNERPARSTRAW ;
	In_Put.apotemamaxskewstraw = APOTEMAMAXSKEWSTRAW ;
	In_Put.apotemaminouterparstraw = APOTEMAMINOUTERPARSTRAW ;
	In_Put.apotemaminskewstraw = APOTEMAMINSKEWSTRAW ;
	In_Put.bfield = BFIELD ;
	In_Put.Charge = Charge ;
	In_Put.cvel = CVEL ;
	In_Put.daTrackFoundaTrackMC = daTrackFoundaTrackMC ;
	In_Put.dimensionscitil = DIMENSIONSCITIL ;
	In_Put.doMcComparison = doMcComparison ;
	In_Put.FI0 = FI0 ;
	In_Put.fMCTrackArray = fMCTrackArray ;
	In_Put.fSttPointArray = fSttPointArray ;
	In_Put.info =  &info[0][0] ;
	In_Put.IVOLTE = IVOLTE ;
	In_Put.KAPPA = KAPPA ;
	In_Put.keepit = keepit ;
	In_Put.InclusionListSciTil = InclusionListSciTil ;
	In_Put.istampa = istampa ;
	In_Put.ListMvdPixelHitsinTrack = &ListMvdPixelHitsinTrack[0][0] ;
	In_Put.ListMvdStripHitsinTrack = &ListMvdStripHitsinTrack[0][0] ;
	In_Put.ListSciTilHitsinTrack = &ListSciTilHitsinTrack[0][0] ;
	In_Put.ListSttParHitsinTrack = &ListSttParHitsinTrack[0][0] ;
	In_Put.ListSttSkewHitsinTrack = &ListSttSkewHitsinTrack[0][0] ;
	In_Put.ListTrackCandHit = &ListTrackCandHit[0][0] ;
	In_Put.ListTrackCandHitType = &ListTrackCandHitType[0][0] ;
	In_Put.MAXMCTRACKS = MAXMCTRACKS ;
	In_Put.MAXMVDPIXELHITS = MAXMVDPIXELHITS ;
	In_Put.MAXMVDPIXELHITSINTRACK = MAXMVDPIXELHITSINTRACK ;
	In_Put.MAXMVDSTRIPHITS = MAXMVDSTRIPHITS ;
	In_Put.MAXMVDSTRIPHITSINTRACK = MAXMVDSTRIPHITSINTRACK ;
	In_Put.MAXSCITILHITSINTRACK = MAXSCITILHITSINTRACK ;
	In_Put.MAXSCITILHITS = MAXSCITILHITS ;
	In_Put.MAXSTTHITS = MAXSTTHITS ;
	In_Put.MAXSTTHITSINTRACK = MAXSTTHITSINTRACK ;
	In_Put.MAXTRACKSPEREVENT = MAXTRACKSPEREVENT ;
	In_Put.MCMvdPixelAloneList = MCMvdPixelAloneList ;
	In_Put.MCMvdStripAloneList = MCMvdStripAloneList ;
	In_Put.MCParalAloneList = MCParalAloneList ;
	In_Put.MCSciTilAloneList = MCSciTilAloneList;
	In_Put.MCSkewAloneList = MCSkewAloneList ;
	In_Put.MCSkewAloneX = MCSkewAloneX ;
	In_Put.MCSkewAloneY = MCSkewAloneY ;
	In_Put.MvdPixelCommonList = MvdPixelCommonList ;
	In_Put.MvdPixelSpuriList = MvdPixelSpuriList ;
	In_Put.MvdStripCommonList = MvdStripCommonList ;
	In_Put.MvdStripSpuriList = MvdStripSpuriList ;
	In_Put.nMCMvdPixelAlone = nMCMvdPixelAlone ;
	In_Put.nMCMvdStripAlone = nMCMvdStripAlone ;
	In_Put.nMCParalAlone = nMCParalAlone ;
	In_Put.nMCSciTilAlone = nMCSciTilAlone;
	In_Put.nMCSkewAlone = nMCSkewAlone ;
	In_Put.nMCTracks = nMCTracks ;
	In_Put.nMvdPixelCommon = nMvdPixelCommon ;
	In_Put.nMvdPixelHit = nMvdPixelHit ;
	In_Put.nMvdPixelHitsinTrack = nMvdPixelHitsinTrack ;
	In_Put.nMvdPixelSpuriinTrack = nMvdPixelSpuriinTrack ;
	In_Put.nMvdStripCommon = nMvdStripCommon ;
	In_Put.nMvdStripHit = nMvdStripHit ;
	In_Put.nMvdStripHitsinTrack = nMvdStripHitsinTrack ;
	In_Put.nMvdStripSpuriinTrack = nMvdStripSpuriinTrack ;
	In_Put.nParalCommon = nParalCommon ;
	In_Put.nSciTilCommon = nSciTilCommon ;
	In_Put.nSciTilHits = nSciTilHits ;
	In_Put.nSciTilHitsinTrack = nSciTilHitsinTrack ;
	In_Put.nSciTilSpuriinTrack =  nSciTilSpuriinTrack;
	In_Put.nSkewCommon = nSkewCommon ;
	In_Put.nSpuriParinTrack = nSpuriParinTrack ;
	In_Put.nSttHit = nSttHit ;
	In_Put.nSttParHit = nSttParHit ;
	In_Put.nSttParHitsinTrack = nSttParHitsinTrack ;
	In_Put.nSttSkewHit = nSttSkewHit ;
	In_Put.nSttSkewHitsinTrack = nSttSkewHitsinTrack ;
	In_Put.nTotalCandidates = nTotalCandidates ;
	In_Put.nTrackCandHit = nTrackCandHit ;
	In_Put.Ox = Ox ;
	In_Put.Oy = Oy ;
	In_Put.ParalCommonList = ParalCommonList ;
	In_Put.ParSpuriList = ParSpuriList ;
	In_Put.posizSciTil = &posizSciTil[0][0] ;
	In_Put.R = R ;
	In_Put.rstrawdetectormax = RSTRAWDETECTORMAX ;
	In_Put.rstrawdetectormin = RSTRAWDETECTORMIN ;
	In_Put.SchosenSkew = &SchosenSkew[0][0] ;
	In_Put.SciTilCommonList = SciTilCommonList ;
	In_Put.SciTilSpuriList = SciTilSpuriList ;
	In_Put.sigmaXMvdPixel = sigmaXMvdPixel ;
	In_Put.sigmaXMvdStrip = sigmaXMvdStrip ;
	In_Put.sigmaYMvdPixel = sigmaYMvdPixel ;
	In_Put.sigmaYMvdStrip = sigmaYMvdStrip ;
	In_Put.SkewCommonList = SkewCommonList ;
	In_Put.verticalgap = VERTICALGAP ;
	In_Put.XMvdPixel = XMvdPixel ;
	In_Put.XMvdStrip = XMvdStrip ;
	In_Put.YMvdPixel = YMvdPixel ;
	In_Put.YMvdStrip = YMvdStrip ;
	In_Put.WDX = WDX ;
	In_Put.WDY = WDY ;
	In_Put.WDZ = WDZ ;
	In_Put.ZMvdPixel = ZMvdPixel ;
	In_Put.ZMvdStrip = ZMvdStrip ;

	mymacro.WriteAllMacros(
	In_Put
				);
 }

//----------------




 return;

}


//---------------------- end of   PndTrkTracking::Exec


//----------begin of function PndTrkTracking::AssociateBetterAfterFitSkewHitsToXYTrack

  Short_t PndTrkTracking::AssociateBetterAfterFitSkewHitsToXYTrack(
                   Short_t TemporarynSttSkewhitinTrack,  //  input
                   Short_t SkewList[][2], // input,  list of selected skew hits (in skew numbering)
                   Double_t *S,       //  input,  S coordinate of selected Skew hit
                   Double_t *Z,       //  input,  Z coordinate of selected Skew hit
                   Double_t *ZDrift,  //  input,  drift distance IN Z DIRECTION only, of selected Skew hit
                   Double_t *ZErrorafterTilt,   //  input,  Radius taking into account the tilt, IN Z DIRECTION only, of selected Skew hit
                   Double_t KAPPA,    // input, KAPPA result of fit
                   Double_t FI0,    // input, FI0 result of fit
                   Short_t *tempore,  //  output result, associated skew hits
                   Double_t *temporeS,  //  output, associated skew hit  S
                   Double_t *temporeZ,  //  output, associated skew hits Z
                   Double_t *temporeZDrift,  //  output, associated skew hit Z drift
                   Double_t *temporeZErrorafterTilt,  //  output, associated skew hits Z error after tilt
                   Short_t  *STATUS   // output
                                                     )
 {

    Short_t NAssociated;
    Short_t  sign;
    Int_t i, j, i1, ii, iii,  Kincl, nlow, nup;

    Double_t bbb,
             tempZ[2],
             zmin, zmax, deltaz,
		zdist[2],
		zdist1,
		zdist2;

    Double_t allowed_distance = 4.*STRAWRADIUS/sin(STRAW_SKEW_INCLINATION*PI/180.);

    if(fabs(KAPPA)<1.e-20) {
      *STATUS=-1;
      return 0;
    }

    NAssociated=0;
    if(KAPPA>0) {
     zmin = -FI0/KAPPA;
     zmax = (2.*PI-FI0)/KAPPA;
    }  else {
     zmax = -FI0/KAPPA;
     zmin = (2.*PI-FI0)/KAPPA;
    }
    deltaz = zmax-zmin;

     for(i=0; i<TemporarynSttSkewhitinTrack; i++){
       bbb=(S[i]-FI0)/KAPPA;
      for(sign=0;sign<=1; sign ++){
       tempZ[sign]=Z[i]+(2*sign-1)*ZDrift[i];
       if( tempZ[sign] > zmax ){
         tempZ[sign]=fmod( tempZ[sign]-zmax, deltaz) + zmin;
       } else if (tempZ[sign]<zmin){
         tempZ[sign]=fmod( tempZ[sign]-zmin, deltaz) + zmax;
       }
	zdist1 = fabs( bbb - tempZ[sign]);
	zdist2 = deltaz- zdist1;
	if(zdist2<0.) zdist2 = 0.;  // protect against rounding errors.
	zdist[sign] = zdist1 < zdist2 ? zdist1 : zdist2;
      }  //  end of for(sign=0;sign<=1; sign++)
	zdist1 = zdist[0] < zdist[1] ? zdist[0] : zdist[1];
	if(  zdist1 < allowed_distance ){
         tempore[NAssociated]=SkewList[ i ][0];
         temporeS[NAssociated]=S[i];
         temporeZ[NAssociated]=Z[i];
         temporeZDrift[NAssociated]=ZDrift[i];
         temporeZErrorafterTilt[NAssociated]=ZErrorafterTilt[i];
         NAssociated++;
       }
     }   //  end of for(i=0; i<TemporarynSttSkewhitinTrack; i++)

     *STATUS=0;
     return NAssociated;
 }

//----------end of function PndTrkTracking::AssociateBetterAfterFitSkewHitsToXYTrack



//----------begin of function PndTrkTracking::AssociateSkewHitsToXYTrack

Short_t PndTrkTracking::AssociateSkewHitsToXYTrack(
	bool *InclusionListSkew,
	Short_t NSkewhits,
	Short_t *infoskew,  // from skew numbering to original Stt hit numbering;
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t info[][7],
	Double_t *WDX,
	Double_t *WDY,
	Double_t *WDZ,
	Double_t Fi_low_limit,
	Double_t Fi_up_limit,
	Short_t  Charge,
	Short_t SkewList[][2], // output,list of selected skew hits (original numbering)
	Double_t *S,       //  output,  S coordinate of selected Skew hit
	Double_t *Z,       //  output,  Z coordinate of selected Skew hit
	Double_t *ZDrift,   //  output,  drift distance IN Z DIRECTION only,
			   // of selected Skew hit
	Double_t *ZErrorafterTilt   //  output,  Radius taking into account
				// the tilt, IN Z DIRECTION only, of selected Skew hit.
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


 PndTrkCTGeometryCalculations GeomCalculator;


//   calculate the Fi range allowed to the skew hits, with Fi calculated in the TRACK CYLINDER REFERENCE FRAME.


      NAssociated=0;

       for( iii=0; iii< NSkewhits; iii++) {
         i = infoskew[iii];
         if( !InclusionListSkew[i]) continue;


         aaa = sqrt(WDX[i]*WDX[i]+WDY[i]*WDY[i]+WDZ[i]*WDZ[i]);
         vx1 = WDX[i]/aaa;
         vy1 = WDY[i]/aaa;
         vz1 = WDZ[i]/aaa;
         C0x1 = info[i][0];
         C0y1 = info[i][1];
         C0z1 = info[i][2];

       GeomCalculator.calculateintersections(Oxx,Oyy,Rr,C0x1,C0y1,C0z1,info[i][3],
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


        Rx = POINTS1[j]-Oxx ;   //  x component Radial vector of cylinder of trajectory
        Ry = POINTS1[1+j]-Oyy ;   //  y direction Radial vector of cylinder of trajectory

        aaa = sqrt(Rx*Rx+Ry*Ry);
        SkewInclWithRespectToS = (-Ry*vx1 + Rx*vy1)/aaa ;
        SkewInclWithRespectToS /= Rr;
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

        Bellipsis1 = info[i][3]/Rr;

        if( distance >= info[i][4]+Aellipsis1 ){
		 continue;
	}

        S[NAssociated] = atan2(POINTS1[j+1]-Oyy, POINTS1[j]-Oxx) ;  // atan2 returns radians in (-pi and +pi]
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

//----------end of function PndTrkTracking::AssociateSkewHitsToXYTrack


//------------------------- begin of function  PndTrkTracking::CollectParSttHitsagain

 void PndTrkTracking::CollectParSttHitsagain(
	bool *keepit,
	bool *Mvdhits,
	Double_t info[][7],
	Short_t nSttParHit,
	// starting investigation from candidate n. StartTrackCand
	Short_t StartTrackCand,
	//  until candidate n. EndTrackCand-1 included.
	Short_t EndTrackCand,
	Double_t *KAPPA,
	Double_t *FI0,
	Double_t *Fi_low_limit,
	Double_t *Fi_up_limit,
	Short_t *nParHitsinTrack, // input/output
	Short_t ListParHitsinTrack[][MAXSTTHITSINTRACK] // input/output
						)
{

 Short_t i,
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
	if( Fi_low_limit[itrack] < -99998.) continue;  // case in which
		// the track is completely inside the Mvd region.
	nParHitsinTrack[itrack]=0;

//	loop over the STT parallel hits and try to attach to each candidate track; in this
//	way in one shot I collect also the previously non collected hits and I remove the
//	spurious hits.

	nadd=0;
	for(i=0; i<nSttParHit; i++){
		ihit = ListSttParHits[i];

		if( !SingleHitListStt[ihit] ) continue;
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

//------------------------- end of function  PndTrkTracking::CollectParSttHitsagain



//-------------------------  begin of function  PndTrkTracking::EliminateSpuriousSZ

void PndTrkTracking::EliminateSpuriousSZ(
	Short_t MaxTurnofTracks,
	Short_t *nPixelHitsinTrack,
	Short_t *ListPixelHitsinTrack,
	Short_t *nStripHitsinTrack,
	Short_t *ListStripHitsinTrack,
	Short_t *nSkewHitsinTrack,
	Short_t *ListSkewHitsinTrack,
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
	Double_t Rr
	)
{

	Short_t i,
		 j,
		 k,
		 Kmin,
		 auxnMvdPixel,
		 auxListMvdPixel[MAXMVDPIXELHITS],
		 auxnMvdStrip,
		 auxListMvdStrip[MAXMVDSTRIPHITS],
		 auxnSttSkew,
		 auxListSttSkew[MAXSTTHITS];

	Int_t  nr2,
		 nrounds0,
		 nrounds1,
		 nchosen,
		 Nround[4];

	const Double_t  MvdCut=0.8,
//	const Double_t  MvdCut=0.3,
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

 PndTrkCTGeometryCalculations GeomC;

	auxnMvdPixel=0;
	auxnMvdStrip=0;
	auxnSttSkew=0;


	for(i=0;i<*nPixelHitsinTrack;i++){
		if( fabs(GeomC.Dist_SZ(Rr,KAPPA,FI0,ZED[i],S[i],&nrounds0)) < MvdCut
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

		if( fabs(GeomC.Dist_SZ(Rr,KAPPA,FI0,ZED[i],S[i],&nrounds0)) < MvdCut
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
if(istampa>3){
	cout<<"in EliminateSpuriousSZ : nSkew hit = "<<*nSkewHitsinTrack
	<<", K = "<< KAPPA<<", FI0 = "<< FI0 <<endl;
}
//--------------fine stampe.

	for(j=0;j<*nSkewHitsinTrack;j++){
		i=j+(*nPixelHitsinTrack)+
		    (*nStripHitsinTrack);


//-----------------stampe.
if(istampa>3){
	cout<<"in EliminateSpuriousSZ : skew hit n. "<<ListSkewHitsinTrack[j];
	cout<<"  ZED[i] = "<<ZED[i]<<", S[i] "<<S[i];
	cout<<endl;
}
//--------------fine stampe.
			dista =
		fabs(GeomC.Dist_SZ(Rr,KAPPA,FI0,ZED[i]+DriftRadius[i],S[i],&nrounds0));
			ddd = fabs(GeomC.Dist_SZ(Rr,KAPPA,FI0,ZED[i]-DriftRadius[i],S[i],&nr2));
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
if(istampa>3){
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
//-------------------------  end of function  PndTrkTracking::EliminateSpuriousSZ




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


//----------begin of function PndTrkTracking::FindCharge

void PndTrkTracking::FindCharge(
	Double_t oX,
	Double_t oY,
	Short_t nParallelHits,
	Double_t *X,
	Double_t *Y,
	Short_t  * Charge
	)
{

	Short_t ihit,
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

//----------end of function PndTrkTracking::FindCharge





//----------start  function PndTrkTracking::FindingParallelTrackAngularRange

void  PndTrkTracking::FindingParallelTrackAngularRange(
	Double_t oX,
	Double_t oY,
	Double_t Rr,
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
	if(a + Rr <= Rmi )	// in this case there might be hits at radius < Rmi.
		 { *status = -1 ;return;}
	if( a >= Rr + Rma || Rr >= a + Rma)  // in this case there can be no hits with radius < Rma.
		 { *status = -2;return;}

	if( a - Rr >= Rmi ) intersection_inner = false; else intersection_inner = true;

	if( a + Rr <= Rma || a - Rr >= Rma  )
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
		cosFi = (a*a + Rr*Rr - Rma*Rma)/(2.*Rr*a);
		if(cosFi<-1.) cosFi=-1.; else if(cosFi>1.) cosFi=1.;
		Fi = acos(cosFi);
	}

	if( intersection_inner ){
		cosfi = (a*a + Rr*Rr - Rmi*Rmi)/(2.*Rr*a);
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


//---------- end of  function PndTrkTracking::FindingParallelTrackAngularRange



//----------start of function PndTrkTracking::FixDiscontinuitiesFiangleinSZplane

void PndTrkTracking::FixDiscontinuitiesFiangleinSZplane(
	Short_t TemporarynSkewHitsinTrack,
	Double_t *S,
	Double_t *Fi_initial_helix_referenceframe,
	Short_t Charge
	)
{

 Short_t i;
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
//----------end of function PndTrkTracking::FixDiscontinuitiesFiangleinSZplane


//---------- begin of function PndTrkTracking::InfoXYZParal

void PndTrkTracking::InfoXYZParal(
	Double_t info[][7],
	Short_t infopar,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t KAPPA,
	Double_t FI0,
	Short_t Charge,
	Double_t *Posiz
	)
{

   Double_t fi, norm, vers[2];

   vers[0] = Oxx - info[infopar][0];
   vers[1] = Oyy - info[infopar][1];
   norm = sqrt( vers[0]*vers[0] + vers[1]*vers[1] );

   if(norm < 1.e-20) {
     Posiz[0] = -999999999.;
     return;
   }
   





   if( fabs( Rr - fabs( norm - info[infopar][3] ) ) // distance trajectory-drift radius
				<
		fabs( Rr - (norm + info[infopar][3]) )  ) {

	Posiz[0] = info[infopar][0] + info[infopar][3]*vers[0]/norm;
	Posiz[1] = info[infopar][1] + info[infopar][3]*vers[1]/norm;

   } else {

	Posiz[0] = info[infopar][0] - info[infopar][3]*vers[0]/norm;
	Posiz[1] = info[infopar][1] - info[infopar][3]*vers[1]/norm;

   }	// end of if ( fabs( Rr - fabs( Distance - info[infopar][3] ) ).....




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

//----------end of function PndTrkTracking::InfoXYZParal




//----------------------  begin function   PndTrkTracking::LoadPndTrack_TrackCand

void PndTrkTracking::LoadPndTrack_TrackCand(
	bool *keepit,
	bool *SttSZfit,
	Short_t nTotalCandidates,
	Short_t *Charge,
	Int_t nSttTrackCand,
	Double_t *FI0,
	Double_t *KAPPA,
	Double_t info[][7],
	Double_t SchosenSkew[][MAXSTTHITS],
	Double_t ZchosenSkew[][MAXSTTHITS]
			)
{

//-------  load the new PndTrackCand ; each track has the STT and the Mvd hits associated
//-------  also load the new PndTrack ; each track has the STT and the Mvd hits associated

 Int_t
	ipinco,
	j,
	k,
	l,
	ncand
	;

 Double_t
	ddd,
	dis,
	Distance,
	Oxx,
	Oyy,
	Ptras,
	Pxini,
	Pyini,
	Pzini,
	px,
	py,
	qop,
	x,
	y,
	Posiz1[3],
	versor[2]
	;

 TVector3
	ErrMomentum,
	ErrPosition,
	Momentum,
	Position
	;


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
	Pxini = -Charge[ncand]*Ptras*Oyy/dis;
	Pyini = Charge[ncand]*Ptras*Oxx/dis;

//   starting point not necessarily at x=0., y=0.

	x= Ox[ncand] + R[ncand]*cos(FI0[ncand]);
	y= Oy[ncand] + R[ncand]*sin(FI0[ncand]);
	TVector3 posSeed(x,y,0.);  //  the starting point

	if(fabs(KAPPA[ncand])>1.e-20  ){
		Pzini = -Charge[ncand]*0.003*BFIELD/KAPPA[ncand];
		if(fabs(Pzini) > PMAX)  continue;
	} else {
		continue;
	}
	// PndTrackCand Array loading
	new((*fSttMvdPndTrackCandArray)[ipinco])  PndTrackCand;
	PndTrackCand *pTrckCand = (PndTrackCand*) fSttMvdPndTrackCandArray->At(ipinco);
	TVector3 dirSeed(Pxini,Pyini,Pzini); // momentum direction in starting point
	qop = Charge[ncand]/dirSeed.Mag();
	dirSeed.SetMag(1.);
	pTrckCand->setTrackSeed(posSeed, dirSeed, qop);
	pTrckCand->setMcTrackId(  -1   );

	for(j=0; j< nTrackCandHit[ncand]; j++){
	     switch (ListTrackCandHitType[ncand][j]){
		case 0:
			pTrckCand->AddHit(FairRootManager::Instance()->
				GetBranchId(fMvdPixelBranch),
				(Int_t)ListTrackCandHit[ncand][j],j);
		break;
		case 1:
			pTrckCand->AddHit(FairRootManager::Instance()->
				GetBranchId(fMvdStripBranch),
				(Int_t)ListTrackCandHit[ncand][j],j);
		break;
		case 2:
			pTrckCand->AddHit(FairRootManager::Instance()->
				GetBranchId(fSttBranch),
				(Int_t)ListTrackCandHit[ncand][j],j);
		break;
		case 3:
			pTrckCand->AddHit(FairRootManager::Instance()->
				GetBranchId(fSttBranch),
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
		InfoXYZParal(
			info,
			ListTrackCandHit[ncand][0],
			Ox[ncand],
			Oy[ncand],
			R[ncand],
			KAPPA[ncand],
			FI0[ncand],
			Charge[ncand],
			Posiz1
			);

		// cases in which the calculation of the position failed, see
		// InfoXYZParal  method.
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
	px = -Charge[ncand]*Ptras*versor[1];
	py = Charge[ncand]*Ptras*versor[0];
	Momentum.SetX(px);
	Momentum.SetY(py);
	Momentum.SetZ(Pzini);
	ErrMomentum.SetX(0.05*Ptras); //  set at 5% all the times.
	ErrMomentum.SetY(0.05*Ptras); //  set at 5% all the times.
	ErrMomentum.SetZ(0.05*Pzini); //  set at 5% all the times.
	//  the plane of this FairTrackParP better is perpendicular to
	//  the momentum direction
	ddd = Ptras*sqrt(Ptras*Ptras+Pzini*Pzini);

	FairTrackParP first( Position,  Momentum,
	   ErrPosition, ErrMomentum, Charge[ncand],
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
		InfoXYZParal (
			info,
			ListTrackCandHit[ncand][k],
			Ox[ncand],
			Oy[ncand],
			R[ncand],
			KAPPA[ncand],
			FI0[ncand],
			Charge[ncand],
			Posiz1
			);
		if(Posiz1[2]<-888888887. || Posiz1[0] < -999999998.)
			continue;

		ErrPosition.SetX(0.02);	// 200 microns
		ErrPosition.SetY(0.02);	// 200 microns
		ErrPosition.SetZ(1.);	// 1 cm

	} else if ( ListTrackCandHitType[ncand][k] == 3 ){  // it is a skew straw hit

		Posiz1[0] = Ox[ncand]+R[ncand]*cos(SchosenSkew[ncand][ ListTrackCandHit[ncand][k] ]);
		Posiz1[1] = Oy[ncand]+R[ncand]*sin(SchosenSkew[ncand][ ListTrackCandHit[ncand][k] ]);
		Posiz1[2] = ZchosenSkew[ncand][ ListTrackCandHit[ncand][k] ];
		ErrPosition.SetX(0.02);	// 200 microns
		ErrPosition.SetY(0.02);	// 200 microns
		ErrPosition.SetZ(1.);	// 1 cm

	}

	Position.SetX( Posiz1[0] );
	Position.SetY( Posiz1[1] );
	Position.SetZ( Posiz1[2] );
	versor[0] = Ox[ncand]-Posiz1[0];
	versor[1] = Oy[ncand]-Posiz1[1];
	Distance = sqrt(versor[0]*versor[0]+versor[1]*versor[1]);
	versor[0] /= Distance;
	versor[1] /= Distance;
	px = -Charge[ncand]*Ptras*versor[1];
	py = Charge[ncand]*Ptras*versor[0];
	Momentum.SetX(px);
	Momentum.SetY(py);
	Momentum.SetZ(Pzini);
	//  the plane of this FairTrackParP better is perpendicular to
	//  the momentum direction
	ddd = Ptras*sqrt(Ptras*Ptras+Pzini*Pzini);



	FairTrackParP last( Position,  Momentum,
	   ErrPosition, ErrMomentum, Charge[ncand],
	   Position,
	   TVector3(py/Ptras, -px/Ptras, 0.), //  first vector defining the plane
	   TVector3(Pzini*px/ddd,Pzini*py/ddd,-Ptras*Ptras/ddd) //second vector defining the plane
				);



	// loading actually the PndTrack
	PndTrack *pTrck = new((*fSttMvdPndTrackArray)[ipinco]) PndTrack(first,last,*pTrckCand);
	pTrck->SetRefIndex(ipinco);
	pTrck->SetFlag(0);

	ipinco++;
 }	// end of     for(ncand=0, ipinco = 0; ncand< nTotalCandidates; ncand++)


}


//----------end function PndTrkTracking::LoadPndTrack_TrackCand



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



//------------------------- begin of function  PndTrkTracking::MatchMvdHitsToSttTracksagain

void PndTrkTracking::MatchMvdHitsToSttTracksagain(
	bool *keepit,
	bool *Mvdhits,
	Double_t delta,
	Double_t highqualitycut,
	Short_t nSttTrackCand,
	Double_t *FI0,
	Double_t *Fifirst,
	Short_t *CHARGE,

	Short_t *nPixelHitsinTrack, // output
	Short_t ListPixelHitsinTrack[][MAXMVDPIXELHITSINTRACK], // output
	Short_t *nStripHitsinTrack, // output
	Short_t ListStripHitsinTrack[][MAXMVDSTRIPHITSINTRACK] // output
	)
{
	bool
//		flaggo,
		specialcase,
	     downstream,
	     determined;

	Short_t j,
		itrack,
		ipix,
		istr,
		ndownstream,
		ntot,
		naddpix,
		naddstr,
		List[MAXMVDPIXELHITS+MAXMVDSTRIPHITS];

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
//	if( ! Mvdhits[itrack] ) continue;
	ntot=nPixelHitsinTrack[itrack]+nStripHitsinTrack[itrack];
	if( Fifirst[itrack] < -99998. ){  // case with Fifirst[i]=-99999.; in this
					// case the circle is contained
					// in the Mvd region.
		anglemax = 2.*PI;
		anglemin = 0.;
	} else {
		if(CHARGE[itrack]>0){
			// track must rotate clockwise looking into the beam.
			anglemax = FI0[itrack];
			anglemin = Fifirst[itrack];
		} else {
			anglemin = FI0[itrack];
			anglemax = Fifirst[itrack];
		}
		if(anglemax < anglemin) anglemax += 2.*PI;
		if(anglemax < anglemin) anglemax=anglemin;// just to be super-sure.

	} // end of if( Fifirst[itrack] < -99998. )


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
	if(ndownstream>ntot-ndownstream) downstream=true;
	else downstream=false;

//  loop over the Mvd Pixel and try to attach new Pixels to each candidate track

	naddpix=0;
	for(ipix=0; ipix<nMvdPixelHit; ipix++){

/*
		flaggo=true;
		for(j=0; j<nPixelHitsinTrack[itrack]; j++){
		  if( ListPixelHitsinTrack[itrack][j]==ipix){
			flaggo=false;
			break;
		  }
		}
*/

//		if(flaggo){
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
		  if(angle > anglemin && angle < anglemax){
			dist=fabs( sqrt(
			(Ox[itrack]-XMvdPixel[ipix])*(Ox[itrack]-XMvdPixel[ipix])
			+(Oy[itrack]-YMvdPixel[ipix])*(Oy[itrack]-YMvdPixel[ipix])
			) -R[itrack]);
			if(dist<highqualitycut){
				List[naddpix]=ipix;
				naddpix++;
			}
		  }	// end of  if(angle > anglemin)
//		} // end of if(flaggo)

	}	//  end of   for(ipix=0; ipix<nMvdPixelHit; ipix++)

	if(naddpix>0){
		if(naddpix>MAXMVDPIXELHITSINTRACK){
			// protection against strange tracks (and also from
			// out-of-bound indexing of arrays);
			naddpix=MAXMVDPIXELHITSINTRACK;
		}  // 
		for(j=0;j<naddpix;j++){
			ListPixelHitsinTrack[itrack][j]=List[j];
		}
		nPixelHitsinTrack[itrack] = naddpix;
	}	//  end of  if(naddpix>0)


	naddstr=0;
	for(istr=0; istr<nMvdStripHit; istr++){
/*
		flaggo=true;
		for(j=0; j<nStripHitsinTrack[itrack]; j++){
		     if( ListStripHitsinTrack[itrack][j]==istr){
			flaggo=false;
			break;
		     }
		}
*/
//		if(flaggo){
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
		  if(angle > anglemin && angle < anglemax){
			dist=fabs( sqrt(
			  (Ox[itrack]-XMvdStrip[istr])*(Ox[itrack]-XMvdStrip[istr])
			  +(Oy[itrack]-YMvdStrip[istr])*(Oy[itrack]-YMvdStrip[istr])
				) -R[itrack]);

			if(dist<highqualitycut){
				List[naddstr]=istr;
				naddstr++;
			}
		  }	//  end of  if(angle > anglemin)

//		} // end of if(flaggo)

	}	//  end of   for(istr=0; istr<nMvdStripHit; istr++)
	if(naddstr>0){
		if(naddstr>MAXMVDSTRIPHITSINTRACK){
			// protection against strange tracks (and also from
			// out-of-bound indexing of arrays);
			naddstr=MAXMVDSTRIPHITSINTRACK;
		}  // 
		for(j=0;j<naddstr;j++){
			ListStripHitsinTrack[itrack][j]=List[j];
		}
		nStripHitsinTrack[itrack] = naddstr;
	}	//  end of  if(naddpix>0)


	if(nPixelHitsinTrack[itrack]+nStripHitsinTrack[itrack]>0)
		Mvdhits[itrack]=true;

 }	// end of for(itrack=0; itrack<nSttTrackCand; itrack++)

 return;
}

//------------------- end function  PndTrkTracking::MatchMvdHitsToSttTracksagain




//------------------------- begin of function  PndTrkTracking::MatchMvdHitsToSttTracks2

void PndTrkTracking::MatchMvdHitsToSttTracks2(
	bool *keepit,
	Double_t delta,
	Double_t highqualitycut,
	Short_t nSttTrackCand,
	Double_t *FI0,
	Double_t *Fifirst,
	Short_t *CHARGE,
	Short_t *nPixelHitsinTrack, // output
	Short_t ListPixelHitsinTrack[][MAXMVDPIXELHITSINTRACK], // output
	Short_t *nStripHitsinTrack, // output
	Short_t ListStripHitsinTrack[][MAXMVDSTRIPHITSINTRACK] // output
	)
{
	bool specialcase;

	Short_t i,j,j1, j2, imvdcand, jmvdhit, ncont,
		chosenmix,
		chosenmix2,
		ngoodmix,
		oldN,
		nn[MAXMVDTRACKSPEREVENT+2],
		nHighQuality[MAXMVDTRACKSPEREVENT+2],
		List[MAXMVDTRACKSPEREVENT+2][MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK],
		ListType[MAXMVDTRACKSPEREVENT+2][MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK];

	Double_t angle,
		anglemax,
		anglemin,
		dist,
		oldtotal,
		oldtotal2,
		total,
		Dist,
		DIST[MAXMVDTRACKSPEREVENT+1];



 for(i=0; i<nSttTrackCand; i++){
	if( ! keepit[i] ) continue ;

	if( Fifirst[i] < -99998. ){  // case with Fifirst[i]=-99999.; in this
					// case there the circle is contained
					// in the Mvd region.
		anglemax = 2.*PI;
		anglemin = 0.;

	} else { // continuation of if( Fifirst[i] < -99998. )


	if(CHARGE[i]>0){	// track must rotate clockwise looking into the beam.
		anglemax = FI0[i];
		anglemin = Fifirst[i];
	} else {
		anglemin = FI0[i];
		anglemax = Fifirst[i];
	}
	if(anglemax < anglemin) anglemax += 2.*PI;
	if(anglemax < anglemin) anglemax=anglemin; // this is just to be super-sure.

	} // end of if( Fifirst[i] < -99998. )


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


if(istampa>=3 ){cout<<"da PndTrkTracking : appena prima arbitration, IVOLTE = "<<
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
if(istampa>=3 ){cout<<"da PndTrkTracking : goodmix n. 0, total distance (che e' = total distance2) = "<<oldtotal
				<<", e nHighQuality = "<<nHighQuality[0]<<endl;}
			chosenmix=0;
			chosenmix2=0;
			for(j1=1; j1<ngoodmix;j1++){
				total = DIST[j1];
if(istampa>=3){cout<<"da PndTrkTracking :\t goodmix n. "<<j1<<", total distance "<<total
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
if(istampa>=3 ){cout<<"da PndTrkTracking : fine arbitration, IVOLTE = "<<
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

//------------------------- end of function  PndTrkTracking::MatchMvdHitsToSttTracks2


//------begin function PndTrkTracking::OrderingConformal_Loading_ListTrackCandHit
void PndTrkTracking::OrderingConformal_Loading_ListTrackCandHit(
	bool *keepit,
	Short_t ncand,
	Double_t info[][7],
	Double_t Trajectory_Start[][2],
	Short_t *CHARGE,
	Double_t SchosenSkew[][MAXSTTHITS]
				)
{

	Short_t	i,
			j,
			ipar,
			iskew;


//     ordering all the hits belonging to the candidate track, by increasing R;
//     forming the new track with Mvd+Stt hits


	// arrays used to store temporarily the info of Mvd hits to be ordered.
	Int_t ListHits[MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK],
		ListHits2[MAXSTTHITSINTRACK];
	Double_t XY[MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK][2],
		XY2[MAXSTTHITSINTRACK][2];

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
				(MAXMVDPIXELHITS+MAXMVDSTRIPHITS)*10 ;
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
				if(ListHits[i]<(MAXMVDPIXELHITS+MAXMVDSTRIPHITS)*10){//Pixel.
					ListTrackCandHit[ncand][i] = ListHits[i];
					ListTrackCandHitType[ncand][i] = 0;
				} else { // Strip hits.
					ListTrackCandHit[ncand][i] = ListHits[i]-
						(MAXMVDPIXELHITS+MAXMVDSTRIPHITS)*10;
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
			 XY2[j][0] = Ox[ncand]+R[ncand]*
				cos(SchosenSkew[ncand][ListSttSkewHitsinTrack[ncand][i]]);
			 XY2[j][1] = Oy[ncand]+R[ncand]*
				sin(SchosenSkew[ncand][ListSttSkewHitsinTrack[ncand][i]]);
			 ListHits2[j] = ListSttSkewHitsinTrack[ncand][i]+
					MAXSTTHITS*10; // in order to distinguish
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
			  if(ListHits2[j]<MAXSTTHITS*10){  // parallel Stt hit.
			    ListTrackCandHit[ncand][i] = ListHits2[j];
			    ListTrackCandHitType[ncand][i] = 2;
			    ListSttParHitsinTrack[ncand][ipar]=ListHits2[j];
			    ipar++;
			  } else {  // skew Stt hit.
			    ListTrackCandHit[ncand][i] = ListHits2[j]-MAXSTTHITS*10;
			    ListTrackCandHitType[ncand][i] = 3;
			    ListSttSkewHitsinTrack[ncand][iskew]=ListHits2[j]-MAXSTTHITS*10;
			    iskew++;
			  }  // end of  if(ListHits2[j]<MAXSTTHITS*10)
			} // end of for(j=0,ipar=0,iskew=0; ....

		}	// end of  if( nSttParHitsinTrack[ncand]+


if(istampa>=3) for(int ica=0; ica<nMvdPixelHitsinTrack[ncand]+nMvdStripHitsinTrack[ncand]+
	nSttParHitsinTrack[ncand]+nSttSkewHitsinTrack[ncand]; ica++){
	cout<<"from PndTrkTracking, hit n. "<<ListTrackCandHit[ncand][ica]<<", hit type "
	<<ListTrackCandHitType[ncand][ica]<<endl;
}


//	} //   end of  for(ncand=FirstCandidate; ncand< LastCandidate; ncand++)


	return;
}

//--------end function PndTrkTracking::OrderingConformal_Loading_ListTrackCandHit



//----------begin of function PndTrkTracking::Ordering_Loading_ListTrackCandHit

void PndTrkTracking::Ordering_Loading_ListTrackCandHit(
	bool *keepit,
	Short_t FirstCandidate,
	Short_t LastCandidate,
	Double_t info[][7],
	Double_t Trajectory_Start[][2],
	Short_t *CHARGE,
	Double_t SchosenSkew[][MAXSTTHITS]
				)
{
	Short_t ncand;

	for(ncand=FirstCandidate; ncand< LastCandidate; ncand++){
		// for small radius trajectory better the ordering with conformal.


		if( R[ncand]< RSTRAWDETECTORMAX/2.){
			OrderingConformal_Loading_ListTrackCandHit(
				keepit,
				ncand,
				info,
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
//----------end of function PndTrkTracking::Ordering_Loading_ListTrackCandHit



//----------begin of function PndTrkTracking::OrderingR_Loading_ListTrackCandHit

void PndTrkTracking::OrderingR_Loading_ListTrackCandHit(
	bool *keepit,
	Short_t ncand,
	Double_t info[][7]
	)
{

 Short_t	i,
		j,
		ipar,
		iskew;

 PndTrkMergeSort MergeSort;

//     ordering all the hits belonging to the candidate track, by increasing R;
//     forming the new track with Mvd+Stt hits


		if(!keepit[ncand]) return;
		nTrackCandHit[ncand] =nSttParHitsinTrack[ncand]+nSttSkewHitsinTrack[ncand]+
					nMvdPixelHitsinTrack[ncand]+
					nMvdStripHitsinTrack[ncand];
		Short_t tempmvdindex[nMvdPixelHitsinTrack[ncand]+
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
			MergeSort.Merge_Sort( nMvdPixelHitsinTrack[ncand]+
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

		Short_t tempmvdindex2[nSttParHitsinTrack[ncand]+
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
			MergeSort.Merge_Sort( nSttParHitsinTrack[ncand]+nSttSkewHitsinTrack[ncand],
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

//----------end of function PndTrkTracking::OrderingR_Loading_ListTrackCandHit



//----------begin of function PndTrkTracking::OrderingSttSkewandSttParallel

 void PndTrkTracking::OrderingSttSkewandSttParallel(
			Double_t oX,
			Double_t oY,
			Double_t Rr,
			Short_t nSkewhit,
			Short_t *ListSkewHits,
			Double_t *SList, // this is rekated to the skew hits. IMPORTANT :
					// the index must be the ORIGINAL skew hit number,
					// therefore SList[Infoskew[ListSkewHits[*]]].
			Short_t  Charge,
			Short_t nParHits,
			Short_t *ListParHits,
			Double_t *U,
			Double_t *V,
			Short_t *BigList // this is the final ordered Parallel+Skew list;
				// already in NATIVE hit number.
				)
{

 Short_t
	i,
	j,
	tmp[nSkewhit+nParHits],
	tmpList[nSkewhit];

 Int_t	index[nSkewhit+nParHits];

 Double_t	aaa,
		b1,
		sign,
		aux[nSkewhit+nParHits];


 PndTrkMergeSort MergeSort;


//  here there is the ordering of the hits, under the assumption that the circumference
//  in XY goes through (0,0).
//  Moreover, the code before is supposed to have selected trajectories in XY with (Ox,Oy)
//  farther from (0,0) by > 0.9 * RminStrawDetector/2 and consequently Ox and Oy are not both 0.
//  The scheme for the ordering of the hit is as follows :
//  1)  order hits by increasing U of the conformal mapping; see Gianluigi's Logbook page 283;
//  2)  find the charge of the track by checking if it is closest to the center in XY
//	the first or the last of the ordered hits.



//   ordering of the hits

	aaa = atan2( oY, oX);  // atan2 defined between -PI and PI.

	// the following statement is necessary since for unknown reason the root interpreter
	// gives a weird error when using PI directly in the if statement below!!!!!!! I lost
	// 2 hours trying to figure this out!
	b1 = PI/4.;


	if(aaa>b1&&aaa<3.*b1|| (aaa>-3.*b1&&aaa<-b1)){  //  case #1 or #3;see Gianluigi's Logbook page 285.
		if( (aaa>b1&&aaa<3.*b1 && Charge == -1)||( aaa>-3.*b1&&aaa<-b1 && Charge == 1) )
				{  // for speeding up the ordering taking advantage
				    // that the parallel hits were earlier ordered and
				    //  apply the trick of multiplying by   -1.
			sign=-1.;
		} else {  //  normal calculation
			sign=1.;
		}

		for (j = 0 ; j< nParHits; j++){
			aux[j] = sign*U[j];
		}
		for (j = 0; j< nSkewhit; j++){
			// this is U in conformal space
			aux[j+nParHits]=sign*(oX + Rr*cos(SList[ListSkewHits[j]]))/
					(oX*oX+oY*oY+Rr*Rr + 2.*Rr*
					(oX*cos(SList[ListSkewHits[j]])
					+oY*sin(SList[ListSkewHits[j]])));
		}


	} else {    // use V as ordering variable
		    // [case 2 and 4 Gianluigi's Logbook page 285].

		if( ((aaa<=-3.*b1|| aaa>=3.*b1) && Charge == -1)
				|| ( -b1 <= aaa && aaa <= b1 && Charge == 1) ){
			sign=-1.;
		} else {
			sign=1.;
		}
		for (j = 0 ; j< nParHits; j++){
			aux[j] = sign*V[j];
		}
		for (j = 0; j< nSkewhit; j++){
			// this is V in conformal space.
			aux[j+nParHits]=sign*(oY + Rr*sin(SList[ListSkewHits[j]]))/
			(oX*oX+oY*oY+Rr*Rr + 2.*Rr*
				(oX*cos(SList[ListSkewHits[j]])
				+oY*sin(SList[ListSkewHits[j]])));
		}


	}  //  end of  if((aaa>b1&& ....


	for (j = 0 ; j< nParHits; j++){
		BigList[j]=ListParHits[j];
		index[j] = j;
	}
	for (j = 0; j< nSkewhit; j++){
		BigList[j+nParHits]=ListSkewHits[j];
		index[j+nParHits] = j+nParHits;
	}

	MergeSort.Merge_Sort( nSkewhit+nParHits, aux, index);


	for(i=0, j=0;i<nSkewhit+nParHits;i++){
		tmp[i]=BigList[index[i]];
		//  reorder the ListSkewHits also.
		if( index[i] >= nParHits ){
			tmpList[j] = ListSkewHits[index[i]-nParHits];
			j++;
		}
	}
	for(i=0;i<nSkewhit+nParHits;i++){
		BigList[i]= tmp[i];
	}
		//  reorder the ListSkewHits also.
	for(i=0;i<nSkewhit;i++){
		ListSkewHits[i]= tmpList[i];
	}


 return;

}
//----------end of function PndTrkTracking::OrderingSttSkewandSttParallel


//----------begin of function PndTrkTracking::OrderingUsingConformal

void   PndTrkTracking::OrderingUsingConformal(
	Double_t oX,
	Double_t oY,
	Double_t Traj_Sta[2],
	Int_t nHits,
	Double_t XY[][2],
	Short_t  Charge,  // input
	Int_t *ListHits
	)
{




      Short_t	i,j, 
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


 PndTrkMergeSort MergeSort;



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
		MergeSort.Merge_Sort( nHits, U, ListHits);

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
		MergeSort.Merge_Sort( nHits, V, ListHits);

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
//----------end of function PndTrkTracking::OrderingUsingConformal


//------------------ begin function  PndTrkTracking::RefitMvdStt

void  PndTrkTracking::RefitMvdStt(
	Short_t nCandHit,
	Short_t *ListCandHit,
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

	Short_t	i,
			iparallel;


	Short_t	exitstatus;

	Double_t dist2,
		 mindis,
		 emme,
		 factor,
		 gamma,
		 qu,
		 ErrorStraw = 0.03,
		 ErrorMvd = 0.01,
Xconformal[MAXSTTHITSINTRACK+MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK],
Yconformal[MAXSTTHITSINTRACK+MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK],
DriftRadiusconformal[MAXSTTHITSINTRACK+MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK],
ErrorDriftRadiusconformal[MAXSTTHITSINTRACK+MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK];


 PndTrkGlpkFits fit;


	*status= false;
	factor=3.;
	mindis=0.5;
//	trajectory_vertex[0]=trajectory_vertex[1]=0.;

	for(i=0, iparallel=0; i<nCandHit && iparallel< MAXHITSINFIT; i++){

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
			// this is to exclude hits too close to the traslated
			// center of reference frame (given by tv[0], tv[1]). This
			// would cause the subsequent fit to fail.
			if(dist2<mindis) continue;
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

	exitstatus = fit.FitHelixCylinder(
					iparallel,
					Xconformal,
					Yconformal,
					DriftRadiusconformal,
					ErrorDriftRadiusconformal,
					rotationangle,  //  rotationangle, da mettere
					tv,	//  vertex in (X,Y) of this trajectory
					MAXHITSINFIT,  //  maximum n. of hits allowed in fast fit
					&emme,
					&qu,
					pAlfa,
					pBeta,
					pGamma,
					&Type,
					0, // istampa
					0 //  IVOLTE
					);

	//  existatus > 0, Type= true --> fit ok, it is a Circle in XY; 
	//  existatus > 0, Type= false --> fit ok, it is a Straigh Line in XY; 
	//  existatus = -1, fit failed, equation of XY circle : X**2 + Y**2 =0, impossible in principle; 
	//  existatus < 0, fit failed. 
	if( exitstatus > 0 && Type)	*status=true;
	return;
}


//------------------ end function  PndTrkTracking::RefitMvdStt


ClassImp(PndTrkTracking)

