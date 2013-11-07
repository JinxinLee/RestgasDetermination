#include "PndTrkTracking2.h"
#include "PndTrkBoundaryParStraws2.h"
#include "PndTrkChi2Fits.h"
#include "PndTrkComparisonMCtruth.h"
#include "PndTrkSttConformalFilling.h"
#include "PndTrkLegendreFits.h"
#include "PndTrkCleanup.h"
#include "PndTrkCTFindTrackInXY.h"
#include "PndTrkCTFindTrackInXY2.h"
#include "PndTrkCTGeometryCalculations.h"
#include "PndTrkMergeSort.h"
#include "PndTrkPlotMacros2.h"
#include "PndTrkPrintouts.h"
#include "PndTrkSttAdjacencies.h"
#include "PndTrkSttClusterFinder.h"

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
#include "TStopwatch.h"

#include <iostream>
#include <cmath>




// definition of constants used in this class

// integer constants
#define MAXHITSINCELL		20
// the following MAXHITSINFIT cannot be too large because the fit
// crashes 'silently' for too much memory consumption in the character arrays
// or takes too long time;


// MAXHITSINFIT has to be 12 when using the GLPK fitting procedure; there is no limitation
// with the other type of fits;
//#define MAXHITSINFIT		12
#define MAXFOUNDCLUSTERS	100  // maximum allowed clusters in the event;
#define MAXHITSINCLUSTER	200  // maximum allowed hits in a cluster;
#define MAXHITSINFIT		50
#define MAXMVDMCPOINTS		2000
#define MAXSKEWHITSINFIT	8
#define MINIMUMHITSPERTRACK	2
#define MINOUTERHITSPERTRACK	5
#define PI			3.141592654
#define TIMEOUT			60



// floating point constants

// APOTEMA means the radius of a circle INSCRIBED in a Hexagon (this is the same as the
// mathematical definition); otherwise the name Radius is used instead;
#define APOTEMAMAXINNERPARSTRAW	23.246827
#define APOTEMAMAXSKEWSTRAW	31.517569 // delimitation of the skew area
#define APOTEMAMINOUTERPARSTRAW	31.863369
#define APOTEMAMINSKEWSTRAW	23.246827 // delimitation of the skew area
#define APOTEMASTRAWDETECTORMIN	16.119  // minimum APOTEMA of the Stt detector in  cm
					// this is the Radius of the circle INSCRIBED in the
					// smaller Hexagon delimiting the STT detector;
#define BFIELD			2.  // in Tesla
#define CVEL			2.99792  //  velocity of light
#define DELTAnR			2.   //range of nR in TrkAssociatedParallelHitsToHelixquater
#define DIAMETERSTRAWTUBE	1.
#define DIMENSIONSCITIL		2.85 // cm
#define ERRORPIXEL		0.02611
#define ERRORSTRIP		0.02611
#define ERRORSQPIXEL		0.00068175
#define ERRORSQSTRIP		0.00068175
#define PMAX			100.
#define RSTRAWDETECTORMAX	40.73 // maximum radius of the Stt detector in  cm; this is
					// the radius CIRCUMSCRIBED to the outer Hexagon
					// delimiting the STT detector;
#define STRAWRADIUS		0.5
#define STRAWRESOLUTION		0.015
#define STRAW_SKEW_INCLINATION	3.
#define STTDRIFTVEL		0.0025	//   in cm/nsec
#define VERTICALGAP		4. // (cm) gap between Left and Right sections of detector.



using namespace std;

// -----   Default constructor   -------------------------------------------
PndTrkTracking2::PndTrkTracking2() : FairTask("Tracking") { 
 fPersistence = kTRUE;
 istampa = 0;
 iplotta = false;
 doMcComparison = false;
 fYesClean = false;
 fYesCleanMvd = true;
 fYesSciTil = false ;
 fMvdAloneTracking = false;
 fNevents_to_plot = 10;


 Initialization_ClassVariables();
 sprintf(fSttBranch,"STTHit");
 sprintf(fMvdPixelBranch,"MVDHitsPixel");
 sprintf(fMvdStripBranch,"MVDHitsStrip");
}
// -------------------------------------------------------------------------

PndTrkTracking2::PndTrkTracking2(Int_t verbose) : FairTask("Tracking") { 
 fPersistence = kTRUE;
 istampa = verbose;
 iplotta = false;
 doMcComparison = false;
 fYesClean = false;
 fYesCleanMvd = true;
 fYesSciTil = false ;
 fMvdAloneTracking = false;

 Initialization_ClassVariables();
 sprintf(fSttBranch,"STTHit");
 sprintf(fMvdPixelBranch,"MVDHitsPixel");
 sprintf(fMvdStripBranch,"MVDHitsStrip");
}
// -------------------------------------------------------------------------

PndTrkTracking2::PndTrkTracking2(int istamp, bool  iplot, bool imc)
		: FairTask("Tracking") { 
 fPersistence = kTRUE;
 istampa = istamp;
 iplotta = iplot;
 doMcComparison = imc;
 fYesClean = false;
 fYesCleanMvd = true;
 fYesSciTil = false ;
 fMvdAloneTracking = false;
 Initialization_ClassVariables();
 sprintf(fSttBranch,"STTHit");
 sprintf(fMvdPixelBranch,"MVDHitsPixel");

 sprintf(fMvdStripBranch,"MVDHitsStrip");
}


// -------------------------------------------------------------------------

PndTrkTracking2::PndTrkTracking2(int istamp, bool  iplot, bool imc, bool doSciTil)
		: FairTask("Tracking") { 
 fPersistence = kTRUE;
 istampa = istamp;
 iplotta = iplot;
 doMcComparison = imc;
 fYesClean = false;
 fYesCleanMvd = true;
 fYesSciTil = doSciTil ;
 fMvdAloneTracking = false;
 Initialization_ClassVariables();
 sprintf(fSttBranch,"STTHit");
 sprintf(fMvdPixelBranch,"MVDHitsPixel");

  sprintf(fMvdStripBranch,"MVDHitsStrip");
}
// -----   Destructor   ----------------------------------------------------
PndTrkTracking2::~PndTrkTracking2() {}
// -----------------------


//--------------- begin  PndTrkTracking2::Initialization_ClassVariables

void PndTrkTracking2::Initialization_ClassVariables()
{
// this is only for initializing the Class Variables.

	size_t len;


// booleans :
	len = sizeof(fSingleHitListStt);
	memset (fSingleHitListStt,true,len);

	len = sizeof(fInclusionListSciTil);
	memset (fInclusionListSciTil,true,len);

	len = sizeof(fInclusionListStt);
	memset (fInclusionListStt,true,len);

	len = sizeof(finMvdTrackCandPixel);
	memset (finMvdTrackCandPixel,false,len);

	len = sizeof(finMvdTrackCandStrip);
	memset (finMvdTrackCandStrip,false,len);

	len = sizeof(fTypeConf);
	memset (fTypeConf,false,len);


// char :


	len = sizeof(fSttBranch);
	memset (fSttBranch,0,len);

	len = sizeof(fMvdPixelBranch);
	memset (fMvdPixelBranch,0,len);

	len = sizeof(fMvdStripBranch);
	memset (fMvdStripBranch,0,len);

//  Short_t :

	fnMCTracks=0;
	fnSciTilHits=0;

	len = sizeof(fListMvdPixelHitsinTrack);
	memset (fListMvdPixelHitsinTrack,0,len);

	len = sizeof(fListMvdStripHitsinTrack);
	memset (fListMvdStripHitsinTrack,0,len);

	len = sizeof(fListSciTilHitsinTrack);
	memset (fListSciTilHitsinTrack,0,len);

	len = sizeof(fListSttParHits);
	memset (fListSttParHits,0,len);

	len = sizeof(fListSttParHitsinTrack);
	memset (fListSttParHitsinTrack,0,len);

	len = sizeof(fListSttSkewHitsinTrack);
	memset (fListSttSkewHitsinTrack,0,len);

	len = sizeof(fListSttSkewHits);
	memset (fListSttSkewHits,0,len);

	len = sizeof(fListSttSkewHitsinTrackSolution);
	memset (fListSttSkewHitsinTrackSolution,0,len);

	len = sizeof(fListTrackCandHit);
	memset (fListTrackCandHit,0,len);

	len = sizeof(fnMvdPixelHitsinTrack);
	memset (fnMvdPixelHitsinTrack,0,len);

	len = sizeof(fnMvdStripHitsinTrack);
	memset (fnMvdStripHitsinTrack,0,len);

	len = sizeof(fnTrackCandHit);
	memset (fnTrackCandHit,0,len);

	len = sizeof(fnSciTilHitsinTrack);
	memset (fnSciTilHitsinTrack,0,len);

	len = sizeof(fnSttParHitsinTrack);
	memset (fnSttParHitsinTrack,0,len);

	len = sizeof(fnSttSkewHitsinTrack);
	memset (fnSttSkewHitsinTrack,0,len);

	fnMvdDSPixelHitNotTrackCand=0;
	fnMvdDSStripHitNotTrackCand=0;
	fnMvdPixelHit=0;
	fnMvdStripHit=0;
	fnMvdTrackCand=0;
	fnMvdUSPixelHitNotTrackCand=0;
	fnMvdUSStripHitNotTrackCand=0;

	len = sizeof(fnHitMvdTrackCand);
	memset (fnHitMvdTrackCand,0,len);

	len = sizeof(fListHitMvdTrackCand);
	memset (fListHitMvdTrackCand,0,len);

	len = sizeof(fListHitTypeMvdTrackCand);
	memset (fListHitTypeMvdTrackCand,0,len);

	len = sizeof(fListMvdDSPixelHitNotTrackCand);
	memset (fListMvdDSPixelHitNotTrackCand,0,len);

	len = sizeof(fListMvdUSPixelHitNotTrackCand);
	memset (fListMvdUSPixelHitNotTrackCand,0,len);

	len = sizeof(fListMvdDSStripHitNotTrackCand);
	memset (fListMvdDSStripHitNotTrackCand,0,len);

	len = sizeof(fListMvdUSStripHitNotTrackCand);
	memset (fListMvdUSStripHitNotTrackCand,0,len);

	len = sizeof(fListTrackCandHitType);
	memset (fListTrackCandHitType,0,len);

	len = sizeof(fnParContiguous);
	memset (fnParContiguous,0,len);

	len = sizeof(fListParContiguous);
	memset (fListParContiguous,0,len);

	memset (fStrawCode,0,sizeof(fStrawCode));

	memset (fStrawCode,0,sizeof(fStrawCode2));
//  int :
	fNevents_to_plot = 10;


//  Double_t :

	fFimin=0.;
	SEMILENGTH_STRAIGHT=75.;
	ZCENTER_STRAIGHT=35.;

	len = sizeof(fALFA);
	memset (fALFA,0,len);

	len = sizeof(fBETA);
	memset (fBETA,0,len);

	len = sizeof(fGAMMA);
	memset (fGAMMA,0,len);

	len = sizeof(fCxMC);
	memset (fCxMC,0,len);

	len = sizeof(fCyMC);
	memset (fCyMC,0,len);

	len = sizeof(fR_MC);
	memset (fR_MC,0,len);

	len = sizeof(fMCtruthTrkInfo);
	memset (fMCtruthTrkInfo,0,len);

	len = sizeof(fMCSkewAloneX);
	memset (fMCSkewAloneX,0,len);

	len = sizeof(fMCSkewAloneY);
	memset (fMCSkewAloneY,0,len);

	len = sizeof(fradiaConf);
	memset (fradiaConf,0,len);

	len = sizeof(fOx);
	memset (fOx,0,len);

	len = sizeof(fOy);
	memset (fOy,0,len);

	len = sizeof(fR);
	memset (fR,0,len);

	len = sizeof(frefindexMvdPixel);
	memset (frefindexMvdPixel,0,len);

	len = sizeof(fsigmaXMvdPixel);
	memset (fsigmaXMvdPixel,0,len);

	len = sizeof(fsigmaYMvdPixel);
	memset (fsigmaYMvdPixel,0,len);

	len = sizeof(fsigmaZMvdPixel);
	memset (fsigmaZMvdPixel,0,len);

	len = sizeof(fXMvdPixel);
	memset (fXMvdPixel,0,len);

	len = sizeof(fYMvdPixel);
	memset (fYMvdPixel,0,len);

	len = sizeof(fZMvdPixel);
	memset (fZMvdPixel,0,len);

	len = sizeof(fXMvdStrip);
	memset (fXMvdStrip,0,len);

	len = sizeof(fYMvdStrip);
	memset (fYMvdStrip,0,len);

	len = sizeof(fZMvdStrip);
	memset (fZMvdStrip,0,len);

	len = sizeof(fsigmaXMvdStrip);
	memset (fsigmaXMvdStrip,0,len);

	len = sizeof(fsigmaYMvdStrip);
	memset (fsigmaYMvdStrip,0,len);

	len = sizeof(fsigmaZMvdStrip);
	memset (fsigmaZMvdStrip,0,len);

	len = sizeof(frefindexMvdStrip);
	memset (frefindexMvdStrip,0,len);

	len = sizeof(fposizSciTil);
	memset (fposizSciTil,0,len);



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

//--------------- end of  PndTrkTracking2::Initialization_ClassVariables


//----------------------------------------- begin PndTrkTracking2::Init

InitStatus PndTrkTracking2::Init() {


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
    cout << "-E- PndTrkTracking2::Init: "
	 << "RootManager not instantiated, return!" << endl;
    return kFATAL;
 }
//  -----   maps of STT tubes
 // CHECK added 
 PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
 fSttTubeArray = mapper->FillTubeArray();
 //----------------------------------------------------  end map


// load the array indicating if a straw is external of not;
// remember that the numbering of the STT Straws starts at 1 and goes up to 4542 included;

// StrawCode convention (in the following left or right is looking to the beam from downstream) :
//   -1 = not a boundary straw;
//   10= inner axial boundary left;
//   20= inner axial boundary right;
//   12= outer left axial Stt : Vertical (BUT NOT OUTERMOST) + inside boundary ;
//   22= outer right axial Stt : Vertical (BUT NOT OUTERMOST) + inside boundary ;
//   13= outermost axial boundary left;
//   23= outermost axial boundary right;

	PndTrkBoundaryParStraws2 BoundaryParStraws ;
	BoundaryParStraws.Set(
		// inputs :
				APOTEMAMAXINNERPARSTRAW,
				APOTEMAMINOUTERPARSTRAW,
				NUMBER_STRAWS,
				APOTEMASTRAWDETECTORMIN,
				RSTRAWDETECTORMAX,
				false,	// printout flag;
				fSttTubeArray,
				STRAWRADIUS,
				VERTICALGAP,
		// outputs :
				fStrawCode,
				fStrawCode2
				);



 //----------------------------------------------------

 // load the adjacencies table for the Stt tubes;
 PndTrkSttAdjacencies  Adjacent;
 Adjacent.CalculateAdjacentStt(
	NUMBER_STRAWS,
	fSttTubeArray,
	fnParContiguous,  // output; number of contiguous straws (axial Stt);
	fListParContiguous  // output list (axial Stt);
	);

//-----------------------------------------

//  calculate the sines and cosines for the Legiandre fit;

//	LEGIANDRE_NTHETADIV,	input;
//	fSinus,			output;
//	fCosine		output;

 CalculateSinandCosin(
		);


//    get   the MCTrack  array

 fMCTrackArray = (TClonesArray*) ioman->GetObject("MCTrack");
 if ( ! fMCTrackArray)
 {
	cout << "-E- PndTrkTracking2::Init: No MCTrack array!"
	   << endl;
//	return kERROR;
 }

//  -------------------------   get the SciTil hits
 if(fYesSciTil) {
	fSciTHitArray = (TClonesArray*) ioman->GetObject("SciTHit");
 } else {
	fSciTHitArray = NULL;
 }
//---------------------------

//  -------------------------   get the SciTil MC Points
 if(fYesSciTil && doMcComparison) {
	fSciTPointArray = (TClonesArray*) ioman->GetObject("SciTPoint");
 } else {
	fSciTPointArray = NULL;
 }
//---------------------------
 // Get input array   these are the MC point of STT
 fSttPointArray = (TClonesArray*) ioman->GetObject("STTPoint");
 if ( ! fSttPointArray ) {
	cout << "-W- PndSttHelixHitProducer::Init: "
	 << "No STTPoint array, return!" << endl;
	return kERROR;
 }

 // Get input array   hit of STT after digi
 fSttHitArray = (TClonesArray*) ioman->GetObject(fSttBranch);
//  fSttHitArray = (TClonesArray*) ioman->GetObject("STTHit");
 if ( ! fSttHitArray ) {
	cout << "-W- PndTrkTracking2::Init: "
	 << "No STTHit array, return!" << endl;
	return kERROR;
 }

//  -------------------------   get the Mvd hits
 fMvdPixelHitArray = (TClonesArray*) ioman->GetObject(fMvdPixelBranch);
//  fMvdPixelHitArray = (TClonesArray*) ioman->GetObject("MVDHitsPixel");
 if ( !fMvdPixelHitArray){
	cout << "-W- PndTrkTracking2::Init: " << "No MVD Pixel hitArray, return!" <<endl;
//	return kERROR;
 }
 fMvdStripHitArray = (TClonesArray*) ioman->GetObject(fMvdStripBranch);
//  fMvdStripHitArray = (TClonesArray*) ioman->GetObject("MVDHitsStrip");

 if ( !fMvdStripHitArray){
	cout << "-W- PndTrkTracking2::Init: " << "No MVD Strip hitArray, return!" <<endl;
//	return kERROR;
 }

//  -------------------------   get the Mvd track candidates

 if ( fMvdAloneTracking ){
	fMvdTrackCandArray = (TClonesArray*) ioman->GetObject("MVDRiemannTrackCand");
	if ( !fMvdTrackCandArray){
		cout << "-W- PndTrkTracking2::Init: " << "No MVD TrackCand Array, return!" <<endl;
//		return kERROR;
	}
 }
 cout << "-I- PndTrkTracking2: Initialization successfull" << endl;

//  -------------------------   get the Mvd MC points

 fMvdMCPointArray = (TClonesArray*) ioman->GetObject("MVDPoint");
 if ( !fMvdMCPointArray){
	cout << "-W- PndTrkTracking2::Init: " << "No MVD MC Point Array, return!" <<endl;
	return kERROR;
 }
 cout << "-I- PndTrkTracking2: Initialization successfull" << endl;


//--------------------------------  output TClonesArrays ------------------------

 // Create and register output array for PndTrackCand of Stt+Mvd combined

 fSttMvdPndTrackCandArray = new TClonesArray("PndTrackCand");
// ioman->Register("SttMvdTrackCand","SttMvd",fSttMvdPndTrackCandArray, kTRUE);
 ioman->Register("SttMvdTrackCand","SttMvd",fSttMvdPndTrackCandArray, fPersistence);


 // Create and register output array for PndTrack of Stt+Mvd combined

 fSttMvdPndTrackArray = new TClonesArray("PndTrack");
 ioman->Register("SttMvdTrack","SttMvd",fSttMvdPndTrackArray, fPersistence);

//-----------------------


//   calculate the boundaries of the Box in Conformal Space, see Gianluigi logbook on pag. 210-211
 Short_t i;
 Double_t
	A,
	r1,
	r2;

 fradiaConf[0] = 1./RSTRAWDETECTORMAX;
 r1 = APOTEMASTRAWDETECTORMIN;
 A = (RSTRAWDETECTORMAX - r1)/NRDIVCONFORMAL;
 if ( NRDIVCONFORMAL > 1 ) {
	for(i = 1; i< NRDIVCONFORMAL ; i++){
		r2 = r1 + A;
		fradiaConf[NRDIVCONFORMAL-i] = 1./r2;
		r1=r2;
	}
 }


 return kSUCCESS;

}

//----------------------------------------- end of PndTrkTracking2::Init




void PndTrkTracking2::SetParContainers() {
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}




void PndTrkTracking2::WriteHistograms(){

  TFile* file = FairRootManager::Instance()->GetOutFile();
  file->cd();
  file->mkdir("PndTrkTracking2");
  file->cd("PndTrkTracking2");
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

void PndTrkTracking2::Exec(Option_t* opt) {


 bool
	flag,
	intersect,
	outcome;

//----------------------------------------------
/*
 bool
	tGoodSkewFit[MAXTRACKSPEREVENT],
	tkeepit[MAXTRACKSPEREVENT],
	tMvdhits[MAXTRACKSPEREVENT],
	tstatus[MAXTRACKSPEREVENT],
	tSttSZfit[MAXTRACKSPEREVENT];


	Vec <bool>
	GoodSkewFit(tGoodSkewFit,MAXTRACKSPEREVENT,"GoodSkewFit"),
	keepit(tkeepit ,MAXTRACKSPEREVENT,"keepit"),
	Mvdhits(tMvdhits, MAXTRACKSPEREVENT,"Mvdhits"),
	status(tstatus, MAXTRACKSPEREVENT,"status"),
	SttSZfit(tSttSZfit, MAXTRACKSPEREVENT,"SttSZfit");
*/
//----------------------------------------------
 bool
	GoodSkewFit[MAXTRACKSPEREVENT],
	keepit[MAXTRACKSPEREVENT],
	Mvdhits[MAXTRACKSPEREVENT],
	status[MAXTRACKSPEREVENT],
	SttSZfit[MAXTRACKSPEREVENT];




 Short_t
	tnHitsInMCTrack[MAXTRACKSPEREVENT],
	tnMCParalAlone[MAXTRACKSPEREVENT],
	tnMCSkewAlone[MAXTRACKSPEREVENT],
	tnParalCommon[MAXTRACKSPEREVENT],
	tnSkewCommon[MAXTRACKSPEREVENT],
	tnSkewHitsInMCTrack[MAXTRACKSPEREVENT],
	tnSpuriParinTrack[MAXTRACKSPEREVENT],
	tnSpuriSkewinTrack[MAXTRACKSPEREVENT],
	//  given a Hit number it gives its radial box number
	RConformalIndex[MAXSTTHITS],
	//  given a Hit number it gives its azimuthal box number
	FiConformalIndex[MAXSTTHITS],
	tempore[MAXSTTHITS],
	TemporarySkewList[2*MAXSTTHITS][2],
	BigList[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	// nBoxConformal,  first index -> radial divisions,
	// 2nd index -> azimuthal divisions; n. of hits falling in this cell.
	nBoxConformal[NRDIVCONFORMAL*NFIDIVCONFORMAL],
	tParalCommonList[MAXTRACKSPEREVENT*MAXSTTHITSINTRACK],
	tParSpuriList[MAXTRACKSPEREVENT*MAXSTTHITSINTRACK],
	tSkewCommonList[MAXTRACKSPEREVENT*MAXSTTHITSINTRACK],
	tSkewSpuriList[MAXTRACKSPEREVENT*MAXSTTHITSINTRACK],
	HitsinBoxConformal[MAXHITSINCELL*NRDIVCONFORMAL*NFIDIVCONFORMAL],
	Charge[MAXTRACKSPEREVENT],
	tdaTrackFoundaTrackMC[MAXTRACKSPEREVENT],
	resultFitSZagain[MAXTRACKSPEREVENT],
	statusflag[MAXTRACKSPEREVENT],
	SttStrawOn[NUMBER_STRAWS];



 Vec <Short_t>
	nHitsInMCTrack(tnHitsInMCTrack, MAXTRACKSPEREVENT,"nHitsInMCTrack"),
	nMCParalAlone(tnMCParalAlone, MAXTRACKSPEREVENT,"nMCParalAlone"),
	nMCSkewAlone(tnMCSkewAlone, MAXTRACKSPEREVENT,"nMCSkewAlone"),
	nParalCommon(tnParalCommon,MAXTRACKSPEREVENT,"nParalCommon"),
	nSkewCommon(tnSkewCommon,MAXTRACKSPEREVENT,"nSkewCommon"),
	nSkewHitsInMCTrack(tnSkewHitsInMCTrack, MAXTRACKSPEREVENT,"nSkewHitsInMCTrack"),
	nSpuriParinTrack(tnSpuriParinTrack, MAXTRACKSPEREVENT,"nSpuriParinTrack"),
	nSpuriSkewinTrack(tnSpuriSkewinTrack, MAXTRACKSPEREVENT,"nSpuriSkewinTrack"),
	//  given a Hit number it gives its radial box number

//	RConformalIndex(tRConformalIndex, MAXSTTHITS,"RConformalIndex"),

	//  given a Hit number it gives its azimuthal box number

//	FiConformalIndex(tFiConformalIndex, MAXSTTHITS,"FiConformalIndex"),
//	tempore(ttempore, MAXSTTHITS,"tempore"),
	// nBoxConformal,  first index -> radial divisions,
	// 2nd index -> azimuthal divisions; n. of hits falling in this cell.
//	nBoxConformal(tnBoxConformal, NRDIVCONFORMAL*NFIDIVCONFORMAL,"nBoxConformal"),
	ParalCommonList(tParalCommonList, MAXTRACKSPEREVENT*MAXSTTHITSINTRACK,"ParalCommonList"),
	ParSpuriList(tParSpuriList, MAXTRACKSPEREVENT*MAXSTTHITSINTRACK,"ParSpuriList"),
	SkewCommonList(tSkewCommonList, MAXTRACKSPEREVENT*MAXSTTHITSINTRACK,"SkewCommonList"),
	SkewSpuriList(tSkewSpuriList, MAXTRACKSPEREVENT*MAXSTTHITSINTRACK,"SkewSpuriList"),
//	HitsinBoxConformal(tHitsinBoxConformal, MAXHITSINCELL*NRDIVCONFORMAL*NFIDIVCONFORMAL,"HitsinBoxConformal"),

//	Charge(tCharge,MAXTRACKSPEREVENT,"Charge"),

	daTrackFoundaTrackMC(tdaTrackFoundaTrackMC,MAXTRACKSPEREVENT,"daTrackFoundaTrackMC");
//	resultFitSZagain(tresultFitSZagain,MAXTRACKSPEREVENT,"resultFitSZagain"),
//	statusflag(tstatusflag,MAXTRACKSPEREVENT,"statusflag"),
//	SttStrawOn(tSttStrawOn,NUMBER_STRAWS,"SttStrawOn");  //  SttStrawOn[i] >= 0 --> it is the Stt hit number corresponding to Stt
							// i-th Tube ID; SttStrawOn[i] == -1 --> i-th Stt straw NOT hit;

 memset(SttStrawOn,-1,sizeof(SttStrawOn));

//-----------------------------------




 Short_t
	nalone,
	ncand,
	nhitsinfit,
	Nint,
	NNN,
	nRemainingCandidates,
	npixelhitsintrack,
	nstriphitsintrack,
	nTotalCandidates,
	nXYZhits,
	i,
	iCluster,
	iexcl,
	iParHit,
	ipunto,
	j,
	k,
	kall,
	l
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
	tListHits[MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK];

 Vec <Int_t> ListHits(tListHits,MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK,"ListHits");

 Double_t
	tS[2*MAXSTTHITS],
	tZ[2*MAXSTTHITS];
 Vec <Double_t>
	S(tS,2*MAXSTTHITS,"S"),
	Z(tZ,2*MAXSTTHITS,"Z");

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
	rotationangle,
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


//--------------------
	Double_t
	tDriftRadius[MAXSTTHITSINTRACK+MAXMVDPIXELHITSINTRACK
		+MAXMVDSTRIPHITSINTRACK+MAXSCITILHITSINTRACK],
	tErrorDriftRadius[MAXSTTHITSINTRACK+MAXMVDPIXELHITSINTRACK
		+MAXMVDSTRIPHITSINTRACK+MAXSCITILHITSINTRACK],
	tZED[MAXSTTHITSINTRACK+MAXMVDPIXELHITSINTRACK
		+MAXMVDSTRIPHITSINTRACK+MAXSCITILHITSINTRACK];

	Vec <Double_t>
	DriftRadius(tDriftRadius,MAXSTTHITSINTRACK+MAXMVDPIXELHITSINTRACK
		+MAXMVDSTRIPHITSINTRACK+MAXSCITILHITSINTRACK,"DriftRadius"),
	ErrorDriftRadius(tErrorDriftRadius,MAXSTTHITSINTRACK+MAXMVDPIXELHITSINTRACK
		+MAXMVDSTRIPHITSINTRACK+MAXSCITILHITSINTRACK,"ErrorDriftRadius"),
	ZED(tZED,MAXSTTHITSINTRACK+MAXMVDPIXELHITSINTRACK
		+MAXMVDSTRIPHITSINTRACK+MAXSCITILHITSINTRACK,"ZED");
//--------------------------------------





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

 PndTrkCleanup Cleaner;

 PndTrkCTGeometryCalculations GeomCalculator;

 PndTrkPrintouts fPrint;


 // the class with all the fits. This is used for the SZ fit.
 bool YesGLPKfitSZ = false;
// PndTrkGlpkFits fit;  YesGLPKfitSZ=true;
// PndTrkLegendreFits fit;
 PndTrkChi2Fits fit;

//  reset the TClones Arrays of the PndTrackCand and PndTrack; it is necessary
//  to do this for every event at the very beginning of the Exec (those TClones Arrays
//  have been registered  in the Init and they are automatically writted each event).

 fSttMvdPndTrackCandArray->Delete();
 fSttMvdPndTrackArray->Delete();

//------------------------------------

 IVOLTE++;

 if(istampa>0)
	cout<<endl<<"Entering in PndTrkTrack : evt (starting from 0)  n. "<<IVOLTE<<endl;


// -------------------------------------  fetch info from MVD

 if( fMvdPixelHitArray ){
	fnMvdPixelHit = fMvdPixelHitArray->GetEntriesFast();
 } else {
	fnMvdPixelHit = 0;
 }

 if( fMvdStripHitArray ){
	fnMvdStripHit = fMvdStripHitArray->GetEntriesFast();
 } else {
	fnMvdStripHit = 0;
 }

 if(fnMvdPixelHit>MAXMVDPIXELHITS){
	cout<<"from PndTrkTracking2, fnMvdPixelHit is > maximum allowed ("
	<<MAXMVDPIXELHITS<<") and therefore is set to "
	<<MAXMVDPIXELHITS<<endl;
	fnMvdPixelHit=MAXMVDPIXELHITS;
 }
 if(fnMvdStripHit>MAXMVDSTRIPHITS){
	cout<<"from PndTrkTracking2, fnMvdStripHit is > maximum allowed ("
	<<MAXMVDSTRIPHITS<<") and therefore is set to "
	<<MAXMVDSTRIPHITS<<endl;
	fnMvdStripHit=MAXMVDSTRIPHITS;
 }

// ---------------------------------------------  extract info from HITS Pixel MVD

 for( i= 0; i< fnMvdPixelHit; i++){
	pMvdPixelHit = (PndSdsHit *) fMvdPixelHitArray->At(i);
	TVector3 temp = pMvdPixelHit->GetPosition();
	fXMvdPixel[i] = temp.X();
	fYMvdPixel[i] = temp.Y();
	fZMvdPixel[i] = temp.Z();
	fsigmaXMvdPixel[i] = pMvdPixelHit->GetDx();
	fsigmaYMvdPixel[i] = pMvdPixelHit->GetDy();
	fsigmaZMvdPixel[i] = pMvdPixelHit->GetDz();
	frefindexMvdPixel[i] = pMvdPixelHit->GetRefIndex();
 }

// -------------------------------------------  extract info from HITS Strip MVD

 for( i= 0; i< fnMvdStripHit; i++){
	pMvdStripHit = (PndSdsHit *) fMvdStripHitArray->At(i);
	TVector3 temp = pMvdStripHit->GetPosition();
	fXMvdStrip[i] = temp.X();
	fYMvdStrip[i] = temp.Y();
	fZMvdStrip[i] = temp.Z();
	fsigmaXMvdStrip[i] = pMvdStripHit->GetDx();
	fsigmaYMvdStrip[i] = pMvdStripHit->GetDy();
	fsigmaZMvdStrip[i] = pMvdStripHit->GetDz();
	frefindexMvdStrip[i] = pMvdStripHit->GetRefIndex();
 }


// ---------------   printout of Mvd Hits;
 if(istampa>=1)
	fPrint.stampaMvdHits(
		fMvdPixelBranch,
		fMvdStripBranch,
		fnMvdPixelHit,
		fnMvdStripHit,
		frefindexMvdPixel,
		frefindexMvdStrip,
		fsigmaXMvdPixel,
		fsigmaXMvdStrip,
		fsigmaYMvdPixel,
		fsigmaYMvdStrip,
		fsigmaZMvdPixel,
		fsigmaZMvdStrip,
		fXMvdPixel,
		fXMvdStrip,
		fYMvdPixel,
		fYMvdStrip,
		fZMvdPixel,
		fZMvdStrip
			);


// ------------------------------------------ get info from trackcand  of MVD

 if( fMvdAloneTracking ){
	fnMvdTrackCand = fMvdTrackCandArray->GetEntriesFast();
	if (fnMvdTrackCand> MAXMVDTRACKSPEREVENT) {
		cout<<"from PndTrkTracking2  :  N. of MvdTrackCand = "<<
		fnMvdTrackCand<<" and it is > MAXMVDTRACKSPEREVENT (="<<MAXMVDTRACKSPEREVENT
		<<"),  therefore it is set to "<<MAXMVDTRACKSPEREVENT<<endl;
		fnMvdTrackCand= MAXMVDTRACKSPEREVENT;
		ExtractInfoFromMvdTrackCand();
	}

 } else {
	fnMvdTrackCand=0;
 }





//---------------------------------------------   fetching the STT  MC points
 nSttMCPoint = fSttPointArray->GetEntriesFast();
 if (nSttMCPoint ==0){
	if(istampa>0) cout<<"from PndTrkTracking2  :  N. di Stt MC points = 0"<<endl<<endl;
 } else  if( nSttMCPoint>MAXSTTHITS){
	cout<<"from PndTrkTracking2  :  N. di Stt MC points = "<<nSttMCPoint
	<<" and it is > MAXSTTHITS ("<<MAXSTTHITS
	     <<"), therefore consider only the first "<<MAXSTTHITS<<" hits"<<endl<<endl;
	nSttMCPoint= MAXSTTHITS;
 }


//--------------------------------------------------------   fetching the STT  hits

 nSttHit = fSttHitArray->GetEntriesFast();
 if (nSttHit ==0){
	if (istampa >= 1) cout<<"from PndTrkTracking2  :  N. di Stt Hits = 0, return!"<<endl<<endl;
	return;
 } else if (nSttHit> MAXSTTHITS) {
	cout<<"from PndTrkTracking2  :  N. di Stt Hits = "<<nSttHit
	     <<" and it is > MAXSTTHITS (="<<MAXSTTHITS
	     <<"), therefore consider only the first "<<MAXSTTHITS<<" hits"<<endl<<endl;
	nSttHit= MAXSTTHITS;
 }

 if (istampa >= 1) {
	cout<<"from PndTrkTracking2  :  total # Hits in STT  : "<<nSttHit<<endl;
 }

 nSttParHit=0; 
 nSttSkewHit=0;

 for( i= 0; i< nSttHit; i++){
	pSttHit = (PndSttHit *) fSttHitArray->At(i);
	// right way to extract the corrisponding MC point.
	ipunto= pSttHit->GetRefIndex();
	// fTubeID[i] = STT tubeID correspondint to the hit number i ;
	//	the  STT tubeID  goes from 1 to 4542 inclusive;
	fTubeID[i] = pSttHit->GetTubeID();
	pSttTube = (PndSttTube *) fSttTubeArray->At(fTubeID[i]);
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
		fListSttParHits[nSttParHit]=i;
		nSttParHit++;
	} else {
		info[i][5]= 99.;// to signal that it is a skew straw.
		fListSttSkewHits[nSttSkewHit]=i;
		nSttSkewHit++;
	}

 //  printout of the Stt hits;
 if (istampa >= 1 ) fPrint.stampaSttHits2(i,ipunto,dradius,WDX,WDY,WDZ,puntator,pSttTube,fTubeID[i]);

  }  //   end of for( i= 0; i< nSttHit; i++)

//   reordering the list of parallel hits ( fListSttParHits) by decreasing spatial radius;
//   first the outermost then the innermost. This is necessary because later the search
//   must starts from the outer hits. 

 Initial_SttParHits_DecreasingR_Ordering( info, fListSttParHits,nSttParHit );


//	fill the inclusion list for Stt, include only first hit for those straws with
//	multiple hits; fInclusionListStt  is BY STT HIT NUMBER;

 MakeInclusionListStt(nSttHit, fTubeID, info);


//-----------------------------------  end of exclusion of straws with multiple hits

//  this MUST go after the MakeInclusionListStt


 for( i= 0; i< nSttHit; i++){

	if(!fInclusionListStt[i]) continue;
//  SttStrawOn is a Short_t used in the XY track pattern finding later; it was initialized at -1;
	SttStrawOn[ fTubeID[i] - 1 ] = i;
 }  // end of for( i= 0; i< nSttHit; i++)




//-------------------------------------------- fetch the SciTil hits
 fnSciTilHits = 0;
 if( fSciTHitArray != NULL){
	// number SciTil hits/event
	fnSciTilHits = fSciTHitArray->GetEntriesFast();
	if(fnSciTilHits>MAXSCITILHITS){
		cout<<"from PndTrkTracking2  :  N. of SciTil Hits = "<<fnSciTilHits
	     <<" and it is > MAXSCITILHITS (="<<MAXSCITILHITS
	     <<"), therefore consider only the first "<<MAXSCITILHITS <<" hits"<<endl<<endl;
		fnSciTilHits= MAXSCITILHITS;
	}
 }  // end of if( fSciTHitArray != NULL)

 // it is important that fSciTilMaxNumber and OriginalSciTilList have a scope
 // extending in all   exec  method; that's why they are stated here, out
 // of the if( fSciTHitArray != NULL).
 Short_t fSciTilMaxNumber;
 if( fnSciTilHits > 0) fSciTilMaxNumber = fnSciTilHits ; else fSciTilMaxNumber = 1;

 Short_t
	nHitsInSciTile[fSciTilMaxNumber],
	OriginalSciTilList[fSciTilMaxNumber][fSciTilMaxNumber];

//--------   initialization (to 0) of nHitsInSciTile array;
 memset (nHitsInSciTile,0,sizeof(nHitsInSciTile));
//---

 if(istampa>0) cout<<"from PndTrkTracking2, event "<<IVOLTE<<", "<<fnSciTilHits
		<<" SciTil hits present initially."<<endl;
	for(j=0; j<fnSciTilHits; j++){
	}

   if( fnSciTilHits >0 ){
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
	if(istampa>0)cout<<"from PndTrkTracking2 SciTil n. "<<0<<
	" not purged, Xpos "<<
			posiz.X()<<", Ypos "<<
			posiz.Y()<<", Zpos "<<posiz.Z()<<endl;

	fposizSciTil[0][0]=posiz.X();
	fposizSciTil[0][1]=posiz.Y();
	fposizSciTil[0][2]=posiz.Z();
	fpSciTilx[0]=posiz.X();
	fpSciTily[0]=posiz.Y();
	fpSciTilz[0]=posiz.Z();
	OriginalSciTilList[0][0]=0;
	nHitsInSciTile[0]=1;
	iaccept=1;
	// the other SciTil hits; purge them if they are duplicate.
	for(j=1; j<fnSciTilHits; j++){
		pPndSciTHit = (PndSciTHit*) fSciTHitArray->At(j);
		posiz = pPndSciTHit->GetPosition();
		if(istampa>0)cout<<"from PndTrkTracking2 SciTil n. "<<j<<" not purged, Xpos "
			<<posiz.X()<<", Ypos "<<posiz.Y()<<", Zpos "<<posiz.Z()<<endl;

	// purging the duplicate SciTil hits.
	    flag = true;
	    for(k=0; k<iaccept; k++){
		if(
			(fabs(posiz.X() - fposizSciTil[k][0])< 1.e-20) &&
			(fabs(posiz.Y() - fposizSciTil[k][1])< 1.e-20) &&
			(fabs(posiz.Z() - fposizSciTil[k][2])< 1.e-20)
		  ){
			flag=false;
			OriginalSciTilList[k][nHitsInSciTile[k]]= j;
			nHitsInSciTile[k]++;
			break;
		}  // end of if((fabs(posiz.X() - old...
	    } // end of for(k=0; k<iaccept; k++)
	    if(flag){
		fposizSciTil[iaccept][0]=posiz.X();
		fposizSciTil[iaccept][1]=posiz.Y();
		fposizSciTil[iaccept][2]=posiz.Z();
		fpSciTilx[iaccept]=posiz.X();
		fpSciTily[iaccept]=posiz.Y();
		fpSciTilz[iaccept]=posiz.Z();
		OriginalSciTilList[iaccept][0]= j;
		nHitsInSciTile[iaccept]=1;
		iaccept++;
	    }
	 }  // end of for(j=0; j<fnSciTilHits; j++)
	 fnSciTilHits=iaccept;

	 //  set the inclusion list of the SciTils to true.

	for(i=0;i<fnSciTilHits;i++){
		fInclusionListSciTil[i]=true;
	}


//-----------stampe.
if(istampa>0){
  cout<<"from PndTrkTracking2, after purging  SciTil; # hits = "<<fnSciTilHits<<endl;
  for(j=0; j<fnSciTilHits; j++){
	cout<<"from PndTrkTracking2 SciTil Xpos "<<fposizSciTil[j][0]<<", Ypos "<<
	fposizSciTil[j][1]<<", Zpos "<<fposizSciTil[j][2]<<endl;
  }
}
//---------- fine stampe.



   }  // end of if( fnSciTilHits >0 )


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


//------------------------------------ start the timer;
 	ftimer.Start(kTRUE);
 if(IVOLTE==0){
 	ftimer2.Start(kTRUE);
 } else {
 	ftimer2.Start(kFALSE);
 }
//----------------------------------------------------


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
	fListSttParHits,
	nSttParHit,
	infoparalConformal,
	STRAWRADIUS
		);


// find the relevant Stt clusters in XY projection, necessary for the Pattern recognition

 PndTrkSttClusterFinder cluster;
 Short_t	ListHitsinCluster[MAXFOUNDCLUSTERS*MAXHITSINCLUSTER],
 		nFoundClusters,
 		nHitsinCluster[MAXFOUNDCLUSTERS];


 cluster.GetClusters(
 		fInclusionListStt,	// input; this is the exclusion of Stt hits for Stt multiple hits;
		fListParContiguous,	// input list (axial Stt); first dimension is NUMBER_STRAWS;
		fListSttParHits,	// input
		fnParContiguous,  	// input; number of contiguous straws (axial Stt);
					// NUMBER_STRAWS even if the numbering scheme for the Stt
					// straws goes  from 1 to NUMBER_STRAWS included;
		nSttParHit,		// input;
		MAXFOUNDCLUSTERS,	// input;
		MAXHITSINCLUSTER,	// input;
		MAXSTTHITS,		// input; number of maximux allowed total Stt hits;
		NUMBER_STRAWS,		// input; number of Stt Straws in total;
		fStrawCode,		// input;
		fStrawCode2,		// input;
		SttStrawOn,		// input;
		fSttTubeArray,		// input; array of the Stt tubes;
		fTubeID,			// input;

		ListHitsinCluster,	// output;
		nFoundClusters,	// output;
		nHitsinCluster	// output;

 		);

//------------------------------------- stampe;
if(istampa>=2){
	cout<<"from PndTrkTracking2, evt "<<IVOLTE<<";  number of clusers found : "<<nFoundClusters<<" and their list :\n";
	for(int ic=0;ic<nFoundClusters;ic++){
		cout<<"cluster n. "<<ic<<endl;
		for(int icz=0;icz<nHitsinCluster[ic];icz++){
			cout<<"\tStt || hit n. "<<ListHitsinCluster[ic*MAXHITSINCLUSTER+icz]<<endl;
		}
	}
}
//-------------------------fine stampe;

//-------------------------------------------------------------------
 // class that finds the track (Stt hits only) in XY projection
 PndTrkCTFindTrackInXY2 SttTrackXYFinder;

 //  struct  necessary to pass all the parametrs to the PndTrkCTFindTrackInXY::FindTrackInXYProjection
 //  method. Since these parameters are > 60, cint does NOT accept to pass them in the usual
 //  way (parameters in the calling sequence) to PndTrkCTFindTrackInXY::FindTrackInXYProjection.
 FindTrackInXYProjection2_InputOutputData InOut;

 // loading those elements of the struct common to all the candidate tracks.
 InOut.apotemastrawdetectormin = APOTEMASTRAWDETECTORMIN;
 InOut.info = info;
 InOut.infoparalConformal = infoparalConformal;
 InOut.ListParContiguous = fListParContiguous;	// array [NUMBER_STRAWS][6] large; list of Stt
 InOut.maxhitsinfit  = MAXHITSINFIT;
 InOut.maxmvdpixelhitsintrack = MAXMVDPIXELHITSINTRACK;
 InOut.maxmvdstriphitsintrack = MAXMVDSTRIPHITSINTRACK;
 InOut.maxstthitsintrack = MAXSTTHITSINTRACK;
 InOut.minimumhitspertrack = MINIMUMHITSPERTRACK;
 InOut.nMvdPixelHit = fnMvdPixelHit;
 InOut.nMvdStripHit = fnMvdStripHit;
 InOut.nParContiguous = fnParContiguous; // array NUMBER_STRAWS large; nParContiguous[i] --> number of
 					//  contiguous Stt Straws (max = 6);
 InOut.number_straws = NUMBER_STRAWS;
// r_stt_inner_par_max ==> radius of the circumscribed circumference to the
					//; outer hexagon defining THE INNER axial Stt straw region;
 InOut.r_stt_inner_par_max = APOTEMAMAXINNERPARSTRAW *2./sqrt(3.) ;
 InOut.StrawCode = fStrawCode; // Short_t array NUMBER_STRAWS large; 
 InOut.StrawCode2 = fStrawCode2; // Short_t array NUMBER_STRAWS large; 
 InOut.SttStrawOn = SttStrawOn;  //  tSttStrawOn[i] >= 0 --> it is the Stt hit number corresponding to Stt
				// i-th Tube ID; tSttStrawOn[i] == -1 --> i-th Stt straw NOT hit;
 InOut.TubeID = fTubeID;  // list of Tube ID; fTubeID[i] is Tube Id of i-th Stt hit;
 InOut.thetamax = THETAMAX;
 InOut.thetamin = THETAMIN;
 InOut.XMvdPixel = fXMvdPixel;
 InOut.XMvdStrip = fXMvdStrip;
 InOut.YMvdPixel = fYMvdPixel;
 InOut.YMvdStrip = fYMvdStrip;




//-------------------
 InOut.apotemamaxskewstraw = APOTEMAMAXSKEWSTRAW;
 InOut.Cosine = fCosine;
 InOut.deltanr = DELTAnR;
 InOut.dimensionscitil = DIMENSIONSCITIL;
 InOut.FiConformalIndex = FiConformalIndex;
 InOut.HitsinBoxConf = HitsinBoxConformal;
 InOut.InclusionListStt = fInclusionListStt;
 InOut.InclusionListSciTil = fInclusionListSciTil;
 InOut.legiandre_nthetadiv = LEGIANDRE_NTHETADIV;
 InOut.legiandre_nradiusdiv = LEGIANDRE_NRADIUSDIV;
 InOut.ListSttParHits = fListSttParHits;
 InOut.maxhitsinfit = MAXHITSINFIT;
 InOut.maxscitilhitsintrack = MAXSCITILHITSINTRACK;
 InOut.maxstthits = MAXSTTHITS;
 InOut.minouterhitspertrack = MINOUTERHITSPERTRACK;
 InOut.nBoxConf = nBoxConformal;
 InOut.nfidivconformal = NFIDIVCONFORMAL;
 InOut.nrdivconformal = NRDIVCONFORMAL;
 InOut.nSciTilHits = fnSciTilHits;
 InOut.nsttparhit = nSttParHit;
 InOut.posizSciT = fposizSciTil;
 InOut.radiaConf = fradiaConf;
 InOut.RConformalIndex = RConformalIndex;
 InOut.rstrawdetectormax = RSTRAWDETECTORMAX;
 InOut.Sinus = fSinus;
 InOut.strawradius = STRAWRADIUS;
 InOut.trajectory_vertex = trajectory_vertex;
 InOut.YesSciTil = fYesSciTil;

//---------------------------------------------------------------------------------



 //  U and V only for the parallel Stt hits.

 Double_t
	U[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	V[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK];
 //  variable usedd in the plotting in the Legiandre plot;
 InOut.icounter=0;







//----- loop over the parallel hits

//   begins the first iteration with more severe cuts on the # hits in track candidate

int iconta=0;

 nSttTrackCand=0;    // # tracks found

 for(iCluster=0; iCluster<nFoundClusters ; iCluster++) {

	if(istampa>=2){  cout<<"Begin of Loop of Clusters;  processing cluster n. "<<iCluster<<endl;}

	// output of the found track candidate; assignment of InOut.Fi_low_limit to a already assigned memory
	// is necessary ! So it is for InOut.Fi_up_limit;
	InOut.Fi_low_limit = &Fi_low_limit[nSttTrackCand];
	InOut.Fi_up_limit = &Fi_up_limit[nSttTrackCand];
	InOut.ListMvdPixelHitsinTrack = &fListMvdPixelHitsinTrack[nSttTrackCand][0];
	InOut.ListMvdStripHitsinTrack = &fListMvdStripHitsinTrack[nSttTrackCand][0];
	InOut.Mvdhits = &Mvdhits[nSttTrackCand];
	InOut.nMvdPixelHitsinTrack = &fnMvdPixelHitsinTrack[nSttTrackCand];
	InOut.nMvdStripHitsinTrack = &fnMvdStripHitsinTrack[nSttTrackCand];
	InOut.Oxx = &fOx[nSttTrackCand];
	InOut.Oyy = &fOy[nSttTrackCand];
	InOut.Rr = &fR[nSttTrackCand];

	// StrawCode convention (in the following left or right is looking to the beam from downstream) :
	//   -1 = not a boundary straw;
	//   10= inner axial boundary left;
	//   20= inner axial boundary right;
	//   12= outer VERTICAL (BUT NOT OUTERMOST) axial boundary left;
	//   22= outer VERTICAL (BUT NOT OUTERMOST)  axial boundary right;
	//   13= outermost axial boundary left;
	//   23= outermost axial boundary;  right;
	//   14= outer axial boundary, (innermost), left;
	//   24= outer axial boundary (innermost), right;
	//   15= outer axial boundary, special zone, left;
	//   25= outer axial boundary, special zone, right;

	if( nHitsinCluster[ iCluster ] < MINIMUMHITSPERTRACK )
	{
		if(istampa>=1){  cout<<" Loop of Clusters; this cluster (n. "<<iCluster<<
		") has nHitsinCluster = "<<nHitsinCluster[ iCluster ]
		<<" which is < MINIMUMHITSPERTRACK (= "<<
		MINIMUMHITSPERTRACK<<"), no further processing.\n";}
		continue;
	}
	if( nSttTrackCand >= MAXTRACKSPEREVENT) {
		cout<<"from PndTrkTracking2 :  # n. Tracks found so far = "
		<<nSttTrackCand<<" and it is >= MAXTRACKSPEREVENT ( = "
		 <<MAXTRACKSPEREVENT<<"); exiting from || hit loop.\n";
		break;
	}

	iconta++;

	// the following is only the initial hits in the current track cand;
	fnSttParHitsinTrack[nSttTrackCand] = nHitsinCluster[iCluster];
	for(i=0;i<nHitsinCluster[iCluster];i++){
	  fListSttParHitsinTrack[nSttTrackCand][i]=ListHitsinCluster[iCluster*MAXHITSINCLUSTER+i];
	}

	// outputs from the FindTrackInXYProjection class are stored here;
	InOut.ALFA = &fALFA[nSttTrackCand];
	InOut.BETA = &fBETA[nSttTrackCand];
	InOut.Charge = &Charge[nSttTrackCand];
	InOut.GAMMA = &fGAMMA[nSttTrackCand];
	InOut.ListHitsinTrack = &fListSttParHitsinTrack[nSttTrackCand][0];
	InOut.nHitsinTrack = &fnSttParHitsinTrack[nSttTrackCand];


//---------------------------------------------
// parte da togliere;
	// outputs from the FindTrackInXYProjection class are stored here;
	InOut.Fi_final_helix_referenceframe = &Fi_final_helix_referenceframe[nSttTrackCand];
	InOut.Fi_initial_helix_referenceframe = &Fi_initial_helix_referenceframe[nSttTrackCand];
	InOut.ListSciTilHitsinTrack = &fListSciTilHitsinTrack[nSttTrackCand][0];
	InOut.nSciTilHitsinTrack = &fnSciTilHitsinTrack[nSttTrackCand];
	InOut.S_SciTilHitsinTrack = &fS_SciTilHitsinTrack[nSttTrackCand][0];
	InOut.TypeConf = &fTypeConf[nSttTrackCand];
	InOut.U = &U[nSttTrackCand][0];
	InOut.V = &V[nSttTrackCand][0];

	InOut.icounter++;  // this is the plot number;
//--------------------------------------------------------

	outcome = SttTrackXYFinder.FindTrackInXYProjection(&InOut);

	if(!outcome){
		if(istampa>=1){  cout<<" Loop of Clusters; this cluster (n. "<<iCluster<<
		") has a bad outcome, no further processing.\n";}
	}

	if(!outcome)  continue;
	if( fnMvdPixelHitsinTrack[nSttTrackCand]+
		fnMvdStripHitsinTrack[nSttTrackCand]+
		fnSttParHitsinTrack[nSttTrackCand]<2)
	{
		if(istampa>=1){  cout<<" Loop of Clusters; this cluster (n. "<<iCluster<<
		") generated a track having nPixel+nStrip+nSttParallel<2; no further processing.\n";}
		continue;
	}


	InOut.Oyy = &fOy[nSttTrackCand];
	InOut.Rr = &fR[nSttTrackCand];

// --------  here the track and its hits were found, filling the Inclusion list


//	for(j=0; j<fnSttParHitsinTrack[nSttTrackCand]; j++){
//		fInclusionListStt[fListSttParHitsinTrack[nSttTrackCand][j]] = false;
//	}

	keepit[nSttTrackCand]=true;
	nSttTrackCand++;

	if(istampa>=2){  cout<<"At the bottome of Loop of Clusters; end processing cluster n."
	<<iCluster<<endl;
	}
  }      // end  of   for(iCluster=0; iCluster<nFoundClusters ; iCluster++)



 for(i=0; i<nSttTrackCand;i++){

	//  set FI0 now; FI0 may be changed later in case there is a SZ fitting
	FI0[i]=Fi_initial_helix_referenceframe[i];

	// flag of a good fit in SZ space; later they will be set at true if fit in SZ is successful;
	GoodSkewFit[i]=false;
	fnSttSkewHitsinTrack[i]=0;
	keepit[i]=true;

 }  // end for(i=0; i<nSttTrackCand;i++)



 nTotalCandidates = nSttTrackCand;  // nSttTrackCand is already <= MAXTRACKSPEREVENT.


//-------------- stampa
 if(istampa>=1){
cout<<"\tstampa dopo FindTrackInXYProjection di tutte le trackCand rimaste :\n";
//fPrint.stampetta(IVOLTE,tkeepit,&fListMvdPixelHitsinTrack[0][0],
fPrint.stampetta(IVOLTE,keepit,&fListMvdPixelHitsinTrack[0][0],
&fListMvdStripHitsinTrack[0][0],&fListSttParHitsinTrack[0][0],
&fListSttSkewHitsinTrack[0][0],&fListSciTilHitsinTrack[0][0],
fnMvdPixelHitsinTrack,fnMvdStripHitsinTrack,fnSttParHitsinTrack,
fnSttSkewHitsinTrack,fnSciTilHitsinTrack,nSttTrackCand,
-1, // print all candidates;
MAXMVDPIXELHITSINTRACK,MAXMVDSTRIPHITSINTRACK,MAXSCITILHITSINTRACK,MAXSTTHITSINTRACK,fR,fOx,fOy,FI0,KAPPA);
 }
//-------------- fine stampa



//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//----------- from here on, the XY parameters of the Helix don't change. ------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------

// use the risult just obtained from the fit in XY to do the association of the Skew Straw hits


  for(ncand=0; ncand< nTotalCandidates; ncand++)
  {
	if(!keepit[ncand]) continue;

	if( statusflag[ncand] == -1 ) continue; // this is when the XY circle is contained in the
						// the Mvd region completely; skip the association of
						// the Skews.

	// the arrays  Temporary....   are related to the current candidate; they loose meaning
	// as soon as the loop over the candidate tracks finishes;

	fnSttSkewHitsinTrack[ncand]= AssociateSkewHitsToXYTrack(
		fInclusionListStt, // hit is excluded only if it multiple hit.
		nSttSkewHit,
		fListSttSkewHits,
		fOx[ncand],   //  input : X of center of XY plane circle
		fOy[ncand],   //  input : Y of center of XY plane circle
		fR[ncand],   //  input : Radius of XY plane circle
		info,
		WDX,
		WDY,
		WDZ,
		Fi_low_limit[ncand],	// Fi (in Helix XY frame) lower limit using the Stt detector minimum/maximum radius
		Fi_up_limit[ncand],	// Fi (in Helix XY frame) upper limit using the Stt detector maximum/minimum radius
		Charge[ncand],

		TemporarySkewList, // output : TemporarySkewList[*][0] =  list of selected skew hits
				   // numbers(in skew numbering);
				   // TemporarySkewList[*][1] =  solution number (0 or 1) as per
				   // the calculateintersections  method;
	// the info on TemporaryS, TemporaryZ, TemporaryZDrift, TemporaryZErrorafterTilt  as follows :
	// if   i  is the (original) Skew Hit number, and ii (0 or 1) is
	// the solution, then the infos are stored in the   i + ii*MAXSTTHITS location;
		TemporaryS,       //  output,  S coordinate of selected Skew hit
		TemporaryZ,       //  output,  Z coordinate of selected Skew hit (center wire)
		TemporaryZDrift,   //  output,  drift distance IN Z DIRECTION only, of selected Skew hit
		TemporaryZErrorafterTilt   //  output,  Radius taking into account the tilt, IN Z DIRECTION only, of selected Skew hit
			);



	// limit the total # Stt hits to MAXSTTHITSINTRACK
	if( fnSttSkewHitsinTrack[ncand]+fnSttParHitsinTrack[ncand] > MAXSTTHITSINTRACK ) {
	  if(MAXSTTHITSINTRACK > fnSttParHitsinTrack[ncand])
		fnSttSkewHitsinTrack[ncand]=MAXSTTHITSINTRACK-fnSttParHitsinTrack[ncand];
	  else fnSttSkewHitsinTrack[ncand]=0;
	}
	for(j=0;j<fnSttSkewHitsinTrack[ncand];j++)
	{
		fListSttSkewHitsinTrack[ncand][j]=TemporarySkewList[j][0];
		fListSttSkewHitsinTrackSolution[ncand][j]=TemporarySkewList[j][1];
	}

//-------------------------------------------  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//-------------- stampa
 if(istampa>=2){
cout<<"\tstampa dopo AssociateSkewHitsToXYTrack;\n";
//fPrint.stampetta(IVOLTE,tkeepit,&fListMvdPixelHitsinTrack[0][0],
fPrint.stampetta(IVOLTE,keepit,&fListMvdPixelHitsinTrack[0][0],
&fListMvdStripHitsinTrack[0][0],&fListSttParHitsinTrack[0][0],
&fListSttSkewHitsinTrack[0][0],&fListSciTilHitsinTrack[0][0],
fnMvdPixelHitsinTrack,fnMvdStripHitsinTrack,fnSttParHitsinTrack,
fnSttSkewHitsinTrack,fnSciTilHitsinTrack,nSttTrackCand,
ncand, // print the candidate  ncand;
MAXMVDPIXELHITSINTRACK,MAXMVDSTRIPHITSINTRACK,MAXSCITILHITSINTRACK,MAXSTTHITSINTRACK,fR,fOx,fOy,FI0,KAPPA);
 }
//-------------- fine stampa


// calculate the number of hits to use in the SZ fit later;
	//  nXYZhits = n. of Mvd hits + SciTil hits. However, if there are 2 SciTil
	//  hits in this track (namely two adjacent SciTil tiles have a hit
	//  caused PRESUMABLY by the same track) then count them AS ONE because below
	//  the average of their postions is considered !

	if( fnSciTilHitsinTrack[ncand] == 2) {
	   nXYZhits = fnMvdPixelHitsinTrack[ncand]+fnMvdStripHitsinTrack[ncand]+ 1;
	}else{   // in this case fnSciTilHitsinTrack[ncand] is 0 or 1;
	   nXYZhits = fnMvdPixelHitsinTrack[ncand]+fnMvdStripHitsinTrack[ncand]+
			fnSciTilHitsinTrack[ncand];
	}

	// calculate if there is the need of using some skew hits in the subsequent SZ fit;
	// put in  nhitsinfit  the number of hits used in the subsequent  SZ  fit.


	// the following is valid only for GLPK fits;
	if(YesGLPKfitSZ)
	{	// flag for the GLPK choice of fit;
	  if( nXYZhits <=2){
		fnSttSkewHitsinTrack[ncand]<5 ?
			nhitsinfit = nXYZhits + fnSttSkewHitsinTrack[ncand] :
			nhitsinfit = nXYZhits + 5 ; // 1 0 2 XYZ hit + 5 Skew hits.
	  } else {
		nhitsinfit= nXYZhits;
	  }
	} else {  // other fit choice;

	  // the following is valid for non-GLPK fits;
	  nhitsinfit = nXYZhits + fnSttSkewHitsinTrack[ncand];
	}  // end of if(YesGLPKfitSZ)

	// the following is a protection against declaration of 0 dimension array;
	int dime ;
	if(nhitsinfit>0) dime = nhitsinfit ; else dime=1;


//---------------------   here calculate the S and Z values of Mvd Pixels, Mvd Strips,
//	 Stt Skew hits and SciTil hits (if present).

//  the difference between S and Sbis, ZED and ZEDbis, DriftRadius and DriftRadiusbis,
//  ErrorDriftRadius and ErrorDriftRadiusbis, is that S, ZED etc. contain the list of
//  Pixel+Strips+SciTil + other Skew Stt hits in case Pixel+Strips+SciTil are <= 2; instead
//  Sbis, ZEDbis etc. contain Pixel+Strips+all Skew Stt hits.


	// load the quantities needed for the SZ fit;

	LoadSZetc_forSZfit(
		ncand,	// input
		nhitsinfit,
		TemporaryS,		// input
		TemporaryZ,		// input
		TemporaryZDrift,	// input
		TemporaryZErrorafterTilt,	// input
		YesGLPKfitSZ,		// input

		ErrorDriftRadius,	 // output
		ErrorDriftRadiusbis,	 // output
		DriftRadius,		 // output
		DriftRadiusbis,	 // output
		S,			 // output
		Sbis,		 // output
		ZED,			 // output
		ZEDbis		 // output
	);


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

//-------------- stampa
 if(istampa>=2){
	cout<<"\tstampa prima di FitSZspace, [non in Mvd track section] .\n";
	fPrint.stampetta(
// IVOLTE,tkeepit,&fListMvdPixelHitsinTrack[0][0],&fListMvdStripHitsinTrack[0][0],
IVOLTE,keepit,&fListMvdPixelHitsinTrack[0][0],&fListMvdStripHitsinTrack[0][0],
&fListSttParHitsinTrack[0][0],&fListSttSkewHitsinTrack[0][0],&fListSciTilHitsinTrack[0][0],
fnMvdPixelHitsinTrack,fnMvdStripHitsinTrack,fnSttParHitsinTrack,fnSttSkewHitsinTrack,
fnSciTilHitsinTrack,nSttTrackCand,ncand,MAXMVDPIXELHITSINTRACK,MAXMVDSTRIPHITSINTRACK,
MAXSCITILHITSINTRACK,MAXSTTHITSINTRACK,fR,fOx,fOy,FI0,KAPPA);

 }
//-------------- fine stampa
//---------------------   here do the fit again in the SZ space if there are Mvd hits.
//			  For this, reordering of the  Mvd hits is not necessary.

	resultFitSZagain[ncand] = 0;	// default value, corresponding to a bad SZ fit result;

	if(nhitsinfit>0){
//		resultFitSZagain[ncand] = fit.FitSZspace(
		resultFitSZagain[ncand] = fit.FitSZspace_Chi2_AnnealingtheMvdOnly(
				nhitsinfit,	// n. hits to be fitted
				tS,
				tZED,
				tDriftRadius,
				tErrorDriftRadius,
				FI0[ncand],
				MAXSKEWHITSINFIT,// maximum number of STT Skew hits in fit;
				&emme,
				IVOLTE*100+ncand // number of the accumulation plot.
						);

		if( resultFitSZagain[ncand]==1){
			KAPPA[ncand] = emme;
			GoodSkewFit[ncand] = true;
			if( ncand<= nSttTrackCand ) SttSZfit[ncand]=true;
		} else {
			keepit[ncand]=false;
			GoodSkewFit[ncand] = false;
		}


	}  else {  // continuation of  if(nhitsinfit>0)
		keepit[ncand]=false;
		GoodSkewFit[ncand] = false;
	}   // end of  if(nhitsinfit>0)


//-------------- stampa
 if(istampa>=2){
	cout<<"stampa dopo FitSZspace, [not in Mvd track section], SttTrackCand n. "<<ncand<<", result (1 va bene) = "
	<<resultFitSZagain[ncand]<<endl;
	fPrint.stampetta(
//IVOLTE,tkeepit,&fListMvdPixelHitsinTrack[0][0],&fListMvdStripHitsinTrack[0][0],
IVOLTE,keepit,&fListMvdPixelHitsinTrack[0][0],&fListMvdStripHitsinTrack[0][0],
&fListSttParHitsinTrack[0][0],&fListSttSkewHitsinTrack[0][0],&fListSciTilHitsinTrack[0][0],
fnMvdPixelHitsinTrack,fnMvdStripHitsinTrack,fnSttParHitsinTrack,fnSttSkewHitsinTrack,
fnSciTilHitsinTrack,nSttTrackCand,ncand,MAXMVDPIXELHITSINTRACK,MAXMVDSTRIPHITSINTRACK,
MAXSCITILHITSINTRACK,MAXSTTHITSINTRACK,fR,fOx,fOy,FI0,KAPPA);
	}
//---------------------------------------------------------------------------

//-------------------------------------------

//	use the risult just obtained from the fit in SZ to reject the spurious Skew Straw hits
//	and the Mvd spurious hits; also in this function there is the calculation of the Z position
//	of the SKEW hits and the MVD hits, for a given track candidate (ie for a given Helix
//	circle in the XY plane)

	Short_t MaxTurns;
	Double_t Turns;


	  if(keepit[ncand]){
	    if(fR[ncand] < RSTRAWDETECTORMAX/2.){
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
		&fnMvdPixelHitsinTrack[ncand],	// input and output
		&fListMvdPixelHitsinTrack[ncand][0],// input and output
		&fnMvdStripHitsinTrack[ncand],	// input and output
		&fListMvdStripHitsinTrack[ncand][0],// input and output
		&fnSttSkewHitsinTrack[ncand],	// input and output
		&fListSttSkewHitsinTrack[ncand][0],// input and output
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
		fR[ncand]
		    );

	  }  // end of  if(keepit[ncand])

//------------------------


//-------------- stampa
 if(istampa>=1){
	cout<<"\tstampa dopo EliminateSpuriousSZ, [non in Mvd track section] .\n";

	fPrint.stampetta(
//IVOLTE,tkeepit,&fListMvdPixelHitsinTrack[0][0],&fListMvdStripHitsinTrack[0][0],
IVOLTE,keepit,&fListMvdPixelHitsinTrack[0][0],&fListMvdStripHitsinTrack[0][0],
&fListSttParHitsinTrack[0][0],&fListSttSkewHitsinTrack[0][0],&fListSciTilHitsinTrack[0][0],
fnMvdPixelHitsinTrack,fnMvdStripHitsinTrack,fnSttParHitsinTrack,fnSttSkewHitsinTrack,
fnSciTilHitsinTrack,nSttTrackCand,ncand,MAXMVDPIXELHITSINTRACK,MAXMVDSTRIPHITSINTRACK,
MAXSCITILHITSINTRACK,MAXSTTHITSINTRACK,fR,fOx,fOy,FI0,KAPPA);
 }
//-------------- fine stampa

//--------------------------------------------------------------------------




//	First cleanup based on the absence of Mvd hits

//fYesCleanMvd=false;

	if(fYesCleanMvd){

		// reject the candidate if it is NOT contained in the pipe and
		// therefore it should have at least 1 Mvd hit but it has none.
		if( (!GeomCalculator.IsInTargetPipe(
			fOx[ncand],
			fOy[ncand],
			fR[ncand],
			FI0[ncand],
			KAPPA[ncand],
			Charge[ncand],
			VERTICALGAP/2.) )
				 &&
			fnMvdStripHitsinTrack[ncand]+fnMvdPixelHitsinTrack[ncand]==0)
		{
			keepit[ncand]=false;
		}
	}  // end of  (fYesCleanMvd)

  }	//  end of for(ncand=0; ncand< nTotalCandidates; ncand++)


 // In case some candidate track were not processed in the previous loop, there might
 // be still GoodSkewFit[ncand] = false; in that case the candidate track is rejected
 // because it has no information on KAPPA;
  for(ncand=0; ncand< nTotalCandidates; ncand++){
	if(!GoodSkewFit[ncand]) keepit[ncand]=false;
  }

//--------

//-------------- stampa
 if(istampa>=2){
	cout<<"\tstampa dopo il Cleanup piccolo\n";
	fPrint.stampetta(
//IVOLTE,tkeepit,&fListMvdPixelHitsinTrack[0][0],&fListMvdStripHitsinTrack[0][0],
IVOLTE,keepit,&fListMvdPixelHitsinTrack[0][0],&fListMvdStripHitsinTrack[0][0],
&fListSttParHitsinTrack[0][0],&fListSttSkewHitsinTrack[0][0],&fListSciTilHitsinTrack[0][0],
fnMvdPixelHitsinTrack,fnMvdStripHitsinTrack,fnSttParHitsinTrack,fnSttSkewHitsinTrack,
fnSciTilHitsinTrack,nSttTrackCand,-1,MAXMVDPIXELHITSINTRACK,MAXMVDSTRIPHITSINTRACK,
MAXSCITILHITSINTRACK,MAXSTTHITSINTRACK,fR,fOx,fOy,FI0,KAPPA);
 }
//-------------- fine stampa


//-------------------------------
// reject tracks with too few or too many hits;

  for(ncand=0; ncand< nTotalCandidates; ncand++){
	// check if the track candidate has still at least 2 axial hits;
	if( fnMvdPixelHitsinTrack[ncand]+fnMvdStripHitsinTrack[ncand]+fnSttParHitsinTrack[ncand]<2){
		keepit[ncand]=false;
		continue;
	}
	// limit the total # Stt hits to MAXSTTHITSINTRACK
	if( fnSttSkewHitsinTrack[ncand]+fnSttParHitsinTrack[ncand] > MAXSTTHITSINTRACK ) {
	  if(MAXSTTHITSINTRACK > fnSttSkewHitsinTrack[ncand])
		fnSttParHitsinTrack[ncand]=MAXSTTHITSINTRACK-fnSttSkewHitsinTrack[ncand];
	  else fnSttParHitsinTrack[ncand]=0;
	}
  }  // end of  for(ncand=0; ncand< nTotalCandidates; ncand++)
//---------------

//	ordering all the hits belonging to the candidate track, by increasing fR (large
//	trajectories)  or Conformal variables (better for small trajectories);
//	from candidate n. 0 to candidate n. nTotalCandidates-1; loading fListTrackCandHit.
//	the array ordered are :
//	fListTrackCandHit, fListTrackCandHitType, fListSttParHitsinTrack, fListSttSkewHitsinTrack
//	and also at the end the SciTil hit (if present) is added.


	Ordering_Loading_ListTrackCandHit(
		keepit,
		0,	// starting from candidate # 0
		nTotalCandidates,	// .... up to candidate # nTotalCandidates -1;
		info,
		Trajectory_Start,
		Charge,
		SchosenSkew
		);

	// adding at the end the SciTil hits (if present).
	for(ncand=0; ncand< nTotalCandidates; ncand++){
		if(!keepit[ncand]) continue;
		i=fnMvdPixelHitsinTrack[ncand]+fnMvdStripHitsinTrack[ncand]+
			fnSttParHitsinTrack[ncand]+fnSttSkewHitsinTrack[ncand];
		for(j=0;j<fnSciTilHitsinTrack[ncand];j++) {
			fListTrackCandHit[ncand][i+j]=fListSciTilHitsinTrack[ncand][j];
			fListTrackCandHitType[ncand][i+j] = 1001;
		}
	}  // end of for(ncand=0; ncand< nTotalCandidates; ncand++)






//------------- cleanup section.
	Start[0]=0.;
	Start[1]=0.;
	Start[2]=0.;
	gap = (Double_t) (VERTICALGAP);


    for(ncand=0, nRemainingCandidates=0; ncand< nTotalCandidates; ncand++){
	if(!keepit[ncand]) continue;
	Short_t &nHitsPar = fnSttParHitsinTrack[ncand];
	Short_t &nHitsSkew = fnSttSkewHitsinTrack[ncand];
	int dime;
	if(nHitsSkew>0) { dime = nHitsSkew; } else { dime =1; }
	Double_t auxS[dime];

	for(i=0;i<nHitsSkew;i++){
		auxS[i] = SchosenSkew[ncand][fListSttSkewHitsinTrack[ncand][i]];
	}

	if(fYesClean){
		if(istampa>1) cout<<"PndTrkTracking2, entra in TrackCleanup "<<
			"tracce normali, IVOLTE "<<IVOLTE<<" e track cand. "<<ncand<<endl;
		if ( !Cleaner.TrackCleanup(
		APOTEMAMAXINNERPARSTRAW,
		APOTEMAMAXSKEWSTRAW,
		APOTEMAMINOUTERPARSTRAW,
		APOTEMAMINSKEWSTRAW,
		auxS,
		Charge[ncand],
		FI0[ncand],
		gap,
		info,
		istampa,
		IVOLTE,
		KAPPA[ncand],
		&fListSttParHitsinTrack[ncand][0],
		&fListSttSkewHitsinTrack[ncand][0],
		MAXSTTHITS,
		nHitsPar,
		nHitsSkew,
		fOx[ncand],
		fOy[ncand],
		fR[ncand],
		RSTRAWDETECTORMAX,
		APOTEMASTRAWDETECTORMIN,
		SEMILENGTH_STRAIGHT,
		Start,
		STRAWRADIUS,
		ZCENTER_STRAIGHT
				) ) {
			keepit[ncand]=false;
			continue;
		} // end if

	}  // end of if(fYesClean)

	nRemainingCandidates++;

    }	//  end of for(ncand=0; ncand< nTotalCandidates; ncand++)

//------------- end of cleanup section.


// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------


//--------------------------
 if(istampa>1){
	for(ncand=0; ncand< nTotalCandidates; ncand++){
		cout<<"evento n. "<<IVOLTE<<",  prima di LoadPndTrack_TrackCand, candidato n. "
		<<ncand<<", suo keepit "<<keepit[ncand]<<endl;
	} // end of   for(ncand=nSttTrackCand; ncand< nTotalCandidates; ncand++)
}
//--------------------------




//---------------- begin timer stuff;

  frtime = ftimer.RealTime();
  fctime = ftimer.CpuTime();

  frtime2 = ftimer2.RealTime();
  fctime2 = ftimer2.CpuTime();

/*
  cout <<"\nEvento n." <<IVOLTE<< endl;
  cout << "My Real time " << frtime << " sec, my CPU time " << fctime << " sec" << endl;

  cout << "My cumulative Real time " << frtime2 << " sec, my cumulative CPU time " << fctime2 << " sec" << endl;
*/
//----------------------------- end timer stuff;



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
	dim4 = nTotalCandidates*fnMvdPixelHit;
	dim5 = nTotalCandidates*fnMvdStripHit;
	dim6 = nTotalCandidates*MAXSCITILHITSINTRACK;
	dim7 = nTotalCandidates*fnSciTilHits;
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

	// since in the TrackCand the associated MC track is written in all cases, initialization to -1
	// of daTrackFoundaTrackMC is here performed whether or not doMcComparison is true;
	for(i=0;i<nTotalCandidates;i++){
		daTrackFoundaTrackMC[i] = -1;
	}
	//------------------

 if( doMcComparison ){
	// make the struct for the data to pass to the
	// method PndTrkComparisonMCtruth::ComparisonwithMC ;
	PndTrkComparisonMCtruth_io_Data ioData;
	// load the structure;

	ioData.Bfield = BFIELD;
	ioData.Charge = Charge;
	ioData.Cvel = CVEL;
	ioData.daTrackFoundaTrackMC = tdaTrackFoundaTrackMC;
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
	ioData.InclusionListStt = fSingleHitListStt;  // these are straws with only a single hit;
	ioData.ListMvdPixelHitsinTrack = &fListMvdPixelHitsinTrack[0][0];
	ioData.ListMvdStripHitsinTrack = &fListMvdStripHitsinTrack[0][0];
	ioData.ListSciTilHitsinTrack = &fListSciTilHitsinTrack[0][0];
	ioData.ListSttParHitsinTrack = &fListSttParHitsinTrack[0][0];
	ioData.ListSttSkewHitsinTrack = &fListSttSkewHitsinTrack[0][0];
	ioData.ListTrackCandHit = &fListTrackCandHit[0][0];
	ioData.ListTrackCandHitType = &fListTrackCandHitType[0][0];
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
	ioData.MCSkewAloneX = fMCSkewAloneX ;
	ioData.MCSkewAloneY = fMCSkewAloneY ;
	ioData.MvdPixelCommonList = MvdPixelCommonList;
	ioData.MvdPixelSpuriList = MvdPixelSpuriList;
	ioData.MvdStripCommonList = MvdStripCommonList;
	ioData.MvdStripSpuriList = MvdStripSpuriList;
	ioData.nHitsInMCTrack = tnHitsInMCTrack;
	ioData.nHitsInSciTile = nHitsInSciTile;
	ioData.nMCMvdPixelAlone = nMCMvdPixelAlone;
	ioData.nMCMvdStripAlone = nMCMvdStripAlone;
	ioData.nMCParalAlone = tnMCParalAlone;
	ioData.nMCSciTilAlone = nMCSciTilAlone;
	ioData.nMCSkewAlone = tnMCSkewAlone;
	ioData.nMvdPixelCommon = nMvdPixelCommon;
	ioData.nMvdPixelHitsinTrack = fnMvdPixelHitsinTrack;
	ioData.nMvdStripHitsinTrack = fnMvdStripHitsinTrack;
	ioData.nMvdPixelHit = fnMvdPixelHit;
	ioData.nMvdPixelSpuriinTrack = nMvdPixelSpuriinTrack;
	ioData.nMvdStripCommon = nMvdStripCommon;
	ioData.nMvdStripHit = fnMvdStripHit;
	ioData.nMvdStripSpuriinTrack = nMvdStripSpuriinTrack;
	ioData.nParalCommon = tnParalCommon;
	ioData.nSciTilCommon = nSciTilCommon;
	ioData.nSciTilHits = fnSciTilHits;
	ioData.nSciTilHitsinTrack = fnSciTilHitsinTrack ;
	ioData.nSciTilSpuriinTrack = nSciTilSpuriinTrack ;
	ioData.nSkewCommon = tnSkewCommon;
	ioData.nSkewHitsInMCTrack = tnSkewHitsInMCTrack;
	ioData.nSpuriParinTrack = tnSpuriParinTrack;
	ioData.nSpuriSkewinTrack = tnSpuriSkewinTrack;
	ioData.nSttHit = nSttHit;
	ioData.nSttParHitsinTrack = fnSttParHitsinTrack;
	ioData.nSttSkewHitsinTrack = fnSttSkewHitsinTrack;
	ioData.nTotalCandidates = nTotalCandidates;
	ioData.OriginalSciTilList = &OriginalSciTilList[0][0];
	ioData.Ox = fOx;
	ioData.Oy = fOy;
	ioData.ParalCommonList = tParalCommonList;
	ioData.ParSpuriList = tParSpuriList;
	ioData.R = fR;
	ioData.refindexMvdPixel = frefindexMvdPixel;
	ioData.refindexMvdStrip = frefindexMvdStrip;
	ioData.resultFitSZagain = resultFitSZagain;
	ioData.SciTilCommonList = SciTilCommonList;
	ioData.SciTilSpuriList = SciTilSpuriList;
	ioData.SkewCommonList = tSkewCommonList;
	ioData.SkewSpuriList = tSkewSpuriList;
	ioData.SttSZfit = SttSZfit;
	ioData.XMvdPixel = fXMvdPixel;
	ioData.XMvdStrip = fXMvdStrip;
	ioData.XSciTilCenter = fpSciTilx;
	ioData.YMvdPixel = fYMvdPixel;
	ioData.YMvdStrip = fYMvdStrip;
	ioData.YSciTilCenter = fpSciTily;
	ioData.ZMvdPixel = fZMvdPixel;
	ioData.ZMvdStrip = fZMvdStrip;
	ioData.ZSciTilCenter = fpSciTilz;

	// class for the MC comparison;
	PndTrkComparisonMCtruth cmp;
	fnMCTracks = cmp.ComparisonwithMC( ioData);

 }

//----------

 // write the Macro for visualization of tracks and hits;

 if(iplotta && IVOLTE< fNevents_to_plot){

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


	PndTrkPlotMacros2 mymacro;
	PndTrkPlotMacros2_InputData In_Put;
	In_Put.apotemamaxinnerparstraw = APOTEMAMAXINNERPARSTRAW ;
	In_Put.apotemamaxskewstraw = APOTEMAMAXSKEWSTRAW ;
	In_Put.apotemaminouterparstraw = APOTEMAMINOUTERPARSTRAW ;
	In_Put.apotemaminskewstraw = APOTEMAMINSKEWSTRAW ;
	In_Put.bfield = BFIELD ;
	In_Put.Charge = Charge ;
	In_Put.cvel = CVEL ;
	In_Put.daTrackFoundaTrackMC = tdaTrackFoundaTrackMC ;
	In_Put.dimensionscitil = DIMENSIONSCITIL ;
	In_Put.doMcComparison = doMcComparison ;
	In_Put.FI0 = FI0 ;
	In_Put.fMCTrackArray = fMCTrackArray ;
	In_Put.fSttPointArray = fSttPointArray ;
	In_Put.info =  &info[0][0] ;
	In_Put.IVOLTE = IVOLTE ;
	In_Put.KAPPA = KAPPA ;
	In_Put.keepit = keepit ;
	In_Put.InclusionListSciTil = fInclusionListSciTil ;
	In_Put.InclusionListStt = fSingleHitListStt ;

	In_Put.istampa = istampa ;
	In_Put.ListMvdPixelHitsinTrack = &fListMvdPixelHitsinTrack[0][0] ;
	In_Put.ListMvdStripHitsinTrack = &fListMvdStripHitsinTrack[0][0] ;
	In_Put.ListSciTilHitsinTrack = &fListSciTilHitsinTrack[0][0] ;
	In_Put.ListSttParHitsinTrack = &fListSttParHitsinTrack[0][0] ;
	In_Put.ListSttSkewHitsinTrack = &fListSttSkewHitsinTrack[0][0] ;
	In_Put.ListTrackCandHit = &fListTrackCandHit[0][0] ;
	In_Put.ListTrackCandHitType = &fListTrackCandHitType[0][0] ;
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
	In_Put.MCSkewAloneX = fMCSkewAloneX ;
	In_Put.MCSkewAloneY = fMCSkewAloneY ;
	In_Put.MvdPixelCommonList = MvdPixelCommonList ;
	In_Put.MvdPixelSpuriList = MvdPixelSpuriList ;
	In_Put.MvdStripCommonList = MvdStripCommonList ;
	In_Put.MvdStripSpuriList = MvdStripSpuriList ;
	In_Put.NFIDIVCONFORMAL =  NFIDIVCONFORMAL;
	In_Put.nMCMvdPixelAlone = nMCMvdPixelAlone ;
	In_Put.nMCMvdStripAlone = nMCMvdStripAlone ;
	In_Put.nMCParalAlone = tnMCParalAlone ;
	In_Put.nMCSciTilAlone = nMCSciTilAlone;
	In_Put.nMCSkewAlone = tnMCSkewAlone ;
	In_Put.nMCTracks = fnMCTracks ;
	In_Put.nMvdPixelCommon = nMvdPixelCommon ;
	In_Put.nMvdPixelHit = fnMvdPixelHit ;
	In_Put.nMvdPixelHitsinTrack = fnMvdPixelHitsinTrack ;
	In_Put.nMvdPixelSpuriinTrack = nMvdPixelSpuriinTrack ;
	In_Put.nMvdStripCommon = nMvdStripCommon ;
	In_Put.nMvdStripHit = fnMvdStripHit ;
	In_Put.nMvdStripHitsinTrack = fnMvdStripHitsinTrack ;
	In_Put.nMvdStripSpuriinTrack = nMvdStripSpuriinTrack ;
	In_Put.NRDIVCONFORMAL = NRDIVCONFORMAL;
	In_Put.nParalCommon = tnParalCommon ;
	In_Put.nSciTilCommon = nSciTilCommon ;
	In_Put.nSciTilHits = fnSciTilHits ;
	In_Put.nSciTilHitsinTrack = fnSciTilHitsinTrack ;
	In_Put.nSciTilSpuriinTrack =  nSciTilSpuriinTrack;
	In_Put.nSkewCommon = tnSkewCommon ;
	In_Put.nSpuriParinTrack = tnSpuriParinTrack ;
	In_Put.nSttHit = nSttHit ;
	In_Put.nSttParHit = nSttParHit ;
	In_Put.nSttParHitsinTrack = fnSttParHitsinTrack ;
	In_Put.nSttSkewHit = nSttSkewHit ;
	In_Put.nSttSkewHitsinTrack = fnSttSkewHitsinTrack ;
	In_Put.nTotalCandidates = nTotalCandidates ;
	In_Put.nTrackCandHit = fnTrackCandHit ;
	In_Put.number_straws = NUMBER_STRAWS;
	In_Put.Ox = fOx ;
	In_Put.Oy = fOy ;
	In_Put.ParalCommonList = tParalCommonList ;
	In_Put.ParSpuriList = tParSpuriList ;
	In_Put.posizSciTil = &fposizSciTil[0][0] ;
	In_Put.R = fR ;
	In_Put.radiaConf = fradiaConf;
	In_Put.rstrawdetectormax = RSTRAWDETECTORMAX ;
	In_Put.apotemastrawdetectormin = APOTEMASTRAWDETECTORMIN ;
	In_Put.SchosenSkew = &SchosenSkew[0][0] ;
	In_Put.SciTilCommonList = SciTilCommonList ;
	In_Put.SciTilSpuriList = SciTilSpuriList ;
	In_Put.sigmaXMvdPixel = fsigmaXMvdPixel ;
	In_Put.sigmaXMvdStrip = fsigmaXMvdStrip ;
	In_Put.sigmaYMvdPixel = fsigmaYMvdPixel ;
	In_Put.sigmaYMvdStrip = fsigmaYMvdStrip ;
	In_Put.SkewCommonList = tSkewCommonList ;
	In_Put.SttTubeArray = fSttTubeArray;
	In_Put.StrawCode = fStrawCode;
	In_Put.StrawCode2 = fStrawCode2;
	In_Put.verticalgap = VERTICALGAP ;
	In_Put.XMvdPixel = fXMvdPixel ;
	In_Put.XMvdStrip = fXMvdStrip ;
	In_Put.YMvdPixel = fYMvdPixel ;
	In_Put.YMvdStrip = fYMvdStrip ;
	In_Put.WDX = WDX ;
	In_Put.WDY = WDY ;
	In_Put.WDZ = WDZ ;
	In_Put.ZMvdPixel = fZMvdPixel ;
	In_Put.ZMvdStrip = fZMvdStrip ;

	mymacro.WriteAllMacros(
	In_Put
				);
 }

//----------------

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
	ZchosenSkew,
	tdaTrackFoundaTrackMC	// MC track to which reconstructed tracks are associated;
	);





 return;

}


//---------------------- end of   PndTrkTracking2::Exec


//----------begin of function PndTrkTracking2::AssociateBetterAfterFitSkewHitsToXYTrack

  Short_t PndTrkTracking2::AssociateBetterAfterFitSkewHitsToXYTrack(
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

//----------end of function PndTrkTracking2::AssociateBetterAfterFitSkewHitsToXYTrack



//----------begin of function PndTrkTracking2::AssociateSkewHitsToXYTrack

Short_t PndTrkTracking2::AssociateSkewHitsToXYTrack(
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

 int
	i,
	i1,
	ii,
	iii,
	j,
	Kincl,
	location,
	NAssociated,
	nlow,
	Nmax,
	Nmin,
	nup,
	STATUS;

 Double_t xmin , xmax, ymin, ymax,
           dx, dy, diff, dista, d1, d2,
           delta, deltax, deltay, deltaz, deltaS,
           factor,
           zmin, zmax, Smin, Smax, S1, S2,
           z1, z2, y1, y2,
           vx1, vy1, vz1, C0x1, C0y1, C0z1,
           aaa, bbb, ccc, angle, minor, major,
           distance, Rx, Ry, LL,
           Aellipsis1, Bellipsis1,fi1,
           fmin, fmax, offset, step,
           SkewInclWithRespectToS,
	Tilted_Radius,
	zpos, zpos1, zpos2,
           Tiltdirection1[2],
           zl[200],zu[200],
           POINTS1[6];


 PndTrkCTGeometryCalculations GeomCalculator;


      NAssociated=0;

       for( iii=0; iii< NSkewhits; iii++) {
         i = infoskew[iii];  // i  is the Hit number in original Skew Hit numbering;
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

	// ii =0, 1 is the solution number according to the algoritm of
	// the method   calculateintersections;
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

	Tilted_Radius = STRAWRADIUS*aaa/LL;
	//   info[i][4]  =  semilength of the wire;

	// the following statement adds some margin to take into account the imprecision in the determination
	// of the Helix radius;
	dista = 2.*Aellipsis1 > Tilted_Radius ?  Tilted_Radius :  2.*Aellipsis1;

        if( distance >= info[i][4]+ dista ){
		 continue;
	}

	// store S, Z, ZDrift, ZErrorafterTilt  as follows :
	// if   i  is the (original) Skew Hit number, and ii (0 or 1) is
	// the solution, then the infos are stored in the   i + ii*MAXSTTHITS location;

	location = i + ii*MAXSTTHITS;


        S[location] = atan2(POINTS1[j+1]-Oyy, POINTS1[j]-Oxx) ;  // atan2 returns radians in (-pi and +pi]
        if( S[location] < 0.) S[location] += 2.*PI;


        if(  S[location] < Fi_low_limit) {
           if(  S[location]+2.*PI > Fi_up_limit)  continue;
        }  else if(  S[location] > Fi_up_limit) {
	   if(  S[location]- 2.*PI < Fi_low_limit)  continue;
        }



//---------------------------   end check


	Z[location] = POINTS1[j+2];
	ZDrift[location] = Aellipsis1*Tiltdirection1[0];
	ZErrorafterTilt[location] = 0.02*aaa*Tiltdirection1[0]/LL;

	// the list of the selected Skew skew hits instead, is stored sequentially;

	SkewList[NAssociated][0] = i;  // n. skew hit in original hit numbering
	SkewList[NAssociated][1] = ii;  //  solution 0 or solution 1 were accepted


// check if this skew hit doesn't "push out"  the most external parallel hit
// (see Gianluigi's logbook on page 251)

/*
        Double_t Zh1 = Z[location] - ZDrift[location];
        Double_t Zh2 = Z[location] + ZDrift[location];
        Double_t Sh1 = S[location] - Aellipsis1*Tiltdirection1[1];
        Double_t Sh2 = S[location] + Aellipsis1*Tiltdirection1[1];
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

//----------end of function PndTrkTracking2::AssociateSkewHitsToXYTrack


//------------------------- begin of function  PndTrkTracking2::CalculateSinandCosin
 void PndTrkTracking2::CalculateSinandCosin()
{

  Short_t	i;

  fDELTATHETA	= (THETAMAX-THETAMIN)/LEGIANDRE_NTHETADIV;
  for(i=0;i<LEGIANDRE_NTHETADIV;i++){
	fSinus[i] = sin(THETAMIN + (i+0.5)*fDELTATHETA);
	fCosine[i] = cos(THETAMIN + (i+0.5)*fDELTATHETA);
  }
}
//------------------------- end of function  PndTrkTracking2::CalculateSinandCosin


//------------------------- begin of function  PndTrkTracking2::CollectParSttHitsagain

 void PndTrkTracking2::CollectParSttHitsagain(
	Vec <bool>& keepit,
	Vec <bool>& Mvdhits,
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
		ihit = fListSttParHits[i];

		if( !fSingleHitListStt[ihit] ) continue;
		angle = atan2(info[ihit][1]-fOy[itrack],info[ihit][0]-fOx[itrack]);
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
			(fOx[itrack]-info[ihit][0])*(fOx[itrack]-info[ihit][0])+
			(fOy[itrack]-info[ihit][1])*(fOy[itrack]-info[ihit][1])
				) - fR[itrack]
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

//------------------------- end of function  PndTrkTracking2::CollectParSttHitsagain



//-------------------------  begin of function  PndTrkTracking2::EliminateSpuriousSZ

void PndTrkTracking2::EliminateSpuriousSZ(
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

	bool	already[MAXSTTHITS];

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

	Int_t	len,
		 nr2,
		 nrounds0,
		 nrounds1,
		 nchosen,
		 Nround[4];

//	const Double_t  MvdCut=1.8,
	const Double_t  MvdCut=0.8,
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
		 dista_storage[MAXSTTHITS],
		 Errore[4],
		 Esse[4],
		 Zeta[4];

 PndTrkCTGeometryCalculations GeomC;

	len = sizeof(already);
	memset (already,false,len);

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


		if(	dista < 4.*error
//			dista < 1.1*error
				||
			dista < 2.*minimumSttDriftError
			)
		{

			//  check now if the other solution of the same hit
			//  has already been selected before;

			if( already[ ListSkewHitsinTrack[j] ]){
			  // in this case the other solution has already been
			  // selected; therefore in   dista_storage[ ListSkewHitsinTrack[j] ]
			  // there must be the distance  previously calculated;
			  if( dista_storage[ ListSkewHitsinTrack[j] ]>dista ){
				auxListSttSkew[auxnSttSkew]=ListSkewHitsinTrack[j];
				ErrorchosenSkew[ListSkewHitsinTrack[j]]=error;
				auxnSttSkew++;
			  }
			} else {  // continuation of  if( already[ ListSkewHitsinTrack[j] ] 


			  already[ ListSkewHitsinTrack[j] ] = true;
			  // store the  distance calculated;
			  dista_storage[ListSkewHitsinTrack[j] ]=dista;
			  auxListSttSkew[auxnSttSkew]=ListSkewHitsinTrack[j];
			  ErrorchosenSkew[ListSkewHitsinTrack[j]]=error;
			  auxnSttSkew++;




			} // end of   if( already[ ListSkewHitsinTrack[j] ] )





		}  // end of  if( dista < 4.*error ....)
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
//-------------------------  end of function  PndTrkTracking2::EliminateSpuriousSZ




//----------------  begin of function PndTrkTracking2::ExtractInfoFromMvdTrackCand

void PndTrkTracking2::ExtractInfoFromMvdTrackCand()
{

  Short_t
	i,
	j,
	k;

  Double_t qop;

  PndTrackCand* pMvdTrackCand;

  PndTrackCandHit pndtrackcandhit; 

  for( i= 0; i< fnMvdTrackCand ; i++){

	pMvdTrackCand= (PndTrackCand*) fMvdTrackCandArray->At(i);
	TVector3 dirSeed=pMvdTrackCand->getDirSeed();
	TVector3 posSeed=pMvdTrackCand->getPosSeed();
	qop = pMvdTrackCand->getQoverPseed();
	// n. hits in this track cand
	fnHitMvdTrackCand[i] = pMvdTrackCand->GetNHits();
	if( fnHitMvdTrackCand[i]>MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK){
		cout<<"from PndTrkTracking2, fnHitMvdTrackCand[i] = "<<fnHitMvdTrackCand[i]
		<<" and it is > MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK (="
		<<MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK
		<<"); setting fnHitMvdTrackCand[i] to "
		<<MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK<<endl;
	fnHitMvdTrackCand[i]=MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK;
	}

	Short_t kPixel,kStrip;

	for(j=0, k=0, kPixel=0, kStrip=0; j<fnHitMvdTrackCand[i]; j++){
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
			cout<<"from PndTrkTracking2, this Pixel Mvd hit has a number = "
			<<pndtrackcandhit.GetHitId()<<
			" that is > MAXMVDPIXELHITS (="<<
				MAXMVDPIXELHITS<<"), rejected!"<<endl;
			continue;
		}


		finMvdTrackCandPixel[ pndtrackcandhit.GetHitId() ]= true;
		fListHitTypeMvdTrackCand[i][k] = pndtrackcandhit.GetDetId();
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
			cout<<"from PndTrkTracking2, this Strip Mvd hit has a number = "
			<<pndtrackcandhit.GetHitId()<<
			" that is > MAXMVDSTRIPHITS (="<<
			MAXMVDSTRIPHITS<<"), rejected!"<<endl;
			continue;
		}

		finMvdTrackCandStrip[ pndtrackcandhit.GetHitId() ]= true;
		fListHitTypeMvdTrackCand[i][k] = pndtrackcandhit.GetDetId();
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
	fListHitMvdTrackCand[i][k] = pndtrackcandhit.GetHitId();

	k++;

     }    //   end of    for(j=0; j<fnHitMvdTrackCand; j++)

     fnHitMvdTrackCand[i]=k; // if the case, readjust the # of hits in this candidate.

  }   //   end of        for( i= 0; i< fnMvdTrackCand ; i++)


//  now load the arrays containing the Pixel Hits and Strip Hits
// not belonging to any Mvd Track Cand

  fnMvdUSPixelHitNotTrackCand=0;
  fnMvdDSPixelHitNotTrackCand=0;
  fnMvdUSStripHitNotTrackCand=0;
  fnMvdDSStripHitNotTrackCand=0;
  for( i= 0; i< fnMvdPixelHit ; i++){
	if( ! finMvdTrackCandPixel[i] ){
		if( fZMvdPixel[i]>=0.){
			fListMvdDSPixelHitNotTrackCand[fnMvdDSPixelHitNotTrackCand] = i;
			fnMvdDSPixelHitNotTrackCand++;
		} else {
			fListMvdUSPixelHitNotTrackCand[fnMvdUSPixelHitNotTrackCand] = i;
			fnMvdUSPixelHitNotTrackCand++;
		}
	}
  }

  for( i= 0; i< fnMvdStripHit ; i++){
	if( ! finMvdTrackCandStrip[i] ){
		if( fZMvdStrip[i]>=0.){
			fListMvdDSStripHitNotTrackCand[fnMvdDSStripHitNotTrackCand] = i;
			fnMvdDSStripHitNotTrackCand++;
		} else {
			fListMvdUSStripHitNotTrackCand[fnMvdUSStripHitNotTrackCand] = i;
			fnMvdUSStripHitNotTrackCand++;
		}
	}
  }

 
 }
//-----------------------  end of function   PndTrkTracking2::ExtractInfoFromMvdTrackCand


//----------begin of function PndTrkTracking2::FindCharge

void PndTrkTracking2::FindCharge(
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

//----------end of function PndTrkTracking2::FindCharge




//----------start of function PndTrkTracking2::FixDiscontinuitiesFiangleinSZplane

void PndTrkTracking2::FixDiscontinuitiesFiangleinSZplane(
	Short_t TemporarynSkewHitsinTrack,
	Vec <Double_t> & S,
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
//----------end of function PndTrkTracking2::FixDiscontinuitiesFiangleinSZplane


//---------- begin of function PndTrkTracking2::InfoXYZParal

void PndTrkTracking2::InfoXYZParal(
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

//----------end of function PndTrkTracking2::InfoXYZParal



//--------begin of function PndTrkTracking2::Initial_SttParHits_DecreasingR_Ordering

void  PndTrkTracking2::Initial_SttParHits_DecreasingR_Ordering(
	Double_t info[][7],
	Short_t *ListSttParHi,
	Int_t nSttParHit
	)
{

 Short_t
	j,
	OLDListSttParHits[nSttParHit];

 Int_t
	auxIndex[nSttParHit];

 Double_t
	auxRvalues[nSttParHit];


//   ordering the parallel hits by decreasing spatial radius.

 for (j = 0; j< nSttParHit; j++){
	auxIndex[j]=j;
	auxRvalues[j]=
		info[ListSttParHi[ j ]  ][0]*info[ListSttParHi[ j ]  ][0]+
		info[ListSttParHi[ j ]  ][1]*info[ListSttParHi[ j ]  ][1];
	OLDListSttParHits[j]=ListSttParHi[j];
 }

 PndTrkMergeSort Sorter;

 Sorter.Merge_Sort( (Short_t) nSttParHit, auxRvalues, auxIndex);

 for (j = 0; j< nSttParHit; j++){
	ListSttParHi[ nSttParHit-1-j] = OLDListSttParHits[ auxIndex[ j ]   ];
 }

 return;
}

//--------end function PndTrkTracking2::Initial_SttParHits_DecreasingR_Ordering




//----------------------  begin function   PndTrkTracking2::LoadPndTrack_TrackCand

void PndTrkTracking2::LoadPndTrack_TrackCand(
	bool * keepit,
	bool * SttSZfit,
	Short_t nTotalCandidates,
	Short_t * Charge,
	Int_t nSttTrackCand,
	Double_t *FI0,
	Double_t *KAPPA,
	Double_t info[][7],
	Double_t SchosenSkew[][MAXSTTHITS],
	Double_t ZchosenSkew[][MAXSTTHITS],
	Short_t *daTrackFoundaTrackMC
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
	Oxx = fOx[ncand];
	Oyy = fOy[ncand];
	dis=sqrt( Oxx*Oxx+Oyy*Oyy );
	if( dis < 1.e-20)  continue;
	Ptras = fR[ncand]*0.003*BFIELD;
	Pxini = -Charge[ncand]*Ptras*Oyy/dis;
	Pyini = Charge[ncand]*Ptras*Oxx/dis;

//   starting point not necessarily at x=0., y=0.

	x= fOx[ncand] + fR[ncand]*cos(FI0[ncand]);
	y= fOy[ncand] + fR[ncand]*sin(FI0[ncand]);
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
//	pTrckCand->setMcTrackId(  -1   );
	pTrckCand->setMcTrackId(  daTrackFoundaTrackMC[ncand]   );

	for(j=0; j< fnTrackCandHit[ncand]; j++){

	     switch (fListTrackCandHitType[ncand][j]){
		case 0:
			pTrckCand->AddHit(FairRootManager::Instance()->
				GetBranchId(fMvdPixelBranch),
				(Int_t)fListTrackCandHit[ncand][j],j);

		break;
		case 1:
			pTrckCand->AddHit(FairRootManager::Instance()->
				GetBranchId(fMvdStripBranch),
				(Int_t)fListTrackCandHit[ncand][j],j);

		break;
		case 2:
			pTrckCand->AddHit(FairRootManager::Instance()->
				GetBranchId(fSttBranch),
				(Int_t)fListTrackCandHit[ncand][j],j);

		break;
		case 3:
			pTrckCand->AddHit(FairRootManager::Instance()->
				GetBranchId(fSttBranch),
				(Int_t)fListTrackCandHit[ncand][j],j);

		break;
	     }
	}

	// PndTrack Array loading
	//  last = last hit in found track; first = first hit in found track;
	//  pTrckCand =  pointer to the corresponding  PndTrackCand.

	//  the first hit

	if (fListTrackCandHitType[ncand][0] == 0){  //  Mvd Pixel
		Posiz1[0] = fXMvdPixel[ fListTrackCandHit[ncand][0] ];
		Posiz1[1] = fYMvdPixel[ fListTrackCandHit[ncand][0] ];
		Posiz1[2] = fZMvdPixel[ fListTrackCandHit[ncand][0] ];
		ErrPosition.SetX(fsigmaXMvdPixel[ fListTrackCandHit[ncand][0] ]/sqrt(12.));
		ErrPosition.SetY(fsigmaXMvdPixel[ fListTrackCandHit[ncand][0] ]/sqrt(12.));
		ErrPosition.SetZ(fsigmaXMvdPixel[ fListTrackCandHit[ncand][0] ]/sqrt(12.));
	} else if (fListTrackCandHitType[ncand][0] == 1){  //  Mvd Strip
		Posiz1[0] = fXMvdStrip[ fListTrackCandHit[ncand][0] ];
		Posiz1[1] = fYMvdStrip[ fListTrackCandHit[ncand][0] ];
		Posiz1[2] = fZMvdStrip[ fListTrackCandHit[ncand][0] ];
		ErrPosition.SetX(fsigmaXMvdStrip[ fListTrackCandHit[ncand][0] ]/sqrt(12.));
		ErrPosition.SetY(fsigmaXMvdStrip[ fListTrackCandHit[ncand][0] ]/sqrt(12.));
		ErrPosition.SetZ(fsigmaXMvdStrip[ fListTrackCandHit[ncand][0] ]/sqrt(12.));
	} else if( fListTrackCandHitType[ncand][0] == 2 ){  // it is a parallel straw hit
		InfoXYZParal(
			info,
			fListTrackCandHit[ncand][0],
			fOx[ncand],
			fOy[ncand],
			fR[ncand],
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


	} else if ( fListTrackCandHitType[ncand][0] == 3 ){  // it is a skew straw hit

		Posiz1[0] = fOx[ncand]+fR[ncand]*cos(SchosenSkew[ncand][ fListTrackCandHit[ncand][0] ]);
		Posiz1[1] = fOy[ncand]+fR[ncand]*sin(SchosenSkew[ncand][ fListTrackCandHit[ncand][0] ]);
		Posiz1[2] = ZchosenSkew[ncand][ fListTrackCandHit[ncand][0] ];
		ErrPosition.SetX(0.02);	// 200 microns
		ErrPosition.SetY(0.02);	// 200 microns
		ErrPosition.SetZ(1.);		// 1 cm

	}

	Position.SetX( Posiz1[0] );
	Position.SetY( Posiz1[1] );
	Position.SetZ( Posiz1[2] );
	versor[0] = fOx[ncand]-Posiz1[0];
	versor[1] = fOy[ncand]-Posiz1[1];
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

	k = fnTrackCandHit[ncand]-1;
	if (fListTrackCandHitType[ncand][k] == 0){  //  Mvd Pixel
		Posiz1[0] = fXMvdPixel[ fListTrackCandHit[ncand][k] ];
		Posiz1[1] = fYMvdPixel[ fListTrackCandHit[ncand][k] ];
		Posiz1[2] = fZMvdPixel[ fListTrackCandHit[ncand][k] ];
		ErrPosition.SetX(fsigmaXMvdPixel[ fListTrackCandHit[ncand][k] ]/sqrt(12.));
		ErrPosition.SetY(fsigmaXMvdPixel[ fListTrackCandHit[ncand][k] ]/sqrt(12.));
		ErrPosition.SetZ(fsigmaXMvdPixel[ fListTrackCandHit[ncand][k] ]/sqrt(12.));
	} else if (fListTrackCandHitType[ncand][k] == 1){  //  Mvd Strip
		Posiz1[0] = fXMvdStrip[ fListTrackCandHit[ncand][k] ];
		Posiz1[1] = fYMvdStrip[ fListTrackCandHit[ncand][k] ];
		Posiz1[2] = fZMvdStrip[ fListTrackCandHit[ncand][k] ];
		ErrPosition.SetX(fsigmaXMvdStrip[ fListTrackCandHit[ncand][k] ]/sqrt(12.));
		ErrPosition.SetY(fsigmaXMvdStrip[ fListTrackCandHit[ncand][k] ]/sqrt(12.));
		ErrPosition.SetZ(fsigmaXMvdStrip[ fListTrackCandHit[ncand][k] ]/sqrt(12.));
	} else if( fListTrackCandHitType[ncand][k] == 2 ){  // it is a parallel straw hit
		InfoXYZParal (
			info,
			fListTrackCandHit[ncand][k],
			fOx[ncand],
			fOy[ncand],
			fR[ncand],
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

	} else if ( fListTrackCandHitType[ncand][k] == 3 ){  // it is a skew straw hit

		Posiz1[0] = fOx[ncand]+fR[ncand]*cos(SchosenSkew[ncand][ fListTrackCandHit[ncand][k] ]);
		Posiz1[1] = fOy[ncand]+fR[ncand]*sin(SchosenSkew[ncand][ fListTrackCandHit[ncand][k] ]);
		Posiz1[2] = ZchosenSkew[ncand][ fListTrackCandHit[ncand][k] ];
		ErrPosition.SetX(0.02);	// 200 microns
		ErrPosition.SetY(0.02);	// 200 microns
		ErrPosition.SetZ(1.);	// 1 cm

	}

	Position.SetX( Posiz1[0] );
	Position.SetY( Posiz1[1] );
	Position.SetZ( Posiz1[2] );
	versor[0] = fOx[ncand]-Posiz1[0];
	versor[1] = fOy[ncand]-Posiz1[1];
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


//----------end function PndTrkTracking2::LoadPndTrack_TrackCand



//----------begin function PndTrkTracking2::LoadSZetc_forSZfit


void PndTrkTracking2::LoadSZetc_forSZfit(
	Short_t ncand,	// input
	Short_t nhitsinfit,
	Double_t * TemporaryS,		// input
	Double_t * TemporaryZ,		// input
	Double_t * TemporaryZDrift,	// input
	Double_t * TemporaryZErrorafterTilt,	// input
	bool YesGLPKfitSZ,		// input

	Vec <Double_t>& ErrorDriftRadius,	 // output
	Double_t * ErrorDriftRadiusbis,	 // output
	Vec <Double_t>& DriftRadius,		 // output
	Double_t * DriftRadiusbis,	 // output
	Vec <Double_t> & S,			 // output
	Double_t * Sbis,		 // output
	Vec <Double_t>& ZED,			 // output
	Double_t * ZEDbis		 // output
	)
{
 //---------------------   here calculate the S and Z values of Mvd Pixels, Mvd Strips,
 //	 Stt Skew hits and SciTil hits (if present).

 //  the difference between S and Sbis, ZED and ZEDbis, DriftRadius and DriftRadiusbis,
 //  ErrorDriftRadius and ErrorDriftRadiusbis, is that S, ZED etc. contain the list of
 //  Pixel+Strips+SciTil + other Skew Stt hits in case Pixel+Strips+SciTil are <= 2; instead
 //  Sbis, ZEDbis etc. contain Pixel+Strips+all Skew Stt hits.


	int
	i,
	j,
	k,
	kall,
	location;


	// the Mvd Pixels hit
	for(i=0; i< fnMvdPixelHitsinTrack[ncand]; i++){
		k=fListMvdPixelHitsinTrack[ncand][i];
		ZEDbis[i] = ZED[i] = fZMvdPixel[k];
		S[i] = atan2( fYMvdPixel[k]-fOy[ncand],fXMvdPixel[k]-fOx[ncand]);
		if(S[i]<0.) S[i] +=2.*PI;
		Sbis[i] = S[i];
		// DriftRadius is set conventionally at -1, for later use in the SZ fit;
		// the error on the point used in the fit is ErrorDriftRadius and this
		// is overestimated to be  1cm.
		DriftRadiusbis[i]=DriftRadius[i]=-1.;
		if(YesGLPKfitSZ){
		// the following is the error in case of GLPK fit;
		 ErrorDriftRadiusbis[i]=ErrorDriftRadius[i]= 1. ;
		} else{
		// the following error is conventional for the chi**2 type of fit;
		 ErrorDriftRadiusbis[i]=ErrorDriftRadius[i]= 0.5 ;
		}
	}
	// the Mvd Strips hit
	for(j=0, i = fnMvdPixelHitsinTrack[ncand]; j< fnMvdStripHitsinTrack[ncand]; j++){
		k=fListMvdStripHitsinTrack[ncand][j];
		ZEDbis[i] = ZED[i] = fZMvdStrip[k];
		S[i] = atan2( fYMvdStrip[k]-fOy[ncand],fXMvdStrip[k]-fOx[ncand]);
		if(S[i]<0.) S[i] +=2.*PI;
		Sbis[i] = S[i] ;
		// DriftRadius is set conventionally at -1, for later use in the SZ fit;
		// the error on the point used in the fit is ErrorDriftRadius and this
		// is overestimated to be  1cm.
		DriftRadiusbis[i]=DriftRadius[i]=-1.;
		if(YesGLPKfitSZ){
		// the following is the error in case of GLPK fit;
		 ErrorDriftRadiusbis[i]=ErrorDriftRadius[i]= 1. ;
		} else {
		// the following error is conventional for the chi**2 type of fit;
		 ErrorDriftRadiusbis[i]=ErrorDriftRadius[i]= 0.5 ;
		}
		i ++;
	}

	// the SciTil hit ( when they are 2, the fS_SciTilHitsinTrack is already a mean
	// of the two; then consider only 1 SciTil hit, the first, and make an average
	// of the two Z positions).

	i = fnMvdPixelHitsinTrack[ncand]+fnMvdStripHitsinTrack[ncand];
	if(fnSciTilHitsinTrack[ncand]==2){
		ZED[i]=0.5*(fposizSciTil[fListSciTilHitsinTrack[ncand][0]][2]+
			fposizSciTil[fListSciTilHitsinTrack[ncand][1]][2]);
		S[i] = fS_SciTilHitsinTrack[ncand][0];
		// DriftRadius is set conventionally at -2, for later use in the SZ fit;
		// the error on the point used in the fit is ErrorDriftRadius and this
		// is overestimated to be DIMENSIONSCITIL/2.
		DriftRadius[i]=-2.;
		ErrorDriftRadius[i]= DIMENSIONSCITIL/2.; ;
	} else if (fnSciTilHitsinTrack[ncand]==1){
		ZED[i]=fposizSciTil[fListSciTilHitsinTrack[ncand][0]][2];
		S[i] = fS_SciTilHitsinTrack[ncand][0];
		// DriftRadius is set conventionally at -2, for later use in the SZ fit;
		// the error on the point used in the fit is ErrorDriftRadius and this
		// is overestimated to be DIMENSIONSCITIL/2.
		DriftRadius[i]=-2.;
		ErrorDriftRadius[i]= DIMENSIONSCITIL/2.; ;
	}



	// the Skew Stt hits
	// the info on TemporaryS, TemporaryZ, TemporaryZDrift, TemporaryZErrorafterTilt  as follows :
	// if   i  is the (original) Skew Hit number, and ii (0 or 1) is
	// the solution according to the calculateintersections method ,
	// then the infos are stored in the   i + ii*MAXSTTHITS location;

	for(j=0;j<fnSttSkewHitsinTrack[ncand]; j++){

		kall = fnMvdPixelHitsinTrack[ncand]+
			fnMvdStripHitsinTrack[ncand]+j;
		if( fnSciTilHitsinTrack[ncand] ==2 ){ // in this case only 1 SciTil hit
						// has been considered above;
		  i = fnMvdPixelHitsinTrack[ncand]+fnMvdStripHitsinTrack[ncand]+1+j;
		} else {  // this is the case of 1 or 0 SciTil hits in track; the
			//  (impossible?) case of > 2 SciTil hits has already been
			//  prevented early in PndTrkCTFindTrackInXY.
		  i = fnMvdPixelHitsinTrack[ncand]+fnMvdStripHitsinTrack[ncand]+
			fnSciTilHitsinTrack[ncand] +j ;
		}
		// here :		k = Skew Hit number (original notation),
		//  fListSttSkewHitsinTrackSolution[ncand][j] = number of the solution
		// (0 or 1) according to the  calculateintersections method;
		k=fListSttSkewHitsinTrack[ncand][j];
		location = k + fListSttSkewHitsinTrackSolution[ncand][j]*MAXSTTHITS;

		// calculate the quantities used for the SZ fit only.
		if(i<nhitsinfit){
			S[i] = TemporaryS[location];
			ZED[i]=TemporaryZ[location];
			DriftRadius[i]=TemporaryZDrift[location];
			ErrorDriftRadius[i]=2.*TemporaryZDrift[location];
		}
		//-----------
		ZEDbis[kall]=TemporaryZ[location];
		Sbis[kall]= TemporaryS[location];
		DriftRadiusbis[kall]=TemporaryZDrift[location];
		// overestimate the error on the Drift Radius used in the SZ  fit.
		if( fabs(TemporaryZDrift[location]) >1.e-10) {
		   ErrorDriftRadiusbis[kall]=TemporaryZErrorafterTilt[location];
		} else {
		   ErrorDriftRadiusbis[kall]=0.5;
		}


	}	//   end of  for(j=0;j<fnSttSkewHitsinTrack[ncand]; j++)


 return;
}
//----------end function PndTrkTracking2::LoadSZetc_forSZfit



//--------------------------------  begin function   PndTrkTracking2::MakeInclusionListStt


void PndTrkTracking2::MakeInclusionListStt(
	Int_t nSttHit,
	Short_t * TubeID,
	Double_t info[][7]
	)
{

 int i,j;

// it needs to be initialized for each event !
 memset (fSingleHitListStt,true,sizeof(fSingleHitListStt));
 memset (fInclusionListStt,true,sizeof(fInclusionListStt));
/*
 for(i=0;i<nSttHit;i++){
	fInclusionListStt[i] = true;
	fSingleHitListStt[i] = true;
 }
*/



//	fill the inclusion list for Stt, include only first hit for those straws with
//	multiple hits.


 for(i=0; i< nSttHit-1; i++){
	if( !fInclusionListStt[ i ] ) continue;
	for(j=i+1; j< nSttHit; j++){


//		if(fInclusionListStt[ j ] &&
//			fabs(info[i][0] - info[j][0])<1.e-20 &&
//			fabs(info[i][1] - info[j][1])<1.e-20  )
		if(fInclusionListStt[ j ] &&
			TubeID[i] == TubeID[j]  )
		{
			fSingleHitListStt[j]=fInclusionListStt[j]= false ;
		}
	} //  end of  for(j=i+1; j< Nhits;; j++)


 }   //   end of for(i=0; i< Nhits-1; i++)




}
//--------------------------------  end of function   PndTrkTracking2::MakeInclusionListStt





//------------------------- begin of function  PndTrkTracking2::MatchMvdHitsToSttTracks

void PndTrkTracking2::MatchMvdHitsToSttTracks(
	Vec <bool>& keepit,
	Double_t delta,
	Double_t highqualitycut,
	Short_t nSttTrackCand,
	Double_t *FI0,
	Double_t *Fifirst,
	Vec <Short_t>& CHARGE,
	Short_t *nPixelHitsinTrack, // output
	Short_t ListPixelHitsinTrack[][MAXMVDPIXELHITSINTRACK], // output
	Short_t *nStripHitsinTrack, // output
	Short_t ListStripHitsinTrack[][MAXMVDSTRIPHITSINTRACK] // output
	)
{
	bool specialcase;

	Short_t i,
		jmvdhit;

	Double_t angle,
		anglemax,
		anglemin,
		dist;



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


 // first try attach the Pixels;

	nPixelHitsinTrack[i] = 0;
	for( jmvdhit=0; jmvdhit<fnMvdPixelHit; jmvdhit++){
		angle = atan2(fYMvdPixel[jmvdhit]-fOy[i],fXMvdPixel[jmvdhit]-fOx[i]);
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
			dist=fabs( sqrt( (fOx[i]-fXMvdPixel[jmvdhit])* (fOx[i]-fXMvdPixel[jmvdhit])
			+(fOy[i]-fYMvdPixel[jmvdhit])*(fOy[i]-fYMvdPixel[jmvdhit]))-fR[i]);
			if(dist<delta)
			{
				ListPixelHitsinTrack[i][nPixelHitsinTrack[i]]=jmvdhit;
				nPixelHitsinTrack[i]++;
				if( nPixelHitsinTrack[i] == MAXMVDPIXELHITSINTRACK ) break;
			}
		}	// end of  if(angle > anglemin)
	 }  // end of  for( jmvdhit=0; jmvdhit<fnMvdPixelHits; jmvdhit++)


	// then try attach the Strips;
	nStripHitsinTrack[i] = 0;
	for( jmvdhit=0; jmvdhit<fnMvdStripHit; jmvdhit++){
		angle = atan2(fYMvdStrip[jmvdhit]-fOy[i],fXMvdStrip[jmvdhit]-fOx[i]);
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
			dist=fabs( sqrt( (fOx[i]-fXMvdStrip[jmvdhit])* (fOx[i]-fXMvdStrip[jmvdhit])
			+(fOy[i]-fYMvdStrip[jmvdhit])*(fOy[i]-fYMvdStrip[jmvdhit]))-fR[i]);
			if(dist<delta)
			{
				ListStripHitsinTrack[i][nStripHitsinTrack[i]]=jmvdhit;
				nStripHitsinTrack[i]++;
				if( nStripHitsinTrack[i] == MAXMVDSTRIPHITSINTRACK ) break;
			}
		}	// end of  if(angle > anglemin)
	}  // end of  for( jmvdhit=0; jmvdhit<fnMvdStripHits; jmvdhit++)

 }	// end of for(i=0; i<nSttTrackCand; i++)


 return;
}

//------------------------- end of function  PndTrkTracking2::MatchMvdHitsToSttTracks





//------------------------- begin of function  PndTrkTracking2::MatchMvdHitsToSttTracksagain

void PndTrkTracking2::MatchMvdHitsToSttTracksagain(
	Vec <bool>& keepit,
	Vec <bool>& Mvdhits,
	Double_t delta,
	Double_t highqualitycut,
	Short_t nSttTrackCand,
	Double_t *FI0,
	Double_t *Fifirst,
	Vec <Short_t>& CHARGE,

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
		if( fZMvdPixel[ListPixelHitsinTrack[itrack][j]]>0.){
			ndownstream++ ;
		}
	}
	for(j=0; j<nStripHitsinTrack[itrack]; j++){
		if( fZMvdPixel[ListStripHitsinTrack[itrack][j]]>0.){
			ndownstream++ ;
		}
	}
	if(ndownstream>ntot-ndownstream) downstream=true;
	else downstream=false;

//  loop over the Mvd Pixel and try to attach new Pixels to each candidate track

	naddpix=0;
	for(ipix=0; ipix<fnMvdPixelHit; ipix++){

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
		  angle = atan2(fYMvdPixel[ipix]-fOy[itrack],
			fXMvdPixel[ipix]-fOx[itrack]);
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
			(fOx[itrack]-fXMvdPixel[ipix])*(fOx[itrack]-fXMvdPixel[ipix])
			+(fOy[itrack]-fYMvdPixel[ipix])*(fOy[itrack]-fYMvdPixel[ipix])
			) -fR[itrack]);
			if(dist<highqualitycut){
				List[naddpix]=ipix;
				naddpix++;
			}
		  }	// end of  if(angle > anglemin)
//		} // end of if(flaggo)

	}	//  end of   for(ipix=0; ipix<fnMvdPixelHit; ipix++)

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
	for(istr=0; istr<fnMvdStripHit; istr++){
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
		  angle = atan2(fYMvdStrip[istr]-fOy[itrack],
				fXMvdStrip[istr]-fOx[itrack]);
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
			  (fOx[itrack]-fXMvdStrip[istr])*(fOx[itrack]-fXMvdStrip[istr])
			  +(fOy[itrack]-fYMvdStrip[istr])*(fOy[itrack]-fYMvdStrip[istr])
				) -fR[itrack]);

			if(dist<highqualitycut){
				List[naddstr]=istr;
				naddstr++;
			}
		  }	//  end of  if(angle > anglemin)

//		} // end of if(flaggo)

	}	//  end of   for(istr=0; istr<fnMvdStripHit; istr++)
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

//------------------- end function  PndTrkTracking2::MatchMvdHitsToSttTracksagain




//------------------------- begin of function  PndTrkTracking2::MatchMvdHitsToSttTracks2

void PndTrkTracking2::MatchMvdHitsToSttTracks2(
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
 for( imvdcand=0; imvdcand<fnMvdTrackCand; imvdcand++){
	Dist = 0.;
	ncont=0;
	nn[ngoodmix]=0;
	nHighQuality[ngoodmix]=0;
	for( jmvdhit=0; jmvdhit<fnHitMvdTrackCand[imvdcand]; jmvdhit++){

		if(fListHitTypeMvdTrackCand[imvdcand][jmvdhit]==
		    FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)){
			ncont++;
			angle = atan2(
			fYMvdPixel[fListHitMvdTrackCand[imvdcand][jmvdhit]]-fOy[i],
			fXMvdPixel[fListHitMvdTrackCand[imvdcand][jmvdhit]]-fOx[i]
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
				 (fOx[i]-fXMvdPixel[fListHitMvdTrackCand[imvdcand][jmvdhit]])*
				 (fOx[i]-fXMvdPixel[fListHitMvdTrackCand[imvdcand][jmvdhit]])
				+(fOy[i]-fYMvdPixel[fListHitMvdTrackCand[imvdcand][jmvdhit]])*
				 (fOy[i]-fYMvdPixel[fListHitMvdTrackCand[imvdcand][jmvdhit]]))-fR[i]);
				if(dist<delta)
				{
				     List[ngoodmix][nn[ngoodmix]]=
					fListHitMvdTrackCand[imvdcand][jmvdhit];
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
			fYMvdStrip[fListHitMvdTrackCand[imvdcand][jmvdhit]]-fOy[i],
			fXMvdStrip[fListHitMvdTrackCand[imvdcand][jmvdhit]]-fOx[i]
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
			 (fOx[i]-fXMvdStrip[fListHitMvdTrackCand[imvdcand][jmvdhit]])*
			 (fOx[i]-fXMvdStrip[fListHitMvdTrackCand[imvdcand][jmvdhit]])
			 +(fOy[i]-fYMvdStrip[fListHitMvdTrackCand[imvdcand][jmvdhit]])*
			 (fOy[i]-fYMvdStrip[fListHitMvdTrackCand[imvdcand][jmvdhit]])) -fR[i]);
				if(dist<delta)
				{
				   List[ngoodmix][nn[ngoodmix]]=
					fListHitMvdTrackCand[imvdcand][jmvdhit];
				   ListType[ngoodmix][nn[ngoodmix]]=
				    FairRootManager::Instance()->GetBranchId(fMvdStripBranch);
				   Dist += dist;
				   if( dist<highqualitycut) nHighQuality[ngoodmix]++;

				   nn[ngoodmix]++;
				}
			}	// end of   if(angle > anglemin)
		} // end of    if(fListHitTypeMvdTrackCand[imvdcand][jmvdhit]

	}	// end of   for( jmvdhit=0; jmvdhit<fnHitMvdTrackCand[imvdcand];


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
 }	// end of for( imvdcand=0;imvdcand<fnMvdTrackCand;imvdcand++)


//-------  now use the Mvd which are in no Mvd Track Candidate


//------- first, the DS (downstream) Mvd hits
		nn[ngoodmix]=0;
		DIST[ngoodmix] = 0.;
		nHighQuality[ngoodmix]=0;
		for( jmvdhit=0; jmvdhit<fnMvdDSPixelHitNotTrackCand; jmvdhit++){

			angle = atan2(
				fYMvdPixel[fListMvdDSPixelHitNotTrackCand[jmvdhit]]-fOy[i],
				fXMvdPixel[fListMvdDSPixelHitNotTrackCand[jmvdhit]]-fOx[i]
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
				 (fOx[i]-fXMvdPixel[fListMvdDSPixelHitNotTrackCand[jmvdhit]])*
				 (fOx[i]-fXMvdPixel[fListMvdDSPixelHitNotTrackCand[jmvdhit]])
				+(fOy[i]-fYMvdPixel[fListMvdDSPixelHitNotTrackCand[jmvdhit]])*
				 (fOy[i]-fYMvdPixel[fListMvdDSPixelHitNotTrackCand[jmvdhit]])
						) -fR[i]);
				if(dist<delta)
				{
					List[ngoodmix][nn[ngoodmix]]=
					fListMvdDSPixelHitNotTrackCand[jmvdhit];
					ListType[ngoodmix][nn[ngoodmix]]=
					 FairRootManager::Instance()->GetBranchId(fMvdPixelBranch);
					DIST[ngoodmix] += dist;
					if( dist<highqualitycut) nHighQuality[ngoodmix]++;
						nn[ngoodmix]++;
				}
			}  //  end of     if(angle > anglemin )
		}	//  end  of for( jmvdhit=0; jmvdhit<fnMvdDSPixelHitNotTrackCand; jmvdhit++)

		for( jmvdhit=0; jmvdhit<fnMvdDSStripHitNotTrackCand; jmvdhit++){

			angle = atan2(
				fYMvdStrip[fListMvdDSStripHitNotTrackCand[jmvdhit]]-fOy[i],
				fXMvdStrip[fListMvdDSStripHitNotTrackCand[jmvdhit]]-fOx[i]
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
				 (fOx[i]-fXMvdStrip[fListMvdDSStripHitNotTrackCand[jmvdhit]])*
				 (fOx[i]-fXMvdStrip[fListMvdDSStripHitNotTrackCand[jmvdhit]])
				+(fOy[i]-fYMvdStrip[fListMvdDSStripHitNotTrackCand[jmvdhit]])*
				 (fOy[i]-fYMvdStrip[fListMvdDSStripHitNotTrackCand[jmvdhit]])
					) -fR[i]);


				if(dist<delta)
				{
					List[ngoodmix][nn[ngoodmix]]=
					fListMvdDSStripHitNotTrackCand[jmvdhit];
					ListType[ngoodmix][nn[ngoodmix]]=
					 FairRootManager::Instance()->GetBranchId(fMvdStripBranch);
					DIST[ngoodmix] += dist;
					if( dist<highqualitycut) nHighQuality[ngoodmix]++;
					nn[ngoodmix]++;
				}
			}  //  end of     if(angle > anglemin )

		}	//  end  of for( jmvdhit=0; jmvdhit<fnMvdDSStripHitNotTrackCand; jmvdhit++)

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
		for( jmvdhit=0; jmvdhit<fnMvdUSPixelHitNotTrackCand; jmvdhit++){

			angle = atan2(
			fYMvdPixel[fListMvdUSPixelHitNotTrackCand[jmvdhit]]-fOy[i],
					fXMvdPixel[fListMvdUSPixelHitNotTrackCand[jmvdhit]]-fOx[i]
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
				 (fOx[i]-fXMvdPixel[fListMvdUSPixelHitNotTrackCand[jmvdhit]])*
				 (fOx[i]-fXMvdPixel[fListMvdUSPixelHitNotTrackCand[jmvdhit]])
				+(fOy[i]-fYMvdPixel[fListMvdUSPixelHitNotTrackCand[jmvdhit]])*
				 (fOy[i]-fYMvdPixel[fListMvdUSPixelHitNotTrackCand[jmvdhit]])
						) -fR[i]);
				if(dist<delta)
				{
					List[ngoodmix][nn[ngoodmix]]=
						fListMvdUSPixelHitNotTrackCand[jmvdhit];
					ListType[ngoodmix][nn[ngoodmix]]=
					 FairRootManager::Instance()->GetBranchId(fMvdPixelBranch);
					DIST[ngoodmix] += dist;
					if( dist<highqualitycut) nHighQuality[ngoodmix]++;
						nn[ngoodmix]++;
				}
			}  //  end of     if(angle > anglemin )

		}	//  end  of for( jmvdhit=0; jmvdhit<fnMvdUSPixelHitNotTrackCand; jmvdhit++)

		for( jmvdhit=0; jmvdhit<fnMvdUSStripHitNotTrackCand; jmvdhit++){

			angle = atan2(
				fYMvdStrip[fListMvdUSStripHitNotTrackCand[jmvdhit]]-fOy[i],
				fXMvdStrip[fListMvdUSStripHitNotTrackCand[jmvdhit]]-fOx[i]
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
				 (fOx[i]-fXMvdStrip[fListMvdUSStripHitNotTrackCand[jmvdhit]])*
				 (fOx[i]-fXMvdStrip[fListMvdUSStripHitNotTrackCand[jmvdhit]])
				+(fOy[i]-fYMvdStrip[fListMvdUSStripHitNotTrackCand[jmvdhit]])*
				 (fOy[i]-fYMvdStrip[fListMvdUSStripHitNotTrackCand[jmvdhit]])
					) -fR[i]);


				if(dist<delta)
				{
					List[ngoodmix][nn[ngoodmix]]=
					fListMvdUSStripHitNotTrackCand[jmvdhit];
					ListType[ngoodmix][nn[ngoodmix]]=
					 FairRootManager::Instance()->GetBranchId(fMvdStripBranch);
					DIST[ngoodmix] += dist;
					if( dist<highqualitycut) nHighQuality[ngoodmix]++;
					nn[ngoodmix]++;
				}
			}  //  end of     if(angle > anglemin )

		}	//  end  of for( jmvdhit=0; jmvdhit<fnMvdUSStripHitNotTrackCand; jmvdhit++)

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


if(istampa>=3 ){cout<<"from PndTrkTracking2 : appena prima arbitration, IVOLTE = "<<
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
if(istampa>=3 ){cout<<"from PndTrkTracking2 : goodmix n. 0, total distance (che e' = total distance2) = "<<oldtotal
				<<", e nHighQuality = "<<nHighQuality[0]<<endl;}
			chosenmix=0;
			chosenmix2=0;
			for(j1=1; j1<ngoodmix;j1++){
				total = DIST[j1];
if(istampa>=3){cout<<"from PndTrkTracking2 :\t goodmix n. "<<j1<<", total distance "<<total
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
if(istampa>=3 ){cout<<"from PndTrkTracking2 : fine arbitration, IVOLTE = "<<
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

//------------------------- end of function  PndTrkTracking2::MatchMvdHitsToSttTracks2


//------begin function PndTrkTracking2::OrderingConformal_Loading_ListTrackCandHit
void PndTrkTracking2::OrderingConformal_Loading_ListTrackCandHit(
	bool * keepit,
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


//     ordering all the hits belonging to the candidate track, by increasing fR;
//     forming the new track with Mvd+Stt hits


	// arrays used to store temporarily the info of Mvd hits to be ordered.
	Int_t ListHits[MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK],
		ListHits2[MAXSTTHITSINTRACK];
	Double_t XY[MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK][2],
		XY2[MAXSTTHITSINTRACK][2];

//	for(ncand=FirstCandidate; ncand< LastCandidate; ncand++){

		if(!keepit[ncand]) return;
		fnTrackCandHit[ncand] =fnSttParHitsinTrack[ncand]+fnSttSkewHitsinTrack[ncand]+
					fnMvdPixelHitsinTrack[ncand]+
					fnMvdStripHitsinTrack[ncand];
		// adding the Mvd hits (Pixel and Strips)

		if( fnMvdPixelHitsinTrack[ncand]+
		    fnMvdStripHitsinTrack[ncand] >0){
			for(i=0; i< fnMvdPixelHitsinTrack[ncand]; i++){
			  XY[i][0] = fXMvdPixel[ fListMvdPixelHitsinTrack[ncand][i] ];
			  XY[i][1] = fYMvdPixel[ fListMvdPixelHitsinTrack[ncand][i] ];
			  ListHits[i] = fListMvdPixelHitsinTrack[ncand][i];
			}
			for(i=0; i< fnMvdStripHitsinTrack[ncand]; i++){
			  XY[i+fnMvdPixelHitsinTrack[ncand]][0] =
			    fXMvdStrip[ fListMvdStripHitsinTrack[ncand][i] ];
			  XY[i+fnMvdPixelHitsinTrack[ncand]][1] =
			    fYMvdStrip[ fListMvdStripHitsinTrack[ncand][i] ];
			  // to distinguish between Pixels and Strips, add a number
			  // to the original Strip hit number.
			  ListHits[i+fnMvdPixelHitsinTrack[ncand]] =
				fListMvdStripHitsinTrack[ncand][i]+
				(MAXMVDPIXELHITS+MAXMVDSTRIPHITS)*10 ;
			}

			//  ordering the Mvd Hits
			OrderingUsingConformal(
			   fOx[ncand],
			   fOy[ncand],
			   &Trajectory_Start[ncand][0],
			   fnMvdPixelHitsinTrack[ncand]+fnMvdStripHitsinTrack[ncand],
			   XY, // XY[*][0] = X position, XY[*][0] = Y position.
			   CHARGE[ncand],  // input
			   ListHits  // output
						);
			//  constructing the ordered new Track  Candidate now
			for(i=0; i< fnMvdPixelHitsinTrack[ncand]+
					fnMvdStripHitsinTrack[ncand]; i++){
				if(ListHits[i]<(MAXMVDPIXELHITS+MAXMVDSTRIPHITS)*10){//Pixel.
					fListTrackCandHit[ncand][i] = ListHits[i];
					fListTrackCandHitType[ncand][i] = 0;
				} else { // Strip hits.
					fListTrackCandHit[ncand][i] = ListHits[i]-
						(MAXMVDPIXELHITS+MAXMVDSTRIPHITS)*10;
					fListTrackCandHitType[ncand][i] = 1;
				}
			}  // end of  for(i=0; i< fnMvdPixelHitsinTrack[ncand]+

		}	// end of  if( fnMvdPixelHitsinTrack[ncand]+




		// construction of the second part of the ordered new Track  Candidate

		if( fnSttParHitsinTrack[ncand]+fnSttSkewHitsinTrack[ncand] >0){

			for(i=0; i<fnSttParHitsinTrack[ncand]; i++){
			 XY2[i][0] = info[ fListSttParHitsinTrack[ncand][i] ][0];
			 XY2[i][1] = info[ fListSttParHitsinTrack[ncand][i] ][1];
			 ListHits2[i] = fListSttParHitsinTrack[ncand][i];


			}

			for(i=0; i<fnSttSkewHitsinTrack[ncand]; i++){
			 j = i+fnSttParHitsinTrack[ncand];
			 XY2[j][0] = fOx[ncand]+fR[ncand]*
				cos(SchosenSkew[ncand][fListSttSkewHitsinTrack[ncand][i]]);
			 XY2[j][1] = fOy[ncand]+fR[ncand]*
				sin(SchosenSkew[ncand][fListSttSkewHitsinTrack[ncand][i]]);
			 ListHits2[j] = fListSttSkewHitsinTrack[ncand][i]+
					MAXSTTHITS*10; // in order to distinguish
							//  the Skew hits.
			}


		//  ordering the Stt Hits

			OrderingUsingConformal(
			 fOx[ncand],
			 fOy[ncand],
			 &Trajectory_Start[ncand][0],
			 fnSttParHitsinTrack[ncand]+fnSttSkewHitsinTrack[ncand],
			 XY2, // XY2[*][0] = X position, XY2[*][0] = Y position.
			 CHARGE[ncand],  // input
			 ListHits2  // output
					);

			for(j=0,ipar=0,iskew=0;
				j< fnSttParHitsinTrack[ncand]+fnSttSkewHitsinTrack[ncand];j++){
			  i = j+fnMvdPixelHitsinTrack[ncand]+ fnMvdStripHitsinTrack[ncand];
			  if(ListHits2[j]<MAXSTTHITS*10){  // parallel Stt hit.
			    fListTrackCandHit[ncand][i] = ListHits2[j];
			    fListTrackCandHitType[ncand][i] = 2;
			    fListSttParHitsinTrack[ncand][ipar]=ListHits2[j];
			    ipar++;
			  } else {  // skew Stt hit.
			    fListTrackCandHit[ncand][i] = ListHits2[j]-MAXSTTHITS*10;
			    fListTrackCandHitType[ncand][i] = 3;
			    fListSttSkewHitsinTrack[ncand][iskew]=ListHits2[j]-MAXSTTHITS*10;
			    iskew++;
			  }  // end of  if(ListHits2[j]<MAXSTTHITS*10)
			} // end of for(j=0,ipar=0,iskew=0; ....

		}	// end of  if( fnSttParHitsinTrack[ncand]+


if(istampa>=3) for(int ica=0; ica<fnMvdPixelHitsinTrack[ncand]+fnMvdStripHitsinTrack[ncand]+
	fnSttParHitsinTrack[ncand]+fnSttSkewHitsinTrack[ncand]; ica++){
	cout<<"from PndTrkTracking2, hit n. "<<fListTrackCandHit[ncand][ica]<<", hit type "
	<<fListTrackCandHitType[ncand][ica]<<endl;
}


//	} //   end of  for(ncand=FirstCandidate; ncand< LastCandidate; ncand++)


	return;
}

//--------end function PndTrkTracking2::OrderingConformal_Loading_ListTrackCandHit

//----------begin of function PndTrkTracking2::Ordering_Loading_ListTrackCandHit

void PndTrkTracking2::Ordering_Loading_ListTrackCandHit(
//	Vec<bool>& keepit,
	bool * keepit,
	Short_t FirstCandidate,
	Short_t LastCandidate,
	Double_t info[][7],
	Double_t Trajectory_Start[][2],
//	Vec <Short_t>& CHARGE,
	Short_t * CHARGE,
	Double_t SchosenSkew[][MAXSTTHITS]
				)
{
	Short_t ncand;

	for(ncand=FirstCandidate; ncand< LastCandidate; ncand++){
		// for small radius trajectory better the ordering with conformal.


		if( fR[ncand]< RSTRAWDETECTORMAX/2.){
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
//----------end of function PndTrkTracking2::Ordering_Loading_ListTrackCandHit



//----------begin of function PndTrkTracking2::OrderingR_Loading_ListTrackCandHit

void PndTrkTracking2::OrderingR_Loading_ListTrackCandHit(
	bool* keepit,
	Short_t ncand,
	Double_t info[][7]
	)
{

 Short_t	i,
		j,
		ipar,
		iskew;

 PndTrkMergeSort MergeSort;

//     ordering all the hits belonging to the candidate track, by increasing fR;
//     forming the new track with Mvd+Stt hits


		if(!keepit[ncand]) return;
		fnTrackCandHit[ncand] =fnSttParHitsinTrack[ncand]+fnSttSkewHitsinTrack[ncand]+
					fnMvdPixelHitsinTrack[ncand]+
					fnMvdStripHitsinTrack[ncand];
		Short_t tempmvdindex[fnMvdPixelHitsinTrack[ncand]+
					fnMvdStripHitsinTrack[ncand] ],
			 tempmvdtype[fnMvdPixelHitsinTrack[ncand]+
					fnMvdStripHitsinTrack[ncand] ];
		Int_t auxIndex[fnMvdPixelHitsinTrack[ncand]+
					fnMvdStripHitsinTrack[ncand] ];
		Double_t auxR[fnMvdPixelHitsinTrack[ncand]+
					fnMvdStripHitsinTrack[ncand] ];
		// adding the Mvd hits (Pixel and Strips)
		for(i=0; i< fnMvdPixelHitsinTrack[ncand]; i++){
			auxR[i] =
			 fXMvdPixel[ fListMvdPixelHitsinTrack[ncand][i] ]*
			 fXMvdPixel[ fListMvdPixelHitsinTrack[ncand][i] ]+
			 fYMvdPixel[ fListMvdPixelHitsinTrack[ncand][i] ]*
			 fYMvdPixel[ fListMvdPixelHitsinTrack[ncand][i] ];
			tempmvdindex[i]=fListMvdPixelHitsinTrack[ncand][i];
			tempmvdtype[i]=0;
			auxIndex[i] = i;
		}
		for(i=0; i< fnMvdStripHitsinTrack[ncand]; i++){
			auxR[i+fnMvdPixelHitsinTrack[ncand]] =
			 fXMvdStrip[ fListMvdStripHitsinTrack[ncand][i] ]*
			 fXMvdStrip[ fListMvdStripHitsinTrack[ncand][i] ]+
			 fYMvdStrip[ fListMvdStripHitsinTrack[ncand][i] ]*
			 fYMvdStrip[ fListMvdStripHitsinTrack[ncand][i] ];
			tempmvdindex[i+fnMvdPixelHitsinTrack[ncand]]=
				fListMvdStripHitsinTrack[ncand][i];
			tempmvdtype[i+fnMvdPixelHitsinTrack[ncand]]=1;
			auxIndex[i+fnMvdPixelHitsinTrack[ncand]]=
				i+fnMvdPixelHitsinTrack[ncand];
		}

		//  ordering the Mvd Hits
		if( fnMvdPixelHitsinTrack[ncand]+
		    fnMvdStripHitsinTrack[ncand] >0){
			MergeSort.Merge_Sort( fnMvdPixelHitsinTrack[ncand]+
		                    fnMvdStripHitsinTrack[ncand],
				    auxR, auxIndex);

		//  constructing the first part of the ordered new Track  Candidate
			for(i=0; i< fnMvdPixelHitsinTrack[ncand]+
					fnMvdStripHitsinTrack[ncand]; i++){
				fListTrackCandHit[ncand][i] = tempmvdindex[ auxIndex[i] ];
				fListTrackCandHitType[ncand][i] = tempmvdtype[ auxIndex[i] ];

			}
		}	// end of  if( fnMvdPixelHitsinTrack[ncand]+

		// construction of the second part of the ordered new Track  Candidate

		Short_t tempmvdindex2[fnSttParHitsinTrack[ncand]+
					fnSttSkewHitsinTrack[ncand] ],
			 tempmvdtype2[fnSttParHitsinTrack[ncand]+
					fnSttSkewHitsinTrack[ncand] ];
		Int_t auxIndex2[fnSttParHitsinTrack[ncand]+
					fnSttSkewHitsinTrack[ncand] ];
		Double_t auxR2[fnSttParHitsinTrack[ncand]+
					fnSttSkewHitsinTrack[ncand] ];

		for(i=0; i<fnSttParHitsinTrack[ncand]; i++){
			auxR2[i] =
			 info[ fListSttParHitsinTrack[ncand][i] ][0]*
			 info[ fListSttParHitsinTrack[ncand][i] ][0]+
			 info[ fListSttParHitsinTrack[ncand][i] ][1]*
			 info[ fListSttParHitsinTrack[ncand][i] ][1];
			tempmvdindex2[i]=fListSttParHitsinTrack[ncand][i];
			tempmvdtype2[i]=2;
			auxIndex2[i] = i;
		}
		for(i=0; i<fnSttSkewHitsinTrack[ncand]; i++){
			j = i+fnSttParHitsinTrack[ncand];
			auxR2[j] =
			 info[ fListSttSkewHitsinTrack[ncand][i] ][0]*
			 info[ fListSttSkewHitsinTrack[ncand][i] ][0]+
			 info[ fListSttSkewHitsinTrack[ncand][i] ][1]*
			 info[ fListSttSkewHitsinTrack[ncand][i] ][1];
			tempmvdindex2[j]=fListSttSkewHitsinTrack[ncand][i];
			tempmvdtype2[j]=3;
			auxIndex2[j] = j;
		}


		//  ordering the Stt Hits
		if( fnSttParHitsinTrack[ncand]+fnSttSkewHitsinTrack[ncand] >0){
			MergeSort.Merge_Sort( fnSttParHitsinTrack[ncand]+fnSttSkewHitsinTrack[ncand],
				    auxR2, auxIndex2);

			for(j=0,ipar=0,iskew=0;
				  j< fnSttParHitsinTrack[ncand]+fnSttSkewHitsinTrack[ncand];j++){
				i = j+fnMvdPixelHitsinTrack[ncand]+
				    fnMvdStripHitsinTrack[ncand];
				fListTrackCandHit[ncand][i] = tempmvdindex2[ auxIndex2[j] ];
				fListTrackCandHitType[ncand][i] = tempmvdtype2[ auxIndex2[j] ];

				if( fListTrackCandHitType[ncand][i]==2) {
				  fListSttParHitsinTrack[ncand][ipar]=tempmvdindex2[auxIndex2[j]];
				  ipar++;
				} else {
				  fListSttSkewHitsinTrack[ncand][iskew]=tempmvdindex2[auxIndex2[j]];
				  iskew++;
				}
			}
		}	// end of  if( fnSttParHitsinTrack[ncand]+






	return;
}

//----------end of function PndTrkTracking2::OrderingR_Loading_ListTrackCandHit



//----------begin of function PndTrkTracking2::OrderingSttSkewandSttParallel

 void PndTrkTracking2::OrderingSttSkewandSttParallel(
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
//  Moreover, the code before is supposed to have selected trajectories in XY with (fOx,fOy)
//  farther from (0,0) by > 0.9 * RminStrawDetector/2 and consequently fOx and fOy are not both 0.
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
//----------end of function PndTrkTracking2::OrderingSttSkewandSttParallel


//----------begin of function PndTrkTracking2::OrderingUsingConformal

void   PndTrkTracking2::OrderingUsingConformal(
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
//  Moreover, the code before is supposed to have selected trajectories in XY with (fOx,fOy)
//  farther from (0,0) by > 0.9 * RminStrawDetector/2 and consequently fOx and fOy are not both 0.
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
//----------end of function PndTrkTracking2::OrderingUsingConformal


//------------------ begin function  PndTrkTracking2::RefitMvdStt

void  PndTrkTracking2::RefitMvdStt(
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


// PndTrkGlpkFits fit;
// PndTrkLegendreFits fit;
 PndTrkChi2Fits fit;


	*status= false;
	factor=3.;
	mindis=0.5;

	for(i=0, iparallel=0; i<nCandHit && iparallel< MAXHITSINFIT; i++){



		if(i==iexcl) continue;
		if( ListCandHitType[i] == 0 ){	// mvd pixels
			//----- translate the little circumference in XY representing
			//  approximately the sensitive area into the conformal space
			//  circumference

			dist2 = (fXMvdPixel[ListCandHit[i]]-tv[0])*
				(fXMvdPixel[ListCandHit[i]]-tv[0])+
				(fYMvdPixel[ListCandHit[i]]-tv[1])*
				(fYMvdPixel[ListCandHit[i]]-tv[1]);
			if(dist2<mindis) continue;	// this is to exclude Mvd hits too close to the traslated
						// center of reference frame (given by tv[0], tv[1]). This
						// would cause the subsequent fit to fail.

			gamma = dist2 - ErrorMvd*ErrorMvd; // for Pixels
				// I assume the 'drift radius' to be  the max dimension of
				//  the Pixel
			Xconformal[iparallel] = (fXMvdPixel[ListCandHit[i]]-tv[0])/gamma;
			Yconformal[iparallel] = (fYMvdPixel[ListCandHit[i]]-tv[1])/gamma;
			DriftRadiusconformal[iparallel]=-1.;// only to signal later this is a Mvd hit.
			ErrorDriftRadiusconformal[iparallel]=factor*ErrorMvd/fabs(gamma);
			iparallel++;
		} else if ( ListCandHitType[i] == 1 ){	// mvd strips
			//----- translate the little circumference in XY representing
			//  approximately the sensitive area into the conformal space
			//  circumference
			dist2 = (fXMvdStrip[ListCandHit[i]]-tv[0])*
				(fXMvdStrip[ListCandHit[i]]-tv[0])+
				(fYMvdStrip[ListCandHit[i]]-tv[1])*
				(fYMvdStrip[ListCandHit[i]]-tv[1]);
			if(dist2<mindis) continue;	// this is to exclude Mvd hits too close to the traslated
						// center of reference frame (given by tv[0], tv[1]). This
						// would cause the subsequent fit to fail.

			gamma = dist2 - ErrorMvd*ErrorMvd;// for Strips also
				// I assume the 'drift radius' to be  the largest error of
				//  the Strips (which is in the X dimension)
			Xconformal[iparallel] = (fXMvdStrip[ListCandHit[i]]-tv[0])/gamma;
			Yconformal[iparallel] = (fYMvdStrip[ListCandHit[i]]-tv[1])/gamma;
			DriftRadiusconformal[iparallel]=-1.;// only to signal later this is a Mvd hit.
			ErrorDriftRadiusconformal[iparallel]=factor* ErrorMvd/fabs(gamma);
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
			DriftRadiusconformal[iparallel]=info[ListCandHit[i]][3]/fabs(gamma);
			ErrorDriftRadiusconformal[iparallel]=factor*ErrorStraw/fabs(gamma);
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
	//  existatus < 0, fit failed;  Type was set to false in this case;
	if( exitstatus > 0 && Type)	*status=true;
	return;
}


//------------------ end function  PndTrkTracking2::RefitMvdStt


ClassImp(PndTrkTracking2)

