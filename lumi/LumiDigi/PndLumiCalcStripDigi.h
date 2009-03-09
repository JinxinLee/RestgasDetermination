//**********************************************************
//
// This class returns to vectors PndLumiStrip (stripID ,
// path inside , charge collected).
//
// First, total Landau deposited charge is shared among
// all the strips fired according to the path that they
// contain respectively.
//
// Then, these charge are smeared by a Gaus at every point
// along the whole path for strip multiplicity 1 and
// at every point along the path at the edges (head and
// tail )for strip multiplicity more or equal to 2
//
//*********************************************************

#ifndef PNDLUMICALCSTRIPDIGI_H_
#define PNDLUMICALCSTRIPDIGI_H_


#include "PndLumiStrip.h"

#include "FairGeoVector.h"
#include "PndLumiCalcStrip.h"
#include "TMath.h"
#include <cmath>
#include <TRandom3.h>
#include "TVector2.h"
#include <map>
using std::cout;
using std::endl;

extern TRandom3* fRND;
//TRandom3 *fRND = new TRandom3();
class PndLumiCalcStripDigi
{
public:

	/** Default constructor */
	PndLumiCalcStripDigi();

	/** Constructor */
	PndLumiCalcStripDigi(Double_t pitch, Double_t orient,
			Double_t width, Double_t length, Double_t threshold,
			Double_t noise, Double_t sigma,	TVector2 stripzeroId);

	/** Destructor */
	~PndLumiCalcStripDigi();

	/** Collect Strips Fired by MCTrack and Share Charge Among These Strips
	 * 	According To The Fraction of Path Inside Them Respectively
	 */
	std::vector<PndLumiStrip> GetStrips(FairGeoVector in,
			FairGeoVector out, Double_t eLoss);

	/** Apply a Gaussian Smearing To The Charge Collected At the Head
	 * and The Tail of The Strips Cluster From the GetStrips-Method
	 *  and Collect Them
	 */
	std::vector<PndLumiStrip> GetStripsDigi(FairGeoVector in,
			FairGeoVector out, Double_t eLoss);

	std::map<Int_t,PndLumiStrip> GetClusters(std::vector<PndLumiStrip>
	strip);

	/** Compute StripID from a Hit : This corresponds to the y-position
	 * of the hit in the strip orientation coordinate system
	 */
	Double_t CalcStripFromHit(Double_t x, Double_t y);


private:
	/** Variables declarations */
	Double_t fPitch;       // pitch of the strip
	Double_t fOrient;      // angle orientation of the strips with respect of the global coord syst
	Double_t fWidth;       // width of the rectangular sensor
	Double_t fLength;      // length of the rectangular sensor
	Double_t fThreshold;   // threshold
	Double_t fNoise;	   // noise
	Double_t fSigma;	   // width of the Gaussian distribution applied on the charge
	TVector2 fStripZeroId; // position of the strip with ID = 0
	//TRandom3 *fRNG; // random number generator (needed for the Gaus smearing of the charge with the noise)

	bool AboveThreshold(Double_t charge);

	/** Add Noise According a Random Gaussian Generator Number
	 *  to the Charge at each Strip
	 */
	Double_t AddNoise(Double_t charge);

	/** Gaussian Smearing Method for Landau Charge at every
	 *  Point along the Path inside each Strip
	 */
	Double_t ChargeDiffusion(Double_t x, Double_t y,
			Double_t path, Double_t dir, Double_t eLoss );

	ClassDef(PndLumiCalcStripDigi,1);

};
#endif /*PNDLUMICALCSTRIPCLUSTER_H_*/
