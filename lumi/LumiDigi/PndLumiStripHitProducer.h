#ifndef PNDLUMISTRIPHITPRODUCER_H_
#define PNDLUMISTRIPHITPRODUCER_H_

#include "FairTask.h"
#include "FairRootManager.h"
#include "FairGeoVector.h"
#include "FairGeoNode.h"
#include "FairGeoTransform.h"

#include "PndLumiPoint.h"
#include "PndLumiStripHit.h"
#include "PndLumiStrip.h"
#include "PndLumiCalcStrip.h"
#include "PndLumiTransposition.h"

#include "TGeoBBox.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TClonesArray.h"
#include "TVector3.h"

#include <cmath>
#include <vector>
#include "TVector2.h"
#include "TString.h"


using std::cout;
using std::endl;

class PndLumiStripHitProducer : public FairTask
{
public:

	PndLumiStripHitProducer();

	PndLumiStripHitProducer(Double_t pitch, Double_t orient, Double_t width,
			Double_t length, Double_t threshold, Double_t noise, Int_t verbose);

	~PndLumiStripHitProducer();

	virtual InitStatus Init();

	virtual void Exec(Option_t* opt);

	void Print() const;

private:
	/** Pointer to input array of PndLumiPoints */
	TClonesArray* fLumiPointCollection;

	/** Pointer to output array of PndLumiStripHits */
	TClonesArray* fLumiStripHitCollection;

	Int_t fVerboseLevel;

    Double_t fPitch;
    Double_t fOrient;

    Double_t fSensorWidth; // sensor width
    Double_t fSensorLength; // sensor length

    Double_t fThreshold;
    Double_t fNoise;

    ClassDef(PndLumiStripHitProducer,1);
};

#endif /*PNDLUMISTRIPHITPRODUCER_H_*/
