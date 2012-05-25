#ifndef PndTrkTracking_H
#define PndTrkTracking_H 1
#include <vector>

#include "FairTask.h"
#include "PndGeoSttPar.h"
#include "PndMCTrack.h"
#include "PndSttTrack.h"
#include "PndSttTrackFinder.h"
#include "PndSttTube.h"

#include "TClonesArray.h"
#include "TList.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH2F.h"


class FairMCPoint;

class PndTrkTracking : public FairTask
{

 public:

  /** Default constructor **/
  PndTrkTracking();

  /** Second constructor **/
  PndTrkTracking(Int_t verbose);

  /** Third constructor **/
  PndTrkTracking(int istamp, bool  iplot, bool doMcComparison);

  /** Fourth constructor **/
  PndTrkTracking(int istamp, bool  iplot, bool doMcComparison, bool doSciTil);

  /** Destructor **/
  ~PndTrkTracking();


  void Cleanup( ){YesClean=true; return;};


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  /** Virtual method Init **/
  virtual InitStatus Init();


  void NOCleanupMvd( ){YesCleanMvd=false; return;};


  void NoMvdAloneTracking( ){ MvdAloneTracking=false; return;};

  void SetInputBranchName(
	char* string1,
	char* string2,
	char* string3
	)
  {
	sprintf(fSttBranch,"%s", string1);
	sprintf(fMvdPixelBranch,"%s", string2);
	sprintf(fMvdStripBranch,"%s", string3);
	return;
  };


  void SetParContainers();

  /** set persistence flag **/
  void SetPersistence(Bool_t persistence) { fPersistence = persistence; }


  //  write out the histograms
  void WriteHistograms();



 private:


  static const UShort_t
	MAXMCTRACKS		= 10000,
	MAXMVDPIXELHITS		= 500,
	MAXMVDPIXELHITSINTRACK	= 10,
	MAXMVDSTRIPHITS		= 500,
	MAXMVDSTRIPHITSINTRACK	= 10,
	MAXMVDTRACKSPEREVENT	= 400,
	MAXSCITILHITS		= 200, // max SciTil hits total.
	MAXSCITILHITSINTRACK	= 2,   // max SciTil hits in one track.
	MAXSTTHITS		= 1050,
	MAXSTTHITSINTRACK	= 25,
	MAXTRACKSPEREVENT	= 200,
	NFIDIVCONFORMAL		= (UShort_t) (3.141592654 * 45./0.5),
	NRDIVCONFORMAL		= 10
	;
  bool
	doMcComparison,
	SingleHitListStt[MAXSTTHITS],
	iplotta,
	MvdAloneTracking,
	YesClean,
	YesCleanMvd,
	YesSciTil,
	InclusionListStt[MAXSTTHITS],
	InclusionListSciTil[MAXSCITILHITS],
	inMvdTrackCandPixel[MAXMVDPIXELHITS],
	inMvdTrackCandStrip[MAXMVDSTRIPHITS],
	TypeConf[MAXTRACKSPEREVENT];

  /** object persistence **/
  Bool_t
		fPersistence;



  /**  Branch names to be used to fetch the hits of the backgound mixed events  **/

  char
	fSttBranch[200],
	fMvdPixelBranch[200],
	fMvdStripBranch[200];




  UShort_t
	ListSttParHits[MAXSTTHITS],
	ListSttSkewHits[MAXSTTHITS],
	nMCTracks,
	nSciTilHits,
	nMvdPixelHitsinTrack[MAXTRACKSPEREVENT],
	nMvdStripHitsinTrack[MAXTRACKSPEREVENT],
	nSciTilHitsinTrack[MAXTRACKSPEREVENT],
	nSttParHitsinTrack[MAXTRACKSPEREVENT],
	nSttSkewHitsinTrack[MAXTRACKSPEREVENT],
	nTrackCandHit[MAXTRACKSPEREVENT],
	ListMvdPixelHitsinTrack[MAXTRACKSPEREVENT][MAXMVDPIXELHITSINTRACK],
	ListMvdStripHitsinTrack[MAXTRACKSPEREVENT][MAXMVDSTRIPHITSINTRACK],
	ListSttParHitsinTrack[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	ListSttSkewHitsinTrack[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	ListSttSkewHitsinTrackSolution[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],

	// accepted only 1 or 2 SciTil hits in a Track.
	ListSciTilHitsinTrack[MAXTRACKSPEREVENT][MAXSCITILHITSINTRACK],
	ListTrackCandHit[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK+MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK+MAXSCITILHITSINTRACK]
	;



  Short_t
	nMvdDSPixelHitNotTrackCand,
	nMvdDSStripHitNotTrackCand,
	nMvdPixelHit,
	nMvdStripHit,
	nMvdTrackCand,
	nMvdUSPixelHitNotTrackCand,
	nMvdUSStripHitNotTrackCand,
	ListHitMvdTrackCand[MAXMVDTRACKSPEREVENT][MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK],
	ListHitTypeMvdTrackCand[MAXMVDTRACKSPEREVENT][MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK],
	ListMvdDSPixelHitNotTrackCand[MAXMVDPIXELHITS],
	ListMvdUSPixelHitNotTrackCand[MAXMVDPIXELHITS],
	nHitMvdTrackCand[MAXMVDTRACKSPEREVENT],
	ListMvdDSStripHitNotTrackCand[MAXMVDSTRIPHITS],
	ListMvdUSStripHitNotTrackCand[MAXMVDSTRIPHITS],
  //  type = 0 --> Mvd Pixel;  type = 1 --> Mvd Strip; type = 1 --> Mvd Strip; type = 2 --> Stt Parallel
  //  type = 3 --> Stt Straw; type 1001 --> SciTil;  type -1 -->  noise.
	ListTrackCandHitType[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK+MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK+MAXSCITILHITSINTRACK];


  int
	istampa,
	IVOLTE ;


  Double_t
	ALFA[MAXTRACKSPEREVENT],
	BETA[MAXTRACKSPEREVENT],
	CxMC[MAXMCTRACKS],
	CyMC[MAXMCTRACKS],
	Fimin,
	GAMMA[MAXTRACKSPEREVENT],
	MCSkewAloneX[MAXSTTHITS],
	MCSkewAloneY[MAXSTTHITS],
	MCtruthTrkInfo[15][MAXMCTRACKS],
	Ox[MAXTRACKSPEREVENT],
	Oy[MAXTRACKSPEREVENT],
	posizSciTil[MAXSCITILHITS][3],
	R[MAXTRACKSPEREVENT],
	refindexMvdPixel[MAXMVDPIXELHITS],
	refindexMvdStrip[MAXMVDSTRIPHITS],
	radiaConf[NRDIVCONFORMAL],
	R_MC[MAXMCTRACKS],
	SciTilHitsXwithTrack[MAXTRACKSPEREVENT][MAXSCITILHITSINTRACK],
	SciTilHitsYwithTrack[MAXTRACKSPEREVENT][MAXSCITILHITSINTRACK],
	SEMILENGTH_STRAIGHT,
	sigmaXMvdPixel[MAXMVDPIXELHITS],
	sigmaYMvdPixel[MAXMVDPIXELHITS],
	sigmaZMvdPixel[MAXMVDPIXELHITS],
	sigmaXMvdStrip[MAXMVDSTRIPHITS],
	sigmaYMvdStrip[MAXMVDSTRIPHITS],
	sigmaZMvdStrip[MAXMVDSTRIPHITS],
	S_SciTilHitsinTrack[MAXTRACKSPEREVENT][MAXSCITILHITS],
	XMvdPixel[MAXMVDPIXELHITS],
	XMvdStrip[MAXMVDSTRIPHITS],
	YMvdPixel[MAXMVDPIXELHITS],
	YMvdStrip[MAXMVDSTRIPHITS],
	ZCENTER_STRAIGHT,
	ZMvdPixel[MAXMVDPIXELHITS],
	ZMvdStrip[MAXMVDSTRIPHITS];

  FILE
	* HANDLE,
	* HANDLE2,
	* HANDLEXYZ,
	* PHANDLEX,
	* PHANDLEY,
	* PHANDLEZ,
	* SHANDLEX,
	* SHANDLEY,
	* SHANDLEZ ;

  TH1F
	*hdeltaRPixel,
	*hdeltaRStrip,
	*hdeltaRPixel2,
	*hdeltaRStrip2;


  TClonesArray
 /** Input array of PndSttTube (map of STT tubes) **/
	*fMCTrackArray,
	*fSttTubeArray,
 /** Input array of PndSttPoints **/
	*fSttPointArray,
 /** Input array of PndSttHit **/
	*fSttHitArray,
 /** Input array of PndSttTracks **/
	*fSttTrackArray,
 /** Input array of PndTracksCand of Stt **/
	*fSttTrackCandArray,
 /** Input array of MC points  of Mvd**/
	*fMvdMCPointArray,
 /** Input array of MvdPixelHitArray **/
	*fMvdPixelHitArray,
 /** Input array of MvdStripHitArray **/
	*fMvdStripHitArray,
 /** Input array of PndTracksCand of Mvd**/
	*fMvdTrackCandArray,
 /** Input array of SciTil Hit Array **/
	*fSciTHitArray,
 /** Output array of PndSttMvd  PndTrackCand **/
	*fSttMvdPndTrackCandArray,
 /** Output array of PndSttMvd   PndTrack **/
	*fSttMvdPndTrackArray;

  PndGeoSttPar *fSttParameters;  //  CHECK added



  void Initialization_ClassVariables();


  bool  AcceptHitsConformal(
	Double_t  distance,
	Double_t  DriftConfR, //drift radius in conformal space
	Double_t  StrawConfR  // straw radius in conformal space
	);



  UShort_t AssociateBetterAfterFitSkewHitsToXYTrack(
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
	);

  void AssociateFoundTrackstoMC(
	Double_t info[][7],
	UShort_t nTracksFoundSoFar,
	UShort_t *nHitsinTrack,
	UShort_t  ListHitsinTrack[][MAXSTTHITS],
	UShort_t *nSkewHitsinTrack,
	UShort_t  ListSkewHitsinTrack[][MAXSTTHITS],
	Short_t *daTrackFoundaTrackMC
	);

  void AssociateFoundTrackstoMCbis(
	Double_t info[][7],
	UShort_t nTracksFoundSoFar,
	UShort_t nHitsinTrack[MAXTRACKSPEREVENT],
	UShort_t  ListHitsinTrack[MAXTRACKSPEREVENT][MAXSTTHITS],
	UShort_t nSkewHitsinTrack[MAXTRACKSPEREVENT],
	UShort_t  ListSkewHitsinTrack[MAXTRACKSPEREVENT][MAXSTTHITS],
	Short_t daTrackFoundaTrackMC[MAXTRACKSPEREVENT]
	);


  void AssociateFoundTrackstoMCtris(
	Double_t info[][7],
	UShort_t nTracksFoundSoFar,
	UShort_t nHitsinTrack[MAXTRACKSPEREVENT],
	UShort_t  ListHitsinTrack[MAXTRACKSPEREVENT][MAXSTTHITS],
	UShort_t nSkewHitsinTrack[MAXTRACKSPEREVENT],
	UShort_t  ListSkewHitsinTrack[MAXTRACKSPEREVENT][MAXSTTHITS],
	Short_t daTrackFoundaTrackMC[MAXTRACKSPEREVENT]
	);

  void AssociateFoundTrackstoMCquater(
	bool *keepit,
	Double_t info[][7],
	Double_t X1[MAXTRACKSPEREVENT],
	Double_t Y1[MAXTRACKSPEREVENT],
	Double_t X2[MAXTRACKSPEREVENT],
	Double_t Y2[MAXTRACKSPEREVENT],
	Double_t X3[MAXTRACKSPEREVENT],
	Double_t Y3[MAXTRACKSPEREVENT],
	UShort_t nTracksFoundSoFar,
	UShort_t nHitsinTrack[MAXTRACKSPEREVENT],
	UShort_t ListHitsinTrack[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	UShort_t nSkewHitsinTrack[MAXTRACKSPEREVENT],
	UShort_t ListSkewHitsinTrack[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],

	UShort_t nMvdPixelHitsinTrack[MAXTRACKSPEREVENT],
	UShort_t ListMvdPixelHitsinTrack[MAXTRACKSPEREVENT][MAXMVDPIXELHITSINTRACK],
	Int_t *FromPixeltoMCTrack,
	UShort_t nMvdStripHitsinTrack[MAXTRACKSPEREVENT],
	UShort_t ListMvdStripHitsinTrack[MAXTRACKSPEREVENT][MAXMVDSTRIPHITSINTRACK],
	Int_t *FromStriptoMCTrack,

	Short_t daTrackFoundaTrackMC[MAXTRACKSPEREVENT]
	);


  UShort_t AssociateSciTilHit(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	UShort_t *List, // output, list of SciTil hits associated (max. 2);
	Double_t *esse // output, list of  S of the SciTil hits associated. 
	);


  UShort_t AssociateSkewHitsToXYTrack(
	bool *InclusionListSkew,
	UShort_t NSkewhits,
	UShort_t *infoskew,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t info[][7],
	Double_t *WDX,
	Double_t *WDY,
	Double_t *WDZ,
	Double_t Fi_low_limit,
	Double_t Fi_up_limit,
	Short_t  Charge,
	UShort_t SkewList[][2], // output,list of selected skew hits (skew numbering)
	Double_t *S,       //  output,  S coordinate of selected Skew hit
	Double_t *Z,       //  output,  Z coordinate of selected Skew hit
	Double_t *ZDrift,   //  output,  drift distance IN Z DIRECTION only,
			   // of selected Skew hit
	Double_t *ZErrorafterTilt   //  output,  Radius taking into account
				// the tilt, IN Z DIRECTION only, of selected Skew hit.
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



  Double_t CalculateArcLength(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Short_t Charge,
	Double_t *Xcross, // entrance-exit point
	Double_t *Ycross // entrance-exit point
	);


  bool CalculateCircleThru3Points(
	Double_t x1,
	Double_t y1,
	Double_t x2,
	Double_t y2,
	Double_t x3,
	Double_t y3,
	Double_t *o_x,
	Double_t *o_y,
	Double_t *r_r
	);

  void calculateintersections(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t C0x,
	Double_t C0y,
	Double_t C0z,
	Double_t r,
	Double_t vx,
	Double_t vy,
	Double_t vz,
	Int_t *STATUS,
	Double_t* POINTS
	);



  void CalculateSandZ(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Short_t skewnum,
	Double_t info[][7],
	Double_t *WDX,
	Double_t *WDY,
	Double_t *WDZ,
	Double_t S[2],
	Double_t Z[2],
	Double_t Zdrift[2],
	Double_t Zerror[2]
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




  void CollectParSttHitsagain(
	bool *keepit,
	bool *Mvdhits,
	Double_t info[][7],
	UShort_t nSttParHit,
	UShort_t StartTrackCand,
	UShort_t EndTrackCand,
	Double_t *KAPPA,
	Double_t *FI0,
	Double_t *Fi_low_limit,
	Double_t *Fi_up_limit,
	UShort_t *nSttParHitsinTrack, // input/output
	UShort_t ListSttParHitsinTrack[][MAXSTTHITSINTRACK] // input/output
	);


  void ComparisonwithMC(
	Short_t * Charge,
	Short_t * daTrackFoundaTrackMC,
	Double_t *FI0,
	Double_t info[][7],
	Double_t *KAPPA,
	bool * keepit,
	UShort_t *nHitsInMCTrack,
	UShort_t *nMCParalAlone,
	UShort_t *nMCSkewAlone,
	UShort_t *nParalCommon,
	UShort_t *nSkewCommon,
	UShort_t *nSkewHitsInMCTrack,
	UShort_t *nSpuriParinTrack,
	UShort_t *nSpuriSkewinTrack,
	UInt_t nSttHit,
	UShort_t nTotalCandidates,
	UShort_t MCParalAloneList[][MAXSTTHITSINTRACK],
	UShort_t MCSkewAloneList[][MAXSTTHITSINTRACK],
	UShort_t ParalCommonList[][MAXSTTHITSINTRACK],
	UShort_t ParSpuriList[][MAXSTTHITSINTRACK],
	PndMCTrack* pMCtr,
	Short_t *resultFitSZagain,
	UShort_t SkewCommonList[][MAXSTTHITSINTRACK],
	UShort_t SkewSpuriList[][MAXSTTHITSINTRACK],
	bool *SttSZfit,

	UShort_t nMvdPixelCommon[],
	UShort_t MvdPixelCommonList[][MAXMVDPIXELHITSINTRACK],
	UShort_t nMvdPixelSpuriinTrack[],
	UShort_t MvdPixelSpuriList[][MAXMVDPIXELHITSINTRACK],
	UShort_t nMCMvdPixelAlone[],
	UShort_t MCMvdPixelAloneList[][MAXMVDPIXELHITSINTRACK],

	UShort_t nMvdStripCommon[],
	UShort_t MvdStripCommonList[][MAXMVDSTRIPHITSINTRACK],
	UShort_t nMvdStripSpuriinTrack[],
	UShort_t MvdStripSpuriList[][MAXMVDSTRIPHITSINTRACK],
	UShort_t nMCMvdStripAlone[],
	UShort_t MCMvdStripAloneList[][MAXMVDSTRIPHITSINTRACK]


	);

  Double_t Dist_SZ(
	Double_t Rr,
	Double_t KAPPA,
	Double_t FI0,
	Double_t ZED,
	Double_t S,
	Int_t *nrounds
	);


  void disegnaAssiXY(
	FILE * MACRO,
	double xmin,
	double xmax,
	double ymin,
	double ymax
	);


  void disegnaSciTilHit(
	FILE * MACRO,
	int ScitilHit,
	double posx,
	double posy,
	int tipo
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
	Double_t Rr
	);


  void ExtractInfoFromMvdTrackCand();




  void FindCharge(
	Double_t oX,
	Double_t oY,
	UShort_t nHits,
	Double_t *X,
	Double_t *Y,
	Short_t  * Charge
	);




  Double_t FindDistance(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t tanlow,
	Double_t tanmid,
	Double_t tanup,
	Double_t alfa,
	Double_t beta,
	Double_t gamma
	);




  void  FindingParallelTrackAngularRange(
	Double_t oX,
	Double_t oY,
	Double_t Rr,
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


  Short_t FindIntersectionsOuterCircle(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t RMax,
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



  Short_t FindTrackStrictCollection(
	UShort_t NFiCELLDISTANCE,
	//  seed track (original notation) as far as the Fi angle is concerned
	UShort_t iSeed,
	//  n. of hits to search in ListHitsinTrackinWhichToSearch
	UShort_t NParallelToSearch,
	UShort_t *ListHitsinTrackinWhichToSearch,
	bool *InclusionList,
	UShort_t *FiConformalIndex,
	UShort_t  *OutputListHitsinTrack
	);


  bool FindTrackInXYProjection(
	Short_t iHit,	// seed hit; it is negative for SciTil Hits.
	Short_t nRcell,  // R cell of the seed hit;
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
	Double_t *Fi_low_limit,
	Double_t *Fi_up_limit,
	Double_t *Fi_initial_helix_referenceframe,
	Double_t *Fi_final_helix_referenceframe,
	Short_t * Charge,
	Double_t *U,
	Double_t *V
	);


  Short_t FindTrackPatterninBoxConformal(
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
	);

  Short_t FindTrackPatterninBoxConformalSpecial(
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
	);


  void FixDiscontinuitiesFiangleinSZplane(
	UShort_t TemporarynSkewHitsinTrack,
	Double_t *S,
	Double_t *Fi_initial_helix_referenceframe,
	Short_t Charge
	);





  void getMCInfo(
	Int_t MCTrack,
	Double_t * Cx,
	Double_t * Cy,
	Double_t * Rr
	);



  void InfoXYZParal(
	Double_t info[][7],
	UShort_t infopar,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t KAPPA,
	Double_t FI0,
	Short_t Charge,
	Double_t *Posiz	//  output
	);



  bool IntersectionCircle_Segment(
	Double_t a, // coefficients implicit equation.
	Double_t b, // of segment : a*x + b*y + c =0.
	Double_t c,
	Double_t P1x, // point delimiting the segment.
	Double_t P2x, // point delimiting the segment.
	Double_t P1y, // point delimiting the segment.
	Double_t P2y, // point delimiting the segment.
	Double_t Oxx, // center of circle.
	Double_t Oyy,
	Double_t Rr, // Radius of circle.
	UShort_t * Nintersections,
	Double_t XintersectionList[2],
	Double_t YintersectionList[2],
	Double_t *distance
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


  Short_t   IntersectionsWithClosedbiHexagonLeft(
	Double_t vgap,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t Ami,	// Apotema min of inner Hexagon;
	Double_t Ama,	// Apotema max of outer Hexagon;

	//-------- outputs
	UShort_t *nIntersections,
	Double_t *XintersectionList,
	Double_t *YintersectionList
	);

  Short_t IntersectionsWithClosedbiHexagonRight(
	Double_t vgap,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t Ami,	// Apotema min of inner Hexagon;
	Double_t Ama,	// Apotema max of outer Hexagon;

	//-------- outputs
	UShort_t *nIntersections,
	Double_t *XintersectionList,
	Double_t *YintersectionList
	);


  Short_t  IntersectionsWithClosedPolygon(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t Rmi,	// Rmin of cylindrical volume intersected by track;
	Double_t Rma,	// Rmax of cylindrical volume intersected by track;

	//-------- outputs
	UShort_t nIntersections[2],
	Double_t XintersectionList[][2],
	Double_t YintersectionList[][2]
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





  UShort_t IntersectionsWithOpenPolygon(
	Double_t Oxx, // Track parameter
	Double_t Oyy, // Track parameter
	Double_t Rr, // Track parameter
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



  bool IsInternal(
	Double_t Px,	// point
	Double_t Py,
	Double_t Xtraslation,
	Double_t Ytraslation,
	Double_t Theta
	);


  void LoadPndTrack_TrackCand(
	bool *keepit,
	bool *SttSZfit,
	UShort_t nTotalCandidates,
	Short_t *Charge,
	UInt_t nSttTrackCand,
	Double_t *FI0,
	Double_t *KAPPA,
	Double_t info[][7],
	Double_t SchosenSkew[][MAXSTTHITS],
	Double_t ZchosenSkew[][MAXSTTHITS]
	);



  void MakeInclusionListStt(
	UInt_t nSttHit,
	Double_t info[][7]
	);


  void MatchMvdHitsToSttTracks(
	Double_t delta,
	UShort_t nSttTrackCand,
	Double_t *FI0,
	Double_t *Fifirst,

	UShort_t *nPixelHitsinTrack, // output
	UShort_t ListPixelHitsinTrack[][MAXMVDPIXELHITS], // output
	UShort_t *nStripHitsinTrack, // output
	UShort_t ListStripHitsinTrack[][MAXMVDSTRIPHITS] // output
	);



  void MatchMvdHitsToSttTracksagain(
	bool *keepit,
	bool *Mvdhits,
	Double_t delta,
	Double_t highqualitycut,
	UShort_t nSttTrackCand,
	Double_t *FI0,
	Double_t *Fifirst,
	Short_t *CHARGE,

	UShort_t *nPixelHitsinTrack, // output
	UShort_t ListPixelHitsinTrack[][MAXMVDPIXELHITSINTRACK], // output
	UShort_t *nStripHitsinTrack, // output
	UShort_t ListStripHitsinTrack[][MAXMVDSTRIPHITSINTRACK] // output
	);




  void MatchMvdHitsToSttTracks2(
	bool *keepit,
	Double_t delta,
	Double_t highqualitycut,
	UShort_t nSttTrackCand,
	Double_t *FI0,
	Double_t *Fifirst,
	Short_t *CHARGE,
	UShort_t *nPixelHitsinTrack, // output
	UShort_t ListPixelHitsinTrack[][MAXMVDPIXELHITSINTRACK], // output
	UShort_t *nStripHitsinTrack, // output
	UShort_t ListStripHitsinTrack[][MAXMVDSTRIPHITSINTRACK] // output
	);


  void Merge(
	UShort_t nl,
	Double_t *left,
	Int_t *ind_left,
	UShort_t nr,
	Double_t *right,
	Int_t *ind_right,
	Double_t *result,
	Int_t *ind
	);



  void  Merge_Sort(
	UShort_t n_ele,
	Double_t *array,
	Int_t *ind
	);


  void MvdMatchtoMC(
	UShort_t nMvdMCPoint,

	Int_t *FromPixeltoMCTrack,	// output
	Int_t *FromStriptoMCTrack	// output
	   );

  void MvdMatchedSpurioustoTrackCand(
	UShort_t nSttTrackCand,	// input
	bool *keepit,	// input
	Short_t *daTrackFoundaTrackMC,	// input
	Int_t *FromPixeltoMCTrack,	// input
	Int_t *FromStriptoMCTrack,	// input
	UShort_t *nPixelHitsinTrack,	// input
	UShort_t ListPixelHitsinTrack[][MAXMVDPIXELHITSINTRACK],// input
	UShort_t *nStripHitsinTrack,	// input
	UShort_t ListStripHitsinTrack[][MAXMVDSTRIPHITSINTRACK],// input

	UShort_t *nMvdPixelCommon,	// output
	UShort_t *MvdPixelCommonList,	// output
	UShort_t *nMvdPixelSpuriinTrack,	// output
	UShort_t *MvdPixelSpuriList,	// output
	UShort_t *nMCMvdPixelAlone,	// output
	UShort_t *MCMvdPixelAloneList,	// output

	UShort_t *nMvdStripCommon,	// output
	UShort_t *MvdStripCommonList,	// output
	UShort_t *nMvdStripSpuriinTrack,	// output
	UShort_t *MvdStripSpuriList,	// output
	UShort_t *nMCMvdStripAlone,	// output
	UShort_t *MCMvdStripAloneList	// output
	);


  void OrderingConformal_Loading_ListTrackCandHit(
	bool *keepit,
	UShort_t ncand,
	Double_t info[][7],
	Double_t Trajectory_Start[][2],
	Short_t *CHARGE,
	Double_t SchosenSkew[][MAXSTTHITS]
	);


  void  OrderingParallel(
	Double_t oX,
	Double_t oY,
	Double_t info[][7],
	UShort_t nParallelHits,
	UShort_t *ListParallelHits,
	Short_t  Charge,
	Double_t *Fi_initial_helix_referenceframe,
	Double_t *Fi_final_helix_referenceframe,
	Double_t *U,
	Double_t *V
	);

  void OrderingR_Loading_ListTrackCandHit(
	bool *keepit,
	UShort_t ncand,
	Double_t info[][7]
	);

  void OrderingSttSkewandSttParallel(
	Double_t oX,
	Double_t oY,
	Double_t Rr,
	UShort_t nSkewhit,
	UShort_t *ListSkewHits,
	Double_t *SList, // it is related to the skew hits. IMPORTANT :
		// the index must be the ORIGINAL skew hit number,
		// therefore SList[ListSkewHits[*]].
	Short_t  Charge,
	UShort_t nParHits,
	UShort_t *ListParHits,
	Double_t *U,
	Double_t *V,
	UShort_t *BigList // final ordered Parallel+Skew list;
		// already in NATIVE hit number.
	);

  void OrderingUsingConformal(
	Double_t oX,
	Double_t oY,
	Double_t Traj_Sta[2],
	Int_t nHits,
	Double_t XY[][2], // XY[*][0] = X position, XY[*][0] = Y position.
	Short_t  Charge,  // input
	Int_t *ListHits
	);




  void Ordering_Loading_ListTrackCandHit(
	bool *keepit,
	UShort_t FirstCandidate,
	UShort_t LastCandidate,
	Double_t info[][7],
	Double_t Trajectory_Start[][2],
	Short_t *CHARGE,
	Double_t SchosenSkew[][MAXSTTHITS]
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

  void StartFromSciTil(
	Short_t * Charge,
	UShort_t * FiConformalIndex,
	Double_t *Fi_final_helix_referenceframe,
	Double_t *Fi_initial_helix_referenceframe,
	Double_t * Fi_low_limit,
	Double_t * Fi_up_limit,
	UShort_t HitsinBoxConformal[][NRDIVCONFORMAL][NFIDIVCONFORMAL],
	Double_t info[][7],
	Double_t infoparalConformal[][5],
	UShort_t nBoxConformal[][NFIDIVCONFORMAL],
	UInt_t nSttParHit,
	UInt_t &nSttTrackCand,
	UShort_t * RConformalIndex,
	Double_t *trajectory_vertex,
	Double_t *UU,
	Double_t *VV
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


  void stampafinale(
	UInt_t nTotalCandidates,
	bool * keepit
	);

  void stampaMCSttPoints(
	UInt_t nSttMCPoint
	);

  void stampaMCTracks(
	);

  void stampaMvdHits();


  void stampaMvdTrackCandInfo();


  void stampaSttHits(
	Short_t iHit,
	Short_t ipunto,
	Double_t dradius,
	Double_t *WDX,
	Double_t *WDY,
	Double_t *WDZ,
	FairMCPoint* puntator,
	PndSttTube * pSttTube
	);

  void stampadopoTrackCand(
	Double_t info[][7],
	UShort_t nTotalCandidates
	);

  void stampetta(
	UShort_t nCandidate,
	bool *keepit
	);

  void SttInfoXYZParal(
	Double_t info[][7],
	UShort_t infopar,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t KAPPA,
	Double_t FI0,
	Short_t Charge,
	Double_t *Posiz
	);


  void SttMatchedSpurious(
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


  UShort_t TrkAssociatedParallelHitsToHelixQuater(
	bool *ExclusionList,
	Double_t m,
	Double_t q,
	Short_t Status,
	UShort_t nHitsinTrack,
	UShort_t *ListHitsinTrack,
	UInt_t NhitsParallel,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t info[][7],
	Double_t infoparalConformal[][5],
	UShort_t *RConformalIndex,
	UShort_t *FiConformalIndex,
	UShort_t nBoxConformal[][NFIDIVCONFORMAL],
	UShort_t HitsinBoxConformal[][NRDIVCONFORMAL][NFIDIVCONFORMAL],
	UShort_t *auxListHitsinTrack
	);



  UShort_t TrkAssociatedParallelHitsToHelix5(
	bool *ExclusionList,
	UInt_t NhitsParallel,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t info[][7],
	Double_t Fi_low,
	Double_t Fi_up,
	UShort_t *auxListHitsinTrack
	);

  void WriteAllMacros(
	Short_t *Charge,
	Double_t *FI0,
	Double_t info[][7],
	Double_t *KAPPA,
	bool* keepit,
	UShort_t *nMCParalAlone,
	UShort_t *nMCSkewAlone,
	UShort_t * nParalCommon,
	UShort_t * nSkewCommon,
	UShort_t * nSpuriParinTrack,
	UInt_t nSttHit,
	UInt_t nSttParHit,
	UInt_t nSttSkewHit,
	UShort_t nTotalCandidates,
	UShort_t MCParalAloneList[][MAXSTTHITSINTRACK],
	UShort_t MCSkewAloneList[][MAXSTTHITSINTRACK],
	UShort_t ParalCommonList[][MAXSTTHITSINTRACK],
	UShort_t ParSpuriList[][MAXSTTHITSINTRACK],
	Double_t SchosenSkew[][MAXSTTHITS],
	UShort_t SkewCommonList[][MAXSTTHITSINTRACK],
	Short_t * daTrackFoundaTrackMC,
	Double_t * WDX,
	Double_t * WDY,
	Double_t * WDZ,

	UShort_t nMvdPixelCommon[],
	UShort_t MvdPixelCommonList[][MAXMVDPIXELHITSINTRACK],
	UShort_t nMvdPixelSpuriinTrack[],
	UShort_t MvdPixelSpuriList[][MAXMVDPIXELHITSINTRACK],
	UShort_t nMCMvdPixelAlone[],
	UShort_t MCMvdPixelAloneList[][MAXMVDPIXELHITSINTRACK],

	UShort_t nMvdStripCommon[],
	UShort_t MvdStripCommonList[][MAXMVDSTRIPHITSINTRACK],
	UShort_t nMvdStripSpuriinTrack[],
	UShort_t MvdStripSpuriList[][MAXMVDSTRIPHITSINTRACK],
	UShort_t nMCMvdStripAlone[],
	UShort_t MCMvdStripAloneList[][MAXMVDSTRIPHITSINTRACK]
	);


  void WriteMacroAllHitsRestanti(
	UShort_t nSttHit,
	UShort_t nSttParHit,
	UShort_t nSttSkewHit,
	Double_t info[][7],
	bool * keepit,
	UShort_t nSttTrackCand,
	UShort_t nTrackCandHit[MAXTRACKSPEREVENT],
	UShort_t ListTrackCandHit[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK+
	MAXMVDPIXELHITSINTRACK+
	MAXMVDSTRIPHITSINTRACK+MAXSCITILHITSINTRACK],
	Short_t ListTrackCandHitType[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK+
	MAXMVDPIXELHITSINTRACK+
	MAXMVDSTRIPHITSINTRACK+MAXSCITILHITSINTRACK]
	);

  void WriteMacroParallelHitsGeneral(
	Int_t Nhits,
	Double_t info[][7],
	UShort_t nTracksFoundSoFar,
	bool *keepit,
	Double_t *FI0,
	Double_t *ultimoangolo,
	Double_t *primoangolo
	);



  void WriteMacroSkewAssociatedHitswithMC(
	Double_t KAPPA,
	Double_t FI0,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Short_t  charge,
	Double_t info[][7],
	Double_t WDX[MAXSTTHITS],
	Double_t WDY[MAXSTTHITS],
	Double_t WDZ[MAXSTTHITS],
	UShort_t iTrack,
	Int_t iNome,
	UShort_t nSkewHitsinTrack,
UShort_t ListSkewHitsinTrack[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	UShort_t nSkewCommon,
	UShort_t SkewCommonList[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	Short_t daTrackFoundaTrackMC,
	UShort_t nMCSkewAlone,
	UShort_t MCSkewAloneList[MAXMCTRACKS][MAXSTTHITSINTRACK],
	UShort_t nMvdPixelHitsAssociatedToSttTrack[MAXTRACKSPEREVENT], // output
UShort_t ListPixel[MAXTRACKSPEREVENT][MAXMVDPIXELHITSINTRACK], // output
	UShort_t nMvdStripHitsAssociatedToSttTrack[MAXTRACKSPEREVENT], // output
UShort_t ListStrip[MAXTRACKSPEREVENT][MAXMVDSTRIPHITSINTRACK], // output


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


  void WriteMacroSttParallelAssociatedHitsandMvdwithMC(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t primoangolo,
	Double_t ultimoangolo,
	UShort_t Nhits,
UShort_t ListParHitsinTrack[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
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
UShort_t ListPixel[MAXTRACKSPEREVENT][MAXMVDPIXELHITSINTRACK],
	UShort_t nMvdStripHitsAssociatedToSttTra,
UShort_t ListStrip[MAXTRACKSPEREVENT][MAXMVDSTRIPHITSINTRACK],

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
	UShort_t nMCSkewAlone[MAXMCTRACKS],
	UShort_t MCSkewAloneList[MAXMCTRACKS][MAXSTTHITSINTRACK]
	);






  ClassDef(PndTrkTracking,1);

};

#endif
