#ifndef PndTrkSttConformalFilling_H
#define PndTrkSttConformalFilling_H 1
#include <vector>
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

	UShort_t *FiConformalIndex,
	void* HitsinBoxConf,
	std::vector <bool> *InclusionListStt,
	Double_t infoparalConformal[][5],
	std::vector <UShort_t> *ListSttParHits,
	UShort_t MAXHITSINCELL,
	void* nBoxConf,
	UShort_t NFIDIVCONFORMAL,
	UInt_t Nparal,
	UShort_t NRDIVCONFORMAL,
	std::vector <Double_t> *radiaConf,
	UShort_t *RConformalIndex
	);

  void FromXYtoConformal(
	Double_t trajectory_vertex[2],
	Double_t info[][7],
	std::vector <UShort_t> *ListSttParHits,
	UInt_t Nparal,
	Double_t infoparalConformal[][5],
	Double_t STRAWRADIUS
	);

  ClassDef(PndTrkSttConformalFilling,1);

};

#endif
