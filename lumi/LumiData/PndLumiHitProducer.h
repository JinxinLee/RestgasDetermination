#ifndef PNDLUMIHITPRODUCER_H
#define PNDLUMIHITPRODUCER_H

#include "FairTask.h"
#include "FairRootManager.h"

#include "PndLumiPoint.h"
#include "PndLumiHit.h"

#include "TVector3.h"
#include "TRandom.h"
#include "TClonesArray.h"


#include "TRandom3.h"



class PndLumiHitProducer : public FairTask
{
public:
	/** Default constructor */
	PndLumiHitProducer();

	/** Standard constructor */
	PndLumiHitProducer(Double_t dpos, Double_t efficiency, Double_t threshold,
			Int_t verbose);
	/** Destructor */
	~PndLumiHitProducer();

	/** Virtual method Init */
	virtual InitStatus Init();

	/** Virtual method Exec */
	virtual void Exec(Option_t* opt);

	/** Output to screen */
	void Print() const;

private:
	/** Pointer to input array of PndLumiPoints */
	TClonesArray* fLumiPointCollection;

	/** Pointer to output array of PndLumiHits */
	TClonesArray* fLumiHitCollection;

	Double_t fdPos;
	Double_t fEfficiency;
	Double_t fThreshold;
	Int_t fVerboseLevel;

	TRandom3* fRNG;

    ClassDef(PndLumiHitProducer,1);
};

#endif

