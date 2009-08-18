#ifndef PNDLUMIDIGIPRODUCER_H_
#define PNDLUMIDIGIPRODUCER_H_

#include "FairTask.h"
#include "FairRootManager.h"

#include "PndLumiPoint.h"
#include "PndLumiDigi.h"
#include "PndLumiStrip.h"
#include "PndLumiCalcStrip.h"
#include "PndLumiCalcStripDigi.h"
#include "PndLumiTransposition.h"

#include "TMath.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include <cmath>
#include <vector>
#include <TVector2.h>

using std::cout;
using std::endl;

class PndLumiDigiProducer : public FairTask
{
public:
	PndLumiDigiProducer();
	PndLumiDigiProducer(Double_t Z0, Double_t pitch, Double_t of, Double_t ob, Double_t width,
			Double_t length, Double_t detdistance, Double_t distplan, Double_t threshold,
			Double_t noise,	Double_t side, Double_t sigma, Int_t verbose);
	~PndLumiDigiProducer();

	virtual InitStatus Init();
	virtual void Exec(Option_t* opt);

	void Print() const;

private:

	/**Put all strips hit in order to make the identification of left strip
	 * and right strip more easier
	 */
	std::map<Int_t,PndLumiStrip> GetClusters(std::vector<PndLumiStrip> strip);

	/**To identify the two strips which collect the highest Energy deposited
	 * in the sensor : these two strips are identified as left and right
	 */
	std::map<Int_t,Double_t> GetLeftAndRight(std::map<Int_t,PndLumiStrip> clust);


	/** Pointer to input array of PndLumiPoints */
	TClonesArray* fLumiPointCollection;

	/** Pointer to output array of PndLumiStripHits */
	TClonesArray* fLumiDigiCollection;

	Int_t fVerboseLevel;
    Double_t fZ0;

    Double_t fPitch;
    Double_t fOrient_front, fOrient_back;

    Double_t fSensorWidth;
    Double_t fSensorLength;
    Double_t fRadialDistance;
    Double_t fDistancePlan;

    Double_t fThreshold;
    Double_t fNoise;
    Double_t fSide;

    Double_t fSigma;// charge diffusion

    ClassDef(PndLumiDigiProducer,1);
};
#endif /*PNDLUMISTRIPDIGIPRODUCER_H_*/
