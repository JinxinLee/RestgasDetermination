#ifndef PNDSTTMVDTRACKING_H
#define PNDSTTMVDTRACKING_H 1


#include "PndMCTrack.h"
#include "PndSttTrack.h"
#include "FairTask.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairTrackParP.h"
#include "PndGeoSttPar.h"

#include "TVector3.h"
#include "TH1F.h"
#include "TH2F.h"

class TClonesArray;
class TObjectArray;

class PndSttMvdTracking : public FairTask
{

 public:

  /** Default constructor **/
  PndSttMvdTracking();

  PndSttMvdTracking(Int_t verbose);


  /** Destructor **/
  ~PndSttMvdTracking();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  void WriteHistograms();

  /** set persistence flag **/
  void SetPersistence(Bool_t persistence) { fPersistence = persistence; }
 

  void SetParContainers();


  void SetInputBranchName(	char* string1,
			char* string2,
			char* string3
		    )
  {
	sprintf(fSttBranch,"%s", string1);
	sprintf(fMvdPixelBranch,"%s", string2);
	sprintf(fMvdStripBranch,"%s", string3);
	return;
  };


 private:

  UShort_t nMCTracks;
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


  int static const TIMEOUT= 60;

  static const bool  iplotta = true , ianalizza = true ;
#define maxTracks 40
  static const UShort_t   nmaxSttHits = maxTracks*30,
			  MAXMCTRACKS=maxTracks,
			  MAXTRACKSPEREVENT=maxTracks,
			  nmaxMvdPixelHits=500,
			  nmaxMvdStripHits=500,
			  nmaxMvdPixelHitsInTrack=30,
			  nmaxMvdStripHitsInTrack=30,
			  MAXMVDTRACKSPEREVENT=400,
			  MAXTURNSOFTRACK=0;
  static const Double_t   BFIELD=2.,  // in Tesla
			  PI = 3.141592654,
			  CVEL = 2.99792,  //  velocity of light
			  RStrawDetectorMin = 16., // minimum radius of the Stt detector in  cm
			  RStrawDetectorMax = 42.2, // maximum radius of the Stt detector in  cm
			  STTdriftVEL = 0.0025,	//   in cm/nsec
			  STRAWRADIUS = 0.5,
			  STRAWRESOLUTION= 0.015;
  bool    ExclusionListStt[nmaxSttHits];

  UShort_t	nTrackCandHit[MAXTRACKSPEREVENT],
		nSttParHitsinTrack[MAXTRACKSPEREVENT],
		nSttSkewHitsinTrack[MAXTRACKSPEREVENT],
		nMvdPixelHitsinTrack[MAXTRACKSPEREVENT],
		nMvdStripHitsinTrack[MAXTRACKSPEREVENT],
		ListMvdPixelHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHits],
		ListMvdStripHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdStripHits],
		ListTrackCandHit[MAXTRACKSPEREVENT][nmaxSttHits+
				nmaxMvdPixelHitsInTrack+
				nmaxMvdStripHitsInTrack],
		ListSttParHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits],
		ListSttSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits]
		;

	Short_t	nMvdPixelHit,
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
			ListMvdUSStripHitNotTrackCand[nmaxMvdStripHitsInTrack],
	ListTrackCandHitType[MAXTRACKSPEREVENT][nmaxSttHits+  //  type = 0 --> Mvd Pixel
				nmaxMvdPixelHitsInTrack+   //  type = 1 --> Mvd Strip
				nmaxMvdStripHitsInTrack];  //  type = 2 --> Stt Parallel
							      //  type = 3 --> Stt Straw
							      //  type -1 -->  noise

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
           refindexMvdPixel[nmaxMvdPixelHits],
           XMvdStrip[nmaxMvdStripHits],
           YMvdStrip[nmaxMvdStripHits],
           ZMvdStrip[nmaxMvdStripHits],
           sigmaXMvdStrip[nmaxMvdStripHits],
           sigmaYMvdStrip[nmaxMvdStripHits],
           sigmaZMvdStrip[nmaxMvdStripHits],
           refindexMvdStrip[nmaxMvdPixelHits];

      FILE * HANDLE ;
      FILE * HANDLE2 ;
      FILE * HANDLEXYZ ;
      FILE * PHANDLEX ;
      FILE * PHANDLEY ;
      FILE * PHANDLEZ ;
      FILE * SHANDLEX ;
      FILE * SHANDLEY ;
      FILE * SHANDLEZ ;

  TH1F  *hdeltaRPixel,
	*hdeltaRStrip,
	*hdeltaRPixel2,
	*hdeltaRStrip2;


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
 /** Input array of PndTracksCand of Mvd**/
  TClonesArray* fMvdTrackCandArray;

 /** Input array of MC points  of Mvd**/
  TClonesArray* fMvdMCPointArray;



  /** Output array of PndSttMvd  PndTrackCand **/
  TClonesArray* fSttMvdPndTrackCandArray;
  /** Output array of PndSttMvd   PndTrack **/
  TClonesArray* fSttMvdPndTrackArray;



  /** object persistence **/
  Bool_t  fPersistence; //!

  PndGeoSttPar *fSttParameters;  //  CHECK added


  /**  Branch names to be used to fetch the hits of the backgound mixed events  **/

  char	fSttBranch[200],
		fMvdPixelBranch[200],
		fMvdStripBranch[200];


  void WriteMacroSttParallelAssociatedHitsandMvdwithMC(
                   Double_t Ox,
		   Double_t Oy,
		   Double_t R,
		   Double_t primoangolo,
		   Double_t ultimoangolo,
                   UShort_t Nhits,
		   UShort_t ListParHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits],
                   Double_t info[][7],
                   UShort_t imaxima,
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
		Double_t *SchosenSkew,
		UShort_t nSkewCommon[MAXTRACKSPEREVENT],
		UShort_t SkewCommonList[MAXTRACKSPEREVENT][nmaxSttHits],
		UShort_t nMCSkewAlone[MAXMCTRACKS],
		UShort_t MCSkewAloneList[MAXMCTRACKS][nmaxSttHits]
			);


  void WriteMacroParallelHitsGeneral(
                   Int_t Nhits,
		   Double_t info[][7],
                   UShort_t nTracksFoundSoFar,
                   Double_t *Ox,
		   Double_t *Oy,
		   Double_t *R,
		   Double_t *FI0,
		   Double_t *ultimoangolo,
		   Double_t *primoangolo
                                                     );      

  void WriteMacroParallelHitsGeneralspecial(
		   Double_t time,
                   Int_t Nhits,
		   Double_t info[][7],
                   UShort_t nTracksFoundSoFar,
                   Double_t *Ox,
		   Double_t *Oy,
		   Double_t *R,
		   Double_t *FI0,
		   Double_t *ultimoangolo,
		   Double_t *primoangolo
                                                     );      

  void WriteMacroSkewAssociatedHitswithMC(
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
						);



    void WriteMacroAllHitsRestanti(
		UShort_t nSttHit,
		UShort_t nSttParHit,
		UShort_t nSttSkewHit,
		Double_t info[][7],
		UShort_t nSttTrackCand,
		UShort_t nTrackCandHit[MAXTRACKSPEREVENT],
		UShort_t ListTrackCandHit[MAXTRACKSPEREVENT][nmaxSttHits+
	                           nmaxMvdPixelHitsInTrack+
				   nmaxMvdStripHitsInTrack],
		Short_t ListTrackCandHitType[MAXTRACKSPEREVENT][nmaxSttHits+
	                           nmaxMvdPixelHitsInTrack+
				   nmaxMvdStripHitsInTrack]
					);



    void AssociateFoundTrackstoMC(
		  Double_t info[][7],
                  UShort_t nTracksFoundSoFar,
                  UShort_t nHitsinTrack[MAXTRACKSPEREVENT],
                  UShort_t  ListHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits],
                  UShort_t nSkewHitsinTrack[MAXTRACKSPEREVENT],
                  UShort_t  ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits],
                  Short_t daTrackFoundaTrackMC[MAXTRACKSPEREVENT]

                                                        );


    void AssociateFoundTrackstoMCbis(
		  Double_t info[][7],
                  UShort_t nTracksFoundSoFar,
                  UShort_t nHitsinTrack[MAXTRACKSPEREVENT],
                  UShort_t  ListHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits],
                  UShort_t nSkewHitsinTrack[MAXTRACKSPEREVENT],
                  UShort_t  ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits],
                  Short_t daTrackFoundaTrackMC[MAXTRACKSPEREVENT]
					);


    void AssociateFoundTrackstoMCtris(
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
					);

    void AssociateFoundTrackstoMCquater(
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

		UShort_t nMvdPixelHitsinTrack[MAXTRACKSPEREVENT],
		UShort_t ListMvdPixelHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHits],
		Short_t *FromPixeltoMCTrack,
		UShort_t nMvdStripHitsinTrack[MAXTRACKSPEREVENT],
		UShort_t ListMvdStripHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdStripHits],
		Short_t *FromStriptoMCTrack,

                  Short_t daTrackFoundaTrackMC[MAXTRACKSPEREVENT]
					);

  void calculateintersections(Double_t Ox,Double_t Oy,Double_t R,Double_t C0x,Double_t C0y,
                   Double_t C0z,Double_t r,Double_t vx,Double_t vy,Double_t vz,
                   Int_t *STATUS, Double_t* POINTS);


  void SttMatchedSpurious(
			UShort_t ntotalHits,
			Double_t info[][7],
                        UShort_t nTracksFoundSoFar, //  quelle trovate dal PR
  			UShort_t nHitsinTrack[MAXTRACKSPEREVENT], // n. hits paralleli, dal PR
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
                                               );

  void MatchMvdHitsToSttTracks(
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
                    );




  void MatchMvdHitsToSttTracks2(
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
                    );

  void MatchMvdHitsToSttTracksagain(
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


			UShort_t nMvdPixelHitsAssociatedToSttTrack[MAXTRACKSPEREVENT], // output
			UShort_t ListMvdPixelHitsAssociatedToSttTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHits], // output
			UShort_t nMvdStripHitsAssociatedToSttTrack[MAXTRACKSPEREVENT], // output
			UShort_t ListMvdStripHitsAssociatedToSttTrack[MAXTRACKSPEREVENT][nmaxMvdStripHits] // output
                    );



  void CollectParSttHitsagain(
			bool *Mvdhits,
			Double_t info[][7],
			UShort_t nSttParHit,
			UShort_t ListAllParHits[nmaxSttHits],
			UShort_t StartTrackCand,
			UShort_t EndTrackCand,
			Double_t Ox[MAXTRACKSPEREVENT],
			Double_t Oy[MAXTRACKSPEREVENT],
			Double_t R[MAXTRACKSPEREVENT],
			Double_t KAPPA[MAXTRACKSPEREVENT],
			Double_t FI0[MAXTRACKSPEREVENT],
			Double_t Fi_low_limit[MAXTRACKSPEREVENT],
			Double_t Fi_up_limit[MAXTRACKSPEREVENT],
			UShort_t nSttParHitsinTrack[MAXTRACKSPEREVENT], // input/output
			UShort_t ListSttParHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHits] // input/output
                    );


  Short_t FitHelixCylinder( UShort_t nHitsinTrack,
		Double_t *Xconformal,
		Double_t *Yconformal,
		Double_t *DriftRadiusconformal,
		Double_t *ErrorDriftRadiusconformal,
		Double_t rotationangle,
		Double_t *trajectory_vertex,
		UShort_t NMAX,
		Double_t *m,
		Double_t *q,
		Double_t * ALFA,
		Double_t * BETA,
		Double_t * GAMMA,
		bool *TypeConf
			);



  Short_t FitSZspace(
			UShort_t nSkewHitsinTrack,
			Double_t *S,
			Double_t *Z,
			Double_t *DriftRadius,
			Double_t *ErrorDriftRadius,
			Double_t FInot,
			UShort_t NMAX,
			Double_t *emme
			);

  void RefitMvdStt(
			UShort_t nCandHit,
			UShort_t *ListTrackCandHit,
			Short_t *ListTrackCandHitType,
			Double_t info[][7],
			Double_t rotationangle,
			Double_t trajectory_vertex[2],
			Short_t iexcl,
			Double_t *ALFA, // output of the fit
			Double_t *BETA, // output of the fit
			Double_t *GAMMA,// set at zero always for now
			bool *status    // fit status; true = successful
			  );


  void MvdMatchtoMC(
		UShort_t nMvdMCPoint,
		Double_t *XMvdMCPoint,
		Double_t *YMvdMCPoint,
		Double_t *ZMvdMCPoint,
		Short_t *MCPointtoMCTrackID,

		Short_t *FromPixeltoMCTrack,	// output
		Short_t *FromStriptoMCTrack	// output
		   );


  void MvdMatchedSpurioustoTrackCand(
			UShort_t nSttTrackCand,				// input
			Short_t *daTrackFoundaTrackMC,			// input
//			UShort_t nMvdPixelHit,				// input
			Short_t *FromPixeltoMCTrack,			// input
//			UShort_t nMvdStripHit,				// input
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
				);



  void  Merge_Sort(
	UShort_t n_ele,
	Double_t *array,
	UShort_t *ind
	);

  void Merge(UShort_t nl, Double_t *left, UShort_t *ind_left, UShort_t nr,
	Double_t *right, UShort_t *ind_right,  Double_t *result, UShort_t *ind);


  void getMCInfo(
			Short_t MCTrack,
			Double_t * Cx,
			Double_t * Cy,
			Double_t * Rr
		);


  Double_t FindDistance(
			Double_t Ox,
			Double_t Oy,
			Double_t R,
			Double_t tanlow,
			Double_t tanmid,
			Double_t tanup,
			Double_t alfa,
			Double_t beta,
			Double_t gamma
			);

  void CalculateSandZ(
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
			);



  void EliminateSpuriousSZ(
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
			Double_t FI0,
			Double_t R
				    );




  Double_t Dist_SZ(
				Double_t R,
				Double_t KAPPA,
				Double_t FI0,
				Double_t ZED,
				Double_t S,
				Int_t *nrounds
			);




  Double_t SignedDist_SZbis(
				Double_t KAPPA,
				Double_t FI0,
				Double_t ZED,
				Double_t S,
				Short_t *nrounds
			);


    void PndSttInfoXYZParal (
                             Double_t info[][7],
                             UShort_t infopar,
                             Double_t Ox,
                             Double_t Oy,
                             Double_t R,
                             Double_t KAPPA,
                             Double_t FI0,
                             Short_t Charge,
                             Double_t *Posiz      //  output
                            );



   void FixDiscontinuitiesFiangleinSZplane(
                          UShort_t TemporarynSkewHitsinTrack,
                          Double_t *S,
                          Double_t *Fi_initial_helix_referenceframe,
                          Short_t Charge
                                                                );

  UShort_t AssociateSkewHitsToXYTrack(
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
							);

   void   PndSttFindingParallelTrackAngularRange(
			Double_t oX,
			Double_t oY,
			Double_t R,
			Short_t  Charge,
			Double_t *Fi_low_limit,
			Double_t *Fi_up_limit 
						);

   bool CalculateCircleThru3Points(
			Double_t x1,
			Double_t y1,
			Double_t x2,
			Double_t y2,
			Double_t x3,
			Double_t y3,
			Double_t *Ox,
			Double_t *Oy,
			Double_t *R
				);


   void   Ordering_Loading_ListTrackCandHit(
			UShort_t FirstCandidate,
			UShort_t LastCandidate,
			Double_t info[][7]
		  );

  ClassDef(PndSttMvdTracking,1);

};

#endif
