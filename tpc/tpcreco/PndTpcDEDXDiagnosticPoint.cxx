#include "PndTpcDEDXDiagnosticPoint.h"

ClassImp(PndTpcDEDXDiagnosticPoint);

PndTpcDEDXDiagnosticPoint::PndTpcDEDXDiagnosticPoint(Double_t X, Double_t Y, Double_t Z, Double_t Amp, int Number) :
fX(X), fY(Y), fZ(Z), fAmp(Amp), fNumber(Number)
{
}

void PndTpcDEDXDiagnosticPoint::SetCoordinates(Double_t x, Double_t y, Double_t z, Double_t amp)
{
	fX=x;
	fY=y;
	fZ=z;
	fAmp=amp;
}

