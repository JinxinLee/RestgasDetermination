//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//   
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Viola Michael    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef PNDTPCDEDXDIAGNOSTICPOINT_HH
#define PNDTPCDEDXDIAGNOSTICPOINT_HH

// Base Class Headers ----------------
#include "TObject.h"


class PndTpcDEDXDiagnosticPoint : public TObject
{
public:
	PndTpcDEDXDiagnosticPoint():fX(0.),fY(0.),fZ(0.),fAmp(0.),fNumber(0)	{}
	PndTpcDEDXDiagnosticPoint(Double_t X, Double_t Y, Double_t Z, Double_t Amp, int Number);
	virtual ~PndTpcDEDXDiagnosticPoint()	{}
	void SetCoordinates(Double_t x, Double_t y, Double_t z, Double_t amp);
	void SetHitNumber(int n)	{ fNumber=n; }
	Double_t GetX() const { return fX;	} 
	Double_t GetY() const { return fY;	}	
	Double_t GetZ() const { return fZ;	} 	
private:
	Double_t fX;
	Double_t fY;
	Double_t fZ;
	Double_t fAmp;
	int fNumber;	//drawing clusters one by one might be nice
public:
  	ClassDef(PndTpcDEDXDiagnosticPoint,1)
};

#endif
