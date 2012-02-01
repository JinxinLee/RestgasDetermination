#ifndef PNDSTTTRACKFINDERREAL
#define PNDSTTTRACKFINDERREAL 1

#include "PndSttTrackFinder.h"
#include "PndMCTrack.h"
#include "PndSttTrack.h"

#include "TList.h"
#include "TClonesArray.h"
#include "TH1.h"
//-------------------------------




class PndSttTrack;
class PndSttHit;
class FairMCPoint;


struct CalculatedCircles {
                 Int_t Ncircles;
                 Double_t CX[8];
                 Double_t CY[8];
                 Double_t R[8];
               } ;


struct CalculatedHelix {
                 Int_t Nhelix[3];
                 Double_t CX;
                 Double_t CY;
                 Double_t R;
                 Double_t KAPPA[3][16];
                 Double_t FI0[3][16];
               } ;

#define            nmaxAmbiguities  10
#define            nmaxAssociatedHits  200


class PndSttTrackFinderReal : public PndSttTrackFinder
{

 public:
  
  /** Default constructor **/
  PndSttTrackFinderReal();


  /** Standard constructor **/
  PndSttTrackFinderReal(Int_t verbose);

  /** Second constructor **/
  PndSttTrackFinderReal( int istamp, bool  iplot, bool doMcComparison);

  /** Third constructor **/
  PndSttTrackFinderReal( int istamp, bool  iplot,
		bool doMcComparison, bool doSciTil);

  /** Destructor **/
  virtual ~PndSttTrackFinderReal();


  /** Initialisation **/
  virtual void Init();

  /** Histogramming **/

  void WriteHistograms();

  /** Track finding algorithm **/
  virtual Int_t DoFind(TClonesArray* trackCandArray, TClonesArray* trackArray, TClonesArray* helixHitArray);
  virtual Int_t DoFind(TClonesArray* mHitArray, TClonesArray* mHelixHitArray);
  
  virtual void AddHitCollection(TClonesArray* mHitArray, TClonesArray* mPointArray)
             {fHitCollectionList.Add(mHitArray); fPointCollectionList.Add(mPointArray);};
  
  TClonesArray *fTubeArray;
  void SetTubeArray(TClonesArray *tubeArray) { fTubeArray = tubeArray; };

  
  void SetInputBranchName( char* string1 )
  {
    sprintf(fSttBranch,"%s", string1);
    return;
  };


 private:


#define maximumTracks 50
 static const UShort_t
	nmaxHits = 1000, // max hits total.
	nmaxHitsInTrack=60,
	nmaxSciTilHits = 200, // max SciTil hits total.
	nmaxSciTilHitsInTrack = 2, // max SciTil hits in one track.
	MAXMCTRACKS=10000,
	MAXTRACKSPEREVENT=maximumTracks,
	MAXHITSINCELL=50,
	nmaxinclinationversors=20,
	nAdmittedRadia = 3,
	nbinCX=100,
	nbinCY = 100,
	nbinZ = 100,
	nbinR = 100,
	nbinD = 250,
	nbinFi= 250,
	nbinKAPPA = 200,
	nbinFI0 = 200,
	MINIMUMCOUNTSDFiR = 18,
	MINIMUMCOUNTSKAPPAFI0 = 5,
	MAXElementsOverThresholdinHough = 500,
	nRdivConformal =10,
	NHITSINFIT=15,
	DELTAnR = 2;   //  defines the range of nR in PndSttTrkAssociatedParallelHitsToHelixBis

#define RadiusMinStrawDetector 16.119
#define DiameterStrawTube  1.
    static const  Double_t PI = 3.141592654,
                 RStrawDetectorMin = RadiusMinStrawDetector, // minimum radius of the Stt detector in  cm
		ApotemaMaxInnerParStraw = 23.246827,
		ApotemaMinSkewStraw = 23.246827, // delimitation of the skew area
		ApotemaMaxSkewStraw = 31.517569, // delimitation of the skew area
		ApotemaMinOuterParStraw = 31.863369,
                 RStrawDetectorMax = 40.73, // maximum radius of the Stt detector in  cm
		VERTICALGAP = 4.,  // in cm, the gap between Left and Right sections of the
				   //  Central Detector.
                 Rmin=20.,
                 Rmax=700.,
		 PMAX=100.,
                 STRAWRADIUS = DiameterStrawTube/2. ,
                 StrawDriftError = 0.02,
		 SKEWinclination_DEGREES = 3.,
                 CXmin=-150.,  CXmax=150.,
                 CYmin=-300.,  CYmax=300.,
                 Dmin=-21.,    Dmax=21.,
                 KAPPAmin =-2.,  KAPPAmax = 2.,
                 Zmin = -75.,   Zmax = 75.,
               DELTA_R=5.,
               DELTA_Fi=0.3,
               DELTA_D=2.,
               DELTA_KAPPA=0.03,
               DELTA_FI0= 0.3,
	RMAXSCITIL = 50., // cm
	DIMENSIONSCITIL=2.85, // cm
               BFIELD=2.,  // in Tesla
               CVEL = 2.99792;  //  velocity of light
	static const bool YesClean = false;
	bool YesSciTil ;

//               nFidivConformal = (UShort_t) (    PI*RStrawDetectorMax /StrawRadius   );
       static const UShort_t  nFidivConformal = (UShort_t) (3.141592654 * 45./0.5)  ;

       Double_t Fimin,     Fimax,
                 FI0min, FI0max,
               stepD,
               stepFi,
               stepR,
               stepKAPPA,
               stepFI0,
               stepfineKAPPA,
               stepfineFI0,
		ALFA[MAXTRACKSPEREVENT],
		BETA[MAXTRACKSPEREVENT],
		GAMMA[MAXTRACKSPEREVENT];


      static const int TIMEOUT=60;  // timeout in seconds for the GLPK fitting.



      int IVOLTE, ntimes, INTERO,
          N_INTENDED;

 bool	iplotta,
	doMcComparison,
	TypeConf[MAXTRACKSPEREVENT];

      int istampa ;

      static const int  nmassimo=50;

	TH1F	*hdist,
		*hdistgoodlast,
		*hdistbadlast;

      FILE * HANDLE ;
      FILE * HANDLE2 ;
      FILE * HANDLEXYZ ;
      FILE * PHANDLEX ;
      FILE * PHANDLEY ;
      FILE * PHANDLEZ ;
      FILE * SHANDLEX ;
      FILE * SHANDLEY ;
      FILE * SHANDLEZ ;
    Double_t veritaMC[nmaxHits][3];
    UShort_t
              nHitsInMCTrack[MAXTRACKSPEREVENT],
              nSttSkewhitInMCTrack[MAXTRACKSPEREVENT];


  UShort_t nRdivConformalEffective,
           MINIMUMHITSPERTRACK,
           MINIMUMOUTERHITSPERTRACK,
           nSttSkewhit,
		nSciTHits,
           infoparal[nmaxHits],
           infoskew[nmaxHits];


  Double_t radiaConf[nRdivConformal];



    UShort_t nMCTracks;

    Double_t SEMILENGTH_STRAIGHT,
             ZCENTER_STRAIGHT,
             CxMC[MAXTRACKSPEREVENT],
             CyMC[MAXTRACKSPEREVENT],
             R_MC[MAXTRACKSPEREVENT];

  TClonesArray	*fMCTrackArray,
		*fSciTPointArray,
		*fSciTHitArray;

  PndMCTrack* pMCtr;


  /** Verbosity level **/
  Int_t fVerbose;





  TList fHitCollectionList;
  TList fPointCollectionList;

  PndSttHit* GetHitFromCollections(Int_t hitCounter);
  FairMCPoint* GetPointFromCollections(Int_t hitCounter);
  TClonesArray* fSttHitArray;


  /**  Branch name to be used to fetch the hits of the backgound mixed events  **/
  char	fSttBranch[100];


  void PndSttFromXYtoConformal(Double_t trajectory_vertex[3],
                        Double_t info[][7],
                        Int_t Nparal,
                        Double_t infoparalConformal[][5],
                        Int_t * status
                        );


  void PndSttFromXYtoConformal2(Double_t trajectory_vertex[3],
                                UShort_t nHitsinTrack ,
                                UShort_t iExclude ,
                                UShort_t *ListHits,
                                Double_t info[][7],
                                Double_t auxinfoparalConformal[][5],
                                Int_t * status
                                );


  void PndSttBoxConformalFilling(
			bool ExclusionList[nmaxHits],
			Double_t infoparalConformal[][5],Int_t Nparal,
			UShort_t nBoxConformal[nRdivConformal][nFidivConformal],
			UShort_t HitsinBoxConformal[][nRdivConformal][nFidivConformal],
			UShort_t  RConformalIndex[nmaxHits],
			UShort_t  FiConformalIndex[nmaxHits]
			);





  void Merge_Sort(UShort_t n_ele, Double_t *array, UShort_t *ind);

  void Merge(UShort_t nl, Double_t *left, UShort_t *ind_left, UShort_t nr,
                                         Double_t *right, UShort_t *ind_right,  Double_t *result, UShort_t *ind);



  Short_t PndSttFindTrackPatterninBoxConformal(
			UShort_t NRCELLDISTANCE,
			UShort_t NFiCELLDISTANCE,
			UShort_t Nparal,
			Short_t ihit, // seed hit;
			Short_t nRcell, // R cell of the seed hit;
					// can be negative beacuse of SciTil hits;
			UShort_t nFicell, // Fi cell of the seed hit;
			Double_t info[][7],
			bool Exclusion_List[nmaxHits],
			UShort_t RConformalIndex[nmaxHits],
			UShort_t FiConformalIndex[nmaxHits],
			UShort_t nBoxConformal[nRdivConformal][nFidivConformal],
			UShort_t HitsinBoxConformal[][nRdivConformal][nFidivConformal],
			UShort_t  *ListHitsinTrack
						);


  Short_t PndSttFindTrackPatterninBoxConformalSpecial(
		UShort_t NRCELLDISTANCE,
		UShort_t NFiCELLDISTANCE,
		UShort_t Nparal,
		UShort_t NparallelToSearch,
		UShort_t iSeed,
		UShort_t *ListHitsinTrackinWhichToSearch,
		Double_t info[][7],
		bool InclusionList[nmaxHits],
		UShort_t RConformalIndex[nmaxHits],
		UShort_t FiConformalIndex[nmaxHits],
		UShort_t nBoxConformal[nRdivConformal][nFidivConformal],
		UShort_t HitsinBoxConformal[][nRdivConformal][nFidivConformal],
		UShort_t  *OutputListHitsinTrack
							);


  Short_t PndSttFindTrackStrictCollection(
		UShort_t NFiCELLDISTANCE,
		UShort_t iSeed,   //  seed track (parallel notation) as fa as the Fi angle is concerned
		UShort_t NParallelToSearch,    //  n. of hits to search in ListHitsinTrackinWhichToSearch
		UShort_t *ListHitsinTrackinWhichToSearch,
		bool ExclusionList[nmaxHits],
		UShort_t FiConformalIndex[nmaxHits],
		UShort_t  *OutputListHitsinTrack
					);




//   la seguente funzione verra' sostituita da quella che viene dopo.
  Short_t PndSttFitHelixCylinder( UShort_t nHitsinTrack,
                          Double_t auxinfoparalConformal[][5],
                          UShort_t  nTracksFoundSoFar,
                          Double_t rotationangle,
                          Double_t *trajectory_vertex,
                          UShort_t NMAX,
                          Double_t *m,
                          Double_t *q
                          );


//----  questa funzione sostituira' quella precedente; questa funzione e'
//  identica a quella di sttmvd che funziona sia con soli STT hits che con
//  Stt+Mvd hits.
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

















  Short_t PndSttFitSZspace(
                          UShort_t nHitsinTrack,
                          Double_t *S,
                          Double_t *Z,
                          Double_t *DriftRadius,
                          Double_t rotationangle,
                          UShort_t NMAX,
                          Double_t *m,
                          Double_t *q
                          );

  Short_t PndSttFitSZspacebis(
                          UShort_t nSttSkewhitinTrack,
                          Double_t *S,
                          Double_t *Z,
                          Double_t *DriftRadius,
                          Double_t FInot,
                          UShort_t NMAX,
                          Double_t *m
                          );
  UShort_t PndSttTrkAssociatedParallelHitsToHelix(
                   Double_t Ox,
                   Double_t Oy,
                   Double_t R,
                   Int_t Nhits,
                   Double_t info[][7],
                   UShort_t *auxListHitsinTrack              //  this is the output
			   );



  UShort_t PndSttTrkAssociatedParallelHitsToHelixQuater(
		   bool ExclusionList[nmaxHits],
                   Double_t m,
                   Double_t q,
                   Short_t Status,
                   UShort_t nHitsinTrack,
                   UShort_t *ListHitsinTrack,
                   Int_t NhitsParallel,
                   Double_t Ox,
                   Double_t Oy,
                   Double_t R,
                   Double_t info[][7],
                   Double_t infoparalConformal[][5],
                   UShort_t *RConformalIndex,
                   UShort_t *FiConformalIndex,
                   UShort_t nBoxConformal[nRdivConformal][nFidivConformal],
                   UShort_t HitsinBoxConformal[][nRdivConformal][nFidivConformal],
                   UShort_t *auxListHitsinTrack
                                                     );

  UShort_t PndSttTrkAssociatedParallelHitsToHelix5(
		   bool ExclusionList[nmaxHits],
                   Int_t NhitsParallel,
                   Double_t Ox,
                   Double_t Oy,
                   Double_t R,
                   Double_t info[][7],
		Double_t Fi_low,
		Double_t Fi_up,
                   UShort_t *auxListHitsinTrack
                                                     );

 bool  PndSttAcceptHitsConformal( Double_t  distance,
                                                         Double_t  DriftConfR,
                                                         Double_t  StrawConfR
                                                      );


  CalculatedCircles PndSttTrkFindCircles(Double_t,Double_t,Double_t,Double_t,Double_t,Double_t,Double_t,Double_t,Double_t);

  CalculatedHelix PndSttTrkFindHelix(
      Double_t Ox, Double_t Oy, Double_t R,
      Double_t Zcenter1,Double_t Zcenter2,Double_t Zcenter3,
      Double_t semilengthStraight1, Double_t semilengthStraight2, Double_t semilengthStraight3,
      Double_t C0x1, Double_t C0y1, Double_t C0z1, Double_t semilengthSkew1,
      Double_t r1, Double_t vx1, Double_t vy1, Double_t vz1,
      Double_t C0x2, Double_t C0y2, Double_t C0z2, Double_t semilengthSkew2,
      Double_t r2, Double_t vx2, Double_t vy2, Double_t vz2,
      Int_t * STATUS
                                    );

  void calculateintersections(Double_t Ox,Double_t Oy,Double_t R,Double_t C0x,Double_t C0y,
                   Double_t C0z,Double_t r,Double_t vx,Double_t vy,Double_t vz,
                   Int_t *STATUS, Double_t* POINTS);


  void plottamentiParalleleGenerali(   Int_t Nremaining, Float_t * RemainingR, Float_t * RemainingD,
                      Float_t * RemainingFi, Float_t * RemainingCX, Float_t * RemainingCY,
                      bool * Goodflag
                          ) ;

  void plottamentiParalleleconMassimo( char * tipo,  Int_t nMaxima,  Int_t Nremaining, Float_t * RemainingR, Float_t * RemainingD,
                      Float_t * RemainingFi, Float_t * RemainingCX, Float_t * RemainingCY,
                      Double_t Rup, Double_t Rlow, Double_t Dup, Double_t Dlow, Double_t Fiup, Double_t Filow
                                    ) ;



  bool iscontiguous( int ncomponents, UShort_t * vec1, UShort_t *vec2);

  void clustering2 (
          UShort_t vec1[2],                                       // input
          int nListElements, UShort_t List[][2],                 // input
          int & nClusterElementsFound, UShort_t ClusterElementsFound[][2],  // output
          int & nRemainingElements, UShort_t  RemainingElements[][2]     // output
                );

  void clustering3 (
          UShort_t vec1[3],                                       // input
          int nListElements, UShort_t List[][3],                 // input
          int & nClusterElementsFound, UShort_t ClusterElementsFound[][3],  // output
          int & nRemainingElements, UShort_t  RemainingElements[][3]     // output
                );


  void WriteMacroParallelAssociatedHits(
                   Double_t Ox,Double_t Oy,Double_t R,
                   UShort_t Nhits, UShort_t ListHitsinTrack[MAXTRACKSPEREVENT][nmaxHitsInTrack],
                   Double_t info[][7], Int_t Nincl, Int_t Minclinations[], Double_t inclination[][3],
                   UShort_t imaxima,
		Int_t sequencial
                                                     );

  void WriteMacroParallelAssociatedHitswithMC(
                   Double_t Ox,Double_t Oy,Double_t R,
                  Short_t TrackFoundaTrackMC,
                   UShort_t Nhits,
		UShort_t ListHitsinTrack[MAXTRACKSPEREVENT][nmaxHitsInTrack],
                   Double_t info[][7],
                   UShort_t ifoundtrack,
		Int_t sequentialNTrack,
		UShort_t nParalCommon[MAXTRACKSPEREVENT],
		UShort_t ParalCommonList[MAXTRACKSPEREVENT][nmaxHits],
		UShort_t nSpuriParinTrack[MAXTRACKSPEREVENT],
		UShort_t ParSpuriList[MAXTRACKSPEREVENT][nmaxHits],
		UShort_t nMCParalAlone[MAXTRACKSPEREVENT],
		UShort_t MCParalAloneList[MAXTRACKSPEREVENT][nmaxHits]
                                                     );
  void WriteMacroParallelHitsGeneral(
		bool * keepit,		
                   Int_t Nhits, Double_t info[][7],
		    Int_t Nincl,
		     Int_t Minclinations[],
		     Double_t inclination[][3],
		   UShort_t nSciTilHits,
		   Double_t posizSciTil[nmaxSciTilHits][3],
                   UShort_t nTracksFoundSoFar
                                                     );

  void WriteMacroParallelHitsGeneralConformalwithMC(
		bool * keepit,
                   Int_t Nhits, Double_t info[][7],
		   Int_t Nincl,
		    Int_t Minclinations[],
		     Double_t inclination[][3],
		   UShort_t nSciTilHits,
		   Double_t posizSciTil[nmaxSciTilHits][3],
                   UShort_t nTracksFoundSoFar
                                                     );



  void WriteMacroParallelHitsConformalwithMCspecial(
                   Int_t Nhits,
                   Double_t auxinfoparalConformal[][5],
                   UShort_t nTracksFoundSoFar,
//                   Double_t *ALFA,
//                   Double_t * BETA,
//                   Double_t * GAMMA,
                   Short_t Status, Double_t *trajectory_vertex
                                                     );




  void WriteMacroSkewAssociatedHits(
		bool goodskewfit,
                   Double_t KAPPA,
                   Double_t FI0,
                   Double_t D,
                   Double_t Fi,
                   Double_t R,
                   Double_t info[][7],
                   Int_t Nincl,
                   Int_t Minclinations[],
                   Double_t inclination[][3],
                   Int_t imaxima,
		   Int_t sequentialNTrack,
                   UShort_t nSttSkewhitinTrack,
                   UShort_t ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxHitsInTrack]

                                                     );


  void WriteMacroSkewAssociatedHitswithMC(
		bool goodskewfit,
                   Double_t KAPPA,
                   Double_t FI0,
                   Double_t D,
                   Double_t Fi,
                   Double_t R,
                   Double_t info[][7],
                   Int_t Nincl,
                   Int_t Minclinations[],
                   Double_t inclination[][3],
                   Int_t imaxima,
		   Int_t sequentialNTrack,
                   UShort_t nSttSkewhitinTrack,
                   UShort_t ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxHitsInTrack],
                   UShort_t nSkewCommon,
                   UShort_t SkewCommonList[MAXTRACKSPEREVENT][nmaxHits],
                   UShort_t daTrackFoundaTrackMC,
		UShort_t nMCSkewAlone[MAXTRACKSPEREVENT],
		UShort_t MCSkewAloneList[MAXTRACKSPEREVENT][nmaxHits]
 
                                                     );


  UShort_t AssociateSkewHitsToXYTrack(
                   bool *ExclusionListSkew,
                   Double_t Ox,
                   Double_t Oy,
                   Double_t R,
                   Double_t info[][7],
                   Double_t inclination[][3],
                   Double_t Fi_low_limit,
                   Double_t Fi_up_limit,
//                   Double_t Fi_allowedforskew_low,
//                   Double_t Fi_allowedforskew_up,
                   Short_t Charge,
                   Double_t Fi_initial_helix_referenceframe,
                   Double_t Fi_final_helix_referenceframe,
                   UShort_t SkewList[nmaxHits][2], // output,  list of selected skew hits (in skew numbering)
                   Double_t *S,       //  output,  S coordinate of selected Skew hit
                   Double_t *Z,       //  output,  Z coordinate of selected Skew hit
                   Double_t *ZDrift,   //  output,  drift distance IN Z DIRECTION only, of selected Skew hit
                   Double_t *ZRadiusafterTilt   //  output,  Radius taking into account the tilt, IN Z DIRECTION only, of selected Skew hit
                                                     );

  UShort_t AssociateBetterAfterFitSkewHitsToXYTrack(
                   UShort_t TemporarynSttSkewhitinTrack,
                   UShort_t SkewList[nmaxHits][2], // input,  list of selected skew hits (in skew numbering)
                   Double_t *S,       //  input,  S coordinate of selected Skew hit
                   Double_t *Z,       //  input,  Z coordinate of selected Skew hit
                   Double_t *ZDrift,   //  input,  drift distance IN Z DIRECTION only, of selected Skew hit
                   Double_t *ZRadiusafterTilt,   //  input,  Radius taking into account the tilt, IN Z DIRECTION only, of selected Skew hit
                   Double_t KAPPA,    // input, KAPPA result of fit
                   Double_t FI0,    // input, FI0 result of fit
                   UShort_t *tempore,
                   Double_t *temporeS,  //  output, associated skew hit  S
                   Double_t *temporeZ,  //  output, associated skew hits Z
                   Double_t *temporeZDrift,  //  output, associated skew hit Z drift
                   Double_t *temporeZErrorafterTilt,  //  output, associated skew hits Z error after tilt
                   Int_t  *STATUS   // output
                                                     );


  void   PndSttFitwithKalman(
                                                     Double_t Ox,
                                                     Double_t Oy,
                                                     Double_t Pxini,
                                                     Double_t Pyini,
                                                     Double_t Pzini,
                                                     Double_t Ptras,
                                                     Double_t info[][7],
                                                     UShort_t nParallelHits,
                                                     UShort_t *ListParallelHits,
                                                     UShort_t nSttSkewhit,
                                                     UShort_t *ListSkewHits,
                                                     Double_t *S,
                                                     UShort_t *infoparal,
                                                     UShort_t *infoskew
                                                   );


      void   PndSttOrderingParallel(
		Double_t oX,
		Double_t oY,
		Double_t info[][7],
		UShort_t nParallelHits,
		UShort_t *ListParallelHits,
		UShort_t *Infoparal,
		Short_t  Charge,
		Double_t *Fi_initial_helix_referenceframe,
		Double_t *Fi_final_helix_referenceframe,
		Double_t *U,
		Double_t *V
				);


      void   PndSttOrderingSkewandParallel(
			UShort_t *Infoparal,
			UShort_t *Infoskew,
			Double_t oX,
			Double_t oY,
			Double_t Rr,
			UShort_t nSttSkewhit,
			UShort_t *ListSkewHits,
			Double_t *SList,
			Short_t  Charge,
			UShort_t nParHits,
			UShort_t *ListParHits,
			Double_t *U,
			Double_t *V,
			UShort_t *BigList

                                                       );

      void   PndSttOrdering(
                                                     Double_t oX,
                                                     Double_t oY,
                                                     Double_t info[][7],
                                                     UShort_t nParallelHits,
                                                     UShort_t *ListParallelHits,
                                                     UShort_t nSttSkewhit,
                                                     UShort_t *ListSkewHits,
                                                     Double_t *S,
                                                     UShort_t *Infoparal,
                                                     UShort_t *Infoskew,
                                                     UShort_t *nTotal,
                                                     UShort_t *BigList,
                                                     Short_t  * Charge
                                                       );

      void   PndSttFindingParallelTrackAngularRange(
		Double_t oX,
		Double_t oY,
		Double_t r,
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

      void WriteMacroParallelHitswithRfromMC(
                   Int_t Nhits, Double_t info[][7],
                   UShort_t nTracksFoundSoFar,
//                   bool *TypeConf,
                   Double_t *Ox,
                   Double_t *Oy,
                   Short_t * daParTrackFoundaTrackMC
                                                     );



  void WriteMacroSkewAssociatedHitswithRfromMC(
                   Double_t KAPPA,Double_t FI0,Double_t D,Double_t Fi,Double_t R,
                   Int_t Nhits, Double_t info[][7],
		     Int_t Nincl, Int_t Minclinations[], Double_t inclination[][3],
                   Int_t imaxima, Int_t nMaxima 
                                                     );




    void AssociateFoundTrackstoMC(
		  Double_t info[][7],
                  UShort_t nTracksFoundSoFar,
                  UShort_t *nHitsinTrack,
                  UShort_t  ListHitsinTrack[MAXTRACKSPEREVENT][nmaxHits],
                  UShort_t *nSttSkewhitinTrack,
                  UShort_t  ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxHits],
                  Short_t daTrackFoundaTrackMC[MAXTRACKSPEREVENT]
				);


    void AssociateFoundTrackstoMCbis(
		bool *keepit,
		  Double_t info[][7],
                  UShort_t nTracksFoundSoFar,
                  UShort_t nHitsinTrack[MAXTRACKSPEREVENT],
                  UShort_t  ListHitsinTrack[MAXTRACKSPEREVENT][nmaxHitsInTrack],
                  UShort_t nSttSkewhitinTrack[MAXTRACKSPEREVENT],
                  UShort_t  ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxHitsInTrack],
                  Short_t daTrackFoundaTrackMC[MAXTRACKSPEREVENT]
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


    void PndSttInfoXYZSkew (
                             Double_t Z,       //  Z coordinate of selected Skew hit
                             Double_t ZDrift,   // drift distance IN Z DIRECTION only, of Skew hit
                             Double_t S,
                             Double_t Ox,
                             Double_t Oy,
                             Double_t R,
                             Double_t KAPPA,
                             Double_t FI0,
                             Short_t Charge,
                             Double_t *Posiz      //  output
                            );

  void FixDiscontinuitiesFiangleinSZplane(
                          UShort_t TemporarynSttSkewhitinTrack,
                          Double_t *S,
                          Double_t *Fi_initial_helix_referenceframe,
                          Short_t Charge
                          );



	void   FindCharge(
		Double_t oX,
		Double_t oY,
		Double_t info[][7],
		UShort_t nParallelHits,
		UShort_t *ListParallelHits,
		UShort_t *Infoparal,
		Short_t  * Charge
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
			Double_t cut, // cut distance (in cm).
			UShort_t maxnum // max number allowed of failures to pass the cut.
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



	void ChooseEntranceExit(
			Double_t Oxx,
			Double_t Oyy,
			Short_t flag,
			Short_t  Charge,
			Double_t FiStart,
			UShort_t nIntersections[2],
			Double_t XintersectionList[][2],
			Double_t YintersectionList[][2],
			Double_t Xcross[2],	// output
			Double_t Ycross[2]	// output
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



	Short_t FindTrackEntranceExitbiHexagon(
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



	Short_t FindTrackEntranceExitHexagonCircle(
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





	void   OrderingUsingConformal(
		Double_t oX,
		Double_t oY,
		Int_t nHits,
		Double_t XY[][2],
		Int_t  Charge,  // input
		UShort_t *ListHits
							);


	bool FindTrackInXYProjection(
		Short_t iHit,	// seed hit; it is negative for SciTil Hits.
		UShort_t nRcell,  // R cell of the seed hit;
		UShort_t nFicell, // Fi cell of the seed hit;
		Int_t *Minclinations,
		Double_t info[nmaxHits][7],
		bool *ExclusionList,
		UShort_t *RConformalIndex,
		UShort_t *FiConformalIndex,
		UShort_t nBoxConformal[nRdivConformal][nFidivConformal],
		UShort_t HitsinBoxConformal[MAXHITSINCELL][nRdivConformal][nFidivConformal],
		UShort_t nTracksFoundSoFar,
		UShort_t *nHitsinTrack,
		UShort_t ListHitsinTrack[MAXTRACKSPEREVENT][nmaxHitsInTrack],
		UShort_t *nSciTilHitsinTrack,
		UShort_t ListSciTilHitsinTrack[MAXTRACKSPEREVENT][nmaxSciTilHitsInTrack],
		bool *InclusionListSciTil,
		Double_t *trajectory_vertex,
		Double_t infoparalConformal[nmaxHits][5],
		Double_t infoSciTilConformal[nmaxHits][3],
		Double_t *Ox,
		Double_t *Oy,
		Double_t *R,
		Double_t *Fi_low_limit,
		Double_t *Fi_up_limit,
		Double_t *Fi_initial_helix_referenceframe,
		Double_t *Fi_final_helix_referenceframe,
		Short_t * Charge,
		Double_t U[MAXTRACKSPEREVENT][nmaxHits],
		Double_t V[MAXTRACKSPEREVENT][nmaxHits]
					);


//----------------------------------------------

  ClassDef(PndSttTrackFinderReal,1);

};


#endif
