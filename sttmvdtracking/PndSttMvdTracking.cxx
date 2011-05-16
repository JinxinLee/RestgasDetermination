#include "glpk.h"

#include "PndSttMvdTracking.h"

#include "PndSttHit.h"
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

/*
  hx = new TH1F("hx", "x: mc - reco", 100, -1, 1);

  hzresvsslope = new TH2F("hzresvsslope", "z: mc - reco vs slope", 100, -3.5, 3.5, 100, -3., 3.);

*/

  hdeltaRPixel = new TH1F("hdeltaRPixel", "distance MC Pixel point from trajectory in XY plane", 100, -1, 1);
  hdeltaRStrip = new TH1F("hdeltaRStrip", "distance MC Strip point from trajectory in XY plane", 100, -1, 1);
  hdeltaRPixel2 = new TH1F(
  "hdeltaRPixel2", "distance MC point from trajectory in XY plane (Pixels)", 100, -10, 10);
  hdeltaRStrip2 = new TH1F(
  "hdeltaRStrip2", "distance MC point from trajectory in XY plane (Strips)", 100, -10,10);



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

  cout << "-I- PndSttMvdTracking: Initialization successfull" << endl;
  








  return kSUCCESS;

}

// -------------------------------------------------------------------------

// CHECK added 
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


 bool	Status,
	status[MAXTRACKSPEREVENT],
	SttSZfit[MAXTRACKSPEREVENT],
	Mvdhits[MAXTRACKSPEREVENT];


 Short_t Candidato,
         Charge,
	 statusflag[MAXTRACKSPEREVENT],
         iflag,
         iHit,
	 i,
	 j,
	 k,
	 resultFitSZagain[MAXTRACKSPEREVENT],
	 tubeID
         ;

  Short_t  daTrackFoundaTrackMC[MAXTRACKSPEREVENT],
	   CHARGE[MAXTRACKSPEREVENT];

  UShort_t l,
	   kall;

  UShort_t  FromHitToMCTrack[nmaxSttHits],
           nMCParalAlone[MAXTRACKSPEREVENT],
           nMCSkewAlone[MAXTRACKSPEREVENT],
           MCParalAloneList[MAXTRACKSPEREVENT][nmaxSttHits],
           MCSkewAloneList[MAXTRACKSPEREVENT][nmaxSttHits],
	nHitsInMCTrack[MAXTRACKSPEREVENT],
	nSkewHitsInMCTrack[MAXTRACKSPEREVENT],
           nParalCommon[MAXTRACKSPEREVENT],
           ParalCommonList[MAXTRACKSPEREVENT][nmaxSttHits],
           nSpuriParinTrack[MAXTRACKSPEREVENT],
           ParSpuriList[MAXTRACKSPEREVENT][nmaxSttHits],
           nSkewCommon[MAXTRACKSPEREVENT],
           SkewCommonList[MAXTRACKSPEREVENT][nmaxSttHits],
           nSpuriSkewinTrack[MAXTRACKSPEREVENT],
           SkewSpuriList[MAXTRACKSPEREVENT][nmaxSttHits],
	   TemporarySkewList[2*nmaxSttHits][2];

  UShort_t
	   nTotalCandidates,
	   nMvdMCPoint,
	   nSttHit,
	   nSttParHit,
	   nSttSkewHit,
	   nSttHelixTrack,
	   nSttMCPoint,
	   nSttTrackCand,
	   TemporarynSkewHitsinTrack,
	   ListAllParHits[nmaxSttHits],
	   ListAllSkewHits[nmaxSttHits],
	   nSttHitsinTrack[MAXTRACKSPEREVENT],
	   ListSttHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits],
	   ListSttHitsinTrackType[MAXTRACKSPEREVENT][nmaxSttHits]
//	   ListSttParHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits],
//	   ListSttSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits]
		;
  UShort_t	ipinco,
		ncand,
		n,
		nalone,
		nnoise;


  Int_t	nrounds0,
	nrounds1,
	ipunto;

  Double_t Dist,
	   Distance,
	   Fi_final_helix_referenceframe,
           HoughFi[MAXTRACKSPEREVENT],
	   Ntras,
           Phi,
	   Ptras,
           Rad,
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
	   highqualitycut,
	   Pxini,
	   px,
	   Pyini,
	   py,
	   Pzini,
	   qop,
	   x,
	   y,
	   s[2],
	   versor[2],
	   z[2],
	   zeta0,
	   zeta1,
	   zdrift[2],
	   zerror[2],
	   primoangolo[MAXTRACKSPEREVENT],
	   ultimoangolo[MAXTRACKSPEREVENT],
	   AloneX[nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack],
	   AloneY[nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack];
;

  Double_t ALFA[MAXTRACKSPEREVENT],
	   BETA[MAXTRACKSPEREVENT],
	   GAMMA[MAXTRACKSPEREVENT],
	   KAPPA[MAXTRACKSPEREVENT],
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
	   S[2*nmaxSttHits+nmaxMvdPixelHits+nmaxMvdStripHits], // multiplication by 2 in the
	   ZED[2*nmaxSttHits+nmaxMvdPixelHits+nmaxMvdStripHits], // rather improbable chance that
	   DriftRadius[2*nmaxSttHits+nmaxMvdPixelHits+nmaxMvdStripHits],// all skew hits have double
	   ErrorDriftRadius[2*nmaxSttHits+nmaxMvdPixelHits+nmaxMvdStripHits],// solutions
	   Sbis[2*nmaxSttHits+nmaxMvdPixelHits+nmaxMvdStripHits][2], // multiplication by 2 in the
	   ZEDbis[2*nmaxSttHits+nmaxMvdPixelHits+nmaxMvdStripHits][2], // rather improbable chance that
	   DriftRadiusbis[2*nmaxSttHits+nmaxMvdPixelHits+nmaxMvdStripHits][2],// all skew hits have double
	   ErrorDriftRadiusbis[2*nmaxSttHits+nmaxMvdPixelHits+nmaxMvdStripHits][2],// solutions
	   SchosenPixel[MAXTRACKSPEREVENT][nmaxMvdPixelHits],
	   SchosenStrip[MAXTRACKSPEREVENT][nmaxMvdStripHits],
	   SchosenSkew[MAXTRACKSPEREVENT][nmaxSttHits], // NO multiplication by 2 here because for the
	   				// skew hits only one
	   				// solution is selected.
	   ZchosenPixel[MAXTRACKSPEREVENT][nmaxMvdPixelHits],
	   ZchosenStrip[MAXTRACKSPEREVENT][nmaxMvdStripHits],
	   ZchosenSkew[MAXTRACKSPEREVENT][nmaxSttHits],
	   ErrorchosenPixel[MAXTRACKSPEREVENT][nmaxMvdPixelHits],
	   ErrorchosenStrip[MAXTRACKSPEREVENT][nmaxMvdStripHits],
	   ErrorchosenSkew[MAXTRACKSPEREVENT][nmaxSttHits],
	   info[nmaxSttHits][7],
	   TemporaryS[2*nmaxSttHits],
	   TemporaryZ[2*nmaxSttHits],
	   TemporaryZDrift[2*nmaxSttHits],
	   TemporaryZErrorafterTilt[2*nmaxSttHits];

  TVector3   Momentum,ErrMomentum,Position,ErrPosition;


  FairMCPoint *  pSttMCPoint[nmaxSttHits];

  PndTrackCand * pMvdTrackCand,
               * pSttTrackCand;

  PndSttTrack * pSttHelixTrack;

  PndSttTube * pSttTube;

  PndSttHit * pSttHit;

  PndSdsHit * pMvdPixelHit,
            * pMvdStripHit;
  PndSdsMCPoint * pMvdMCPoint;

  PndMCTrack* pMCtr;

  PndTrackCandHit pndtrackcandhit; 

  fSttMvdPndTrackCandArray->Delete();

  fSttMvdPndTrackArray->Delete();

  IVOLTE++;



//----------   fetching the MC truth tracks
	nMCTracks = fMCTrackArray->GetEntriesFast(); // num. tracce/evento
	if (istampa >= 1  && IVOLTE<20) {
	     cout<<"da PndSttMvdTracking  : evento (partendo da 0)  N. "<<
	       IVOLTE<< "\n       N. di MC truth tracks  : "<<nMCTracks<<endl;
	}
	if (nMCTracks ==0){
		cout<<"da PndSttMvdTracking  :  N. di MC truth tracks = 0, return!\n"<<endl;
		return;
	}
//------------------------------

//----------   fetching the STT  MC points
 nSttMCPoint = fSttPointArray->GetEntriesFast();
 if (nSttMCPoint ==0){
	cout<<"da PndSttMvdTracking  :  N. di Stt MC points = 0\n"<<endl;
//	return;
 }

 if(istampa>=1&& IVOLTE<20) cout<<"da PndSttMvdTracking  : n. punti MC = "<<nSttMCPoint<<endl;
 	for( i= 0; i< nSttMCPoint; i++){
	pSttMCPoint[i] = (FairMCPoint*) fSttPointArray->At(i);
 }

//------------------------------




//----------   fetching the STT  hits

 nSttHit = fSttHitArray->GetEntriesFast();
 if (nSttHit ==0){
	cout<<"da PndSttMvdTracking  :  N. di Stt Hits = 0, return!\n"<<endl;
	return;
 } else if (nSttHit>= nmaxSttHits) {
	cout<<"da PndSttMvdTracking  :  N. di Stt Hits = "<<nSttHit
	     <<" and it is >= nmaxSttHits (="<<nmaxSttHits<<"), return!\n"<<endl;
	return;
 }

 if (istampa >= 1  && IVOLTE<20) {
     cout<<"da PndSttMvdTracking  : evento (partendo da 0)  N. "<<
       IVOLTE<< "\n       N. totale Hits in STT  : "<<nSttHit<<endl;
 }
// ---------------------------------------------  estraggo le info da MVD

 nMvdPixelHit = fMvdPixelHitArray->GetEntriesFast();
 nMvdStripHit = fMvdStripHitArray->GetEntriesFast();
 if (nMvdPixelHit+nMvdStripHit ==0){
	cout<<"da PndSttMvdTracking  :  N. of MvdPixelHit=nMvdStripHit=0, return!\n"<<endl;
	return;
 } else if (nMvdPixelHit>= nmaxMvdPixelHits) {
	cout<<"da PndSttMvdTracking  :  N. of MvdPixelHit = "<<nMvdPixelHit
	<<" and it is >= nmaxMvdPixelHits ("<<nmaxMvdPixelHits<<"),  return!\n";
	return;
 } else if (nMvdStripHit>= nmaxMvdStripHits) {
	cout<<"da PndSttMvdTracking  :  N. of MvdStripHit = "<<nMvdStripHit
	<<" and it is >= nmaxMvdStripHits ("<<nmaxMvdStripHits<<"),  return!\n";
	return;
 }

// ---------------------------------------------  get MC Points of  MVD

   nMvdMCPoint = fMvdMCPointArray->GetEntriesFast();
// if (nMvdMCPoint ==0){
//	cout<<"da PndSttMvdTracking  :  N. di MvdMCPoint =0, return!\n"<<endl;
//	return;
// }
   if(istampa>2&& IVOLTE<20) cout<<"N. MC Points delle Mvd = "<<nMvdMCPoint<<endl;

// ------------------------------------------ estraggo le altre info della trackcand  del MVD

  nMvdTrackCand = fMvdTrackCandArray->GetEntriesFast();

/*
 if (nMvdTrackCand ==0){
	cout<<"da PndSttMvdTracking  :  N. of MvdTrackCand =0, return!\n"<<endl;
	return;
 } else 
*/

 if (nMvdTrackCand>= MAXMVDTRACKSPEREVENT) {
	cout<<"da PndSttMvdTracking  :  N. of MvdTrackCand = "<<
	nMvdTrackCand<<" and it is >= MAXMVDTRACKSPEREVENT (="<<MAXMVDTRACKSPEREVENT
	<<"),  return!\n";
	return;
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
 if(istampa>=3  && IVOLTE<20){ cout<<"N. totale di PndTrackCand del PR solo = "
                              <<nSttTrackCand<<endl; }



//------------------------------------------------------------------


 nSttParHit=0; 
 nSttSkewHit=0;
 for( i= 0; i< nSttHit; i++){
	ExclusionListStt[i]= true ;


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
	info[i][6]= pSttMCPoint[ipunto]->GetTrackID();
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
  if (istampa >= 3) {
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
      cout <<"             hit X, Y, Z space position "   << pSttMCPoint[ipunto]->GetX() << " " <<
                       pSttMCPoint[ipunto]->GetY() << " " << pSttMCPoint[ipunto]->GetZ()<<endl; 
      cout <<"             hit wire pos. in middle "   << pSttTube->GetPosition().X() << " " <<
             pSttTube->GetPosition().Y() << " " << pSttTube->GetPosition().Z() 
           << "; R = "<<sqrt(pSttTube->GetPosition().X()*pSttTube->GetPosition().X()+
	   pSttTube->GetPosition().Y()*pSttTube->GetPosition().Y())<<
	   ", suo drift radius = "<<dradius <<endl;
      cout <<"             wire direction, X, Y, Z (Z direction set always positive)"
      << WDX[i]<<"  "<<WDY[i]<<"  "<<WDZ[i] <<endl
           <<"             this hit belongs to MC track n. "<<pSttMCPoint[ipunto]->GetTrackID()<<endl;
      }
  }  //  end of   if(istampa >= 

//--------  fine stampaggi



  }  //   end of for( i= 0; i< nSttHit; i++)





//	fill the exclusion list for those straws with multiple hits


      //   first the parallel straws
      for(i=0; i< nSttHit-1; i++){
	if( !ExclusionListStt[ i ] ) continue;
			for(j=i+1; j< nSttHit; j++){
				if(ExclusionListStt[ j ] &&
					fabs(info[i][0] - info[j][0])<1.e-20 &&
					fabs(info[i][1] - info[j][1])<1.e-20  )
				{
					ExclusionListStt[j]= false ;
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
if(istampa>=2  && IVOLTE<20){
  cout<<"da PndSttMvdTraking  :  n. Mvd Pixel Hits = "<<nMvdPixelHit<<endl;
  cout<<"da PndSttMvdTracking  :  n. Mvd Strip Hits = "<<nMvdStripHit<<endl;
}
if(istampa>=2  && IVOLTE<20){
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



// ---------------------------------------------  get MC Points of  MVD

   Short_t	MCPointtoMCTrackID[nMvdMCPoint];
   Double_t	XMvdMCPoint[nMvdMCPoint],
		YMvdMCPoint[nMvdMCPoint],
		ZMvdMCPoint[nMvdMCPoint];

   for(  i= 0; i< nMvdMCPoint; i++){
	pMvdMCPoint = (PndSdsMCPoint*) fMvdMCPointArray->At(i);
if(istampa>2&& IVOLTE<20) cout<<"Il punto MC n. "<<i<<" Mvd e' associato alla traccia MC n. "
	   <<pMvdMCPoint->GetTrackID()<<endl;
	TVector3 position;
	pMvdMCPoint->Position(position);
	XMvdMCPoint[i]=position.X();
	YMvdMCPoint[i]=position.Y();
	ZMvdMCPoint[i]=position.Z();
	MCPointtoMCTrackID[i]= pMvdMCPoint->GetTrackID();

   }
// ----------------------------------------------------------------------







// ------------------------------------------ estraggo le altre info della trackcand  del MVD

  bool inMvdTrackCandPixel[nMvdPixelHit],
       inMvdTrackCandStrip[nMvdStripHit];


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
     if(nHitMvdTrackCand[i]>= nmaxMvdPixelHitsInTrack + nmaxMvdStripHitsInTrack){
	cout<<"from PndSttMvdTracking : # Mvd hits of this Mvd Track Cand is >= than the maximum allowed ("
		<<nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack<<"); skipping this candidate !\n";
	continue;
     }


if(istampa>=3  && IVOLTE<20) 
   cout<<"Evento n. "<<IVOLTE<<", info da MvdTrackCand n. "<<i<<"  ---------------------------------------\n"<<
       "\n      direzione X traccia = "<<dirSeed.X()<<
       "\n      direzione Y traccia = "<<dirSeed.Y()<<
       "\n      direzione Z traccia = "<<dirSeed.Z()<<
       "\n      Q over P traccia = "<<qop<<
       "\nn. hits in questa MvdTrackCand = "<<nHitMvdTrackCand[i]<<endl<<"    e loro elenco :\n";

     for(j=0; j<nHitMvdTrackCand[i]; j++){
       pndtrackcandhit = pMvdTrackCand->GetSortedHit(j);
       ListHitMvdTrackCand[i][j] = pndtrackcandhit.GetHitId(); // questo e' il n. Hit nativo che posso usare
                                          // per estrarre tutte le info che voglio. Se il n. e' -1
					  // dopo non lo considero; chiedere a Tobias cos'e' perche'
					  // anche il suo detID e' -1 : ne' Pixel ne' Strip...!
       ListHitTypeMvdTrackCand[i][j] = pndtrackcandhit.GetDetId(); // questo in realta' e' il Branch dello
					  // Hit che viene usato - stupidamente - per dire che e'
					  // un Pixel. Roba da matti.
                                          // Se e' -1 dovrebbe essere noise ma a in quale Pixel
					  // o Strip?? Mistero.
	if( ListHitTypeMvdTrackCand[i][j]==
			FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)){
		inMvdTrackCandPixel[ ListHitMvdTrackCand[i][j] ]= true;

	} else if( ListHitTypeMvdTrackCand[i][j]==
			FairRootManager::Instance()->GetBranchId(fMvdStripBranch)){
		inMvdTrackCandStrip[ ListHitMvdTrackCand[i][j] ]= true;
	}

if(istampa>=3  && IVOLTE<20){  cout<<"\thit n. "<<ListHitMvdTrackCand[i][j]
	<<" e suo Detector Id "<<ListHitTypeMvdTrackCand[i][j]
	<<" (Pixel DetId = "<<FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)<<", Strip DetId = "
	<<FairRootManager::Instance()->GetBranchId(fMvdStripBranch)
	<<")"<<endl;}


     }    //   end of    for(j=0; j<nHitMvdTrackCand; j++)
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
			ListMvdUSPixelHitNotTrackCand[nMvdDSPixelHitNotTrackCand] = i;
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

 if(istampa>2){
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

 for(  i= 0; i< nSttTrackCand; i++){
// ----  estraggo gli hits appartenenti a questa TrackCand
  pSttTrackCand = (PndTrackCand *) fSttTrackCandArray->At(i);
  nSttHitsinTrack[i] = pSttTrackCand->GetNHits();  // n. hits in questa track cand


    //  for the calculation of the approximate Fi of the first hit in thos track
    pndtrackcandhit = pSttTrackCand->GetSortedHit(0);
    x = info[ pndtrackcandhit.GetHitId() ][0];  //  this is in the middle of the tube
    y = info[ pndtrackcandhit.GetHitId() ][1];  //  this is in the middle of the tube

  for(j=0,nSttParHitsinTrack[i]=0,nSttSkewHitsinTrack[i]=0; j<nSttHitsinTrack[i]; j++){
    pndtrackcandhit = pSttTrackCand->GetSortedHit(j);

    ListSttHitsinTrack[i][j] = pndtrackcandhit.GetHitId(); // # hit of Stt

    if ( fabs(info[ pndtrackcandhit.GetHitId() ][5]- 1.) < 0.0001) {
      ListSttHitsinTrackType[i][j] = 2; 
      ListSttParHitsinTrack[i][nSttParHitsinTrack[i]] = pndtrackcandhit.GetHitId(); // # hit of Stt
      nSttParHitsinTrack[i]++;
    }  else {
      ListSttHitsinTrackType[i][j] = 3; 
      ListSttSkewHitsinTrack[i][nSttSkewHitsinTrack[i]] = pndtrackcandhit.GetHitId(); // # hit of Stt
      nSttSkewHitsinTrack[i]++;
    }

  }    //   end of    for(j=0; j<nSttHitsinTrack[i]; j++)






// --- estraggo le altre info della TrackCand

	TVector3 dirSeed=pSttTrackCand->getDirSeed();
	TVector3 posSeed=pSttTrackCand->getPosSeed();
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
	    Ox[i] = -R[i]*dirSeed.Y()/Ntras;
	    Oy[i] = R[i]*dirSeed.X()/Ntras;
	    KAPPA[i]=0.003*BFIELD*fabs(qop)/dirSeed.Z();
	    CHARGE[i]=-1;
	  } else {
	    Ox[i] = R[i]*dirSeed.Y()/Ntras;
	    Oy[i] = -R[i]*dirSeed.X()/Ntras;
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

	Fifirst[i] = atan2( y-Oy[i], x-Ox[i]);
	FI0[i] = atan2(-Oy[i], -Ox[i]);
	HoughFi[i] = FI0[i] + PI;
	if(HoughFi[i]<0.) HoughFi[i]=0.;
	if( FI0[i] < 0. )  FI0[i]+= 2.*PI; if( FI0[i] < 0. ) FI0[i]=0.;
	if( Fifirst[i] < 0. )  Fifirst[i]+= 2.*PI;if( Fifirst[i] < 0. )  Fifirst[i]=0.;



 }            //   end of   for(  i= 0; i< nSttTrackCand; i++)

//---------------  end fetching of the Stt PndTrackCand from PR of the STT


//---------------  Plotting the distance between the Mvd Strip and Pixel points and the trajectory
//                 found by pattern recognition

 for(  i= 0; i< nSttTrackCand; i++){
	for( j= 0; j< nMvdPixelHit; j++){
		dis = sqrt( pow(XMvdPixel[j]-Ox[i],2)+pow(YMvdPixel[j]-Oy[i],2)) - R[i];
		hdeltaRPixel->Fill(dis);
		hdeltaRPixel2->Fill(dis);
	}
	for( j= 0; j< nMvdStripHit; j++){
		dis = sqrt( pow(XMvdStrip[j]-Ox[i],2)+pow(YMvdStrip[j]-Oy[i],2)) - R[i];
		hdeltaRStrip->Fill(dis);
		hdeltaRStrip2->Fill(dis);
	}
 }            //   end of   for(  i= 0; i< nSttTrackCand; i++)



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

//----   find the angular range (in Fi) allowed for the STT hits, with the present Ox,Oy and R
//	of the track candidates, and for the Mvd hits (FI0 and Fi_low_limit ).

 for(  ncand= 0; ncand< nSttTrackCand; ncand++){
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
	else if (statusflag[ncand] == -2){ Fi_low_limit[ncand] = -100000.; };

 }	// end of for(  ncand= 0; ncand< nSttTrackCand; ncand++)

//---------------------   here call to the function that matches Mvd hits with Stt tracks
   delta=0.5; //  parameter of proximity for associating Mvd hits to Stt tracks
   highqualitycut=0.2; //  parameter of proximity for associating Mvd hits to Stt tracks
   MatchMvdHitsToSttTracks2(
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


if(istampa>2&& IVOLTE<20){
           cout<<"da PndSttMvdTracking ;  n. SttTrackCand totali = "<<nSttTrackCand
	       <<"--------------------------------------\n";
      for(  i= 0; i< nSttTrackCand; i++){
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
      }// end of   for(  i= 0; i< nSttTrackCand; i++)


}   //end of if(istampa>=0)

//-----------------  end of section with match Mvd hits with Stt hits



//----------------------------------------------------------
//     ordering all the hits belonging to the candidate track, by increasing R;
//     forming the new track with Mvd+Stt hits

	nTotalCandidates = nSttTrackCand;  // nSttTrackCand is already <= MAXTRACKSPEREVENT.
//	nTotalCandidates =
//		nSttTrackCand< MAXTRACKSPEREVENT ? nSttTrackCand  : MAXTRACKSPEREVENT;

	for(ncand=0; ncand< nTotalCandidates; ncand++){
		nTrackCandHit[ncand] =nSttHitsinTrack[ncand]+
					nMvdPixelHitsinTrack[ncand]+
					nMvdStripHitsinTrack[ncand];
		UShort_t tempmvdindex[nMvdPixelHitsinTrack[ncand]+
					nMvdStripHitsinTrack[ncand] ],
			 tempmvdtype[nMvdPixelHitsinTrack[ncand]+
					nMvdStripHitsinTrack[ncand] ],
			auxIndex[nMvdPixelHitsinTrack[ncand]+
					nMvdStripHitsinTrack[ncand] ];
		Double_t auxR[nMvdPixelHitsinTrack[ncand]+
					nMvdStripHitsinTrack[ncand] ];
		// adding the Mvd hits (Pixel and Strips)
		for(i=0; i< nMvdPixelHitsinTrack[ncand]; i++){
			auxR[i] = sqrt(
			 XMvdPixel[ ListMvdPixelHitsinTrack[ncand][i] ]*
			 XMvdPixel[ ListMvdPixelHitsinTrack[ncand][i] ]+
			 YMvdPixel[ ListMvdPixelHitsinTrack[ncand][i] ]*
			 YMvdPixel[ ListMvdPixelHitsinTrack[ncand][i] ]
					    );
			tempmvdindex[i]=ListMvdPixelHitsinTrack[ncand][i];
			tempmvdtype[i]=0;
			auxIndex[i] = i;
		}
		for(i=0; i< nMvdStripHitsinTrack[ncand]; i++){
			auxR[i+nMvdPixelHitsinTrack[ncand]] = sqrt(
			 XMvdStrip[ ListMvdStripHitsinTrack[ncand][i] ]*
			 XMvdStrip[ ListMvdStripHitsinTrack[ncand][i] ]+
			 YMvdStrip[ ListMvdStripHitsinTrack[ncand][i] ]*
			 YMvdStrip[ ListMvdStripHitsinTrack[ncand][i] ]
					    );
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

		//  constructing the ordered new Track  Candidate now
			for(i=0; i< nMvdPixelHitsinTrack[ncand]+
			    nMvdStripHitsinTrack[ncand]; i++){
				ListTrackCandHit[ncand][i] = tempmvdindex[ auxIndex[i] ];
				ListTrackCandHitType[ncand][i] = tempmvdtype[ auxIndex[i] ];

			}
		}	// end of  if( nMvdPixelHitsinTrack[ncand]+

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



//---------------------   refit the Helix in XY plane using Stt + Mvd associated hits


	Short_t	iexcl;
	Double_t	d,
			diff,
			rotationangle,
			trajectory_vertex[2];

	for(ncand=0; ncand< nTotalCandidates; ncand++){

		if( nMvdPixelHitsinTrack[ncand]+
		    nMvdStripHitsinTrack[ncand] ==0){ // in this case  don't refit
			ALFA[ncand]=-2.*Ox[ncand];
			BETA[ncand]=-2.*Oy[ncand];
			GAMMA[ncand]= Ox[ncand]*Ox[ncand]+Oy[ncand]*Oy[ncand]-R[ncand]*R[ncand];
			Mvdhits[ncand]=false;
			if(istampa>=2) cout<<"da PndSttMvdTracking, IVOLTE = "<<
					IVOLTE<<", ncand = "
					<<ncand<<
					" non rifittato perche' non ha Mvd hits associati;"
					<<endl;
//			status[ncand]=true;	//  because it is still a good track!
			continue;
		}

		Mvdhits[ncand]=true;
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
			FI0[ncand] = atan2(-Oy[ncand], -Ox[ncand]);
			if( FI0[ncand] < 0. )  FI0[ncand]+= 2.*PI;
		}


if(istampa>2) {
cout<<"PndSttMvdTracking, prima di MatchMvdHitsToSttTracksagain, IVOLTE = "<<
IVOLTE<<", ncand "<<ncand<<endl;
for(int iiii=0;iiii<nMvdPixelHitsinTrack[ncand];iiii++)
{
	cout<<"\tpixel hit n. "<<
	ListMvdPixelHitsinTrack[ncand][iiii]<<endl;
}
for(int iiii=0;iiii<nMvdStripHitsinTrack[ncand];iiii++)
{
	cout<<"\tStrip hit n. "<<
	ListMvdStripHitsinTrack[ncand][iiii]<<endl;
}
}

	//   finding the FI angular range (in the laboratory frame) spanned by this parallel track
	//   taking into account the Rmax and Rmin of the straw detector and under the
	//   hypothesis that the track originates at (0,0).

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
					// Fi_low_limit and Fi_up_limit are undefined
					// when statusflag is negative.
		&statusflag[ncand],
		RStrawDetectorMin,
		RStrawDetectorMax
						);
	// Fi_low_limit set to  -100000. when circle does not intersect
	// Mvd nor Stt; it is set to  -99999. when it is contained in Mvd
	// region completely.
	if( statusflag[ncand] == -1) { Fi_low_limit[ncand] = -99999.; }
	else if (statusflag[ncand] == -2){ Fi_low_limit[ncand] = -100000.; };


	}	// end of for(ncand=0; ncand< nTotalCandidates; ncand++)

//--------------------- end of  refit the Helix in XY plane using Stt + Mvd associated hits


//---------------------   here call to the function that matches Mvd hits with the new
//  circular trajectory in XY found  for the second time, after first refit

   delta=0.5; //  parameter of proximity for associating Mvd hits to Stt tracks
//   highqualitycut=0.3; //  parameter of proximity for associating Mvd hits to Stt tracks
   highqualitycut=0.5; //  parameter of proximity for associating Mvd hits to Stt tracks


   MatchMvdHitsToSttTracksagain(
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


//---------------------   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// use the risult just obtained from the fit in XY to redo the association of the Skew Straw hits

  for(ncand=0; ncand< nTotalCandidates; ncand++)
  {
	if( ! Mvdhits[ncand]) continue;

	if( statusflag[ncand] == -2 ) continue; // this is when the XY circle is external to
							// the STT region (it should never happen in
							// principle at this point of the code).
	if( statusflag[ncand] == -1 ) continue; // this is when the XY circle is contained in the
							// the Mvd region completely.

	Fi_final_helix_referenceframe = atan2(
		info[ ListSttParHitsinTrack[ncand][ nSttParHitsinTrack[ncand]-1 ]-1 ][1]-Oy[ncand],
		info[ ListSttParHitsinTrack[ncand][ nSttParHitsinTrack[ncand]-1 ]-1 ][0]-Ox[ncand]
						);
	if( Fi_final_helix_referenceframe < 0. )  Fi_final_helix_referenceframe += 2.*PI;
	if( Fi_final_helix_referenceframe < FI0[ncand] )  Fi_final_helix_referenceframe += 2.*PI;
	if( Fi_final_helix_referenceframe < FI0[ncand] )  Fi_final_helix_referenceframe = FI0[ncand];


	TemporarynSkewHitsinTrack = AssociateSkewHitsToXYTrack(
                   ExclusionListStt,
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
                   FI0[ncand],	// Fi of the vertex of track (0,0) in the Helix XY frame.
                   Fi_final_helix_referenceframe, // Fi of last parallel Stt hit of track, in the Helix XY frame.
                   TemporarySkewList, // output,  list of selected skew hits (in skew numbering)
                   TemporaryS,       //  output,  S coordinate of selected Skew hit
                   TemporaryZ,       //  output,  Z coordinate of selected Skew hit (center wire)
                   TemporaryZDrift,   //  output,  drift distance IN Z DIRECTION only, of selected Skew hit
                   TemporaryZErrorafterTilt   //  output,  Radius taking into account the tilt, IN Z DIRECTION only, of selected Skew hit
                                                     );
	nSttSkewHitsinTrack[ncand]=TemporarynSkewHitsinTrack;   // it can be also zero!
	for(j=0;j<TemporarynSkewHitsinTrack;j++)
	{
		ListSttSkewHitsinTrack[ncand][j]=TemporarySkewList[j][0];
	}
if(istampa>2) {
cout<<"PndSttMvdTracking, dopo di AssociateSkewHitsToXYTrack, IVOLTE = "<<
IVOLTE<<", ncand "<<ncand<<", FI0 = "<< FI0[ncand] <<"\n\tFI finale = "
	<<Fi_final_helix_referenceframe
	<<", Ox = "<< Ox[ncand]<< ", Oy = "<<Oy[ncand] <<", R = "<<R[ncand]<<endl;
for(int iiii=0;iiii<nMvdPixelHitsinTrack[ncand];iiii++)
{
	cout<<"\tpixel hit n. "<<
	ListMvdPixelHitsinTrack[ncand][iiii]<<endl;
}
for(int iiii=0;iiii<nMvdStripHitsinTrack[ncand];iiii++)
{
	cout<<"\tStrip hit n. "<<
	ListMvdStripHitsinTrack[ncand][iiii]<<endl;
}
for(int iiii=0;iiii<nSttSkewHitsinTrack[ncand];iiii++)
{
	cout<<"\tSkew straw hit n. "<<
	ListSttSkewHitsinTrack[ncand][iiii]<<endl;
}
}
  }	// end of for(ncand=0; ncand< nTotalCandidates; ncand++)


//-------------------------------------------  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



//---------------------   here calculate the S and Z values of Mvd Pixels, Mvd Strips and Stt Skew hits.

    for(ncand=0; ncand< nTotalCandidates; ncand++){
		// the Mvd Pixels hit
		for(i=0; i< nMvdPixelHitsinTrack[ncand]; i++){
			k=ListMvdPixelHitsinTrack[ncand][i];
			ZEDbis[i][0] = ZED[i] = ZMvdPixel[k];
			S[i] = atan2( YMvdPixel[k]-Oy[ncand],XMvdPixel[k]-Ox[ncand]);
			if(S[i]<0.) S[i] +=2.*PI;
			Sbis[i][0] = S[i];
			DriftRadiusbis[i][0]=DriftRadius[i]=-1.;
			ErrorDriftRadiusbis[i][0]=ErrorDriftRadius[i]= 0.01 ;
		}
		// the Mvd Strips hit
		for(j=0; j< nMvdStripHitsinTrack[ncand]; j++){
			i = nMvdPixelHitsinTrack[ncand]+j;
			k=ListMvdStripHitsinTrack[ncand][j];
			ZEDbis[i][0] = ZED[i] = ZMvdStrip[k];
			S[i] = atan2( YMvdStrip[k]-Oy[ncand],XMvdStrip[k]-Ox[ncand]);
			if(S[i]<0.) S[i] +=2.*PI;
			Sbis[i][0] = S[i] ;
			DriftRadiusbis[i][0]=DriftRadius[i]=-1.;
			ErrorDriftRadiusbis[i][0]=ErrorDriftRadius[i]= 0.01 ;
		}
		// the Skew Stt hits
		for(j=0, i = nMvdPixelHitsinTrack[ncand]+
				nMvdStripHitsinTrack[ncand];
				j<nSttSkewHitsinTrack[ncand]; j++){
			k=ListSttSkewHitsinTrack[ncand][j];
			kall = nMvdPixelHitsinTrack[ncand]+
				nMvdStripHitsinTrack[ncand]+j;
			CalculateSandZ( Ox[ncand],
					Oy[ncand],
					R[ncand],
					k,
					info,
					WDX,
					WDY,
					WDZ,
					s,	// output, alway between 0 and 2*PI
					z,	// Zcoordinate of the central wire.
					zdrift, //  drift radius projected onto the Helix
					zerror  // STRAWRESOLUTION (0.015 cm) projected onto the Helix.
					);
				if( z[0]<999998. && z[1]<999998.){
					ZEDbis[kall][0]=ZED[i]=z[0];
					Sbis[kall][0]=S[i] = s[0];
					DriftRadiusbis[kall][0]=DriftRadius[i]=zdrift[0];
					if( fabs(zdrift[0]) >1.e-10) {
					 ErrorDriftRadiusbis[kall][0]=ErrorDriftRadius[i]=zerror[0];
					} else {
					 ErrorDriftRadiusbis[kall][0]=ErrorDriftRadius[i]=0.5;
					}
					i++;

					ZEDbis[kall][1]=ZED[i]=z[1];
					Sbis[kall][1]=S[i] = s[1];
					DriftRadiusbis[kall][1]=DriftRadius[i]=zdrift[1];
					if( fabs(zdrift[1]) >1.e-10) {
					 ErrorDriftRadiusbis[kall][1]=ErrorDriftRadius[i]=zerror[1];
					} else {
					 ErrorDriftRadiusbis[kall][1]=ErrorDriftRadius[i]=0.5;
					}
					i++;

					if(!Mvdhits[ncand])
					// in this case calculate also ZchosenSkew and SchosenSkew
					{
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
					} // end of   if(Mvdhits[ncand])



				}else if( z[0]<999998.){

					ZEDbis[kall][0]=ZED[i]=z[0];
					Sbis[kall][0]=S[i] = s[0];
					DriftRadiusbis[kall][0]=DriftRadius[i]=zdrift[0];
					if( fabs(zdrift[0]) >1.e-10) {
					 ErrorDriftRadiusbis[kall][0]=ErrorDriftRadius[i]=zerror[0];
					} else {
					 ErrorDriftRadiusbis[kall][0]=ErrorDriftRadius[i]=0.5;
					}
					ZEDbis[kall][1]=999999.;
					i++;
					if(!Mvdhits[ncand])
					// in this case calculate also ZchosenSkew and SchosenSkew
					{
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
					}// end of  if(Mvdhits[ncand])

				} else if( z[1]<999998.){
					ZEDbis[kall][1]=ZED[i]=z[1];
					Sbis[kall][1]=S[i] = s[1];
					DriftRadiusbis[kall][1]=DriftRadius[i]=zdrift[1];
					if( fabs(zdrift[1]) >1.e-10) {
					 ErrorDriftRadiusbis[kall][1]=ErrorDriftRadius[i]=zerror[1];
					} else {
					 ErrorDriftRadiusbis[kall][1]=ErrorDriftRadius[i]=0.5;
					}
					ZEDbis[kall][0]=999999.;
					i++;

					if(!Mvdhits[ncand])
					{
					// in this case calculate also ZchosenSkew and SchosenSkew
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
					}	// end of  if(Mvdhits[ncand])



				} else {
					ZEDbis[kall][1]=ZEDbis[kall][0]=999999.;
				}


		}	//   end of   for(j=0, i = nMvdPixelHitsinTrack[ncand]+


			j=nMvdPixelHitsinTrack[ncand]+
			nMvdStripHitsinTrack[ncand];


//  finding if there are discontinuity at 0 for fi value of the Mvd Hit.
//  In case of discontinuity at 0, add 2*PI to fi of those hits with fi in the 1st quadrant.
//  This is necessary because the discontinuities would make the fit
//  in the SZ plane fail.
//  In this discontinuity fixing, the value FI0 of the vertex (0,0) is also included.
//  If there is discontinuity fixing, the values of S[i] AND POSSIBLY Fi_initial_helix_referenceframe[i]
//  might be modified (+2.*PI) from  now on.

	if(Mvdhits[ncand]){	//	in this case there is at least 1 Mvd hits associated to Stt track.


		if( j ==1){
			nSttSkewHitsinTrack[ncand]<5 ? j += nSttSkewHitsinTrack[ncand] :   j += 5; 
		} else if (j==2){
			// trick when 2 Mvd hit are very discordant
			double dot = ZED[0]*ZED[1] + (s[0]-FI0[0])*(s[1]-FI0[0]);
			double modulo0 = ZED[0]*ZED[0] + (s[0]-FI0[0])*(s[0]-FI0[0]);
			double modulo1 = ZED[1]*ZED[1] + (s[1]-FI0[0])*(s[1]-FI0[0]);
			if( modulo0 > 1.e-20 && modulo1 > 1.e-20 ){
				dot /= (sqrt(modulo1*modulo0));
				if(dot < 0.866 ) {
					nSttSkewHitsinTrack[ncand]<5?j +=nSttSkewHitsinTrack[ncand]:j += 5;
				}
			}

		}



		FixDiscontinuitiesFiangleinSZplane(
			j,	//  n hits in track. 
			S,	// S can be modified by +-2*PI if necessary.
			&FI0[ncand],	// this remains unchanged.
			CHARGE[ncand]	// this remains unchanged.
					);

//---------------------   here do the fit again in the SZ space if there are Mvd hits.
//			   For this, reordering of the  Mvd hits is not necessary.


		resultFitSZagain[ncand] = FitSZspace(
					j,	// n. hits to be fitted
					S,
					ZED,
					DriftRadius,
					ErrorDriftRadius,
					FI0[ncand],
					20,	// maximum number allowed in the fit
					&emme
						);

		if( resultFitSZagain[ncand]==1) KAPPA[ncand] = emme;



//-------------------------------------------

//	use the risult just obtained from the fit in SZ to reject the spurious Skew Straw hits
//	and the Mvd spurious hits; also in this function there is the calculation of the Z position
//	of the SKEW hits and the MVD hits, for a given track candidate (ie for a given Helix
//	circle in the XY plane)


	EliminateSpuriousSZ(
				&nMvdPixelHitsinTrack[ncand],	// input and output
				&ListMvdPixelHitsinTrack[ncand][0],// input and output
				&nMvdStripHitsinTrack[ncand],	// input and output
				&ListMvdStripHitsinTrack[ncand][0],// input and output
				&nSttSkewHitsinTrack[ncand],	// input and output
				&ListSttSkewHitsinTrack[ncand][0],// input and output
				Sbis,	// input
				ZEDbis,	// input
				DriftRadiusbis,	// input
				ErrorDriftRadiusbis,	// input
				&SchosenPixel[ncand][0],
				&SchosenStrip[ncand][0],
				&SchosenSkew[ncand][0],
				&ZchosenPixel[ncand][0],
				&ZchosenStrip[ncand][0],
				&ZchosenSkew[ncand][0],
				&ErrorchosenPixel[ncand][0],
				&ErrorchosenStrip[ncand][0],
				&ErrorchosenSkew[ncand][0],
				KAPPA[ncand],
				FI0[ncand],
				R[ncand]
				    );

//------------------------
	}	// end of   if(Mvdhits[ncand])




    }	//  end of for(ncand=0; ncand< nTotalCandidates; ncand++)



//---  redo association of parallel Stt  straw  hits to this track, after better refit.
	CollectParSttHitsagain(
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

//	ordering all the hits belonging to the candidate track, by increasing R;
//	from candidate n. 0 to candidate n. nTotalCandidates-1; loading ListTrackCandHit.

	Ordering_Loading_ListTrackCandHit(0,nTotalCandidates,info);


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//   this section collects more tracks starting from the remaining Mvd hits
//   not associated to any Stt track.



	if( nTotalCandidates == MAXTRACKSPEREVENT ) goto dontdoit ; // if there are already
								// too many candidates
								// don't do anything.


//  load the UsedPixel and UsedStrip  vectors.
	bool	UsedPixel[nMvdPixelHit],
		UsedStrip[nMvdStripHit];

	UShort_t List[nMvdPixelHit+nMvdStripHit],
		 ListType[nMvdPixelHit+nMvdStripHit];

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
		nalone=0;
		nnoise=0;

		for(j=0;j<nHitMvdTrackCand[i];j++){
		  if(ListHitTypeMvdTrackCand[i][j]<0){ nnoise++; continue;} // -1 hits, noise.

		  if(ListHitTypeMvdTrackCand[i][j] == 
			FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)){
		   if(UsedPixel[ ListHitMvdTrackCand[i][j] ])   continue;
		   AloneX[nalone] = XMvdPixel[ ListHitMvdTrackCand[i][j] ];
		   AloneY[nalone] = YMvdPixel[ ListHitMvdTrackCand[i][j] ];
		   List[nalone] = ListHitMvdTrackCand[i][j];
		   ListType[nalone] = 0;
		   nalone++;
		  }else if (ListHitTypeMvdTrackCand[i][j] ==	// this condition in principle at this
								// point is always satisfied.
			FairRootManager::Instance()->GetBranchId(fMvdStripBranch)){
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
			if( status[nTotalCandidates] ){
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
				Mvdhits[nTotalCandidates]=true;
				nTotalCandidates++;
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
			if(status[nTotalCandidates]){
				Ox[nTotalCandidates] = -ALFA[nTotalCandidates]/2.;
				Oy[nTotalCandidates] = -BETA[nTotalCandidates]/2.;
				R[nTotalCandidates] =
					Ox[nTotalCandidates]*Ox[nTotalCandidates]
					+Oy[nTotalCandidates]*Oy[nTotalCandidates]
					-GAMMA[nTotalCandidates];
				if( R[nTotalCandidates] > 0. ) R[nTotalCandidates]=
					sqrt(R[nTotalCandidates]) ;
				else  R[nTotalCandidates]=0.;
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



				Mvdhits[nTotalCandidates]=true;
				nTotalCandidates++;


			} // end of  if(status[nTotalCandidates])

		} // end of  if( nalone == 3 )






	} //end of  for(i=0; i<nMvdTrackCand; i++)


//     ordering all the hits belonging to the new candidate tracks, by increasing R;
//     loading    ListTrackCandHit. The ordering is necessary here because the charge
//     has to be calculated.

	Ordering_Loading_ListTrackCandHit(nSttTrackCand,nTotalCandidates,info);

//-------------------- end of ordering

// -----------------  finding the charge of those track found starting from Mvd

	bool flag;
	int nMvdOnly;
	Double_t Sini, Slast;


	for(ncand=nSttTrackCand; ncand< nTotalCandidates; ncand++){
	    Double_t esse[nTrackCandHit[ncand]];

	// S[]  is used later in the SZ fit, therefore it contains only info of
	// the Mvd and Skew Straw;
	// esse[] is used for the CHARGE determination and therefore it contains
	// also info of Parallel Straws (the last hit is in general a parallel straw hit).

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
			DriftRadius[nMvdOnly]= -1. ;
			ErrorDriftRadius[nMvdOnly]= 0.01 ;
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
			DriftRadius[nMvdOnly]= -1. ;
			ErrorDriftRadius[nMvdOnly]= 0.01 ;
			nMvdOnly++;
//		} else { // Parallel and Skew Straws.
//			esse[i] = atan2(info[ListTrackCandHit[ncand][i]][1]-Oy[ncand],
//					info[ListTrackCandHit[ncand][i]][0]-Ox[ncand]
//					);
//			if(esse[i]<0.) esse[i] +=2.*PI;
//			if(esse[i]<0.) esse[i] =0.;
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
		&statusflag[ncand],
		RStrawDetectorMin,
		RStrawDetectorMax
						);

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
					&emme
						);
		if( resultFitSZagain[ncand]==1) KAPPA[ncand] = emme;

	} //   end of  for(ncand=nSttTrackCand; ncand< nTotalCandidates; ncand++)

// ----------------------------- end fit in SZ with the Mvd tracks




//---  try to associate of parallel Stt  straw  hits to this new tracks.


	CollectParSttHitsagain(
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


	Fi_final_helix_referenceframe = atan2(
		info[ ListSttParHitsinTrack[ncand][ nSttParHitsinTrack[ncand]-1 ]-1 ][1]-Oy[ncand],
		info[ ListSttParHitsinTrack[ncand][ nSttParHitsinTrack[ncand]-1 ]-1 ][0]-Ox[ncand]
						);
	if( Fi_final_helix_referenceframe < 0. )  Fi_final_helix_referenceframe += 2.*PI;
	if( Fi_final_helix_referenceframe < FI0[ncand] )  Fi_final_helix_referenceframe += 2.*PI;
	if( Fi_final_helix_referenceframe < FI0[ncand] )  Fi_final_helix_referenceframe = FI0[ncand];

	if( statusflag[ncand]>=0) {// when statusflag[ncand]<0 the track does not intersect Stt region.
	   nSttSkewHitsinTrack[ncand] = AssociateSkewHitsToXYTrack(
		ExclusionListStt,
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
		FI0[ncand],	// Fi of the vertex of track (0,0) in the Helix XY frame.
		Fi_final_helix_referenceframe, // Fi of last parallel Stt hit of track,
				// in the Helix XY frame. Used to check if a skew hit 'pushes
				// out' the last parallel Stt hit.
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
	   }

	}  // end of if( statusflag[ncand]>=0)

  //  ------------------- end of attachment of skew hits to the new tracks.


   } // end of   for(ncand=nSttTrackCand; ncand< nTotalCandidates; ncand++)



//     ordering again all the hits belonging to the new candidate tracks, by increasing R;
//     loading    ListTrackCandHit.

	Ordering_Loading_ListTrackCandHit(nSttTrackCand,nTotalCandidates,info);

//-------------------- end of ordering



dontdoit:  ;	//  this is the label where the computer jumps if there are already
		//  MAXTRACKSPEREVENT candidates.



// ----------------------------- end of finding Tracks starting with the Mvd tracks
// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------




//----------stampaggi
if(istampa>=2){
cout<<"Evt. n. "<<IVOLTE<<", n. Track Cand totali = "<<nTotalCandidates<<endl;

for(int it=0; it<nTotalCandidates;it++){
cout<<"-------------------\n";
cout<<"Evt. n. "<<IVOLTE<<",  Track Cand n. "<<it;
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

cout<<"\tand now the list of hits as it is in ListTrackCandHit :\n";
for(l =0;l<nSttSkewHitsinTrack[it]+nSttParHitsinTrack[it]+nMvdStripHitsinTrack[it]
	+nMvdPixelHitsinTrack[it];l++){
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

   Short_t	FromPixeltoMCTrack[nMvdPixelHit],
		FromStriptoMCTrack[nMvdStripHit];

  if( doMcComparison){
	MvdMatchtoMC(
		nMvdMCPoint,
		XMvdMCPoint,
		YMvdMCPoint,
		ZMvdMCPoint,
		MCPointtoMCTrackID,
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
		cout<<"da PndSttMvdTracking : Track candidate n. "<<i
		<<"  associated to MC Track n. "<<daTrackFoundaTrackMC[i]<<endl;
	}
}

//  the following method counts the matched to MC and spurious hits for all STT  Found tracks and
//  also the ghost tracks
   SttMatchedSpurious(
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
			nTotalCandidates,			// input
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
     for(ncand=0; ncand< nTotalCandidates; ncand++){
//----------------- ora la traccia MC corrispondente a questa traccia Stt
		cout<<"\n\nda PndSttMvdTracking : la PndTrackCand n. "<<ncand;
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
		cout<<"Track Cand n. "<<ncand<<", associato a Traccia MC n. "<<
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


		cout<<"inoltre, sempre Track Cand n. "<<ncand<<", associato a Traccia MC n. "<<
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
		nMCTracksaccettabili++;
	}


}


//----------- fine conteggio delle tracce MC accettabili






//  if(istampa>=1 )  fprintf(HANDLE, "\n Evento %d  NTotaleTracceMC %d ------\n",IVOLTE, nMCTracks);
  if(istampa>=3 ) {

   fprintf(HANDLE, "\n Evento %d  NTotaleTracceMC %d ------\n",IVOLTE,
  	nMCTracksaccettabili);
}
int ii;
Double_t HoughFiii;

for (ii=0; ii<nSttTrackCand && istampa>=3 ;ii++){
   fprintf(HANDLE,"----------------------------------------------------------\n");
   i=daTrackFoundaTrackMC[ii];

   if( i <0  ) {
    fprintf(HANDLE,
"   No TracciaMC associated to found track n. %d in pattern recognition, with %d Hits ||, %d skew hits, %f Radius \n "
             ,ii,nSttParHitsinTrack[ii], nSttSkewHitsinTrack[ii],  R[ii] );
            continue;
         }
   if( ( !SttSZfit[ii] )&&(resultFitSZagain[ii] !=1 )) {
    fprintf(HANDLE,
"       TracciaMC %d; sua FoundTrack associata (n. %d) NONsoddisfaRequisitiMinimi perche' in Z-S e' fallito il fit \n",i,ii);
            continue;
         }
   if(fabs(KAPPA[ii])<1.e-20 ){
    fprintf(HANDLE,
"       TracciaMC %d; sua FoundTrack associata (n. %d) NONsoddisfaRequisitiMinimi perche' KAPPA troppo piccolo; KAPPA = %g\n",
          i,ii,KAPPA[ii]);
           continue;
   }
   	dista=sqrt( Ox[ii]*Ox[ii]+Oy[ii]*Oy[ii] );
   if(fabs(dista)<1.e-20 ){
    fprintf(HANDLE,
"       TracciaMC %d; sua FoundTrack associata (n. %d) NONsoddisfaRequisitiMinimi perche' centro Helix Cilinder trovato dista solo %g da (0,0)\n",
           i,ii,dista);
           continue;
   }
   pMCtr = (PndMCTrack*) fMCTrackArray->At(i);
   if ( ! pMCtr ){
		fprintf(HANDLE,
		"       MC track n. %d doesn't have pointer to MC Track TClones Array\n",
		i);
	  continue;
   }


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
"       e corrisponde a track found n. %d\n", ii );
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


  }   //   end of  for (ii=0; ii<nSttTrackCand && istampa>=1 ;ii++)


//--------------ghosts

if( istampa>=3){
    int NParghost=0, NParhitsghost=0,icc;
    for(icc=0; icc<nSttTrackCand;icc++){
       if( daTrackFoundaTrackMC[icc] == -1){
          NParghost++;
          NParhitsghost += nSttParHitsinTrack[icc]+nSttSkewHitsinTrack[icc];

          fprintf(HANDLE,"          tracce Trovata n. %d e' Ghost\n",icc);
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
	for(ncand=0, ipinco = 0; ncand< nTotalCandidates; ncand++){
//		if(!status[ncand]) continue;
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
		} else {
			Pzini = 999999.;
		}
		// PndTrackCand Array loading
		new((*fSttMvdPndTrackCandArray)[ipinco])  PndTrackCand;
		PndTrackCand *pTrckCand = (PndTrackCand*) fSttMvdPndTrackCandArray->At(ipinco);
		TVector3 dirSeed(Pxini,Pyini,Pzini); // momentum direction in starting point
		qop = CHARGE[ncand]/dirSeed.Mag();
		dirSeed.SetMag(1.);
		pTrckCand->setTrackSeed(posSeed, dirSeed, qop);
		pTrckCand->setMcTrackId(  daTrackFoundaTrackMC[ncand]   );
//		pTrckCand->sorted=true;


		for(j=0; j< nTrackCandHit[ncand]; j++){
		     switch (ListTrackCandHitType[ncand][j]){
			case 0:
			   pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId(fMvdPixelBranch),(Int_t)ListTrackCandHit[ncand][j],j);
			break;
			case 1:
			   pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId(fMvdStripBranch),(Int_t)ListTrackCandHit[ncand][j],j);
			break;
			case 2:
			   pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId(fSttBranch),(Int_t)ListTrackCandHit[ncand][j],j);
			break;
			case 3:
			   pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId(fSttBranch),(Int_t)ListTrackCandHit[ncand][j],j);
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
			PndSttInfoXYZParal (
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

if(istampa>=3) cout<<" evento = "<<IVOLTE<<", track cand n. "<<ncand<<endl<<
	"\tfirst hit, tipo hit = "<<ListTrackCandHitType[ncand][0]<<", X = "<<Posiz1[0]
	<<", Y = "<<Posiz1[1]<<", Z = "<<Posiz1[2]<<
	"\n\tPx = "<<px<<", Py = "<<py<<", Pz = "<<Pzini<<", Ptras "<<Ptras<<endl
	<<"\t ErrX = "<<ErrPosition[0]
	<<", ErrY = "<<ErrPosition[1]<<", ErrZ = "<<ErrPosition[2]<<endl<<
	"\tErrPx = "<<ErrMomentum.X()<<", ErrPy = "<<ErrMomentum.Y()<<", ErrPz = "
	<<ErrMomentum.Z()<<endl
	<<"\tKAPPA = "<<KAPPA[ncand]<<", FI0 = "<<FI0[ncand]<<", n. hit (original) = "<<
	ListTrackCandHit[ncand][0]<<endl;

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

if(istampa>=3) cout<<" evento = "<<IVOLTE<<", track cand n. "<<ncand<<endl<<
	"\tlast hit, tipo hit = "<<ListTrackCandHitType[ncand][k]
	<<", X = "<<Posiz1[0]
	<<", Y = "<<Posiz1[1]<<", Z = "<<Posiz1[2]<<
	"\n\tPx = "<<px<<", Py = "<<py<<", Pz = "<<Pzini<<", Ptras "<<Ptras<<endl
	<<"\t ErrX = "<<ErrPosition[0]
	<<", ErrY = "<<ErrPosition[1]<<", ErrZ = "<<ErrPosition[2]<<endl<<
	"\tErrPx = "<<ErrMomentum.X()<<", ErrPy = "<<ErrMomentum.Y()<<", ErrPz = "<<ErrMomentum.Z()
	<<endl
	<<"\tKAPPA = "<<KAPPA[ncand]<<", FI0 = "<<FI0[ncand]<<", n. hit (original) = "<<
	ListTrackCandHit[ncand][k]<<endl;

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

		ipinco++;
	}	// end of     for(ncand=0, ipinco = 0; ncand< nTotalCandidates; ncand++)




//--------------------- end of load the new PndTrackCand
















//------------------------------   plottamenti --------------------------------------------



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





 if(iplotta && IVOLTE<10){


//    for(  i= 0; i< nSttTrackCand; i++){
    for(  i= 0; i< nTotalCandidates; i++){


	int npunti=-1+nSttParHitsinTrack[i]+nSttSkewHitsinTrack[i]+
		nMvdPixelHitsinTrack[i]+nMvdStripHitsinTrack[i];

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
	   	MCSkewAloneX[ MCSkewAloneList[i][j] ]=pSttMCPoint[MCSkewAloneList[i][j]]->GetX();
	   	MCSkewAloneY[ MCSkewAloneList[i][j] ]=pSttMCPoint[MCSkewAloneList[i][j]]->GetY();
	   }


	   WriteMacroSttParallelAssociatedHitsandMvdwithMC(
                   Ox[i], Oy[i], R[i],
			primoangolo[i],ultimoangolo[i],
                   nSttParHitsinTrack[i],
		   ListSttParHitsinTrack,
                   info,
                   i,

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
                   KAPPA[i],FI0[i], Ox[i], Oy[i], R[i],
                   info,
		   WDX,WDY,WDZ,
                   i,0,
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
		&MCMvdStripAloneList[i][0]

                                                     );
      }  //  end of	if(  nSttSkewHitsinTrack[i]+nMvdPixelHitsinTrack[i]+
	//	nMvdStripHitsinTrack[i]>0 &&  doMcComparison)
    }            //   end of   for(  i= 0; i< nSttTrackCand; i++)
i=0;



        WriteMacroParallelHitsGeneral(
                   nSttHit, info,
//		   nSttTrackCand,
		   nTotalCandidates,
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



        WriteMacroAllHitsRestanti(
		nSttHit,
		nSttParHit,
		nSttSkewHit,
		info,
//		nSttTrackCand,
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





//--start of function PndSttTrackFinderReal::WriteMacroSttParallelAssociatedHitsandMvdwithMC

  void PndSttMvdTracking::WriteMacroSttParallelAssociatedHitsandMvdwithMC(
                   Double_t Ox,Double_t Oy,Double_t R,
		   Double_t primoangolo,
		   Double_t ultimoangolo,
                   UShort_t Nhits,
		   UShort_t ListHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits],
                   Double_t info[][7],
                   UShort_t iTrack,

		Short_t daSttTrackaMCTrack,
		UShort_t nParalCommon[MAXTRACKSPEREVENT],
		UShort_t ParalCommonList[MAXMCTRACKS][nmaxSttHits],
		UShort_t nSpuriParinTrack[MAXTRACKSPEREVENT],
		UShort_t ParSpuriList[MAXTRACKSPEREVENT][nmaxSttHits],

		UShort_t nMCParalAlone[MAXTRACKSPEREVENT],
		UShort_t MCParalAloneList[MAXTRACKSPEREVENT][nmaxSttHits],

	UShort_t nMvdPixelHitsAssociatedToSttTra,
UShort_t ListPixelHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHits],
	UShort_t nMvdStripHitsAssociatedToSttTra,
UShort_t ListStripHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdStripHits],

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
		UShort_t ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits],
		Double_t *SchosenSkew,
		UShort_t nSkewCommon[MAXTRACKSPEREVENT],
		UShort_t SkewCommonList[MAXTRACKSPEREVENT][nmaxSttHits],
		UShort_t nMCSkewAlone[MAXTRACKSPEREVENT],
		UShort_t MCSkewAloneList[MAXMCTRACKS][nmaxSttHits]
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





//    Ox = (D+R)*cos(Fi);
//    Oy = (D+R)*sin(Fi);

//cout<<"da MacroTrackparalleletc. Ox, Oy "<<Ox<<",  "<<Oy<<endl;


//---------- parallel straws Macro now
      char nome[300], nome2[300];
      sprintf(nome,"MacroAllHitswithMCEvent%dT%d", IVOLTE,iTrack);
      sprintf(nome2,"%s.C",nome);
      FILE * MACRO = fopen(nome2,"w");
//      fprintf(MACRO,"void %s()\n{\n",nome);
      fprintf(MACRO,"{\n");
      xmin=1.e20;
      xmax=-1.e20;
      ymin=1.e20;
      ymax=-1.e20;
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


//--end of function PndSttTrackFinderReal::WriteMacroSttParallelAssociatedHitsandMvdwithMC














//----------start of function PndSttMvdTracking::WriteMacroParallelHitsGeneral

  void PndSttMvdTracking::WriteMacroParallelHitsGeneral(
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
           distance, Rx, Ry, LL,
           Aellipsis1, Bellipsis1,fi1,
           fmin, fmax, offset, step,
           SkewInclWithRespectToS, zpos, zpos1, zpos2,
           Tiltdirection1[2],
           zl[200],zu[200],
           POINTS1[6];

      char nome[300], nome2[300];





//---------- parallel straws Macro now
      sprintf(nome,"MacroAllHitsEvent%d", IVOLTE);
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


       fprintf(MACRO,"TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n",xmin,0.,xmax,0.,xmin,xmax);
       fprintf(MACRO,"Assex->Draw();\n");
       fprintf(MACRO,"TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", 0.,ymin,0.,ymax,ymin,ymax);
       fprintf(MACRO,"Assey->Draw();\n");


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



//-------------------------------   plotting all the tracks found

    for(i=0; i<nTracksFoundSoFar; i++){

       aaa = Ox[i];
       bbb = Oy[i];
       rrr = Radius[i];
          fprintf(MACRO,
//"TEllipse* ris%d=new TEllipse(%f,%f,%f,%f,0.,360.);\nris%d->SetFillStyle(0);\nris%d->SetLineColor(2);\nris%d->Draw();\n",
//                     i,aaa,bbb,rrr,rrr,i,i,i);
"TEllipse* ris%d=new TEllipse(%f,%f,%f,%f,%f,%f);\nris%d->SetFillStyle(0);\nris%d->SetLineColor(2);\nris%d->Draw(\"only\");\n",
                     i,aaa,bbb,rrr,rrr,primoangolo[i],ultimoangolo[i],i,i,i);


    }

// -----------


      fprintf(MACRO,"}\n");
      fclose(MACRO);
       
//------------------------------------------------------------------------------------------------------------


//---------- parallel straws Macro now con anche le tracce MC

      sprintf(nome,"MacroAllHitswithMCEvent%d", IVOLTE);
      sprintf(nome2,"%s.C",nome);
      MACRO = fopen(nome2,"w");
      fprintf(MACRO,"void %s()\n{\n",nome);


       fprintf(MACRO,"TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n",xmin,ymin,xmax,ymax);


       fprintf(MACRO,"TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n",xmin,0.,xmax,0.,xmin,xmax);
       fprintf(MACRO,"Assex->Draw();\n");
       fprintf(MACRO,"TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", 0.,ymin,0.,ymax,ymin,ymax);
       fprintf(MACRO,"Assey->Draw();\n");


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




//-------------------------------   plotting all the tracks found

    for(i=0; i<nTracksFoundSoFar; i++){

       aaa = Ox[i];
       bbb = Oy[i];
       rrr = Radius[i];
       fprintf(MACRO,
//       "TEllipse* ris%d=new TEllipse(%f,%f,%f,%f,0.,360.);\nris%d->SetFillStyle(0);\nris%d->SetLineColor(2);\nris%d->Draw();\n",
//                     i,aaa,bbb,rrr,rrr,i,i,i);
"TEllipse* ris%d=new TEllipse(%f,%f,%f,%f,%f,%f);\nris%d->SetFillStyle(0);\nris%d->SetLineColor(2);\nris%d->Draw(\"only\");\n",
                     i,aaa,bbb,rrr,rrr,primoangolo[i],ultimoangolo[i],i,i,i);

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
		sprintf(nome,"MacroAllHitsTime%dEvent%d",i, IVOLTE);
	} else {
		i = (Int_t) -time;
		sprintf(nome,"MacroAllHitsTime-%dEvent%d",i, IVOLTE);
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
                   Double_t info[][7],

                   Double_t WDX[nmaxSttHits],
                   Double_t WDY[nmaxSttHits],
                   Double_t WDZ[nmaxSttHits],

                   UShort_t iTrack, Int_t nMaxima, 
                   UShort_t nSkewHitsinTrack,
                   UShort_t ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits],
                   UShort_t nSkewCommon,
                   UShort_t SkewCommonList[MAXTRACKSPEREVENT][nmaxSttHits],
                   Short_t daTrackFoundaTrackMC,
                   UShort_t nMCSkewAlone,
                   UShort_t MCSkewAloneList[MAXMCTRACKS][nmaxSttHits],
		   UShort_t nPixelHitsinTrack[MAXTRACKSPEREVENT], // output
		   UShort_t ListPixelHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHits], // output
		   UShort_t nStripHitsinTrack[MAXTRACKSPEREVENT], // output
		   UShort_t ListStripHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdStripHits], // output



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
		UShort_t *MCMvdStripAloneList

						)
 {


	TDatabasePDG *fdbPDG= TDatabasePDG::Instance();

    Int_t i, j, i1, ii, iii, index, Kincl, nlow, nup, STATUS, imc, Nmin, Nmax;

    Double_t xmin , xmax, ymin, ymax,
           dx, dy, diff, d1, d2,
           delta, deltax, deltay, deltaz, deltaS,
           esse,factor,
           zmin, zmax, Smin, Smax, S1, S2,
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
      sprintf(nome,  "MacroSZwithMvdwithMCEvent%dT%d", IVOLTE,iTrack);
      sprintf(nome2,  "%s.C",nome);
      MACRO = fopen(nome2,"w");
      fprintf(MACRO,"{\n");

//KAPPA = 1./166.67 ;  FI0 = 1.5*PI;

      Smin=zmin = 1.e10;
      Smax=zmax = -zmin;
      index=0;

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

  }   //   end of  for( i=1; i< Nhits; i++)

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

  }   //   end of  for( i=1; i< Nhits; i++)








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
   fprintf(MACRO,"TLine* FOUND%d = new TLine(%f,%f,%f,%f);\nFOUND%d->SetLineColor(2);\nFOUND%d->Draw();\n",
//                 i-Nmin,z1,0.,z2, 2.*PI,i-Nmin,i-Nmin);
                 i-Nmin,z1,0.,z2, R*2.*PI,i-Nmin,i-Nmin);

  }   //  end of  for(i=Nmin; i<= Nmax;++)



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
		UShort_t nSttTrackCand,
		UShort_t nCandHit[MAXTRACKSPEREVENT],
		UShort_t ListCandHit[MAXTRACKSPEREVENT][nmaxSttHits+
	                           nmaxMvdPixelHitsInTrack+
				   nmaxMvdStripHitsInTrack],
		Short_t ListCandHitType[MAXTRACKSPEREVENT][nmaxSttHits+
	                           nmaxMvdPixelHitsInTrack+
				   nmaxMvdStripHitsInTrack]
					)
 {


//	nSttHit = parallel+skew.

	bool	exclusionStt[nSttHit],
		exclusionPixel[nMvdPixelHit],
		exclusionStrip[nMvdStripHit];

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


      sprintf(nome,"MacroHitsRestantiEvent%d", IVOLTE);
      sprintf(nome2,"%s.C",nome);
      FILE * MACRO = fopen(nome2,"w");
      fprintf(MACRO,"void %s()\n{\n",nome);
      fprintf(MACRO,"TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n",xmin,ymin,xmax,ymax);
       fprintf(MACRO,"TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n",xmin,0.,xmax,0.,xmin,xmax);
       fprintf(MACRO,"Assex->Draw();\n");
       fprintf(MACRO,"TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", 0.,ymin,0.,ymax,ymin,ymax);
       fprintf(MACRO,"Assey->Draw();\n");



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

      fprintf(MACRO,"}\n");
      fclose(MACRO);



	return;
 }







//----------end of function PndSttMvdTracking::WriteMacroAllHitsRestanti








//----------begin of function PndSttMvdTracking::AssociateFoundTrackstoMC

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

   bool	       inclusionMC[nTracksFoundSoFar][nmaxSttHits],
		inclusionExp[nTracksFoundSoFar];

   UShort_t	ntoMCtrack[nTracksFoundSoFar],
		toMCtracklist[nTracksFoundSoFar][nmaxSttHits],
		toMCtrackfrequency[nTracksFoundSoFar][nmaxSttHits];

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

   bool	       inclusionMC[nTracksFoundSoFar][nmaxSttHits],
		inclusionExp[nTracksFoundSoFar];

   UShort_t	ntoMCtrack[nTracksFoundSoFar],
		toMCtracklist[nTracksFoundSoFar][nmaxSttHits],
		toMCtrackfrequency[nTracksFoundSoFar][nmaxSttHits];

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
	inclusionMC[nTracksFoundSoFar][nmaxSttHits],
		inclusionExp[nTracksFoundSoFar];

   UShort_t	ntoMCtrack[nTracksFoundSoFar],
		toMCtrackfrequency[nTracksFoundSoFar][nmaxSttHits];

   UShort_t  i, j, jtemp,jexp , nmid;

   Short_t  enne, itemp, massimo,
		toMCtracklist[nTracksFoundSoFar][nmaxSttHits];

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
		 toMCtrackdistance[nTracksFoundSoFar][nmaxSttHits];


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
		enne = (Short_t)( info[  ListHitsinTrack[jexp][i] ][6]+0.01 );
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
                  UShort_t  ListHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits],
                  UShort_t nSkewHitsinTrack[MAXTRACKSPEREVENT],
                  UShort_t  ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits],

		UShort_t nPixelHitsinTrack[MAXTRACKSPEREVENT],
		UShort_t ListPixelHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHits],
		Short_t *FromPixeltoMCTrack,
		UShort_t nStripHitsinTrack[MAXTRACKSPEREVENT],
		UShort_t ListStripHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdStripHits],
		Short_t *FromStriptoMCTrack,

		Short_t daTrackFoundaTrackMC[MAXTRACKSPEREVENT]
						)
{

   bool	firstime,
	inclusionMC[nTracksFoundSoFar][nmaxSttHits+nmaxMvdPixelHits+nmaxMvdStripHits],
		inclusionExp[nTracksFoundSoFar];

   UShort_t	ntoMCtrack[nTracksFoundSoFar],
		toMCtrackfrequency[nTracksFoundSoFar][nmaxSttHits];

   UShort_t  i, j, jtemp,jexp , nmid;

   Short_t  enne, itemp, massimo,
		toMCtracklist[nTracksFoundSoFar][nmaxSttHits];

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
		 toMCtrackdistance[nTracksFoundSoFar][nmaxSttHits];




   for(i=0; i<nTracksFoundSoFar;i++){



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

	firstime=true;
	ntoMCtrack[jexp]=0;


// prima  gli hits paralleli ---------------------
	for(i=0; i<nHitsinTrack[jexp]; i++){
		enne = (Short_t)( info[  ListHitsinTrack[jexp][i] ][6]+0.01 );
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
		enne = FromStriptoMCTrack[ ListPixelHitsinTrack[jexp][i] ] ;
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
			UShort_t ntotalHits,
			Double_t info[][7],
                        UShort_t nTracksFoundSoFar, //  quelle trovate dal PR
  			UShort_t nHitsinTrack[MAXTRACKSPEREVENT], // n. hits PARALLELI+SKEW, dal PR
			UShort_t ListHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits], // dal PR
  			UShort_t nSkewHitsinTrack[MAXTRACKSPEREVENT], // n. hits skew, dal PR
			UShort_t ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits], // dal PR

			UShort_t nParalCommon[MAXTRACKSPEREVENT],
                        UShort_t ParalCommonList[MAXMCTRACKS][nmaxSttHits],
                        UShort_t nSpuriParinTrack[MAXTRACKSPEREVENT],
                        UShort_t ParSpuriList[MAXTRACKSPEREVENT][nmaxSttHits],

			UShort_t nSkewCommon[MAXTRACKSPEREVENT],
                        UShort_t SkewCommonList[MAXMCTRACKS][nmaxSttHits],
                        UShort_t nSpuriSkewinTrack[MAXTRACKSPEREVENT],
                        UShort_t SkewSpuriList[MAXTRACKSPEREVENT][nmaxSttHits],

                        UShort_t nHitsInMCTrack[MAXTRACKSPEREVENT],
                        UShort_t nSkewHitsInMCTrack[MAXTRACKSPEREVENT],

			UShort_t nMCParalAlone[MAXTRACKSPEREVENT],
                        UShort_t MCParalAloneList[MAXTRACKSPEREVENT][nmaxSttHits],
			UShort_t nMCSkewAlone[MAXTRACKSPEREVENT],
                        UShort_t MCSkewAloneList[MAXTRACKSPEREVENT][nmaxSttHits],

			Short_t  daTrackFoundaTrackMC[MAXTRACKSPEREVENT]
                                               )
{

    UShort_t	i, jexp, exphit, iHit,
		enne[MAXTRACKSPEREVENT][nmaxSttHits];
  Short_t	emme;


  for(jexp=0; jexp<nTracksFoundSoFar;jexp++){
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
		if( !ExclusionListStt[i]) continue; // escludo gli hits con multiple hits
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
		if( !ExclusionListStt[i]) continue; // escludo gli hits con multiple hits
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
	UShort_t *ind)
{

  UShort_t nr, nl, middle, i,
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



//----------begin of function PndSttTrackFinderReal::PndStt_Merge




void PndSttMvdTracking::Merge(UShort_t nl, Double_t *left, UShort_t *ind_left, UShort_t nr,
                                         Double_t *right, UShort_t *ind_right,  Double_t *result, UShort_t *ind)
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

      Short_t PndSttMvdTracking::FitHelixCylinder( UShort_t nHitsinTrack,
		Double_t *Xconformal,
		Double_t *Yconformal,
		Double_t *DriftRadiusconformal,
		Double_t *ErrorDriftRadiusconformal,
		Double_t rotationangle,
		Double_t * trajectory_vertex,
		UShort_t NMAX,
		Double_t *emme,
		Double_t *qu,
		Double_t *ALFA,
		Double_t *BETA,
		Double_t *GAMMA,
		bool *TypeConf
					)
{


    //   definition of variables for the glpsol  solver
   //    ROWS (for read_rows  function)
   //
   UShort_t  NpointsInFit = nHitsinTrack-NMAX <0 ?  nHitsinTrack :  NMAX;
// NpointsInFit =10;
   bool mvdhit[NpointsInFit];




     Double_t M = 1.,
              m_result,
              q_result,
              A,
              alfetta,
              angle,
              offsety,
              Delta[nHitsinTrack],
              Ox[nHitsinTrack],
              Oy[nHitsinTrack];

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
          BoundStructVarName[NpointsInFit+nSttHits]=&auxBoundStructVarName[NpointsInFit+nSttHits][0];
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

    *GAMMA = 0.;
    if( fabs( *qu ) > 1.e-10) {    //  trajectory is a circle in XY space
     *ALFA = *emme/(*qu);
     *BETA = -1./(*qu);
     *TypeConf=true;
//  now take into account the rotation and correct; the only affected quantities are ALFA and BETA
      alfetta = *ALFA;
      *ALFA = *ALFA*cose - *BETA*sine;
      *BETA = alfetta*sine + *BETA*cose;


    }  else if(fabs(*emme)> 1.e-10)  {    //  trajectory is a straight line in XY space of equation y= m*x
       //  the rotation first
       angle = atan(*emme) + rotationangle;
       if( fabs(cos(angle)) > 1.e-10 ) {
         *ALFA = 999999.;
         *BETA = -*ALFA/tan(angle);
         *TypeConf=false;

       } else {  //  in this case the equation is y = 0.
         *ALFA = 0.;
         *BETA = 999999.;
         *TypeConf=false;
       }
    }  else {   //  in this case also the equation in XY plane is  y = 0.
         *ALFA = 0.;
         *BETA = 999999.;
         *TypeConf=false;
    }	// end of 	if( fabs( *qu ) > 1.e-10)

//------------------


// now take into account the displacement and correct
      *GAMMA += (trajectory_vertex[0]*trajectory_vertex[0]+ trajectory_vertex[1]*trajectory_vertex[1]
                                  -*ALFA*trajectory_vertex[0]-*BETA*trajectory_vertex[1]);
      *ALFA -=  2.*trajectory_vertex[0];
      *BETA -=  2.*trajectory_vertex[1];


      if(fabs(cose-*emme*sine)> 1.e-10) {
        *qu=*qu/(cose-*emme*sine);
        *emme=(*emme*cose+sine)/(cose-*emme*sine);
        return 1;
      } else {    //  in this case the equation is   0 = x+*qu .
        if(fabs(sine+*emme*cose) < 1.e-10)  {
  cout<<" From FitHelixCylinder, situation impossible in principle! Returning -1"
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
// NpointsInFit =10;
   bool mvdhit[NpointsInFit];




     Double_t ave,
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
              Ox[nSkewHitsinTrack],
              Oy[nSkewHitsinTrack],
              Delta[nSkewHitsinTrack];

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
//		rotationangle = atan(ave);
		rotationangle = atan2(avey,avex);
	} else {
		rotationangle=PI/2.;
	}





//  use the trick of increasing the rotation angle by 10 degrees in order to obtain always a positive m
//      rotationangle -= PI/18.;
      rotationangle = PI/2.;


      cose = cos(rotationangle);
      sine = sin(rotationangle);

      nSttHits = nMvdHits = 0;
      for(i=0;i<NpointsInFit; i++){
       Ox[i] =   Z[i]*cose +(S[i] - FInot)*sine;
       Oy[i] = -Z[i]*sine +(S[i] - FInot)*cose;
//       Delta[i] = ErrorDriftRadius[i];
          Delta[i] = 2.*DriftRadius[i];

	if( DriftRadius[ i ]<0. )
	{
		mvdhit[i]=true;
		nMvdHits++;
//		Delta[i] = 0.006;
		Delta[i] = 1.;
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
        *emme=(*emme*cose+sine)/(cose-*emme*sine);
	return 1;
      } else {    //  in this case the equation is   0 = x+*qu .
           return -99;
      }



}


//----------end of function PndSttMvdTracking::FitSZspace








//------------------ begin function  PndSttMvdTracking::RefitMvdStt



 void   PndSttMvdTracking::RefitMvdStt(
			UShort_t nCandHit,
			UShort_t *ListCandHit,
			Short_t *ListCandHitType,
			Double_t info[][7],
			Double_t rotationangle, //  this is between 0. and 2*PI
			Double_t tv[2],
			Short_t iexcl,

			Double_t *ALFA, // output of the fit
			Double_t *BETA, // output of the fit
			Double_t *GAMMA,// set at zero always for now
			bool *status    // fit status; true = successful
						)
{
	bool TypeConf;

	UShort_t	i,
			iparallel;

	UShort_t MAXIMUMHITSINFIT = 20;

	Short_t	exitstatus;

	Double_t dist2,
		 maxdis2,
		 emme,
		 factor,
		 gamma,
		 qu,
		 ErrorStraw = 0.03,
		 ErrorMvd = 0.01,
		 Xconformal[nCandHit],
		 Yconformal[nCandHit],
		 DriftRadiusconformal[nCandHit],
		 ErrorDriftRadiusconformal[nCandHit];

	*status= false;
	factor=3.;
	maxdis2=0.5;
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
			if(dist2<maxdis2) continue;	// this is to exclude Mvd hits too close to the traslated
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
			if(dist2<maxdis2) continue;	// this is to exclude Mvd hits too close to the traslated
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
			if(dist2<maxdis2) continue;	// this is to exclude hits too close to the traslated
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
					ALFA,
					BETA,
					GAMMA,
					&TypeConf
					);
	if( exitstatus > 0)	*status=true;
	return;
}


//------------------ end function  PndSttMvdTracking::RefitMvdStt



//------------------ begin function  PndSttMvdTracking::MvdMatchtoMC

  void PndSttMvdTracking::MvdMatchtoMC(
		UShort_t nMvdMCPoint,
		Double_t *XMvdMCPoint,
		Double_t *YMvdMCPoint,
		Double_t *ZMvdMCPoint,
		Short_t  *MCPointtoMCTrackID,
		Short_t *FromPixeltoMCTrack,	// output
		Short_t *FromStriptoMCTrack	// output
		   )
{

	bool	inclusionMCPoint[nMvdMCPoint];


	UShort_t	i,
			j,
			jmcpoint;
	Double_t	dist,
			distance;

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

			if( !inclusionMCPoint[j]) continue;
			distance = (XMvdMCPoint[j]-XMvdPixel[i])*(XMvdMCPoint[j]-XMvdPixel[i])+
				(YMvdMCPoint[j]-YMvdPixel[i])*(YMvdMCPoint[j]-YMvdPixel[i])+
				(ZMvdMCPoint[j]-ZMvdPixel[i])*(ZMvdMCPoint[j]-ZMvdPixel[i]);
if(istampa>2)	{
	cout<<"distanza**2 di Pixel hit n. "<<i
   <<" da MC Mvd Point n. "<<j<<" = "<<distance<<endl;
		}
			if( distance<dist){
				FromPixeltoMCTrack[i]=MCPointtoMCTrackID[j];
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

			if( !inclusionMCPoint[j]) continue;
			distance = (XMvdMCPoint[j]-XMvdStrip[i])*(XMvdMCPoint[j]-XMvdStrip[i])+
				(YMvdMCPoint[j]-YMvdStrip[i])*(YMvdMCPoint[j]-YMvdStrip[i])+
				(ZMvdMCPoint[j]-ZMvdStrip[i])*(ZMvdMCPoint[j]-ZMvdStrip[i]);
if(istampa>2) cout<<"distanza**2 di Strip hit n. "<<i
   <<" da MC Mvd Point n. "<<j<<" = "<<distance<<endl;
			if( distance<dist){
				FromStriptoMCTrack[i]=MCPointtoMCTrackID[j];
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
			Short_t *daTrackFoundaTrackMC,			// input
			Short_t *FromPixeltoMCTrack,			// input
			Short_t *FromStriptoMCTrack,			// input
			UShort_t *nPixelHitsinTrack,	// input
UShort_t ListPixelHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHits],// input
			UShort_t *nStripHitsinTrack,	// input
UShort_t ListStripHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdStripHits],// input

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


	bool includePixel[nSttTrackCand][nMvdPixelHit],
	     includeStrip[nSttTrackCand][nMvdStripHit];

	UShort_t i,j;

	int index;

	for(i=0; i<nSttTrackCand;i++){
		nMvdPixelCommon[i]=0;
		nMvdPixelSpuriinTrack[i]=0;
		nMCMvdPixelAlone[i]=0;
		nMvdStripCommon[i]=0;
		nMvdStripSpuriinTrack[i]=0;
		nMCMvdStripAlone[i]=0;
	}





	for(i=0; i<nSttTrackCand;i++){
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
			UShort_t ListPixelHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHits], // output
			UShort_t nStripHitsinTrack[MAXTRACKSPEREVENT], // output
			UShort_t ListStripHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdStripHits] // output
                    )
{
	bool specialcase;

	UShort_t i,j,j1, j2, imvdcand, jmvdhit, ncont,
		chosenmix,
		chosenmix2,
		ngoodmix,
		oldN,
		nn[nMvdTrackCand+2],
		nHighQuality[nMvdTrackCand+2],
		List[nMvdTrackCand+2][nMvdPixelHit+nMvdStripHit],
		ListType[nMvdTrackCand+2][nMvdPixelHit+nMvdStripHit];

	Double_t angle,
		anglemax,
		anglemin,
		dist,
		oldtotal,
		oldtotal2,
		total,
		Dist,
		DIST[nMvdTrackCand+1];



//int temporaneo=4;

    for(i=0; i<nSttTrackCand; i++){

	if( Fifirst[i] < -99999.5 ) continue ; // case with Fifirst[i]=-100000.; in this
						// case there can be no Mvd hits.
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

		nPixelHitsinTrack[i]=0;
		nStripHitsinTrack[i]=0;
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
					 (Oy[i]-YMvdPixel[ListHitMvdTrackCand[imvdcand][jmvdhit]])) -R[i]);
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

				} else if (ListHitTypeMvdTrackCand[imvdcand][jmvdhit]==
				FairRootManager::Instance()->GetBranchId(fMvdStripBranch)){

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
if(istampa>=3 && IVOLTE<20 ){cout<<"\tquesto Mvd candidato (n. ngoodmix = "<<ngoodmix-1<<
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
if(istampa>2 && IVOLTE<20 ){cout<<"\tevento n. "<<IVOLTE<<" questi Mvd ALONE DS hits passano  :\n"<<endl;

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
if(istampa>=3 && IVOLTE<20 ){cout<<"\tevento n. "<<IVOLTE<<" questi Mvd ALONE US hits passano  :\n"<<endl;
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
if(istampa>=3 ){cout<<"da PndSttMvdTracking :\t goodmix n. "<<j1<<", total distance "<<total
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
			UShort_t ListPixelHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHits], // output
			UShort_t nStripHitsinTrack[MAXTRACKSPEREVENT], // output
			UShort_t ListStripHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdStripHits] // output
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
		List[nMvdPixelHit+nMvdStripHit];

	Double_t angle,
		anglemax,
		anglemin,
		dist,
		oldtotal,
		oldtotal2,
		total,
		Dist,
		DIST[nMvdTrackCand+1];




	for(itrack=0; itrack<nSttTrackCand; itrack++){
		if( ! Mvdhits[itrack] ) continue;
		ntot=nPixelHitsinTrack[itrack]+
			nStripHitsinTrack[itrack];
	if( Fifirst[itrack] < -99998. ){  // case with Fifirst[i]=-99999.; in this
					// case there the circle is contained
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
//			Short_t CHARGE[MAXTRACKSPEREVENT],
			UShort_t nParHitsinTrack[MAXTRACKSPEREVENT], // input/output
			UShort_t ListParHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits] // input/output
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


	for(itrack=StartTrackCand; itrack<EndTrackCand; itrack++){
		if( ! Mvdhits[itrack] ) continue;
		if( Fi_low_limit[itrack] < -99998.) continue;  // case in which the track
						// is completely outside the Stt region
						// or it is completely inside the Mvd region.
		nParHitsinTrack[itrack]=0;

//	loop over the STT parallel hits and try to attach to each candidate track; in this
//	way in one shot I collect also the previously non collected hits and I remove the
//	spurious hits.

		nadd=0;
		for(i=0; i<nSttParHit; i++){
			ihit = ListAllParHits[i];


			if( !ExclusionListStt[ihit] ) continue;
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
			if(dist<NTIMES*STRAWRADIUS){
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
  					Short_t MCTrack,
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

/*
        if( bbb > 1.e-10){
           Tiltdirection1[0] = vz1/bbb;
           Tiltdirection1[1] = SkewInclWithRespectToS/bbb;
        } else {
           Tiltdirection1[0] = 1.;
           Tiltdirection1[1] = 0.;
        }
*/


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
//	Zerror[ii] = STRAWRADIUS*aaa/LL;
	Zerror[ii] = STRAWRESOLUTION*aaa/LL;



//	Double_t rotation1 = 180.*atan2(Tiltdirection1[1],Tiltdirection1[0])/PI;


   }    //  end of    for( ii=0; ii<2; ii++)

	return;
}

//-------------------------  end of function  PndSttMvdTracking::CalculateSandZ




//-------------------------  begin of function  PndSttMvdTracking::EliminateSpuriousSZ

  void PndSttMvdTracking::EliminateSpuriousSZ(
			UShort_t *nPixelHitsinTrack,
			UShort_t *ListPixelHitsinTrack,
			UShort_t *nStripHitsinTrack,
			UShort_t *ListStripHitsinTrack,
			UShort_t *nSkewHitsinTrack,
			UShort_t *ListSkewHitsinTrack,
			Double_t S[2*nmaxSttHits+nmaxMvdPixelHits+nmaxMvdStripHits][2],
			Double_t ZED[2*nmaxSttHits+nmaxMvdPixelHits+nmaxMvdStripHits][2],
			Double_t DriftRadius[2*nmaxSttHits+nmaxMvdPixelHits+nmaxMvdStripHits][2],
			Double_t ErrorDriftRadius[2*nmaxSttHits+nmaxMvdPixelHits+nmaxMvdStripHits][2],
			Double_t SchosenPixel[nmaxMvdPixelHits],
			Double_t SchosenStrip[nmaxMvdStripHits],
			Double_t SchosenSkew[nmaxSttHits],
			Double_t ZchosenPixel[nmaxMvdPixelHits],
			Double_t ZchosenStrip[nmaxMvdStripHits],
			Double_t ZchosenSkew[nmaxSttHits],
			Double_t ErrorchosenPixel[nmaxMvdPixelHits],
			Double_t ErrorchosenStrip[nmaxMvdStripHits],
			Double_t ErrorchosenSkew[nmaxSttHits],
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
		 auxListMvdPixel[(*nPixelHitsinTrack)],
		 auxnMvdStrip,
		 auxListMvdStrip[(*nStripHitsinTrack)],
		 auxnSttSkew,
		 auxListSttSkew[(*nSkewHitsinTrack)];

	Int_t  nr2,
		 nrounds0,
		 nrounds1,
		 Nround[4];

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


		if( Dist_SZ(R,KAPPA,FI0,ZED[i][0],S[i][0],&nrounds0) < MvdCut
						&&
					abs(nrounds0)<=MAXTURNSOFTRACK){
			auxListMvdPixel[auxnMvdPixel]=ListPixelHitsinTrack[i];
			SchosenPixel[ListPixelHitsinTrack[i]]=S[i][0];
			ZchosenPixel[ListPixelHitsinTrack[i]]=ZED[i][0];
			ErrorchosenPixel[ListPixelHitsinTrack[i]]=errorPixel;
			auxnMvdPixel++;
		}
	}	// end of  for(i=0;i<*nPixelHitsinTrack;i++)

	for(j=0;j<*nStripHitsinTrack;j++){
		i=j+(*nPixelHitsinTrack);


		if( Dist_SZ(R,KAPPA,FI0,ZED[i][0],S[i][0],&nrounds0) < MvdCut
						&&
					abs(nrounds0)<=MAXTURNSOFTRACK){
			auxListMvdStrip[auxnMvdStrip]=ListStripHitsinTrack[j];
			SchosenStrip[ListStripHitsinTrack[j]]=S[i][0];
			ZchosenStrip[ListStripHitsinTrack[j]]=ZED[i][0];
			ErrorchosenStrip[ListStripHitsinTrack[j]]=errorStrip;
			auxnMvdStrip++;
		}
	}	// end of  for(j=0;j<*nStripHitsinTrack;j++)

	for(j=0;j<*nSkewHitsinTrack;j++){
		i=j+(*nPixelHitsinTrack)+
		    (*nStripHitsinTrack);

		if( ZED[i][0]<999990. && ZED[i][1]<999990.){

			Dista[0] = Dist_SZ(R,KAPPA,FI0,ZED[i][0]+DriftRadius[i][0],S[i][0],&Nround[0]);
			Dista[1] = Dist_SZ(R,KAPPA,FI0,ZED[i][0]-DriftRadius[i][0],S[i][0],&Nround[1]);
			Dista[2] = Dist_SZ(R,KAPPA,FI0,ZED[i][1]+DriftRadius[i][1],S[i][1],&Nround[2]);
			Dista[3] = Dist_SZ(R,KAPPA,FI0,ZED[i][1]-DriftRadius[i][1],S[i][1],&Nround[3]);
			Zeta[0] = ZED[i][0]-DriftRadius[i][0];
			Zeta[1] = ZED[i][0]+DriftRadius[i][0];
			Zeta[2] = ZED[i][1]-DriftRadius[i][1];
			Zeta[3] = ZED[i][1]+DriftRadius[i][1];
			Esse[0] = S[i][0];
			Esse[1] = S[i][0];
			Esse[2] = S[i][1];
			Esse[3] = S[i][1];
			Errore[0]= DriftRadius[i][0];
			Errore[1]= DriftRadius[i][1];
			Errore[2]= DriftRadius[i][2];
			Errore[3]= DriftRadius[i][3];
//			Errore[0]= ErrorDriftRadius[i][0];
//			Errore[1]= ErrorDriftRadius[i][1];
//			Errore[2]= ErrorDriftRadius[i][2];
//			Errore[3]= ErrorDriftRadius[i][3];

			d_min=999999999.;
			for(k=1;k<4;k++){
				if( Nround[k]<=MAXTURNSOFTRACK )
				{
					if( fabs(Dista[k]) < d_min ){
					 d_min=fabs(Dista[k]);
					 Kmin = k;
					}
				}
			}
			if( d_min > 999999998.) continue;
			dista = Dista[Kmin];
			error = Errore[Kmin];
			SchosenSkew[ListSkewHitsinTrack[j]]=Esse[Kmin];
			ZchosenSkew[ListSkewHitsinTrack[j]]=Zeta[Kmin];


		} else if (ZED[i][0]<999990.){

			dista = Dist_SZ(R,KAPPA,FI0,ZED[i][0]+DriftRadius[i][0],S[i][0],&nrounds0);
			ddd = Dist_SZ(R,KAPPA,FI0,ZED[i][0]-DriftRadius[i][0],S[i][0],&nr2);
		if( abs(nrounds0) > MAXTURNSOFTRACK &&
				 abs(nr2)>MAXTURNSOFTRACK)
			{
				continue;
			} else if ( abs(nr2)>MAXTURNSOFTRACK)
			{
				 zeta0 = ZED[i][0]+DriftRadius[i][0];
			} else if ( abs(nrounds0) > MAXTURNSOFTRACK )
			{
				 dista = ddd;
				 zeta0 = ZED[i][0]-DriftRadius[i][0];
			} else
			{
				if( fabs(dista)>fabs(ddd) ) {
				 dista = ddd;
				 zeta0 = ZED[i][0]-DriftRadius[i][0];
				} else {
				 zeta0 = ZED[i][0]+DriftRadius[i][0];
				}
			}
			error = DriftRadius[i][0];
//			error = ErrorDriftRadius[i][0];
			SchosenSkew[ListSkewHitsinTrack[j]]=S[i][0];
			ZchosenSkew[ListSkewHitsinTrack[j]]=zeta0;

		} else if (ZED[i][1]<999990.){

			dista = Dist_SZ(R,KAPPA,FI0,ZED[i][1]+DriftRadius[i][1],S[i][1],&nrounds1);
			ddd = Dist_SZ(R,KAPPA,FI0,ZED[i][1]-DriftRadius[i][1],S[i][1],&nr2);


			if( abs(nrounds1) > MAXTURNSOFTRACK &&
				 abs(nr2)>MAXTURNSOFTRACK)
			{
				continue;
			} else if ( abs(nr2)>MAXTURNSOFTRACK)
			{
				 zeta1 = ZED[i][1]+DriftRadius[i][1];
			} else if ( abs(nrounds1) > MAXTURNSOFTRACK )
			{
				 dista = ddd;
				 zeta1 = ZED[i][1]-DriftRadius[i][1];
			} else
			{
				if( dista>ddd ) {
				 dista = ddd;
				 zeta1 = ZED[i][1]-DriftRadius[i][1];
				} else {
				 zeta1 = ZED[i][1]+DriftRadius[i][1];
				}
			}
			error = DriftRadius[i][1];
//			error = ErrorDriftRadius[i][1];
			SchosenSkew[ListSkewHitsinTrack[j]]=S[i][1];
			ZchosenSkew[ListSkewHitsinTrack[j]]=zeta1;

		} else {
			continue;
		}




		if(
//			dista < 2.*error
			dista < 1.1*error
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

	dis_segments = 2.*PI*R/sqrt(1.+KAPPA*KAPPA*R*R); // distance between two consecutive segments
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


    void PndSttMvdTracking::PndSttInfoXYZParal (
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

//  working in the hypothesis that the starting point of the track is near (0,0) so that
//  R_vertex < RStrawDetectorMin

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
                   Double_t Fi_initial_helix_referenceframe,
                   Double_t Fi_final_helix_referenceframe,
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

        if( distance >= info[i][4]+Aellipsis1 ) continue;



// checks that the projected ellipsis doesn't go out the boundaries of both the skew straw and the trajectory cylinder

//        if(
//             fabs(POINTS1[j+2]-info[i][2]) > SEMILENGTH_STRAIGHT- Aellipsis1 ||
//             distance + bbb > info[i][4]        //  the ellipsis goes out of the boundaries of the skew straw
//          ) {
// if( istampa)  cout<<"the ellipsis goes out of the boundaries of the skew straw, hit n. "<<i<<endl
//     <<"dis. from center "<<distance+bbb<<",  length of the straw "<<info[i][4]<<endl;
//           continue;
//          }
//--------------------------


        S[NAssociated] = atan2(POINTS1[j+1]-Oy, POINTS1[j]-Ox) ;  // atan2 returns radians in (-pi and +pi]
        if( S[NAssociated] < 0.) S[NAssociated] += 2.*PI;

//  check if the S of this intersection is compatible with information coming from the parallel fit of this track
//        Double_t Sprime = atan2(POINTS1[j+1], POINTS1[j]) ;
//        if( Sprime < 0.) Sprime += 2.*PI;

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

        Double_t Zh1 = Z[NAssociated] - ZDrift[NAssociated];
        Double_t Zh2 = Z[NAssociated] + ZDrift[NAssociated];
        Double_t Sh1 = S[NAssociated] - Aellipsis1*Tiltdirection1[1];
        Double_t Sh2 = S[NAssociated] + Aellipsis1*Tiltdirection1[1];
        Double_t Zlast1 = (Fi_final_helix_referenceframe-Fi_initial_helix_referenceframe)*Zh1
                                  /(Sh1-Fi_initial_helix_referenceframe);
        Double_t Zlast2 = (Fi_final_helix_referenceframe-Fi_initial_helix_referenceframe)*Zh2
                                  /(Sh2-Fi_initial_helix_referenceframe);



/*
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
			UShort_t FirstCandidate,
			UShort_t LastCandidate,
			Double_t info[][7]
				)
{

	UShort_t	i,
			j,
			ncand;


//     ordering all the hits belonging to the candidate track, by increasing R;
//     forming the new track with Mvd+Stt hits


	for(ncand=FirstCandidate; ncand< LastCandidate; ncand++){
		nTrackCandHit[ncand] =nSttParHitsinTrack[ncand]+nSttSkewHitsinTrack[ncand]+
					nMvdPixelHitsinTrack[ncand]+
					nMvdStripHitsinTrack[ncand];
		UShort_t tempmvdindex[nMvdPixelHitsinTrack[ncand]+
					nMvdStripHitsinTrack[ncand] ],
			 tempmvdtype[nMvdPixelHitsinTrack[ncand]+
					nMvdStripHitsinTrack[ncand] ],
			auxIndex[nMvdPixelHitsinTrack[ncand]+
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
					nSttSkewHitsinTrack[ncand] ],
			auxIndex2[nSttParHitsinTrack[ncand]+
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

			for(j=0; j< nSttParHitsinTrack[ncand]+nSttSkewHitsinTrack[ncand];j++){
				i = j+nMvdPixelHitsinTrack[ncand]+
				    nMvdStripHitsinTrack[ncand];
				ListTrackCandHit[ncand][i] = tempmvdindex2[ auxIndex2[j] ];
				ListTrackCandHitType[ncand][i] = tempmvdtype2[ auxIndex2[j] ];

			}
		}	// end of  if( nSttParHitsinTrack[ncand]+




	} //   end of  for(ncand=FirstCandidate; ncand< LastCandidate; ncand++)


	return;
}

//----------end of function PndSttMvdTracking::Ordering_Loading_ListTrackCandHit




ClassImp(PndSttMvdTracking)
