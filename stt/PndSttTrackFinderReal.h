#ifndef PNDSTTTRACKFINDERREAL
#define PNDSTTTRACKFINDERREAL 1

#include "PndSttTrackFinder.h"
#include "PndMCTrack.h"
// #include "CbmMCTrack.h"

#include "TList.h"
#include "TClonesArray.h"


#include "TGeoTorus.h"

class PndSttTrack;
class PndSttHit;
class FairMCPoint;
//class CbmMCPoint;
//class TClonesArray;

const Int_t nmaxHits = 200,
            nmaxinclinationversors=100,
            nmaxAmbiguities = 10,
            nmaxAssociatedHits = 200,
            nAdmittedRadia = 3,
            nbinCX=100,
            nbinCY = 100,
            nbinZ = 100,
            nbinR = 100,
            nbinD = 250,
            nbinFi= 250,
            nbinKAPPA = 200,
            nbinFI0 = 200,
            MAXSTTINFO=100000,
            minParallelAssociatedHits = 5,
            minSkewAssociatedHits=5,
            MINIMUMCOUNTSDFiR = 18,
            MINIMUMCOUNTSKAPPAFI0 = 5,
            MAXElementsOverThresholdinHough = 500;



const Double_t PI = 3.141592654;
const Double_t   StrawRadius = 0.5,
                 StrawDriftError = 0.03,
                 RMINIMUM= 100.,
                 CXmin=-150.,  CXmax=150.,
                 CYmin=-300.,  CYmax=300.,
                 Rmin=RMINIMUM,      Rmax=700.,
                 Dmin=-21.,    Dmax=21.,
                 Fimin=0.,     Fimax=2.*PI,
                 FI0min = 0., FI0max = 2.*PI,
                 KAPPAmin =-2.,  KAPPAmax = 2.,
                 Zmin = -75.,   Zmax = 75.,
               DELTA_R=5.,
               DELTA_Fi=0.3,
               DELTA_D=2.,
               DELTA_KAPPA=0.03,
               DELTA_FI0= 0.3,
               stepD=(Dmax-Dmin)/nbinD,
               stepFi=(Fimax-Fimin)/nbinFi,
               stepR=(Rmax-Rmin)/nbinR,
               stepKAPPA=(KAPPAmax-KAPPAmin)/nbinKAPPA,
               stepFI0=(FI0max-FI0min)/nbinFI0,
               stepfineKAPPA=2.*DELTA_KAPPA/nbinKAPPA,
               stepfineFI0=2.*DELTA_FI0/nbinFI0;


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
  virtual Int_t DoFind(TClonesArray* mHitArray);
  
  virtual void AddHitCollection(TClonesArray* mHitArray, TClonesArray* mPointArray) {fHitCollectionList.Add(mHitArray); fPointCollectionList.Add(mPointArray);}
  

 private:

  /** Verbosity level **/
  Int_t fVerbose;

  TList fHitCollectionList;
  TList fPointCollectionList;

  PndSttHit* GetHitFromCollections(Int_t hitCounter);
  FairMCPoint* GetPointFromCollections(Int_t hitCounter);

// added by Gianluigi from 22-9-2008 on ---------------------------------------

  void PndSttTrkFinderPartial(Int_t NN,Double_t information[][6],Int_t nincl,Int_t mincl[],
                        Double_t inclinationversors[][3]);

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
                   Int_t Nhits, Double_t info[][6], Int_t Nincl, Int_t Mincl[], Double_t inclination[][3],
                   Int_t imaxima, Int_t nMaxima 
                                                     );

  Int_t PndSttTrkAssociatedParallelHitsToHelix(
                   Double_t D,Double_t Fi,Double_t R,
                   Int_t Nhits, Double_t info[][6], Int_t Nincl, Int_t Mincl[], Double_t inclination[][3]
                                                     );


  void plottamentiParalleleGenerali(   Int_t Nremaining, Float_t * RemainingR, Float_t * RemainingD,
                      Float_t * RemainingFi, Float_t * RemainingCX, Float_t * RemainingCY
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
                   Double_t D,Double_t Fi,Double_t R,
                   Int_t Nhits, Double_t info[][6], Int_t Nincl, Int_t Mincl[], Double_t inclination[][3],
                   Int_t imaxima
                                                     );

  void WriteMacroParallelHitsGeneral(
                   Int_t Nhits, Double_t info[][6], Int_t Nincl, Int_t Mincl[], Double_t inclination[][3]
                                                     );

  void WriteMacroSkewAssociatedHits(
                   Double_t KAPPA,Double_t FI0,Double_t D,Double_t Fi,Double_t R,
                   Int_t Nhits, Double_t info[][6], Int_t Nincl, Int_t Mincl[], Double_t inclination[][3],
                   Int_t imaxima, Int_t nMaxima 
                                                     );


//----------------------------------------------

  ClassDef(PndSttTrackFinderReal,1);

};


#endif
