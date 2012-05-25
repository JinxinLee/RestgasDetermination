#ifndef PndTrkCTFindTrackInXY_H
#define PndTrkCTFindTrackInXY_H 1
#include <vector>

// Root includes
#include "TROOT.h"
 struct FindTrackInXYProjection_InputData {
	Double_t* ALFA;
	Double_t apotemamaxskewstraw;
	Double_t* BETA;
	Short_t* Charge;
	Double_t deltanr;
	Double_t dimensionscitil;
	UShort_t *FiConformalIndex;
	Double_t *Fi_final_helix_referenceframe;
	Double_t *Fi_initial_helix_referenceframe;
	Double_t *Fi_low_limit;
	Double_t *Fi_up_limit;
	Double_t* GAMMA;
	void* HitsinBoxConf;
	Short_t iHit;	// seed hit; it is negative for SciTil Hits.
	std::vector <bool>* InclusionListStt;
	std::vector <bool>* InclusionListSciTil;
	Double_t (*info)[7];
	Double_t (*infoparalConformal)[5];
	UShort_t* ListHitsinTrack;
	UShort_t* ListSciTilHitsinTrack;
	std::vector <UShort_t>* ListSttParHits;
	UShort_t maxhitsinfit;
	UShort_t maxscitilhitsintrack;
	UShort_t maxstthits;
	UShort_t maxstthitsintrack;
	UShort_t minimumhitspertrack;
	UShort_t minouterhitspertrack;
	void* nBoxConf;
	UShort_t nFicell; // Fi cell of the seed hit;
	UShort_t nfidivconformal;
	UShort_t *nHitsinTrack;
	Short_t nRcell;  // R cell of the seed hit;
	UShort_t nrdivconformal;
	UShort_t nSciTilHits;
	UShort_t* nSciTilHitsinTrack;
	UInt_t nsttparhit;
	Double_t* Oxx;
	Double_t* Oyy;
	void * posizSciT;
	Double_t posizSciTilx;
	Double_t posizSciTily;
	std::vector <Double_t>* radiaConf;
	UShort_t *RConformalIndex;
	Double_t* Rr;
	Double_t rstrawdetectormax;
	Double_t rstrawdetectormin;
	Double_t strawradius;
//	Double_t* S;
	Double_t* S_SciTilHitsinTrack;
	Double_t *trajectory_vertex;
	bool* TypeConf;
	Double_t *U;
	Double_t *V;
	bool YesSciTil;
	};


class PndTrkCTFindTrackInXY : public TObject
{


 public:

  /** Default constructor **/
  PndTrkCTFindTrackInXY(){};


  /** Destructor **/
  ~PndTrkCTFindTrackInXY(){};




//----------------

  bool  AcceptHitsConformal(
	Double_t  distance,
	Double_t  DriftConfR, //drift radius in conformal space
	Double_t  StrawConfR  // straw radius in conformal space
	);



  UShort_t AssociateSciTilHit(
	Double_t dimensionscitil,
	Double_t *esse, // output, list of  S of the SciTil hits associated. 
	std::vector <bool>* InclusionListSciTil,
	UShort_t *List, // output, list of SciTil hits associated (max. 2);
	UShort_t maxscitilhitsintrack,
	UShort_t nSciTilHits,
	Double_t Oxx,
	Double_t Oyy,
	Double_t posizSciTil[][3],
	Double_t Rr
	);



  void FindCharge(
	Double_t oX,
	Double_t oY,
	UShort_t nHits,
	Double_t *X,
	Double_t *Y,
	Short_t  * Charge
	);



  bool FindTrackInXYProjection(
	struct FindTrackInXYProjection_InputData* inputdata
	);


  Short_t FindTrackPatterninBoxConformal(
	UShort_t *FiConformalIndex,
	void* HitsinBoxConf,
	Short_t ihit, // seed hit;
	std::vector <bool> *InclusionListStt,
	Double_t info[][7],
	UShort_t* ListHitsinTrack,
	std::vector <UShort_t>* ListSttParHits,
	UShort_t maxstthitsintracks,
	UShort_t MAXSTTHITS,
	UShort_t MINIMUMHITSPERTRACK,
	void* nBoxConf,
	UShort_t nfidivconformal,
	UShort_t nFicell, // Fi cell of the seed hit;
	UShort_t NFiCELLDISTANCE,
	UShort_t Nparal,
	Short_t nRcell, // R cell of the seed hit;
	// can be negative beacuse of SciTil hits;
	UShort_t NRCELLDISTANCE,
	UShort_t *RConformalIndex,
	UShort_t nrdivconformal
	);

  Short_t FindTrackPatterninBoxConformalSpecial(
	UShort_t *FiConformalIndex,
	void* HitsinBoxConf,
	std::vector <bool> *InclusionListStt,
	Double_t info[][7],
	UShort_t iSeed,
	UShort_t *ListHitsinTrackinWhichToSearch,
	std::vector <UShort_t>* ListSttParHits,
	UShort_t MAXSTTHITS,
	UShort_t MINIMUMHITSPERTRACK,
	void* nBoxConf,
	UShort_t NFiCELLDISTANCE,
	UShort_t nfidivconformal,
	UShort_t Nparal,
	UShort_t NparallelToSearch,
	UShort_t NRCELLDISTANCE,
	UShort_t nrdivconformal,
	UShort_t *OutputListHitsinTrack,
	UShort_t *RConformalIndex

	);



  Short_t FindTrackStrictCollection(
	UShort_t *FiConformalIndex,
	std::vector <bool> *InclusionListStt,
	//  seed track (original notation) as far as the Fi angle is concerned
	UShort_t iSeed,
	UShort_t *ListHitsinTrackinWhichToSearch,
	UShort_t MAXSTTHITS,
	UShort_t NFiCELLDISTANCE,
	UShort_t nfidivconformal,
	//  n. of hits to search in ListHitsinTrackinWhichToSearch
	UShort_t NParallelToSearch,
	UShort_t  *OutputListHitsinTrack
	);


  void  OrderingParallel(
	Short_t  Charge,
	Double_t *Fi_final_helix_referenceframe,
	Double_t *Fi_initial_helix_referenceframe,
	Double_t info[][7],
	UShort_t *ListParallelHits,
	UShort_t nParallelHits,
	Double_t oX,
	Double_t oY,
	Double_t *U,
	Double_t *V
	);

  UShort_t TrkAssociatedParallelHitsToHelixQuater(
	UShort_t *auxListHitsinTrack,
	Double_t deltanr,
	UShort_t *FiConformalIndex,
	void* HitsinBoxConf,
	std::vector <bool>* InclusionListStt,
	Double_t info[][7],
	Double_t infoparalConformal[][5],
	UShort_t *ListHitsinTrack,
	Double_t m,
	UShort_t MAXSTTHITS,
	void* nBoxConf,
	UShort_t nfidivconformal,
	UShort_t nHitsinTrack,
	UInt_t NhitsParallel,
	UShort_t nrdivconformal,
	Double_t Oxx,
	Double_t Oyy,
	Double_t q,
	std::vector <Double_t>* radiaConf,
	UShort_t *RConformalIndex,
	Double_t Rr,
	Double_t rstrawdetectormin,
	Short_t Status,
	Double_t strawradius
	);



  UShort_t TrkAssociatedParallelHitsToHelix5(
	UShort_t *auxListHitsinTrack,
	std::vector <bool> *InclusionListStt,
	Double_t Fi_low,
	Double_t Fi_up,
	Double_t info[][7],
	std::vector <UShort_t> *ListSttParHits,
	UInt_t NhitsParallel,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t strawradius
	);

  ClassDef(PndTrkCTFindTrackInXY,1);

};

#endif
