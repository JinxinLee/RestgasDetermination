#ifndef PNDLUMIHIT_H
#define PNDLUMIHIT_H 1


#include "TVector3.h"
#include "TString.h"
#include "FairHit.h"

#include "PndLumiStrip.h"
#include "PndDetectorList.h"

class PndLumiHit : public FairHit
{

public:

	PndLumiHit();

	/** Standard constructor with all parameters
	 *    *@param detID   Detector ID
	 *    *@param detName Detector Name
	 *    *@param pos     Position vector [cm]
	 *    *@param dpos    Position error vector [cm]
	 *    *@param index   Array index of ClusterCand
	 *    *@param charge  Charge measured in hit
	 *    *@param NDigiHits Number of pixels fired for this event
	 *     **/

	PndLumiHit(Int_t detID, TVector3 &hitPos, TVector3 &hitErr, Int_t mcindex, TString &detname,
			Double_t meancharge, Int_t clindex, Int_t NDigiHits);

	virtual ~PndLumiHit();

	/** Accessors **/

	void SetDetName(TString name)  { fDetName   = name;}
	void SetCharge(Double_t charge){ fQ  = charge;}
	void SetNDigiHits(Int_t pixel) { fNDigiHits = pixel;}
	void SetClusterIndex(Int_t id) { fClusterIndex = id;}

	TString GetDetName() const { return fDetName;}
	Double_t GetCharge() const { return fQ;}


	Int_t GetNDigiHits() const { return fNDigiHits;}
	TVector3 GetPosition() const { return TVector3(fX,fY,fZ);}
	Int_t GetClusterIndex() const { return fClusterIndex;}
	Int_t GetTopIndex() const { return GetClusterIndex();}
	Int_t GetMCIndex() const {return fMCIndex;}

	/** Screen output **/
//	virtual void Print(const Option_t* opt = 0) const;

private:

	Int_t fMCIndex;

	TString fDetName;  // Detector name
	TString fDetNameBot;  // Detector name
	Double_t fQ; /// deposited Charge
	Int_t fNDigiHits; /// number of fired Digis for this hit,
	Int_t fClusterIndex; /// top/pixel cluster index

	ClassDef(PndLumiHit,4);
};
#endif

