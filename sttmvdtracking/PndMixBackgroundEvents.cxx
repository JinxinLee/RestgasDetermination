#include "glpk.h"

#include "PndMixBackgroundEvents.h"

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

#include "TFile.h"
#include "TGeoManager.h"
#include "TClonesArray.h"
#include "TGeoVolume.h"
#include "TVector3.h"
#include "TRandom3.h"
#include "TH1F.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TGeoTube.h"

#include <iostream>
#include <cmath>


using namespace std;


// -----   Default constructor   -------------------------------------------
PndMixBackgroundEvents::PndMixBackgroundEvents() : FairTask("Mixing bkgrnd hits to Stt-Mvd") { 
  fPersistence = kTRUE;
  fVerbose = 0;
  istampa = 0;
}
// -------------------------------------------------------------------------

PndMixBackgroundEvents::PndMixBackgroundEvents(Int_t verbose) : FairTask("STT Stt-Mvd Tracking") { 
  fPersistence = kTRUE;
  fVerbose = verbose;
  istampa = verbose;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMixBackgroundEvents::~PndMixBackgroundEvents() { 
	delete filerun;
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndMixBackgroundEvents::Init() {


  IVOLTE=-1;
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndMixBackgroundEvents::Init: "
	 << "RootManager not instantiated, return!" << endl;
    return kFATAL;
  }
//  -----   maps of STT tubes
  // CHECK added 
  PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  fSttTubeArray = mapper->FillTubeArray();
 //----------------------------------------------------  end map



//-----------------  input TClones Arrays -----------------------------------------


//    get   the MCTrack   input Array
  fMCTrackArray = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCTrackArray) 
    {
      cout << "-E- PndMixBackgroundEvents::Init: No MCTrack array, return!"
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
  fSttHitArray = (TClonesArray*) ioman->GetObject("STTHit");
  if ( ! fSttHitArray ) {
    cout << "-W- PndMixBackgroundEvents::Init: "
	 << "No STTHit array, return!" << endl;
    return kERROR;
  }

//  -------------------------  get the Mvd hit input Array

  fMvdPixelHitArray = (TClonesArray*) ioman->GetObject("MVDHitsPixel");
  if ( !fMvdPixelHitArray){
    std::cout << "-W- PndMixBackgroundEvents::Init: " << "No MVD Pixel hitArray, return!" << std::endl;
    return kERROR;
  }

  fMvdStripHitArray = (TClonesArray*) ioman->GetObject("MVDHitsStrip");

  if ( !fMvdStripHitArray){
    std::cout << "-W- PndMixBackgroundEvents::Init: " << "No MVD Strip hitArray, return!" << std::endl;
    return kERROR;
  }

//  -------------------------   get the Mvd MC point input Array

  fMvdMCPointArray = (TClonesArray*) ioman->GetObject("MVDPoint");
  if ( !fMvdMCPointArray){
    std::cout << "-W- PndMixBackgroundEvents::Init: " << "No MVD MC Point Array, return!" << std::endl;
    return kERROR;
  }


  //  Background input Arrays

  //   opend background file
  filerun = new TFile(
 "~/panda/installazione_31jan11/pandaroot/macro/sttmvdtracking/dpm/Mix_Generation_digi.root");
  treebkg = (TTree*) filerun->Get("cbmsim");
  nTotalBkgEvents = (Int_t) treebkg->GetEntriesFast();

  // Background STT hits   -----
  treebkg->SetBranchAddress("STTHit",&fSttHitBkgArray);
  if ( ! fSttHitBkgArray ) {
    cout << "-W- PndMixBackgroundEvents::Init: "
	 << "No STT Background Hit array, return!" << endl;
    return kERROR;
  }

  treebkg->SetBranchAddress("MVDPixelDigis",&fMvdPixelHitBkgArray);
  if ( !fMvdPixelHitBkgArray){
    std::cout << "-W- PndMixBackgroundEvents::Init: " << "No MVD Pixel Background hitArray, return!"
    	 << std::endl;
    return kERROR;
  }

  treebkg->SetBranchAddress("MVDStripDigis",&fMvdStripHitBkgArray);
  if ( !fMvdStripHitBkgArray){
    std::cout << "-W- PndMixBackgroundEvents::Init: " << "No MVD Strip Background hitArray, return!"
    	 << std::endl;
    return kERROR;
  }



//-----------------  Output TClone Arrays -----------------------------------------


//  ---------------------- new output array of Stt hits + mixed background

  fSttHitandBckgrndArray = new TClonesArray("PndSttHit"); // PndSttHit is the class tipe.
  ioman->Register("STTMixHit","SttHitandBckgrnd",fSttHitandBckgrndArray, kTRUE);
//  ---------------------- new output array of Mvd hits + mixed background
  //   Pixels
  fMvdPixelHitandBckgrndArray = new TClonesArray("PndSdsHit"); // PndSdsHit is the class tipe.
  ioman->Register("MVDHitsPixelMix","MvdPixelHitandBckgrnd",fMvdPixelHitandBckgrndArray, kTRUE);
  //   Strips
  fMvdStripHitandBckgrndArray = new TClonesArray("PndSdsHit"); // PndSdsHit is the class tipe.
  ioman->Register("MVDHitsStripMix","MvdStripHitandBckgrnd",fMvdStripHitandBckgrndArray, kTRUE);


  return kSUCCESS;

}

// ---------------  end of      InitStatus PndMixBackgroundEvents::Init  --------------------

// CHECK added 
void PndMixBackgroundEvents::SetParContainers() {
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
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
void PndMixBackgroundEvents::Exec(Option_t* opt) {


 bool status[MAXTRACKSPEREVENT],
 	SttSZfit[MAXTRACKSPEREVENT],
	Mvdhits[MAXTRACKSPEREVENT];


 Short_t Candidato,
         Charge,
         iflag,
         iHit,
	 k,
	 resultFitSZagain[MAXTRACKSPEREVENT],
	 tubeID
         ;

  Short_t  daTrackFoundaTrackMC[MAXTRACKSPEREVENT],
	   CHARGE[MAXTRACKSPEREVENT];

  UShort_t l,
	   kall;


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


  Int_t	nrounds0,
	nrounds1;

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
	   ultimoangolo[MAXTRACKSPEREVENT];

  Double_t ALFA[MAXTRACKSPEREVENT],
	   BETA[MAXTRACKSPEREVENT],
	   GAMMA[MAXTRACKSPEREVENT],
	   KAPPA[MAXTRACKSPEREVENT],
	   WDX[nmaxSttHits],
	   WDY[nmaxSttHits],
	   WDZ[nmaxSttHits],
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

//  PndTrackCand * pMvdTrackCand,
//               * pSttTrackCand;

  PndSttTrack * pSttHelixTrack;

  PndSttTube * pSttTube;

  PndSttHit * pSttHit;

  PndSdsHit * pMvdPixelHit,
            * pMvdStripHit;
  PndSdsMCPoint * pMvdMCPoint;

  PndMCTrack* pMCtr;

//  PndTrackCandHit pndtrackcandhit; 

//  fSttMvdPndTrackCandArray->Delete();

//  fSttMvdPndTrackArray->Delete();


//---------------- declaration of variables

	UShort_t	nBkgEventsToAdd;

	Int_t		i,
			ichosen,
			iPix,
			iStrip,
			iStt,
			j;

	Double_t	times[NMAXBCKGRND];


//	TRandom3 rannn;

//-----------------------------------------


  IVOLTE++;


//---------- fetching background info -----------------


	BackgroundNandT(&nBkgEventsToAdd,times);

	if(istampa>=3)
	cout<<"da PndMixBackgroundEvents, IVOLTE = "<<IVOLTE<<endl
	<<"\tBKG : n. di eventi  = "<<nTotalBkgEvents<<endl;


	for(Int_t ic=0;ic<nTotalBkgEvents;ic++){
		treebkg->GetEntry(ic);
//--------------- inizio stampaggi,  stampe di controllo
  if (istampa >= 3) {
	cout<<"da PndMixBackgroundEvents, IVOLTE = "<<IVOLTE<<endl
	<<"\tBKG : n. Stt hit = "<<fSttHitBkgArray->GetEntriesFast()<<endl
	<<"\tBKG : n. Pixel hit = "<<fMvdPixelHitBkgArray->GetEntriesFast()<<endl
	<<"\tBKG : n. Strip hit = "<<fMvdStripHitBkgArray->GetEntriesFast()<<endl;

  }  //  end of   if(istampa >= 

//--------  fine stampaggi
	}


//----------- end background info -----------------



//----------   fetching the MC truth tracks
	nMCTracks = fMCTrackArray->GetEntriesFast(); // num. tracce/evento
	if (istampa >= 3  && IVOLTE<20) {
	     cout<<"da PndMixBackgroundEvents  : evento (partendo da 0)  N. "<<
	       IVOLTE<< "\n       N. di MC truth tracks  : "<<nMCTracks<<endl;
	}
	if (nMCTracks ==0){
		cout<<"da PndMixBackgroundEvents  :  N. di MC truth tracks = 0, return!\n"<<endl;
		return;
	}
//------------------------------

//----------   fetching the STT  MC points
 nSttMCPoint = fSttPointArray->GetEntriesFast();
 if (nSttMCPoint ==0){
	cout<<"da PndMixBackgroundEvents  :  N. di Stt MC points = 0\n"<<endl;
//	return;
 }

 if(istampa>=3&& IVOLTE<20) cout<<"da PndMixBackgroundEvents  : n. punti MC = "<<nSttMCPoint<<endl;
 	for( i= 0; i< nSttMCPoint; i++){
	pSttMCPoint[i] = (FairMCPoint*) fSttPointArray->At(i);
 }

//------------------------------




//----------   fetching the STT  hits

 nSttHit = fSttHitArray->GetEntriesFast();
 if (nSttHit ==0){
	cout<<"da PndMixBackgroundEvents  :  N. di Stt Hits = 0, return!\n"<<endl;
	return;
 } else if (nSttHit>= nmaxSttHits) {
	cout<<"da PndMixBackgroundEvents  :  N. di Stt Hits = "<<nSttHit
	     <<" and it is >= nmaxSttHits (="<<nmaxSttHits<<"), return!\n"<<endl;
	return;
 }

 if (istampa >= 3  && IVOLTE<20) {
     cout<<"da PndMixBackgroundEvents  : evento (partendo da 0)  N. "<<
       IVOLTE<< "\n       N. totale Hits in STT  : "<<nSttHit<<endl;
 }
// ---------------------------------------------  estraggo le info da MVD

 nMvdPixelHit = fMvdPixelHitArray->GetEntriesFast();
 nMvdStripHit = fMvdStripHitArray->GetEntriesFast();
 if (nMvdPixelHit+nMvdStripHit ==0){
	cout<<"da PndMixBackgroundEvents  :  N. of MvdPixelHit=nMvdStripHit=0, return!\n"<<endl;
	return;
 } else if (nMvdPixelHit>= nmaxMvdPixelHits) {
	cout<<"da PndMixBackgroundEvents  :  N. of MvdPixelHit = "<<nMvdPixelHit
	<<" and it is >= nmaxMvdPixelHits ("<<nmaxMvdPixelHits<<"),  return!\n";
	return;
 } else if (nMvdStripHit>= nmaxMvdStripHits) {
	cout<<"da PndMixBackgroundEvents  :  N. of MvdStripHit = "<<nMvdStripHit
	<<" and it is >= nmaxMvdStripHits ("<<nmaxMvdStripHits<<"),  return!\n";
	return;
 }

// ---------------------------------------------  get MC Points of  MVD

   nMvdMCPoint = fMvdMCPointArray->GetEntriesFast();
 if (nMvdMCPoint ==0){
	cout<<"da PndMixBackgroundEvents  :  N. di MvdMCPoint =0, return!\n"<<endl;
	return;
 }
   if(istampa>2&& IVOLTE<20) cout<<"N. MC Points delle Mvd = "<<nMvdMCPoint<<endl;




//------------------------------------------------------------------


 nSttParHit=0; 
 nSttSkewHit=0;
 for( i= 0; i< nSttHit; i++){
	  ExclusionListStt[i]= true ;


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
	  ListAllParHits[nSttParHit]=i;
	  nSttParHit++;
          ZCENTER_STRAIGHT = info[i][2];      //    this works because just few lines below there is the
          SEMILENGTH_STRAIGHT = info[i][4];   //    requirement that Minclinations[0] > 2 (= at least 3 parallel straws)
       } else {
          info[i][5]= 99.;
	  ListAllSkewHits[nSttSkewHit]=i;
	  nSttSkewHit++;
       }



//--------------- inizio stampaggi,  stampe di controllo
  if (istampa >= 3) {
      cout <<"da PndMixBackgroundEvents, Stt iHit "<< i << endl;
      cout <<"             MC point X, Y, Z space position "   << pSttMCPoint[i]->GetX() << " " <<
                       pSttMCPoint[i]->GetY() << " " << pSttMCPoint[i]->GetZ()<<endl; 
      cout <<"             hit wire pos. in middle "   << pSttTube->GetPosition().X() << " " <<
             pSttTube->GetPosition().Y() << " " << pSttTube->GetPosition().Z() 
           << "; R = "<<sqrt(pSttTube->GetPosition().X()*pSttTube->GetPosition().X()+pSttTube->GetPosition().Y()*pSttTube->GetPosition().Y())<< endl;
      cout <<"             wire direction, X, Y, Z (Z direction set always positive)"<< WDX[i]<<"  "<<WDY[i]<<"  "<<WDZ[i] <<endl
           <<"             this hit belongs to MC track n. "<<pSttMCPoint[i]->GetTrackID()<<endl;

  }  //  end of   if(istampa >= 

//--------  fine stampaggi



  }  //   end of for( i= 0; i< nSttHit; i++)


//-------------------------------------------------------------  fine hits STT


// ---------------------------------------------  estraggo gli HITS Pixel MVD
 for( i= 0; i< nMvdPixelHit; i++){
    pMvdPixelHit = (PndSdsHit *) fMvdPixelHitArray->At(i);

	PndSdsHit pinco = *pMvdPixelHit;
	PndSdsHit punco;
	punco = pinco;
    TVector3 temp = pMvdPixelHit->GetPosition();
    XMvdPixel[i] = temp.X();
    YMvdPixel[i] = temp.Y();
    ZMvdPixel[i] = temp.Z();
    sigmaXMvdPixel[i] = punco.GetDx();
    sigmaYMvdPixel[i] = punco.GetDy();
    sigmaZMvdPixel[i] = punco.GetDz();

/*
    TVector3 temp = pMvdPixelHit->GetPosition();
    XMvdPixel[i] = temp.X();
    YMvdPixel[i] = temp.Y();
    ZMvdPixel[i] = temp.Z();
    sigmaXMvdPixel[i] = pMvdPixelHit->GetDx();
    sigmaYMvdPixel[i] = pMvdPixelHit->GetDy();
    sigmaZMvdPixel[i] = pMvdPixelHit->GetDz();
*/

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
if(istampa>=3  && IVOLTE<20){
  cout<<"da PndMixBackgroundEvents  :  n. Mvd Pixel Hits = "<<nMvdPixelHit<<endl;
  cout<<"da PndMixBackgroundEvents  :  n. Mvd Strip Hits = "<<nMvdStripHit<<endl;

  cout<<"da PndMixBackgroundEvents  :  info hits di Mvd pixels ------------------\n";
  for(i= 0; i< nMvdPixelHit; i++){
      cout<<"      Pixel hit n. "<<i<<" Info : X  = "<<XMvdPixel[i]<<";  Y  = "<<YMvdPixel[i]<<
            ";  Z  = "<<ZMvdPixel[i]<<endl;
      cout<<"\t\tPixel Info : sigmaX  = "<<sigmaXMvdPixel[i]<<";  sigmaY  = "<<sigmaYMvdPixel[i]<<
            ";  sigmaZ  = "<<sigmaZMvdPixel[i]<<endl;
  }
  cout<<"        ------------------\n";

  cout<<"da PndMixBackgroundEvents  :  info hits di Mvd strips ------------------\n";
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

   }
// ----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------  Now load the output TClones arrays -----------------------


   fSttHitandBckgrndArray->Delete();

   //	physical event Stt hits --

   for( iStt= 0; iStt< fSttHitArray->GetEntriesFast(); iStt++){
	pSttHit = (PndSttHit *) fSttHitArray->At(iStt);
	PndSttHit * temp = new ((*fSttHitandBckgrndArray)[iStt]) PndSttHit;
	*temp = *pSttHit;
/*
	TVector3 pos;
	pSttHit->Position(pos);
	TVector3  epos;
	pSttHit->PositionError(epos);
	new ((*fSttHitandBckgrndArray)[i])
		PndSttHit(
			FairRootManager::Instance()->GetBranchId("STTHit"),
			pSttHit->GetTubeID(),
			pSttHit->GetRefIndex(),
			pos,
			epos,
			pSttHit->GetPulse(),
			pSttHit->GetIsochrone(),
			pSttHit->GetIsochroneError(),
			pSttHit->GetDepCharge()
		);
*/


   }	// end of for(  iStt= 0; iStt< fSttHitArray


   //	physical event Mvd Pixel hits --

   fMvdPixelHitandBckgrndArray->Delete();

   for( iPix= 0; iPix< fMvdPixelHitArray->GetEntriesFast(); iPix++){
	pMvdPixelHit = (PndSdsHit *) fMvdPixelHitArray->At(iPix);
	PndSdsHit * temp = new ((*fMvdPixelHitandBckgrndArray)[iPix]) PndSdsHit;
	*temp = *pMvdPixelHit;

/*
	new ((*fMvdPixelHitandBckgrndArray)[i])
		PndSdsHit(
			FairRootManager::Instance()->GetBranchId("MVDHitsPixel"),
			pMvdPixelHit->GetSensorID(),
			pMvdPixelHit->GetPosition(),
			pMvdPixelHit->GetPosition(),// sbagliato qui ci vanno errori


TVector3& dpos, Int_t clindex,
	     Double_t charge, Int_t NDigiHits, Int_t mcindex);

//			Int_t detID,
//			 Int_t sensorID,
//  	    TVector3& pos, TVector3& dpos, Int_t clindex,
//	     Double_t charge, Int_t NDigiHits, Int_t mcindex);


*/

   }	// end of for( iPix= 0; iPix< fMvdPixelHitArray


   for( iStrip= 0; iStrip< fMvdStripHitArray->GetEntriesFast(); iStrip++){
	pMvdStripHit = (PndSdsHit *) fMvdStripHitArray->At(iStrip);
	PndSdsHit * temp = new ((*fMvdStripHitandBckgrndArray)[iStrip]) PndSdsHit;
	*temp = *pMvdStripHit;

   }	// end of for( i= 0; i< fSttHitArray->Ge


   //	background hits --


   for(j=0;j<nBkgEventsToAdd;j++){

	ichosen = (Int_t) (nTotalBkgEvents * rannn.Rndm());
	if(ichosen==nTotalBkgEvents) ichosen --;
	ichosen=0;
	treebkg->GetEntry(ichosen);

	//	background Stt hits --
	j = fSttHitArray->GetEntriesFast();
	for( i= 0; i<fSttHitBkgArray->GetEntriesFast() ; i++){
		pSttHit = (PndSttHit *) fSttHitBkgArray->At(i);
		PndSttHit * temp = new ((*fSttHitandBckgrndArray)[j+i]) PndSttHit;
		*temp = *pSttHit;
	}	// end of  for( i= 0;


	//	background Pixel hits --
	j = fMvdPixelHitArray->GetEntriesFast();
	for( i= 0; i<fMvdPixelHitBkgArray->GetEntriesFast() ; i++){
		pMvdPixelHit = (PndSdsHit *) fMvdPixelHitBkgArray->At(i);
		PndSdsHit * temp = new ((*fMvdPixelHitandBckgrndArray)[j+i]) PndSdsHit;
		*temp = *pMvdPixelHit;
	}	// end of  for( i= 0;


	//	background Strip hits --
	j = fMvdStripHitArray->GetEntriesFast();
	for( i= 0; i<fMvdStripHitBkgArray->GetEntriesFast() ; i++){
		pMvdStripHit = (PndSdsHit *) fMvdStripHitBkgArray->At(i);
		PndSdsHit * temp = new ((*fMvdStripHitandBckgrndArray)[j+i]) PndSdsHit;
		*temp = *pMvdStripHit;
	}	// end of  for( i= 0;

   }	// end of  for(j=0;j<nBkgEventsToAdd;j++)
// ----------------------------------------------------------------------




return;

}


//---------------------- end of   PndMixBackgroundEvents::Exec



//---------------------- begin of   PndMixBackgroundEvents::BackgroundNandT


  void PndMixBackgroundEvents::BackgroundNandT(
			UShort_t *nBkgEventsToAdd,
			Double_t *times
			)
  {
	*nBkgEventsToAdd=1;
	if( *nBkgEventsToAdd > NMAXBCKGRND ) *nBkgEventsToAdd=NMAXBCKGRND;
	times[0]=0.;
	return;
  }

//---------------------- end of   PndMixBackgroundEvents::BackgroundNandT








ClassImp(PndMixBackgroundEvents)
