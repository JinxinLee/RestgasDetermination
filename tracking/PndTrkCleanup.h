#ifndef PndTrkCleanup_H
#define PndTrkCleanup_H 1

#include "PndTrkCTGeometryCalculations.h"
#include "PndTrkConstants.h"

// Root includes
#include "TROOT.h"


class PndTrkCleanup : public TObject
{


 public:


 /** Default constructor **/
 PndTrkCleanup(){};
 /** Destructor **/
 ~PndTrkCleanup(){};


 bool BadTrack_ParStt(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t Stawradius,
	Short_t Charge,
	Double_t Xcross[2],  // Xcross[0]=point of entrance;
				//  Xcross[1]=point of exit.
	Double_t Ycross[2],
	Short_t nHits,
	Short_t* ListHits,
	Double_t info[][7],
	int istampa,
	Double_t cut,
	Short_t maxnum,
	Short_t islack// uncertainty allowed as far as
		// the n. of hits that should be present in a given section of the Stt track.
	);


 bool GoodTrack(
		Double_t info[][7],		// input
		bool farthest_hit_is_boundary,	// input
		Double_t Ox,			// input; center of the current track;
		Double_t Oy,			// input; center of the current track;
		Double_t R,			// input; Radius of the current track;
		Short_t Charge,			// input; Charge of the current track;
		Short_t nHits,			// input
		Short_t* ListHits,		// input
		Short_t *StrawCode, // first straw boundary code (a straw can belong to 2 boundaries);
		Short_t *StrawCode2,
		Short_t *TubeID,		// input
		Short_t *nParContiguous,	// input
		Short_t ListParContiguous[][6],	// input
		Double_t *xTube,		// input
		Double_t *yTube,		// input
		Double_t *zTube,		// input
		Double_t *xxyyTube,		// input

		Short_t & holes			// input and output

		);



 Short_t Is_Contained_in_Mvd_Vertical_Strip(
	Short_t iLayer,	// index of the Mvd Disk Layer under scrutiny here;
 	Short_t nXlow, //  index of the strip containing Xlow;
	Short_t nXup, //  index of the strip containing Xlow;
	Double_t tmpYlow, // Ylow (abs of it if it is the case);
	Double_t tmpYup // Yup (abs of it if it is the case);
					);

 bool MvdCleanup(
	Double_t Ox,
	Double_t Oy,
	Double_t R,
	Double_t fi0,
	Double_t kappa,
	Double_t charge,
	Double_t* XMvdPixel,  // list of the X positions of ALL Mvd hits of the event;
	Double_t* XMvdStrip,  // list of the X positions of ALL Mvd hits of the event;
	Double_t* YMvdPixel,  // list of the Y positions of ALL Mvd hits of the event;
	Double_t* YMvdStrip,  // list of the Y positions of ALL Mvd hits of the event;
	Double_t* ZMvdPixel,  // list of the Z positions of ALL Mvd hits of the event;
	Double_t* ZMvdStrip,  // list of the Z positions of ALL Mvd hits of the event;
	Short_t nPixelHitsinTrack,  // number of Mvd Pixel hits in this track;
	Short_t * ListMvdPixelHitsinTrack,	
	Short_t nStripHitsinTrack,  // number of Mvd Strip hits in this track;
	Short_t * ListMvdStripHitsinTrack,
	Double_t extra_distance,
	Double_t extra_distance_Z,
	PndTrkCTGeometryCalculations* GeomCalculator
		);


 bool MvdCleanup_prova(
	Double_t Ox,
	Double_t Oy,
	Double_t R,
	Double_t fi0,
	Double_t kappa,
	Double_t charge,
	Double_t semiverticalgap,
	Short_t nMvdHits,
	PndTrkCTGeometryCalculations* GeomCalculator	
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


void SeparateInnerOuterRightLeftAxialStt(

	// input
	Double_t info[][7],
	Short_t *ListHits,
	Short_t nHits,
	Double_t RStrawDetInnerParMax,

	// output

	Short_t *ListInnerHitsLeft,
	Short_t *ListInnerHitsRight,
	Short_t *ListOuterHitsLeft,
	Short_t *ListOuterHitsRight,
	Short_t *nInnerHitsLeft,
	Short_t *nInnerHitsRight,
	Short_t *nOuterHitsLeft,
	Short_t *nOuterHitsRight
	);


 bool SttParalCleanup(
	Double_t ApotemaInnerParMax,
	Double_t ApotemaMinOuterPar,
	Short_t  Charge,
	Double_t FI0,
	Double_t FiLimitAdmissible,
	Double_t GAP,
	Double_t info[][7],
	int	istampa,
	int	IVOLTE,
	Short_t *Listofhits,
	Short_t nHits,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t RStrawDetMax, // radius of circle encompassing ALL
				// the straw detector;
	Double_t RStrawDetMin,
	Double_t Start[3],
	Double_t Strawradius
 );

bool SttSkewCleanup(
	Double_t ApotemaMaxSkew,
	Double_t ApotemaMinSkew,
	Short_t  Charge,
	Double_t cut, // cut distance (in cm).
	Double_t FI0,
	Double_t FiLimitAdmissible,
	Double_t GAP,
	Double_t info[][7],
	int istampa,
	int IVOLTE,
	Short_t *Listofhits,
	Short_t maxnum, // max number allowed of failures to pass the cut.
	int MAXSTTHITS,
	Short_t nHits,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t RStrawDetMax,
	Double_t *S,
	Double_t Start[3],
	Double_t Strawradius
	);

 bool TrackCleanup(
	Double_t ApotemaMaxInnerPar,
	Double_t ApotemaMaxSkew,
	Double_t ApotemaMinOuterPar,
	Double_t ApotemaMinSkew,
	Double_t *auxS,
	Short_t  Charge,
	Double_t FI0,
	Double_t GAP,
	Double_t info[][7],
	int	istampa,
	int	IVOLTE,
	Double_t KAPPA,
	Short_t *ListHitsPar,
	Short_t *ListHitsSkew,
	int	MAXSTTHITS,
	Short_t nHitsPar,  // n. hits parall Stt
	Short_t nHitsSkew,  // n. hits parall Stt
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t RStrawDetMax,
	Double_t RStrawDetMin,
	Double_t Start[3],
	Double_t Strawradius
	);

 bool XYCleanup(
	// general infos about the axial Straws;
	int istampa,
	Double_t info[][7],
	Short_t (*ListParContiguous)[6],
	Short_t *nParContiguous,
	Short_t *StrawCode,
	Short_t *StrawCode2,
	Short_t *TubeID,
	Double_t *xTube,
	Double_t *yTube,
	Double_t *zTube,
	Double_t *xxyyTube,
	// the following are the info of the track under scrutiny;
	Double_t Ox,
	Double_t Oy,
	Double_t R,
	Short_t Charge,
	Short_t *ListHits,
	Short_t nHits,
	Double_t RStrawDetInnerParMax,
	Short_t nScitilHitsInTrack,	// input, # of SciTil hits in the current track;
	Short_t* ListSciTilHitsinTrack,	// input, list of SciTil hits in the current track;
	Double_t posizSciTil[][3]	// input, info on all the SciTil position;
		);


  ClassDef(PndTrkCleanup,1);

};

#endif
