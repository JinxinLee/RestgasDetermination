#ifndef PndTrkTracking_H
#define PndTrkTracking_H 1
#include <vector>

// #include "FairRootManager.h"
#include "FairTask.h"
#include "PndGeoSttPar.h"
#include "PndMCTrack.h"
#include "PndTrkPrintouts.h"
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


  static const Short_t
	MAXMCTRACKS		= 1000,
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
	NFIDIVCONFORMAL		= (Short_t) (3.141592654 * 45./0.5),
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




  Short_t
	ListHitMvdTrackCand[MAXMVDTRACKSPEREVENT][MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK],
	ListHitTypeMvdTrackCand[MAXMVDTRACKSPEREVENT][MAXMVDPIXELHITSINTRACK+MAXMVDSTRIPHITSINTRACK],
	ListMvdDSPixelHitNotTrackCand[MAXMVDPIXELHITS],
	ListMvdDSStripHitNotTrackCand[MAXMVDSTRIPHITS],
	ListMvdPixelHitsinTrack[MAXTRACKSPEREVENT][MAXMVDPIXELHITSINTRACK],
	ListMvdStripHitsinTrack[MAXTRACKSPEREVENT][MAXMVDSTRIPHITSINTRACK],
	ListMvdUSPixelHitNotTrackCand[MAXMVDPIXELHITS],
	ListMvdUSStripHitNotTrackCand[MAXMVDSTRIPHITS],
	ListSciTilHitsinTrack[MAXTRACKSPEREVENT][MAXSCITILHITSINTRACK],
	ListSttParHitsinTrack[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	ListSttParHits[MAXSTTHITS],
	ListSttSkewHitsinTrack[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	ListSttSkewHitsinTrackSolution[MAXTRACKSPEREVENT][MAXSTTHITSINTRACK],
	ListSttSkewHits[MAXSTTHITS],
	ListTrackCandHit[MAXTRACKSPEREVENT]
				[MAXSTTHITSINTRACK+
				MAXMVDPIXELHITSINTRACK+
				MAXMVDSTRIPHITSINTRACK+
				MAXSCITILHITSINTRACK],
  //  type = 0 --> Mvd Pixel;  type = 1 --> Mvd Strip; type = 1 --> Mvd Strip; type = 2 --> Stt Parallel
  //  type = 3 --> Stt Straw; type 1001 --> SciTil;  type -1 -->  noise.
	ListTrackCandHitType[MAXTRACKSPEREVENT]
				[MAXSTTHITSINTRACK+
				MAXMVDPIXELHITSINTRACK+
				MAXMVDSTRIPHITSINTRACK+
				MAXSCITILHITSINTRACK],
	nHitMvdTrackCand[MAXMVDTRACKSPEREVENT],
	nMCTracks,
	nMvdDSPixelHitNotTrackCand,
	nMvdDSStripHitNotTrackCand,
	nMvdPixelHit,
	nMvdPixelHitsinTrack[MAXTRACKSPEREVENT],
	nMvdStripHit,
	nMvdStripHitsinTrack[MAXTRACKSPEREVENT],
	nMvdTrackCand,
	nMvdUSPixelHitNotTrackCand,
	nMvdUSStripHitNotTrackCand,
	nSciTilHits,
	nSciTilHitsinTrack[MAXTRACKSPEREVENT],
	nSttParHitsinTrack[MAXTRACKSPEREVENT],
	nSttSkewHitsinTrack[MAXTRACKSPEREVENT],
	nTrackCandHit[MAXTRACKSPEREVENT];


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
	pSciTilx[MAXSCITILHITS],
	pSciTily[MAXSCITILHITS],
	pSciTilz[MAXSCITILHITS],
	R[MAXTRACKSPEREVENT],
	refindexMvdPixel[MAXMVDPIXELHITS],
	refindexMvdStrip[MAXMVDSTRIPHITS],
	radiaConf[NRDIVCONFORMAL],
	R_MC[MAXMCTRACKS],
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

//  FairRootManager *ioman;

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
  /** MC Track Array  **/
	*fMCTrackArray,
 /** Input array of MC points  of Mvd**/
	*fMvdMCPointArray,
 /** Input array of MvdPixelHitArray **/
	*fMvdPixelHitArray,
 /** Input array of MvdStripHitArray **/
	*fMvdStripHitArray,
 /** Input array of PndTracksCand of Mvd**/
	*fMvdTrackCandArray,
 /** SciTil Hit Array **/
	*fSciTHitArray,
 /** SciTil MC Point Array **/
	*fSciTPointArray,
 /** Input array of PndSttTube (map of STT tubes) **/
	*fSttTubeArray,
 /** Input array of PndSttPoints **/
	*fSttPointArray,
 /** Input array of PndSttHit **/
	*fSttHitArray,
 /** Input array of PndSttTracks **/
	*fSttTrackArray,
 /** Input array of PndTracksCand of Stt **/
	*fSttTrackCandArray,
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



  Short_t AssociateBetterAfterFitSkewHitsToXYTrack(
	Short_t TemporarynSttSkewhitinTrack,  //  input
	Short_t SkewList[][2], // input,  list of selected skew hits (in skew numbering)
	Double_t *S,       //  input,  S coordinate of selected Skew hit
	Double_t *Z,       //  input,  Z coordinate of selected Skew hit
	Double_t *ZDrift,  //  input,  drift distance IN Z DIRECTION only, of selected Skew hit
	Double_t *ZErrorafterTilt,   //  input,  Radius taking into account the tilt, IN Z DIRECTION only, of selected Skew hit
	Double_t KAPPA,    // input, KAPPA result of fit
	Double_t FI0,    // input, FI0 result of fit
	Short_t *tempore,  //  output result, associated skew hits
	Double_t *temporeS,  //  output, associated skew hit  S
	Double_t *temporeZ,  //  output, associated skew hits Z
	Double_t *temporeZDrift,  //  output, associated skew hit Z drift
	Double_t *temporeZErrorafterTilt,  //  output, associated skew hits Z error after tilt
	Short_t  *STATUS   // output
	);




  Short_t AssociateSciTilHit(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Short_t *List, // output, list of SciTil hits associated (max. 2);
	Double_t *esse // output, list of  S of the SciTil hits associated. 
	);


  Short_t AssociateSkewHitsToXYTrack(
	bool *InclusionListSkew,
	Short_t NSkewhits,
	Short_t *infoskew,
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
	Short_t SkewList[][2], // output,list of selected skew hits (skew numbering)
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
	Short_t nHits,
	Short_t* ListHits,
	Double_t info[][7],
	Double_t cut,
	Short_t maxnum,
	Short_t islack // uncertainty allowed as far as
	// the n. of hits that should be present.
	);



  void CollectParSttHitsagain(
	bool *keepit,
	bool *Mvdhits,
	Double_t info[][7],
	Short_t nSttParHit,
	Short_t StartTrackCand,
	Short_t EndTrackCand,
	Double_t *KAPPA,
	Double_t *FI0,
	Double_t *Fi_low_limit,
	Double_t *Fi_up_limit,
	Short_t *nSttParHitsinTrack, // input/output
	Short_t ListSttParHitsinTrack[][MAXSTTHITSINTRACK] // input/output
	);

  void EliminateSpuriousSZ(
	Short_t MaxTurnofTracks,
	Short_t *nMvdPixelHitsAssociatedToSttTrack,
	Short_t *ListMvdPixelHitsAssociatedToSttTrack,
	Short_t *nMvdStripHitsAssociatedToSttTrack,
	Short_t *ListMvdStripHitsAssociatedToSttTrack,
	Short_t *nSttSkewHitsinTrack,
	Short_t *ListSttSkewHitsinTrack,
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
	Short_t nHits,
	Double_t *X,
	Double_t *Y,
	Short_t  * Charge
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



  Short_t FindTrackStrictCollection(
	Short_t NFiCELLDISTANCE,
	//  seed track (original notation) as far as the Fi angle is concerned
	Short_t iSeed,
	//  n. of hits to search in ListHitsinTrackinWhichToSearch
	Short_t NParallelToSearch,
	Short_t *ListHitsinTrackinWhichToSearch,
	bool *InclusionList,
	Short_t *FiConformalIndex,
	Short_t  *OutputListHitsinTrack
	);






  void FixDiscontinuitiesFiangleinSZplane(
	Short_t TemporarynSkewHitsinTrack,
	Double_t *S,
	Double_t *Fi_initial_helix_referenceframe,
	Short_t Charge
	);




  void InfoXYZParal(
	Double_t info[][7],
	Short_t infopar,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t KAPPA,
	Double_t FI0,
	Short_t Charge,
	Double_t *Posiz	//  output
	);


  void LoadPndTrack_TrackCand(
	bool *keepit,
	bool *SttSZfit,
	Short_t nTotalCandidates,
	Short_t *Charge,
	Int_t nSttTrackCand,
	Double_t *FI0,
	Double_t *KAPPA,
	Double_t info[][7],
	Double_t SchosenSkew[][MAXSTTHITS],
	Double_t ZchosenSkew[][MAXSTTHITS]
	);



  void MakeInclusionListStt(
	Int_t nSttHit,
	Double_t info[][7]
	);


  void MatchMvdHitsToSttTracks(
	Double_t delta,
	Short_t nSttTrackCand,
	Double_t *FI0,
	Double_t *Fifirst,

	Short_t *nPixelHitsinTrack, // output
	Short_t ListPixelHitsinTrack[][MAXMVDPIXELHITS], // output
	Short_t *nStripHitsinTrack, // output
	Short_t ListStripHitsinTrack[][MAXMVDSTRIPHITS] // output
	);



  void MatchMvdHitsToSttTracksagain(
	bool *keepit,
	bool *Mvdhits,
	Double_t delta,
	Double_t highqualitycut,
	Short_t nSttTrackCand,
	Double_t *FI0,
	Double_t *Fifirst,
	Short_t *CHARGE,

	Short_t *nPixelHitsinTrack, // output
	Short_t ListPixelHitsinTrack[][MAXMVDPIXELHITSINTRACK], // output
	Short_t *nStripHitsinTrack, // output
	Short_t ListStripHitsinTrack[][MAXMVDSTRIPHITSINTRACK] // output
	);




  void MatchMvdHitsToSttTracks2(
	bool *keepit,
	Double_t delta,
	Double_t highqualitycut,
	Short_t nSttTrackCand,
	Double_t *FI0,
	Double_t *Fifirst,
	Short_t *CHARGE,
	Short_t *nPixelHitsinTrack, // output
	Short_t ListPixelHitsinTrack[][MAXMVDPIXELHITSINTRACK], // output
	Short_t *nStripHitsinTrack, // output
	Short_t ListStripHitsinTrack[][MAXMVDSTRIPHITSINTRACK] // output
	);



  void OrderingConformal_Loading_ListTrackCandHit(
	bool *keepit,
	Short_t ncand,
	Double_t info[][7],
	Double_t Trajectory_Start[][2],
	Short_t *CHARGE,
	Double_t SchosenSkew[][MAXSTTHITS]
	);


  void  OrderingParallel(
	Double_t oX,
	Double_t oY,
	Double_t info[][7],
	Short_t nParallelHits,
	Short_t *ListParallelHits,
	Short_t  Charge,
	Double_t *Fi_initial_helix_referenceframe,
	Double_t *Fi_final_helix_referenceframe,
	Double_t *U,
	Double_t *V
	);

  void OrderingR_Loading_ListTrackCandHit(
	bool *keepit,
	Short_t ncand,
	Double_t info[][7]
	);

  void OrderingSttSkewandSttParallel(
	Double_t oX,
	Double_t oY,
	Double_t Rr,
	Short_t nSkewhit,
	Short_t *ListSkewHits,
	Double_t *SList, // it is related to the skew hits. IMPORTANT :
		// the index must be the ORIGINAL skew hit number,
		// therefore SList[ListSkewHits[*]].
	Short_t  Charge,
	Short_t nParHits,
	Short_t *ListParHits,
	Double_t *U,
	Double_t *V,
	Short_t *BigList // final ordered Parallel+Skew list;
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
	Short_t FirstCandidate,
	Short_t LastCandidate,
	Double_t info[][7],
	Double_t Trajectory_Start[][2],
	Short_t *CHARGE,
	Double_t SchosenSkew[][MAXSTTHITS]
	);


  void RefitMvdStt(
	Short_t nCandHit,
	Short_t *ListTrackCandHit,
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
	Short_t * FiConformalIndex,
	Double_t *Fi_final_helix_referenceframe,
	Double_t *Fi_initial_helix_referenceframe,
	Double_t * Fi_low_limit,
	Double_t * Fi_up_limit,
	Short_t HitsinBoxConformal[][NRDIVCONFORMAL][NFIDIVCONFORMAL],
	Double_t info[][7],
	Double_t infoparalConformal[][5],
	Short_t nBoxConformal[][NFIDIVCONFORMAL],
	Int_t nSttParHit,
	Int_t &nSttTrackCand,
	Short_t * RConformalIndex,
	Double_t *trajectory_vertex,
	Double_t *UU,
	Double_t *VV
	);

  void SeparateInnerOuterParallel(

	// input
	Short_t nHits,
	Short_t *ListHits,
	Double_t info[][7],
	Double_t RStrawDetInnerParMax,

	// output
	Short_t *nInnerHits,
	Short_t *ListInnerHits,
	Short_t *nOuterHits,
	Short_t *ListOuterHits,

	Short_t *nInnerHitsLeft,
	Short_t *ListInnerHitsLeft,
	Short_t *nInnerHitsRight,
	Short_t *ListInnerHitsRight,

	Short_t *nOuterHitsLeft,
	Short_t *ListOuterHitsLeft,
	Short_t *nOuterHitsRight,
	Short_t *ListOuterHitsRight
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
	Short_t nHits,
	Short_t *Listofhits,
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
	Short_t nHits,
	Short_t *Listofhits,
	Double_t *S,
	Double_t info[][7],
	Double_t RminStrawSkew,
	Double_t RmaxStrawSkew,
	Double_t cut,
	Short_t maxnum
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
	Short_t &nHitsPar,
	Short_t *ListHitsPar,
	Short_t &nHitsSkew,
	Short_t *ListHitsSkew,
	Double_t *auxS,
	Double_t info[][7],
	Double_t RStrawDetMin,
	Double_t ApotemaMaxInnerPar,
	Double_t ApotemaMinSkew,
	Double_t ApotemaMaxSkew,
	Double_t ApotemaMinOuterPar,
	Double_t RStrawDetMax
	);


  Short_t TrkAssociatedParallelHitsToHelixQuater(
	bool *ExclusionList,
	Double_t m,
	Double_t q,
	Short_t Status,
	Short_t nHitsinTrack,
	Short_t *ListHitsinTrack,
	Int_t NhitsParallel,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t info[][7],
	Double_t infoparalConformal[][5],
	Short_t *RConformalIndex,
	Short_t *FiConformalIndex,
	Short_t nBoxConformal[][NFIDIVCONFORMAL],
	Short_t HitsinBoxConformal[][NRDIVCONFORMAL][NFIDIVCONFORMAL],
	Short_t *auxListHitsinTrack
	);



  Short_t TrkAssociatedParallelHitsToHelix5(
	bool *ExclusionList,
	Int_t NhitsParallel,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t info[][7],
	Double_t Fi_low,
	Double_t Fi_up,
	Short_t *auxListHitsinTrack
	);





  ClassDef(PndTrkTracking,1);

};

#endif
