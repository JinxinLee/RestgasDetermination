#ifndef PNDMIXBACKGROUNDEVENTS_H
#define PNDMIXBACKGROUNDEVENTS_H 1


#include "PndMCTrack.h"
#include "PndSttTrack.h"
#include "FairTask.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairTrackParP.h"
#include "PndGeoSttPar.h"

#include "TFile.h"
#include "TVector3.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TRandom3.h"

class TClonesArray;
class TObjectArray;

class PndMixBackgroundEvents : public FairTask
{

 public:


  /** Default constructor **/
  PndMixBackgroundEvents();

  PndMixBackgroundEvents(Int_t verbose);


  /** Destructor **/
  ~PndMixBackgroundEvents();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  void WriteHistograms();

  /** set persistence flag **/
  void SetPersistence(Bool_t persistence) { fPersistence = persistence; }
 

  void SetParContainers();

 private:

  static const UShort_t   NMAXBCKGRND = 1000;

  Int_t	nTotalBkgEvents;

  TFile *filerun;

  TTree *treebkg ;	// pointer to root Tree of background file.

  TRandom3 rannn;


  /** Input array of PndSttTube (map of STT tubes) **/
  TClonesArray* fMCTrackArray;
  TClonesArray* fSttTubeArray;

   /** Input array of PndSttPoints **/
  TClonesArray* fSttPointArray;

  /** Input array of PndSttHit **/
  TClonesArray* fSttHitArray;

  /** Input array of PndSttTracks **/
  TClonesArray* fSttTrackArray;

 /** Input array of PndTracksCand of Stt **/
  TClonesArray* fSttTrackCandArray;

 /** Input array of MvdPixelHitArray **/
  TClonesArray* fMvdPixelHitArray;

 /** Input array of MvdStripHitArray **/
  TClonesArray* fMvdStripHitArray;

 /** Input array of MC points  of Mvd**/
  TClonesArray* fMvdMCPointArray;


//---------------------  input Arrays for the Background hits  -------------

  /** Input array of PndSttHit **/
  TClonesArray* fSttHitBkgArray;

 /** Input array of MvdPixelHitArray **/
  TClonesArray* fMvdPixelHitBkgArray;

 /** Input array of MvdStripHitArray **/
  TClonesArray* fMvdStripHitBkgArray;

//-----------------



  /** Output array of Stt Hits + Background  **/
  TClonesArray* fSttHitandBckgrndArray;

 /** Output array of Mvd Pixel Hit + Background **/
  TClonesArray* fMvdPixelHitandBckgrndArray;

 /** Output array of Mvd Strip Hit + Background **/
  TClonesArray* fMvdStripHitandBckgrndArray;


//---------------------------------------------------------



  UShort_t nMCTracks;
#define maxTracks 20
#define dXPixel 0.03
#define dYPixel 0.03
#define dZPixel 0.03
#define dXStrip 0.03
#define dYStrip 0.03
#define dZStrip 0.03
#define errorsqPixel 1.01*(dXPixel*dXPixel/4.+dYPixel*dYPixel/4.+dZPixel*dZPixel/4.)
#define errorsqStrip 1.01*(dXStrip*dXStrip/4.+dYStrip*dYStrip/4.+dZStrip*dZStrip/4.)
#define errorPixel sqrt(errorsqPixel)
#define errorStrip sqrt(errorsqStrip)

  int istampa ;
  int IVOLTE ;
  static const bool  iplotta = true , ianalizza = true ;
  static const UShort_t   nmaxSttHits = maxTracks*26,
			  nmaxMvdPixelHits=500,
			  nmaxMvdStripHits=500,
			  nmaxMvdPixelHitsInTrack=30,
			  nmaxMvdStripHitsInTrack=30,
			  MAXTRACKSPEREVENT=maxTracks,
//			  MAXMVDTRACKSPEREVENT=50,
			  MAXMVDTRACKSPEREVENT=200,
			  MAXMCTRACKS=30,
			  MAXTURNSOFTRACK=0;
  static const Double_t   BFIELD=2.,  // in Tesla
			  PI = 3.141592654,
			  CVEL = 2.99792,  //  velocity of light
			  RStrawDetectorMin = 16., // minimum radius of the Stt detector in  cm
			  RStrawDetectorMax = 42.2, // maximum radius of the Stt detector in  cm
			  STRAWRADIUS = 0.5,
			  STRAWRESOLUTION= 0.015;
  bool    ExclusionListStt[nmaxSttHits];

	UShort_t	nMvdPixelHit,
			nMvdStripHit,
			nMvdTrackCand,
			nHitMvdTrackCand[MAXMVDTRACKSPEREVENT],
			ListHitMvdTrackCand[MAXMVDTRACKSPEREVENT]
			   [nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack],
			ListHitTypeMvdTrackCand[MAXMVDTRACKSPEREVENT]
			   [nmaxMvdPixelHitsInTrack+nmaxMvdStripHitsInTrack],
			nMvdDSPixelHitNotTrackCand,
			nMvdUSPixelHitNotTrackCand,
			ListMvdDSPixelHitNotTrackCand[nmaxMvdPixelHitsInTrack],
			ListMvdUSPixelHitNotTrackCand[nmaxMvdPixelHitsInTrack],
			nMvdDSStripHitNotTrackCand,
			nMvdUSStripHitNotTrackCand,
			ListMvdDSStripHitNotTrackCand[nmaxMvdStripHitsInTrack],
			ListMvdUSStripHitNotTrackCand[nmaxMvdStripHitsInTrack];

  Double_t	SEMILENGTH_STRAIGHT,
		ZCENTER_STRAIGHT,
		CxMC[MAXMCTRACKS],
             CyMC[MAXMCTRACKS],
             R_MC[MAXMCTRACKS],
             MCtruthTrkInfo[15][MAXMCTRACKS],
	     MCSkewAloneX[nmaxSttHits],
	     MCSkewAloneY[nmaxSttHits];
  Double_t
           XMvdPixel[nmaxMvdPixelHits],
           YMvdPixel[nmaxMvdPixelHits],
           ZMvdPixel[nmaxMvdPixelHits],
           sigmaXMvdPixel[nmaxMvdPixelHits],
           sigmaYMvdPixel[nmaxMvdPixelHits],
           sigmaZMvdPixel[nmaxMvdPixelHits],
           XMvdStrip[nmaxMvdStripHits],
           YMvdStrip[nmaxMvdStripHits],
           ZMvdStrip[nmaxMvdStripHits],
           sigmaXMvdStrip[nmaxMvdStripHits],
           sigmaYMvdStrip[nmaxMvdStripHits],
           sigmaZMvdStrip[nmaxMvdStripHits];






  /** object persistence **/
  Bool_t  fPersistence; //!

  PndGeoSttPar *fSttParameters;  //  CHECK added
 


  void BackgroundNandT(
			UShort_t *nBkgEventsToAdd,
			Double_t *times
			);



  ClassDef(PndMixBackgroundEvents,1);

};

#endif
