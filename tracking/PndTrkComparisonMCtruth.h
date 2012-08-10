#ifndef PndTrkComparisonMCtruth_H
#define PndTrkComparisonMCtruth_H 1

// #include "FairRootManager.h"
#include "PndMCTrack.h"
// Root includes
#include "TROOT.h"
#include "TClonesArray.h"


struct PndTrkComparisonMCtruth_io_Data{
	Double_t Bfield;
	Short_t *Charge;
	Double_t Cvel;
	Short_t *daTrackFoundaTrackMC;
	Double_t DIMENSIONSciTil ;
	Double_t Errorsqpixel;
	Double_t Errorsqstrip;
	Double_t *FI0;
	TClonesArray *fMCTrackArray;
	TClonesArray *fMvdMCPointArray;
	Short_t fSciTilMaxNumber;
	TClonesArray *fSciTHitArray;
	TClonesArray *fSciTPointArray;
	FILE     *HANDLE;
	FILE     *HANDLE2;
	Double_t  (*info)[7];
	int       istampa;
	int       IVOLTE;
	Double_t *KAPPA;
	bool     *keepit;
	bool     *InclusionListStt;
	Short_t  *ListMvdPixelHitsinTrack;
	Short_t  *ListMvdStripHitsinTrack;
	Short_t  *ListSciTilHitsinTrack;
	Short_t  *ListSttParHitsinTrack;
	Short_t  *ListSttSkewHitsinTrack;
	Short_t  *ListTrackCandHit;
	Short_t  *ListTrackCandHitType;
	int       MAXMCTRACKS;
	int	  MAXMVDPIXELHITS;
	int       MAXMVDPIXELHITSINTRACK;
	int       Maxmvdmcpoints;
	int	  MAXMVDSTRIPHITS;
	int       MAXMVDSTRIPHITSINTRACK;
	int	  MAXSCITILHITS;
	int       MAXSCITILHITSINTRACK;
	int	  MAXSTTHITS;
	int	  MAXSTTHITSINTRACK;
	int	  MAXTRACKSPEREVENT;
	Short_t  *MCMvdPixelAloneList;
	Short_t  *MCMvdStripAloneList;
	Short_t  *MCParalAloneList;
	Short_t  *MCSkewAloneList;
	Short_t  *MvdPixelCommonList;
	Short_t  *MvdPixelSpuriList;
	Short_t  *MCSciTilAloneList; // equivalent to a matrix
			//  [nTotalCandidates][nSciTilHits];
	Short_t  *MvdStripCommonList;
	Short_t  *MvdStripSpuriList;
	Short_t  *nHitsInMCTrack;
	Short_t  *nHitsInSciTile;
	Short_t  *nMCMvdPixelAlone;
	Short_t  *nMCMvdStripAlone;
	Short_t  *nMCParalAlone;
	Short_t  *nMCSciTilAlone;
	Short_t  *nMCSkewAlone;
	Short_t  *nMvdPixelCommon;
	Short_t  *nMvdPixelHitsinTrack;
	Short_t  *nMvdStripHitsinTrack;
	Short_t   nMvdPixelHit;
	Short_t  *nMvdPixelSpuriinTrack;
	Short_t  *nMvdStripCommon;
	Short_t   nMvdStripHit;
	Short_t  *nMvdStripSpuriinTrack;
	Short_t  *nParalCommon;
	Short_t  *nSciTilCommon;
	Short_t   nSciTilHits;
	Short_t  *nSciTilHitsinTrack;
	Short_t  *nSciTilSpuriinTrack;
	Short_t  *nSkewCommon;
	Short_t  *nSkewHitsInMCTrack;
	Short_t  *nSpuriParinTrack;
	Short_t  *nSpuriSkewinTrack;
	Int_t     nSttHit;
	Short_t  *nSttParHitsinTrack;
	Short_t  *nSttSkewHitsinTrack;
	Short_t   nTotalCandidates;
	Short_t  *OriginalSciTilList;
	Double_t *Ox;
	Double_t *Oy;
	Short_t  *ParalCommonList;
	Short_t  *ParSpuriList;
	Double_t *R;
	Double_t *refindexMvdPixel;
	Double_t *refindexMvdStrip;
	Short_t  *resultFitSZagain;
	Short_t  *SciTilCommonList; // equivalent to a matrix
			// [nTotalCandidates][MAXSCITILHITSINTRACK];
	Short_t  *SciTilSpuriList; // equivalent to a matrix
			//  [nTotalCandidates][MAXSCITILHITSINTRACK];
	Short_t  *SkewCommonList;
	Short_t  *SkewSpuriList;
	bool     *SttSZfit;
	Double_t *XMvdPixel;
	Double_t *XMvdStrip;
	Double_t *XSciTilCenter;
	Double_t *YMvdPixel;
	Double_t *YMvdStrip;
	Double_t *YSciTilCenter;
	Double_t *ZMvdPixel;
	Double_t *ZMvdStrip;
	Double_t *ZSciTilCenter;
	};


class PndTrkComparisonMCtruth
{


 public:

 /** Default constructor **/
 PndTrkComparisonMCtruth(){};
 /** Destructor **/
 ~PndTrkComparisonMCtruth(){};

 void AssociateFoundTrackstoMCquater(
	Double_t BFIELD,
	Double_t CVEL,
	Short_t *daTrackFoundaTrackMC,
	TClonesArray *fMCTrackArray,
	Int_t   *FromPixeltoMCTrack,
	Int_t   *FromStriptoMCTrack,
	int *FromSciTiltoMCTrackList,	// it is an array of
		//  dimension   nSciTilHits*nMCTracks, equivalent to a matrix
		//  of dimension  [nSciTilHits][nMCTracks].
	bool *keepit,
	Double_t info[][7],
	Short_t *ListSttParHitsinTrack,
	Short_t *ListMvdPixelHitsinTrack,
	Short_t *ListSciTilHitsinTrack,
	Short_t *ListSttSkewHitsinTrack,
	Short_t *ListMvdStripHitsinTrack,
	int MAXMVDPIXELHITSINTRACK,
	int MAXMVDSTRIPHITSINTRACK,
	int MAXSCITILHITSINTRACK,
	int MAXSTTHITSINTRACK,
	int *nFromSciTiltoMCTrack,
	Short_t *nSttParHitsinTrack,
	int nMCTracks,
	Short_t *nMvdPixelHitsinTrack,
	Short_t nSciTilHits,
	Short_t *nSciTilHitsinTrack, // equivalen to a matrix of
		// dimension [MAXTRACKSPEREVENT][MAXSCITILHITSINTRACK]
	Short_t *nSttSkewHitsinTrack,
	Short_t *nMvdStripHitsinTrack,
	Short_t  nTracksFoundSoFar,
	Double_t *Ox,
	Double_t *Oy,
	Double_t *R,
	Double_t *X1,
	Double_t *Y1,
	Double_t *X2,
	Double_t *Y2,
	Double_t *X3,
	Double_t *Y3
					);

 int ComparisonwithMC(
	PndTrkComparisonMCtruth_io_Data ioData
		);

 Double_t FindDistance(
	Double_t Oxx,	//  center from wich distance is calculated
	Double_t Oyy,	//  center from wich distance is calculated
	Double_t Rr,
	Double_t tanlow,
	Double_t tanmid,
	Double_t tanup,
	Double_t alfa,	//  intersection circumference parameter
	Double_t beta,	//  intersection circumference parameter
	Double_t gamma	//  intersection circumference parameter
			);
void getMCInfo(
	Double_t BFIELD,
	Double_t CVEL,
	Double_t * Cx,
	Double_t * Cy,
	TClonesArray *fMCTrackArray,
	Int_t MCTrack,
	Double_t * Rr
	);

void MvdMatchedSpurioustoTrackCand(
	Short_t *daTrackFoundaTrackMC,	// input
	Int_t *FromPixeltoMCTrack,	// input
	Int_t *FromStriptoMCTrack,	// input
	bool *keepit,			// input
	Short_t *ListMvdPixelHitsinTrack,// input
	Short_t *ListMvdStripHitsinTrack,// input
	int MAXMVDPIXELHITSINTRACK,
	int MAXMVDSTRIPHITSINTRACK,
	Short_t nMvdPixelHit,
	Short_t nMvdStripHit,
	Short_t *nMvdPixelHitsinTrack,	// input
	Short_t *nMvdStripHitsinTrack,	// input
	Short_t nSttTrackCand,		// input

	Short_t *nMvdPixelCommon,		// output
	Short_t *MvdPixelCommonList,		// output
	Short_t *nMvdPixelSpuriinTrack,	// output
	Short_t *MvdPixelSpuriList,		// output
	Short_t *nMCMvdPixelAlone,		// output
	Short_t *MCMvdPixelAloneList,		// output

	Short_t *nMvdStripCommon,		// output
	Short_t *MvdStripCommonList,		// output
	Short_t *nMvdStripSpuriinTrack,	// output
	Short_t *MvdStripSpuriList,		// output
	Short_t *nMCMvdStripAlone,		// output
	Short_t *MCMvdStripAloneList		// output
				);

void MvdMatchtoMC(
	Double_t ERRORSQPIXEL,
	Double_t ERRORSQSTRIP,
	TClonesArray *fMvdMCPointArray,
	Short_t nMvdMCPoint,
	int istampa,
	int IVOLTE,
	Short_t nMvdPixelHit,
	Short_t nMvdStripHit,
	Double_t *refindexMvdPixel,
	Double_t *refindexMvdStrip,
	Double_t *XMvdPixel,
	Double_t *XMvdStrip,
	Double_t *YMvdPixel,
	Double_t *YMvdStrip,
	Double_t *ZMvdPixel,
	Double_t *ZMvdStrip,
	Int_t *FromPixeltoMCTrack,	// output
	Int_t *FromStriptoMCTrack	// output
	);

 void SciTilMatchtoMC(
	Double_t BFIELD,
	Double_t CVEL,
	Double_t DIMENSIONSCITIL,
	TClonesArray *fMCTrackArray,
	int *FromSciTiltoMCTrackList,	// output; it is an array of
	//  dimension   nSciTilHits*nMCTracks, equivalent to a matrix
	//  of dimension  [nSciTilHits][nMCTracks].
	TClonesArray *fSciTHitArray,
	Short_t fSciTilMaxNumber,
	TClonesArray *fSciTPointArray,
	int *nFromSciTiltoMCTrack, // output; nFromSciTiltoMCTrack[nSciTilHits].
	Short_t *nHitsInSciTile,
	int nMCTracks,
	Short_t nSciTilHits,
	Short_t *OriginalSciTilList,
	Double_t *XSciTilCenter,
	Double_t *YSciTilCenter,
	Double_t *ZSciTilCenter
		);

 void SciTilMatchedSpurioustoTrackCand(
	Short_t *daTrackFoundaTrackMC,	// input
	int *FromSciTiltoMCTrackList,	// input; equivalent to a matrix
					//  of dimension  [nSciTilHits][nMCTracks]
	bool *keepit,			// input
	Short_t *ListSciTilHitsinTrack,	// input; equivalent to a matrix of dimension
					// [MAXTRACKSPEREVENT][MAXSCITILHITSINTRACK]
	int MAXSCITILHITSINTRACK,	// input
	Short_t *MCSciTilAloneList,	// output; equivalent to a matrix of dimension
					// [MAXTRACKSPEREVENT][MAXSCITILHITSINTRACK]
	int *nFromSciTiltoMCTrack,	// input
	Short_t *nMCSciTilAlone,	// output
	int nMCTracks,			// input
	Short_t nSciTilHits,		// input
	Short_t  *nSciTilHitsinTrack,	// input
	Short_t *nSciTilCommon,		// output
	Short_t *nSciTilSpuriinTrack,	// output
	Short_t nSttTrackCand,		// input
	Short_t *SciTilCommonList,	// output; equivalent to a matrix of dimension
					// [MAXTRACKSPEREVENT][MAXSCITILHITSINTRACK]
	Short_t *SciTilSpuriList	// output; equivalent to a matrix of dimension
					// [MAXTRACKSPEREVENT][MAXSCITILHITSINTRACK].
			);

 void stampaMCTracks(
	Double_t BFIELD,
	Double_t CVEL,
	TClonesArray* fMCTrackArray,
	int nMCTracks
			);

void SttMatchedSpurious(
	Short_t *daTrackFoundaTrackMC,
	bool *InclusionListStt,
	Double_t info[][7],
	bool *keepit,
	int MAXSTTHITS,
	int MAXSTTHITSINTRACK,
	int MAXTRACKSPEREVENT,
	Short_t *ListSttParHitsinTrack, // from PR
	Short_t *ListSttSkewHitsinTrack, // from PR
	Short_t *MCParalAloneList,
	Short_t *MCSkewAloneList,
	Short_t *nHitsInMCTrack,
	Short_t *nSttParHitsinTrack, // n. hits PARALLEL, from PR
	Short_t *nMCParalAlone,
	Short_t *nMCSkewAlone,
	Short_t *nParalCommon,
	Short_t *nSkewCommon,
	Short_t *nSkewHitsInMCTrack,
	Short_t *nSttSkewHitsinTrack, // n. hits skew, from PR
	Short_t *nSpuriParinTrack,
	Short_t *nSpuriSkewinTrack,
	Short_t  nSttHits,
	Short_t  nTracksFoundSoFar, // those found by PR
	Short_t *ParalCommonList,
	Short_t *ParSpuriList,
	Short_t *SkewCommonList,
	Short_t *SkewSpuriList
	);


  ClassDef(PndTrkComparisonMCtruth,1);

};

#endif
