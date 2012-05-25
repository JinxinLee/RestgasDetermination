#ifndef PndTrkSttConformalFilling_H
#define PndTrkSttConformalFilling_H 1

// Root includes
#include "TROOT.h"

class PndTrkSttConformalFilling : public TObject
{

 public:

  /** Default constructor **/
  PndTrkSttConformalFilling(){};


  /** Destructor **/
  ~PndTrkSttConformalFilling(){};




  void BoxConformalFilling(

	Short_t *FiConformalIndex,
	void* HitsinBoxConf,
	bool *InclusionListStt,
	Double_t infoparalConformal[][5],
	Short_t *ListSttParHits,
	Short_t MAXHITSINCELL,
	void* nBoxConf,
	Short_t NFIDIVCONFORMAL,
	Int_t Nparal,
	Short_t NRDIVCONFORMAL,
	Double_t *radiaConf,
	Short_t *RConformalIndex
	);

  void FromXYtoConformal(
	Double_t trajectory_vertex[2],
	Double_t info[][7],
	Short_t *ListSttParHits,
	Int_t Nparal,
	Double_t infoparalConformal[][5],
	Double_t STRAWRADIUS
	);

  ClassDef(PndTrkSttConformalFilling,1);

};

#endif
