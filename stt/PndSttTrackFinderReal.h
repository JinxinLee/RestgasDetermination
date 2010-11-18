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
//class CbmMCPoint;
//class TClonesArray;


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
struct AssociatedHitsToHelix {
                 Int_t NAssociatedHits;
                 Int_t NAssociatedParallelHits;
                 Int_t NAssociatedSkewHits;
                 Int_t mAmbiguities[nmaxAssociatedHits];
                 Double_t AssociatedWireDirection[3][nmaxAssociatedHits];
                 Double_t AssociatedWireCenter[3][nmaxAssociatedHits];
                 Double_t AssociatedWireDriftRadius[nmaxAssociatedHits];
                 Double_t AssociatedHitsCoordinates[3][nmaxAmbiguities][nmaxAssociatedHits];

                 Int_t Hitnumber[nmaxAssociatedHits];//  temporaneo!
               } ;


class PndSttTrackFinderReal : public PndSttTrackFinder
{

 public:
  
  /** Default constructor **/
  PndSttTrackFinderReal();


  /** Standard constructor **/
  PndSttTrackFinderReal(Int_t verbose);


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

 private:


#define maxTracks 20


      static const UShort_t
            nmaxHits = maxTracks*26, // max 20  tracks in Stt, with average 26 hits per track
            MAXMCTRACKS=30,
            MAXTRACKSPEREVENT=maxTracks,
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

    static const  Double_t PI = 3.141592654,
                 RStrawDetectorMin = 16., // minimum radius of the Stt detector in  cm
                 RStrawDetectorMax = 42.2, // maximum radius of the Stt detector in  cm
                 Rmin=20.,  // this must be the same as RMINIMUM
                 Rmax=700.,
                 StrawRadius = 0.5,
                 StrawDriftError = 0.02,
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
               BFIELD=2.,  // in Tesla
               CVEL = 2.99792;  //  velocity of light


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
               RminStrawSkewArea , // delimitation of the skew area
               RmaxStrawSkewArea;




      int IVOLTE, ntimes, INTERO, ITRACCIA,
          N_INTENDED;
//  Double_t SEMILENGTH_STRAIGHT = 75.;
      static const bool  iplotta = false , ianalizza = true ;
      static const int  nmassimo=10;
      int istampa ;
      TH1F * hx;
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
    UShort_t  FromHitToMCTrack[nmaxHits],
//              FromMCTrackToHit[MAXMCTRACKS][nmaxHits],
              nHitsInMCTrack[MAXTRACKSPEREVENT],
//              FromMCTrackToSkewHit[MAXMCTRACKS][nmaxHits],
              nSkewHitsInMCTrack[MAXTRACKSPEREVENT];


  UShort_t nRdivConformalEffective,
           MINIMUMHITSPERTRACK,
           MINIMUMOUTERHITSPERTRACK,
           NSkewhits,
           infoparal[nmaxHits],
           infoskew[nmaxHits];


  Double_t radiaConf[nRdivConformal];



    UShort_t nMCTracks;

    Double_t SEMILENGTH_STRAIGHT,
             ZCENTER_STRAIGHT,
             CxMC[MAXTRACKSPEREVENT],
             CyMC[MAXTRACKSPEREVENT],
             R_MC[MAXTRACKSPEREVENT];
//             MCtruthTrkInfo[15][MAXMCTRACKS];

  TClonesArray* fMCTrackArray;

  PndMCTrack* pMCtr;


  /** Verbosity level **/
  Int_t fVerbose;





  TList fHitCollectionList;
  TList fPointCollectionList;

  PndSttHit* GetHitFromCollections(Int_t hitCounter);
  FairMCPoint* GetPointFromCollections(Int_t hitCounter);
  TClonesArray* fSttHitArray;






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
                                 UShort_t HitsinBoxConformal[nRdivConformal][nFidivConformal][nmaxHits],
                                 UShort_t  RConformalIndex[nmaxHits],
                                 UShort_t  FiConformalIndex[nmaxHits]);





  void PndStt_Merge_Sort(UShort_t n_ele, Double_t *array, UShort_t *ind);

  void PndStt_Merge(UShort_t nl, Double_t *left, UShort_t *ind_left, UShort_t nr,
                                         Double_t *right, UShort_t *ind_right,  Double_t *result, UShort_t *ind);



  Short_t PndSttFindTrackPatterninBoxConformal(
                                                  UShort_t NRCELLDISTANCE,
                                                  UShort_t NFiCELLDISTANCE,
                                                  UShort_t Nparal,
                                                  UShort_t ihit,
                                                  Double_t info[][7],
                                                  bool Exclusion_List[nmaxHits],
                                                  UShort_t RConformalIndex[nmaxHits],
                                                  UShort_t FiConformalIndex[nmaxHits],
                                                  UShort_t nBoxConformal[nRdivConformal][nFidivConformal],
                                                  UShort_t HitsinBoxConformal[nRdivConformal][nFidivConformal][nmaxHits],
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
                                                  bool ExclusionList[nmaxHits],
                                                  UShort_t RConformalIndex[nmaxHits],
                                                  UShort_t FiConformalIndex[nmaxHits],
                                                  UShort_t nBoxConformal[nRdivConformal][nFidivConformal],
                                                  UShort_t HitsinBoxConformal[nRdivConformal][nFidivConformal][nmaxHits],
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



  Short_t PndSttFitHelixCylinder( UShort_t nHitsinTrack,
//                                UShort_t iExclude ,
                          Double_t auxinfoparalConformal[][5],
                          UShort_t  nTracksFoundSoFar,
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



  Short_t PndSttFitHelixCylinder2( UShort_t nHitsinTrack,
                          Double_t auxinfoparalConformal[][5],
                          UShort_t  nTracksFoundSoFar,
                          Double_t rotationangle,
                          Double_t *trajectory_vertex,
                          UShort_t NMAX,
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
                          UShort_t nSkewHitsinTrack,
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


  UShort_t PndSttTrkAssociatedParallelHitsToHelixBis(
                   Double_t m,
                   Double_t q,
                   Short_t Status,
                   UShort_t nHitsinTrack,
                   UShort_t *ListHitsinTrack,
                   Int_t NhitsParallel,
                   Double_t infoparalConformal[][5],
                   UShort_t *RConformalIndex,
                   UShort_t *FiConformalIndex,
                   UShort_t nBoxConformal[nRdivConformal][nFidivConformal],
                   UShort_t HitsinBoxConformal[nRdivConformal][nFidivConformal][nmaxHits],
                   UShort_t *auxListHitsinTrack
                                                     );


  UShort_t PndSttTrkAssociatedParallelHitsToHelixTris(
                   Double_t m,
                   Double_t q,
                   Short_t Status,
                   UShort_t nHitsinTrack,
                   UShort_t *ListHitsinTrack,
                   Int_t NhitsParallel,
                   Double_t infoparalConformal[][5],
                   UShort_t *RConformalIndex,
                   UShort_t *FiConformalIndex,
                   UShort_t nBoxConformal[nRdivConformal][nFidivConformal],
                   UShort_t HitsinBoxConformal[nRdivConformal][nFidivConformal][nmaxHits],
                   UShort_t *auxListHitsinTrack
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
                   UShort_t HitsinBoxConformal[nRdivConformal][nFidivConformal][nmaxHits],
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

  AssociatedHitsToHelix PndSttTrkAssociatedHitsToHelix(
                   Double_t KAPPA,Double_t FI0,Double_t D,Double_t Fi,Double_t R,
                   Int_t Nhits, Double_t info[][7], Int_t Nincl, Int_t Minclinations[], Double_t inclination[][3],
                   Int_t imaxima, Int_t nMaxima 
                                                     );

  void plottamentiParalleleGenerali(   Int_t Nremaining, Float_t * RemainingR, Float_t * RemainingD,
                      Float_t * RemainingFi, Float_t * RemainingCX, Float_t * RemainingCY,
                      bool * Goodflag
                          ) ;

  void plottamentiParalleleconMassimo( char * tipo,  Int_t nMaxima,  Int_t Nremaining, Float_t * RemainingR, Float_t * RemainingD,
                      Float_t * RemainingFi, Float_t * RemainingCX, Float_t * RemainingCY,
                      Double_t Rup, Double_t Rlow, Double_t Dup, Double_t Dlow, Double_t Fiup, Double_t Filow
                                    ) ;

  void plottamentiSkewconMassimo(   Int_t nMaxima, Int_t jmaxima,
                      Int_t Nremaining2, Float_t * RemainingKAPPA, Float_t *RemainingFI0,
                      Double_t KAPPAlow, Double_t KAPPAup, Double_t FI0low, Double_t FI0up,
                      AssociatedHitsToHelix  ResultAssociatedHits,
                      Double_t HoughR, Double_t HoughD, Double_t HoughFi) ;

  void findmaximaDFiR( UShort_t BoxDFiR[nbinD][nbinFi][nbinR], Int_t MINIMUMCOUNTS,
                    Int_t * NumberofMaximaDFiR, Int_t  MaximaIndexesDFiR[][3], Int_t * STATUS);

  void findmaximaKFI0( UShort_t BoxKFI0[nbinKAPPA][nbinFI0], Int_t MINIMUMCOUNTS, 
                    Int_t *NumberofMaximaKFI0, Int_t  MaximaIndexesKFI0[][2], Int_t * STATUS);

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
                   UShort_t Nhits, UShort_t ListHitsinTrack[MAXTRACKSPEREVENT][nmaxHits],
                   Double_t info[][7], Int_t Nincl, Int_t Minclinations[], Double_t inclination[][3],
                   UShort_t imaxima
                                                     );

  void WriteMacroParallelAssociatedHitswithMC(
                   Double_t Ox,Double_t Oy,Double_t R,
                  Short_t TrackFoundaTrackMC,
                   UShort_t Nhits,
		UShort_t ListHitsinTrack[MAXTRACKSPEREVENT][nmaxHits],
                   Double_t info[][7],
                   UShort_t ifoundtrack,
		UShort_t nParalCommon[MAXTRACKSPEREVENT],
		UShort_t ParalCommonList[MAXTRACKSPEREVENT][nmaxHits],
		UShort_t nSpuriParinTrack[MAXTRACKSPEREVENT],
		UShort_t ParSpuriList[MAXTRACKSPEREVENT][nmaxHits],
		UShort_t nMCParalAlone[MAXTRACKSPEREVENT],
		UShort_t MCParalAloneList[MAXTRACKSPEREVENT][nmaxHits]
                                                     );
  void WriteMacroParallelHitsGeneral(
                   Int_t Nhits, Double_t info[][7], Int_t Nincl, Int_t Minclinations[], Double_t inclination[][3],
                   UShort_t nTracksFoundSoFar,
                   bool *TypeConf,
                   Double_t *ALFA, Double_t *BETA, Double_t *GAMMA
                                                     );

  void WriteMacroParallelHitsGeneralConformalwithMC(
                   Int_t Nhits, Double_t info[][7], Int_t Nincl, Int_t Minclinations[], Double_t inclination[][3],
                   UShort_t nTracksFoundSoFar,
                   bool *TypeConf,
                   Double_t *ALFA, Double_t *BETA, Double_t *GAMMA
                                                     );



  void WriteMacroParallelHitsConformalwithMCspecial(
                   Int_t Nhits,
//                   UShort_t iExclude,
                   Double_t auxinfoparalConformal[][5],
                   UShort_t nTracksFoundSoFar,
//                   Double_t m, Double_t q,
                   Double_t *ALFA,
                   Double_t * BETA,
                   Double_t * GAMMA,
                   Short_t Status, Double_t *trajectory_vertex
                                                     );




  void WriteMacroSkewAssociatedHits(
                   Double_t KAPPA,
                   Double_t FI0,
                   Double_t D,
                   Double_t Fi,
                   Double_t R,
                   Double_t info[][7],
                   Int_t Nincl,
                   Int_t Minclinations[],
                   Double_t inclination[][3],
                   Int_t imaxima, Int_t nMaxima, 
                   UShort_t nSkewHitsinTrack,
                   UShort_t ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxHits],
                   UShort_t nSkewCommon,
                   UShort_t SkewCommonList[MAXTRACKSPEREVENT][nmaxHits]
 
                                                     );


  void WriteMacroSkewAssociatedHitswithMC(
                   Double_t KAPPA,
                   Double_t FI0,
                   Double_t D,
                   Double_t Fi,
                   Double_t R,
                   Double_t info[][7],
                   Int_t Nincl,
                   Int_t Minclinations[],
                   Double_t inclination[][3],
                   Int_t imaxima, Int_t nMaxima, 
                   UShort_t nSkewHitsinTrack,
                   UShort_t ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxHits],
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
                   Double_t Fi_allowedforskew_low,
                   Double_t Fi_allowedforskew_up,
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
                   UShort_t TemporarynSkewHitsinTrack,
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
                                                     UShort_t nSkewHits,
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
                                                     Short_t  * Charge,
                                                     Double_t *Fi_initial_helix_referenceframe,
                                                     Double_t *Fi_final_helix_referenceframe
                                                       );


      void   PndSttOrdering(
                                                     Double_t oX,
                                                     Double_t oY,
                                                     Double_t info[][7],
                                                     UShort_t nParallelHits,
                                                     UShort_t *ListParallelHits,
                                                     UShort_t nSkewHits,
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
                                                     Double_t *Fi_low_limit,
                                                     Double_t *Fi_up_limit
                                                       );

      void   PndSttFindingAllowedAngularRangeforSkew(
                                                     Double_t oX,
                                                     Double_t oY,
                                                     Double_t R,
                                                     Short_t  Charge,
                                                     Double_t *Fi_allowedforskew_low,
                                                     Double_t *Fi_allowedforskew_up
                                                       );

      void WriteMacroParallelHitswithRfromMC(
                   Int_t Nhits, Double_t info[][7],
                   UShort_t nTracksFoundSoFar,
                   bool *TypeConf,
                   Double_t *Ox,
                   Double_t *Oy,
                   Short_t * daParTrackFoundaTrackMC
                                                     );



  void WriteMacroSkewAssociatedHitswithRfromMC(
                   Double_t KAPPA,Double_t FI0,Double_t D,Double_t Fi,Double_t R,
                   Int_t Nhits, Double_t info[][7],  Int_t Nincl, Int_t Minclinations[], Double_t inclination[][3],
                   Int_t imaxima, Int_t nMaxima 
                                                     );




    void AssociateFoundTrackstoMC(
		  Double_t info[][7],
                  UShort_t nTracksFoundSoFar,
                  UShort_t *nHitsinTrack,
                  UShort_t  ListHitsinTrack[MAXTRACKSPEREVENT][nmaxHits],
                  UShort_t *nSkewHitsinTrack,
                  UShort_t  ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxHits],
                  Short_t daTrackFoundaTrackMC[MAXTRACKSPEREVENT]
				);


    void AssociateFoundTrackstoMCbis(
		  Double_t info[][7],
                  UShort_t nTracksFoundSoFar,
                  UShort_t nHitsinTrack[MAXTRACKSPEREVENT],
                  UShort_t  ListHitsinTrack[MAXTRACKSPEREVENT][nmaxHits],
                  UShort_t nSkewHitsinTrack[MAXTRACKSPEREVENT],
                  UShort_t  ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxHits],
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
//                             Double_t info[][7],
//                             UShort_t infosk,
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
                          UShort_t TemporarynSkewHitsinTrack,
                          Double_t *S,
                          Double_t *Fi_initial_helix_referenceframe,
                          Short_t Charge
                          );


//----------------------------------------------

  ClassDef(PndSttTrackFinderReal,1);

};


#endif
