/*
 * PndLumiCalcStripDigi.h
 *
 *    Author: tsito
 *
 *  This computes all the strips fired by the track and collects them to a vector <PndLumiStrip>
 *
 */

#ifndef PNDLUMICALCSTRIPDIGI_H_
#define PNDLUMICALCSTRIPDIGI_H_


#include "PndLumiStrip.h"
#include "PndLumiDigiPara.h"

#include "FairGeoVector.h"
#include "PndDetectorList.h"

#include "TObject.h"
#include "TMath.h"
#include <TRandom3.h>
#include "TVector2.h"

#include <cmath>
#include <map>

using std::cout;
using std::endl;

extern TRandom3* fRND;
//TRandom3 *fRNG = new TRandom3();

class PndLumiCalcStripDigi
{
public:

	/** Default constructor */
	PndLumiCalcStripDigi();


	/** Constructor */
	PndLumiCalcStripDigi(const PndLumiDigiPara *digipar, SensorSide side=kTOP);


	/** Destructor */
	~PndLumiCalcStripDigi();


	/** Collect Strips Fired by MCTrack and Share Charge Among These Strips
	 * 	According To The Fraction of Path Inside Them Respectively
	 */
	std::vector<PndLumiStrip> GetStrips(FairGeoVector in,
			FairGeoVector out, Double_t eLoss);


	/** Compute StripID from a Hit : This corresponds to the y-position
	 * of the hit in the strip orientation coordinate system
	 */
	Double_t CalcStripFromHit(Double_t x, Double_t y);


	void Print() const;

private:

	/** Variables declarations */
	Double_t fPitch;       // pitch of the strip
	Double_t fOrient;      // angle which defines the orientation of the strips in one defined side
	Double_t fThreshold;   // threshold
	Double_t fNoise;	   // noise
	Double_t fSigma;	   // width of the Gaussian distribution applied on the charge
	TVector2 fStripZeroId; // position of the strip with ID = 0 in the sensor plane
	//TRandom3 *fRNG;

	bool AboveThreshold(Double_t charge);


	/** Add Noise According a Random Gaussian Generator Number
	 *  to the Charge at each Strip
	 */
	Double_t AddNoise(Double_t charge);


	/** e-h diffusion : Gaussian Smearing charge at every
	 *  Point along the Path inside each Strip
	 */
	Double_t getchargefraction(TVector2 in, TVector2 out, Int_t stripId, Double_t eloss);


	ClassDef(PndLumiCalcStripDigi,1);

};
#endif /*PNDLUMICALCSTRIPCLUSTER_H_*/
