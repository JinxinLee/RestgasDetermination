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
}
// -------------------------------------------------------------------------

PndSttMvdTracking::PndSttMvdTracking(Int_t verbose) : FairTask("STT Stt-Mvd Tracking") { 
  fPersistence = kTRUE;
  fVerbose = verbose;
  istampa = verbose;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndSttMvdTracking::~PndSttMvdTracking() { 

}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndSttMvdTracking::Init() {
  

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



  IVOLTE=0;




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


  // Get SttTrack array // tracce che vengono dal fit di Pavia dell'elica
//  fSttTrackArray  = (TClonesArray*) ioman->GetObject("STTTrack"); 
//  if ( ! fSttTrackArray) 
//    {
//      cout << "-E- PndSttMvdTracking::Init: No SttTrack array, return!"
//	   << endl;
//      return kERROR;
//    }

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

  // Get input array   hit di STT dopo digi
  fSttHitArray = (TClonesArray*) ioman->GetObject("STTHit");
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


  // Create and register output array
/*
  fHelixHitArray = new TClonesArray("PndSttHelixHit");
  ioman->Register("SttHelixHit","STT",fHelixHitArray, fPersistence);
*/
    
  // CHECK added 

/*
  PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  fSttTubeArray = mapper->FillTubeArray();
*/


//  -------------------------   get the Mvd hits

  fMvdPixelHitArray = (TClonesArray*) ioman->GetObject("MVDHitsPixel");
  if ( !fMvdPixelHitArray){
    std::cout << "-W- PndSttMvdTracking::Init: " << "No MVD Pixel hitArray, return!" << std::endl;
    return kERROR;
  }

  fMvdStripHitArray = (TClonesArray*) ioman->GetObject("MVDHitsStrip");

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


 bool status[MAXTRACKSPEREVENT],
 	SttSZfit[MAXTRACKSPEREVENT],
	noMvdhits[MAXTRACKSPEREVENT];


 Short_t Candidato,
         Charge,
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
           SkewSpuriList[MAXTRACKSPEREVENT][nmaxSttHits];

  UShort_t
	   nTotalCandidates,
	   nTrackCandHit[MAXTRACKSPEREVENT],
	   ListTrackCandHit[MAXTRACKSPEREVENT][nmaxSttHits+
	                           nmaxMvdPixelHitsInTrack+
				   nmaxMvdStripHitsInTrack],
	   ListTrackCandHitType[MAXTRACKSPEREVENT][nmaxSttHits+  //  type = 0 --> Mvd Pixel
	                           nmaxMvdPixelHitsInTrack+   //  type = 1 --> Mvd Strip
				   nmaxMvdStripHitsInTrack],  //  type = 2 --> Stt Parallel
							      //  type = 3 --> Stt Straw
//	   nHitMvdTrackCand,
//	   nMvdPixelHit,
//	   nMvdStripHit,
	   nMvdMCPoint,
//	   nMvdTrackCand,
	   nSttHit,
	   nSttParHit,
	   nSttSkewHit,
	   nSttHelixTrack,
	   nSttMCPoint,
	   nSttTrackCand,
	   nSttHitsinTrack[MAXTRACKSPEREVENT],
	   nSttParHitsinTrack[MAXTRACKSPEREVENT],
	   nSttSkewHitsinTrack[MAXTRACKSPEREVENT],
	   ListSttHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits],
	   ListSttHitsinTrackType[MAXTRACKSPEREVENT][nmaxSttHits],
	   ListSttParHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits],
	   ListSttSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits],
	   nMvdPixelHitsAssociatedToSttTrack[MAXTRACKSPEREVENT],
	   ListMvdPixelHitsAssociatedToSttTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHits],
	   nMvdStripHitsAssociatedToSttTrack[MAXTRACKSPEREVENT],
	   ListMvdStripHitsAssociatedToSttTrack[MAXTRACKSPEREVENT][nmaxMvdStripHits]
		;
  UShort_t	ipinco,
		ncand,
		n;


  Double_t Dist,
	   Distance,
           HoughFi[MAXTRACKSPEREVENT],
	   Ntras,
           Phi,
	   Ptras,
           Rad,
           TanL,
           Z,
	   ddd,
	   dis,
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
	   zdrift[2],
	   zerror[2],
	   primoangolo[MAXTRACKSPEREVENT],
	   ultimoangolo[MAXTRACKSPEREVENT],
	   MCSkewAloneX[nmaxSttHits],
	   MCSkewAloneY[nmaxSttHits];

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
	   info[nmaxSttHits][7];

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
	if (istampa >= 2  && IVOLTE<20) {
	     cout<<"da PndSttMvdTracking  : evento (partendo da 1)  N. "<<
	       IVOLTE<< "\n       N. di MC truth tracks  : "<<nMCTracks<<endl;
	}
	if (nMCTracks ==0){
		cout<<"da PndSttMvdTracking  :  N. di MC truth tracks = 0, return!\n"<<endl;
		return;
	}



//------------------------------







//------------------------------

//----------   fetching the STT  MC points
 nSttMCPoint = fSttPointArray->GetEntriesFast();
 if (nSttMCPoint ==0){
	cout<<"da PndSttMvdTracking  :  N. di Stt MC points = 0\n"<<endl;
//	return;
 }

 if(istampa>1&& IVOLTE<20) cout<<"da PndSttMvdTracking  : n. punti MC = "<<nSttMCPoint<<endl;
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

 if (istampa >= 2  && IVOLTE<20) {
     cout<<"da PndSttMvdTracking  : evento (partendo da 1)  N. "<<
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


  if(istampa>=2  && IVOLTE<20)cout<<"da PndSttMvdTracking  :  n. totale di Mvd track cand =    "
                     <<nMvdTrackCand<<"  --------------\n";
//---------------  fetch the Stt PndTrackCand from PR of the STT

 nSttTrackCand = fSttTrackCandArray->GetEntriesFast();
 if (nSttTrackCand ==0){
	cout<<"da PndSttMvdTracking  :  N. of SttTrackCand = 0, return!\n"<<endl;
	return;
 } else if (nSttTrackCand >= MAXTRACKSPEREVENT) {
 	cout<<"da PndSttMvdTracking  :  N. of nSttTrackCand = "<<
	nSttTrackCand <<" and it is >= MAXTRACKSPEREVENT (="<<MAXTRACKSPEREVENT
	<<"),  return!\n";
 }
 if(istampa>=2  && IVOLTE<20){ cout<<"N. totale di PndTrackCand del PR solo = "
                              <<nSttTrackCand<<endl; }



//------------------------------------------------------------------


 nSttParHit=0;
 nSttSkewHit=0;
 for( i= 0; i< nSttHit; i++){
	  ExclusionListStt[i]= true ;

if(istampa>2&& IVOLTE<20) { cout<<" da PndSttMvdTracking  :  Stt hit n. "<<i
     <<" corrisponde a Stt MC track n. "<<pSttMCPoint[i]->GetTrackID()<<endl;}

  pSttHit = (PndSttHit *) fSttHitArray->At(i);
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
      info[i][6]= pSttMCPoint[i]->GetTrackID();

      if( fabs( WDX[i] )< 0.00001 && fabs( WDY[i] )< 0.00001 ){
          info[i][5]= 1.;
	  nSttParHit++;
       } else {
          info[i][5]= 99.;
	  nSttSkewHit++;
       }



//--------------- inizio stampaggi,  stampe di controllo
  if (istampa > 2  && IVOLTE<20) {
      cout <<"da PndSttMvdTracking, iHit "<< i << endl;
      cout <<"             hit X, Y, Z space position "   << pSttMCPoint[i]->GetX() << " " <<
                       pSttMCPoint[i]->GetY() << " " << pSttMCPoint[i]->GetZ()<<endl; 
      cout <<"             hit wire pos. in middle "   << pSttTube->GetPosition().X() << " " <<
             pSttTube->GetPosition().Y() << " " << pSttTube->GetPosition().Z() 
           << "; R = "<<sqrt(pSttTube->GetPosition().X()*pSttTube->GetPosition().X()+pSttTube->GetPosition().Y()*pSttTube->GetPosition().Y())<< endl;
      cout <<"             wire direction, X, Y, Z (Z direction set always positive)"<< WDX[i]<<"  "<<WDY[i]<<"  "<<WDZ[i] <<endl
           <<"             this hit belongs to MC track n. "<<pSttMCPoint[i]->GetTrackID()<<endl;

  }  //  end of   if(istampa >= 

//--------  fine stampaggi



  }  //   end of for( i= 0; i< nSttHit; i++)





//	fill the exclusion list for those straws with multiple hits


      //   first the parallel straws
      for(i=0; i< nSttHit-1; i++){
	if( !ExclusionListStt[ i ] ) continue;
	if( info[i][5]<90.){
			for(j=i+1; j< nSttHit; j++){
				if(ExclusionListStt[ j ] && info[j][5]==1. &&
					fabs(info[i][0] - info[j][0])<1.e-20 &&
					fabs(info[i][1] - info[j][1])<1.e-20  )
				{
					ExclusionListStt[j]= false ;
				}
			} //  end of  for(j=i+1; j< nSttHit; j++)
	} else {	//  continuation of  if( info[i][5]

			for(j=i+1; j< nSttHit; j++){
				if(ExclusionListStt[ j ] && info[j][5] > 90. &&
					fabs(info[i][0] - info[j][0])<1.e-20 &&
					fabs(info[i][1] - info[j][1])<1.e-20  )
				{
					ExclusionListStt[j]= false ;
				}
			} //  end of  for(j=i+1; j< Nhits;; j++)

	}	//	//  end of  if( info[i][5]==1.)

      }   //   end of for(i=0; i< Nhits-1; i++)



//-----------------------------------  end of exclusion of straws with multiple hits




//-------------------------------------------------------------  fine hits STT


// ---------------------------------------------  estraggo gli HITS Pixel MVD
 for( i= 0; i< nMvdPixelHit; i++){
    pMvdPixelHit = (PndSdsHit *) fMvdPixelHitArray->At(i);
    TVector3 temp = pMvdPixelHit->GetPosition();
    XMvdPixel[i] = temp.X();
    YMvdPixel[i] = temp.Y();
    ZMvdPixel[i] = temp.Z();
    sigmaXMvdPixel[i] = pMvdPixelHit->GetDx();
    sigmaYMvdPixel[i] = pMvdPixelHit->GetDy();
    sigmaZMvdPixel[i] = pMvdPixelHit->GetDz();
 }

// ------------------------------------- fine di estraggo gli HITS Pixel MVD


// -------------------------------------------  estraggo gli HITS Strip MVD

  for( i= 0; i< nMvdStripHit; i++){
    pMvdStripHit = (PndSdsHit *) fMvdStripHitArray->At(i);
    TVector3 temp = pMvdStripHit->GetPosition();
    XMvdStrip[i] = temp.X();
    YMvdStrip[i] = temp.Y();
    ZMvdStrip[i] = temp.Z();
    sigmaXMvdStrip[i] = pMvdStripHit->GetDx();
    sigmaYMvdStrip[i] = pMvdStripHit->GetDy();
    sigmaZMvdStrip[i] = pMvdStripHit->GetDz();
  }
// ------------------------------------ fine di estraggo gli HITS Strip MVD

//------------------------------------------ stampaggi  hits MVD
if(istampa>=2  && IVOLTE<20){
  cout<<"da PndSttMvdTracking  :  n. Mvd Pixel Hits = "<<nMvdPixelHit<<endl;
  cout<<"da PndSttMvdTracking  :  n. Mvd Strip Hits = "<<nMvdStripHit<<endl;
}
if(istampa>=2  && IVOLTE<20){
  cout<<"da PndSttMvdTracking  :  info hits di Mvd pixels ------------------\n";
  for(i= 0; i< nMvdPixelHit; i++){
      cout<<"      Pixel hit n. "<<i<<" Info : X  = "<<XMvdPixel[i]<<";  Y  = "<<YMvdPixel[i]<<
            ";  Z  = "<<ZMvdPixel[i]<<endl;
      cout<<"\t\tPixel Info : sigmaX  = "<<sigmaXMvdPixel[i]<<";  sigmaY  = "<<sigmaYMvdPixel[i]<<
            ";  sigmaZ  = "<<sigmaZMvdPixel[i]<<endl;
  }
  cout<<"        ------------------\n";

  cout<<"da PndSttMvdTracking  :  info hits di Mvd strips ------------------\n";
  for(i= 0; i< nMvdStripHit; i++){
      cout<<"      Strip hit n. "<<i<<" Info : X  = "<<XMvdStrip[i]<<";  Y  = "<<YMvdStrip[i]<<
            ";  Z  = "<<ZMvdStrip[i]<<endl;
      cout<<"\t\tStrip Info : sigmaX  = "<<sigmaXMvdStrip[i]<<";  sigmaY  = "<<sigmaYMvdStrip[i]<<
            ";  sigmaZ  = "<<sigmaZMvdStrip[i]<<endl;
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
if(istampa>2&& IVOLTE<20) cout<<"Il punto n. "<<i<<" Mvd MC e' associato alla traccia MC n. "
	   <<pMvdMCPoint->GetTrackID()<<endl;
	TVector3 position;
	pMvdMCPoint->Position(position);
	XMvdMCPoint[i]=position.X();
	YMvdMCPoint[i]=position.Y();
	ZMvdMCPoint[i]=position.Z();
	MCPointtoMCTrackID[i]= pMvdMCPoint->GetTrackID();
	if(istampa>2) cout<<"\tPosizione X point Mvd MC = "<<XMvdMCPoint[i]
			<<"\n\tPosizione Y point Mvd MC = "<<YMvdMCPoint[i]
			<<"\n\tPosizione Z point Mvd MC = "<<ZMvdMCPoint[i]
			<<endl<<"--------------\n";

   }
// ----------------------------------------------------------------------





//---------------  recupero le Helix-Lia  PndSttTrack trovate dopo il PR
//                 delle STT + fit di Lia

/*
 nSttHelixTrack = fSttTrackArray->GetEntriesFast();
 if(istampa>2  && IVOLTE<20){ cout<<"N. totale di PndSttTrack dopo PR+fit Lia = "
                              <<nSttHelixTrack<<endl; }

  for(  i= 0; i< nSttHelixTrack; i++){
   pSttHelixTrack = (PndSttTrack *) fSttTrackArray->At(i);

// ------  estraggo il PndTrackCand [che era uscito dal PR]
//         sul quale HelixFit e' stato applicato

  Candidato = pSttHelixTrack->GetTrackCandIndex();
  // parameters of the helix: d0, phi0, Rad, tanlambda, z0
  Dist =pSttHelixTrack->GetDist();
  Phi  = pSttHelixTrack->GetPhi();
  Rad=pSttHelixTrack->GetRad();
  TanL=pSttHelixTrack->GetTanL();
  Z=pSttHelixTrack->GetZ();
  
  Charge= pSttHelixTrack->GetCharge();
  iflag = pSttHelixTrack->GetFlag();
 }


*/

//---------------  fine del recupero le Helix-Lia  PndSttTrack trovate dopo il PR delle STT + fit di Lia



   
// ------------------------------------------ estraggo le altre info della trackcand  del MVD

  bool inMvdTrackCandPixel[nMvdPixelHit],
       inMvdTrackCandStrip[nMvdStripHit];

  for( i= 0; i< nMvdTrackCand ; i++){
	for(j=0; j<nMvdPixelHit; j++){
		inMvdTrackCandPixel[j]=false;
	}
	for(j=0; j<nMvdStripHit; j++){
		inMvdTrackCandStrip[j]=false;
	}
  }

  for( i= 0; i< nMvdTrackCand ; i++){

     pMvdTrackCand= (PndTrackCand*) fMvdTrackCandArray->At(i);
     TVector3 dirSeed=pMvdTrackCand->getDirSeed();
     TVector3 posSeed=pMvdTrackCand->getPosSeed();
     qop = pMvdTrackCand->getQoverPseed();
     nHitMvdTrackCand[i] = pMvdTrackCand->GetNHits();  // n. hits in questa track cand
     if(nHitMvdTrackCand[i]>= nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack){
	cout<<"from PndSttMvdTracking : # Mvd hits of this Mvd Track Cand is >= than the maximum allowed ("
		<<nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack<<"); skipping this candidate !\n";
	continue;
     }


if(istampa>=2  && IVOLTE<20) 
   cout<<"Evento n. "<<IVOLTE<<", info da MvdTrackCand n. "<<i<<"  ---------------------------------------\n"<<
       "\n      direzione X traccia = "<<dirSeed.X()<<
       "\n      direzione Y traccia = "<<dirSeed.Y()<<
       "\n      direzione Z traccia = "<<dirSeed.Z()<<
       "\n      Q over P traccia = "<<qop<<
       "\nn. hits in questa MvdTrackCand = "<<nHitMvdTrackCand[i]<<endl<<"    e loro elenco :\n";

     for(j=0; j<nHitMvdTrackCand[i]; j++){
       pndtrackcandhit = pMvdTrackCand->GetSortedHit(j);
       ListHitMvdTrackCand[i][j] = pndtrackcandhit.GetHitId(); // questo e' il n. Hit nativo che posso usare
                                          // per estrarre tutte le info che voglio.
       ListHitTypeMvdTrackCand[i][j] = pndtrackcandhit.GetDetId(); // questo e' il n. Hit nativo che posso usare
                                          // per estrarre tutte le info che voglio.
       if( ListHitTypeMvdTrackCand[i][j]==FairRootManager::Instance()->GetBranchId("MVDHitsPixel")){
		inMvdTrackCandPixel[ ListHitMvdTrackCand[i][j] ]= true;
	} else if( ListHitTypeMvdTrackCand[i][j]==FairRootManager::Instance()->GetBranchId("MVDHitsStrip")){
		inMvdTrackCandStrip[ ListHitMvdTrackCand[i][j] ]= true;
	}
		


if(istampa>=2  && IVOLTE<20){  cout<<"           hit n. "<<ListHitMvdTrackCand[i][j]
	<<" e suo Detector Id "<<ListHitTypeMvdTrackCand[i][j]
	<<" (Pixel DetId = "<<FairRootManager::Instance()->GetBranchId("MVDHitsPixel")<<", Strip DetId = "<<FairRootManager::Instance()->GetBranchId("MVDHitsStrip")<<")"<<endl;}


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


 for(  i= 0; i< nSttTrackCand; i++){
// ----  estraggo gli hits appartenenti a questa TrackCand
  pSttTrackCand = (PndTrackCand *) fSttTrackCandArray->At(i);
  nSttHitsinTrack[i] = pSttTrackCand->GetNHits();  // n. hits in questa track cand
  if(istampa>=2  && IVOLTE==2){
             cout<< "            Stt PR solo : Stt PndTrackCand  n. "<<i
	      <<"  che ha "<<nSttHitsinTrack[i]<<" hits associati" <<endl;
	                    }


  for(j=0,nSttParHitsinTrack[i]=0,nSttSkewHitsinTrack[i]=0; j<nSttHitsinTrack[i]; j++){
    pndtrackcandhit = pSttTrackCand->GetSortedHit(j);

    //  for the calculation of the approximate Fi of the first hit in thos track
    x = info[ pndtrackcandhit.GetHitId() ][0];  //  this is in the middle of the tube
    y = info[ pndtrackcandhit.GetHitId() ][1];  //  this is in the middle of the tube

    ListSttHitsinTrack[i][j] = pndtrackcandhit.GetHitId(); // # hit of Stt

    if ( fabs(info[ pndtrackcandhit.GetHitId() ][5]- 1.) < 0.0001) {
      ListSttHitsinTrackType[i][j] = 2; 
      ListSttParHitsinTrack[i][nSttParHitsinTrack[i]] = pndtrackcandhit.GetHitId(); // # hit of Stt
      if(istampa>=2  && IVOLTE==2){  cout<<"                 ha lo hit || n. "
       <<ListSttParHitsinTrack[i][nSttParHitsinTrack[i]]<<endl;}
      nSttParHitsinTrack[i]++;
    }  else {
      ListSttHitsinTrackType[i][j] = 3; 
      ListSttSkewHitsinTrack[i][nSttSkewHitsinTrack[i]] = pndtrackcandhit.GetHitId(); // # hit of Stt
      if(istampa>=2  && IVOLTE==2){  cout<<"                 ha lo hit skew n. "
       <<ListSttSkewHitsinTrack[i][nSttSkewHitsinTrack[i]]<<endl;}
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

if(istampa > 2&& IVOLTE<20){
   cout<<"    da PndSttMvdtracking, Evento n. "<<IVOLTE<<
   ";  estratto Stt PndTrackCand n. "<<i
        <<", qop = "<<qop<<
	", Ox[i] = "<<  Ox[i]<<"\n     Oy[i] = "<<Oy[i]<<",  KAPPA[i] = "
	<<KAPPA[i]<<endl;
}

	Fifirst[i] = atan2( y-Oy[i], x-Ox[i]);
	FI0[i] = atan2(-Oy[i], -Ox[i]);
	HoughFi[i] = FI0[i] + PI;
	if(HoughFi[i]<0.) HoughFi[i]=0.;
	if( FI0[i] < 0. )  FI0[i]+= 2.*PI;
	if( Fifirst[i] < 0. )  Fifirst[i]+= 2.*PI;

if(istampa>2  && IVOLTE<20){
  cout<<"    e FI0 = "<<FI0[i]<<",  e Fi = "  <<HoughFi[i]<<endl;
                            }
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





//-------------------  start the combined Mvd-Stt  PR






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
			Fifirst,
			CHARGE,
			nMvdPixelHitsAssociatedToSttTrack, // output
			ListMvdPixelHitsAssociatedToSttTrack, // output
			nMvdStripHitsAssociatedToSttTrack, // output
			ListMvdStripHitsAssociatedToSttTrack // output
			);



if(istampa>2&& IVOLTE<20){
           cout<<"da PndSttMvdTracking ;  n. SttTrackCand totali = "<<nSttTrackCand
	       <<"--------------------------------------\n";
      for(  i= 0; i< nSttTrackCand; i++){
           cout<<"da PndSttMvdTracking --------------------------------------\n"<<
	   "	SttTrackCand n.  "<<i<<";  n. Hits in Pixels associati = "
	   <<nMvdPixelHitsAssociatedToSttTrack[i]<<"   e loro lista \n";
	   for(j=0; j<nMvdPixelHitsAssociatedToSttTrack[i];j++){
	        cout<<"\t\tMvd Pixel Hit n. "<<
		     ListMvdPixelHitsAssociatedToSttTrack[i][j]<<endl;
	   }


           cout<<"da PndSttMvdTracking --------------------------------------\n"<<
	   "	SttTrackCand n.  "<<i<<";  n. Hits in Strips associati = "
	   <<nMvdStripHitsAssociatedToSttTrack[i]<<"   e loro lista \n";
	   for(j=0; j<nMvdStripHitsAssociatedToSttTrack[i];j++){
	        cout<<"\t\tMvd Strip Hit n. "<<
		     ListMvdStripHitsAssociatedToSttTrack[i][j]<<endl;
	   }
      }
}   //end of if(istampa>=0)

//-----------------  end of section with match Mvd hits with Stt hits














//----------------------------------------------------------
//     ordering all the hits belonging to the candidate track, by increasing R;
//     forming the new track with Mvd+Stt hits

	nTotalCandidates = nSttTrackCand;  //  this may change in the future

	for(ncand=0; ncand< nTotalCandidates; ncand++){
		nTrackCandHit[ncand] =nSttHitsinTrack[ncand]+
					nMvdPixelHitsAssociatedToSttTrack[ncand]+
					nMvdStripHitsAssociatedToSttTrack[ncand];
		UShort_t tempmvdindex[nMvdPixelHitsAssociatedToSttTrack[ncand]+
					nMvdStripHitsAssociatedToSttTrack[ncand] ],
			 tempmvdtype[nMvdPixelHitsAssociatedToSttTrack[ncand]+
					nMvdStripHitsAssociatedToSttTrack[ncand] ],
			auxIndex[nMvdPixelHitsAssociatedToSttTrack[ncand]+
					nMvdStripHitsAssociatedToSttTrack[ncand] ];
		Double_t auxR[nMvdPixelHitsAssociatedToSttTrack[ncand]+
					nMvdStripHitsAssociatedToSttTrack[ncand] ];
		// adding the Mvd hits (Pixel and Strips)
		for(i=0; i< nMvdPixelHitsAssociatedToSttTrack[ncand]; i++){
			auxR[i] = sqrt(
			 XMvdPixel[ ListMvdPixelHitsAssociatedToSttTrack[ncand][i] ]*
			 XMvdPixel[ ListMvdPixelHitsAssociatedToSttTrack[ncand][i] ]+
			 YMvdPixel[ ListMvdPixelHitsAssociatedToSttTrack[ncand][i] ]*
			 YMvdPixel[ ListMvdPixelHitsAssociatedToSttTrack[ncand][i] ]
					    );
			tempmvdindex[i]=ListMvdPixelHitsAssociatedToSttTrack[ncand][i];
			tempmvdtype[i]=0;
			auxIndex[i] = i;
		}
		for(i=0; i< nMvdStripHitsAssociatedToSttTrack[ncand]; i++){
			auxR[i+nMvdPixelHitsAssociatedToSttTrack[ncand]] = sqrt(
			 XMvdStrip[ ListMvdStripHitsAssociatedToSttTrack[ncand][i] ]*
			 XMvdStrip[ ListMvdStripHitsAssociatedToSttTrack[ncand][i] ]+
			 YMvdStrip[ ListMvdStripHitsAssociatedToSttTrack[ncand][i] ]*
			 YMvdStrip[ ListMvdStripHitsAssociatedToSttTrack[ncand][i] ]
					    );
			tempmvdindex[i+nMvdPixelHitsAssociatedToSttTrack[ncand]]=
				ListMvdStripHitsAssociatedToSttTrack[ncand][i];
			tempmvdtype[i+nMvdPixelHitsAssociatedToSttTrack[ncand]]=1;
			auxIndex[i+nMvdPixelHitsAssociatedToSttTrack[ncand]]=
				i+nMvdPixelHitsAssociatedToSttTrack[ncand];
		}

		//  ordering the Mvd Hits
		if( nMvdPixelHitsAssociatedToSttTrack[ncand]+
		    nMvdStripHitsAssociatedToSttTrack[ncand] >0){
			Merge_Sort( nMvdPixelHitsAssociatedToSttTrack[ncand]+
		                    nMvdStripHitsAssociatedToSttTrack[ncand],
				    auxR, auxIndex);

		//  constructing the ordered new Track  Candidate now
			for(i=0; i< nMvdPixelHitsAssociatedToSttTrack[ncand]+
			    nMvdStripHitsAssociatedToSttTrack[ncand]; i++){
				ListTrackCandHit[ncand][i] = tempmvdindex[ auxIndex[i] ];
				ListTrackCandHitType[ncand][i] = tempmvdtype[ auxIndex[i] ];

			}
		}	// end of  if( nMvdPixelHitsAssociatedToSttTrack[ncand]+

		for(i=0; i<nSttHitsinTrack[ncand]; i++){
			ListTrackCandHit[ncand][i+
				nMvdPixelHitsAssociatedToSttTrack[ncand]+
				nMvdStripHitsAssociatedToSttTrack[ncand]
					] = ListSttHitsinTrack[ncand][i];
			ListTrackCandHitType[ncand][i+
				nMvdPixelHitsAssociatedToSttTrack[ncand]+
				nMvdStripHitsAssociatedToSttTrack[ncand]
					] = ListSttHitsinTrackType[ncand][i];
		}




	} //   end of  for(ncand=0; ncand< nTotalCandidates; ncand++)


//-------------------- end of ordering



//---------------------   refit the Helix in XY plane using Stt + Mvd associated hits


	Short_t	iexcl;
	Double_t	d,
			diff,
			trajectory_vertex[2];

	for(ncand=0; ncand< nTotalCandidates; ncand++){

		if( nMvdPixelHitsAssociatedToSttTrack[ncand]+
		    nMvdStripHitsAssociatedToSttTrack[ncand] ==0){ // in this case  don't refit
			ALFA[ncand]=-2.*Ox[ncand];
			BETA[ncand]=-2.*Oy[ncand];
			GAMMA[ncand]= Ox[ncand]*Ox[ncand]+Oy[ncand]*Oy[ncand]-R[ncand]*R[ncand];
			noMvdhits[ncand]=false;
if(istampa>=2) cout<<"da PndSttMvdTracking ncand = "
	<<ncand<<" non rifittato perche' non ha Mvd hits associati;"<<endl;
			continue;
		}
		noMvdhits[ncand]=true;

if(istampa>2) cout<<"da PndSttMvdTracking ncand = "<<ncand<<",  Ox[ncand] = "<<Ox[ncand]<<",  Oy[ncand] = "<<Oy[ncand]
	<<",  R[ncand] = "<<R[ncand]<<endl;
		Double_t rotationangle= atan2( Py[ncand], Px[ncand]); // Py/Px = m in
								//  v = m*u + q formula
		if( rotationangle<0. ) rotationangle += 2.*PI;

//---------- translation of the reference system in the best Mvd hit position
		for(i=0, diff = 1.e20;i<nTrackCandHit[ncand];i++){
			if( ListTrackCandHitType[ncand][i] > 1) continue;
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

		}

		if( diff > 0.5 ) {
			trajectory_vertex[0]=trajectory_vertex[1]=0.;
			iexcl=-1;
		}
		RefitMvdStt(
			&nTrackCandHit[ncand], //this is both input and output after the new fit
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


	}	// end of for(ncand=0; ncand< nTotalCandidates; ncand++)



//--------------------- end of  refit the Helix in XY plane using Stt + Mvd associated hits


//---------------------   here call to the function that matches Mvd hits with Stt hits
//	for the second time, after first refit
   delta=0.5; //  parameter of proximity for associating Mvd hits to Stt tracks
   highqualitycut=0.2; //  parameter of proximity for associating Mvd hits to Stt tracks



   MatchMvdHitsToSttTracksagain(
			delta,
			highqualitycut,
			nSttTrackCand,
			Ox,
			Oy,
			R,
			FI0,
			Fifirst,
			CHARGE,
			nMvdPixelHitsAssociatedToSttTrack, // input and output
			ListMvdPixelHitsAssociatedToSttTrack, // input and output
			nMvdStripHitsAssociatedToSttTrack, // input and output
			ListMvdStripHitsAssociatedToSttTrack // input and output
			);





//---------------------   here do the fit again in the SZ space if there are Mvd hits.
//			   For this, reordering of the
//			  Mvd hits is not necessary. Using here only the Stt Skew hits obviously.

	for(ncand=0; ncand< nTotalCandidates; ncand++){

		if( nMvdPixelHitsAssociatedToSttTrack[ncand]+
			nMvdStripHitsAssociatedToSttTrack[ncand] == 0){
			noMvdhits[ncand]=false;
			continue;
		}
		noMvdhits[ncand]=true;




		// adding the Mvd Pixels hit
		for(i=0; i< nMvdPixelHitsAssociatedToSttTrack[ncand]; i++){
			k=ListMvdPixelHitsAssociatedToSttTrack[ncand][i];
			ZEDbis[i][0] = ZED[i] = ZMvdPixel[k];
			S[i] = atan2( YMvdPixel[k]-Oy[ncand],XMvdPixel[k]-Ox[ncand]);
			if(S[i]<0.) S[i] +=2.*PI;
			Sbis[i][0] = S[i];
			DriftRadiusbis[i][0]=DriftRadius[i]=-1.;
			ErrorDriftRadiusbis[i][0]=ErrorDriftRadius[i]= 0.01 ;
		}
		// adding the Mvd Strips hit
		for(j=0; j< nMvdStripHitsAssociatedToSttTrack[ncand]; j++){
			i = nMvdPixelHitsAssociatedToSttTrack[ncand]+j;
			k=ListMvdStripHitsAssociatedToSttTrack[ncand][j];
			ZEDbis[i][0] = ZED[i] = ZMvdStrip[k];
			S[i] = atan2( YMvdStrip[k]-Oy[ncand],XMvdStrip[k]-Ox[ncand]);
			if(S[i]<0.) S[i] +=2.*PI;
			Sbis[i][0] = S[i] ;
			DriftRadiusbis[i][0]=DriftRadius[i]=-1.;
			ErrorDriftRadiusbis[i][0]=ErrorDriftRadius[i]= 0.01 ;
		}




		for(j=0, i = nMvdPixelHitsAssociatedToSttTrack[ncand]+
				nMvdStripHitsAssociatedToSttTrack[ncand];
				j<nSttSkewHitsinTrack[ncand]; j++){
			k=ListSttSkewHitsinTrack[ncand][j];
			kall = nMvdPixelHitsAssociatedToSttTrack[ncand]+
				nMvdStripHitsAssociatedToSttTrack[ncand]+j;
			CalculateSandZ( Ox[ncand],
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
					ZEDbis[kall][0]=ZED[i]=z[0];
					Sbis[kall][0]=S[i] = s[0];
					DriftRadiusbis[kall][0]=DriftRadius[i]=zdrift[0];
					if( fabs(zdrift[0]) >1.e-10) {
					 ErrorDriftRadiusbis[kall][0]=ErrorDriftRadius[i]=zdrift[0];
					} else {
					 ErrorDriftRadiusbis[kall][0]=ErrorDriftRadius[i]=0.5;
					}
					i++;

					ZEDbis[kall][1]=ZED[i]=z[1];
					Sbis[kall][1]=S[i] = s[1];
					DriftRadiusbis[kall][1]=DriftRadius[i]=zdrift[1];
					if( fabs(zdrift[1]) >1.e-10) {
					 ErrorDriftRadiusbis[kall][1]=ErrorDriftRadius[i]=zdrift[1];
					} else {
					 ErrorDriftRadiusbis[kall][1]=ErrorDriftRadius[i]=0.5;
					}
					i++;

				}else if( z[0]<999998.){
					ZEDbis[kall][0]=ZED[i]=z[0];
					Sbis[kall][0]=S[i] = s[0];
					DriftRadiusbis[kall][0]=DriftRadius[i]=zdrift[0];
					if( fabs(zdrift[0]) >1.e-10) {
					 ErrorDriftRadiusbis[kall][0]=ErrorDriftRadius[i]=zdrift[0];
					} else {
					 ErrorDriftRadiusbis[kall][0]=ErrorDriftRadius[i]=0.5;
					}
					ZEDbis[kall][1]=999999.;
					i++;
				} else if( z[1]<999998.){
					ZEDbis[kall][1]=ZED[i]=z[1];
					Sbis[kall][1]=S[i] = s[1];
					DriftRadiusbis[kall][1]=DriftRadius[i]=zdrift[1];
					if( fabs(zdrift[1]) >1.e-10) {
					 ErrorDriftRadiusbis[kall][1]=ErrorDriftRadius[i]=zdrift[1];
					} else {
					 ErrorDriftRadiusbis[kall][1]=ErrorDriftRadius[i]=0.5;
					}
					ZEDbis[kall][0]=999999.;
					i++;
				} else {
					ZEDbis[kall][1]=ZEDbis[kall][0]=999999.;
				}


		}	//   end of   for(j=0, i = nMvdPixelHitsAssociatedToSttTrack[ncand]+


			j=nMvdPixelHitsAssociatedToSttTrack[ncand]+
			nMvdStripHitsAssociatedToSttTrack[ncand];

		resultFitSZagain[ncand] = FitSZspace(
					j,	//  only the Mvd hits
					S,
					ZED,
					DriftRadius,
					ErrorDriftRadius,
					FI0[ncand],
					20,	// maximum number allowed in the fit
					&emme
						);


		if( resultFitSZagain[ncand]==1) KAPPA[ncand] = emme;


//---------------------   end of the call to the function that fits again in the SZ space


//	use the risult just obtained from the fit in SZ to reject the spurious Skew Straw hits
//	and the Mvd spurious hits; also in this function there is the calculation of the Z position
//	of the SKEW hits and the MVD hits, for a given track candidate (ie for a given Helix
//	circle in the XY plane)


		EliminateSpuriousSZ(
				&nMvdPixelHitsAssociatedToSttTrack[ncand],	// input and output
				&ListMvdPixelHitsAssociatedToSttTrack[ncand][0],// input and output
				&nMvdStripHitsAssociatedToSttTrack[ncand],	// input and output
				&ListMvdStripHitsAssociatedToSttTrack[ncand][0],// input and output
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
				FI0[ncand]
				    );

	}	//  end of for(ncand=0; ncand< nTotalCandidates; ncand++)






//----------------------------------------------------------

//  Questo va razionalizzato maggiormente dopo Groningen!! Riordino degli hits, per la seconda volta.


//     ordering all the hits belonging to the candidate track, by increasing R;
//     forming the new track with Mvd+Stt hits

	nTotalCandidates = nSttTrackCand;  //  this may change in the future

	for(ncand=0; ncand< nTotalCandidates; ncand++){
		nTrackCandHit[ncand] =nSttParHitsinTrack[ncand]+nSttSkewHitsinTrack[ncand]+
					nMvdPixelHitsAssociatedToSttTrack[ncand]+
					nMvdStripHitsAssociatedToSttTrack[ncand];
		UShort_t tempmvdindex[nMvdPixelHitsAssociatedToSttTrack[ncand]+
					nMvdStripHitsAssociatedToSttTrack[ncand] ],
			 tempmvdtype[nMvdPixelHitsAssociatedToSttTrack[ncand]+
					nMvdStripHitsAssociatedToSttTrack[ncand] ],
			auxIndex[nMvdPixelHitsAssociatedToSttTrack[ncand]+
					nMvdStripHitsAssociatedToSttTrack[ncand] ];
		Double_t auxR[nMvdPixelHitsAssociatedToSttTrack[ncand]+
					nMvdStripHitsAssociatedToSttTrack[ncand] ];
		// adding the Mvd hits (Pixel and Strips)
		for(i=0; i< nMvdPixelHitsAssociatedToSttTrack[ncand]; i++){
			auxR[i] =
			 XMvdPixel[ ListMvdPixelHitsAssociatedToSttTrack[ncand][i] ]*
			 XMvdPixel[ ListMvdPixelHitsAssociatedToSttTrack[ncand][i] ]+
			 YMvdPixel[ ListMvdPixelHitsAssociatedToSttTrack[ncand][i] ]*
			 YMvdPixel[ ListMvdPixelHitsAssociatedToSttTrack[ncand][i] ];
			tempmvdindex[i]=ListMvdPixelHitsAssociatedToSttTrack[ncand][i];
			tempmvdtype[i]=0;
			auxIndex[i] = i;
		}
		for(i=0; i< nMvdStripHitsAssociatedToSttTrack[ncand]; i++){
			auxR[i+nMvdPixelHitsAssociatedToSttTrack[ncand]] =
			 XMvdStrip[ ListMvdStripHitsAssociatedToSttTrack[ncand][i] ]*
			 XMvdStrip[ ListMvdStripHitsAssociatedToSttTrack[ncand][i] ]+
			 YMvdStrip[ ListMvdStripHitsAssociatedToSttTrack[ncand][i] ]*
			 YMvdStrip[ ListMvdStripHitsAssociatedToSttTrack[ncand][i] ];
			tempmvdindex[i+nMvdPixelHitsAssociatedToSttTrack[ncand]]=
				ListMvdStripHitsAssociatedToSttTrack[ncand][i];
			tempmvdtype[i+nMvdPixelHitsAssociatedToSttTrack[ncand]]=1;
			auxIndex[i+nMvdPixelHitsAssociatedToSttTrack[ncand]]=
				i+nMvdPixelHitsAssociatedToSttTrack[ncand];
		}

		//  ordering the Mvd Hits
		if( nMvdPixelHitsAssociatedToSttTrack[ncand]+
		    nMvdStripHitsAssociatedToSttTrack[ncand] >0){
			Merge_Sort( nMvdPixelHitsAssociatedToSttTrack[ncand]+
		                    nMvdStripHitsAssociatedToSttTrack[ncand],
				    auxR, auxIndex);

		//  constructing the first part of the ordered new Track  Candidate
			for(i=0; i< nMvdPixelHitsAssociatedToSttTrack[ncand]+
			    nMvdStripHitsAssociatedToSttTrack[ncand]; i++){
				ListTrackCandHit[ncand][i] = tempmvdindex[ auxIndex[i] ];
				ListTrackCandHitType[ncand][i] = tempmvdtype[ auxIndex[i] ];

			}
		}	// end of  if( nMvdPixelHitsAssociatedToSttTrack[ncand]+

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
				i = j+nMvdPixelHitsAssociatedToSttTrack[ncand]+
				    nMvdStripHitsAssociatedToSttTrack[ncand];
				ListTrackCandHit[ncand][i] = tempmvdindex2[ auxIndex2[j] ];
				ListTrackCandHitType[ncand][i] = tempmvdtype2[ auxIndex2[j] ];

			}
		}	// end of  if( nSttParHitsinTrack[ncand]+




	} //   end of  for(ncand=0; ncand< nTotalCandidates; ncand++)


//-------------------- end of ordering









//----------stampaggi
if(istampa>2){
for(int it=0; it<nSttTrackCand;it++){
cout<<"\tTrack Cand n. "<<it;
cout<<"\n\t nMvdPixelHitsAssociatedToSttTrack "<<nMvdPixelHitsAssociatedToSttTrack[it]
	<<" e loro elenco :\n"<<endl;
for(l =0;l<nMvdPixelHitsAssociatedToSttTrack[it];l++){
	cout<<"\tMvd Pixel hit n. "<<ListMvdPixelHitsAssociatedToSttTrack[it][l]<<endl;
};
cout<<"\n\t nMvdStripHitsAssociatedToSttTrack "<<nMvdStripHitsAssociatedToSttTrack[it]
	<<" e loro elenco :\n"<<endl;
for(l =0;l<nMvdStripHitsAssociatedToSttTrack[it];l++){
	cout<<"\tMvd Strip hit n. "<<ListMvdStripHitsAssociatedToSttTrack[it][l]<<endl;
};
cout<<"\n\t nSttSkewHitsinTrack "<<nSttSkewHitsinTrack[it]<<" e loro elenco :\n"<<endl;
for(l =0;l<nSttSkewHitsinTrack[it];l++){
	cout<<"\tstt hit skew n. "<<ListSttSkewHitsinTrack[it][l]<<endl;
};

}

}



//----- end stampaggi



//------------------  section with comparison MC Mvd hits - associated hits to a certain track

//--------------------  comparison of the PndTrackCand from PR of the STT to the MC truth
//    associate the tracks found with Pattern Recognition to the MC tracks



//  the following method associates the found tracks (for now : the STT found tracks) to the
//  MC tracks, creating a bilinear correspondence between MC tracks and PR Found tracks

   if( nMCTracks >0 && nSttTrackCand > 0 ){



         AssociateFoundTrackstoMCtris(
		  info,
		  Ox,
		  Oy,
		  R,
                  nSttTrackCand,
                  nSttParHitsinTrack,
                  ListSttParHitsinTrack,
                  nSttSkewHitsinTrack,
                  ListSttSkewHitsinTrack,
                  daTrackFoundaTrackMC
                                   );

   }	// endo of if( nMCTracks >0 && nSttTrackCand > 0 )

if(istampa>2){
	for(i=0;i<nSttTrackCand;i++){
		cout<<"da PndSttMvdTracking : Stt track candidate n. "<<i
		<<"  associated to MC Track n. "<<daTrackFoundaTrackMC[i]<<endl;
	}
}

//  the following method counts the matched to MC and spurious hits for all STT  Found tracks and
//  also the ghost tracks

   SttMatchedSpurious(
			nSttHit,
			info,
                        nSttTrackCand,
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


//  the following method associates the Mvd hits to corresponding MC tracks

   Short_t	FromPixeltoMCTrack[nMvdPixelHit],
		FromStriptoMCTrack[nMvdStripHit];

//----------- only for comparison to the MC truth
   MvdMatchtoMC(
		nMvdMCPoint,
		XMvdMCPoint,
		YMvdMCPoint,
		ZMvdMCPoint,
		MCPointtoMCTrackID,
		FromPixeltoMCTrack,	// output
		FromStriptoMCTrack	// output
		);
//--------------------







//	assumo che la traccia MC associata alla traccia Stt sia quella giusta e di
//	conseguenza calcolo gli hits Mvd spuri e comuni
	UShort_t	nMvdPixelCommon[nSttTrackCand],
			MvdPixelCommonList[nSttTrackCand][nMvdPixelHit],
			nMvdPixelSpuriinTrack[nSttTrackCand],
			MvdPixelSpuriList[nSttTrackCand][nMvdPixelHit],
			nMCMvdPixelAlone[nSttTrackCand],
			MCMvdPixelAloneList[nSttTrackCand][nMvdPixelHit],

			nMvdStripCommon[nSttTrackCand],
			MvdStripCommonList[nSttTrackCand][nMvdStripHit],
			nMvdStripSpuriinTrack[nSttTrackCand],
			MvdStripSpuriList[nSttTrackCand][nMvdStripHit],
			nMCMvdStripAlone[nSttTrackCand],
			MCMvdStripAloneList[nSttTrackCand][nMvdStripHit];


   MvdMatchedSpurioustoTrackCand(
			nSttTrackCand,			// input
			&daTrackFoundaTrackMC[0],	// input

//			nMvdPixelHit,			// input
			&FromPixeltoMCTrack[0],		// input
//			nMvdStripHit,			// input
			&FromStriptoMCTrack[0],		// input

			&nMvdPixelHitsAssociatedToSttTrack[0],	// input
			ListMvdPixelHitsAssociatedToSttTrack,	// input
			&nMvdStripHitsAssociatedToSttTrack[0],	// input
			ListMvdStripHitsAssociatedToSttTrack,	// input

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
if(istampa>2){

     for(ncand=0; ncand< nSttTrackCand; ncand++){
//----------------- ora la traccia MC corrispondente a questa traccia Stt
		cout<<"da PndSttMvdTracking : la PndTrackCand n. "<<ncand;
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
		nMvdPixelHitsAssociatedToSttTrack[ncand]<<
		" Pixel Hits associati, di cui ecco la lista :\n";
		for(j=0;j<nMvdPixelHitsAssociatedToSttTrack[ncand];j++){
			cout<<"\tHit n. "<<
			ListMvdPixelHitsAssociatedToSttTrack[ncand][j]<<
			" che fu associato a MC track "
			<<FromPixeltoMCTrack[
			  ListMvdPixelHitsAssociatedToSttTrack[ncand][j]
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
		nMvdStripHitsAssociatedToSttTrack[ncand]<<
		" Strip Hits associati, di cui ecco la lista :\n";
		if(nMvdStripHitsAssociatedToSttTrack[ncand]==0) cout<<"\t-----------nessuna lista\n";
		for(j=0;j<nMvdStripHitsAssociatedToSttTrack[ncand];j++){
			cout<<"\tHit n. "<<
			ListMvdStripHitsAssociatedToSttTrack[ncand][j]<<
			" che fu associato a MC track "
			<<FromStriptoMCTrack[
			  ListMvdStripHitsAssociatedToSttTrack[ncand][j]
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
/*
         TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
         TParticlePDG *fParticle= fdbPDG->GetParticle(icode);
         if (icode>1000000000) carica = 1.;
         else  carica = fParticle->Charge()/3. ;    //   charge of track
         Cx = Oxx + Pyy*1000./(BFIELD*CVEL*carica);
         Cy = Oyy - Pxx*1000./(BFIELD*CVEL*carica);
         Fifi = atan2(Cy, Cx);       // MC truth Fifi angle of circle of Helix trajectory
         if(Fifi<0.)  Fifi += 2.*PI;
         Double_t Kakka ;
         if( fabs( pMCtr->GetMomentum().Z() )< 1.e-20) Kakka = 99999999.;
         else  Kakka = -carica*0.001*BFIELD*CVEL/pMCtr->GetMomentum().Z();
*/
	for(int ic=0;ic<nSttHit;ic++){
		if( ( (int) (info[ic][6]+0.1) ) == i   && info[ic][5]<2.){
			citata++;
//			goto putta;
		}
//		putta: ;
	}
	if( citata>2 && fabs(Oxx)<1. && fabs(Oyy) < 1. ) {
		nMCTracksaccettabili++;
//		cout<<"\tla traccia MC ha "<<citata<<" Stt || hits, e' accettabile\n";
	}

/*
cout<<"perbacco, Oxx = "<<Oxx<<", Oyy = "<<Oyy<<", R = "<<Rr<<", Px = "<<Pxx<<", Py = "<<Pyy;
if( citata) cout<<" ed ha Stt || hits\n";
else   cout<<"  e non ha Stt || hits\n";
*/

}


//----------- fine conteggio delle tracce MC accettabili






//  if(istampa>=1 )  fprintf(HANDLE, "\n Evento %d  NTotaleTracceMC %d ------\n",IVOLTE, nMCTracks);
  if(istampa>=1 ) {

   fprintf(HANDLE, "\n Evento %d  NTotaleTracceMC %d ------\n",IVOLTE,
  	nMCTracksaccettabili);
}
int ii;
Double_t HoughFiii;

for (ii=0; ii<nSttTrackCand && istampa>=1 ;ii++){
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
   Double_t dista=sqrt( Ox[ii]*Ox[ii]+Oy[ii]*Oy[ii] );
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

if( istampa>=1){
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


//------------------  end section with comparison MC Mvd hits - associated hits to a certain track








//-------  load the new PndTrackCand ; each track has the STT and the Mvd hits associated
//-------  also load the new PndTrack ; each track has the STT and the Mvd hits associated

	Double_t Oxx, Oyy;
	for(ncand=0, ipinco = 0; ncand< nTotalCandidates; ncand++){
		if(!status[ncand]) continue;
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


		for(j=0; j< nTrackCandHit[ncand]; j++){
		     switch (ListTrackCandHitType[ncand][j]){
			case 0:
			   pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId("MVDHitsPixel"),(Int_t)ListTrackCandHit[ncand][j],j);
			break;
			case 1:
			   pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId("MVDHitsStrip"),(Int_t)ListTrackCandHit[ncand][j],j);
			break;
			case 2:
			   pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId("STTHit"),(Int_t)ListTrackCandHit[ncand][j],j);
			break;
			case 3:
			   pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId("STTHit"),(Int_t)ListTrackCandHit[ncand][j],j);
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

if(istampa>2) cout<<" evento = "<<IVOLTE<<", track cand n. "<<ncand<<endl<<
	"\tfirst hit, tipo hit = "<<ListTrackCandHitType[ncand][0]<<", X = "<<Posiz1[0]
	<<", Y = "<<Posiz1[1]<<", Z = "<<Posiz1[2]<<
	"\tPx = "<<px<<", Py = "<<py<<", Pz = "<<Pzini<<", Ptras "<<Ptras<<endl
	<<"\t ErrX = "<<ErrPosition[0]
	<<", ErrY = "<<ErrPosition[1]<<", ErrZ = "<<ErrPosition[2]<<endl<<
	"\tErrPx = "<<ErrMomentum.X()<<", ErrPy = "<<ErrMomentum.Y()<<", ErrPz = "<<ErrMomentum.Z()<<endl;

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

if(istampa>2) cout<<" evento = "<<IVOLTE<<", track cand n. "<<ncand<<endl<<
	"\tlast hit, tipo hit = "<<ListTrackCandHitType[ncand][k]
	<<", X = "<<Posiz1[0]
	<<", Y = "<<Posiz1[1]<<", Z = "<<Posiz1[2]<<
	"\tPx = "<<px<<", Py = "<<py<<", Pz = "<<Pzini<<", Ptras "<<Ptras<<endl
	<<"\t ErrX = "<<ErrPosition[0]
	<<", ErrY = "<<ErrPosition[1]<<", ErrZ = "<<ErrPosition[2]<<endl<<
	"\tErrPx = "<<ErrMomentum.X()<<", ErrPy = "<<ErrMomentum.Y()<<", ErrPz = "<<ErrMomentum.Z()<<endl;

		FairTrackParP last( Position,  Momentum,
		   ErrPosition, ErrMomentum, CHARGE[ncand],
		   Position,
		   TVector3(py/Ptras, -px/Ptras, 0.), //  first vector defining the plane
		   TVector3(Pzini*px/ddd,Pzini*py/ddd,-Ptras*Ptras/ddd) //second vector defining the plane
						);




		PndTrack *pTrck = new((*fSttMvdPndTrackArray)[ipinco]) PndTrack(first,last,*pTrckCand);
//		PndTrack *pTrck = (PndTrack*) fSttMvdPndTrackArray->At(ipinco);
		pTrck->SetRefIndex(ipinco);

		ipinco++;
	}	// end of     for(ncand=0, ipinco = 0; ncand< nTotalCandidates; ncand++)

//--------------------- end of load the new PndTrackCand

















//------------------------------   plottamenti --------------------------------------------



if(istampa>=2){
for(int it=0; it<nSttTrackCand;it++){
cout<<"da PndSttMvdTracking, IVOLTE = "<<IVOLTE<<", traccia n. "<<it<<", elenco ordinato degli hits :\n";
for(l =0;l<nMvdPixelHitsAssociatedToSttTrack[it]+nMvdStripHitsAssociatedToSttTrack[it]
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


    for(  i= 0; i< nSttTrackCand; i++){


	int npunti=-1+nSttParHitsinTrack[i]+nSttSkewHitsinTrack[i]+
		nMvdPixelHitsAssociatedToSttTrack[i]+nMvdStripHitsAssociatedToSttTrack[i];

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








	if( nSttParHitsinTrack[i]+nMvdPixelHitsAssociatedToSttTrack[i]+
		nMvdStripHitsAssociatedToSttTrack[i]>0) {

	   for( j=0;j<nMCSkewAlone[i];j++){
	   	MCSkewAloneX[ MCSkewAloneList[i][j] ]=pSttMCPoint[i]->GetX();
	   	MCSkewAloneY[ MCSkewAloneList[i][j] ]=pSttMCPoint[i]->GetY();
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

		nMvdPixelHitsAssociatedToSttTrack[i],
		ListMvdPixelHitsAssociatedToSttTrack,
		nMvdStripHitsAssociatedToSttTrack[i],
		ListMvdStripHitsAssociatedToSttTrack,

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
		nMCSkewAlone[i],
		MCSkewAloneList,
		MCSkewAloneX,
		MCSkewAloneY
				);

	}	// end of  if ( nSttParHitsinTrack[i]+ ...
      if(  nSttSkewHitsinTrack[i]+nMvdPixelHitsAssociatedToSttTrack[i]+
      	nMvdStripHitsAssociatedToSttTrack[i]>0){
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
		   nMvdPixelHitsAssociatedToSttTrack,
		   ListMvdPixelHitsAssociatedToSttTrack,
		   nMvdStripHitsAssociatedToSttTrack,
		   ListMvdStripHitsAssociatedToSttTrack,


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
      }
    }            //   end of   for(  i= 0; i< nSttTrackCand; i++)
i=0;



        WriteMacroParallelHitsGeneral(
                   nSttHit, info,
		   nSttTrackCand,Ox,Oy,R,
		   FI0,
		   ultimoangolo,
		   primoangolo
					);





        WriteMacroAllHitsRestanti(
		nSttHit,
		nSttParHit,
		nSttSkewHit,
		info,
		nSttTrackCand,
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
UShort_t ListMvdPixelHitsAssociatedToSttTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHits],
	UShort_t nMvdStripHitsAssociatedToSttTra,
UShort_t ListMvdStripHitsAssociatedToSttTrack[MAXTRACKSPEREVENT][nmaxMvdStripHits],

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
		Double_t SchosenSkew[nmaxSttHits],
		UShort_t nSkewCommon[MAXTRACKSPEREVENT],
		UShort_t SkewCommonList[MAXTRACKSPEREVENT][nmaxSttHits],
		UShort_t nMCSkewAlone,
		UShort_t MCSkewAloneList[MAXMCTRACKS][nmaxSttHits],
		Double_t MCSkewAloneX[nmaxSttHits],
		Double_t MCSkewAloneY[nmaxSttHits]
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
      sprintf(nome,"MacroAllSttMvdwithMCEvent%dT%d", IVOLTE,iTrack);
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
       for( ii=0; ii< nSkewHitsinTrack; ii++) {
            i = ListSkewHitsinTrack[iTrack][ii] ;
            if (info[i][0]-info[i][3] < xmin)   xmin = info[i][0]-info[i][3];
            if (info[i][0]+info[i][3] > xmax)   xmax = info[i][0]+info[i][3];
            if (info[i][1]-info[i][3] < ymin)   ymin = info[i][1]-info[i][3];
            if (info[i][1]+info[i][3] > ymax)   ymax = info[i][1]+info[i][3];
       }
//       for( ii=0; ii< nMvdStripHit; ii++) {
       for( i=0; i< nMvdStripHitsAssociatedToSttTra; i++) {
        ii = ListMvdStripHitsAssociatedToSttTrack[iTrack][i];
            if (XMvdStrip[ii] < xmin)   xmin = XMvdStrip[ii];
            if (XMvdStrip[ii] > xmax)   xmax = XMvdStrip[ii] ;
            if (YMvdStrip[ii] < ymin)   ymin = YMvdStrip[ii];
            if (YMvdStrip[ii] > ymax)   ymax = YMvdStrip[ii];
       }
//       for( ii=0; ii< nMvdPixelHit; ii++) {
       for( i=0; i< nMvdPixelHitsAssociatedToSttTra; i++) {
        ii = ListMvdPixelHitsAssociatedToSttTrack[iTrack][i];
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

       fprintf(MACRO,"TEllipse* TC = new TEllipse(%f,%f,%f,%f,%f,%f);\n",Ox,Oy,R,R,primoangolo,ultimoangolo);

       fprintf(MACRO,"TC->SetLineColor(2);\nTC->SetFillStyle(0);\nTC->Draw(\"only\");\n");

       fprintf(MACRO,"TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n",xmin,0.,xmax,0.,xmin,xmax);
       fprintf(MACRO,"Assex->Draw();\n");
       fprintf(MACRO,"TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", 0.,ymin,0.,ymax,ymin,ymax);
       fprintf(MACRO,"Assey->Draw();\n");


//----------------------
/*
       for( ii=0; ii< Nhits; ii++) {
            i = ListHitsinTrack[iTrack][ii] ;
            fprintf(MACRO,"TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nE%d->SetFillStyle(0);\nE%d->Draw();\n",
                     i,info[i][0],info[i][1],info[i][3],info[i][3],i,i);
       }

*/
//------------- hits paralleli in comune con traccia MC
       for( ii=0; ii< nParalCommon[iTrack]; ii++) {
            i = ParalCommonList[iTrack][ii] ;
            fprintf(MACRO,
  "TEllipse* EC%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nEC%d->SetFillStyle(0);\nEC%d->Draw();\n",
                     i,info[i][0],info[i][1],info[i][3],info[i][3],i,i);
       }
//------------- hits paralleli spuri
       for( ii=0; ii< nSpuriParinTrack[iTrack]; ii++) {
            i = ParSpuriList[iTrack][ii] ;
            fprintf(MACRO,
   "TEllipse* ES%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nES%d->SetFillStyle(0);\nES%d->SetLineColor(2);\nES%d->Draw();\n",
                     i,info[i][0],info[i][1],info[i][3],info[i][3],i,i,i);
       }
//------------- hits paralleli MC 'alone'
       for( ii=0; ii< nMCParalAlone[iTrack]; ii++) {
            i = MCParalAloneList[iTrack][ii] ;
            fprintf(MACRO,
   "TEllipse* EA%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nEA%d->SetFillStyle(0);\nEA%d->SetLineColor(4);\nEA%d->Draw();\n",
                     i,info[i][0],info[i][1],info[i][3],info[i][3],i,i,i);
       }

//-------------








//------------- hits skew in comune e spuri di traccia MC

       for( i=0; i< nSkewHitsinTrack; i++) {
        ii = ListSkewHitsinTrack[iTrack][i];
	    aaa=Ox+R*cos(SchosenSkew[ii]);
	    bbb=Oy+R*sin(SchosenSkew[ii]);

            fprintf(MACRO,"TMarker* SS%d = new TMarker(%f,%f,%d);\n",
                    ii,aaa,bbb,28);

		for( int k=0; k<nSkewCommon[iTrack];k++){
			if( SkewCommonList[iTrack][k]== ii){
		fprintf(MACRO,"SS%d->SetMarkerColor(1);\nSS%d->Draw();\n",
                    ii,ii);
				goto punco ;
			}
		}
		fprintf(MACRO,"SS%d->SetMarkerColor(2);\nSS%d->Draw();\n",ii,ii);
punco: ;

       }
//------------- hits paralleli MC 'alone'
       for( ii=0; ii< nMCSkewAlone; ii++) {
            i = MCSkewAloneList[iTrack][ii] ;
           fprintf(MACRO,
   "TMarker* SSA%d = new TMarker(%f,%f,%d);\nSSA%d->SetMarkerColor(4);\nSSA%d->Draw();\n",
                     i,MCSkewAloneX[i],MCSkewAloneY[i],28,i,i);
       }

//------------- now the Strips

       for( i=0; i< nMvdStripHitsAssociatedToSttTra; i++) {
        ii = ListMvdStripHitsAssociatedToSttTrack[iTrack][i];
            x1= XMvdStrip[ii]-sigmaXMvdStrip[ii];
            x2= XMvdStrip[ii]+sigmaXMvdStrip[ii];
            y1= YMvdStrip[ii]-sigmaYMvdStrip[ii];
            y2= YMvdStrip[ii]+sigmaYMvdStrip[ii];

            fprintf(MACRO,"TMarker* BS%d = new TMarker(%f,%f,%d);\n",
                    ii,XMvdStrip[ii],YMvdStrip[ii],25);
		for( int k=0; k<nMvdStripCommon;k++){
			if( MvdStripCommonList[k]== ii){
		fprintf(MACRO,"BS%d->SetMarkerColor(1);\nBS%d->Draw();\n",
                    ii,ii);
				goto pinco ;
			}
		}
		fprintf(MACRO,"BS%d->SetMarkerColor(2);\nBS%d->Draw();\n",ii,ii);
pinco: ;

       }

//-------------- hit Mvd Strips 'Alone'
       for( ii=0; ii< nMCMvdStripAlone; ii++) {
            i = MCMvdStripAloneList[ii] ;
            fprintf(MACRO,
   "TMarker* BSA%d = new TMarker(%f,%f,%d);\nBSA%d->SetMarkerColor(4);\nBSA%d->Draw();\n",
                     i,XMvdStrip[i],YMvdStrip[i],25,i,i);
       }
//-------------


//       for( ii=0; ii< nMvdPixelHit; ii++) {
       for( i=0; i< nMvdPixelHitsAssociatedToSttTra; i++) {
        ii = ListMvdPixelHitsAssociatedToSttTrack[iTrack][i];
            x1= XMvdPixel[ii]-sigmaXMvdPixel[ii];
            x2= XMvdPixel[ii]+sigmaXMvdPixel[ii];
            y1= YMvdPixel[ii]-sigmaYMvdPixel[ii];
            y2= YMvdPixel[ii]+sigmaYMvdPixel[ii];
//            fprintf(MACRO,"TBox* BP%d = new TBox(%f,%f,%f,%f);\nBP%d->SetFillColor(4);\nBP%d->Draw();\n",
//                     ii,x1,y1,x2,y2,ii,ii);

            fprintf(MACRO,"TMarker* BP%d = new TMarker(%f,%f,%d);\n",
                    ii,XMvdPixel[ii],YMvdPixel[ii],26);
		for( int k=0; k<nMvdPixelCommon;k++){
			if( MvdPixelCommonList[k]== ii){
 fprintf(MACRO,"BP%d->SetMarkerColor(1);\nBP%d->Draw();\n",
                    ii,ii);
				goto panco ;
			}
		}
 fprintf(MACRO,"BP%d->SetMarkerColor(2);\nBP%d->Draw();\n",ii,ii);
panco: ;		

       }

//-------------- hit Mvd Pixel 'Alone'
       for( ii=0; ii< nMCMvdPixelAlone; ii++) {
            i = MCMvdPixelAloneList[ii] ;
            fprintf(MACRO,
   "TMarker* BPA%d = new TMarker(%f,%f,%d);\nBPA%d->SetMarkerColor(4);\nBPA%d->Draw();\n",
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
           	Cx = Oxx + Py*1000./(BFIELD*CVEL*carica);
           	Cy = Oyy - Px*1000./(BFIELD*CVEL*carica);
            	fprintf(MACRO,
//"TEllipse* MC%d = new TEllipse(%f,%f,%f,%f,%f,%f);\nMC%d->SetFillStyle(0);\nMC%d->SetLineColor(3);\nMC%d->Draw(\"only\");\n",
//                     im,Cx,Cy,Rr,Rr,primoangolo,ultimoangolo,im,im,im);
"TEllipse* MC%d = new TEllipse(%f,%f,%f,%f,%f,%f);\nMC%d->SetFillStyle(0);\nMC%d->SetLineColor(3);\nMC%d->Draw(\"only\");\n",
                     im,Cx,Cy,Rr,Rr,0.,360.,im,im,im);
	}
       }
//----------- fine parte del MC

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


/*
		   UShort_t nMvdPixelHit
		   Double_t* XMvdPixel,
		   Double_t* sigmaXMvdPixel,
		   Double_t* YMvdPixel,
		   Double_t* sigmaYMvdPixel,
		   Double_t* ZMvdPixel,
		   Double_t* sigmaZMvdPixel,
		   UShort_t nMvdStripHit,
		   Double_t* XMvdStrip,
		   Double_t* sigmaXMvdStrip,
		   Double_t* YMvdStrip,
		   Double_t* sigmaYMvdStrip,
		   Double_t* ZMvdStrip,
		   Double_t* sigmaZMvdStrip
*/

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
            fprintf(MACRO,"TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nE%d->SetFillStyle(0);\nE%d->Draw();\n",
                     i,info[i][0],info[i][1],info[i][3],info[i][3],i,i);
          } else {	// skew straws.
	    fprintf(MACRO,"TMarker* SS%d = new TMarker(%f,%f,%d);\nSS%d->SetMarkerColor(1);\nSS%d->Draw();\n",
		i,info[i][0],info[i][1],28,i,i,i);
	  }
       }

       for( ii=0; ii< nMvdStripHit; ii++) {
            x1= XMvdStrip[ii]-sigmaXMvdStrip[ii];
            x2= XMvdStrip[ii]+sigmaXMvdStrip[ii];
            y1= YMvdStrip[ii]-sigmaYMvdStrip[ii];
            y2= YMvdStrip[ii]+sigmaYMvdStrip[ii];
//            fprintf(MACRO,"TBox* BS%d = new TBox(%f,%f,%f,%f);\nBS%d->SetFillColor(2);\nBS%d->Draw();\n",
//                     ii,x1,y1,x2,y2,ii,ii);
            fprintf(MACRO,"TMarker* BS%d = new TMarker(%f,%f,%d);\nBS%d->SetMarkerColor(1);\nBS%d->Draw();\n",
                    ii,XMvdStrip[ii],YMvdStrip[ii],25,ii,ii,ii);


       }
       for( ii=0; ii< nMvdPixelHit; ii++) {
            x1= XMvdPixel[ii]-sigmaXMvdPixel[ii];
            x2= XMvdPixel[ii]+sigmaXMvdPixel[ii];
            y1= YMvdPixel[ii]-sigmaYMvdPixel[ii];
            y2= YMvdPixel[ii]+sigmaYMvdPixel[ii];
//            fprintf(MACRO,"TBox* BP%d = new TBox(%f,%f,%f,%f);\nBP%d->SetFillColor(4);\nBP%d->Draw();\n",
//                     ii,x1,y1,x2,y2,ii,ii);
            fprintf(MACRO,"TMarker* BP%d = new TMarker(%f,%f,%d);\nBP%d->SetMarkerColor(1);\nBP%d->Draw();\n",
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
            fprintf(MACRO,"TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nE%d->SetFillStyle(0);\nE%d->Draw();\n",
                     i,info[i][0],info[i][1],info[i][3],info[i][3],i,i);
          } else {	// skew straws.
	    fprintf(MACRO,"TMarker* SS%d = new TMarker(%f,%f,%d);\nSS%d->SetMarkerColor(1);\nSS%d->Draw();\n",
		i,info[i][0],info[i][1],28,i,i,i);
	  }
       }

       for( ii=0; ii< nMvdStripHit; ii++) {
            x1= XMvdStrip[ii]-sigmaXMvdStrip[ii];
            x2= XMvdStrip[ii]+sigmaXMvdStrip[ii];
            y1= YMvdStrip[ii]-sigmaYMvdStrip[ii];
            y2= YMvdStrip[ii]+sigmaYMvdStrip[ii];
//            fprintf(MACRO,"TBox* BS%d = new TBox(%f,%f,%f,%f);\nBS%d->SetFillColor(2);\nBS%d->Draw();\n",
//                     ii,x1,y1,x2,y2,ii,ii);
            fprintf(MACRO,"TMarker* BS%d = new TMarker(%f,%f,%d);\nBS%d->SetMarkerColor(1);\nBS%d->Draw();\n",
                    ii,XMvdStrip[ii],YMvdStrip[ii],25,ii,ii,ii);


       }
       for( ii=0; ii< nMvdPixelHit; ii++) {
            x1= XMvdPixel[ii]-sigmaXMvdPixel[ii];
            x2= XMvdPixel[ii]+sigmaXMvdPixel[ii];
            y1= YMvdPixel[ii]-sigmaYMvdPixel[ii];
            y2= YMvdPixel[ii]+sigmaYMvdPixel[ii];
//            fprintf(MACRO,"TBox* BP%d = new TBox(%f,%f,%f,%f);\nBP%d->SetFillColor(4);\nBP%d->Draw();\n",
//                     ii,x1,y1,x2,y2,ii,ii);
            fprintf(MACRO,"TMarker* BP%d = new TMarker(%f,%f,%d);\nBP%d->SetMarkerColor(1);\nBP%d->Draw();\n",
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
		if( fabs(carica) > 0.1 ){
         		Oxx = pMC->GetStartVertex().X();    //   X of starting point track
         		Oyy = pMC->GetStartVertex().Y();    //   Y of starting point track
         		Px = pMC->GetMomentum().X();
         		Py = pMC->GetMomentum().Y();
         		aaa = sqrt( Px*Px + Py*Py);
         		Rr =   aaa*1000./(BFIELD*CVEL);    //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla
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
//"TEllipse* MC%d = new TEllipse(%f,%f,%f,%f,%f,%f);\nMC%d->SetFillStyle(0);\nMC%d->SetLineColor(3);\nMC%d->Draw(\"only\");\n",
//			i,Cx,Cy,Rr,Rr,primoangolo[i],ultimoangolo[i],i,i,i);
"TEllipse* MC%d = new TEllipse(%f,%f,%f,%f,%f,%f);\nMC%d->SetFillStyle(0);\nMC%d->SetLineColor(3);\nMC%d->Draw(\"only\");\n",
			i,Cx,Cy,Rr,Rr,primo*180./PI,ultimo*180./PI,i,i,i);
		}
	}



  }   //    end of for(i=0; i<nMCTracks;i++)
//----------- fine parte del MC

      fprintf(MACRO,"}\n");
      fclose(MACRO);
       
//------------------------------------------------------------------------------------------------------------




    return ;

}


//----------end of function PndSttMvdTracking::WriteMacroParallelHitsGeneral

















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
		   UShort_t nMvdPixelHitsAssociatedToSttTrack[MAXTRACKSPEREVENT], // output
		   UShort_t ListMvdPixelHitsAssociatedToSttTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHits], // output
		   UShort_t nMvdStripHitsAssociatedToSttTrack[MAXTRACKSPEREVENT], // output
		   UShort_t ListMvdStripHitsAssociatedToSttTrack[MAXTRACKSPEREVENT][nmaxMvdStripHits], // output



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
           Tiltdirection1[2],
           zl[200],zu[200],
           POINTS1[6];


//-------------------  skew straws hits Macro now

      char  nome2[300],nome[300];
      FILE *MACRO;
      sprintf(nome,  "MacroSkewTrack%dSkewHitsMvdwithMCEvent%dT%d",nMaxima, IVOLTE,iTrack);
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
        if( distance >= info[i][4] ) continue;




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



// checks that the projected ellipsis doesn't go out the boundaries of both the skew straw and the trajectory cylinder

/*
        if(
          fabs(POINTS1[j+2]-ZCENTER_STRAIGHT) > SEMILENGTH_STRAIGHT- Aellipsis1 ||
          distance + bbb > info[i][4]        //  the ellipsis goes out of the boundaries of the skew straw
          ) {
cout<<"the ellipsis goes out of the boundaries of the skew straw, hit n. "<<i<<endl
     <<"dis. from center "<<distance+bbb<<",  length of the straw "<<info[i][4]<<endl;
           continue;
          }
*/
//--------------------------


        fi1 = atan2(POINTS1[j+1]-Oy, POINTS1[j]-Ox) ;  // atan2 returns radians in (-pi and +pi]
        if( fi1 < 0.) fi1 += 2.*PI;

        if( zmin > POINTS1[j+2] - Aellipsis1 ) zmin = POINTS1[j+2] - Aellipsis1;
        if( zmax < POINTS1[j+2] + Aellipsis1 ) zmax = POINTS1[j+2] + Aellipsis1;

        if( Smin > fi1 - Bellipsis1 ) Smin = fi1 - Bellipsis1;
        if( Smax < fi1 + Bellipsis1 ) Smax = fi1 + Bellipsis1;


        Double_t rotation1 = 180.*atan2(Tiltdirection1[1],Tiltdirection1[0])/PI;
        fprintf(MACRO,"TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.,%f);\nE%d->SetFillStyle(0);\n",
                     index,POINTS1[j+2],fi1,Aellipsis1,Bellipsis1,rotation1,index);





// ------ se lo hit e' spurio marcalo in rosso
        for( i1=0; i1<nSkewCommon; i1++){
          if ( SkewCommonList[   iTrack   ][i1] == i ){

                goto fuori ;
          }

        }
        fprintf(MACRO,"E%d->SetLineColor(2);\n",index);
fuori: ;






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
        distance = sqrt(
                  (POINTS1[j]-C0x1)*(POINTS1[j]-C0x1) + 
                  (POINTS1[1+j]-C0y1)*(POINTS1[1+j]-C0y1) + 
                  (POINTS1[2+j]-C0z1)*(POINTS1[2+j]-C0z1) 
                            );
        if( distance >= info[i][4] ) continue;


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
        fprintf(MACRO,"TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.,%f);\nE%d->SetFillStyle(0);\n",
                     index,POINTS1[j+2],fi1,Aellipsis1,Bellipsis1,rotation1,index);

// ------  marca lo hit in blu
        fprintf(MACRO,"E%d->SetLineColor(4);\n",index);

        index++;

   }    //  end of    for( ii=0; ii<2; ii++)

  }   //   end of  for( iii=0; iii< nMCSkewAlone[imaxima]; iii++)

//-------------------------------
//------ fine aggiunta in blu eventuali punti della traccia MC che sono non mecciati









//   ora la parte delle Mvd

//   prima i pixel

  for(i=0; i<nMvdPixelHitsAssociatedToSttTrack[iTrack];i++){
        ii=ListMvdPixelHitsAssociatedToSttTrack[iTrack][i];

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
		"TMarker* BP%d = new TMarker(%f,%f,%d);\nBP%d->SetMarkerColor(1);\n",
				ii,ZMvdPixel[ii],esse,26,ii);
				goto punco ;
			}
		}
            fprintf(MACRO,"TMarker* BP%d = new TMarker(%f,%f,%d);\nBP%d->SetMarkerColor(2);\n",
                    ii,ZMvdPixel[ii],esse,26,ii);
punco: ;
  }
//   poi le strip

  for(i=0; i<nMvdStripHitsAssociatedToSttTrack[iTrack];i++){
        ii=ListMvdStripHitsAssociatedToSttTrack[iTrack][i];
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
            fprintf(MACRO,"TMarker* BS%d = new TMarker(%f,%f,%d);\nBS%d->SetMarkerColor(1);\n",
                    ii,ZMvdStrip[ii],esse,25,ii);
				goto ponco ;
			}
		}

            fprintf(MACRO,"TMarker* BS%d = new TMarker(%f,%f,%d);\nBS%d->SetMarkerColor(2);\n",
                    ii,ZMvdStrip[ii],esse,25,ii);


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
           fprintf(MACRO,"TMarker* BPA%d = new TMarker(%f,%f,%d);\nBPA%d->SetMarkerColor(4);\n",
                    ii,ZMvdPixel[ii],esse,26,ii);
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
 
            fprintf(MACRO,"TMarker* BSA%d = new TMarker(%f,%f,%d);\nBSA%d->SetMarkerColor(4);\n",
                    ii,ZMvdStrip[ii],esse,25,ii);

  }





  if( zmax < zmin ) goto nohits ;
  if( Smax < Smin ) goto nohits;
  aaa = Smax-Smin;
  Smin -= aaa*1.;
  Smax += aaa*1.;

  aaa = zmax-zmin;
  zmin -= aaa*0.05;
  zmax += aaa*0.05;

  if(Smax > 2.*PI) Smax = 2.*PI;
  if( Smin < 0.) Smin = 0.;

//  Smin=0.;
//  Smax=2.*PI;


  fprintf(MACRO,"TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n",zmin,Smin,zmax,Smax);
  for( ii=0; ii< index; ii++) {
       fprintf(MACRO,"E%d->Draw();\n",ii);
  }

   deltaz = zmax-zmin;
   deltaS = Smax-Smin;
   fprintf(MACRO,"TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n",
        zmin+0.05*deltaz,Smin+0.05*deltaS,zmax-0.05*deltaz,Smin+0.05*deltaS,zmin+0.05*deltaz,zmax-0.05*deltaz);
   fprintf(MACRO,"Assex->Draw();\n");
   fprintf(MACRO,"TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n",
        zmin+0.05*deltaz,Smin+0.05*deltaS,zmin+0.05*deltaz,Smax-0.05*deltaS,Smin+0.05*deltaS,Smax-0.05*deltaS);
   fprintf(MACRO,"Assey->Draw();\n");

// Mvd pixels
  for(i=0; i<nMvdPixelHitsAssociatedToSttTrack[iTrack];i++){
        ii=ListMvdPixelHitsAssociatedToSttTrack[iTrack][i];
        fprintf(MACRO,"BP%d->Draw();\n",ii);
  }
// Mvd strip

  for(i=0; i<nMvdStripHitsAssociatedToSttTrack[iTrack];i++){
        ii=ListMvdStripHitsAssociatedToSttTrack[iTrack][i];
        fprintf(MACRO,"BS%d->Draw();\n",ii);
  }
// Mvd pixels Alone
  for(i=0; i<nMCMvdPixelAlone;i++){
        ii=MCMvdPixelAloneList[i];
        fprintf(MACRO,"BPA%d->Draw();\n",ii);
  }
// Mvd Strips Alone
  for(i=0; i<nMCMvdStripAlone;i++){
        ii=MCMvdStripAloneList[i];
        fprintf(MACRO,"BSA%d->Draw();\n",ii);
  }


// --------------------------------

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
                 i-Nmin,z1,0.,z2, 2.*PI,i-Nmin,i-Nmin);

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
                 imc,i-Nmin,z1,0.,z2, 2.*PI,imc,i-Nmin,imc,i-Nmin);
  }   //  end of  for(i=Nmin; i<= Nmax;++)
	}  // end of if ( pMC )
       }  // end of if( imc>-1 )









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
		UShort_t nTrackCandHit[MAXTRACKSPEREVENT],
		UShort_t ListTrackCandHit[MAXTRACKSPEREVENT][nmaxSttHits+
	                           nmaxMvdPixelHitsInTrack+
				   nmaxMvdStripHitsInTrack],
		UShort_t ListTrackCandHitType[MAXTRACKSPEREVENT][nmaxSttHits+
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

		for(j=0;j<nTrackCandHit[i];j++){

		     switch (ListTrackCandHitType[i][j]){
			case 0:
			   exclusionPixel[ ListTrackCandHit[i][j] ] = true;
			   break;
			case 1:
			   exclusionStrip[ ListTrackCandHit[i][j] ] = true;
			   break;
			default:
			   exclusionStt[ ListTrackCandHit[i][j] ] = true;
			   break;
		     }
		}	// end of  for(j=0;j<nTrackCandHit[i];j++)
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
            fprintf(MACRO,"TMarker* BP%d = new TMarker(%f,%f,%d);\nBP%d->SetMarkerColor(1);\nBP%d->Draw();\n",
                    i,XMvdPixel[i],YMvdPixel[i],26,i,i);
          }
       }
       for( i=0; i< nMvdStripHit; i++) {
         if( !exclusionStrip[i]) {     // all Pixels
            fprintf(MACRO,"TMarker* BS%d = new TMarker(%f,%f,%d);\nBS%d->SetMarkerColor(1);\nBS%d->Draw();\n",
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




/*
	ntoMCtrack[jexp]=1;
	toMCtracklist[jexp][0]=(UShort_t)( info[ ListHitsinTrack[jexp][0]  ][6]+0.01);
	toMCtrackfrequency[jexp][0]=1;
	getMCInfo( toMCtracklist[jexp][0], &Cx, &Cy, &Rr);
	if( Rr<0.) {
		toMCtrackdistance[jexp][0]=-1.;
	} else {
		alfa = -2.*Cx;
		beta = -2.*Cy;
		gamma = Cx*Cx+Cy*Cy-Rr*Rr;
		toMCtrackdistance[jexp][0]= FindDistance(Ox[jexp],Oy[jexp],R[jexp],tanlow[jexp],
							tanmid[jexp],tanup[jexp],alfa,beta,gamma);
	}
*/



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
    		emme,
		enne[MAXTRACKSPEREVENT][nmaxSttHits];


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
		emme = (UShort_t) ( info[ i ][6] + 0.01);


		// escludo gli hits paralleli oppure che non appartengono alla giusta
		// traccia MC
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

			UShort_t nMvdPixelHitsAssociatedToSttTrack[MAXTRACKSPEREVENT], // output
			UShort_t ListMvdPixelHitsAssociatedToSttTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHits], // output
			UShort_t nMvdStripHitsAssociatedToSttTrack[MAXTRACKSPEREVENT], // output
			UShort_t ListMvdStripHitsAssociatedToSttTrack[MAXTRACKSPEREVENT][nmaxMvdStripHits] // output
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
	   nMvdPixelHitsAssociatedToSttTrack[i]=0;
	   nMvdStripHitsAssociatedToSttTrack[i]=0;
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
		ListMvdPixelHitsAssociatedToSttTrack[i][nMvdPixelHitsAssociatedToSttTrack[i]]=j;
		nMvdPixelHitsAssociatedToSttTrack[i]++;
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
		ListMvdStripHitsAssociatedToSttTrack[i][nMvdStripHitsAssociatedToSttTrack[i]]=j;
		nMvdStripHitsAssociatedToSttTrack[i]++;
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


//----------begin of function PndSttMvdTracking::










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
if(istampa>=1){
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
if(istampa>=1){
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
	//--- the following is for the  lam   structural variables
      for(i=0,ii=0; i< NpointsInFit ; i++) {
		if( mvdhit[i]){
			NRowsInWhichStructVarArePresent[4+ii]= 4;
			ii++;
		} else {
			NRowsInWhichStructVarArePresent[4+ii]= 5;
			NRowsInWhichStructVarArePresent[4+ii+1]= 5;
			ii+=2;
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
      sprintf(nome,"GeneralParalleEvent%d.mcs", IVOLTE);
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
      int status= glp_main(
            nRows,nameRows,typeRows, //  ROWS info
            NStructVar, NStructRowsMax, NRowsInWhichStructVarArePresent,  //  COLUMNS info
      StructVarName, NameRowsInWhichStructVarArePresent,  //  COLUMNS info
      Coefficients,  //  COLUMNS info
      ValueB,  // RHS  info
      nRanges, ValueRanges, NameRanges, //  RANGES  info
      nBounds, BoundValue, BoundStructVarName, TypeofBound //  BOUNDS info
      ,final_values
       );


//--------stampaggi
if(istampa>=1){
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

if(istampa>=2) cout<<"Risults : m1 = "<<m1_result<<", m2= "<<m2_result<<", q1 = "<<q1_result<<
	", q2 = "<<q2_result<<endl;

//---------  case in which the fit failed
	if( final_values[0]==0. && final_values[1]==0. && final_values[2]==0. &&final_values[3]==0. )
		return -10;

//------------------------  transformation of the result in terms of ALFA, BETA, GAMMA


     *qu = q1_result - q2_result;
     *emme = m1_result-m2_result ;
if(istampa>=3 && IVOLTE == 2) {
  cout<<"Stampa dal fitter, cose = "<<cose<<",  sine = "<<sine<<endl;

  cout<<"Stampa dal fitter, risultato :   qu, emme non ri-ruotati  "<<*qu<<",  "<<*emme<<endl<<
        "                                 qu, emme  ri-ruotati  "<<*qu/(cose-*emme*sine)<<
	",  "<<(*emme*cose+sine)/(cose-*emme*sine)<<endl;
}

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

if(istampa>=3 && IVOLTE == 2) {
  cout<<"Stampa dal fitter, prima di traslazione; Alfa = "<<*ALFA<<",  Beta = "<<*BETA<<
  	", Gamma = "<<*GAMMA<<"\n\t e conto finale;  residuo = "<<
	trajectory_vertex[0]+trajectory_vertex[0]+
	trajectory_vertex[1]+trajectory_vertex[1]+
	(*ALFA)*trajectory_vertex[0]+
	(*BETA)*trajectory_vertex[1]+
	*GAMMA
	<<endl;
}

// now take into account the displacement and correct
      *GAMMA += (trajectory_vertex[0]*trajectory_vertex[0]+ trajectory_vertex[1]*trajectory_vertex[1]
                                  -*ALFA*trajectory_vertex[0]-*BETA*trajectory_vertex[1]);
      *ALFA -=  2.*trajectory_vertex[0];
      *BETA -=  2.*trajectory_vertex[1];

if(istampa>=3 && IVOLTE == 2) {
  cout<<"Stampa dal fitter, dopo traslazione; Alfa = "<<*ALFA<<",  Beta = "<<*BETA<<
  	", Gamma = "<<*GAMMA<<"\n\t e conto finale;  residuo = "<<
	trajectory_vertex[0]*trajectory_vertex[0]+
	trajectory_vertex[1]*trajectory_vertex[1]+
	(*ALFA)*trajectory_vertex[0]+
	(*BETA)*trajectory_vertex[1]+
	*GAMMA
	<<endl;
}

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
		cout<<"from PndSttMvdTracking::FitSZspace  :  no points in fit, return!\n";
		return -10;
	}



	ave=0.;
	n=0;
	for(i=0;i<nSkewHitsinTrack;i++){
		if( fabs(Z[ i ]) > 1.e-10){
			n++;
			ave += (S[ i ] - FInot)/Z[ i ];
		}
	}

	if( n>0) {
		ave /=n;
		rotationangle = atan(ave);
	} else {
		rotationangle=PI/2.;
	}



//-------------- stampaggi
if(istampa>=2){
	cout<<"from FitSZspace, Evento "<<IVOLTE<<", NpointsInFit = "<<NpointsInFit<<endl;
	for(i=0 ; i< NpointsInFit ; i++) {
		cout<<"  Z["<<i<<"] = "<<Z[ i ]<<
		";   S["<<i<<"] = "<<S[ i ]<<"\tErrordiriftradius = "
		<<ErrorDriftRadius[i]<<endl;
	}
}
//------------ end stampaggi



//  use the trick of increasing the rotation angle by 10 degrees in order to obtain always a positive m
      rotationangle -= PI/18.;


      cose = cos(rotationangle);
      sine = sin(rotationangle);

      nSttHits = nMvdHits = 0;
      for(i=0;i<NpointsInFit; i++){
       Ox[i] =   Z[i]*cose +(S[i] - FInot)*sine;
       Oy[i] = -Z[i]*sine +(S[i] - FInot)*cose;
       Delta[i] = ErrorDriftRadius[i];
//          Delta[i] = 3.*ErrorDriftRadius[ i ];   //   3 times the Drift Radius

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
	//--- the following is for the  lam   structural variables
      for(i=0,ii=0; i< NpointsInFit ; i++) {
		if( mvdhit[i]){
			NRowsInWhichStructVarArePresent[4+ii]= 4;
			ii++;
		} else {
			NRowsInWhichStructVarArePresent[4+ii]= 5;
			NRowsInWhichStructVarArePresent[4+ii+1]= 5;
			ii+=2;
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
*/
//-------fine stampaggi

//-----------------------  funzioni chiamate direttamente
      int status= glp_main(
            nRows,nameRows,typeRows, //  ROWS info
            NStructVar, NStructRowsMax, NRowsInWhichStructVarArePresent,  //  COLUMNS info
      StructVarName, NameRowsInWhichStructVarArePresent,  //  COLUMNS info
      Coefficients,  //  COLUMNS info
      ValueB,  // RHS  info
      nRanges, ValueRanges, NameRanges, //  RANGES  info
      nBounds, BoundValue, BoundStructVarName, TypeofBound //  BOUNDS info
      ,final_values
       );


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

     if (status != 0) return -5;	// fit failed

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
			UShort_t *nTrackCandHit,
			UShort_t *ListTrackCandHit,
			UShort_t *ListTrackCandHitType,
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
		 Xconformal[*nTrackCandHit],
		 Yconformal[*nTrackCandHit],
		 DriftRadiusconformal[*nTrackCandHit],
		 ErrorDriftRadiusconformal[*nTrackCandHit];

	*status= false;
	factor=3.;
	maxdis2=0.5;
//	trajectory_vertex[0]=trajectory_vertex[1]=0.;

	for(i=0, iparallel=0; i<*nTrackCandHit && iparallel< MAXIMUMHITSINFIT; i++){
		if(i==iexcl) continue;
		if( ListTrackCandHitType[i] == 0 ){	// mvd pixels
			//----- translate the little circumference in XY representing
			//  approximately the sensitive area into the conformal space
			//  circumference

			dist2 = (XMvdPixel[ListTrackCandHit[i]]-tv[0])*
				(XMvdPixel[ListTrackCandHit[i]]-tv[0])+
				(YMvdPixel[ListTrackCandHit[i]]-tv[1])*
				(YMvdPixel[ListTrackCandHit[i]]-tv[1]);
			if(dist2<maxdis2) continue;	// this is to exclude Mvd hits too close to the traslated
						// center of reference frame (given by tv[0], tv[1]). This
						// would cause the subsequent fit to fail.

			gamma = dist2 - ErrorMvd*ErrorMvd; // for Pixels
				// I assume the 'drift radius' to be  the max dimension of
				//  the Pixel
			Xconformal[iparallel] = (XMvdPixel[ListTrackCandHit[i]]-tv[0])/gamma;
			Yconformal[iparallel] = (YMvdPixel[ListTrackCandHit[i]]-tv[1])/gamma;
			DriftRadiusconformal[iparallel]=-1.;// only to signal later this is a Mvd hit.
			ErrorDriftRadiusconformal[iparallel]=factor*ErrorMvd/gamma;
			iparallel++;
		} else if ( ListTrackCandHitType[i] == 1 ){	// mvd strips
			//----- translate the little circumference in XY representing
			//  approximately the sensitive area into the conformal space
			//  circumference
			dist2 = (XMvdStrip[ListTrackCandHit[i]]-tv[0])*
				(XMvdStrip[ListTrackCandHit[i]]-tv[0])+
				(YMvdStrip[ListTrackCandHit[i]]-tv[1])*
				(YMvdStrip[ListTrackCandHit[i]]-tv[1]);
			if(dist2<maxdis2) continue;	// this is to exclude Mvd hits too close to the traslated
						// center of reference frame (given by tv[0], tv[1]). This
						// would cause the subsequent fit to fail.

			gamma = dist2 - ErrorMvd*ErrorMvd;// for Strips also
				// I assume the 'drift radius' to be  the largest error of
				//  the Strips (which is in the X dimension)
			Xconformal[iparallel] = (XMvdStrip[ListTrackCandHit[i]]-tv[0])/gamma;
			Yconformal[iparallel] = (YMvdStrip[ListTrackCandHit[i]]-tv[1])/gamma;
			DriftRadiusconformal[iparallel]=-1.;// only to signal later this is a Mvd hit.
			ErrorDriftRadiusconformal[iparallel]=factor* ErrorMvd/gamma;
			iparallel++;
		} else if ( ListTrackCandHitType[i] == 2 ){	// Stt parallel hit.

			dist2 = (info[ListTrackCandHit[i]][0]-tv[0])*
				(info[ListTrackCandHit[i]][0]-tv[0])+
				(info[ListTrackCandHit[i]][1]-tv[1])*
				(info[ListTrackCandHit[i]][1]-tv[1]);
			if(dist2<maxdis2) continue;	// this is to exclude hits too close to the traslated
						// center of reference frame (given by tv[0], tv[1]). This
						// would cause the subsequent fit to fail.
			gamma = dist2 -
				info[ListTrackCandHit[i]][3]*
				info[ListTrackCandHit[i]][3];
			Xconformal[iparallel] = (info[ListTrackCandHit[i]][0]-tv[0])/gamma;
			Yconformal[iparallel] = (info[ListTrackCandHit[i]][1]-tv[1])/gamma;
			DriftRadiusconformal[iparallel]=info[ListTrackCandHit[i]][3]/gamma;
			ErrorDriftRadiusconformal[iparallel]=factor*ErrorStraw/gamma;
			iparallel++;
		}
	}	// end of for(i=0, iparallel=0;

	if( *nTrackCandHit < 2)  return;
if(istampa>=2){
	cout<<"cavolaccio, n. Hits = "<<iparallel<<endl;
	for(int ig=0;ig<iparallel;ig++){
		cout<<"\tcavolo, ErrorDriftRadiusconformal["<<ig<<"] = "<<
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
if(istampa>2){
	if( FromPixeltoMCTrack[i]<0 ){
cout<<"il pixel hit n. "<<i<<" non e' associato ad alcun Mvd Point (FromPixeltoMCTrack=-1).\n";
	} else {
		cout<<"associato pixel hit n. "<<i<<" a  MC Mvd Point n. "<<jmcpoint
		<<" e di conseguenza alla traccia MC n. "<<FromPixeltoMCTrack[i]<<endl;
	}
}

	}	// end of for(i=0; i<nMvdPixelHit;i++)






	for(i=0; i<nMvdStripHit;i++){
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
if(istampa>2){
	if( FromPixeltoMCTrack[i]<0 ){
cout<<"lo strip hit n. "<<i<<" non e' associato ad alcun Mvd Point (FromStriptoMCTrack=-1).\n";
	} else {
		cout<<"associato strip hit n. "<<i<<" a  MC Mvd Point n. "<<jmcpoint
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
			UShort_t *nMvdPixelHitsAssociatedToSttTrack,	// input
UShort_t ListMvdPixelHitsAssociatedToSttTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHits],// input
			UShort_t *nMvdStripHitsAssociatedToSttTrack,	// input
UShort_t ListMvdStripHitsAssociatedToSttTrack[MAXTRACKSPEREVENT][nmaxMvdStripHits],// input

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

		for(j=0;j<nMvdPixelHitsAssociatedToSttTrack[i];j++){
			includePixel[i][ListMvdPixelHitsAssociatedToSttTrack[i][j]]=false;
		  if( daTrackFoundaTrackMC[i]> -1){
			if( daTrackFoundaTrackMC[i] ==
				FromPixeltoMCTrack[
				     ListMvdPixelHitsAssociatedToSttTrack[i][j]
						]
			  ){
				index = i*nMvdPixelHit+nMvdPixelCommon[i];
				*(MvdPixelCommonList+index) =
				     ListMvdPixelHitsAssociatedToSttTrack[i][j];
				nMvdPixelCommon[i]++;
			} else {
				index = i*nMvdPixelHit+nMvdPixelSpuriinTrack[i];
				*(MvdPixelSpuriList+index) =
				     ListMvdPixelHitsAssociatedToSttTrack[i][j];
				nMvdPixelSpuriinTrack[i]++;
			}
		  }
		}

		for(j=0;j<nMvdStripHitsAssociatedToSttTrack[i];j++){
			includeStrip[i][ListMvdStripHitsAssociatedToSttTrack[i][j]]=false;
		  if( daTrackFoundaTrackMC[i]> -1){
			if( daTrackFoundaTrackMC[i] ==
				FromStriptoMCTrack[
				     ListMvdStripHitsAssociatedToSttTrack[i][j]
						]
			  ){
			  	index = i*nMvdStripHit+nMvdStripCommon[i];
				*(MvdStripCommonList+index) =
				     ListMvdStripHitsAssociatedToSttTrack[i][j];
				nMvdStripCommon[i]++;
			} else {
				index = i*nMvdStripHit+nMvdStripSpuriinTrack[i];
				*(MvdStripSpuriList+index) =
				     ListMvdStripHitsAssociatedToSttTrack[i][j];
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
				break;
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

			UShort_t nMvdPixelHitsAssociatedToSttTrack[MAXTRACKSPEREVENT], // output
			UShort_t ListMvdPixelHitsAssociatedToSttTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHits], // output
			UShort_t nMvdStripHitsAssociatedToSttTrack[MAXTRACKSPEREVENT], // output
			UShort_t ListMvdStripHitsAssociatedToSttTrack[MAXTRACKSPEREVENT][nmaxMvdStripHits] // output
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



int temporaneo=4;

	for(i=0; i<nSttTrackCand; i++){

//   handle those cases when FI0 and Fifirst are just around 0.
		if( fabs( FI0[i] - Fifirst[i]) > PI ) {     //  this is the special case
			specialcase = true;
			if( FI0[i] < Fifirst[i] ){
				FI0[i] += 2.*PI;
			} else {
				Fifirst[i] += 2.*PI;
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
		nMvdPixelHitsAssociatedToSttTrack[i]=0;
		nMvdStripHitsAssociatedToSttTrack[i]=0;
		ngoodmix=0;

		for( imvdcand=0; imvdcand<nMvdTrackCand; imvdcand++){
			Dist = 0.;
			ncont=0;
if(istampa>2 ){cout<<"da PndSttMvdTracking evento n. "<<IVOLTE<<" -------Stt Track Cand n. "<<i
	<<" (Ox="<<Ox[i]<<", Oy="<<Oy[i]<<", R="<<R[i]<<")"
	<<",  Mvd cand n. "<<imvdcand<<endl<<
	"\tn. Hits in questo MvdTrackCand = "<<nHitMvdTrackCand[imvdcand]<<endl;}
			nn[ngoodmix]=0;
			nHighQuality[ngoodmix]=0;
			for( jmvdhit=0; jmvdhit<nHitMvdTrackCand[imvdcand]; jmvdhit++){

				if(ListHitTypeMvdTrackCand[imvdcand][jmvdhit]==FairRootManager::Instance()->GetBranchId("MVDHitsPixel")){
					ncont++;
					angle = atan2(
					YMvdPixel[ListHitMvdTrackCand[imvdcand][jmvdhit]]-Oy[i],
					XMvdPixel[ListHitMvdTrackCand[imvdcand][jmvdhit]]-Ox[i]
							);
					if(angle<0.) angle += 2.*PI;
					if(specialcase){ if (angle<PI) angle += 2.*PI; }

					dist=fabs( sqrt(
	 (Ox[i]-XMvdPixel[ListHitMvdTrackCand[imvdcand][jmvdhit]])*
	 (Ox[i]-XMvdPixel[ListHitMvdTrackCand[imvdcand][jmvdhit]])
	+(Oy[i]-YMvdPixel[ListHitMvdTrackCand[imvdcand][jmvdhit]])*
	 (Oy[i]-YMvdPixel[ListHitMvdTrackCand[imvdcand][jmvdhit]])
						) -R[i]);


if(istampa>2 ){cout<<"da PndSttMvdTracking : pixel hit "<<ListHitMvdTrackCand[imvdcand][jmvdhit] <<", X = "<<
     XMvdPixel[ListHitMvdTrackCand[imvdcand][jmvdhit]]
     <<", Y = "<<
     YMvdPixel[ListHitMvdTrackCand[imvdcand][jmvdhit]]
     <<", dist "<<dist<<", angle = "<<angle<<" (min="<<anglemin<<", max="<<anglemax<<
     ", max dist "<<delta<<")"<<endl;
    }


					if(dist<delta &&angle > anglemin && angle < anglemax)
					{
						List[ngoodmix][nn[ngoodmix]]=
							ListHitMvdTrackCand[imvdcand][jmvdhit];
						ListType[ngoodmix][nn[ngoodmix]]= FairRootManager::Instance()->GetBranchId("MVDHitsPixel");
						Dist += dist;
						if( dist<highqualitycut) nHighQuality[ngoodmix]++;
						nn[ngoodmix]++;
					}

				} else if (ListHitTypeMvdTrackCand[imvdcand][jmvdhit]==FairRootManager::Instance()->GetBranchId("MVDHitsStrip")){

					ncont++;
					angle = atan2(
					YMvdStrip[ListHitMvdTrackCand[imvdcand][jmvdhit]]-Oy[i],
					XMvdStrip[ListHitMvdTrackCand[imvdcand][jmvdhit]]-Ox[i]
							);
					if(angle<0.) angle += 2.*PI;
					if(specialcase){ if (angle<PI) angle += 2.*PI; }

					dist=fabs( sqrt(
	 (Ox[i]-XMvdStrip[ListHitMvdTrackCand[imvdcand][jmvdhit]])*
	 (Ox[i]-XMvdStrip[ListHitMvdTrackCand[imvdcand][jmvdhit]])
	+(Oy[i]-YMvdStrip[ListHitMvdTrackCand[imvdcand][jmvdhit]])*
	 (Oy[i]-YMvdStrip[ListHitMvdTrackCand[imvdcand][jmvdhit]])
						) -R[i]);


if(istampa>2 ){cout<<"da PndSttMvdTracking : Strip hit "<<  ListHitMvdTrackCand[imvdcand][jmvdhit] <<
", X = "<<
     XMvdStrip[ListHitMvdTrackCand[imvdcand][jmvdhit]]
     <<", Y = "<<
     YMvdStrip[ListHitMvdTrackCand[imvdcand][jmvdhit]]
     <<", dist "<<dist<<", angle = "<<angle<<" (min="<<anglemin<<", max="<<anglemax<<
     ", max dist "<<delta<<")"<<endl;
    }
					if(dist<delta&& angle > anglemin && angle < anglemax)
					{
						List[ngoodmix][nn[ngoodmix]]=
							ListHitMvdTrackCand[imvdcand][jmvdhit];
						ListType[ngoodmix][nn[ngoodmix]]= FairRootManager::Instance()->GetBranchId("MVDHitsStrip");
						Dist += dist;
						if( dist<highqualitycut) nHighQuality[ngoodmix]++;

						nn[ngoodmix]++;
					}	// end of   if(ListHitTypeMvd......
				}

			}	// end of   for( jmvdhit=0; jmvdhit<nHitMvdTrackCand[imvdcand];


			if( nn[ngoodmix]>0) {
				DIST[ngoodmix]=Dist/nn[ngoodmix];
				ngoodmix++;


//--------- stampaggi
if(istampa>=2 && IVOLTE<20 ){cout<<"\tquesto Mvd candidato (n. ngoodmix = "<<ngoodmix-1<<
	") passa con i seguenti hits :"<<endl;

	for(int icc=0; icc<nn[ngoodmix-1]; icc++){
		if(ListType[ngoodmix-1][icc]==8) {
			cout<<"\tPixel hit n. "<<List[ngoodmix-1][icc]<<endl;
		} else if(ListType[ngoodmix-1][icc]==7){
			cout<<"\tStrip hit n. "<<List[ngoodmix-1][icc]<<endl;
		} else{
			cout<<"\tstranissimo , hit tipo "<<ListType[ngoodmix-1][icc]<<endl;
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
			if(specialcase){ if (angle<PI) angle += 2.*PI; }
			dist=fabs( sqrt(
			 (Ox[i]-XMvdPixel[ListMvdDSPixelHitNotTrackCand[jmvdhit]])*
			 (Ox[i]-XMvdPixel[ListMvdDSPixelHitNotTrackCand[jmvdhit]])
			+(Oy[i]-YMvdPixel[ListMvdDSPixelHitNotTrackCand[jmvdhit]])*
			 (Oy[i]-YMvdPixel[ListMvdDSPixelHitNotTrackCand[jmvdhit]])
						) -R[i]);
if(istampa>2 ){cout<<"da PndSttMvdTracking : alone\t DS Pixel hit n. "<< ListMvdDSPixelHitNotTrackCand[jmvdhit]<<", X = "<<
    XMvdPixel[ListMvdDSPixelHitNotTrackCand[jmvdhit]]
     <<", Y = "<<
     YMvdPixel[ListMvdDSPixelHitNotTrackCand[jmvdhit]]
     <<", e dist "<<dist<<endl;
    }
			if(dist<delta &&angle > anglemin && angle < anglemax)
			{
				List[ngoodmix][nn[ngoodmix]]=
					ListMvdDSPixelHitNotTrackCand[jmvdhit];
					ListType[ngoodmix][nn[ngoodmix]]= FairRootManager::Instance()->GetBranchId("MVDHitsPixel");
				DIST[ngoodmix] += dist;
				if( dist<highqualitycut) nHighQuality[ngoodmix]++;
						nn[ngoodmix]++;
					}

		}	//  end  of for( jmvdhit=0; jmvdhit<nMvdDSPixelHitNotTrackCand; jmvdhit++)

		for( jmvdhit=0; jmvdhit<nMvdDSStripHitNotTrackCand; jmvdhit++){

			angle = atan2(
				YMvdStrip[ListMvdDSStripHitNotTrackCand[jmvdhit]]-Oy[i],
				XMvdStrip[ListMvdDSStripHitNotTrackCand[jmvdhit]]-Ox[i]
				      );
			if(angle<0.) angle += 2.*PI;
			if(specialcase){ if (angle<PI) angle += 2.*PI; }

			dist=fabs( sqrt(
			 (Ox[i]-XMvdStrip[ListMvdDSStripHitNotTrackCand[jmvdhit]])*
			 (Ox[i]-XMvdStrip[ListMvdDSStripHitNotTrackCand[jmvdhit]])
			+(Oy[i]-YMvdStrip[ListMvdDSStripHitNotTrackCand[jmvdhit]])*
			 (Oy[i]-YMvdStrip[ListMvdDSStripHitNotTrackCand[jmvdhit]])
					) -R[i]);


if(istampa>2 ){cout<<"da PndSttMvdTracking : alone\t DS Strip hit n. "<< ListMvdDSStripHitNotTrackCand[jmvdhit]
 <<", X = "<<
      XMvdStrip[ListMvdDSStripHitNotTrackCand[jmvdhit]]
     <<", Y = "<<
     YMvdStrip[ListMvdDSStripHitNotTrackCand[jmvdhit]]
     <<", e dist "<<dist<<endl;
    }
			if(dist<delta &&angle > anglemin && angle < anglemax)
			{
				List[ngoodmix][nn[ngoodmix]]=
				ListMvdDSStripHitNotTrackCand[jmvdhit];
				ListType[ngoodmix][nn[ngoodmix]]= FairRootManager::Instance()->GetBranchId("MVDHitsStrip");
				DIST[ngoodmix] += dist;
				if( dist<highqualitycut) nHighQuality[ngoodmix]++;
				nn[ngoodmix]++;
			}

		}	//  end  of for( jmvdhit=0; jmvdhit<nMvdDSStripHitNotTrackCand; jmvdhit++)

			if( nn[ngoodmix]>0) {
				DIST[ngoodmix] /= nn[ngoodmix];
				ngoodmix++;
//--------- stampaggi
if(istampa>=2 && IVOLTE<20 ){cout<<"\tevento n. "<<IVOLTE<<" questi Mvd ALONE DS hits passano  :\n"<<endl;

	for(int icc=0; icc<nn[ngoodmix-1]; icc++){
		if(ListType[ngoodmix-1][icc]==FairRootManager::Instance()->GetBranchId("MVDHitsPixel")) {
			cout<<"\tDS Pixel hit n. "<<List[ngoodmix-1][icc]<<endl;
		} else if(ListType[ngoodmix-1][icc]==FairRootManager::Instance()->GetBranchId("MVDHitsStrip")){
			cout<<"\tDS Strip hit n. "<<List[ngoodmix-1][icc]<<endl;
		} else{
			cout<<"\tstranissimo , hit tipo "<<ListType[ngoodmix-1][icc]<<endl;
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
			if(specialcase){ if (angle<PI) angle += 2.*PI; }
			dist=fabs( sqrt(
			 (Ox[i]-XMvdPixel[ListMvdUSPixelHitNotTrackCand[jmvdhit]])*
			 (Ox[i]-XMvdPixel[ListMvdUSPixelHitNotTrackCand[jmvdhit]])
			+(Oy[i]-YMvdPixel[ListMvdUSPixelHitNotTrackCand[jmvdhit]])*
			 (Oy[i]-YMvdPixel[ListMvdUSPixelHitNotTrackCand[jmvdhit]])
						) -R[i]);
if(istampa>2 ){cout<<"da PndSttMvdTracking : alone\t US Pixel hit n. "<< ListMvdUSPixelHitNotTrackCand[jmvdhit]<<", X = "<<
    XMvdPixel[ListMvdUSPixelHitNotTrackCand[jmvdhit]]
     <<", Y = "<<
     YMvdPixel[ListMvdUSPixelHitNotTrackCand[jmvdhit]]
     <<", e dist "<<dist<<endl;
    }
			if(dist<delta &&angle > anglemin && angle < anglemax)
			{
				List[ngoodmix][nn[ngoodmix]]=
					ListMvdUSPixelHitNotTrackCand[jmvdhit];
					ListType[ngoodmix][nn[ngoodmix]]= FairRootManager::Instance()->GetBranchId("MVDHitsPixel");
				DIST[ngoodmix] += dist;
				if( dist<highqualitycut) nHighQuality[ngoodmix]++;
						nn[ngoodmix]++;
					}

		}	//  end  of for( jmvdhit=0; jmvdhit<nMvdUSPixelHitNotTrackCand; jmvdhit++)

		for( jmvdhit=0; jmvdhit<nMvdUSStripHitNotTrackCand; jmvdhit++){

			angle = atan2(
				YMvdStrip[ListMvdUSStripHitNotTrackCand[jmvdhit]]-Oy[i],
				XMvdStrip[ListMvdUSStripHitNotTrackCand[jmvdhit]]-Ox[i]
				      );
			if(angle<0.) angle += 2.*PI;
			if(specialcase){ if (angle<PI) angle += 2.*PI; }

			dist=fabs( sqrt(
			 (Ox[i]-XMvdStrip[ListMvdUSStripHitNotTrackCand[jmvdhit]])*
			 (Ox[i]-XMvdStrip[ListMvdUSStripHitNotTrackCand[jmvdhit]])
			+(Oy[i]-YMvdStrip[ListMvdUSStripHitNotTrackCand[jmvdhit]])*
			 (Oy[i]-YMvdStrip[ListMvdUSStripHitNotTrackCand[jmvdhit]])
					) -R[i]);


if(istampa>2 ){cout<<"da PndSttMvdTracking : alone\t US Strip hit n. "<< ListMvdUSStripHitNotTrackCand[jmvdhit]
 <<", X = "<<
      XMvdStrip[ListMvdUSStripHitNotTrackCand[jmvdhit]]
     <<", Y = "<<
     YMvdStrip[ListMvdUSStripHitNotTrackCand[jmvdhit]]
     <<", e dist "<<dist<<endl;
    }
			if(dist<delta &&angle > anglemin && angle < anglemax)
			{
				List[ngoodmix][nn[ngoodmix]]=
				ListMvdUSStripHitNotTrackCand[jmvdhit];
				ListType[ngoodmix][nn[ngoodmix]]= FairRootManager::Instance()->GetBranchId("MVDHitsStrip");
				DIST[ngoodmix] += dist;
				if( dist<highqualitycut) nHighQuality[ngoodmix]++;
				nn[ngoodmix]++;
			}

		}	//  end  of for( jmvdhit=0; jmvdhit<nMvdUSStripHitNotTrackCand; jmvdhit++)

			if( nn[ngoodmix]>0) {
				DIST[ngoodmix] /= nn[ngoodmix];
				ngoodmix++;
//--------- stampaggi
if(istampa>=2 && IVOLTE<20 ){cout<<"\tevento n. "<<IVOLTE<<" questi Mvd ALONE US hits passano  :\n"<<endl;

	for(int icc=0; icc<nn[ngoodmix-1]; icc++){
		if(ListType[ngoodmix-1][icc]==FairRootManager::Instance()->GetBranchId("MVDHitsPixel")) {
			cout<<"\tUS Pixel hit n. "<<List[ngoodmix-1][icc]<<endl;
		} else if(ListType[ngoodmix-1][icc]==FairRootManager::Instance()->GetBranchId("MVDHitsStrip")){
			cout<<"\tUS Strip hit n. "<<List[ngoodmix-1][icc]<<endl;
		} else{
			cout<<"\tstranissimo , hit tipo "<<ListType[ngoodmix-1][icc]<<endl;
		}
	}

}
//------------fine stampaggi
			}	// end of if( nn[ngoodmix]>0)




//-------  end of using the Mvd which are in no Mvd Track Candidate


if(istampa>2 ){cout<<"da PndSttMvdTracking : appena prima arbitration, ngoodmix = "<<ngoodmix<<endl;}

		if( ngoodmix==1){
			chosenmix=0;
			chosenmix2=0;
		} else if( ngoodmix>1) {
//--- here the arbitration if there are more than 1 Stt+Mvd hit combination for a given SttTrackCand
			oldtotal = DIST[0];
			oldtotal2 = DIST[0];
			oldN = nHighQuality[0];
//			oldtotal /= nTotali[0];
if(istampa>2 ){cout<<"da PndSttMvdTracking : goodmix n. 0, total distance (che e' = total distance2) = "<<oldtotal
				<<", e nHighQuality = "<<nHighQuality[0]<<endl;}
			chosenmix=0;
			chosenmix2=0;
			for(j1=1; j1<ngoodmix;j1++){
				total = DIST[j1];
if(istampa>2 ){cout<<"da PndSttMvdTracking :\t goodmix n. "<<j1<<", total distance "<<total
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





	nMvdPixelHitsAssociatedToSttTrack[i]=0;
	nMvdStripHitsAssociatedToSttTrack[i]=0;
	if( ngoodmix>0){
		chosenmix=chosenmix2;
		for(j=0;j<nn[chosenmix];j++){
			if( ListType[chosenmix][j]==FairRootManager::Instance()->GetBranchId("MVDHitsPixel") ){
				ListMvdPixelHitsAssociatedToSttTrack[i]
				   [nMvdPixelHitsAssociatedToSttTrack[i]]=List[chosenmix][j];
				nMvdPixelHitsAssociatedToSttTrack[i]++;
			} else{
				ListMvdStripHitsAssociatedToSttTrack[i]
				   [nMvdStripHitsAssociatedToSttTrack[i]]=List[chosenmix][j];
				nMvdStripHitsAssociatedToSttTrack[i]++;
			}
		}
	}	// end of if( ngoodmix>0)


	}	// end of for(i=0; i<nSttTrackCand; i++)


 return;
}

//------------------------- end of function  PndSttMvdTracking::MatchMvdHitsToSttTracks2

















//------------------------- begin of function  PndSttMvdTracking::MatchMvdHitsToSttTracksagain

	void	PndSttMvdTracking::MatchMvdHitsToSttTracksagain(
			Double_t delta,
			Double_t highqualitycut,
			UShort_t nSttTrackCand,
			Double_t Ox[MAXTRACKSPEREVENT],
			Double_t Oy[MAXTRACKSPEREVENT],
			Double_t R[MAXTRACKSPEREVENT],
			Double_t FI0[MAXTRACKSPEREVENT],
			Double_t Fifirst[MAXTRACKSPEREVENT],
			Short_t CHARGE[MAXTRACKSPEREVENT],

			UShort_t nMvdPixelHitsAssociatedToSttTrack[MAXTRACKSPEREVENT], // output
			UShort_t ListMvdPixelHitsAssociatedToSttTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHits], // output
			UShort_t nMvdStripHitsAssociatedToSttTrack[MAXTRACKSPEREVENT], // output
			UShort_t ListMvdStripHitsAssociatedToSttTrack[MAXTRACKSPEREVENT][nmaxMvdStripHits] // output
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
		ntot=nMvdPixelHitsAssociatedToSttTrack[itrack]+
			nMvdStripHitsAssociatedToSttTrack[itrack];
		if(ntot==0) continue;

//   handle those cases when FI0 and Fifirst are just around 0.

		if( fabs( FI0[itrack] - Fifirst[itrack]) > PI ) {     //  this is the special case
			specialcase = true;
			if( FI0[itrack] < Fifirst[itrack] ){
				FI0[itrack] += 2.*PI;
			} else {
				Fifirst[itrack] += 2.*PI;
			}
		} else {
			specialcase = false;
		}

		if( FI0[itrack] < Fifirst[itrack] ){
			anglemax = Fifirst[itrack];
			anglemin = FI0[itrack];
		} else {
			anglemax = FI0[itrack];
			anglemin = Fifirst[itrack];
		}

//--------------------
//		nMvdPixelHitsAssociatedToSttTrack[i]=0;
//		nMvdStripHitsAssociatedToSttTrack[i]=0;
//		ngoodmix=0;

//  find if this track goes downstream or upstream

		ndownstream=0;
		  for(j=0; j<nMvdPixelHitsAssociatedToSttTrack[itrack]; j++){
			if( ZMvdPixel[ListMvdPixelHitsAssociatedToSttTrack[itrack][j]]>0.){
				ndownstream++ ;
			}
		  }
		  for(j=0; j<nMvdStripHitsAssociatedToSttTrack[itrack]; j++){
			if( ZMvdPixel[ListMvdStripHitsAssociatedToSttTrack[itrack][j]]>0.){
				ndownstream++ ;
			}
		  }
		  if(ndownstream>ntot-ndownstream) downstream=true; else downstream=false;



//  loop over the Mvd Pixel and try to attach new Pixels to each candidate track



		naddpix=0;
		for(ipix=0; ipix<nMvdPixelHit; ipix++){
			for(j=0; j<nMvdPixelHitsAssociatedToSttTrack[itrack]; j++){
			     if( ListMvdPixelHitsAssociatedToSttTrack[itrack][j]==ipix) goto out ;
			}
				if (    (downstream && ZMvdPixel[ipix]>=0.) ||
					((!downstream) && ZMvdPixel[ipix]<0.)
				   ){
					angle = atan2(YMvdPixel[ipix]-Oy[itrack],
							XMvdPixel[ipix]-Ox[itrack]);
					if(angle<0.) angle += 2.*PI;
					if(specialcase){ if (angle<PI) angle += 2.*PI; }
					dist=fabs( sqrt(
					  (Ox[itrack]-XMvdPixel[ipix])*(Ox[itrack]-XMvdPixel[ipix])
					  +(Oy[itrack]-YMvdPixel[ipix])*(Oy[itrack]-YMvdPixel[ipix])
						) -R[itrack]);
					if(dist<highqualitycut
					    &&angle > anglemin && angle < anglemax)
					{
						List[naddpix]=ipix;
						naddpix++;
					}
				}
			out: ;
		}	//  end of   for(ipix=0; ipix<nMvdPixelHit; ipix++)

		if(naddpix>0){
			for(j=0;j<naddpix;j++){
				ListMvdPixelHitsAssociatedToSttTrack[itrack]
				[nMvdPixelHitsAssociatedToSttTrack[itrack]+j]=List[j];
			}
			nMvdPixelHitsAssociatedToSttTrack[itrack] += naddpix;
		}	//  end of  if(naddpix>0)




		naddstr=0;
		for(istr=0; istr<nMvdStripHit; istr++){
			for(j=0; j<nMvdStripHitsAssociatedToSttTrack[itrack]; j++){
			     if( ListMvdStripHitsAssociatedToSttTrack[itrack][j]==istr) goto out2 ;
			}
				if (    (downstream && ZMvdStrip[istr]>=0.) ||
					( (!downstream) && ZMvdStrip[istr]<0.)
				   ){
					angle = atan2(YMvdStrip[istr]-Oy[itrack],
							XMvdStrip[istr]-Ox[itrack]);
					if(angle<0.) angle += 2.*PI;
					if(specialcase){ if (angle<PI) angle += 2.*PI; }
					dist=fabs( sqrt(
					  (Ox[itrack]-XMvdStrip[istr])*(Ox[itrack]-XMvdStrip[istr])
					  +(Oy[itrack]-YMvdStrip[istr])*(Oy[itrack]-YMvdStrip[istr])
						) -R[itrack]);
					if(dist<highqualitycut
					   &&angle > anglemin&& angle < anglemax)
					{
						List[naddstr]=istr;
						naddstr++;
					}
				}



			out2: ;
		}	//  end of   for(istr=0; istr<nMvdStripHit; istr++)
		if(naddstr>0){
			for(j=0;j<naddstr;j++){
				ListMvdStripHitsAssociatedToSttTrack[itrack]
				[nMvdStripHitsAssociatedToSttTrack[itrack]+j]=List[j];
			}
			nMvdStripHitsAssociatedToSttTrack[itrack] += naddstr;
		}	//  end of  if(naddpix>0)





	}	// end of for(itrack=0; itrack<nSttTrackCand; itrack++)









 return;
}

//------------------------- end of function  PndSttMvdTracking::MatchMvdHitsToSttTracksagain
















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



//if(IVOLTE==1) { cout<<"controprova, alfa = "<<alfa<<", beta = "<<beta<<", gamma = "<<gamma<<endl;}


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
						Double_t Z[2],
						Double_t Zdrift[2],
						Double_t Zerror[2]
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
        if( distance >= info[i][4] ) continue;


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

//--------------------------
        S[ii] = atan2(POINTS1[j+1]-Oy, POINTS1[j]-Ox) ;  // atan2 returns radians in (-pi and +pi]
        if( S[ii] < 0.) S[ii] += 2.*PI;


	Z[ii] = POINTS1[j+2];
	Zdrift[ii] =  Aellipsis1;
	Zerror[ii] = StrawRadius*aaa/LL;



//	Double_t rotation1 = 180.*atan2(Tiltdirection1[1],Tiltdirection1[0])/PI;


   }    //  end of    for( ii=0; ii<2; ii++)

	return;
}

//-------------------------  end of function  PndSttMvdTracking::CalculateSandZ




//-------------------------  begin of function  PndSttMvdTracking::EliminateSpuriousSZ

  void PndSttMvdTracking::EliminateSpuriousSZ(
			UShort_t *nMvdPixelHitsAssociatedToSttTrack,
			UShort_t *ListMvdPixelHitsAssociatedToSttTrack,
			UShort_t *nMvdStripHitsAssociatedToSttTrack,
			UShort_t *ListMvdStripHitsAssociatedToSttTrack,
			UShort_t *nSttSkewHitsinTrack,
			UShort_t *ListSttSkewHitsinTrack,
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
			Double_t FI0
				    )
{




	UShort_t i,
		 j,
		 auxnMvdPixel,
		 auxListMvdPixel[(*nMvdPixelHitsAssociatedToSttTrack)],
		 auxnMvdStrip,
		 auxListMvdStrip[(*nMvdStripHitsAssociatedToSttTrack)],
		 auxnSttSkew,
		 auxListSttSkew[(*nSttSkewHitsinTrack)];

	const Double_t  MvdCut=0.5,
			minimumSttDriftError = 1.;

	Double_t dista,
		 dista1,
		 dista0,
		 error;


	auxnMvdPixel=0;
	auxnMvdStrip=0;
	auxnSttSkew=0;
	for(i=0;i<*nMvdPixelHitsAssociatedToSttTrack;i++){

		if( fabs(SignedDist_SZ(KAPPA,FI0,ZED[i][0],S[i][0])) < MvdCut ){
			auxListMvdPixel[auxnMvdPixel]=ListMvdPixelHitsAssociatedToSttTrack[i];
			SchosenPixel[ListMvdPixelHitsAssociatedToSttTrack[i]]=S[i][0];
			ZchosenPixel[ListMvdPixelHitsAssociatedToSttTrack[i]]=ZED[i][0];
			ErrorchosenPixel[ListMvdPixelHitsAssociatedToSttTrack[i]]=errorPixel;
			auxnMvdPixel++;
		}
	}	// end of  for(i=0;i<*nMvdPixelHitsAssociatedToSttTrack;i++)

	for(j=0;j<*nMvdStripHitsAssociatedToSttTrack;j++){
		i=j+(*nMvdPixelHitsAssociatedToSttTrack);

		if( fabs(SignedDist_SZ(KAPPA,FI0,ZED[i][0],S[i][0])) < MvdCut ){
			auxListMvdStrip[auxnMvdStrip]=ListMvdStripHitsAssociatedToSttTrack[j];
			SchosenStrip[ListMvdStripHitsAssociatedToSttTrack[j]]=S[i][0];
			ZchosenStrip[ListMvdStripHitsAssociatedToSttTrack[j]]=ZED[i][0];
			ErrorchosenStrip[ListMvdStripHitsAssociatedToSttTrack[j]]=errorStrip;
			auxnMvdStrip++;
		}
	}	// end of  for(j=0;j<*nMvdStripHitsAssociatedToSttTrack;j++)

	for(j=0;j<*nSttSkewHitsinTrack;j++){
		i=j+(*nMvdPixelHitsAssociatedToSttTrack)+
		    (*nMvdStripHitsAssociatedToSttTrack);

		if( ZED[i][0]<999990. && ZED[i][1]<999990.){
			dista0 = SignedDist_SZ(KAPPA,FI0,ZED[i][0],S[i][0]);
			dista1 = SignedDist_SZ(KAPPA,FI0,ZED[i][1],S[i][1]);
			if( fabs(dista1)<fabs(dista0)){
				dista = dista1;
				error = ErrorDriftRadius[i][1];
				SchosenSkew[ListSttSkewHitsinTrack[j]]=S[i][1];
				if( dista>= 0.){
				  ZchosenSkew[ListSttSkewHitsinTrack[j]]=ZED[i][1]+DriftRadius[i][1];
				} else {
				  ZchosenSkew[ListSttSkewHitsinTrack[j]]=ZED[i][1]-DriftRadius[i][1];
				}
			}else{
				dista = dista0;
				error = ErrorDriftRadius[i][0];
				SchosenSkew[ListSttSkewHitsinTrack[j]]=S[i][0];
				if( dista>= 0.){
				  ZchosenSkew[ListSttSkewHitsinTrack[j]]=ZED[i][0]+DriftRadius[i][0];
				} else {
				  ZchosenSkew[ListSttSkewHitsinTrack[j]]=ZED[i][0]-DriftRadius[i][0];
				}
			}
		} else if (ZED[i][0]<999990.){
			dista = SignedDist_SZ(KAPPA,FI0,ZED[i][0],S[i][0]);
			error = ErrorDriftRadius[i][0];
			SchosenSkew[ListSttSkewHitsinTrack[j]]=S[i][0];
			if( dista>= 0.){
			  ZchosenSkew[ListSttSkewHitsinTrack[j]]=ZED[i][0]+DriftRadius[i][0];
			} else {
			  ZchosenSkew[ListSttSkewHitsinTrack[j]]=ZED[i][0]-DriftRadius[i][0];
			}
		} else if (ZED[i][1]<999990.){
			dista = SignedDist_SZ(KAPPA,FI0,ZED[i][1],S[i][1]);
			error = ErrorDriftRadius[i][1];
			SchosenSkew[ListSttSkewHitsinTrack[j]]=S[i][1];
			if( dista>= 0.){
			  ZchosenSkew[ListSttSkewHitsinTrack[j]]=ZED[i][1]+DriftRadius[i][1];
			} else {
			  ZchosenSkew[ListSttSkewHitsinTrack[j]]=ZED[i][1]-DriftRadius[i][1];
			}
		} else {
			continue;
		}

		if(
			fabs(dista) < 2.*error
				||
			fabs(dista) < 2.*minimumSttDriftError
			){
			auxListSttSkew[auxnSttSkew]=ListSttSkewHitsinTrack[j];
			ErrorchosenSkew[ListSttSkewHitsinTrack[j]]=error;
			auxnSttSkew++;
		}
	}	// end of  for(j=0;j<*nSttSkewHitsinTrack;j++)




//	reload the list of good hits

	*nMvdPixelHitsAssociatedToSttTrack = auxnMvdPixel;
	*nMvdStripHitsAssociatedToSttTrack = auxnMvdStrip;
	*nSttSkewHitsinTrack = auxnSttSkew;
	for(j=0;j<*nMvdPixelHitsAssociatedToSttTrack;j++){
		ListMvdPixelHitsAssociatedToSttTrack[j]=auxListMvdPixel[j];
	}	// end of  for(j=0;j<*nMvdPixelHitsAssociatedToSttTrack;j++)
	for(j=0;j<*nMvdStripHitsAssociatedToSttTrack;j++){
		ListMvdStripHitsAssociatedToSttTrack[j]=auxListMvdStrip[j];
	}	// end of  for(j=0;j<*nMvdStripHitsAssociatedToSttTrack;j++)
	for(j=0;j<*nSttSkewHitsinTrack;j++){
		ListSttSkewHitsinTrack[j]=auxListSttSkew[j];
	}	// end of  for(j=0;j<*nSttSkewHitsinTrack;j++)


	return;

}
//-------------------------  end of function  PndSttMvdTracking::EliminateSpuriousSZ



//-------------------------  begin of function  PndSttMvdTracking::SignedDist_SZ

  Double_t PndSttMvdTracking::SignedDist_SZ(
  					Double_t KAPPA,
					Double_t FI0,
					Double_t ZED,
					Double_t S
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
		return -1.;
	} else if (fabs(KAPPA)>1.e10) {
		return fabs(ZED);
	}

	gap = fabs(2.*PI/KAPPA);
	n = (Short_t) ((KAPPA*ZED+FI0)/(2.*PI));
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

//-------------------------  end of function  PndSttMvdTracking::SignedDist_SZ






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













ClassImp(PndSttMvdTracking)
