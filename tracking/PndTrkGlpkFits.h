#ifndef PndTrkGlpkFits_H
#define PndTrkGlpkFits_H 1

// Root includes
#include "TROOT.h"

class PndTrkGlpkFits : public TObject
{

 public:

  /** Default constructor **/
  PndTrkGlpkFits(){};


  /** Destructor **/
  ~PndTrkGlpkFits(){};




  Short_t FitHelixCylinder(
	UShort_t nHitsinTrack,
	Double_t *Xconformal,
	Double_t *Yconformal,
	Double_t *DriftRadiusconformal,
	Double_t *ErrorDriftRadiusconformal,
	Double_t rotationangle,
	Double_t trajectory_vertex[2],
	UShort_t NMAX,
	Double_t *m,
	Double_t *q,
	Double_t *pAlfa,
	Double_t *pBeta,
	Double_t *pGamma,
	bool *Type,
	int istampa,
	int IVOLTE
	);

  Short_t FitSZspace(
	UShort_t nSkewHitsinTrack,
	Double_t *S,
	Double_t *Z,
	Double_t *DriftRadius,
	Double_t *ErrorDriftRadius,
	Double_t FInot,
	UShort_t NMAX,
	Double_t *emme,
	int IVOLTE
	);



  ClassDef(PndTrkGlpkFits,1);

};

#endif
