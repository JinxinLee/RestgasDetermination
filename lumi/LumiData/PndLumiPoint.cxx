#include "PndLumiPoint.h"
using std::cout;
using std::endl;

PndLumiPoint::PndLumiPoint()
    : FairMCPoint()
{
}

PndLumiPoint:: PndLumiPoint(Int_t trackID, Int_t detID, TVector3 in,
		TVector3 p_in, Double_t tof, Double_t length, Double_t eLoss,
		TVector3 out, TVector3 p_out, TString detname)
    : FairMCPoint(trackID, detID, in, p_in, tof, length, eLoss)
 {
	fPosOut = out;
	fMomOut = p_out;

	fXOut = out.X();
	fYOut = out.Y();
	fZOut = out.Z();

	fDetName = detname;
}

PndLumiPoint::~PndLumiPoint()
{
}

void PndLumiPoint::Print(const Option_t* opt) const
{
    std::cout << "PndLumiPoint: " << endl
	      << " Track index                 : " << fTrackID << endl
	      << " Detector ID                 : " << fDetectorID << endl
	      << " Point coordinates           : (" << fX << ", " << fY << ", " << fZ << ") [cm]" << endl
	      << " Momentum components         : (" << fPx<< ", " << fPy<< ", " << fPz << ") [GeV]" << endl
	      << " Time since event start      : " << fTime << " [ns]" << endl
	      << " Track length since creation : " << fLength << " [cm]" << endl
	      << " Energy loss at this point   : " << fELoss << " [GeV]" << endl;
}

ClassImp(PndLumiPoint)
