#ifndef PndTrkCleanup_H
#define PndTrkCleanup_H 1

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
	Double_t STRAWRADIUS,
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
	Double_t STRAWRADIUS
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
	Double_t STRAWRADIUS
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
	Double_t SEMILENGTH_STRAIGHT,
	Double_t Start[3],
	Double_t STRAWRADIUS,
	Double_t ZCENTER_STRAIGHT
	);

 bool XYCleanup(
	Double_t info[][7],
	Short_t (*ListParContiguous)[6],
	Short_t *nParContiguous,
	Short_t *StrawCode,
	Short_t *StrawCode2,
	Short_t *TubeID,
	Short_t *ListHits,
	Short_t nHits,
	Double_t RStrawDetInnerParMax
		);


  ClassDef(PndTrkCleanup,1);

};

#endif
