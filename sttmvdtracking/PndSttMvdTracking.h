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

  /** First constructor **/
  PndSttMvdTracking(Int_t verbose);

  /** Second constructor **/
  PndSttMvdTracking(int istamp, bool  iplot, bool doMcComparison);

  /** Third constructor **/
  PndSttMvdTracking(int istamp, bool  iplot, bool doMcComparison, bool doSciTil);

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

  void Cleanup( ){YesClean=true; return;};

  void NOCleanupMvd( ){YesCleanMvd=false; return;};

  void NoMvdAloneTracking( ){ MvdAloneTracking=false; return;};


 private:

  UShort_t nMCTracks;

#define DiameterStrawTube  1.
  int static const TIMEOUT= 60;

  int istampa ;

 bool
	iplotta,
	doMcComparison,
	YesSciTil ;


  int IVOLTE ;

#define maxTracks 200
 static const UShort_t
	nmassimo=50,
	nmaxSttHits = 1050,
	MAXMCTRACKS=10000,
	MAXTRACKSPEREVENT=maxTracks,
	nmaxMvdPixelHits=500,
	nmaxMvdStripHits=500,
	nmaxMvdMCPoints = 2000,
	nmaxSciTilHits = 200, // max SciTil hits total.
	nmaxSciTilHitsinTrack = 2, // max SciTil hits in one track.
	nmaxSttHitsInTrack=60,
	nmaxMvdPixelHitsInTrack=30,
	nmaxMvdStripHitsInTrack=30,
	MAXMVDTRACKSPEREVENT=400;

 static const Double_t   BFIELD=2.,  // in Tesla
			  PI = 3.141592654,
			  CVEL = 2.99792,  //  velocity of light
			  RStrawDetectorMin = 16.119, // minimum radius of the Stt detector in  cm
		ApotemaMaxInnerParStraw = 23.246827,
		ApotemaMinSkewStraw = 23.246827, // delimitation of the skew area
		ApotemaMaxSkewStraw = 31.517569, // delimitation of the skew area
		ApotemaMinOuterParStraw = 31.863369,
	DIMENSIONSCITIL=2.85, // cm
	ERRORPIXEL=0.02611,
	ERRORSTRIP=0.02611,
	errorsqPixel=0.00068175,
	errorsqStrip=0.00068175,
			  RStrawDetectorMax = 40.73, // maximum radius of the Stt detector in  cm
		VERTICALGAP = 4.,  // in cm, the gap between Left and Right sections of the
			  STTdriftVEL = 0.0025,	//   in cm/nsec
			  STRAWRADIUS = DiameterStrawTube/2.,
			  STRAWRESOLUTION= 0.015,
			  PMAX=100.;


  bool	YesClean,
	YesCleanMvd,
	MvdAloneTracking,
	InclusionListStt[nmaxSttHits],
	InclusionListSciTil[nmaxSciTilHits];


 UShort_t
	nSciTilHits,
	nTrackCandHit[MAXTRACKSPEREVENT],
	nSciTilHitsinTrack[MAXTRACKSPEREVENT],
	nSttParHitsinTrack[MAXTRACKSPEREVENT],
	nSttSkewHitsinTrack[MAXTRACKSPEREVENT],
	nMvdPixelHitsinTrack[MAXTRACKSPEREVENT],
	nMvdStripHitsinTrack[MAXTRACKSPEREVENT],
	ListMvdPixelHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHitsInTrack],
	ListMvdStripHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdStripHitsInTrack],
	ListSciTilHitsinTrack[MAXTRACKSPEREVENT][nmaxSciTilHitsinTrack],// possible only 1 or 2 hits.
	ListTrackCandHit[MAXTRACKSPEREVENT][nmaxSttHitsInTrack+
				nmaxMvdPixelHitsInTrack+
				nmaxMvdStripHitsInTrack+nmaxSciTilHitsinTrack],
	ListSttParHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],
	ListSttSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],
	ListSttSkewHitsinTrackSolution[MAXTRACKSPEREVENT][nmaxSttHitsInTrack]
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
			ListMvdDSPixelHitNotTrackCand[nmaxMvdPixelHits],
			ListMvdUSPixelHitNotTrackCand[nmaxMvdPixelHits],
			nMvdDSStripHitNotTrackCand,
			nMvdUSStripHitNotTrackCand,
			ListMvdDSStripHitNotTrackCand[nmaxMvdStripHits],
			ListMvdUSStripHitNotTrackCand[nmaxMvdStripHits],
	ListTrackCandHitType[MAXTRACKSPEREVENT][nmaxSttHitsInTrack+  //  type = 0 --> Mvd Pixel
				nmaxMvdPixelHitsInTrack+	//  type = 1 --> Mvd Strip
		nmaxMvdStripHitsInTrack+nmaxSciTilHitsinTrack];	//  type = 2 --> Stt Parallel
								//  type = 3 --> Stt Straw
								//  type 1001 --> SciTil
								//  type -1 -->  noise

  Double_t	SEMILENGTH_STRAIGHT,
		ZCENTER_STRAIGHT,
		CxMC[MAXMCTRACKS],
             CyMC[MAXMCTRACKS],
             R_MC[MAXMCTRACKS],
             MCtruthTrkInfo[15][MAXMCTRACKS],
	     MCSkewAloneX[nmaxSttHits],
	     MCSkewAloneY[nmaxSttHits],
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
           refindexMvdStrip[nmaxMvdPixelHits],
	ALFA[MAXTRACKSPEREVENT],
	BETA[MAXTRACKSPEREVENT],
	GAMMA[MAXTRACKSPEREVENT],
	posizSciTil[nmaxSciTilHits][3],
	SciTilHitsXwithTrack[MAXTRACKSPEREVENT][nmaxSciTilHitsinTrack],
	SciTilHitsYwithTrack[MAXTRACKSPEREVENT][nmaxSciTilHitsinTrack];

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

 /** Input array of SciTil Hit Array **/
  TClonesArray* fSciTHitArray;

 /** Input array of MC points  of Mvd**/
  TClonesArray* fMvdMCPointArray;



  /** Output array of PndSttMvd  PndTrackCand **/
  TClonesArray* fSttMvdPndTrackCandArray;
  /** Output array of PndSttMvd   PndTrack **/
  TClonesArray* fSttMvdPndTrackArray;



  /** object persistence **/
  Bool_t  fPersistence; //

  PndGeoSttPar *fSttParameters;  //  CHECK added


  /**  Branch names to be used to fetch the hits of the backgound mixed events  **/

  char	fSttBranch[200],
		fMvdPixelBranch[200],
		fMvdStripBranch[200];


  void Initialization_ClassVariables();


  void WriteMacroSttParallelAssociatedHitsandMvdwithMC(
                   Double_t Ox,
		   Double_t Oy,
		   Double_t R,
		   Double_t primoangolo,
		   Double_t ultimoangolo,
                   UShort_t Nhits,
UShort_t ListParHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],
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
UShort_t ListPixel[MAXTRACKSPEREVENT][nmaxMvdPixelHitsInTrack],
	UShort_t nMvdStripHitsAssociatedToSttTra,
UShort_t ListStrip[MAXTRACKSPEREVENT][nmaxMvdStripHitsInTrack],

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
		UShort_t nMCSkewAlone[MAXMCTRACKS],
		UShort_t MCSkewAloneList[MAXMCTRACKS][nmaxSttHitsInTrack]
			);


 void stampetta(
		UShort_t nCandidate,
		bool *keepit
		);




  void WriteMacroParallelHitsGeneral(
                   Int_t Nhits,
		   Double_t info[][7],
                   UShort_t nTracksFoundSoFar,
		   bool *keepit,
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
		   Short_t  charge,
                   Double_t info[][7],
                   Double_t WDX[nmaxSttHits],
                   Double_t WDY[nmaxSttHits],
                   Double_t WDZ[nmaxSttHits],
                   UShort_t iTrack,
			Int_t iNome,
                   UShort_t nSkewHitsinTrack,
UShort_t ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],
                   UShort_t nSkewCommon,
                   UShort_t SkewCommonList[MAXTRACKSPEREVENT][nmaxSttHitsInTrack],
                   Short_t daTrackFoundaTrackMC,
                   UShort_t nMCSkewAlone,
                   UShort_t MCSkewAloneList[MAXMCTRACKS][nmaxSttHitsInTrack],
		   UShort_t nMvdPixelHitsAssociatedToSttTrack[MAXTRACKSPEREVENT], // output
UShort_t ListPixel[MAXTRACKSPEREVENT][nmaxMvdPixelHitsInTrack], // output
		   UShort_t nMvdStripHitsAssociatedToSttTrack[MAXTRACKSPEREVENT], // output
UShort_t ListStrip[MAXTRACKSPEREVENT][nmaxMvdStripHitsInTrack], // output


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
						);



    void WriteMacroAllHitsRestanti(
		UShort_t nSttHit,
		UShort_t nSttParHit,
		UShort_t nSttSkewHit,
		Double_t info[][7],
		bool * keepit,
		UShort_t nSttTrackCand,
		UShort_t nTrackCandHit[MAXTRACKSPEREVENT],
		UShort_t ListTrackCandHit[MAXTRACKSPEREVENT][nmaxSttHitsInTrack+
	                           nmaxMvdPixelHitsInTrack+
				   nmaxMvdStripHitsInTrack+nmaxSciTilHitsinTrack],
Short_t ListTrackCandHitType[MAXTRACKSPEREVENT][nmaxSttHitsInTrack+
	                           nmaxMvdPixelHitsInTrack+
				   nmaxMvdStripHitsInTrack+nmaxSciTilHitsinTrack]
					);

    void DrawBiHexagonInMacro(
					Double_t vgap,
					FILE * MACRO,
					Double_t Ami,
					Double_t Ama,
					UShort_t color,
					char *name
				);


    void DrawHexagonCircleInMacro(
					Double_t GAP,
					FILE * MACRO,
					Double_t ApotemaMin,
					Double_t Rma,
					UShort_t color,
					char *name
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

		UShort_t nMvdPixelHitsinTrack[MAXTRACKSPEREVENT],
UShort_t ListMvdPixelHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHitsInTrack],
		Int_t *FromPixeltoMCTrack,
		UShort_t nMvdStripHitsinTrack[MAXTRACKSPEREVENT],
UShort_t ListMvdStripHitsinTrack[MAXTRACKSPEREVENT][nmaxMvdStripHitsInTrack],
		Int_t *FromStriptoMCTrack,

                  Short_t daTrackFoundaTrackMC[MAXTRACKSPEREVENT]
					);

  void calculateintersections(Double_t Ox,Double_t Oy,Double_t R,Double_t C0x,Double_t C0y,
                   Double_t C0z,Double_t r,Double_t vx,Double_t vy,Double_t vz,
                   Int_t *STATUS, Double_t* POINTS);


  void SttMatchedSpurious(
			bool *keepit,
			UShort_t ntotalHits,
			Double_t info[][7],
                        UShort_t nTracksFoundSoFar, //  quelle trovate dal PR
  			UShort_t nHitsinTrack[], // n. hits paralleli, dal PR
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

UShort_t nMvdPixelHitsAssociatedToSttTrack[MAXTRACKSPEREVENT], // output
UShort_t ListMvdPixelHitsAssociatedToSttTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHitsInTrack], // output
UShort_t nMvdStripHitsAssociatedToSttTrack[MAXTRACKSPEREVENT], // output
UShort_t ListMvdStripHitsAssociatedToSttTrack[MAXTRACKSPEREVENT][nmaxMvdStripHitsInTrack] // output
                    );

  void MatchMvdHitsToSttTracksagain(
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


UShort_t nMvdPixelHitsAssociatedToSttTrack[MAXTRACKSPEREVENT], // output
UShort_t ListMvdPixelHitsAssociatedToSttTrack[MAXTRACKSPEREVENT][nmaxMvdPixelHitsInTrack], // output
UShort_t nMvdStripHitsAssociatedToSttTrack[MAXTRACKSPEREVENT], // output
UShort_t ListMvdStripHitsAssociatedToSttTrack[MAXTRACKSPEREVENT][nmaxMvdStripHitsInTrack] // output
		);



  void CollectParSttHitsagain(
			bool *keepit,
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
UShort_t ListSttParHitsinTrack[MAXTRACKSPEREVENT][nmaxSttHitsInTrack] // input/output
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
		Double_t *pAlfa,
		Double_t *pBeta,
		Double_t *pGamma,
		bool *Type
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
			Double_t *pAlfa, // output of the fit
			Double_t *pBeta, // output of the fit
			Double_t *pGamma,// set at zero always for now
			bool *status    // fit status; true = successful
			  );


  void MvdMatchtoMC(
		UShort_t nMvdMCPoint,

		Int_t *FromPixeltoMCTrack,	// output
		Int_t *FromStriptoMCTrack	// output
		   );


  void MvdMatchedSpurioustoTrackCand(
			UShort_t nSttTrackCand,				// input
			bool * keepit,				// input
			Short_t *daTrackFoundaTrackMC,			// input
			Int_t *FromPixeltoMCTrack,			// input
			Int_t *FromStriptoMCTrack,			// input
			UShort_t *nMvdPixelHitsAssociatedToSttTrack,	// input
UShort_t ListPixel[MAXTRACKSPEREVENT][nmaxMvdPixelHitsInTrack],// input
			UShort_t *nMvdStripHitsAssociatedToSttTrack,	// input
UShort_t ListStrip[MAXTRACKSPEREVENT][nmaxMvdStripHitsInTrack],// input

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
	Int_t *ind
	);

  void Merge(UShort_t nl, Double_t *left, Int_t *ind_left, UShort_t nr,
	Double_t *right, Int_t *ind_right,  Double_t *result, Int_t *ind);


  void getMCInfo(
			Int_t MCTrack,
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
			UShort_t MaxTurnofTracks,
			UShort_t *nMvdPixelHitsAssociatedToSttTrack,
			UShort_t *ListMvdPixelHitsAssociatedToSttTrack,
			UShort_t *nMvdStripHitsAssociatedToSttTrack,
			UShort_t *ListMvdStripHitsAssociatedToSttTrack,
			UShort_t *nSttSkewHitsinTrack,
			UShort_t *ListSttSkewHitsinTrack,
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
                   bool *InclusionListSkew,
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
							);

   void   PndSttFindingParallelTrackAngularRange(
		Double_t oX,
		Double_t oY,
		Double_t R,
		Short_t  Charge,
		Double_t *Fi_low_limit,	// Fi (in XY Helix frame) lower limit using
					// the Stt detector minimum/maximum radius
					// Fi_low_limit is ALWAYS between 0. and 2PI
		Double_t *Fi_up_limit,	// Fi (in XY Helix frame) upper limit using
					// the Stt detector maximum/minimum radius
					// Fi_up_limit is ALWAYS > Fi_low_limit and
					// possibly > 2PI.
		Short_t * status,
		Double_t Rmin,	// Rmin of cylindrical volume intersected by track;
		Double_t Rmax	// Rmax of cylindrical volume intersected by track;
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
					);

  void OrderingR_Loading_ListTrackCandHit(
			bool *keepit,
			UShort_t ncand,
			Double_t info[][7]
				);

   void   OrderingConformal_Loading_ListTrackCandHit(
			bool *keepit,
			UShort_t ncand,
			Double_t info[][7],
			Double_t * Ox,
			Double_t * Oy,
			Double_t * Rr,
			Double_t Trajectory_Start[MAXTRACKSPEREVENT][2],
			Short_t *CHARGE,
			Double_t SchosenSkew[][nmaxSttHits]
					);


	void   OrderingUsingConformal(
		Double_t oX,
		Double_t oY,
		Double_t Traj_Sta[2],
		Int_t nHits,
		Double_t XY[][2], // XY[*][0] = X position, XY[*][0] = Y position.
		Short_t  Charge,  // input
		Int_t *ListHits
					);

//--------

	void SeparateInnerOuterParallel(

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
					);



//------ clenup methods

	bool TrackCleanup(
				Double_t GAP,
				Double_t Oxx,
				Double_t Oyy,
				Double_t Rr,
				Double_t KAPPA,
				Double_t FI0,
				Short_t  Charge,
				Double_t Start[3],
				UShort_t &nHitsPar,
				UShort_t *ListHitsPar,
				UShort_t &nHitsSkew,
				UShort_t *ListHitsSkew,
				Double_t *auxS,
				Double_t info[][7],
				Double_t RStrawDetMin,
				Double_t ApotemaMaxInnerPar,
				Double_t ApotemaMinSkew,
				Double_t ApotemaMaxSkew,
				Double_t ApotemaMinOuterPar,
				Double_t RStrawDetMax
			);


	bool SttParalCleanup(
			Double_t GAP,
			Double_t Oxx,
			Double_t Oyy,
			Double_t Rr,
			Short_t Charge,
			Double_t Start[3],
			Double_t FI0,
			Double_t FiLimitAdmissible,
			UShort_t nHits,
			UShort_t *Listofhits,
			Double_t info[][7],
			Double_t RStrawDetMin,
			Double_t RStrawDetInnerParMax,
			Double_t RStrawDetOuterParMin,
			Double_t RStrawDetMax
			);


	bool SttSkewCleanup(
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
			Double_t cut,
			UShort_t maxnum
				);

	bool BadTrack_ParStt(
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
			UShort_t islack // uncertainty allowed as far as
				// the n. of hits that should be present.
				);

//--------  geometry intersection methods.

	Short_t   IntersectionsWithClosedPolygon(
		Double_t Ox,
		Double_t Oy,
		Double_t R,
		Double_t Rmi,	// Rmin of cylindrical volume intersected by track;
		Double_t Rma,	// Rmax of cylindrical volume intersected by track;

		//-------- outputs
		UShort_t nIntersections[2],
		Double_t XintersectionList[][2],
		Double_t YintersectionList[][2]
					);



	UShort_t   IntersectionsWithOpenPolygon(
		Double_t Ox, // Track parameter
		Double_t Oy, // Track parameter
		Double_t R, // Track parameter
		UShort_t nSides, // input, n. of Sides of open Polygon.
		Double_t *a, //  coefficient of formula :  aX + bY + c = 0 defining
		Double_t *b, //  the Polygon sides.
		Double_t *c,
		Double_t *side_x,  // X,Y coordinate of the Sides vertices (in sequence, following
		Double_t *side_y,  // the Polygon along.
		//-------- outputs
		Double_t *XintersectionList, // XintersectionList
		Double_t *YintersectionList // YintersectionList.
					);

	Short_t   IntersectionsWithClosedbiHexagonLeft(
		Double_t vgap,
		Double_t Ox,
		Double_t Oy,
		Double_t R,
		Double_t Ami,	// Apotema min of inner Hexagon;
		Double_t Ama,	// Apotema max of outer Hexagon;

		//-------- outputs
		UShort_t *nIntersections,
		Double_t *XintersectionList,
		Double_t *YintersectionList
					);


	Short_t   IntersectionsWithClosedbiHexagonRight(
		Double_t vgap,
		Double_t Ox,
		Double_t Oy,
		Double_t R,
		Double_t Ami,	// Apotema min of inner Hexagon;
		Double_t Ama,	// Apotema max of outer Hexagon;

		//-------- outputs
		UShort_t *nIntersections,
		Double_t *XintersectionList,
		Double_t *YintersectionList
					);


	bool IntersectionCircle_Segment(
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
								);


	UShort_t IntersectionsWithGapSemicircle(
			Double_t Oxx,
			Double_t Oyy,
			Double_t Rr,
			Double_t gap,
			bool left,
			Double_t Rma,
			Double_t *XintersectionList,
			Double_t *YintersectionList
						);

	bool IsInternal(
			Double_t Px,	// point
			Double_t Py,
			Double_t Xtraslation,
			Double_t Ytraslation,
			Double_t Theta
					);



	void ChooseEntranceExitbis(
			Double_t Oxx,
			Double_t Oyy,
			Short_t  Charge,
			Double_t FiStart,
			UShort_t nIntersections,
			Double_t *XintersectionList,
			Double_t *YintersectionList,
			Double_t Xcross[2],	// output
			Double_t Ycross[2]	// output
					);


	Short_t FindTrackEntranceExitbiHexagonLeft(
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
					);

	Short_t FindTrackEntranceExitbiHexagonRight(
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
					);

	Short_t FindTrackEntranceExitHexagonCircleLeft(
				Double_t Oxx,
				Double_t Oyy,
				Double_t Rr,
				Short_t  Charge,
				Double_t Start[3],
				Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
				Double_t ApotemaMax,
				Double_t GAP,
				Double_t Xcross[2],
				Double_t Ycross[2]
					);
	Short_t FindTrackEntranceExitHexagonCircleRight(
				Double_t Oxx,
				Double_t Oyy,
				Double_t Rr,
				Short_t  Charge,
				Double_t Start[3],
				Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
				Double_t ApotemaMax,
				Double_t GAP,
				Double_t Xcross[2],
				Double_t Ycross[2]
					);

	Short_t FindIntersectionsOuterCircle(
				Double_t Oxx,
				Double_t Oyy,
				Double_t Rr,
				Double_t RMax,
				Double_t Xcross[2],
				Double_t Ycross[2]
					);

	bool IsInsideArc(
			Double_t Oxx,
			Double_t Oyy,
			Short_t Charge,
			Double_t Xcross[2],
			Double_t Ycross[2],
			Double_t Spoint
			);


	bool IsInTargetPipe(
			Double_t Oxx,
			Double_t Oyy,
			Double_t Rr,
			Double_t fi0,
			Double_t kappa,
			Short_t charge,
			Double_t gap
			);


	Double_t CalculateArcLength(
			Double_t Oxx,
			Double_t Oyy,
			Double_t Rr,
			Short_t Charge,
			Double_t Xcross[2], // entrance-exit point
			Double_t Ycross[2] // entrance-exit point
			);

	void   FindCharge(
		Double_t oX,
		Double_t oY,
		UShort_t nHits,
		Double_t *X,
		Double_t *Y,
		Short_t  * Charge
				);



	bool IntersectionSciTil_Circle(
			Double_t posizSciTilx,
			Double_t posizSciTily,
			Double_t Oxx, // center of circle.
			Double_t Oyy,
			Double_t Rr, // Radius of circle.
			UShort_t * Nintersections,
			Double_t XintersectionList[2],
			Double_t YintersectionList[2]
								);


	UShort_t AssociateSciTilHit(
		Double_t Oxx,
		Double_t Oyy,
		Double_t Rr,
		UShort_t *List, // output, list of SciTil hits associated (max. 2);
		Double_t *esse // output, list of  S of the SciTil hits associated. 
				);



	void disegnaSciTilHit(
			FILE * MACRO,
			int ScitilHit,
			double posx,
			double posy,
			int tipo
			);


	void disegnaAssiXY(
			FILE * MACRO,
			double xmin,
			double xmax,
			double ymin,
			double ymax
			);


  ClassDef(PndSttMvdTracking,1);

};

#endif
