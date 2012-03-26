#include "glpk.h"
#include "PndTracking.h"

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
PndTracking::PndTracking() : FairTask("Tracking") { 
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

PndTracking::PndTracking(Int_t verbose) : FairTask("Tracking") { 
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

PndTracking::PndTracking(int istamp, bool  iplot, bool imc)
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

PndTracking::PndTracking(int istamp, bool  iplot, bool imc, bool doSciTil)
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
PndTracking::~PndTracking() { 

}
// -------------------------------------------------------------------------

//--------------- begin  PndTracking::Initialization_ClassVariables

void PndTracking::Initialization_ClassVariables()
{
// this is only for initializing the Class Variables.
	size_t len;

// first the various values used in the class.
// the UShort_t values :

	MAXHITSINCELL=50;
	MAXHITSINFIT=15;
	MAXMVDMCPOINTS = 2000;
	MINIMUMHITSPERTRACK=3;
	MINOUTERHITSPERTRACK=5;
	TIMEOUT= 60;

//  the Double_t values :

	APOTEMAMAXINNERPARSTRAW = 23.246827;
	APOTEMAMAXSKEWSTRAW = 31.517569; // delimitation of the skew area
	APOTEMAMINOUTERPARSTRAW = 31.863369;
	APOTEMAMINSKEWSTRAW = 23.246827; // delimitation of the skew area
	BFIELD=2.;  // in Tesla
	CVEL = 2.99792;  //  velocity of light
	DELTAnR=2.;   //range of nR in TrkAssociatedParallelHitsToHelixquater
	DIAMETERSTRAWTUBE=1.;
	DIMENSIONSCITIL=2.85; // cm
	ERRORPIXEL=0.02611;
	ERRORSTRIP=0.02611;
	ERRORSQPIXEL=0.00068175;
	ERRORSQSTRIP=0.00068175;
	PI = 3.141592654;
	PMAX=100.;
	RSTRAWDETECTORMAX = 40.73; // maximum radius of the Stt detector in  cm
	RSTRAWDETECTORMIN = 16.119; // minimum radius of the Stt detector in  cm
	STRAWRADIUS = 0.5;
	STRAWRESOLUTION= 0.015;
	STRAW_SKEW_INCLINATION_DEGREES=3.;
	STTDRIFTVEL = 0.0025;	//   in cm/nsec
	VERTICALGAP = 4.; // (cm) gap between Left and Right sections of detector.





// booleans :
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

//  UShort_t :

	nMCTracks=0;
	nSciTilHits=0;

	len = sizeof(ListMvdPixelHitsinTrack);
	memset (ListMvdPixelHitsinTrack,0,len);

	len = sizeof(ListMvdStripHitsinTrack);
	memset (ListMvdStripHitsinTrack,0,len);

	len = sizeof(ListSciTilHitsinTrack);
	memset (ListSciTilHitsinTrack,0,len);

	len = sizeof(ListSttParHitsinTrack);
	memset (ListSttParHitsinTrack,0,len);

	len = sizeof(ListSttSkewHitsinTrack);
	memset (ListSttSkewHitsinTrack,0,len);

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
	nRdivConformalEffective=0;

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


//  puntatori :

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

//--------------- end  PndTracking::Initialization_ClassVariables


// -----   Public method Init   --------------------------------------------

InitStatus PndTracking::Init() {


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
    cout << "-E- PndTracking::Init: "
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
	cout << "-E- PndTracking::Init: No MCTrack array, return!"
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
	cout << "-W- PndTracking::Init: "
	 << "No STTHit array, return!" << endl;
	return kERROR;
 }

//  -------------------------   get the Mvd hits
 fMvdPixelHitArray = (TClonesArray*) ioman->GetObject(fMvdPixelBranch);
//  fMvdPixelHitArray = (TClonesArray*) ioman->GetObject("MVDHitsPixel");
 if ( !fMvdPixelHitArray){
	cout << "-W- PndTracking::Init: " << "No MVD Pixel hitArray, return!" <<endl;
	return kERROR;
 }
 fMvdStripHitArray = (TClonesArray*) ioman->GetObject(fMvdStripBranch);
//  fMvdStripHitArray = (TClonesArray*) ioman->GetObject("MVDHitsStrip");

 if ( !fMvdStripHitArray){
	cout << "-W- PndTracking::Init: " << "No MVD Strip hitArray, return!" <<endl;
	return kERROR;
 }

//  -------------------------   get the Mvd track candidates

 fMvdTrackCandArray = (TClonesArray*) ioman->GetObject("MVDRiemannTrackCand");
 if ( !fMvdTrackCandArray){
	cout << "-W- PndTracking::Init: " << "No MVD TrackCand Array, return!" <<endl;
	return kERROR;
 }

 cout << "-I- PndTracking: Initialization successfull" << endl;

//  -------------------------   get the Mvd MC points

 fMvdMCPointArray = (TClonesArray*) ioman->GetObject("MVDPoint");
 if ( !fMvdMCPointArray){
	cout << "-W- PndTracking::Init: " << "No MVD MC Point Array, return!" <<endl;
	return kERROR;
 }
 cout << "-I- PndTracking: Initialization successfull" << endl;


//--------------------------------  output TClonesArrays ------------------------

 // Create and register output array for PndTrackCand of Stt+Mvd combined

 fSttMvdPndTrackCandArray = new TClonesArray("PndTrackCand");
 ioman->Register("SttMvdTrackCand","SttMvd",fSttMvdPndTrackCandArray, kTRUE);


 // Create and register output array for PndTrack of Stt+Mvd combined

 fSttMvdPndTrackArray = new TClonesArray("PndTrack");
 ioman->Register("SttMvdTrack","SttMvd",fSttMvdPndTrackArray, kTRUE);

//-----------------------


//   calculate the boundaries of the Box in Conformal Space, see Gianluigi logbook on pag. 210-211
 UShort_t i;
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

 nRdivConformalEffective = NRDIVCONFORMAL;



 return kSUCCESS;

}

// -------------------------------------------------------------------------




void PndTracking::SetParContainers() {
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}




void PndTracking::WriteHistograms(){

  TFile* file = FairRootManager::Instance()->GetOutFile();
  file->cd();
  file->mkdir("PndTracking");
  file->cd("PndTracking");
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
void PndTracking::Exec(Option_t* opt) {

 bool
	flag,
	intersect,
	outcome,
	Mvdhits[MAXTRACKSPEREVENT],
	status[MAXTRACKSPEREVENT],
	SttSZfit[MAXTRACKSPEREVENT];

 UShort_t
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
	nSttHitsinTrack[MAXTRACKSPEREVENT],
	//  given a Hit number it gives its radial box number
	RConformalIndex[MAXSTTHITS],
	//  given a Hit number it gives its azimuthal box number
	FiConformalIndex[MAXSTTHITS],
	nTotalCandidates,
	tempore[MAXSTTHITS],
	TemporarySkewList[2*MAXSTTHITS][2],
	BigList[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	ListHitsinTrack[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	ListSttHitsinTrack[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	ListSttHitsinTrackType[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	MCParalAloneList[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	MCSkewAloneList[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	// nBoxConformal,  first index -> radial divisions,
	// 2nd index -> azimuthal divisions; n. of hits falling in this cell.
	nBoxConformal[NRDIVCONFORMAL][NFIDIVCONFORMAL],
	ParalCommonList[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	ParSpuriList[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	SkewCommonList[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	SkewSpuriList[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	HitsinBoxConformal[MAXHITSINCELL][NRDIVCONFORMAL][NFIDIVCONFORMAL];

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

 UInt_t
	iaccept,
	nMvdMCPoint,
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
	Ox[MAXTRACKSPEREVENT],
	Oy[MAXTRACKSPEREVENT],
	Posiz1[3],
	primoangolo[MAXTRACKSPEREVENT],
	Px[MAXTRACKSPEREVENT],
	Py[MAXTRACKSPEREVENT],
	R[MAXTRACKSPEREVENT],
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
	cout<<"\nEntering in PndTrack : evt (starting from 0)  n. "<<IVOLTE<<endl;

//------------------------------------



//----------   fetching the MC truth tracks
	nMCTracks = fMCTrackArray->GetEntriesFast(); // num. tracce/evento
	if (istampa > 1) {
	     cout<<"da PndTracking  : evento (partendo da 0)  N. "<<
	       IVOLTE<< "\n       N. di MC truth tracks  : "<<nMCTracks<<endl;
	}
	if (nMCTracks ==0){
		cout<<"da PndTracking  :  N. di MC truth tracks = 0, return!\n"<<endl;
		return;
	} else if(nMCTracks> MAXMCTRACKS){
		cout<<"da PndTracking  :  N. di MC truth tracks = "<<nMCTracks
		<<" and it is > MAXMCTRACKS = "<<MAXMCTRACKS<<", setting it equal to MAXMCTRACKS.\n";
		nMCTracks = MAXMCTRACKS;
	}

if(istampa>1) stampaMCTracks();


// -------------------------------------  fetch info from MVD

 nMvdPixelHit = fMvdPixelHitArray->GetEntriesFast();
 nMvdStripHit = fMvdStripHitArray->GetEntriesFast();
 if(nMvdPixelHit>MAXMVDPIXELHITS){
	cout<<"from PndTracking, nMvdPixelHit is > maximum allowed ("
	<<MAXMVDPIXELHITS<<") and therefore is set to "
	<<MAXMVDPIXELHITS<<endl;
	nMvdPixelHit=MAXMVDPIXELHITS;
 }
 if(nMvdStripHit>MAXMVDSTRIPHITS){
	cout<<"from PndTracking, nMvdStripHit is > maximum allowed ("
	<<MAXMVDSTRIPHITS<<") and therefore is set to "
	<<MAXMVDSTRIPHITS<<endl;
	nMvdStripHit=MAXMVDSTRIPHITS;
 }

// ---------------------------------------------  get MC Points of  MVD

   nMvdMCPoint = fMvdMCPointArray->GetEntriesFast();
   if(nMvdMCPoint>MAXMVDMCPOINTS) {
	cout<<"from PndTracking, nMvdMCPoint = "<<nMvdMCPoint
	<<" and it is > the maximum number allowed ("<<MAXMVDMCPOINTS<<
	")"<<
	", setting nMvdMCPoint to "<<MAXMVDMCPOINTS<<endl;
	nMvdMCPoint=MAXMVDMCPOINTS;
   }
   if(istampa>2) cout<<"N. MC Points delle Mvd = "<<nMvdMCPoint<<endl;




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

 if(istampa>=2) stampaMvdHits();


// ------------------------------------------ get info from trackcand  of MVD

  nMvdTrackCand = fMvdTrackCandArray->GetEntriesFast();


 if (nMvdTrackCand> MAXMVDTRACKSPEREVENT) {
	cout<<"da PndTracking  :  N. of MvdTrackCand = "<<
	nMvdTrackCand<<" and it is > MAXMVDTRACKSPEREVENT (="<<MAXMVDTRACKSPEREVENT
	<<"),  therefore it is set to "<<MAXMVDTRACKSPEREVENT<<endl;
	nMvdTrackCand= MAXMVDTRACKSPEREVENT;
 }


 if(istampa>=3)cout<<"da PndTracking  :  n. totale di Mvd track cand =    "
		<<nMvdTrackCand<<"  --------------\n";


// -----------------------more info from Mvd trackcand.


 ExtractInfoFromMvdTrackCand();

// -----------------------------------



//---------------------------------------------   fetching the STT  MC points
 nSttMCPoint = fSttPointArray->GetEntriesFast();
 if (nSttMCPoint ==0){
	cout<<"da PndTracking  :  N. di Stt MC points = 0\n"<<endl;
//	return;
 } else  if( nSttMCPoint>MAXSTTHITS){
	cout<<"da PndTracking  :  N. di Stt MC points = "<<nSttMCPoint
	<<" and it is > MAXSTTHITS ("<<MAXSTTHITS<<").\n";
 }


 if(istampa>1) stampaMCSttPoints(nSttMCPoint);


//----------   fetching the STT  hits

 nSttHit = fSttHitArray->GetEntriesFast();
 if (nSttHit ==0){
	cout<<"da PndTracking  :  N. di Stt Hits = 0, return!\n"<<endl;
	return;
 } else if (nSttHit> MAXSTTHITS) {
	cout<<"da PndTracking  :  N. di Stt Hits = "<<nSttHit
	     <<" and it is > MAXSTTHITS (="<<MAXSTTHITS
	     <<"), therefore consider only the first "<<MAXSTTHITS<<" hits\n"<<endl;
	nSttHit= MAXSTTHITS;
 }

 if (istampa >= 1) {
	cout<<"da PndTracking  : evento (partendo da 0)  N. "<<
		IVOLTE<< "\n\tN. totale Hits in STT  : "<<nSttHit<<endl;
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

	if (istampa >= 1) {
		stampaSttHits(
			i,
			ipunto,
			dradius,
			WDX,
			WDY,
			WDZ,
			puntator,
			pSttTube
			);
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
	if(istampa>0)
		cout<<"da PndTracking, event "<<IVOLTE<<", "<<nSciTilHits
		<<" SciTil hits presenti inizialmente.\n";
	if( nSciTilHits>0){
	 PndSciTHit *pPndSciTHit;
	 TVector3  posiz;
	 // the first SciTil hit; this cannot be duplicate hit by definition.
		pPndSciTHit = (PndSciTHit*) fSciTHitArray->At(0);
		posiz = pPndSciTHit->GetPosition();
		if(istampa>0)
			cout<<"da PndTracking SciTil non purgati, Xpos "<<
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
		cout<<"da PndTracking SciTil non purgati, Xpos "
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
	}  // end of if( nSciTilHits>0){

//-----------stampe.
if(istampa>0){
  cout<<"da PndTracking, dopo purga di SciTil; n. hits = "<<nSciTilHits<<endl;
  for(j=0; j<nSciTilHits; j++){
	cout<<"da PndTracking SciTil Xpos "<<posizSciTil[j][0]<<", Ypos "<<
	posizSciTil[j][1]<<", Zpos "<<posizSciTil[j][2]<<endl;
  }
}
//---------- fine stampe.


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




 trajectory_vertex[0]=trajectory_vertex[1]=0.;

 FromXYtoConformal(
	trajectory_vertex,
	info,
	nSttParHit,
	infoparalConformal
		);

 BoxConformalFilling(
	infoparalConformal,
	nSttParHit,
	nBoxConformal,
	HitsinBoxConformal,
	RConformalIndex,
	FiConformalIndex
			);

//     start the track finding procedure


 Double_t
	U[MAXTRACKSPEREVENT][nSttParHit],
	V[MAXTRACKSPEREVENT][nSttParHit];

 nSttTrackCand=0;    // # tracks found

//----- loop over the SciTil hits first;
 if(YesSciTil) {

    for(i=0; i<nSciTilHits ; i++) {
	if( nSttTrackCand > MAXTRACKSPEREVENT) continue;

	nRcell = -1;  // because SciTil hit is outside of the Stt system.
	Fi =  atan2(posizSciTil[i][1],posizSciTil[i][0]) ;
	if ( Fi < 0. ) Fi += 2.*PI;
	 nFicell =  (Short_t) (0.5*NFIDIVCONFORMAL*Fi/PI);
	 if(nFicell > NFIDIVCONFORMAL ) {
		nFicell = NFIDIVCONFORMAL;
	 } else if (nFicell<0) {
		nFicell = 0;
	 }
	outcome = FindTrackInXYProjection(
			-i-1, // seed hit; it is negative for SciTil Hits.
			nRcell,
			nFicell,
			nSttParHit,
			info,
			InclusionListStt,
			RConformalIndex,
			FiConformalIndex,
			nBoxConformal,
			HitsinBoxConformal,
			nSttTrackCand,
			nSttParHitsinTrack,
			ListSttParHitsinTrack,
			trajectory_vertex,
			infoparalConformal,
			posizSciTil[i][0],
			posizSciTil[i][1],
			&S_SciTilHitsinTrack[nSttTrackCand][0],
			Ox,
			Oy,
			R,
			Fi_low_limit,
			Fi_up_limit,
			Fi_initial_helix_referenceframe,
			Fi_final_helix_referenceframe,
			Charge,
			&U[nSttTrackCand][0],
			&V[nSttTrackCand][0]
				);
	if(!outcome){
		continue;
	}
	for(j=0; j<nSttParHitsinTrack[nSttTrackCand]; j++){
	  InclusionListStt[ListSttParHitsinTrack[nSttTrackCand][j]] = false;
	}


//---------stampe.
if(istampa>0){
	cout<<"PndTracking, evt. "<<IVOLTE<<
	", cand. "<<nSttTrackCand<<
	", partendo da SciTil, traccia n. "
	<<nSttTrackCand<<",n SciTilhitsintrack  "<<
	nSciTilHitsinTrack[nSttTrackCand]<<" e loro stampa :\n";
	for(j=0; j<nSciTilHitsinTrack[nSttTrackCand]; j++){
		cout<<"\t hit || n. "<<
		ListSciTilHitsinTrack[nSttTrackCand][j]
		<<", S "<<S_SciTilHitsinTrack[nSttTrackCand][j]<<endl;
	}
	cout<<"\t\tora stampa hit || :\n";
	for(j=0; j<nSttParHitsinTrack[nSttTrackCand]; j++){
		cout<<"\t hit || n. "<<
	  ListSttParHitsinTrack[nSttTrackCand][j]<<endl;
	}
}
//-------fine stampe.

	nSttTrackCand++;

	if( nSttTrackCand >= MAXTRACKSPEREVENT ){
		cout<<
		"from PndSttTrackFinderReal :  # n. Tracks found so far = "
		<<nSttTrackCand
		 <<" and it is >= MAXTRACKSPEREVENT ( = "
		 <<MAXTRACKSPEREVENT
		 <<"); rejecting this event!\n";
		return ;
	}

    } // end of  for(i=0; i<nSciTilHits ; i++)


 }  // end of  if(YesSciTil)


//----- end loop over the SciTil hits.





//----- loop over the parallel hits

//   begins the first iteration with more severe cuts on the # hits in track candidate

 for(iParHit=0; iParHit<nSttParHit + 1 -  MINIMUMHITSPERTRACK ; iParHit++) {
	if( nSttTrackCand > MAXTRACKSPEREVENT) continue;
	if( ! InclusionListStt[ListSttParHits[iParHit]] )  continue;

	nRcell = RConformalIndex[ListSttParHits[iParHit]];
	nFicell = FiConformalIndex[ListSttParHits[iParHit]];

	outcome = FindTrackInXYProjection(
				iParHit,//seed hit; it is positive for STT Hits.
				nRcell,
				nFicell,
				nSttParHit,
				info,
				InclusionListStt,
				RConformalIndex,
				FiConformalIndex,
				nBoxConformal,
				HitsinBoxConformal,
				nSttTrackCand,
				nSttParHitsinTrack,
				ListHitsinTrack,
				trajectory_vertex,
				infoparalConformal,
				1.,	// dummy value, there is no SciTil info in this case;
				1.,	// dummy value, there is no SciTil info in this case
				NULL,	// dummy pointer, there is no
					// S_SciTilHitsinTrack[nSttTrackCand][0]  info.
				Ox,
				Oy,
				R,
				Fi_low_limit,
				Fi_up_limit,
				Fi_initial_helix_referenceframe,
				Fi_final_helix_referenceframe,
				Charge,
				&U[nSttTrackCand][0],
				&V[nSttTrackCand][0]
				);

	if(!outcome)  continue;

// --------  here the track and its hits were found, filling the Inclusion list

 for(j=0; j<nSttParHitsinTrack[nSttTrackCand]; j++){
   InclusionListStt[ListHitsinTrack[nSttTrackCand][j]] = false;
 }
//---------stampe.
if(istampa>0){
	cout<<"PndTrackFinderReal, evt. "<<IVOLTE<<
	", cand. "<<nSttTrackCand<<
	", partendo da Stt || hits, traccia n. "<<nSttTrackCand<<",n SciTilhitsintrack  "<<
	nSciTilHitsinTrack[nSttTrackCand]<<" e loro stampa :\n";
	for(j=0; j<nSciTilHitsinTrack[nSttTrackCand]; j++){
		cout<<"\t hit || n. "<<
		ListSciTilHitsinTrack[nSttTrackCand][j]
		<<", S "<<S_SciTilHitsinTrack[nSttTrackCand][j]<<endl;
	}
	cout<<"\t\tora stampa hit || :\n";
	for(j=0; j<nSttParHitsinTrack[nSttTrackCand]; j++){
		cout<<"\t hit || n. "<<
	  ListHitsinTrack[nSttTrackCand][j]<<endl;
	}
}
//-------fine stampe.

 nSttTrackCand++;

 if( nSttTrackCand >= MAXTRACKSPEREVENT ){
	cout<<"from PndSttTrackFinderReal :  # n. Tracks found so far = "<<nSttTrackCand
	 <<" and it is >= MAXTRACKSPEREVENT ( = "<<MAXTRACKSPEREVENT
	 <<"; rejecting this event and returning -15!\n";
	return;
   }

  }      // end  of   for(iParHit=0; iParHit<nSttParHit+1-MINIMUMHITSPERTRACK; iParHit++)


//------------ stampe.
if(istampa>=2){
    for(j=0;j<nSttTrackCand;j++){
	for(int jc=0;jc<nSciTilHitsinTrack[j];jc++){
		cout<<"from PndTrackFinderReal evt "<<IVOLTE<<", cand "<<jc<<
		", SciTil hit n. "<<ListSciTilHitsinTrack[j][jc]
		<<", Z "<<posizSciTil[ ListSciTilHitsinTrack[j][jc] ][2]
		<<", S  "<<S_SciTilHitsinTrack[j][jc]<<endl;
	}
    }
}
//---------------fine stampe.


//-----------------------
//-----------------------
//-----------------------
//-----------------------  doing the fit with the skew hits for each XY plane track found
//-----------------------
//-----------------------
//-----------------------

/*    daquiparte

 bool
	GoodSkewFit[MAXTRACKSPEREVENT],
	keepit[nSttTrackCand];



 for(i=0; i<nSttTrackCand;i++){

	Double_t
	S[2*MAXSTTHITS],
	Z[2*MAXSTTHITS];

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
	if(MAXSTTHITSINTRACK-nSttParHitsinTrack[i]>0)
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



 outcome = FitSZspace( 
	nSttSkewHitsinTrack[i]+nSciTilHitsinTrack[i],
	tmpS,
	tmpZ,
	tmpZDrift,  // drift radius onto the SZ projection; if negative --> SciTil hit.
	tmpErrorZDrift,
	Fi_initial_helix_referenceframe[i],   //   this is an input;
	MAXHITSINFIT,   // maximum n. hits in fit.
	&KAPPA[i]
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
	 if(MAXSTTHITSINTRACK-nSttParHitsinTrack[i]>0)
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
             Sfinal[i][ListSttSkewHitsinTrack[i][i1]]  =  S[i1] ;
             Zfinal[i][ListSttSkewHitsinTrack[i][i1]]  =  Z[i1] ;
             ZDriftfinal[i][ListSttSkewHitsinTrack[i][i1]]  =  ZDrift[i1] ;
             ZErrorafterTiltfinal[i][ListSttSkewHitsinTrack[i][i1]]  =  ZErrorafterTilt[i1] ;
    }
     // ---------


//     -------------------------------------------------------------



   }   //  end of     for(i=0; i<nSttTrackCand;i++)
//------------------------------------------------------  end of skew hits section


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


//	if ( nSttSkewhitinTrack[i]==0) continue;


if(istampa>=2) 
{
  cout<<"Evt. n. "<<IVOLTE<<", Traccia n. "<<i<<",  list dei "
  <<nSttParHitsinTrack[i]+nSttSkewHitsinTrack[i]
  <<"   hits come stanno in BigList (original notation) :\n";

   for(int ig=0;ig<nSttParHitsinTrack[i]+nSttSkewHitsinTrack[i];ig++){
        cout<<"          hit n.  "<<BigList[i][ig] <<endl;
   }
  cout<<"e ora gli skew hits ordinati (original notation) :\n";
   for(int ig=0;ig<nSttSkewHitsinTrack[i];ig++){
        cout<<"          hit n.  "<<ListSttSkewHitsinTrack[i][ig] <<endl;
   }
}


   }   //  end of     for(i=0; i<nSttTrackCand;i++)

//-----------------------------------

//--------------------- pezzo vecchio

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
			Ox,
			Oy,
			R,
			FI0,
			Fi_low_limit,
			Charge,
			nMvdPixelHitsinTrack, // output
			ListMvdPixelHitsinTrack, // output
			nMvdStripHitsinTrack, // output
			ListMvdStripHitsinTrack // output
			);

//---------inizio stampe.
if(istampa>=2&& IVOLTE<20){
           cout<<"da PndTracking, dopo MatchMvd..2 :\n";
	stampetta(nSttTrackCand,keepit);
}   //end of if(istampa>=0)
//---------------------------------- fine stampe.

//-----------------  end of section with match Mvd hits with Stt hits


	for(  ncand= 0; ncand< nSttTrackCand; ncand++){

		if(nMvdPixelHitsinTrack[ncand]+MAXMVDSTRIPHITSINTRACK==0){

		   Mvdhits[ncand]=false;
		   ALFA[ncand]=-2.*Ox[ncand];
		   BETA[ncand]=-2.*Oy[ncand];
		   GAMMA[ncand]= Ox[ncand]*Ox[ncand]+Oy[ncand]*Oy[ncand]-R[ncand]*R[ncand];



		   // just copy and go to the next track cand.
		   int nbuoni, nparbuoni, nskewbuoni;
		   for(j=0, nbuoni=0,nparbuoni=0, nskewbuoni=0;j<nSttHitsinTrack[ncand]; j++){
			if(istampa>=2) cout<<"da PndTracking, IVOLTE = "<<
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


		if( nMvdPixelHitsinTrack[ncand] > MAXMVDPIXELHITSINTRACK){
			cout<<"from PndTracking, nMvdPixelHitsinTrack["<<ncand
			<<"] is "<<nMvdPixelHitsinTrack[ncand]
			<<"and it is > MAXMVDPIXELHITSINTRACK ("
			<<MAXMVDPIXELHITSINTRACK
			<<"); setting nMvdPixelHitsinTrack[ncand] to "<<
			MAXMVDPIXELHITSINTRACK<<endl;
			nMvdPixelHitsinTrack[ncand]=MAXMVDPIXELHITSINTRACK;
		}

		if( nMvdStripHitsinTrack[ncand] > MAXMVDSTRIPHITSINTRACK){
			cout<<"from PndTracking, nMvdStripHitsinTrack["<<ncand
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
           cout<<"da PndTracking, dopo riordinamento :\n";
	stampetta(nSttTrackCand,keepit);


}   //end of if(istampa>=0)
//---------------------------------- fine stampe.



//---------------------   refit the Helix in XY plane using Stt + Mvd associated hits


	Short_t	iexcl;
	Double_t
		d,
		diff,
		rotationangle;

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

	if(R[ncand]<RSTRAWDETECTORMIN/2.) {
		keepit[ncand]=false;
		continue;
	}

//--------------

//---------inizio stampe.
if(istampa>=2&& IVOLTE<20){
	cout<<"da PndTracking, dopo XY Refit :\n";
	stampetta(nSttTrackCand,keepit);
}   //end of if(istampa>=0)
//---------------------------------- fine stampe.





//  find out if the SciTil hit associated with this track is still acceptable after the last
//  XY refit.

	for(i=0, iaccept=0;i<nSciTilHitsinTrack[ncand];i++){

		intersect=IntersectionSciTil_Circle(
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
			Ox,
			Oy,
			R,
			FI0,
			Fi_low_limit, // because here we deal with hits in Mvd region
			Charge,
			nMvdPixelHitsinTrack, // input and output
			ListMvdPixelHitsinTrack, // input and output
			nMvdStripHitsinTrack, // input and output
			ListMvdStripHitsinTrack // input and output
			);


 if(istampa>=2){
	cout<<"\n\nda PndTracking, dopo MatchMvdHitsToSttTracksagain :"<<endl;
		stampetta(nSttTrackCand,keepit);
 }


  } // end of  if(nMvdPixelHit+nMvdStripHit>0)
//---------------------   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// use the risult just obtained from the fit in XY to redo the association of the Skew Straw hits

  for(ncand=0; ncand< nTotalCandidates; ncand++)
  {
	if(!keepit[ncand]) continue;
	if( ! Mvdhits[ncand])
	{
		if(YesCleanMvd){
	// reject the candidate if it is NOT contained in the pipe and
	// therefore it should have at least 1 Mvd hit but it has none.
			if( (!IsInTargetPipe(	Ox[ncand],
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
	for(j=0;j<nSttSkewHitsinTrack[ncand];j++)
	{
		ListSttSkewHitsinTrack[ncand][j]=TemporarySkewList[j][0];
		ListSttSkewHitsinTrackSolution[ncand][j]=TemporarySkewList[j][1];
	}

 if(istampa>=2){
	cout<<"\n\nda PndTracking, dopo AssociateSkewHitsToXYTrack :"<<endl;
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
	nXYZhits = nMvdPixelHitsinTrack[ncand]+nMvdStripHitsinTrack[ncand]+
			nSciTilHitsinTrack[ncand];
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
	for(j=0;j<nSciTilHitsinTrack[ncand];j++){
		// calculate S on the lateral face of the Helix.
		i = nMvdPixelHitsinTrack[ncand]+nMvdStripHitsinTrack[ncand]+j;
		S[i] = atan2(	SciTilHitsYwithTrack[ncand][j]-Oy[ncand],
				SciTilHitsXwithTrack[ncand][j]-Ox[ncand]);

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
			Charge[ncand]	// this remains unchanged.
					);

//---------------------   here do the fit again in the SZ space if there are Mvd hits.
//			  For this, reordering of the  Mvd hits is not necessary.

 if(istampa>=2){
	cout<<"\n\nda PndTracking, prima di FitSZspace :"<<endl;
	cout<<"\tKAPPA = "<<KAPPA[ncand]<<endl;
	cout<<"\tLista degli S :\n";
	for(int iz=0;iz<nhitsinfit;iz++){
		cout<<"\tS = "<<S[iz]<<", Z "<<ZED[iz]<<endl;
	}
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
			if( ncand<= nSttTrackCand ) SttSZfit[ncand]=true;
		} else {
			keepit[ncand]=false;
		}


 if(istampa>=2){
	cout<<"\n\nda PndTracking, dopo FitSZspace :"<<endl;
	cout<<"\tKAPPA = "<<KAPPA[ncand]<<endl;
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
	    if(R[ncand] < RSTRAWDETECTORMAX/2.){
		if(-Charge[ncand]*KAPPA[ncand]>0.){	// this means Pz>0.
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
	cout<<"\n\nda PndTracking, dopo EliminatespuriousSZ :"<<endl;
		stampetta(nSttTrackCand,keepit);
 }


//	First cleanup based on the absence of Mvd hits


 if(YesCleanMvd){
	// reject the candidate if it is NOT contained in the pipe and
	// therefore it should have at least 1 Mvd hit but it has none.
	if( (!IsInTargetPipe(	Ox[ncand],
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
	 if(istampa>=2){
	cout<<"\n\nda PndTracking, dopo isintargetpipe (Ox="<<Ox[ncand]
	<<", Oy="<<Oy[ncand]<<", R= "<<R[ncand]<<" :"<<endl;
		stampetta(nSttTrackCand,keepit);

	 }
 }  // end of  (YesCleanMvd)

    }	//  end of for(ncand=0; ncand< nTotalCandidates; ncand++)




//---  redo association of parallel Stt  straw  hits to this track, after better refit.
  if(nMvdPixelHit+nMvdStripHit>0){

//---------inizio stampe.
if(istampa>=2&& IVOLTE<20){
           cout<<"da PndTracking, prima di CollectParSttHitsagain :\n";
		stampetta(nSttTrackCand,keepit);

}   //end of if(istampa>=0)
//---------------------------------- fine stampe.

	CollectParSttHitsagain(
			keepit,
			Mvdhits,
			info,
			nSttParHit,
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
           cout<<"da PndTracking, dopo CollectParSttHitsagain :\n";
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

  }  // end of if(nMvdPixelHit+nMvdStripHit==0)

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
if(istampa>1) cout<<"PndTracking, entra in TrackCleanup tracce normali, IVOLTE "<<IVOLTE
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

	UShort_t List[MAXMVDPIXELHITS+MAXMVDSTRIPHITS],
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
		Ox,
		Oy,
		R,
		Trajectory_Start,
		Charge,
		SchosenSkew  // here this array is irrelevant since there are not yet
			// any Stt hits in Track Cand.
		);

//-------------------- end of ordering

// -----------------  finding the charge of those track found starting from Mvd

	int nMvdOnly;
	Double_t Sini, Slast,
		esse[MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK+MAXSTTHITSINTRACK];

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

	   for(j=0;j<nSttSkewHitsinTrack[ncand];j++)
	   {
		ListSttSkewHitsinTrack[ncand][j]=TemporarySkewList[j][0];
		SchosenSkew[ncand][ListSttSkewHitsinTrack[ncand][j]] = TemporaryS[j];

//------------- inizio stampe.
if(istampa>=2){
	cout<<"PndTracking, partendo da Mvd evt. "<<IVOLTE
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

if(istampa>1){ cout<<"PndTracking, entra in TrackCleanup tracce Mvd, IVOLTE "<<IVOLTE
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
if(istampa>1){ cout<<"\tPndTracking, uscito da TrackCleanup tracce Mvd, keepit = ";
	if(keepit[ncand]) cout<<" true!\n"; else  cout<<" false!\n";
}

  }  // end of if(YesClean)

//-----------  end of cleanup of new tracks found.



	} // end of  if(MvdAloneTracking)



// ----------------------------- end of finding Tracks starting with the Mvd tracks
// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------


// skipping: ;

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

		if( Charge[i]>0 ){ //  clockwise.
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
		cout<<"from PndTracking : Track candidate n. "<<i;
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
//		cout<<"\n\nda PndTracking : la PndTrackCand n. "<<ncand;
		cout<<"\n\nda PndTracking, evt. "<<IVOLTE
		<<": la PndTrackCand n. "<<nbuone;
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

//------

		cout<<"inoltre, sempre Track Cand n. "<<nbuone<<", associato a Traccia MC n. "<<
		daTrackFoundaTrackMC[ncand]<<", ha "<<
		nSttParHitsinTrack[ncand]<<
		" Stt || Hits associati, di cui ecco la lista :\n";
		if(nSttParHitsinTrack[ncand]==0) cout<<"\t-----------nessuna lista\n";
		for(j=0;j<nSttParHitsinTrack[ncand];j++){
			cout<<"\tHit n. "<<
			ListSttParHitsinTrack[ncand][j]<<
			" che fu associato a MC track "
			<<info[
			  ListSttParHitsinTrack[ncand][j]
			  ][6]<<endl;
		}
		cout<<"dalla mia funzione risulta che ci sono "<<
		nParalCommon[ncand]<<
		" Stt || hits mecciati alla giusta traccia MC e la loro lista e' :\n";
		if(nParalCommon[ncand]==0) cout<<"\t-----------nessuna lista\n";
		for(j=0;j<nParalCommon[ncand];j++){
			cout<<"\tHit n. "<<
			ParalCommonList[ncand][j]<<
			" che fu associato a MC track "
			<<info[
			  ParalCommonList[ncand][j]
			  ][6]<<endl;
		}
		cout<<"  ... inoltre ci sono  "<<nSpuriParinTrack[ncand]
		<<"  Stt || hits spuri, il cui elenco e' :\n";
		if(nSpuriParinTrack[ncand]==0) cout<<"\t-----------nessuna lista\n";
		for(j=0;j<nSpuriParinTrack[ncand];j++){
			cout<<"\tHit n. "<<
			ParSpuriList[ncand][j]<<
			" che fu associato a MC track "
			<<info[
			  ParSpuriList[ncand][j]
			  ][6]<<endl;
		}

		cout<<"  ... inoltre ci sono  "<<nMCParalAlone[ncand]
		<<"  Stt || hits non inclusi, il cui elenco e' :\n";
		if(nMCParalAlone[ncand]==0) cout<<"\t-----------nessuna lista\n";
		for(j=0;j<nMCParalAlone[ncand];j++){
			cout<<"\tHit n. "<<
			MCParalAloneList[ncand][j]<<
			" che fu associato a MC track "
			<<info[
			  MCParalAloneList[ncand][j]
			  ][6]<<endl;
		}


//------


		cout<<"inoltre, sempre Track Cand n. "<<nbuone<<", associato a Traccia MC n. "<<
		daTrackFoundaTrackMC[ncand]<<", ha "<<
		nSttSkewHitsinTrack[ncand]<<
		" Stt skew Hits associati, di cui ecco la lista :\n";
		if(nSttSkewHitsinTrack[ncand]==0) cout<<"\t-----------nessuna lista\n";
		for(j=0;j<nSttSkewHitsinTrack[ncand];j++){
			cout<<"\tHit n. "<<
			ListSttSkewHitsinTrack[ncand][j]<<
			" che fu associato a MC track "
			<<info[
			  ListSttSkewHitsinTrack[ncand][j]
			  ][6]<<endl;
		}
		cout<<"dalla mia funzione risulta che ci sono "<<
		nSkewCommon[ncand]<<
		" Stt skew hits mecciati alla giusta traccia MC e la loro lista e' :\n";
		if(nSkewCommon[ncand]==0) cout<<"\t-----------nessuna lista\n";
		for(j=0;j<nSkewCommon[ncand];j++){
			cout<<"\tHit n. "<<
			SkewCommonList[ncand][j]<<
			" che fu associato a MC track "
			<<info[
			  SkewCommonList[ncand][j]
			  ][6]<<endl;
		}
		cout<<"  ... inoltre ci sono  "<<nSpuriSkewinTrack[ncand]
		<<"  Stt skew hits spuri, il cui elenco e' :\n";
		if(nSpuriSkewinTrack[ncand]==0) cout<<"\t-----------nessuna lista\n";
		for(j=0;j<nSpuriSkewinTrack[ncand];j++){
			cout<<"\tHit n. "<<
			SkewSpuriList[ncand][j]<<
			" che fu associato a MC track "
			<<info[
			  SkewSpuriList[ncand][j]
			  ][6]<<endl;
		}

		cout<<"  ... inoltre ci sono  "<<nMCSkewAlone[ncand]
		<<"  Stt skew hits non inclusi, il cui elenco e' :\n";
		if(nMCSkewAlone[ncand]==0) cout<<"\t-----------nessuna lista\n";
		for(j=0;j<nMCSkewAlone[ncand];j++){
			cout<<"\tHit n. "<<
			MCSkewAloneList[ncand][j]<<
			" che fu associato a MC track "
			<<info[
			  MCSkewAloneList[ncand][j]
			  ][6]<<endl;
		}


//------

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
		cout<<"da PndTracking, evento (cominciando da 0) n. "<<IVOLTE<<
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





if(istampa>=1){cout<<"da PndTracking, MC comparison; evt. "<<IVOLTE<<", nMCTracks "<<nMCTracks
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

bool flaggo;
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

	flaggo = true;
	for(int g=0; g<nMCTracksaccettabili;g++){
		if( i==ListaMCTracksaccettabili[g])
		{
			flaggo=false;
			break;
		}
	}
	if(flaggo) continue;



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
	int ipinco;
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
		px = -Charge[ncand]*Ptras*versor[1];
		py = Charge[ncand]*Ptras*versor[0];
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
		   ErrPosition, ErrMomentum, Charge[ncand],
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
cout<<"da PndTracking, IVOLTE = "<<IVOLTE<<", traccia n. "<<it<<", elenco ordinato degli hits :\n";
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


 if(iplotta){



// calcolo di S degli eventuali hits SciTil presenti nelle tracce trovate.

	Double_t esseSciTil[MAXTRACKSPEREVENT][MAXSCITILHITS];

	for(  i= 0; i< nTotalCandidates; i++){
		for(j=0;j< nSciTilHitsinTrack[i];j++){

		intersect=IntersectionSciTil_Circle(
			posizSciTil[ListSciTilHitsinTrack[i][j]][0],
			posizSciTil[ListSciTilHitsinTrack[i][j]][1],
			Ox[i], // center of circle.
			Oy[i],
			R[i], // Radius of circle.
			&Nint,
			XintersectionList,
			YintersectionList
						);
// reject case with no intersection of the SciTil with the circle trajectory.
	if(intersect){
	// calculate S on the lateral face of the Helix.
		if ( Nint==1){	// the majority of the cases
			esseSciTil[i][j] = atan2(YintersectionList[0]-Oy[i],
					XintersectionList[0]-Ox[i]);
		} else {  // in this case Nint=2 (it should be a very rare case).
		// do an average of the two positions.
			esseSciTil[i][j] = atan2( 0.5*(YintersectionList[0]+YintersectionList[1])
			-Oy[i],0.5*(XintersectionList[0]+XintersectionList[1])-Ox[i]);
		} // end of  if ( Nint==1)
		if ( esseSciTil[i][j]<0.) esseSciTil[i][j] += 2.*PI;

	}  // continuation of if(intersect)

		} // end of for(j....
	}  // end of  for(  i= 0; i< nTotalCandidates; i++)

//------------------

    for(  i= 0, k=-1; i< nTotalCandidates; i++){


	//-------stampe.
if(istampa>=2){
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
			InfoXYZParal (
				info,
				ListTrackCandHit[i][npunti],
				Ox[i],
				Oy[i],
				R[i],
				KAPPA[i],
				FI0[i],
				Charge[i],
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
	if(Charge[i]>0.){
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


if(istampa>1) cout<<"PndTracking, evt. "<<IVOLTE<<
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
		   Charge[i],
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

quifinisce     */
return;

}


//---------------------- end of   PndTracking::Exec


//----------begin of function PndTracking::AcceptHitsConformal

bool  PndTracking::AcceptHitsConformal(
	Double_t  distance,
	Double_t  DriftConfR, //drift radius in conformal space
	Double_t  StrawConfR  // straw radius in conformal space
		)
{
  if( fabs(distance-DriftConfR) < 2.*StrawConfR ) return true;
  return false;


}

//----------end of function PndTracking::AcceptHitsConformal





//----------begin of function PndTracking::AssociateBetterAfterFitSkewHitsToXYTrack

  UShort_t PndTracking::AssociateBetterAfterFitSkewHitsToXYTrack(
                   UShort_t TemporarynSttSkewhitinTrack,  //  input
                   UShort_t SkewList[][2], // input,  list of selected skew hits (in skew numbering)
                   Double_t *S,       //  input,  S coordinate of selected Skew hit
                   Double_t *Z,       //  input,  Z coordinate of selected Skew hit
                   Double_t *ZDrift,  //  input,  drift distance IN Z DIRECTION only, of selected Skew hit
                   Double_t *ZErrorafterTilt,   //  input,  Radius taking into account the tilt, IN Z DIRECTION only, of selected Skew hit
                   Double_t KAPPA,    // input, KAPPA result of fit
                   Double_t FI0,    // input, FI0 result of fit
                   UShort_t *tempore,  //  output result, associated skew hits
                   Double_t *temporeS,  //  output, associated skew hit  S
                   Double_t *temporeZ,  //  output, associated skew hits Z
                   Double_t *temporeZDrift,  //  output, associated skew hit Z drift
                   Double_t *temporeZErrorafterTilt,  //  output, associated skew hits Z error after tilt
                   Short_t  *STATUS   // output
                                                     )
 {



    UShort_t NAssociated;
    Short_t  sign;
    Int_t i, j, i1, ii, iii,  Kincl, nlow, nup;

    Double_t bbb,
             tempZ[2],
             zmin, zmax, deltaz,
		zdist[2],
		zdist1,
		zdist2;

    Double_t allowed_distance = 4.*STRAWRADIUS/sin(STRAW_SKEW_INCLINATION_DEGREES*PI/180.);


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

//       if(  zdist < 4.*ZErrorafterTilt[i] ){

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

//----------end of function PndTracking::AssociateBetterAfterFitSkewHitsToXYTrack



//----------begin of function PndTracking::AssociateFoundTrackstoMC

void PndTracking::AssociateFoundTrackstoMC(
	Double_t info[][7],
	UShort_t nTracksFoundSoFar,
	UShort_t *nHitsinTrack,
	UShort_t  ListHitsinTrack[][MAXSTTHITS],
	UShort_t *nSkewHitsinTrack,
	UShort_t  ListSkewHitsinTrack[][MAXSTTHITS],
	Short_t *daTrackFoundaTrackMC
	)
{

 bool
	flaggo,
	inclusionMC[MAXTRACKSPEREVENT][MAXSTTHITS],
	inclusionExp[MAXTRACKSPEREVENT];

 UShort_t	ntoMCtrack[MAXTRACKSPEREVENT],
		toMCtracklist[MAXTRACKSPEREVENT][MAXSTTHITS],
		toMCtrackfrequency[MAXTRACKSPEREVENT][MAXSTTHITS];

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
		flaggo=true;
		for(j=0; j<ntoMCtrack[jexp]; j++){
			if( enne == toMCtracklist[jexp][j] ) {
				toMCtrackfrequency[jexp][j]++;
				flaggo=false;
				break;
			}
		}
		if(flaggo){
			toMCtracklist[jexp][ ntoMCtrack[jexp] ] = enne;
			toMCtrackfrequency[jexp][ ntoMCtrack[jexp] ] = 1;
			ntoMCtrack[jexp]++;
		}
	}   //  end of for(i=0; i<nHitsinTrack[jexp]; i++)



// poi  gli hits skew ---------------------
	for(i=0; i<nSkewHitsinTrack[jexp]; i++){
		enne = (Int_t)( info[ ListSkewHitsinTrack[jexp][i]  ][6]+0.01 );
		flaggo=true;
		for(j=0; j<ntoMCtrack[jexp]; j++){
			if( enne == toMCtracklist[jexp][j] ) {
				toMCtrackfrequency[jexp][j]++;
				flaggo=false;
				break;
			}
		}
		if(flaggo){
			toMCtracklist[jexp][ ntoMCtrack[jexp] ] = enne;
			toMCtrackfrequency[jexp][ ntoMCtrack[jexp] ] = 1;
			ntoMCtrack[jexp]++;
		}
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


//----------end of function PndTracking::AssociateFoundTrackstoMC



//----------begin of function PndTracking::AssociateFoundTrackstoMCbis

void PndTracking::AssociateFoundTrackstoMCbis(
	Double_t info[][7],
	UShort_t nTracksFoundSoFar,
	UShort_t nHitsinTrack[MAXTRACKSPEREVENT],
	UShort_t  ListHitsinTrack[MAXTRACKSPEREVENT][MAXSTTHITS],
	UShort_t nSkewHitsinTrack[MAXTRACKSPEREVENT],
	UShort_t  ListSkewHitsinTrack[MAXTRACKSPEREVENT][MAXSTTHITS],
	Short_t daTrackFoundaTrackMC[MAXTRACKSPEREVENT]
	)
{

 bool
	flaggo,	
	inclusionMC[MAXTRACKSPEREVENT][MAXSTTHITS],
	inclusionExp[MAXTRACKSPEREVENT];

   UShort_t	ntoMCtrack[MAXTRACKSPEREVENT],
		toMCtracklist[MAXTRACKSPEREVENT][MAXSTTHITS],
		toMCtrackfrequency[MAXTRACKSPEREVENT][MAXSTTHITS];

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
		flaggo=true;
		for(j=0; j<ntoMCtrack[jexp]; j++){
			if( enne == toMCtracklist[jexp][j] ) {
				toMCtrackfrequency[jexp][j]++;
				flaggo=false;
				break;
			}
		}
		if(flaggo){
			toMCtracklist[jexp][ ntoMCtrack[jexp] ] = enne;
			toMCtrackfrequency[jexp][ ntoMCtrack[jexp] ] = 1;
			ntoMCtrack[jexp]++;
		}
	}   //  end of for(i=0; i<nHitsinTrack[jexp]; i++)



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




//----------end of function PndTracking::AssociateFoundTrackstoMCbis



//----------begin of function PndTracking::AssociateFoundTrackstoMCtris

    void PndTracking::AssociateFoundTrackstoMCtris(
		Double_t info[][7],
		Double_t Ox[MAXTRACKSPEREVENT],
		Double_t Oy[MAXTRACKSPEREVENT],
		Double_t R[MAXTRACKSPEREVENT],
                   UShort_t nTracksFoundSoFar,
                  UShort_t nHitsinTrack[MAXTRACKSPEREVENT],
                  UShort_t  ListHitsinTrack[MAXTRACKSPEREVENT][MAXSTTHITS],
                  UShort_t nSkewHitsinTrack[MAXTRACKSPEREVENT],
                  UShort_t  ListSkewHitsinTrack[MAXTRACKSPEREVENT][MAXSTTHITS],
                  Short_t daTrackFoundaTrackMC[MAXTRACKSPEREVENT]
                                                        )
{

 bool
	firstime,
	flaggo,
	inclusionMC[MAXTRACKSPEREVENT][MAXSTTHITS],
		inclusionExp[MAXTRACKSPEREVENT];

   UShort_t	ntoMCtrack[MAXTRACKSPEREVENT],
		toMCtrackfrequency[MAXTRACKSPEREVENT][MAXSTTHITS];

   UShort_t  i, j, jtemp,jexp , nmid;

   Short_t   itemp, massimo,
		toMCtracklist[MAXTRACKSPEREVENT][MAXSTTHITS];

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
		 toMCtrackdistance[MAXTRACKSPEREVENT][MAXSTTHITS];


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

			flaggo=true;
			for(j=0; j<ntoMCtrack[jexp]; j++){
				if( enne == toMCtracklist[jexp][j] ) {
					toMCtrackfrequency[jexp][j]++;
					flaggo=false;
					break;
				}
			}
			if(flaggo){
				toMCtracklist[jexp][ ntoMCtrack[jexp] ] = enne;
				toMCtrackfrequency[jexp][ ntoMCtrack[jexp] ] = 1;
				getMCInfo( enne, &Cx, &Cy, &Rr);
				if( Rr<0.) {
					toMCtrackdistance[jexp][ntoMCtrack[jexp]]=-1.;
				} else {
					alfa = -2.*Cx;
					beta = -2.*Cy;
					gamma = Cx*Cx+Cy*Cy-Rr*Rr;
					toMCtrackdistance[jexp][ntoMCtrack[jexp]]=
						FindDistance(Ox[jexp],Oy[jexp],R[jexp],
						tanlow[jexp],tanmid[jexp],tanup[jexp],
						alfa,beta,gamma);
				}
				ntoMCtrack[jexp]++;
			}  // end of if(flaggo)
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




//----------end of function PndTracking::AssociateFoundTrackstoMCtris




//----------begin of function PndTracking::AssociateFoundTrackstoMCquater

    void PndTracking::AssociateFoundTrackstoMCquater(
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
UShort_t  ListHitsinTrack[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
                  UShort_t nSkewHitsinTrack[MAXTRACKSPEREVENT],
UShort_t  ListSkewHitsinTrack[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],

		UShort_t nPixelHitsinTrack[MAXTRACKSPEREVENT],
UShort_t ListPixelHitsinTrack[MAXTRACKSPEREVENT][MAXMVDPIXELHITSINTRACK],
		Int_t *FromPixeltoMCTrack,
		UShort_t nStripHitsinTrack[MAXTRACKSPEREVENT],
UShort_t ListStripHitsinTrack[MAXTRACKSPEREVENT][MAXMVDSTRIPHITSINTRACK],
		Int_t *FromStriptoMCTrack,

		Short_t daTrackFoundaTrackMC[MAXTRACKSPEREVENT]
						)
{

 bool
	firstime,
	flaggo,
 inclusionMC[nTracksFoundSoFar][MAXSTTHITSINTRACK+MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK],
		inclusionExp[nTracksFoundSoFar];

   UShort_t	ntoMCtrack[nTracksFoundSoFar],
		toMCtrackfrequency[nTracksFoundSoFar][MAXSTTHITSINTRACK];

   UShort_t  i, j, jtemp,jexp , nmid;

   Short_t  itemp, massimo,
		toMCtracklist[nTracksFoundSoFar][MAXSTTHITSINTRACK];

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
		 toMCtrackdistance[nTracksFoundSoFar][MAXSTTHITSINTRACK];




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

			flaggo=true;
			for(j=0; j<ntoMCtrack[jexp]; j++){
				if( enne == toMCtracklist[jexp][j] ) {
					toMCtrackfrequency[jexp][j]++;
					flaggo=false;
					break;
				}
			}
			if(flaggo){
				toMCtracklist[jexp][ ntoMCtrack[jexp] ] = enne;
				toMCtrackfrequency[jexp][ ntoMCtrack[jexp] ] = 1;
				getMCInfo( enne, &Cx, &Cy, &Rr);
				if( Rr<0.) {
					toMCtrackdistance[jexp][ntoMCtrack[jexp]]=-1.;
				} else {
					alfa = -2.*Cx;
					beta = -2.*Cy;
					gamma = Cx*Cx+Cy*Cy-Rr*Rr;
					toMCtrackdistance[jexp][ntoMCtrack[jexp]]=
						FindDistance(Ox[jexp],Oy[jexp],R[jexp],
						tanlow[jexp],tanmid[jexp],tanup[jexp],
						alfa,beta,gamma);
				}
				ntoMCtrack[jexp]++;
			}  // end of if(flaggo)
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

			flaggo=true;
			for(j=0; j<ntoMCtrack[jexp]; j++){
				if( enne == toMCtracklist[jexp][j] ) {
					toMCtrackfrequency[jexp][j]++;
					flaggo=false;
					break;
				}
			}
			if(flaggo){
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
			}  // end of if(flaggo)
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

			flaggo=true;
			for(j=0; j<ntoMCtrack[jexp]; j++){
				if( enne == toMCtracklist[jexp][j] ) {
					toMCtrackfrequency[jexp][j]++;
					flaggo=false;
					break;
				}
			}
			if(flaggo){
				toMCtracklist[jexp][ ntoMCtrack[jexp] ] = enne;
				toMCtrackfrequency[jexp][ ntoMCtrack[jexp] ] = 1;
				getMCInfo( enne, &Cx, &Cy, &Rr);
				if( Rr<0.) {
					toMCtrackdistance[jexp][ntoMCtrack[jexp]]=-1.;
				} else {
					alfa = -2.*Cx;
					beta = -2.*Cy;
					gamma = Cx*Cx+Cy*Cy-Rr*Rr;
					toMCtrackdistance[jexp][ntoMCtrack[jexp]]=
						FindDistance(Ox[jexp],Oy[jexp],R[jexp],
						tanlow[jexp],tanmid[jexp],tanup[jexp],
						alfa,beta,gamma);
				}
				ntoMCtrack[jexp]++;
			} // end of if(flaggo)
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




//----------end of function PndTracking::AssociateFoundTrackstoMCquater



//----------begin of function PndTracking::AssociateSciTilHit

UShort_t PndTracking::AssociateSciTilHit(
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
		if( igoodScit == MAXSCITILHITSINTRACK) break;

	} // end of  if(intersect && distance<olddist)
 }  // end of  for(iScitHit=0; iScitHit<nScitHits; iScitHit++)

 return igoodScit;

}

//----------end of function PndSttTrackFinderReal::AssociateSciTilHit


//----------begin of function PndTracking::AssociateSkewHitsToXYTrack

UShort_t PndTracking::AssociateSkewHitsToXYTrack(
	bool *InclusionListSkew,
	UShort_t NSkewhits,
	UShort_t *infoskew,  // from skew numbering to original Stt hit numbering;
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
	UShort_t SkewList[][2], // output,list of selected skew hits (original numbering)
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

//----------end of function PndTracking::AssociateSkewHitsToXYTrack


//----------begin of function PndTracking::BadTrack_ParStt

bool PndTracking::BadTrack_ParStt(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Short_t Charge,
	Double_t Xcross[2],  // Xcross[0]=point of entrance;
				//  Xcross[1]=point of exit.
	Double_t Ycross[2],
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
			Distance[MAXSTTHITS+1];

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
	if( ninside < ((int) length/DIAMETERSTRAWTUBE)-islack ){
		if(istampa>1){
			cout<<"in BadTrack_ParStt, n. Hits inside = "<<ninside
			<<" is < n. hits that should be inside at least = "
			<<((int) length/DIAMETERSTRAWTUBE)<<"-islack ("<<
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


//----------end of function PndTracking::BadTrack_ParStt


//----------begin of function PndTracking::BoxConformalFilling

void PndTracking::BoxConformalFilling(
		Double_t infoparalConformal[][5],
		UInt_t Nparal,
		UShort_t nBoxConformal[][NFIDIVCONFORMAL],
		UShort_t HitsinBoxConformal[][NRDIVCONFORMAL][NFIDIVCONFORMAL],
		UShort_t  *RConformalIndex,
		UShort_t  *FiConformalIndex
						)
{

 Short_t
	i,
	iFi,
	iR,
	j;

 Double_t Fi;



 for(i = 0; i< nRdivConformalEffective ; i++){
	for(j = 0; j< NFIDIVCONFORMAL ; j++){
		nBoxConformal[i][j]= 0;
	}
 }

 for(i = 0; i< Nparal ; i++){
	if( ! InclusionListStt[ ListSttParHits[i] ] ) continue;
	Fi = atan2(infoparalConformal[ListSttParHits[i]][1],
		infoparalConformal[ListSttParHits[i]][0]) ;
	if ( Fi < 0. ) Fi += 2.*PI;
	iFi =  (Short_t) (0.5*NFIDIVCONFORMAL*Fi/PI);
	if(iFi > NFIDIVCONFORMAL ) {
		iFi = NFIDIVCONFORMAL;
	} else if (iFi<0) {
		iFi = 0;
	}


	Double_t RRR = sqrt(infoparalConformal[ListSttParHits[i]][0]*
			infoparalConformal[ListSttParHits[i]][0]+
			infoparalConformal[ListSttParHits[i]][1]*
			infoparalConformal[ListSttParHits[i]][1]);

	for(j=nRdivConformalEffective-1, iR=0; j>0; j--){
		if( RRR> radiaConf[j] ){
			iR = j;
			break;
		}
	}
	if( nBoxConformal[iR][iFi] >= MAXHITSINCELL ){
		cout<<"Warning from PndTracking::BoxConformalFilling\t:"
		<<"\n\tcontent in nBoxConformal["<<iR<<"]["<<
		iFi<<"] has reached the Max allowed value = "
		<<MAXHITSINCELL<<endl;
		 continue;
	}
	HitsinBoxConformal[ nBoxConformal[iR][iFi] ][iR][iFi]
		=(UShort_t) ListSttParHits[i];
	nBoxConformal[iR][iFi]++;
	RConformalIndex[ ListSttParHits[i] ]  =  iR;
	FiConformalIndex[ ListSttParHits[i] ]  =  iFi;
 }  // end of for(i = 0; i< Nparal ; i++)



 return;

}
//----------end of function PndTracking::BoxConformalFilling



//----------begin of function PndTracking::CalculateArcLength

Double_t PndTracking::CalculateArcLength(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Short_t charge,
	Double_t *Xcross, // entrance-exit point
	Double_t *Ycross // entrance-exit point
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
//----------end of function PndTracking::CalculateArcLength


//----------begin of function PndTracking::CalculateCircleThru3Points

bool PndTracking::CalculateCircleThru3Points(
	Double_t x1,
	Double_t y1,
	Double_t x2,
	Double_t y2,
	Double_t x3,
	Double_t y3,
	Double_t *Ox,
	Double_t *Oy,
	Double_t *R
	)
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
//----------end of function PndTracking::CalculateCircleThru3Points





//-------------------  begin of function   PndTracking::calculateintersections

void PndTracking::calculateintersections(
	Double_t Ox,
	Double_t Oy,
	Double_t R,
	Double_t C0x,
	Double_t C0y,
	Double_t C0z,
	Double_t r,
	Double_t vx,
	Double_t vy,
	Double_t vz,
	Int_t *STATUS,
	Double_t* POINTS)
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



//----------end of function PndTracking::calculateintersections



//------------------------- begin of function  PndTracking::CalculateSandZ

void PndTracking::CalculateSandZ(
	Double_t Ox,
	Double_t Oy,
	Double_t R,
	Short_t skewnum,
	Double_t info[][7],
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

//-------------------------  end of function  PndTracking::CalculateSandZ



//----------star of function PndTracking::ChooseEntranceExitbis
void PndTracking::ChooseEntranceExitbis(
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
//----------end of function PndTracking::ChooseEntranceExitbis


//------------------------- begin of function  PndTracking::CollectParSttHitsagain

 void PndTracking::CollectParSttHitsagain(
	bool *keepit,
	bool *Mvdhits,
	Double_t info[][7],
	UShort_t nSttParHit,
	// starting investigation from candidate n. StartTrackCand
	UShort_t StartTrackCand,
	//  until candidate n. EndTrackCand-1 included.
	UShort_t EndTrackCand,
	Double_t *Ox,
	Double_t *Oy,
	Double_t *R,
	Double_t *KAPPA,
	Double_t *FI0,
	Double_t *Fi_low_limit,
	Double_t *Fi_up_limit,
	UShort_t *nParHitsinTrack, // input/output
	UShort_t ListParHitsinTrack[][MAXSTTHITSINTRACK] // input/output
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
			ihit = ListSttParHits[i];


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

//------------------------- end of function  PndTracking::CollectParSttHitsagain


//-------------------------  begin of function  PndTracking::Dist_SZ

Double_t PndTracking::Dist_SZ(
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
	dis2 = dis_segments-dis1; if(dis2<0.)dis2=0.;


	if( dis1 < dis2 )
	{
		return dis1;
	} else {
		return dis2;
	}

}

//-------------------------  end of function  PndTracking::Dist_SZ



//----------end of function PndTracking::disegnaassiXY
void PndTracking::disegnaAssiXY(
	FILE * MACRO,
	double xmin,
	double xmax,
	double ymin,
	double ymax
	)
{

       fprintf(MACRO,"TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n",xmin,0.,xmax,0.,xmin,xmax);
       fprintf(MACRO,"Assex->SetTitle(\"X\");\n");
       fprintf(MACRO,"Assex->SetTitleOffset(1.5);\n");
       fprintf(MACRO,"Assex->Draw();\n");
       fprintf(MACRO,"TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", 0.,ymin,0.,ymax,ymin,ymax);
       fprintf(MACRO,"Assey->SetTitle(\"Y\");\n");
       fprintf(MACRO,"Assey->SetTitleOffset(1.5);\n");
       fprintf(MACRO,"Assey->Draw();\n");

}


//----------end of function PndTracking::disegnaassiXY



//----------begin of function PndTracking::disegnaSciTilHit

void PndTracking::disegnaSciTilHit(
	FILE * MACRO,
	int ScitilHit,
	double posx,
	double posy,
	int tipo
	)
{
	double	x1,x2,y1,y2,L,R, RR;


	L=DIMENSIONSCITIL/2.;


   if(tipo==0){	// SciTil disegnate in XY.
	R = sqrt(posx*posx+posy*posy);
	x1 = posx + posy*L/R;
	x2 = posx - posy*L/R;
	y1 = posy - posx*L/R;
	y2 = posy + posx*L/R;
   } else if (tipo==1) {// SciTil disegnate in SZ.
	x1 = posx - L;
	x2 = posx + L;
	y1 = posy;
	y2 = posy;
   } else {  // SciTil disegnate in UV.
	RR = posx*posx+posy*posy;
	R = sqrt(RR);
	x1 = posx + posy*L/R;
	x1 /= RR;
	x2 = posx - posy*L/R;
	x2 /= RR;
	y1 = posy - posx*L/R;
	y1 /= RR;
	y2 = posy + posx*L/R;
	y2 /= RR;

   }
	fprintf(MACRO,"TLine *Tile%d = new TLine(%f,%f,%f,%f);\n",ScitilHit,x1,y1,x2,y2);
	fprintf(MACRO,"Tile%d->SetLineColor(1);\n",ScitilHit);


	if(tipo==0){
		// disegna in XY.
		fprintf(MACRO,"Tile%d->SetLineWidth(2);\n",ScitilHit);
	} else if (tipo==1) {
		// disegna in SZ.
		fprintf(MACRO,"Tile%d->SetLineWidth(3);\n",ScitilHit);
	} else {
		// disegna in UV.
		fprintf(MACRO,"Tile%d->SetLineWidth(3);\n",ScitilHit);
	}


	fprintf(MACRO,"Tile%d->Draw();\n",ScitilHit);
/*
	fprintf(MACRO,"TMarker* SciT%d = new TMarker(%f,%f,%d);\n",
			ScitilHit,posx,posy,30);
	fprintf(MACRO,"SciT%d->SetMarkerSize(1.5);\n",ScitilHit);
	fprintf(MACRO,"SciT%d->SetMarkerColor(1);\nSciT%d->Draw();\n"
				,ScitilHit,ScitilHit);
*/
}

//----------end of function PndTracking::disegnaSciTilHit



//----------begin of function PndTracking::DrawBiHexagonInMacro

void PndTracking::DrawBiHexagonInMacro(
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
//----------end of function PndTracking::DrawBiHexagonInMacro


//----------begin of function PndTracking::DrawHexagonCircleInMacro

void PndTracking::DrawHexagonCircleInMacro(
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
//----------end of function PndTracking::DrawHexagonCircleInMacro



//-------------------------  begin of function  PndTracking::EliminateSpuriousSZ

void PndTracking::EliminateSpuriousSZ(
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
//-------------------------  end of function  PndTracking::EliminateSpuriousSZ


//----------------  begin of function PndTracking::ExtractInfoFromMvdTrackCand

void PndTracking::ExtractInfoFromMvdTrackCand()
{

  UShort_t
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
		cout<<"from PndTracking, nHitMvdTrackCand[i] = "<<nHitMvdTrackCand[i]
		<<" and it is > MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK (="
		<<MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK
		<<"); setting nHitMvdTrackCand[i] to "
		<<MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK<<endl;
	nHitMvdTrackCand[i]=MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK;
	}

	UShort_t kPixel,kStrip;

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
			cout<<"from PndTracking, this Pixel Mvd hit has a number = "
			<<pndtrackcandhit.GetHitId()<<
			" that is > MAXMVDPIXELHITS (="<<
				MAXMVDPIXELHITS<<"), rejected!\n";
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
			cout<<"from PndTracking, this Strip Mvd hit has a number = "
			<<pndtrackcandhit.GetHitId()<<
			" that is > MAXMVDSTRIPHITS (="<<
			MAXMVDSTRIPHITS<<"), rejected!\n";
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

 if(istampa>=2) stampaMvdTrackCandInfo();

 }
//-----------------------  end of function   PndTracking::ExtractInfoFromMvdTrackCand



//----------begin of function PndTracking::FindCharge

void PndTracking::FindCharge(
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

//----------end of function PndTracking::FindCharge




//------------------------- begin of function  PndTracking::FindDistance

Double_t PndTracking::FindDistance(
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



//------------------------- end of function  PndTracking::FindDistance




//----------start  function PndTracking::FindingParallelTrackAngularRange

void  PndTracking::FindingParallelTrackAngularRange(
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


//---------- end of  function PndTracking::FindingParallelTrackAngularRange



//----------begin of function PndTracking::FindIntersectionsOuterCircle

Short_t PndTracking::FindIntersectionsOuterCircle(
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
//----------end of function PndTracking::FindIntersectionsOuterCircle


//----------begin of function PndTracking::FindTrackEntranceExitbiHexagonLeft

Short_t PndTracking::FindTrackEntranceExitbiHexagonLeft(
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

//----------end of function PndTracking::FindTrackEntranceExitbiHexagonLeft




//----------begin of function PndTracking::FindTrackEntranceExitbiHexagonRight

Short_t PndTracking::FindTrackEntranceExitbiHexagonRight(
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


//----------end of function PndTracking::FindTrackEntranceExitbiHexagonRight




//----------begin function PndTracking::FindTrackEntranceExitHexagonCircleLeft

Short_t PndTracking::FindTrackEntranceExitHexagonCircleLeft(
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

//----------end of function PndTracking::FindTrackEntranceExitHexagonCircleLeft



//----------begin of function PndTracking::FindTrackEntranceExitHexagonCircleRight

Short_t PndTracking::FindTrackEntranceExitHexagonCircleRight(
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

//----------end of function PndTracking::FindTrackEntranceExitHexagonCircleRight



//----------begin of function PndTracking::FindTrackStrictCollection

Short_t PndTracking::FindTrackStrictCollection(
	UShort_t NFiCELLDISTANCE,
	//  seed track (original notation) as far as the Fi angle is concerned
	UShort_t iSeed,
	//  n. of hits to search in ListHitsinTrackinWhichToSearch
	UShort_t NParallelToSearch,
	UShort_t *ListHitsinTrackinWhichToSearch,
	bool *InclusionList,
	UShort_t *FiConformalIndex,
	UShort_t  *OutputListHitsinTrack
	)
{

 UShort_t
	i,
	iFi,
	iFiseed,
	iR,
	j,
	nHitsinTrack;


 Double_t auxRvalues[MAXSTTHITS];

//   iSeed        is the hit number in the ORIGINAL number scheme

 iFiseed = FiConformalIndex[iSeed];

 nHitsinTrack=0;
 for(i=0; i<NParallelToSearch; i++){
	//  Inclusion of the parallel hit straws already used in other tracks
     if( InclusionList[ListHitsinTrackinWhichToSearch[i]]) {
	//  the index of InclusionList is in the ORIGINAL scheme of hits

	iFi = FiConformalIndex[ListHitsinTrackinWhichToSearch[i]];
	if( iFi == iFiseed ) {
	   OutputListHitsinTrack[nHitsinTrack]=ListHitsinTrackinWhichToSearch[i];
	   nHitsinTrack++;
	} else if ( iFi < iFiseed ) {
	   if(iFiseed - iFi <= NFiCELLDISTANCE ){
		OutputListHitsinTrack[nHitsinTrack]=ListHitsinTrackinWhichToSearch[i];
		nHitsinTrack++;
	   } else {
		if(iFi+NFIDIVCONFORMAL-iFiseed<=NFiCELLDISTANCE) {
		  OutputListHitsinTrack[nHitsinTrack]=ListHitsinTrackinWhichToSearch[i];
		  nHitsinTrack++;
		}
	   } // end of  if(iFiseed - iFi <= NFiCELLDISTANCE)
	}else{   //  continuation of  if( iFi == iFiseed )
	   if( -iFiseed + iFi <= NFiCELLDISTANCE ) {
		OutputListHitsinTrack[nHitsinTrack]=ListHitsinTrackinWhichToSearch[i];
		nHitsinTrack++;
	   }else{
		if( -iFi + NFIDIVCONFORMAL + iFiseed<= NFiCELLDISTANCE ) {
		  OutputListHitsinTrack[nHitsinTrack]=ListHitsinTrackinWhichToSearch[i];
		  nHitsinTrack++;
		}
	   }  // end of  if( -iFiseed + iFi <= NFiCELLDISTANCE )

	}  //  end of   if( iFi == iFiseed )

        }    //  end of    if( InclusionList[ListHitsinTrackinWhichToSearch[i]])

     }   //  end of        for(i=0; i<NparallelToSearch; i++)



 return nHitsinTrack;

}


//----------end of function PndTracking::FindTrackStrictCollection




//----------begin of function PndTracking::FindTrackInXYProjection

bool PndTracking::FindTrackInXYProjection(
	Short_t iHit,    // seed hit; if it is negative it is a SciTil hit.
	Short_t nRcell,  // R cell of the seed hit, it is -1 for a SciTil hit;
	UShort_t nFicell, // Fi cell of the seed hit;
	UInt_t nsttparhit,
	Double_t info[][7],
	bool *InclusionList,
	UShort_t *RConformalIndex,
	UShort_t *FiConformalIndex,
	UShort_t nBoxConformal[][NFIDIVCONFORMAL],
	UShort_t HitsinBoxConformal[][NRDIVCONFORMAL][NFIDIVCONFORMAL],
	UShort_t nTracksFoundSoFar,
	UShort_t *nHitsinTrack,
	UShort_t ListHitsinTrack[][MAXSTTHITSINTRACK],
	Double_t *trajectory_vertex,
	Double_t infoparalConformal[][5],
	Double_t posizSciTilx,
	Double_t posizSciTily,
	Double_t *S,
	Double_t *Ox,
	Double_t *Oy,
	Double_t *R,
	Double_t *Fi_low_limit,
	Double_t *Fi_up_limit,
	Double_t *Fi_initial_helix_referenceframe,
	Double_t *Fi_final_helix_referenceframe,
	Short_t * Charge,
	Double_t *U,
	Double_t *V
	)
{


//---------------

 UShort_t
	i,
	j,
	Naux,
	Nbaux,
	NN,
	Nouter,
	auxListHitsinTrack[MAXSTTHITS],
	OutputListHitsinTrack[MAXSTTHITS],
	OutputList2HitsinTrack[MAXSTTHITS],
	ListHitsinTrackinWhichToSearch[MAXSTTHITS];

 Short_t
	flagStt,
	status;


 Double_t
	aaa,
	m,
	q,
	rotationangle,
	rotationcos,
	rotationsin;


//----------------
 nHitsinTrack[nTracksFoundSoFar] = FindTrackPatterninBoxConformal(
	1,   //  distance in R cells allowed
	2,   //  distance in Fi cells allowed
	nsttparhit,
	iHit, // seed hit; if it is negative it is a SciTil hit.
	nRcell,
	nFicell,
	info,
	InclusionList,
	RConformalIndex,
	FiConformalIndex,
	nBoxConformal,
	HitsinBoxConformal,
	&ListHitsinTrack[nTracksFoundSoFar][0]
			);
//---------stampe.
if(istampa>0){
cout<<"PndSttTracking, evt. "<<IVOLTE<<
", dopo FindTrackPatterninBoxConformal; nTracksFoundSoFar  "
   <<nTracksFoundSoFar<<", nHitsinTrack "<<
   nHitsinTrack[nTracksFoundSoFar]<<", ;loro lista :\n";
for(int iz=0;iz<nHitsinTrack[nTracksFoundSoFar];iz++){
	cout<<"\thit n. (original numbering) "
	<<ListHitsinTrack[nTracksFoundSoFar][iz]
	<<endl;
}
}
//------------------------------fine stampe.

 if( nHitsinTrack[nTracksFoundSoFar] < MINIMUMHITSPERTRACK ||
	nHitsinTrack[nTracksFoundSoFar]>MAXSTTHITSINTRACK) {
	return false;
 }
//-----------------------

//   find among the ListHitsinTrack  if there are at least
//   a minimum # of hits belonging to the outer part of the STT  system.
//   At this point of the code the Stt hits are already ordered from
//   the outermost to the innermost.

 for(j=0, Nouter =0; j<nHitsinTrack[nTracksFoundSoFar]; j++){
  if(info[ ListHitsinTrack[nTracksFoundSoFar][j] ][0]*
	info[ ListHitsinTrack[nTracksFoundSoFar][j] ][0]+
	info[ ListHitsinTrack[nTracksFoundSoFar][j] ][1]*
	info[ ListHitsinTrack[nTracksFoundSoFar][j] ][1]
	< APOTEMAMAXSKEWSTRAW*APOTEMAMAXSKEWSTRAW  ) break;
  Nouter++;
 }

 if( Nouter >= MINOUTERHITSPERTRACK) {
	for(i=0; i< Nouter;i++){
	  ListHitsinTrackinWhichToSearch[i]=
		ListHitsinTrack[nTracksFoundSoFar][i];
	}

	for(i=0; i< Nouter;i++){
	 Naux =  FindTrackPatterninBoxConformalSpecial(
		3,    // NRCELLDISTANCE
		1,    // NFiCELLDISTANCE
		nsttparhit,
		Nouter,
		ListHitsinTrackinWhichToSearch[i], // seed hit.
		ListHitsinTrackinWhichToSearch,
		info,
		InclusionList,
		RConformalIndex,
		FiConformalIndex,
		nBoxConformal,
		HitsinBoxConformal,
		OutputListHitsinTrack);
	 if( Naux >= MINOUTERHITSPERTRACK && Naux > 0.7 * Nouter )  break;
		 if( Naux >= MINOUTERHITSPERTRACK) {

//   further collection of hits in the inner region but this time strictly connected
//   to the outer ones

//  first the list of non outer hits
	   for(j=Nouter;j<nHitsinTrack[nTracksFoundSoFar]; j++){
		ListHitsinTrackinWhichToSearch[j-Nouter] =
			ListHitsinTrack[nTracksFoundSoFar][j];
	   }

	   Nbaux =  FindTrackStrictCollection(
		1,    // NFiCELLDISTANCE
		ListHitsinTrackinWhichToSearch[i],   //  seed hit
		nHitsinTrack[nTracksFoundSoFar]-Nouter,
		ListHitsinTrackinWhichToSearch,
		InclusionList,
		FiConformalIndex,
		OutputList2HitsinTrack
					);
//   add the new hits found to the list

	   nHitsinTrack[nTracksFoundSoFar]=Naux+Nbaux;
	   if( nHitsinTrack[nTracksFoundSoFar] >= MINIMUMHITSPERTRACK &&
		nHitsinTrack[nTracksFoundSoFar]<=MAXSTTHITSINTRACK) {
		for(j=0;j<Naux;j++){
			ListHitsinTrack[nTracksFoundSoFar][j] =
				OutputListHitsinTrack[j];
		}
		for(j=0;j<Nbaux;j++){
			ListHitsinTrack[nTracksFoundSoFar][Naux+j] =
				OutputList2HitsinTrack[j];
		}
		break;

	   }// end of  if( nHitsinTrack[nTracksFoundSoFar] >= ....
	 }  // end of  if( Naux >= MINOUTERHITSPERTRACK)
	}   // end of for(i=0; i< Nouter;i++)
 }    // end of if( Nouter >= MINOUTERHITSPERTRACK)



 if( nHitsinTrack[nTracksFoundSoFar] < MINIMUMHITSPERTRACK ||
  nHitsinTrack[nTracksFoundSoFar]>MAXSTTHITSINTRACK) {
	return false;
 }

//---------------------------

//  finding the rotation angle for best utilization of the MILP procedure

 for(j=0, rotationcos=0., rotationsin=0.; j<nHitsinTrack[nTracksFoundSoFar]; j++){
	rotationcos += cos((0.5+
	FiConformalIndex[ListHitsinTrack[nTracksFoundSoFar][j]])
	*2.*PI/NFIDIVCONFORMAL) ;

	rotationsin += sin((0.5+
	FiConformalIndex[ListHitsinTrack[nTracksFoundSoFar][j]])
	*2.*PI/NFIDIVCONFORMAL) ;
 }
 rotationcos /=nHitsinTrack[nTracksFoundSoFar];
 rotationsin /=nHitsinTrack[nTracksFoundSoFar];
 rotationangle = atan2(rotationsin, rotationcos);

//  fitting with superfast MILP code
 bool Type;
 int	nFitPoints,
	offset;
 Double_t
	Xconformal[1+nHitsinTrack[nTracksFoundSoFar]],
	Yconformal[1+nHitsinTrack[nTracksFoundSoFar]],
	DriftRadiusconformal[1+nHitsinTrack[nTracksFoundSoFar]],
	ErrorDriftRadiusconformal[1+nHitsinTrack[nTracksFoundSoFar]];

 if(iHit<0){	// case with a hit in the SciTil
  aaa = posizSciTilx*posizSciTilx+posizSciTily*posizSciTily;
  Xconformal[0] =posizSciTilx/aaa;
  Yconformal[0] =posizSciTily/aaa;
  ErrorDriftRadiusconformal[0] = DIMENSIONSCITIL/aaa;
  DriftRadiusconformal[0]=-1.;  // treat it like it is a Mvd hit.

 // +1 comes from one SciTil hit.
   offset=1;
   nFitPoints = nHitsinTrack[nTracksFoundSoFar]+1;

 } else {	// no SciTil hit.
   offset=0;
   nFitPoints = nHitsinTrack[nTracksFoundSoFar];
 }  // end of  if(iHit<0)

  for(j=0; j<nHitsinTrack[nTracksFoundSoFar]; j++){
    Xconformal[j+offset] =infoparalConformal[ListHitsinTrack[nTracksFoundSoFar][j]][0];
    Yconformal[j+offset] =infoparalConformal[ListHitsinTrack[nTracksFoundSoFar][j]][1];
    ErrorDriftRadiusconformal[j+offset]=
		infoparalConformal[ListHitsinTrack[nTracksFoundSoFar][j]][2];
    DriftRadiusconformal[j+offset]=
		infoparalConformal[ListHitsinTrack[nTracksFoundSoFar][j]][2];
  }




//------------------------
if(istampa>0){

cout<<"PndSttTrackFinderReal, in FindTrackInXYProjection,  evt. "
<<IVOLTE<<", nTracksFoundSoFar "<<
nTracksFoundSoFar <<" nHits "<<nHitsinTrack[nTracksFoundSoFar]
<<", iseed "<<iHit<<endl;
cout<<"\tLista Stt Hits :\n";
for(int iz=0;iz<nHitsinTrack[nTracksFoundSoFar];iz++){
	cout<<"\tStt hit n. "<<ListHitsinTrack[nTracksFoundSoFar][iz]<<endl;
}
cout<<"\tLista info in Conformal :\n";
int nummm;
 if(iHit<0) { nummm=nHitsinTrack[nTracksFoundSoFar]+1;}
 else{nummm=nHitsinTrack[nTracksFoundSoFar];};
for(int iz=0;iz<nummm;iz++){
	cout<<"\t"<<Xconformal[iz]<<",  "<<
	Yconformal[iz]<<",  "<<
	ErrorDriftRadiusconformal[iz]<<endl;
}

}
//--------------------

 status = FitHelixCylinder(
		nFitPoints, // +1 comes from one SciTil hit.
		Xconformal,
		Yconformal,
		DriftRadiusconformal,
		ErrorDriftRadiusconformal,
		rotationangle,  //  rotationangle, da mettere
		trajectory_vertex,	//  vertex in (X,Y) of this trajectory
		MAXHITSINFIT,  //  maximum n. of hits allowed in fast fit
		&m,
		&q,
		&ALFA[nTracksFoundSoFar],
		&BETA[nTracksFoundSoFar],
		&GAMMA[nTracksFoundSoFar],
		&TypeConf[nTracksFoundSoFar]
			);

 if(status < 0  ) return false;

//  this trasformation is valid even if the equation is a straight line from the fit

 Ox[nTracksFoundSoFar]= -0.5*ALFA[nTracksFoundSoFar];
 Oy[nTracksFoundSoFar]= -0.5*BETA[nTracksFoundSoFar];
 R[nTracksFoundSoFar]= Ox[nTracksFoundSoFar]*Ox[nTracksFoundSoFar]+
		Oy[nTracksFoundSoFar]*Oy[nTracksFoundSoFar]-
		GAMMA[nTracksFoundSoFar];

	// some obvious preliminary cuts
 if( R[nTracksFoundSoFar] < 0. )  return false;
 R[nTracksFoundSoFar]= sqrt( R[nTracksFoundSoFar] );
 aaa = sqrt(Ox[nTracksFoundSoFar]*Ox[nTracksFoundSoFar]+
		Oy[nTracksFoundSoFar]*Oy[nTracksFoundSoFar]);

 // the following is because the circumference is supposed to come from (0,0);
 //   here the factor 0.9 is used in order to be conservative.
 if(aaa< 0.9*RSTRAWDETECTORMIN/2.) return false;

//   here the factor 0.9 is used in order to be conservative.
 if ( R[nTracksFoundSoFar] + aaa < RSTRAWDETECTORMIN *0.9 ) return false;

//---------------------------

//  check again if the SciTil hit (if present) is compatible with this circle trajectory
//  in  XY by finding if it has intersection (in the XY plane) with Helix circle

//  equation of the SciTil segment :  y0 * y + x0 * x - x0**2 - y0**2 = 0
//  where  (x0,y0) = position of center of the SciTil.

//  delimiting points of the SciTil segment :  define L = length of the SciTil, 
//  and RR = sqrt(x0**2+y0**2), SIGN = the sign of (-x0*y0) or SIGN=1 when y0=0,
//  SIGN=irrelevant when x0=0;   then :
//  P1 =  [ x0- abs{(L/2)*y0/RR}; y0-SIGN*abs{(L/2)*x0/RR} ],
//  P2 =  [ x0+abs{(L/2)*y0/RR}; y0+SIGN*abs{(L/2)*x0/RR} ].

 bool intersect;
 UShort_t
	Nint,
	nSciT;
 Double_t distance,
	QQ,
	sqrtRR,
	SIGN,
	XintersectionList[2],
	YintersectionList[2];


 // whether or not the seed hit was a Stt hit try if any SciTil hits are
 // associated to this track cand.

 if(YesSciTil){
	// nScit is the n. of SciTil hit associated to this track.
	nSciT = AssociateSciTilHit(
			Ox[nTracksFoundSoFar],
			Oy[nTracksFoundSoFar],
			R[nTracksFoundSoFar],
			&ListSciTilHitsinTrack[nTracksFoundSoFar][0],
			S// output; S on the lateral face of the Helix
			// of the SciTil hit (if present).
				);
	if(nSciT>0){
		nSciTilHitsinTrack[nTracksFoundSoFar]=nSciT;
		for(j=0;j<nSciTilHitsinTrack[nTracksFoundSoFar];j++){
			InclusionListSciTil[ListSciTilHitsinTrack[nTracksFoundSoFar][0]]
				=false;
			S_SciTilHitsinTrack[nTracksFoundSoFar][j]=S[j];
		}
	} else {
		nSciTilHitsinTrack[nTracksFoundSoFar]=0;
	}
 }else{
	nSciTilHitsinTrack[nTracksFoundSoFar]=0;
 } // end of if(YesSciTil)


//---------------------  better association of the hits in the track candidate
// treat differently the case in which the track has radius < RSTRAWDETECTORMAX/2
// and the other case.

 if( R[nTracksFoundSoFar] < RSTRAWDETECTORMAX/2){
	FindingParallelTrackAngularRange(
		Ox[nTracksFoundSoFar],
		Oy[nTracksFoundSoFar],
		R[nTracksFoundSoFar],
		1,  /// this is supposed to be the charge, irrelevant here if it is +1 or -1.
		&Fi_low_limit[nTracksFoundSoFar],
		&Fi_up_limit[nTracksFoundSoFar],
		&flagStt,
		RSTRAWDETECTORMIN,
		RSTRAWDETECTORMAX
		);

	NN =  TrkAssociatedParallelHitsToHelix5(
		InclusionList,
		nsttparhit,
		Ox[nTracksFoundSoFar],
		Oy[nTracksFoundSoFar],
		R[nTracksFoundSoFar],
		info,
		Fi_low_limit[nTracksFoundSoFar],
		Fi_up_limit[nTracksFoundSoFar],
		auxListHitsinTrack              //  this is the output
				);


 }  else {

	NN =  TrkAssociatedParallelHitsToHelixQuater(
		InclusionList,
		m,
		q,
		status,
		nHitsinTrack[nTracksFoundSoFar],
		&ListHitsinTrack[nTracksFoundSoFar][0],
		nsttparhit,
		Ox[nTracksFoundSoFar],
		Oy[nTracksFoundSoFar],
		R[nTracksFoundSoFar],
		info,
		infoparalConformal,
		RConformalIndex,
		FiConformalIndex,
		nBoxConformal,
		HitsinBoxConformal,
		auxListHitsinTrack	//  this is the output
			);
 } // end of  if( R[nTracksFoundSoFar] < RSTRAWDETECTORMAX/2)

 if( NN < MINIMUMHITSPERTRACK || NN>MAXSTTHITSINTRACK) return false;

 nHitsinTrack[nTracksFoundSoFar]=NN;

 for(i=0; i<nHitsinTrack[nTracksFoundSoFar];i++){
	ListHitsinTrack[nTracksFoundSoFar][i]=auxListHitsinTrack[i];
 }



//----------------------------- Finding the Charge
	// The charge is calculated first by dividing the hits in two categories by using the Perpendicular
	// to the tangent to the trajectory in (0,0). Then simply the majority of the hits decides the
	// sign of the  charge. See Gianluigi's Logbook page 290.

	Double_t
		X[nHitsinTrack[nTracksFoundSoFar]],
		Y[nHitsinTrack[nTracksFoundSoFar]];

	for(i=0;i<nHitsinTrack[nTracksFoundSoFar];i++){
		X[i]=info[ListHitsinTrack[nTracksFoundSoFar][i]][0];
		Y[i]=info[ListHitsinTrack[nTracksFoundSoFar][i]][1];
	}
	FindCharge(
		Ox[nTracksFoundSoFar],
		Oy[nTracksFoundSoFar],
		nHitsinTrack[nTracksFoundSoFar],
		X,
		Y,
		&Charge[nTracksFoundSoFar]
		);


//----------------------------- Ordering.
	// The ordering reflects the angle Fi in the Helix reference frame because
	// the hits are ordered by going around the trajectory cclockwise for
	// positive tracks or counterclockwise for negative particles; in other words
	//  it is not used simply the distance of the hit from (0,0) as ordering parameter
	//  but the track length of the circle.
	// this method finds also Fi_initial_helix_referenceframe and Fi_final_helix_referenceframe.
	// The former is simply the fi angle of the point (0,0) with respect to the center
	// of this Helix. Important : this angle has to be > 0 always and it is between 0. and
	// 2 PI here (later,  FixDiscontinuitiesFiangleinSZplane may change it adding +2PI or
	// -2PI if necessary).
	// Fi_final_helix_referenceframe is made such that  it is < Fi_initial_helix_referenceframe
	// when the track is  positive, and it is > Fi_initial_helix_referenceframe for negative
	// tracks.

 OrderingParallel(
		Ox[nTracksFoundSoFar],
		Oy[nTracksFoundSoFar],
		info,
		nHitsinTrack[nTracksFoundSoFar],
		&ListHitsinTrack[nTracksFoundSoFar][0],
		ListSttParHits,
		Charge[nTracksFoundSoFar],
		&Fi_initial_helix_referenceframe[nTracksFoundSoFar],//output
		&Fi_final_helix_referenceframe[nTracksFoundSoFar],// output
		U,
		V
			);

//   finding the FI angular range (in the laboratory frame) spanned by this parallel track

 FindingParallelTrackAngularRange(
		Ox[nTracksFoundSoFar],
		Oy[nTracksFoundSoFar],
		R[nTracksFoundSoFar],
		Charge[nTracksFoundSoFar],
		&Fi_low_limit[nTracksFoundSoFar],
		&Fi_up_limit[nTracksFoundSoFar],
		&flagStt,
		RSTRAWDETECTORMIN,
		RSTRAWDETECTORMAX
					);


	if( flagStt == -2 ) return false;	// track outside cylinder RSTRAWDETECTORMAX.
	if( flagStt == -1 ) return false;	// track inside cylinder RSTRAWDETECTORMIN.
	if( flagStt ==  1 ) return false;	// track comprised in cylinder : discard,
					// because at this stage only tracks from
					// vertex are searched.





	return true;
};

//----------end of function PndTracking::FindTrackInXYProjection



//----------begin of function PndTracking::FindTrackPatterninBoxConformal

Short_t PndTracking::FindTrackPatterninBoxConformal(
	UShort_t NRCELLDISTANCE,
	UShort_t NFiCELLDISTANCE,
	UShort_t Nparal,
	Short_t ihit, // seed hit;
	Short_t nRcell, // R cell of the seed hit;
		// can be negative beacuse of SciTil hits;
	UShort_t nFicell, // Fi cell of the seed hit;
	Double_t info[][7],
	bool *InclusionList,
	UShort_t *RConformalIndex,
	UShort_t *FiConformalIndex,
	UShort_t nBoxConformal[][NFIDIVCONFORMAL],
	UShort_t HitsinBoxConformal[][NRDIVCONFORMAL][NFIDIVCONFORMAL],
	UShort_t  *ListHitsinTrack
			)
{

 bool
	status,
	TemporaryInclusionList[MAXSTTHITS];

 Short_t	i;
 UShort_t	j,
		iFi,
		iR,
		nRmin,
		nRmax,
		nRemainingHits,
		nHitsinTrack,
		auxIndex[MAXSTTHITS],
		Remaining[MAXSTTHITS];

 Short_t iFi2;

 Double_t auxRvalues[MAXSTTHITS];

//   ihit        is the hit number in the PARALLEL number scheme

 for(i=0, nRemainingHits=0; i<Nparal; i++){

	if( i != ihit && InclusionList[  ListSttParHits[i]   ] ) {   //  Inclusion of the
				//  parallel hit straws already used in other tracks
				//  remember the index of InclusionList is in the
				//  ORIGINAL scheme of hits
		TemporaryInclusionList[ ListSttParHits[i]  ]= true;
		Remaining[nRemainingHits]= i;   //  index of the PARALLEL hit
		nRemainingHits++;
	} else {
		TemporaryInclusionList[ ListSttParHits[i]  ]= false;
	}
 }

 if( nRemainingHits < MINIMUMHITSPERTRACK )    return 0;



//  cells of the seed hit

 if(ihit>=0){
	nHitsinTrack=1;
	ListHitsinTrack[0]=  ihit ;
	i = 0;
 } else {
	nHitsinTrack=0;
	i = -1;
 }

 status=true;
 while( nRemainingHits > 0 &&  i < nHitsinTrack && status) {


	if (nRcell - NRCELLDISTANCE < 0 ) {
		nRmin = 0;
	}  else {
		nRmin = nRcell - NRCELLDISTANCE;
	}
	if (nRcell + NRCELLDISTANCE >= nRdivConformalEffective ) {
		nRmax = nRdivConformalEffective-1;
	}  else {
		nRmax = nRcell + NRCELLDISTANCE;
	}
if(istampa>0) {cout<<"\tin FindTrackPatterninBoxConformal, nRmin = "<<nRmin
	<<", nRmax = "<<nRmax<<endl;
cout<<"\tin FindTrackPatterninBoxConformal, nFicell = "<<nFicell<<endl;
}
    for( iR= nRmin ; iR<= nRmax && status ; iR++){
      for(iFi2=nFicell-NFiCELLDISTANCE;iFi2<=nFicell+NFiCELLDISTANCE && status;iFi2++){
		if ( iFi2 < 0 )  {
			iFi = NFIDIVCONFORMAL + iFi2;
		} else if ( iFi2 >= NFIDIVCONFORMAL) {
			iFi = iFi2  - NFIDIVCONFORMAL;
		}  else {
			iFi = iFi2;
		}
	 for (j = 0; j< nBoxConformal[iR][iFi]; j++){
	    if( InclusionList[ HitsinBoxConformal[j][iR][iFi] ]
					&&
		   TemporaryInclusionList[HitsinBoxConformal[j][iR][iFi]]){
			// hit number in the PARALLEL straws scheme
			ListHitsinTrack[nHitsinTrack]=HitsinBoxConformal[j][iR][iFi] ;
			nHitsinTrack++;
		if( nHitsinTrack >= MAXSTTHITSINTRACK){
		 // finish the search
		 status=false; // finish all outer loops as well.
		 break ;
		}
		TemporaryInclusionList[HitsinBoxConformal[j][iR][iFi]]= false;
		nRemainingHits--;
	    } // end of if( InclusionList[  ListSttParHits[...
         }// end of  for (j = 0; j< nBoxConformal[iR][iFi]; j++)
      }  // end of  for( iFi2 = nFicell - NFiCELLDISTANCE ;
    }	// end of  for( iR= nRmin ; iR<= nRmax ; iR++)
//----------------
    i++;
    if(i<MAXSTTHITSINTRACK){
	nRcell = RConformalIndex[ListHitsinTrack[i]];
	nFicell = FiConformalIndex[ListHitsinTrack[i]];
    }
   }    //  end      while ( nRemainingHits > 0 && i < nHitsinTrack)



    return nHitsinTrack;

}


//----------end of function PndTracking::FindTrackPatterninBoxConformal



//----------begin of function PndTracking::FindTrackPatterninBoxConformalSpecial

//  The difference with FindTrackPatterninBoxConformal is that the search is performed
//  in the selected list    ListHitsinTrackinWhichToSearch
//  instead of all the parallel list.

Short_t PndTracking::FindTrackPatterninBoxConformalSpecial(
	UShort_t NRCELLDISTANCE,
	UShort_t NFiCELLDISTANCE,
	UShort_t Nparal,
	UShort_t NparallelToSearch,
	UShort_t iSeed,
	UShort_t *ListHitsinTrackinWhichToSearch,
	Double_t info[][7],
	bool *InclusionList,
	UShort_t *RConformalIndex,
	UShort_t *FiConformalIndex,
	UShort_t nBoxConformal[][NFIDIVCONFORMAL],
	UShort_t HitsinBoxConformal[][NRDIVCONFORMAL][NFIDIVCONFORMAL],
	UShort_t *OutputListHitsinTrack
					)
{


 bool TemporaryInclusionList[MAXSTTHITS];

 UShort_t	i,
		i2,
		j,
		iFi,
		iR,
		nFicell,
		nHitsinTrack,
		nRcell,
		nRemainingHits,
		nRmax,
		nRmin,
		auxIndex[MAXSTTHITS],
		Remaining[MAXSTTHITS];

 Short_t iFi2;

 Double_t auxRvalues[MAXSTTHITS];

//   iSeed        is the hit number in the ORIGINAL number scheme

//--------    the following initialization is essential for the algorithm to work
 for(i=0; i<Nparal; i++){
	TemporaryInclusionList[ ListSttParHits[i]  ]= false;
 }
//-------------

 for(i2=0, nRemainingHits=0; i2<NparallelToSearch; i2++){
	i=ListHitsinTrackinWhichToSearch[i2];
//  Inclusion of the parallel hit straws already used in other tracks;
//  remember the index of InclusionList is in the ORIGINAL scheme of hits.
	if( i != iSeed && InclusionList[ListSttParHits[i]] ) {
		TemporaryInclusionList[ListSttParHits[i]]= true;
		Remaining[nRemainingHits]= i;   //  index of the PARALLEL hit
		nRemainingHits++;
	}
 }


 if( nRemainingHits < MINIMUMHITSPERTRACK )    return 0;

//  cells of the seed hit

 nHitsinTrack=1;
 OutputListHitsinTrack[0]=  iSeed ;
 i = 0;
 while( nRemainingHits > 0 &&  i < nHitsinTrack) {

	nRcell = RConformalIndex[OutputListHitsinTrack[i]];
		nFicell = FiConformalIndex[OutputListHitsinTrack[i]];

//---------------

	if (nRcell - NRCELLDISTANCE < 0 ) {
		nRmin = 0;
	}  else {
		nRmin = nRcell - NRCELLDISTANCE;
	}
	if (nRcell + NRCELLDISTANCE >= nRdivConformalEffective ) {
		nRmax = nRdivConformalEffective-1;
	}  else {
		nRmax = nRcell + NRCELLDISTANCE;
	}

	for( iR= nRmin ; iR<= nRmax ; iR++){
	  for( iFi2=nFicell-NFiCELLDISTANCE ;
		iFi2<=nFicell+NFiCELLDISTANCE;iFi2++){
		if ( iFi2 < 0 )  {
			iFi = NFIDIVCONFORMAL + iFi2;
		} else if ( iFi2 >= NFIDIVCONFORMAL) {
			iFi = iFi2  - NFIDIVCONFORMAL;
		} else {
			iFi = iFi2;
		}
	     for (j = 0; j< nBoxConformal[iR][iFi]; j++){
		if(InclusionList[HitsinBoxConformal[j][iR][iFi]]
				&&
			TemporaryInclusionList[HitsinBoxConformal[j][iR][iFi]]) {
//  hit number in the ORIGINAL straws scheme
		  OutputListHitsinTrack[nHitsinTrack]=HitsinBoxConformal[j][iR][iFi] ;
		  nHitsinTrack++;
		  TemporaryInclusionList[HitsinBoxConformal[j][iR][iFi]]
			= false;
		  nRemainingHits--;
		}  // end of    if(InclusionList[
	     } // end of for (j = 0; j< nBoxConformal[iR][iFi]; j++)
	  }  // end of    for( iFi2=nFice
	}  // end of  for( iR= nRmin
//----------------
	i++;

 }    //  end while ( nRemainingHits > 0 && i < nHitsinTrack)


 return nHitsinTrack;

}


//----------end of function PndTracking::FindTrackPatterninBoxConformalSpecial


//----------begin of function PndTracking::FitHelixCylinder

Short_t PndTracking::FitHelixCylinder(
	UShort_t nHitsinTrack,
	Double_t *Xconformal,
	Double_t *Yconformal,
	Double_t *DriftRadiusconformal,
	Double_t *ErrorDriftRadiusconformal,
	Double_t rotationangle,
	Double_t trajectory_vertex[2],
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
//   if (NpointsInFit>MAXSTTHITSINTRACK+MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK)
//	NpointsInFit=MAXSTTHITSINTRACK+MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK;
//   bool mvdhit[MAXSTTHITSINTRACK+MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK];
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


//----------end of function PndTracking::FitHelixCylinder



//----------begin of function PndTracking::FitSZspace

Short_t PndTracking::FitSZspace(
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
		cout<<"from PndTracking::FitSZspace  :  no points in fit, return!\n";
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

//----------end of function PndTracking::FitSZspace




//----------start of function PndTracking::FixDiscontinuitiesFiangleinSZplane

void PndTracking::FixDiscontinuitiesFiangleinSZplane(
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
//----------end of function PndTracking::FixDiscontinuitiesFiangleinSZplane



//----------begin of function PndSttTracking::FromXYtoConformal

void PndTracking::FromXYtoConformal(
	Double_t trajectory_vertex[2],
	Double_t info[][7],
	UInt_t Nparal,
	Double_t infoparalConformal[][5]
		)
{


//   do the transformation in the conformal space :  u= x/(x**2+y**2), v= y/(x**2+y**2) for each hit from parallel
//   straws;  also the equidrift radius changes.

//

  UShort_t
	i;

  Double_t
	gamma,
	r,
	x,
	y;

  for(i=0; i<Nparal; i++){
	x = info[ListSttParHits[i]][0]-trajectory_vertex[0];
	y = info[ListSttParHits[i]][1]-trajectory_vertex[1];
	r = info[ListSttParHits[i]][3];
	gamma = x*x + y*y - r*r;
	infoparalConformal[ListSttParHits[i]][0] = x / gamma;
	infoparalConformal[ListSttParHits[i]][1] = y / gamma;
	infoparalConformal[ListSttParHits[i]][2] = r/fabs(gamma);
	//  n. of the Hit (in the original order)
	infoparalConformal[ListSttParHits[i]][3] = ListSttParHits[i] ;
	infoparalConformal[ListSttParHits[i]][4] = STRAWRADIUS/fabs(gamma);

  }


  return;
}

//----------end of function PndSttTracking::FromXYtoConformal




//------------------------- begin of function  PndTracking::getMCInfo

void PndTracking::getMCInfo(
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

//------------------------- end of function  PndTracking::getMCInfo




//---------- begin of function PndTracking::InfoXYZParal

void PndTracking::InfoXYZParal(
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

//----------end of function PndTracking::InfoXYZParal





//----------begin of function PndTracking::IntersectionCircle_Segment

bool PndTracking::IntersectionCircle_Segment(
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
		cout<<"from  PndTracking::IntersectionCircle_Segment :"
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

//----------end of function PndTracking::IntersectionCircle_Segment


//----------begin of function PndTracking::IntersectionSciTil_Circle
bool PndTracking::IntersectionSciTil_Circle(
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
//----------end of function PndTracking::IntersectionSciTil_Circle




//----------start  function PndTracking::IntersectionsWithClosedbiHexagonLeft

Short_t  PndTracking::IntersectionsWithClosedbiHexagonLeft(
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

//---------- end of  function PndTracking::IntersectionsWithClosedbiHexagonLeft







//----------start  function PndTracking::IntersectionsWithClosedbiHexagonRight

Short_t  PndTracking::IntersectionsWithClosedbiHexagonRight(
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

//---------- end of  function PndTracking::IntersectionsWithClosedbiHexagonRight



//----------start  function PndTracking::IntersectionsWithClosedPolygon

Short_t  PndTracking::IntersectionsWithClosedPolygon(
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

//---------- end of  function PndTracking::IntersectionsWithClosedPolygon



//----------begin of function PndTracking::IntersectionsWithGapSemicircle

UShort_t PndTracking::IntersectionsWithGapSemicircle(
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
//----------end of function PndTracking::IntersectionsWithGapSemicircle



//----------start  function PndTracking::IntersectionsWithOpenPolygon

UShort_t  PndTracking::IntersectionsWithOpenPolygon(
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

//---------- end of  function PndTracking::IntersectionsWithOpenPolygon



//----------begin of function PndTracking::IsInsideCircle

bool PndTracking::IsInsideArc(
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

//----------end of function PndTracking::IsInsideCircle




//----------end of function PndTracking::IsInTargetPipe

bool PndTracking::IsInTargetPipe(
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
//----------end of function PndTracking::IsInTargetPipe



//----------star of function PndTracking::IsInternal

bool PndTracking::IsInternal(
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
//----------end of function PndTracking::IsInternal




void PndTracking::MakeInclusionListStt(
	UInt_t nSttHit,
	Double_t info[][7]
	)
{
//	fill the inclusion list for Stt, include only first hit for those straws with
//	multiple hits.
 for(int i=0; i< nSttHit-1; i++){
	if( !InclusionListStt[ i ] ) continue;
	for(int j=i+1; j< nSttHit; j++){
		if(InclusionListStt[ j ] &&
			fabs(info[i][0] - info[j][0])<1.e-20 &&
			fabs(info[i][1] - info[j][1])<1.e-20  )
		{
			InclusionListStt[j]= false ;
		}
	} //  end of  for(j=i+1; j< Nhits;; j++)


 }   //   end of for(i=0; i< Nhits-1; i++)




}
//--------------------------------  end of function   PndTracking::MakeInclusionListStt



//--------------------- begin function  PndTracking::MatchMvdHitsToSttTracks

void PndTracking::MatchMvdHitsToSttTracks(
	Double_t delta,
	UShort_t nSttTrackCand,
	Double_t *Ox,
	Double_t *Oy,
	Double_t *R,
	Double_t *FI0,
	Double_t *Fifirst,
	Short_t *CHARGE,

	UShort_t *nPixelHitsinTrack, // output
	UShort_t ListPixelHitsinTrack[][MAXMVDPIXELHITS], // output
	UShort_t *nStripHitsinTrack, // output
	UShort_t ListStripHitsinTrack[][MAXMVDSTRIPHITS] // output
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

//------------------------- end of function  PndTracking::MatchMvdHitsToSttTracks



//------------------------- begin of function  PndTracking::MatchMvdHitsToSttTracksagain

void PndTracking::MatchMvdHitsToSttTracksagain(
	bool *keepit,
	bool *Mvdhits,
	Double_t delta,
	Double_t highqualitycut,
	UShort_t nSttTrackCand,
	Double_t *Ox,
	Double_t *Oy,
	Double_t *R,
	Double_t *FI0,
	Double_t *Fifirst,
	Short_t *CHARGE,

	UShort_t *nPixelHitsinTrack, // output
	UShort_t ListPixelHitsinTrack[][MAXMVDPIXELHITSINTRACK], // output
	UShort_t *nStripHitsinTrack, // output
	UShort_t ListStripHitsinTrack[][MAXMVDSTRIPHITSINTRACK] // output
	)
{
	bool
		flaggo,
		specialcase,
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
		if( ! Mvdhits[itrack] ) continue;
		ntot=nPixelHitsinTrack[itrack]+
			nStripHitsinTrack[itrack];
	if( Fifirst[itrack] < -99998. ){  // case with Fifirst[i]=-99999.; in this
					// case the circle is contained
					// in the Mvd region.
		anglemax = 2.*PI;
		anglemin = 0.;
	} else {




	if(CHARGE[itrack]>0){	// track must rotate clockwise looking into the beam.
		anglemax = FI0[itrack];
		anglemin = Fifirst[itrack];
	} else {
		anglemin = FI0[itrack];
		anglemax = Fifirst[itrack];
	}
		if(anglemax < anglemin) anglemax += 2.*PI;
		if(anglemax < anglemin) anglemax=anglemin; // this is just to be super-sure.

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
		  if(ndownstream>ntot-ndownstream) downstream=true; else downstream=false;



//  loop over the Mvd Pixel and try to attach new Pixels to each candidate track



		naddpix=0;
		for(ipix=0; ipix<nMvdPixelHit; ipix++){
			flaggo=true;
			for(j=0; j<nPixelHitsinTrack[itrack]; j++){
			     if( ListPixelHitsinTrack[itrack][j]==ipix){
				flaggo=false;
				break;
			     }
			}
			if(flaggo){
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
			} // end of if(flaggo)

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
			flaggo=true;
			for(j=0; j<nStripHitsinTrack[itrack]; j++){
			     if( ListStripHitsinTrack[itrack][j]==istr){
				flaggo=false;
				break;
			     }
			}
			if(flaggo){
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

			} // end of if(flaggo)

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

//------------------- end function  PndTracking::MatchMvdHitsToSttTracksagain



//------------------------- begin of function  PndTracking::MatchMvdHitsToSttTracks2

void PndTracking::MatchMvdHitsToSttTracks2(
	bool *keepit,
	Double_t delta,
	Double_t highqualitycut,
	UShort_t nSttTrackCand,
	Double_t *Ox,
	Double_t *Oy,
	Double_t *R,
	Double_t *FI0,
	Double_t *Fifirst,
	Short_t *CHARGE,
	UShort_t *nPixelHitsinTrack, // output
	UShort_t ListPixelHitsinTrack[][MAXMVDPIXELHITSINTRACK], // output
	UShort_t *nStripHitsinTrack, // output
	UShort_t ListStripHitsinTrack[][MAXMVDSTRIPHITSINTRACK] // output
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



//int temporaneo=4;

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


if(istampa>=3 ){cout<<"da PndTracking : appena prima arbitration, IVOLTE = "<<
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
if(istampa>=3 ){cout<<"da PndTracking : goodmix n. 0, total distance (che e' = total distance2) = "<<oldtotal
				<<", e nHighQuality = "<<nHighQuality[0]<<endl;}
			chosenmix=0;
			chosenmix2=0;
			for(j1=1; j1<ngoodmix;j1++){
				total = DIST[j1];
if(istampa>=3){cout<<"da PndTracking :\t goodmix n. "<<j1<<", total distance "<<total
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
if(istampa>=3 ){cout<<"da PndTracking : fine arbitration, IVOLTE = "<<
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

//------------------------- end of function  PndTracking::MatchMvdHitsToSttTracks2



//----------begin of function PndTracking::Merge

void PndTracking::Merge(
	UShort_t nl,
	Double_t *left,
	Int_t *ind_left,
	UShort_t nr,
	Double_t *right,
	Int_t *ind_right, 
	Double_t *result,
	Int_t *ind
	)
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

//----------end of function PndTracking::Merge




//----------begin of function PndTracking::Merge_Sort

void PndTracking::Merge_Sort(
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


//----------end of function PndTracking::Merge_Sort



//------------------ begin function  PndTracking::MvdMatchtoMC

void PndTracking::MvdMatchtoMC(
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
		dist=ERRORSQPIXEL;
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
		dist=ERRORSQSTRIP;
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


//------------------ end function  PndTracking::MvdMatchtoMC





//------------------ begin function  PndTracking::MvdMatchedSpurioustoTrackCand
void PndTracking::MvdMatchedSpurioustoTrackCand(
	UShort_t nSttTrackCand,		// input
	bool *keepit,			// input
	Short_t *daTrackFoundaTrackMC,	// input
	Int_t *FromPixeltoMCTrack,	// input
	Int_t *FromStriptoMCTrack,	// input
	UShort_t *nPixelHitsinTrack,	// input
	UShort_t ListPixelHitsinTrack[][MAXMVDPIXELHITSINTRACK],// input
	UShort_t *nStripHitsinTrack,				// input
	UShort_t ListStripHitsinTrack[][MAXMVDSTRIPHITSINTRACK],// input

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

//------------------ end function  PndTracking::MvdMatchedSpurioustoTrackCand


//------begin function PndTracking::OrderingConformal_Loading_ListTrackCandHit
void PndTracking::OrderingConformal_Loading_ListTrackCandHit(
	bool *keepit,
	UShort_t ncand,
	Double_t info[][7],
	Double_t * Ox,
	Double_t * Oy,
	Double_t * Rr,
	Double_t Trajectory_Start[][2],
	Short_t *CHARGE,
	Double_t SchosenSkew[][MAXSTTHITS]
				)
{

	UShort_t	i,
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
			 XY2[j][0] = Ox[ncand]+Rr[ncand]*
				cos(SchosenSkew[ncand][ListSttSkewHitsinTrack[ncand][i]]);
			 XY2[j][1] = Oy[ncand]+Rr[ncand]*
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
	cout<<"from PndTracking, hit n. "<<ListTrackCandHit[ncand][ica]<<", hit type "
	<<ListTrackCandHitType[ncand][ica]<<endl;
}


//	} //   end of  for(ncand=FirstCandidate; ncand< LastCandidate; ncand++)


	return;
}

//--------end function PndTracking::OrderingConformal_Loading_ListTrackCandHit




//----------begin of function PndTracking::OrderingParallel

void PndTracking::OrderingParallel(
	Double_t oX,
	Double_t oY,
	Double_t info[][7],
	UShort_t nParallelHits,
	UShort_t *ListParallelHits,
	UShort_t *Infoparal,
	Short_t  Charge,  // input
	Double_t *Fi_initial_helix_referenceframe, // output
	Double_t *Fi_final_helix_referenceframe,   // output
	Double_t *U,  // output
	Double_t *V   // output
	)
{

 UShort_t
	i,
	j,
	tmp[nParallelHits];

 Int_t	tempo[nParallelHits];

      Double_t	aaa,
		b1,
		firstR2,
		lastR2,
		aux[nParallelHits];



//  here there is the ordering of the hits, under the assumption that the circumference
//  in XY goes through (0,0).
//  Moreover, the code before is supposed to have selected trajectories in XY with (Ox,Oy)
//  farther from (0,0) by > 0.9 * RminStrawDetector/2 and consequently Ox and Oy are not both 0.
//  The scheme for the ordering of the hit is as follows :
//  1)  order hits by increasing U or V of the conformal mapping; see Gianluigi's Logbook page 283;
//  2)  find the charge of the track by checking if it is closest to the center in XY
//	the first or the last of the ordered hits.
//  3)  in case, invert the ordering of U, V and ListParallelHits such that the first hits in the
//	list are alway those closer to the (0,0).


//   ordering of the hits

	aaa = atan2( oY, oX);  // atan2 defined between -PI and PI.

	// the following statement is necessary since for unknown reason the root interpreter
	// gives a weird error when using PI directly in the if statement below!!!!!!! I lost
	// 2 hours trying to figure this out!
	b1 = PI/4.;

	if((aaa>b1&&aaa<3.*b1) || (aaa>-3.*b1&&aaa<-b1)){//use U as ordering variable;
							//[case 1 or 3 Gianluigi's Logbook page 285].
		for (j = 0; j< nParallelHits; j++){
			U[j]=info[ Infoparal[ ListParallelHits[j] ]  ][0]/(
			info[ Infoparal[ ListParallelHits[j] ]  ][0]*
			info[ Infoparal[ ListParallelHits[j] ]  ][0]+
			info[ Infoparal[ ListParallelHits[j] ]  ][1]*
			info[ Infoparal[ ListParallelHits[j] ]  ][1]);
		}
		for (j = 0; j< nParallelHits; j++){
			tempo[j]= (Int_t) ListParallelHits[j];
		}
		Merge_Sort( nParallelHits, U, tempo);
//		Merge_Sort( nParallelHits, U, ListParallelHits);
		for (j = 0; j< nParallelHits; j++){
			ListParallelHits[j] = (UShort_t) tempo[j];
		}

		if((aaa>b1&&aaa<3.*b1)){  //  case #1;
			if( Charge == -1){
				// inverting the order of the hits.
				for(i=0;i<nParallelHits;i++){
					tmp[i]=ListParallelHits[nParallelHits-1-i];
					aux[i] = U[nParallelHits-1-i];
				}
				for(i=0;i<nParallelHits;i++){
					ListParallelHits[i]=tmp[i];
					U[i] = aux[i];
				}
			}
			for (j = 0; j< nParallelHits; j++){
				V[j]=info[ Infoparal[ ListParallelHits[j] ]  ][1]/(
				info[ Infoparal[ ListParallelHits[j] ]  ][0]*
				info[ Infoparal[ ListParallelHits[j] ]  ][0]+
				info[ Infoparal[ ListParallelHits[j] ]  ][1]*
				info[ Infoparal[ ListParallelHits[j] ]  ][1]);
			}
		} else{  //  case # 3.
			if(Charge == 1){
				// inverting the order of the hits.
				for(i=0;i<nParallelHits;i++){
					tmp[i]=ListParallelHits[nParallelHits-1-i];
					aux[i] = U[nParallelHits-1-i];
				}
				for(i=0;i<nParallelHits;i++){
					ListParallelHits[i]=tmp[i];
					U[i] = aux[i];
				}
			}// end of  if( Charge ==1)
			for (j = 0; j< nParallelHits; j++){
				V[j]=info[ Infoparal[ ListParallelHits[j] ]  ][1]/(
				info[ Infoparal[ ListParallelHits[j] ]  ][0]*
				info[ Infoparal[ ListParallelHits[j] ]  ][0]+
				info[ Infoparal[ ListParallelHits[j] ]  ][1]*
				info[ Infoparal[ ListParallelHits[j] ]  ][1]);
			}
		}// end of  if((aaa>b1&&aaa<3.*b1))

	} else { // use V as ordering variable [case 2 or 4 Gianluigi's Logbook page 285].
		for (j = 0; j< nParallelHits; j++){
			V[j]=info[ Infoparal[ ListParallelHits[j] ]  ][1]/(
			info[ Infoparal[ ListParallelHits[j] ]  ][0]*
			info[ Infoparal[ ListParallelHits[j] ]  ][0]+
			info[ Infoparal[ ListParallelHits[j] ]  ][1]*
			info[ Infoparal[ ListParallelHits[j] ]  ][1]);
		}
		for (j = 0; j< nParallelHits; j++){
			tempo[j]= (Int_t) ListParallelHits[j];
		}
		Merge_Sort( nParallelHits, U, tempo);
//		Merge_Sort( nParallelHits, V, ListParallelHits);
		for (j = 0; j< nParallelHits; j++){
			ListParallelHits[j] = (UShort_t) tempo[j];
		}

		if((aaa<=-3.*b1 || aaa>=3.*b1)){  //  case #2;
			if( Charge == -1){
				// inverting the order of the hits.
				for(i=0;i<nParallelHits;i++){
					tmp[i]=ListParallelHits[nParallelHits-1-i];
					aux[i] = V[nParallelHits-1-i];
				}
				for(i=0;i<nParallelHits;i++){
					ListParallelHits[i]=tmp[i];
					V[i] = aux[i];
				}
			}
			for (j = 0; j< nParallelHits; j++){
				U[j]=info[ Infoparal[ ListParallelHits[j] ]  ][0]/(
				info[ Infoparal[ ListParallelHits[j] ]  ][0]*
				info[ Infoparal[ ListParallelHits[j] ]  ][0]+
				info[ Infoparal[ ListParallelHits[j] ]  ][1]*
				info[ Infoparal[ ListParallelHits[j] ]  ][1]);
			}
		} else{  //  case # 4.
			if( Charge == 1){
				// inverting the order of the hits.
				for(i=0;i<nParallelHits;i++){
					tmp[i]=ListParallelHits[nParallelHits-1-i];
					aux[i] = V[nParallelHits-1-i];
				}
				for(i=0;i<nParallelHits;i++){
					ListParallelHits[i]=tmp[i];
					V[i] = aux[i];
				}
			}
			for (j = 0; j< nParallelHits; j++){
				U[j]=info[ Infoparal[ ListParallelHits[j] ]  ][0]/(
				info[ Infoparal[ ListParallelHits[j] ]  ][0]*
				info[ Infoparal[ ListParallelHits[j] ]  ][0]+
				info[ Infoparal[ ListParallelHits[j] ]  ][1]*
				info[ Infoparal[ ListParallelHits[j] ]  ][1]);
			}
		}

	} //  end of   if((aaa>b1&& ....






//  FI initial value (at 0,0  vertex) in the Helix reference frame

	*Fi_initial_helix_referenceframe = atan2(-oY,-oX) ;//  this is in order to be coherent
			//  with the calculatation of Fi, which is atan2(oY,oX). 
			//  atan2  is defined in [-PI,PI)
	if ( *Fi_initial_helix_referenceframe <0.)
			*Fi_initial_helix_referenceframe += 2.*PI;

//  FI of the last parallel hit in the Helix reference frame

	*Fi_final_helix_referenceframe = atan2(
		info[ Infoparal[ ListParallelHits[nParallelHits-1] ] ][1]-oY,
		info[ Infoparal[ ListParallelHits[nParallelHits-1] ] ][0]-oX
						);
	if ( *Fi_final_helix_referenceframe <0.)
			*Fi_final_helix_referenceframe += 2.*PI;

	if ( Charge > 0 ) {
		if( *Fi_final_helix_referenceframe> *Fi_initial_helix_referenceframe)
			*Fi_final_helix_referenceframe -= 2.*PI;
		if( *Fi_final_helix_referenceframe> *Fi_initial_helix_referenceframe)
			*Fi_final_helix_referenceframe = *Fi_initial_helix_referenceframe;
	} else {
		if( *Fi_final_helix_referenceframe< *Fi_initial_helix_referenceframe)
			*Fi_final_helix_referenceframe += 2.*PI;
		if( *Fi_final_helix_referenceframe< *Fi_initial_helix_referenceframe)
			*Fi_final_helix_referenceframe = *Fi_initial_helix_referenceframe;
	}




 return; 


}
//----------end of function PndTracking::OrderingParallel



//----------begin of function PndTracking::OrderingR_Loading_ListTrackCandHit

void PndTracking::OrderingR_Loading_ListTrackCandHit(
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

//----------end of function PndTracking::OrderingR_Loading_ListTrackCandHit







//----------begin of function PndTracking::OrderingSttSkewandSttParallel

 void PndTracking::OrderingSttSkewandSttParallel(
			Double_t oX,
			Double_t oY,
			Double_t Rr,
			UShort_t nSkewhit,
			UShort_t *ListSkewHits,
			Double_t *SList, // this is rekated to the skew hits. IMPORTANT :
					// the index must be the ORIGINAL skew hit number,
					// therefore SList[Infoskew[ListSkewHits[*]]].
			Short_t  Charge,
			UShort_t nParHits,
			UShort_t *ListParHits,
			Double_t *U,
			Double_t *V,
			UShort_t *BigList // this is the final ordered Parallel+Skew list;
				// already in NATIVE hit number.
				)
{

 UShort_t
	i,
	j,
	tmp[nSkewhit+nParHits],
	tmpList[nSkewhit];

 Int_t	index[nSkewhit+nParHits];

 Double_t	aaa,
		b1,
		sign,
		aux[nSkewhit+nParHits];



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

	Merge_Sort( nSkewhit+nParHits, aux, index);


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
//----------end of function PndTracking::OrderingSttSkewandSttParallel










//----------begin of function PndTracking::OrderingUsingConformal

void   PndTracking::OrderingUsingConformal(
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
//----------end of function PndTracking::OrderingUsingConformal



//----------begin of function PndTracking::Ordering_Loading_ListTrackCandHit

void PndTracking::Ordering_Loading_ListTrackCandHit(
	bool *keepit,
	UShort_t FirstCandidate,
	UShort_t LastCandidate,
	Double_t info[][7],
	Double_t * Ox,
	Double_t * Oy,
	Double_t * Rr,
	Double_t Trajectory_Start[][2],
	Short_t *CHARGE,
	Double_t SchosenSkew[][MAXSTTHITS]
				)
{
	UShort_t ncand;

	for(ncand=FirstCandidate; ncand< LastCandidate; ncand++){
		// for small radius trajectory better the ordering with conformal.
		if( Rr[ncand]< RSTRAWDETECTORMAX/2.){
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
//----------end of function PndTracking::Ordering_Loading_ListTrackCandHit



//------------------ begin function  PndTracking::RefitMvdStt

void  PndTracking::RefitMvdStt(
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
	if(MAXIMUMHITSINFIT>MAXSTTHITSINTRACK+MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK)
	     MAXIMUMHITSINFIT =
		MAXSTTHITSINTRACK+MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK;

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
	cout<<"PndTracking::RefitMvdStt, n. Hits (Mvd+Stt || ) = "<<iparallel<<endl;
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


//------------------ end function  PndTracking::RefitMvdStt







//----------begin of function PndTracking::SeparateInnerOuterParallel

void PndTracking::SeparateInnerOuterParallel(

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
		if(r>2.*APOTEMAMAXINNERPARSTRAW/sqrt(3.) ){	// outer Parallel hit.
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


//----------end of function PndTracking::SeparateInnerOuterParallel



//----------begin of function PndTracking::stampaMCSttPoints
void PndTracking::stampaMCSttPoints(UInt_t nSttMCPoint)
{
	cout<<"from PndTracking, printout dei "<<nSttMCPoint<<" punti MC :\n";
	for( int i= 0; i< nSttMCPoint; i++){
		FairMCPoint * pSttMC = (FairMCPoint*) fSttPointArray->At(i);
		cout<<"\tpoint n. "<<i<<", Xmc "<<pSttMC->GetX()<<
		", Ymc "<<pSttMC->GetY()<<", Zmc "<<pSttMC->GetZ()
		<<", appartiene a MC Track "<< pSttMC->GetTrackID()<<endl;
	}
 }
//----------end of function PndTracking::stampaMCSttPoints



//----------begin of function PndTracking::stampaMCTracks

 void PndTracking::stampaMCTracks()
 {
	cout<<"from PndTracking, IVOLTE = "<<IVOLTE<<", n. MC Tracks "
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
//----------end of function PndTracking::stampaMCTracks




//----------begin of function PndTracking::stampaMvdHits

void PndTracking::stampaMvdHits()
{
  cout<<"da PndTraking  :  n. Mvd Pixel Hits = "<<nMvdPixelHit<<endl;
  cout<<"da PndTracking  :  n. Mvd Strip Hits = "<<nMvdStripHit<<endl;
  cout<<"da PndTracking  :  info hits di Mvd pixels ------------------\n";
  for(int i= 0; i< nMvdPixelHit; i++){
      cout<<"      Pixel hit n. "<<i<<" Info : X  = "<<XMvdPixel[i]<<";  Y  = "<<YMvdPixel[i]<<
            ";  Z  = "<<ZMvdPixel[i]<<", R=sqrt(X**2+Y**2) = "<<
	    sqrt(XMvdPixel[i]*XMvdPixel[i]+YMvdPixel[i]*YMvdPixel[i])<<endl;
      cout<<"\t\tPixel Info : sigmaX  = "<<sigmaXMvdPixel[i]<<";  sigmaY  = "<<sigmaYMvdPixel[i]<<
            ";  sigmaZ  = "<<sigmaZMvdPixel[i]<<endl<<"\t suo RefIndex = "<<refindexMvdPixel[i]<<
	    ", suo FairRootManager::Instance()->GetBranchId(fMvdPixelBranch) = "<<
	    FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)<<endl;
  }
  cout<<"        ------------------\n";

  cout<<"da PndTracking  :  info hits di Mvd strips ------------------\n";
  for(int i= 0; i< nMvdStripHit; i++){
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


 }

//----------end of function PndTracking::stampaMvdHits




//----------begin of function PndTracking::stampaMvdTrackCandInfo

void PndTracking::stampaMvdTrackCandInfo()
{

	int i,j;


	for( i= 0; i< nMvdTrackCand ; i++){
		cout<<" PndTracking, MvdTrackCand n. "<<i<<" ha "<<nHitMvdTrackCand[i]
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

//----------end of function PndTracking::stampaMvdTrackCandInfo




//----------begin of function PndTracking::stampaSttHits
void PndTracking::stampaSttHits(
	Short_t iHit,
	Short_t ipunto,
	Double_t dradius,
	Double_t *WDX,
	Double_t *WDY,
	Double_t *WDZ,
	FairMCPoint* puntator,
	PndSttTube * pSttTube
	)
{

	UShort_t i = iHit;

	cout <<"da PndTracking, Stt iHit "<< i << " e n. punto MC ottenuto con RefIndex = "
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

 }
//----------end of function PndTracking::stampaSttHits




//----------begin of function PndTracking::stampetta

void PndTracking::stampetta(
	UShort_t nSttTrackCand,
	bool *keepit
	)
{
	int i,j;

           cout<<"da PndTracking, evt. "<<IVOLTE<<",  n. SttTrackCand totali = "
	   <<nSttTrackCand<<endl;
      for(  i= 0; i< nSttTrackCand; i++){
	   cout<<"\tcandidate n. "<<i<<", keepit = ";
	   if(keepit[i]){ cout<<"true "; }else {cout<<"false ";}
	   cout<<"-----------------\n";
      	if(!keepit[i]) continue;
	   cout<<"da PndTracking --------------------------------------\n"<<
	   "	SttTrackCand n.  "<<i<<";  n. Hits in Pixels associati = "
	   <<nMvdPixelHitsinTrack[i]<<"   e loro lista \n";
	   for(j=0; j<nMvdPixelHitsinTrack[i];j++){
		cout<<"\t\tMvd Pixel Hit n. "<<
		     ListMvdPixelHitsinTrack[i][j]<<endl;
	   }
           cout<<"da PndTracking --------------------------------------\n"<<
	   "	SttTrackCand n.  "<<i<<";  n. Hits in Strips associati = "
	   <<nMvdStripHitsinTrack[i]<<"   e loro lista \n";
	   for(j=0; j<nMvdStripHitsinTrack[i];j++){
	        cout<<"\t\tMvd Strip Hit n. "<<
		     ListMvdStripHitsinTrack[i][j]<<endl;
	   }
           cout<<"da PndTracking --------------------------------------\n"<<
	   "	SttTrackCand n.  "<<i<<";  n. || Hits in Stt in Track = "
	   <<nSttParHitsinTrack[i]<<"   e loro lista \n";
	   for(j=0; j<nSttParHitsinTrack[i];j++){
	        cout<<"\t\t|| Stt Hit n. "<<
		     ListSttParHitsinTrack[i][j]<<endl;
	   }
           cout<<"da PndTracking --------------------------------------\n"<<
	   "	SttTrackCand n.  "<<i<<";  n. skew Hits in Stt Track = "
	   <<nSttSkewHitsinTrack[i]<<"   e loro lista \n";
	   for(j=0; j<nSttSkewHitsinTrack[i];j++){
	        cout<<"\t\tskew Stt Hit n. "<<
		     ListSttSkewHitsinTrack[i][j]<<endl;
	   }
           cout<<"da PndTracking --------------------------------------\n"<<
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
//----------end of function PndTracking::stampetta






//------------------------- begin of function  PndTracking::SttMatchedSpurious


void PndTracking::SttMatchedSpurious(
	bool *keepit,
	UShort_t ntotalHits,
	Double_t info[][7],
	UShort_t nTracksFoundSoFar, // thosse found by PR
	UShort_t nHitsinTrack[], // n. hits PARALLELI+SKEW, from PR
	UShort_t ListHitsinTrack[][MAXSTTHITSINTRACK], // from PR
	UShort_t *nSkewHitsinTrack, // n. hits skew, from PR
	UShort_t ListSkewHitsinTrack[][MAXSTTHITSINTRACK], // from PR

	UShort_t *nParalCommon,
	UShort_t ParalCommonList[][MAXSTTHITSINTRACK],
	UShort_t *nSpuriParinTrack,
	UShort_t ParSpuriList[][MAXSTTHITSINTRACK],

	UShort_t *nSkewCommon,
	UShort_t SkewCommonList[][MAXSTTHITSINTRACK],
	UShort_t *nSpuriSkewinTrack,
	UShort_t SkewSpuriList[][MAXSTTHITSINTRACK],

	UShort_t *nHitsInMCTrack,
	UShort_t *nSkewHitsInMCTrack,

	UShort_t *nMCParalAlone,
	UShort_t MCParalAloneList[][MAXSTTHITSINTRACK],
	UShort_t *nMCSkewAlone,
	UShort_t MCSkewAloneList[][MAXSTTHITSINTRACK],

	Short_t *daTrackFoundaTrackMC
	)
{
 bool flaggo;
 UShort_t	i, jexp, exphit, iHit,
		enne[MAXTRACKSPEREVENT][MAXSTTHITS];
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
			flaggo=true;
			for(exphit=0; exphit<nHitsinTrack[jexp]; exphit++){
				if(ListHitsinTrack[jexp][exphit] == i){
					flaggo=false;
					break;
				}
			}
			if(flaggo){
			MCParalAloneList[jexp][ nMCParalAlone[jexp] ] = i;
			nMCParalAlone[jexp]++;
			} // end of  if(flaggo)
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
			flaggo=true;
			for(exphit=0; exphit<nSkewHitsinTrack[jexp]; exphit++){
				if(i == ListSkewHitsinTrack[jexp][exphit] ){
					flaggo=false;
					break;
				}
			}
			if(flaggo){
				MCSkewAloneList[jexp][ nMCSkewAlone[jexp] ] = i;
				nMCSkewAlone[jexp]++;
			}
	}

	nSkewHitsInMCTrack[jexp] = nMCSkewAlone[jexp]+nSkewCommon[jexp];


   }   //   end of  for(jexp=0; jexp<nTracksFoundSoFar;jexp++)



    return;
}


//------------------------- end of function  PndTracking::SttMatchedSpurious






//----------begin of function PndTracking::SttParalCleanup

bool PndTracking::SttParalCleanup(
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


	bool
		flaggo;

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
		// don't discard track yet, see if it should have parallel hits.
	} else {

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

	} // end of if(nHits==0)

//	first of all, find possible intersection points with outer circle encompassing
//	the Stt system.

	flagOutStt = FindIntersectionsOuterCircle(
				Oxx,
				Oyy,
				Rr,
				RSTRAWDETECTORMAX,
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
	} else {



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
		flaggo=true;
		if( flagOutStt ==0 // 2 intersections with outer Stt circle.
		   // case when this track exits the Stt outer circle without
		   // hitting the Stt parallel inner section (for instance the track
		   &&  ((fabs(XcrossOut[0]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[0]-Ycross[0])<1.e-5 )
					||
				(fabs(XcrossOut[1]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[1]-Ycross[0])<1.e-5 )
				)
				){
			flaggo=false ;
		} else { // continuation of  if( flagOutStt ==0)

			// most usual case when track crossed the Inner parallel Stt.
			if (nnn == 0) return false;

			// if the exit point is actually given by FiLimitAdmissible, then allow
			// an extra uncertainty in the # Stt hit that must be present;
			// this is done because FiLimitAdmissible is not a very precise number.
			if( fabs(aux[0]-Xcross[1])<1.e-5&& fabs(aux[1]-Ycross[1])<1.e-5 ){
				islack = 3;
			}
		} //  end of  if( flagOutStt ==0)

	} else { // continuation of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)
		if( flagOutStt ==0){// 2 intersections with outer Stt circle.
			   // case when this track exits the Stt outer circle without
			   // hitting the Stt parallel inner section (for instance the track
			   flaggo=true;
			   if( (fabs(XcrossOut[0]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[0]-Ycross[0])<1.e-5 )
					||
				(fabs(XcrossOut[1]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[1]-Ycross[0])<1.e-5 )
				){
				flaggo=false;
			   }
		} else { // continuation of  if( flagOutStt ==0)

			// most usual case when track crossed the Inner parallel Stt.
			if (nnn == 0) return false;
		}  // end of  if( flagOutStt ==0)
	}  // end of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)



	if(flaggo){

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
			2.*DIAMETERSTRAWTUBE,	//  cut of proximity between hits.
			1,	// maximum allowed # consecutive hits with distance > cut.
			islack // uncertainty allowed as far as the n. of hits that should be present.
					)
	   ){

		return false;
	}

if(istampa>1) cout<<"uscito da BadTrack_ParStt.\n";

//-----------------------------------------------------
	}  // end of if(flaggo)
	} // end of if( flagInnerSttL == -1 && flagInnerSttR == -1 )
//outer: ;

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
				APOTEMAMINOUTERPARSTRAW,
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
				APOTEMAMINOUTERPARSTRAW,
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
			2.*DIAMETERSTRAWTUBE,	//  cut of proximity between hits.
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



//----------end of function PndTracking::SttParalCleanup




//----------begin of function PndTracking::SttSkewCleanup

bool PndTracking::SttSkewCleanup(
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
			Distance[MAXSTTHITS+1],
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
//		goto jampa;
	} else {



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

	} // end of if(nHits==0)
//jampa: ;
//	first of all, find possible intersection points with outer circle encompassing
//	the Stt system.

	flagOutStt = FindIntersectionsOuterCircle(
				Oxx,
				Oyy,
				Rr,
				RSTRAWDETECTORMAX,
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
		APOTEMAMINSKEWSTRAW,
		APOTEMAMAXSKEWSTRAW, // Apotema is the distance of a Hexagonal side from (0,0)
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
		APOTEMAMINSKEWSTRAW,
		APOTEMAMAXSKEWSTRAW, // Apotema is the distance of a Hexagonal side from (0,0)
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
	if( ninside < ((int) length/DIAMETERSTRAWTUBE )-islack ){
		if(istampa>1){
			cout<<"in SttSkewCleanup, n. Hits inside = "<<ninside
			<<" is < n. hits that should be inside at least = "
			<<((int)length/DIAMETERSTRAWTUBE)<<"-islack ("<<
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



//----------end of function PndTracking::SttSkewCleanup


//----------begin of function PndTracking::TrackCleanup

bool PndTracking::TrackCleanup(
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
			ListInnerHits[MAXSTTHITS],
			ListOuterHits[MAXSTTHITS];

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


//----------end of function PndTracking::TrackCleanup




//----------begin of function PndTrackFinderReal::TrkAssociatedParallelHitsToHelixQuater

UShort_t PndTracking::TrkAssociatedParallelHitsToHelixQuater(
	bool *ExclusionList,
	Double_t m,
	Double_t q,
	Short_t Status,
	UShort_t nHitsinTrack,
	UShort_t *ListHitsinTrack,
	UInt_t NhitsParallel,
	Double_t Ox,
	Double_t Oy,
	Double_t R,
	Double_t info[][7],
	Double_t infoparalConformal[][5],
	UShort_t *RConformalIndex,
	UShort_t *FiConformalIndex,
	UShort_t nBoxConformal[][NFIDIVCONFORMAL],
	UShort_t HitsinBoxConformal[][NRDIVCONFORMAL][NFIDIVCONFORMAL],
	UShort_t *auxListHitsinTrack
		)
{

 bool
	passamin,
	passamax,
	Unselected[MAXSTTHITS];

  Short_t i, i2, j, k, l,  l2, l3, itemp, kstart, kend,
          iFi0,FFimin, FFimax;
  UShort_t Nextra=8,
           nFi,
           Fi,
           nR,
           nAssociatedHits,
           nHit_original;
  Double_t maxFi,
           minFi,
           dist,
           xx,
           yy,
           aaa,
           angle,
           r,
           erre1,
           erre2,
           Rin,
           Rout,
           Fi0,
           ddd,
           fi1,
           fi2,
           dx,
           dy,
           distance,
           NTIMES=1.5;   //   number of Straw radia allowed in association

  nAssociatedHits=0;
  for(i=0; i<NhitsParallel;i++){
   Unselected[i]= true;
  }

//   find the range in Fi spanned  by the candidate track

  FFimin = 10000;
  FFimax = 0;
  for(j=0; j<nHitsinTrack; j++){
    i = (UShort_t)  infoparalConformal[ ListHitsinTrack[j] ][3];


    if( FiConformalIndex[i] <  FFimin ) FFimin = FiConformalIndex[i];
    if( FiConformalIndex[i] >  FFimax ) FFimax = FiConformalIndex[i];
  }


  if( FFimax > 3.*NFIDIVCONFORMAL/4. && FFimin < NFIDIVCONFORMAL/4.) {
     FFimin = 10000;
     FFimax =  0;
     for(j=0; j<nHitsinTrack; j++){
       i = (UShort_t)  infoparalConformal[ ListHitsinTrack[j] ][3];
       Fi = FiConformalIndex[i];
       if( Fi < NFIDIVCONFORMAL/4. ) Fi = FiConformalIndex[i]+NFIDIVCONFORMAL;
       if( Fi <  FFimin ) FFimin = Fi;
       if( Fi >  FFimax ) FFimax = Fi;
     }
  }


//  finding the boundaries in the Conformal plane. The basic assumption is that the range
// in Fi is much less that 180 degrees.
  
  FFimin -= (Short_t) NFIDIVCONFORMAL/Nextra;
  FFimax +=  (Short_t) NFIDIVCONFORMAL/Nextra;
if( FFimax - FFimin > NFIDIVCONFORMAL/2 ) {
    cout<<"something fishy is going on in PndSttTrkAssociatedParallelHitsToHelixQuater!"
      <<"Range in Fi (rad) is  "<<(FFimax - FFimin)*2.*PI/NFIDIVCONFORMAL<<endl;
    return 0;
}



//   use the equation of a line in polar coordinates
 
  if( Status ==99) {   //  case in which   0 = x + q

    if(fabs(q) > 1.e-10 ) {
      passamax=false;
      passamin=false;
      for(itemp=FFimin; itemp<=FFimax;itemp++){
        i=itemp;
        if( i< 0 ) {
          i += NFIDIVCONFORMAL;
        } else if (i>=NFIDIVCONFORMAL){
          i -=  NFIDIVCONFORMAL*( i/NFIDIVCONFORMAL );
        }
        angle = (i+0.5)*2.*PI/NFIDIVCONFORMAL;
        aaa = cos(angle);
        if( fabs(cos(angle)) <1.e-10)  continue;
        r = -q/aaa;
        if(r< radiaConf[0] || r>= 1./RSTRAWDETECTORMIN)  continue;
        for(j=nRdivConformalEffective-1; j>=0;j--){
          if( r>= radiaConf[j] ){
           nR = j;
           break;
          }
        }


        for(l=-DELTAnR; l<DELTAnR+1;l++){
          l2 = nR+l;
          if(  l2<0 || l2 >= nRdivConformalEffective )  continue;
              for( k=0;k<nBoxConformal[l2][i];k++){
                nHit_original = (UShort_t) infoparalConformal[HitsinBoxConformal[k][l2][i]][3];
		if( !InclusionListStt[ nHit_original ] ) continue;
// check if the hit position is near the circle of the Helix found by the fit
                dx = -Ox+info[ nHit_original ][0];
                dy = -Oy+info[ nHit_original ][1];
                distance = sqrt(dx*dx+dy*dy);
//cout<<"nuov, R "<<R<<",  distance  "<<distance<<endl;
                if ( fabs(R - distance ) > NTIMES*STRAWRADIUS )  continue;

//-------------------
                xx=infoparalConformal[  HitsinBoxConformal[k][l2][i]  ][0];
                dist = fabs( xx +q );
                if(  AcceptHitsConformal(  dist,
			infoparalConformal[HitsinBoxConformal[k][l2][i]][2],
                                                 infoparalConformal[  HitsinBoxConformal[k][l2][i]  ][4]
                                                      ) )  {
                    auxListHitsinTrack[nAssociatedHits]= HitsinBoxConformal[k][l2][i];
                    nAssociatedHits++; 
                }
              }	//  end of  for( k=0;k<nBoxConformal[l2][i];k++)
        }   //   end of for(l=-DELTAnR; l<DELTAnR+1;l++)

        //------- special cases
           if((nR == nRdivConformalEffective-1 && passamin && ! passamax) ||  (nR==0 && passamax && !passamin)  ) {   //  do the last two Fi columns
            if(nR == nRdivConformalEffective-1)  passamax=true;
            if(nR == 0)   passamin=true;

            for(l2=1;l2<3;l2++){
             i2 = i+l2;
             if(i2>=NFIDIVCONFORMAL) i2 -=  NFIDIVCONFORMAL;
            for(l=-2; l<3;l++){
             l3 = nR+l;
             if(  l3<0 || l3 >= nRdivConformalEffective )  continue;
              for( k=0;k<nBoxConformal[l3][i2];k++){
                nHit_original = (UShort_t) infoparalConformal[  HitsinBoxConformal[k][l3][i2]  ][3];
		if( !InclusionListStt[ nHit_original ] ) continue;
// check if the hit position is near the circle of the Helix found by the fit
                dx = -Ox+info[ nHit_original ][0];
                dy = -Oy+info[ nHit_original ][1];
                distance = sqrt(dx*dx+dy*dy);
                if ( fabs(R - distance ) > NTIMES*STRAWRADIUS )  continue;

//-------------------
                xx=infoparalConformal[  HitsinBoxConformal[k][l3][i2]  ][0];
                dist = fabs( xx +q );
//                if(dist < 3.*infoparalConformal[  HitsinBoxConformal[k][l3][i2]  ][2]){
                if(  AcceptHitsConformal(  dist,
                                                 infoparalConformal[  HitsinBoxConformal[k][l3][i2]  ][2],
                                                 infoparalConformal[  HitsinBoxConformal[k][l3][i2]  ][4]
                                                      ) )  {
                    auxListHitsinTrack[nAssociatedHits]= HitsinBoxConformal[k][l3][i2];
                    nAssociatedHits++; 
                }
              }	//  end of   for( k=0;k<nBoxConformal[l3][i2];k++)
            }   //   end of for(l=-2; l<3;l++)
            }   //   end of for(l2=0;l2<2;l2++)
            return  nAssociatedHits;
           } else if ((nR == nRdivConformalEffective-1 && ! passamin && !passamax) || (nR==0 && !passamax && !passamin)){
             if(nR == nRdivConformalEffective-1)  passamax=true;
             if(nR == 0)   passamin=true;

            for(l2=1;l2<3;l2++){
             i2 = i-l2;
             if(i2<NFIDIVCONFORMAL) i2 += NFIDIVCONFORMAL;
            for(l=-2; l<3;l++){
             l3 = nR+l;
             if(  l3<0 || l3 >= nRdivConformalEffective )  continue;
              for( k=0;k<nBoxConformal[l3][i2];k++){
                nHit_original = (UShort_t) infoparalConformal[  HitsinBoxConformal[k][l3][i2]  ][3];
		if( !InclusionListStt[ nHit_original ] ) continue;
// check if the hit position is near the circle of the Helix found by the fit
                dx = -Ox+info[ nHit_original ][0];
                dy = -Oy+info[ nHit_original ][1];
                distance = sqrt(dx*dx+dy*dy);
                if ( fabs(R - distance ) > NTIMES*STRAWRADIUS )  continue;

//-------------------
                xx=infoparalConformal[  HitsinBoxConformal[k][l3][i2]  ][0];
                dist = fabs( xx +q );
//                if(dist < 3.*infoparalConformal[  HitsinBoxConformal[k][l3][i2]  ][2]){
                if(  AcceptHitsConformal(  dist,
                                                 infoparalConformal[  HitsinBoxConformal[k][l3][i2]  ][2],
                                                 infoparalConformal[  HitsinBoxConformal[k][l3][i2]  ][4]
                                                      ) )  {
                    auxListHitsinTrack[nAssociatedHits]= HitsinBoxConformal[k][l3][i2];
                    nAssociatedHits++; 
                }
              }	//  end of  for( k=0;k<nBoxConformal[l3][i2];k++)
            }   //   end of for(l=-2; l<3;l++)
            }   //   end of for(l2=0;l2<2;l2++)
           }    //   end of if((nR == nRdivConformalEffective-1 && passamin) ||  (nR==0 && passamax)  )



      }   //  end of     for(itemp=Fimin; itemp<=FFimax;itemp++)


    } else {  //  q=0 --> x=0



      if( FFimax > NRDIVCONFORMAL/4 && Fimin < NRDIVCONFORMAL/4 ) {
        iFi0 =  (Short_t)  (NRDIVCONFORMAL/4 );
      } else if ( FFimax > 3*NRDIVCONFORMAL/4 && Fimin < 3*NRDIVCONFORMAL/4 ){
        iFi0 =  (Short_t)  (3*NRDIVCONFORMAL/4 );
      }  else {
                cout <<"From PndSttTrackFinderReal::PndSttTrkAssociatedParallelHitsToHelixQuater  :"
                      <<"  inconsistency, 0 associated hits to this track candidate\n";
        return 0;
     }

      for(itemp=iFi0-5; itemp<=iFi0+5;itemp++){
        i=itemp;
        if( i< 0 ) {
          i += NFIDIVCONFORMAL;
        } else if (i>=NFIDIVCONFORMAL){
          i -=  NFIDIVCONFORMAL*( i/NFIDIVCONFORMAL );
//         i -= NFIDIVCONFORMAL;
        }
        for(l=0; l<nRdivConformalEffective;l++){
              for( k=0;k<nBoxConformal[l][i];k++){
                nHit_original = (UShort_t) infoparalConformal[  HitsinBoxConformal[k][l][i]  ][3];
		if( !InclusionListStt[ nHit_original ] ) continue;
// check if the hit position is near the circle of the Helix found by the fit
                dx = -Ox+info[ nHit_original ][0];
                dy = -Oy+info[ nHit_original ][1];
                distance = sqrt(dx*dx+dy*dy);
                if ( fabs(R - distance ) >NTIMES*STRAWRADIUS )  continue;

//-------------------
                xx=infoparalConformal[  HitsinBoxConformal[k][l][i]  ][0];
                dist = fabs( xx  );

                if(  AcceptHitsConformal(  dist,
                                                 infoparalConformal[  HitsinBoxConformal[k][l][i]  ][2],
                                                 infoparalConformal[  HitsinBoxConformal[k][l][i]  ][4]
                                                      ) )  {
                    auxListHitsinTrack[nAssociatedHits]= HitsinBoxConformal[k][l][i];
                    nAssociatedHits++;
                }
              }  //  end of for( k=0;k<nBoxConformal[l][i];k++)
        }  //  end of for(l=0; l<nRdivConformalEffective;l++)
      }   //  end of for(itemp=iFi0-5; itemp<=iFi0+5;itemp++)




      }    //   end  of if(fabs(q) > 1.e-10 )


  }  else if( fabs(q)> 1.e-10) {   //   second part of    if( Status ==99),  in this case y = m*x +q


	Fi0 = atan(m);	// Fi0 belongs to (-PI/2, PI/2] .
	aaa = atan2(q, -m*q);
	if(Fi0<0.)  {
	 Fi0 += PI;
	 if (Fi0 <0. ) Fi0 =0.;// this is between 0. and PI.
	 if (Fi0 >PI ) Fi0 =PI;// this is between 0. and PI.
	};
	ddd= fabs(q)/sqrt(1.+m*m);


        for(itemp=FFimin; itemp<=FFimax;itemp++){
         i=itemp;
         if( i< 0 ) {
            i += NFIDIVCONFORMAL;
         } else if (i>=NFIDIVCONFORMAL){
            i -=  NFIDIVCONFORMAL*( i/NFIDIVCONFORMAL );
         }

	// erre1 is the distance from origin of point of intersection of the straight
	// line of equation  y= m*x+q  with  line of equation  y = x*tan(fi1);
	// when erre1 is < 0 it means the intersection is on the opposite side of the
	// versor defined by  [cos(fi1); sin(fi1)].
	// Here we are working in the conformal plane U,V.

         fi1 = i*2.*(PI/NFIDIVCONFORMAL);
         if( fabs(sin(fi1)-m*cos(fi1))>1.e-10) {
                 erre1 = q/(sin(fi1)-m*cos(fi1));
         } else {
                 erre1 = 99999999999.;
         }

         fi2 = (i+1)*2.*(PI/NFIDIVCONFORMAL);
         if( fabs(sin(fi2)-m*cos(fi2))>1.e-10) {
                 erre2 = q/(sin(fi2)-m*cos(fi2));
         } else {
                 erre2 = 99999999999.;
         }


         for(j=0; j<NRDIVCONFORMAL; j++){
              Rin = radiaConf[j];
              if(j!=NRDIVCONFORMAL-1) {
                 Rout =  radiaConf[j+1];
              }  else {
                 Rout = 1./RSTRAWDETECTORMIN;
              }

//  note that the following algorithm works also for negative erre1  and   erre2


              if(erre1<-1.e-10 ){
                if(erre2< 0. || erre2 > Rout ){
                     continue;
                }
              } else if(fabs(erre1) < 1.e-10){
                if( Fi0 > fi2 || Fi0 < fi1){
		  continue;
		}
              } else if ( erre1<Rin) {
                if( erre2< Rin &&  erre2> 0. ) {
		 continue;
		}
              }   else if (erre1> Rout  &&  erre2 > Rout && !( fi1<= aaa && aaa<=fi2 && ddd<=Rout )
                ) {
                   continue;
             }

              for(l=itemp-2; l<=itemp+2; l++){
                if( l< 0 ) {
                  l2 = l+NFIDIVCONFORMAL;
                } else if (l>=NFIDIVCONFORMAL){
                  l2 = l- NFIDIVCONFORMAL*( l/NFIDIVCONFORMAL );
               } else {
                 l2 = l;
               }
                if( j-1<0) {
                  kstart=0;
                }  else {
                  kstart = j-1;
                }
                if ( j+1 >=  NRDIVCONFORMAL ) {
                  kend = NRDIVCONFORMAL;
                } else {
                  kend = j+2;
                }

                for(k=kstart;k<kend;k++){

                 for( l3=0;l3<nBoxConformal[k][l2];l3++){
                  if( ! Unselected[HitsinBoxConformal[l3][k][l2] ] )  continue;
                   nHit_original = (UShort_t) infoparalConformal[  HitsinBoxConformal[l3][k][l2]  ][3];
		   if( !InclusionListStt[ nHit_original ] ) continue;
// check if the hit position is near the circle of the Helix found by the fit
                   dx = -Ox+info[ nHit_original ][0];
                   dy = -Oy+info[ nHit_original ][1];
                   distance = sqrt(dx*dx+dy*dy);
                   if ( fabs(R - distance ) > NTIMES*STRAWRADIUS )  continue;

//-------------------
                   xx=infoparalConformal[  HitsinBoxConformal[l3][k][l2]  ][0];
                   yy=infoparalConformal[  HitsinBoxConformal[l3][k][l2]  ][1];
                   dist = fabs( -yy+ m*xx +q )/sqrt(m*m+1.);
                   if(  AcceptHitsConformal(  dist,
                                                 infoparalConformal[  HitsinBoxConformal[l3][k][l2]  ][2],
                                                 infoparalConformal[  HitsinBoxConformal[l3][k][l2]  ][4]
                                                      ) )  {

                    auxListHitsinTrack[nAssociatedHits]= HitsinBoxConformal[l3][k][l2];
                    Unselected[HitsinBoxConformal[l3][k][l2]]= false;
                    nAssociatedHits++;
                   }

                 }   //   end of  for( l3=0;l3<nBoxConformal[k][l2];l3++)
                }   //   end of  for(k=kstart;k<kend;k++)
              }     //   end of  for(l=itemp-1; l<itemp+2; l++)

         }   //  end of for(j=0; j<NRDIVCONFORMAL; j++)

        }   //   end of    for(itemp=FFimin; itemp<=FFimax;itemp++)




  } else {  //  case in which    y= m*x ,  m can be zero    ,  third part of if( Status ==99)

      iFi0 =  (Short_t)  (atan(m)*NRDIVCONFORMAL/(2.*PI) );
      for(itemp=iFi0-5; itemp<=iFi0+5;itemp++){
         i=itemp;
         if( i< 0 ) {
          i += NFIDIVCONFORMAL;
         } else if (i>=NFIDIVCONFORMAL){
          i -=  NFIDIVCONFORMAL*( i/NFIDIVCONFORMAL );
//          i -= NFIDIVCONFORMAL;
         }
        for(l=0; l<nRdivConformalEffective;l++){
              for( k=0;k<nBoxConformal[l][i];k++){
                nHit_original = (UShort_t) infoparalConformal[  HitsinBoxConformal[k][l][i]  ][3];
		if( !InclusionListStt[ nHit_original ] ) continue;
// check if the hit position is near the circle of the Helix found by the fit
                dx = -Ox+info[ nHit_original ][0];
                dy = -Oy+info[ nHit_original ][1];
                distance = sqrt(dx*dx+dy*dy);
                if ( fabs(R - distance ) > NTIMES*STRAWRADIUS )  continue;

//-------------------
                xx=infoparalConformal[  HitsinBoxConformal[k][l][i]  ][0];
                yy=infoparalConformal[  HitsinBoxConformal[k][l][i]  ][1];
                dist = fabs( m*xx-yy  )/sqrt( m*m+1.);
                if(  AcceptHitsConformal(  dist,
                                                 infoparalConformal[  HitsinBoxConformal[k][l][i]  ][2],
                                                 infoparalConformal[  HitsinBoxConformal[k][l][i]  ][4]
                                                      ) )  {
                    auxListHitsinTrack[nAssociatedHits]= HitsinBoxConformal[k][l][i];
                    nAssociatedHits++;
                }
              }  //  end of for( k=0;k<nBoxConformal[l][i];k++)
        }
      }   //  end of for(itemp=FFimin; itemp<=FFimax;itemp++)





  }   //   end of if ( Status ==99)


if(istampa>=3) {
  cout<<"from TrkAssociatedParallelHitsToHelixQuater, before exiting; nAssociatedHits = "
   <<nAssociatedHits<<endl;
}



 return nAssociatedHits;

}



//----------end of function PndTrackFinderReal::TrkAssociatedParallelHitsToHelixQuater


//----------begin of function PndTracking::TrkAssociatedParallelHitsToHelix5

UShort_t PndTracking::TrkAssociatedParallelHitsToHelix5(
	bool *InclusionList,
	UInt_t NhitsParallel,
	Double_t Ox,
	Double_t Oy,
	Double_t R,
	Double_t info[][7],
	Double_t Fi_low,
	Double_t Fi_up,
	UShort_t *auxListHitsinTrack
	)
{

  Short_t i;

  UShort_t nAssociatedHits;

  Double_t angle,
           dx,
           dy,
           distance,
           NTIMES=5.;   //   number of Straw radia allowed in association.

  nAssociatedHits=0;
//   find the Hits belonging to this Track.



  for(i=0; i<NhitsParallel;i++){
	if( !InclusionList[ ListSttParHits[i] ] ) continue;
// check if the hit position is near the circle of the Helix found by the fit





	dx = -Ox+info[ListSttParHits[i]][0];
	dy = -Oy+info[ListSttParHits[i]][1];
	angle=atan2(dy,dx);
	if(angle<0.) angle += 2.*PI;
	if(angle<0.) angle =0.;
	distance = sqrt(dx*dx+dy*dy);
	if ( fabs(R - distance ) > NTIMES*STRAWRADIUS )  continue;
	if(angle<Fi_low) angle += 2.*PI;
	if(angle>Fi_up) continue;
	auxListHitsinTrack[nAssociatedHits]= i;
	nAssociatedHits++; 
  } // end for(i=0; i<NhitsParallel;i++)

 return nAssociatedHits;

}



//----------end of function PndTracking::TrkAssociatedParallelHitsToHelix5



//----------begin of function PndTracking::WriteMacroAllHitsRestanti

void PndTracking::WriteMacroAllHitsRestanti(
		UShort_t nSttHit,
		UShort_t nSttParHit,
		UShort_t nSttSkewHit,
		Double_t info[][7],
		bool *keepit,
		UShort_t nSttTrackCand,
		UShort_t nCandHit[MAXTRACKSPEREVENT],
		UShort_t ListCandHit[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK+
	                           MAXMVDPIXELHITSINTRACK+
				   MAXMVDSTRIPHITSINTRACK+MAXSCITILHITSINTRACK],
	Short_t ListCandHitType[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK+
	                           MAXMVDPIXELHITSINTRACK+
				   MAXMVDSTRIPHITSINTRACK+MAXSCITILHITSINTRACK]
					)
 {


//	nSttHit = parallel+skew.

	bool	exclusionStt[MAXSTTHITS],
		exclusionPixel[MAXMVDPIXELHITS],
		exclusionStrip[MAXMVDSTRIPHITS];

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


	disegnaAssiXY(MACRO,xmin,xmax,ymin,ymax);

//	disegna il BiHexagon destro e sinistro delle inner parallel straws.
	char myname[100];

	sprintf(myname,"InnerPar");
	DrawBiHexagonInMacro(
				VERTICALGAP,
				MACRO,
				RSTRAWDETECTORMIN,
				APOTEMAMAXINNERPARSTRAW,
				4,  // color code, 4= blue.
				myname
				);
//--------------
//	disegna il BiHexagon destro e sinistro delle skew straws.
	sprintf(myname,"Skew");
	DrawBiHexagonInMacro(
				VERTICALGAP,
				MACRO,
				APOTEMAMINSKEWSTRAW,
				APOTEMAMAXSKEWSTRAW,
				2,  // color code.
				myname
				);
//--------------
//	disegna il BiHexagon destro e sinistro delle skew straws.
	sprintf(myname,"OuterPar");
	DrawHexagonCircleInMacro(
				VERTICALGAP,
				MACRO,
				APOTEMAMINOUTERPARSTRAW,
				RSTRAWDETECTORMAX,
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


//----------end of function PndTracking::WriteMacroAllHitsRestanti



//----------start of function PndTracking::WriteMacroParallelHitsGeneral

void PndTracking::WriteMacroParallelHitsGeneral(
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

       xmax = xmax + delta*0.15;
       xmin = xmin - delta*0.15;

       ymax = ymax + delta*0.15;
       ymin = ymin - delta*0.15;


	if( xmin>-1.05*RSTRAWDETECTORMAX) xmin=-1.05*RSTRAWDETECTORMAX;
	if( ymin>-1.05*RSTRAWDETECTORMAX) ymin=-1.05*RSTRAWDETECTORMAX;
	if( xmax<1.05*RSTRAWDETECTORMAX) xmax=1.05*RSTRAWDETECTORMAX;
	if( ymax<1.05*RSTRAWDETECTORMAX) ymax=1.05*RSTRAWDETECTORMAX;

//	ymin=xmin=-1.05*RSTRAWDETECTORMAX;
//	ymax=xmax= 1.05*RSTRAWDETECTORMAX;


       fprintf(MACRO,"TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n",xmin,ymin,xmax,ymax);



	disegnaAssiXY(MACRO,xmin,xmax,ymin,ymax);

//	disegna il BiHexagon destro e sinistro delle inner parallel straws.
	char myname[100];

	sprintf(myname,"InnerPar");
	DrawBiHexagonInMacro(
				VERTICALGAP,
				MACRO,
				RSTRAWDETECTORMIN,
				APOTEMAMAXINNERPARSTRAW,
				4,  // color code, 4= blue.
				myname
				);
//--------------
//	disegna il BiHexagon destro e sinistro delle skew straws.
	sprintf(myname,"Skew");
	DrawBiHexagonInMacro(
				VERTICALGAP,
				MACRO,
				APOTEMAMINSKEWSTRAW,
				APOTEMAMAXSKEWSTRAW,
				2,  // color code.
				myname
				);
//--------------
//	disegna il BiHexagon destro e sinistro delle skew straws.
	sprintf(myname,"OuterPar");
	DrawHexagonCircleInMacro(
				VERTICALGAP,
				MACRO,
				APOTEMAMINOUTERPARSTRAW,
				RSTRAWDETECTORMAX,
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
		disegnaSciTilHit(
				MACRO,
				i,
				posizSciTil[i][0],
				posizSciTil[i][1],
				0	//  0 --> disegna in XY.
				);
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


	disegnaAssiXY(MACRO,xmin,xmax,ymin,ymax);

//	disegna il BiHexagon destro e sinistro delle inner parallel straws.
	sprintf(myname,"InnerPar");
	DrawBiHexagonInMacro(
				VERTICALGAP,
				MACRO,
				RSTRAWDETECTORMIN,
				APOTEMAMAXINNERPARSTRAW,
				4,  // color code, 4= blue.
				myname
				);
//--------------
//	disegna il BiHexagon destro e sinistro delle skew straws.
	sprintf(myname,"Skew");
	DrawBiHexagonInMacro(
				VERTICALGAP,
				MACRO,
				APOTEMAMINSKEWSTRAW,
				APOTEMAMAXSKEWSTRAW,
				2,  // color code.
				myname
				);
//--------------
//	disegna il BiHexagon destro e sinistro delle skew straws.
	sprintf(myname,"OuterPar");
	DrawHexagonCircleInMacro(
				VERTICALGAP,
				MACRO,
				APOTEMAMINOUTERPARSTRAW,
				RSTRAWDETECTORMAX,
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
		disegnaSciTilHit(
				MACRO,
				i,
				posizSciTil[i][0],
				posizSciTil[i][1],
				0
				);
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
		bool flaggo=true;
		for(j=0;j<90;j++){
			newalfa = alfa0 - carica*j*PI/45;
			newx = Cx + Rr*cos(newalfa);
			newy = Cy + Rr*sin(newalfa);
			if(newx > xmax || newx < xmin || newy>ymax||newy<ymin){
				ultimo = newalfa;
				if(primo > ultimo ) { primo = ultimo; ultimo = alfa0;};
				flaggo=false;
				break;
			}
		}
	if(flaggo){
		primo   = 0.;
		ultimo = 2.*PI;
	}

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


//----------end of function PndTracking::WriteMacroParallelHitsGeneral





//----------start of function PndTracking::WriteMacroParallelHitsGeneralspecial

void PndTracking::WriteMacroParallelHitsGeneralspecial(
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



	dR = time*STTDRIFTVEL; // in cm.

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

	disegnaAssiXY(MACRO,  xmin, xmax, ymin, ymax);

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


//----------end of function PndTracking::WriteMacroParallelHitsGeneralspecial





//----------start of function PndTracking::WriteMacroSkewAssociatedHitswithMC


void PndTracking::WriteMacroSkewAssociatedHitswithMC(
                   Double_t KAPPA,
                   Double_t FI0,
                   Double_t Ox,
                   Double_t Oy,
                   Double_t R,
		   Short_t charge,
                   Double_t info[][7],

                   Double_t WDX[MAXSTTHITS],
                   Double_t WDY[MAXSTTHITS],
                   Double_t WDZ[MAXSTTHITS],

                   UShort_t iTrack,
		    Int_t iNome, // questo e' per il nome delle Macro solamente.
                   UShort_t nSkewHitsinTrack,
UShort_t ListSkewHitsinTrack[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
                   UShort_t nSkewCommon,
                   UShort_t SkewCommonList[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
                   Short_t daTrackFoundaTrackMC,
                   UShort_t nMCSkewAlone,
                   UShort_t MCSkewAloneList[MAXMCTRACKS][MAXSTTHITSINTRACK],
		   UShort_t nPixelHitsinTrack[MAXTRACKSPEREVENT], // output
UShort_t ListPixelHitsinTrack[MAXTRACKSPEREVENT][MAXMVDPIXELHITSINTRACK], // output
		   UShort_t nStripHitsinTrack[MAXTRACKSPEREVENT], // output
UShort_t ListStripHitsinTrack[MAXTRACKSPEREVENT][MAXMVDSTRIPHITSINTRACK], // output



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
		j=ListSciTilHitsinTrack[iTrack][i];
		if( ESSE[i]>Smax ) Smax=ESSE[i];
		if( ESSE[i]<Smin ) Smin=ESSE[i];
		if( posizSciTil[j][2]>zmax ) zmax=posizSciTil[j][2];
		if( posizSciTil[j][2]<zmin ) zmin=posizSciTil[j][2];
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








  if( zmax >= zmin  &&  Smax >= Smin ) {



  aaa = Smax-Smin;
  Smin -= aaa*0.2;
  Smax += aaa*0.2;

  aaa = zmax-zmin;
  zmin -= aaa*0.05;
  zmax += aaa*0.05;

  if(Smax > 2.*PI) Smax = 2.*PI;
  if( Smin < 0.) Smin = 0.;


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
		j=ListSciTilHitsinTrack[iTrack][i];
		disegnaSciTilHit(
				MACRO,
				i,
				posizSciTil[j][2],
				ESSE[i]*R,
				1	// disegna in SZ.
				);
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
	bool flaggo=true;
        for( i1=0; i1<nSkewCommon; i1++){
          if ( SkewCommonList[   iTrack   ][i1] == i ){

		flaggo=false;
		break;
          }

        }
	if(flaggo) fprintf(MACRO,"Skew%d_%d->SetLineColor(2);\n",i,ii);
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
 

		bool flaggo=true;
		for( int k=0; k<nMvdPixelCommon;k++){
			if( MvdPixelCommonList[k]== ii){
				fprintf(MACRO,
		"TMarker* CommonPixel%d = new TMarker(%f,%f,%d);\nCommonPixel%d->SetMarkerColor(1);\n",
				ii,ZMvdPixel[ii],R*esse,26,ii);
		fprintf(MACRO,"CommonPixel%d->Draw();\n",ii);
				flaggo=false;
				break;
			}
		}
		if(flaggo){
            fprintf(MACRO,"TMarker* SpuriousPixel%d = new TMarker(%f,%f,%d);\nSpuriousPixel%d->SetMarkerColor(2);\n",
                    ii,ZMvdPixel[ii],R*esse,26,ii);
		fprintf(MACRO,"SpuriousPixel%d->Draw();\n",ii);
		}
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
 
		bool flaggo=true;
		for( int k=0; k<nMvdStripCommon;k++){
			if( MvdStripCommonList[k]== ii){
            fprintf(MACRO,"TMarker* CommonStrip%d = new TMarker(%f,%f,%d);\nCommonStrip%d->SetMarkerColor(1);\n",
                    ii,ZMvdStrip[ii],R*esse,25,ii);
		fprintf(MACRO,"CommonStrip%d->Draw();\n",ii);
				flaggo=false;
				break;
			}
		}
		if(flaggo){
            fprintf(MACRO,"TMarker* SpuriousStrip%d = new TMarker(%f,%f,%d);\nSpuriousStrip%d->SetMarkerColor(2);\n",
                    ii,ZMvdStrip[ii],R*esse,25,ii);
		fprintf(MACRO,"SpuriousStrip%d->Draw();\n",ii);
		}


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
                    ii,ZMvdStrip[ii],R*esse,25,ii);
		fprintf(MACRO,"AloneStrip%d->Draw();\n",ii);

  }




//  plot della traccia trovata dal finder


	zmin2=zmin;
	zmax2=zmax;

	bool flaggo=true;
	if( -KAPPA*charge>0.) {	// Pz>0.
		if( zmax <0.) {
			cout<<"da WriteMacroSkewAssociatedHitswithMC, questa traccia"
			<<" e' inconsistente col proprio Pz, non plottata!\n";
			flaggo=false;
		} else {
			zmin = 0.;
		}
	} else {  // Pz<0.
		if( zmin >0.) {
			cout<<"da WriteMacroSkewAssociatedHitswithMC, questa traccia"
			<<" e' inconsistente col proprio Pz, non plottata!\n";
			flaggo=false;
		} else{
			zmax = 0.;
		}

	}

 if(flaggo){

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
   } else {
	for(i=Nmin; i<= Nmax;i++){
		offset = 2.*PI*i;
		z1 = (i*2.*PI-FI0)/KAPPA;
		z2 = ((i+1)*2.*PI-FI0)/KAPPA;
		fprintf(MACRO,
"TLine* FOUND%d = new TLine(%f,%f,%f,%f);\nFOUND%d->SetLineColor(2);\nFOUND%d->Draw();\n",
			i-Nmin,z1,0.,z2, R*2.*PI,i-Nmin,i-Nmin);

	}   //  end of  for(i=Nmin; i<= Nmax;++)
  } // end of if(fabs(KAPPA)<1.e-10)

 }  // end of if(flaggo)


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
  if (fabs(carica)>=0.1 ){
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
                 imc,i-Nmin,z1,0.,z2,R* 2.*PI,imc,i-Nmin,imc,i-Nmin);
  }   //  end of  for(i=Nmin; i<= Nmax;++)

   }  // end of if (fabs(carica)>=0.1 )
	}  // end of if ( pMC )
       }  // end of if( imc>-1 )



  } // end of  if( zmax >= zmin  &&  Smax >= Smin )

      fprintf(MACRO,"}\n");
      fclose(MACRO);




 }


//----------end of function PndTracking::WriteMacroSkewAssociatedHitswithMC



//--start of function PndTracking::WriteMacroSttParallelAssociatedHitsandMvdwithMC

void PndTracking::WriteMacroSttParallelAssociatedHitsandMvdwithMC(
                   Double_t Ox,Double_t Oy,Double_t R,
		   Double_t primoangolo,
		   Double_t ultimoangolo,
                   UShort_t Nhits,
UShort_t ListHitsinTrack[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
                   Double_t info[][7],
                   UShort_t iTrack,
		Int_t iNome,
		Short_t daSttTrackaMCTrack,
		UShort_t nParalCommon[MAXTRACKSPEREVENT],
		UShort_t ParalCommonList[MAXMCTRACKS][MAXSTTHITSINTRACK],
		UShort_t nSpuriParinTrack[MAXTRACKSPEREVENT],
		UShort_t ParSpuriList[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],

		UShort_t nMCParalAlone[MAXTRACKSPEREVENT],
		UShort_t MCParalAloneList[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],

	UShort_t nMvdPixelHitsAssociatedToSttTra,
UShort_t ListPixelHitsinTrack[MAXTRACKSPEREVENT][MAXMVDPIXELHITSINTRACK],
	UShort_t nMvdStripHitsAssociatedToSttTra,
UShort_t ListStripHitsinTrack[MAXTRACKSPEREVENT][MAXMVDSTRIPHITSINTRACK],

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
UShort_t ListSkewHitsinTrack[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
		Double_t *SchosenSkew,
		UShort_t nSkewCommon[MAXTRACKSPEREVENT],
		UShort_t SkewCommonList[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
		UShort_t nMCSkewAlone[MAXTRACKSPEREVENT],
		UShort_t MCSkewAloneList[MAXMCTRACKS][MAXSTTHITSINTRACK]
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

       xmax = xmax + delta*0.15;
       xmin = xmin - delta*0.15;

       ymax = ymax + delta*0.15;
       ymin = ymin - delta*0.15;


       fprintf(MACRO,"TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n",xmin,ymin,xmax,ymax);


//	disegna il BiHexagon destro e sinistro delle inner parallel straws.

	char myname[100];

	sprintf(myname, "InnerPar");
	DrawBiHexagonInMacro(
				VERTICALGAP,
				MACRO,
				RSTRAWDETECTORMIN,
				APOTEMAMAXINNERPARSTRAW,
				4,  // color code, 4= blue.
				myname
				);
//--------------
//	disegna il BiHexagon destro e sinistro delle skew straws.
	sprintf(myname, "Skew");
	DrawBiHexagonInMacro(
				VERTICALGAP,
				MACRO,
				APOTEMAMINSKEWSTRAW,
				APOTEMAMAXSKEWSTRAW,
				2,  // color code.
				myname
				);
//--------------
//	disegna il BiHexagon destro e sinistro delle skew straws.
	sprintf(myname, "OuterPar");
	DrawHexagonCircleInMacro(
				VERTICALGAP,
				MACRO,
				APOTEMAMINOUTERPARSTRAW,
				RSTRAWDETECTORMAX,
				4,  // color code.
				myname
				);
//--------------

//---- disegna gli Scitil.

	for( ii=0; ii< nSciTilHitsinTrack[iTrack]; ii++) {
		i = ListSciTilHitsinTrack[iTrack][ii] ;
		disegnaSciTilHit(
				MACRO,
				i,
				posizSciTil[i][0],
				posizSciTil[i][1],
				0
				);

	}
//------------------------
       fprintf(MACRO,"TEllipse* FoundTrack = new TEllipse(%f,%f,%f,%f,%f,%f);\n"
			,Ox,Oy,R,R,primoangolo,ultimoangolo);

       fprintf(MACRO,
       "FoundTrack->SetLineColor(2);\nFoundTrack->SetFillStyle(0);\nFoundTrack->Draw(\"only\");\n");


	disegnaAssiXY(MACRO,xmin,xmax,ymin,ymax);
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

		bool flaggo = true;
		for( int k=0; k<nSkewCommon[iTrack];k++){
			if( SkewCommonList[iTrack][k]== ii){
	fprintf(MACRO,"TMarker* CommonSkewHit%d = new TMarker(%f,%f,%d);\n",
				ii,aaa,bbb,28);
	fprintf(MACRO,"CommonSkewHit%d->SetMarkerColor(1);\nCommonSkewHit%d->Draw();\n"
				,ii,ii);
				flaggo = false;
				break;
			}
		}
		if(flaggo){
		fprintf(MACRO,"TMarker* SpurSkewHit%d = new TMarker(%f,%f,%d);\n",
				ii,aaa,bbb,28);
		fprintf(MACRO,"SpurSkewHit%d->SetMarkerColor(2);\nSpurSkewHit%d->Draw();\n",ii,ii);
		}

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

		bool flaggo=true;
		for( int k=0; k<nMvdStripCommon;k++){
			if( MvdStripCommonList[k]== ii){
            fprintf(MACRO,"TMarker* CommonMvdStrip%d = new TMarker(%f,%f,%d);\n",
                    ii,XMvdStrip[ii],YMvdStrip[ii],25);
		fprintf(MACRO,"CommonMvdStrip%d->SetMarkerColor(1);\nCommonMvdStrip%d->Draw();\n",
                    ii,ii);
				flaggo=false;
				break;
			}
		}
		if(flaggo){
            fprintf(MACRO,"TMarker* SpurMvdStrip%d = new TMarker(%f,%f,%d);\n",
                    ii,XMvdStrip[ii],YMvdStrip[ii],25);
		fprintf(MACRO,"SpurMvdStrip%d->SetMarkerColor(2);\nSpurMvdStrip%d->Draw();\n",ii,ii);
		}


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

		bool flaggo=true;
		for( int k=0; k<nMvdPixelCommon;k++){
			if( MvdPixelCommonList[k]== ii){
            fprintf(MACRO,"TMarker* CommonMvdPixel%d = new TMarker(%f,%f,%d);\n",
                    ii,XMvdPixel[ii],YMvdPixel[ii],26);
 fprintf(MACRO,"CommonMvdPixel%d->SetMarkerColor(1);\nCommonMvdPixel%d->Draw();\n",
                    ii,ii);
				flaggo=false;
				break;
			}
		}
		if(flaggo){
            fprintf(MACRO,"TMarker* SpurMvdPixel%d = new TMarker(%f,%f,%d);\n",
                    ii,XMvdPixel[ii],YMvdPixel[ii],26);
 fprintf(MACRO,"SpurMvdPixel%d->SetMarkerColor(2);\nSpurMvdPixel%d->Draw();\n",ii,ii);
		}


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
	  if (fabs(carica)>=0.1 ){
           	Cx = Oxx + Py*1000./(BFIELD*CVEL*carica);
           	Cy = Oyy - Px*1000./(BFIELD*CVEL*carica);
            	fprintf(MACRO,
"TEllipse* MC%d = new TEllipse(%f,%f,%f,%f,%f,%f);\nMC%d->SetFillStyle(0);\nMC%d->SetLineColor(3);\nMC%d->Draw(\"only\");\n",
                     im,Cx,Cy,Rr,Rr,0.,360.,im,im,im);
	  } // end of  if (fabs(carica)>=0.1 )
	} // end if ( pMC )
       };//  end of  if( daSttTrackaMCTrack>-1 
//----------- fine parte del MC
      fprintf(MACRO,"}\n");
      fclose(MACRO);


    return ;

}


//--end of function PndTracking::WriteMacroSttParallelAssociatedHitsandMvdwithMC













ClassImp(PndTracking)
