#ifndef PndTrkChi2Fits_H
#define PndTrkChi2Fits_H 1

// Root includes
#include "TROOT.h"

class PndTrkChi2Fits : public TObject
{

 private:

 int fIcounter;

 

 public:

  /** Default constructor **/
  PndTrkChi2Fits();


  /** Destructor **/
  ~PndTrkChi2Fits(){};


  Short_t FitHelixCylinder(
	Short_t nHitsinTrack,
	Double_t *Xconformal,
	Double_t *Yconformal,
	Double_t *DriftRadiusconformal,
	Double_t *ErrorDriftRadiusconformal,
	Double_t rotationangle,
	Double_t trajectory_vertex[2],
	Short_t NMAX,
	Double_t *m,
	Double_t *q,
	Double_t *pAlfa,
	Double_t *pBeta,
	Double_t *pGamma,
	bool *Type,
	int istampa,
	int IVOLTE
	);



  ClassDef(PndTrkChi2Fits,1);

};

#endif
