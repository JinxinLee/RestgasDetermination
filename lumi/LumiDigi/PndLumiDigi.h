/*
 * PndLumiDigi.h
 *
 *    Author: tsito
 *
 */


#ifndef PNDLUMIDIGI_H_
#define PNDLUMIDIGI_H_

#include "PndDetectorList.h"

#include "PndLumiStrip.h"

#include <cmath>
#include "TMath.h"
#include "TVector3.h"
#include "TObject.h"
//#include "FairHit.h"

using std::cout;
using std::endl;

class PndLumiDigi : public TObject
{
public:

	/** Default constructor*/
	PndLumiDigi();

	/** Constructor */
	PndLumiDigi (Int_t detID, Int_t index, TString detname, Int_t trkId,
			PndLumiStrip strip, SensorSide side);

	/** Destructor */
	virtual ~PndLumiDigi();

	/** Accessing variables methods apart from ones inherited from FairHit*/
	Int_t GetDetId() const {return fDetId;}
	PndLumiStrip GetStrip() const {return fStrip;}
	TString GetDetName() const {return fDetName;}
	SensorSide GetSide() const {return fSide;}
	Int_t GetRefIndex() const {return fIndex;}
	Int_t GetTrackID() const {return fTrkId;}

	void Print(const Option_t* opt) const;

private:

	Int_t fDetId;
	TString fDetName;		// name of the sensor hit
	SensorSide fSide;  		// side hit
	PndLumiStrip fStrip;	// Info of the strips fired
	Int_t fIndex;
	Int_t fTrkId;

	Bool_t fIsActive;

	ClassDef(PndLumiDigi,2);
};
#endif /*PNDLUMIDIGI_H_*/


