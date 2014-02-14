#include "PndFtsHoughTrackCand.h"

#include <iostream>
#include "math.h"

#include "FairRootManager.h"
#include "PndFtsHit.h"
#include "PndTrack.h"
#include "FairTrackParP.h"

ClassImp(PndFtsHoughTrackCand);

PndFtsHoughTrackCand::PndFtsHoughTrackCand() :
														fVerbose(0),

														fZxLineParabola(),

														fZxParabola(),

														fZxParabolaLine(),

														fZyLine(),

														fZLineParabola(0.),
														fZParabolaLine(0.)
{

}

PndFtsHoughTrackCand::~PndFtsHoughTrackCand()
{

}


void PndFtsHoughTrackCand::SetZxFirstLine(const PndFtsHoughTracklet zxLineParabola, const Double_t zLineParabola){
	fZxLineParabola = zxLineParabola;
	fZLineParabola = zLineParabola;
}
void PndFtsHoughTrackCand::SetZxParabola(const PndFtsHoughTracklet zxParabola, const Double_t zLineParabola){
	fZxParabola = zxParabola;
	// warn if line before dipole field and parabola within are not calculated wrt the same z reference value
	if (zLineParabola!=fZLineParabola){
		std::cout << "WARNING from PndFtsHoughTrackCand: First line and parabola were not calculated wrt the same z position! Potentially FATAL ERROR!\n";
	}
}
void PndFtsHoughTrackCand::SetZxSecondLine(const PndFtsHoughTracklet zxParabolaLine, const Double_t zParabolaLine){
	fZxParabolaLine = zxParabolaLine;
	fZParabolaLine = zParabolaLine;
}
void PndFtsHoughTrackCand::SetZyLine(const PndFtsHoughTracklet zyLine){
	fZyLine=zyLine;
}






void PndFtsHoughTrackCand::Print() {
	std::cout << "=========== PndFtsHoughTrackCand::Print() ==========" << std::endl;
	PndTrackCand::Print();
	if (kTRUE==fZxLineParabola.isSet()){
		std::cout << "zx plane\n\n";
		std::cout << "1st line: ";
		fZxLineParabola.Print();
	}
	if (kTRUE==fZxParabola.isSet()){
		std::cout << "parabola: ";
		fZxParabola.Print();
	}
	if (kTRUE==fZxParabolaLine.isSet()){
		std::cout << "2nd line: ";
		fZxParabolaLine.Print();
	}
	if (kTRUE==fZyLine.isSet()){
		std::cout << "zy plane\n\n";
		std::cout << "Line: ";
		fZyLine.Print();
	}
}





// for conversion to PndTrack, compare to PndTools/riemannfit/PndRiemannTrack.cxx, line 1104
/////////////////////////////////////////////////////////////////////////////////////////////
// getNumHits is called getNHits (because my track cand class is derived from PndTrackCand and that's how it's called there)
// I don't need the B field to calculate the parameters
PndTrack PndFtsHoughTrackCand::getPndTrack() {
	FairTrackParP firstPar, lastPar;
	PndTrackCand myCand;
	if (GetNHits() > 0){
		firstPar = getTrackParPForHit(0);
		lastPar = getTrackParPForHit(GetNHits()-1);
	}
	return PndTrack(firstPar, lastPar, myCand);
}

const PndFtsHit* PndFtsHoughTrackCand::getHit(UInt_t index) {
	// Make sure we have a complete track candidate before we try to access any hits
	if (!isComplete()) return 0;
	if (index < GetNHits()){
		TClonesArray *ftsHitArray= (TClonesArray *)FairRootManager::Instance()->GetObject("FTSHit");
		const PndFtsHit *myHit = (PndFtsHit*) ftsHitArray->At(GetSortedHit(index).GetHitId());
		return myHit;
	} else {
		return 0;
	}
}

FairTrackParP PndFtsHoughTrackCand::getTrackParPForHit(UInt_t index) {
	// TODO: Check if all arguments are correct
	//	TVector3 hitPos;
	TVector3 hitPosError;
	TVector3 momError(2, 2, 2); // TODO: Check if that is set correctly for FTS
	TVector3 dj(1,0,0); // TODO: Check if that is set correctly for FTS
	TVector3 dk(0,1,0); // TODO: Check if that is set correctly for FTS
	TVector3 origin(0, 0, 1); // TODO: Check if that is set correctly for FTS

	if (index < GetNHits()){
		// get position of hit with index in track candidate
		const PndFtsHit *myHit = getHit(index);
		if (0==myHit){
			Warning("getTrackParPForHit","Cannot get hit, probably the tracking has not finished or the track contains less hits (index is too large).");
			return FairTrackParP();
		}
		//myHit->Position(hitPos);


		// TODO: Is that how to set the error correctly?
		myHit->PositionError(hitPosError);


		// -----   Constructor with track parameters in LAB -----------------------------------
		// FairTrackParP::FairTrackParP(TVector3 pos, TVector3 Mom, TVector3 posErr, TVector3 MomErr, Int_t Q, TVector3 o, TVector3 dj, TVector3 dk)
		FairTrackParP result(getPositionForHit(index), getPforHit(index), hitPosError, momError, getCharge(), origin, dj, dk);
		return result;
	}
	else {
		return FairTrackParP();
	}
}

TVector3 PndFtsHoughTrackCand::getPforHit(UInt_t index) {
	TVector3 result;

	if (index > GetNHits())
	{
		Warning("getPforHit","Hit index %i is too big for track candidate. Momentum is set to (0,0,0)", index);
		result.SetXYZ(0., 0., 0.);
	}
	else
	{
		const PndFtsHit* myHit = getHit(index);
		// track model is assumed to be line+parabola+line in zx and line in zy
		Int_t station = myHit->GetChamberID();
		// I need z value for the parabola part using the results from the Hough transforms and my track model
		Double_t zLabSys = myHit->GetZ();

		if ( 3 > station ){
			// if hit is in station 1 or 2 use 1st line in zx plane
			result.SetXYZ(0., 0., 0.); // TODO Use correct values
		} else if ( 5 > station ){
			// if hit is in station 3 or 4 use tangent to parabola in zx plane
			result.SetXYZ(0., 0., 0.); // TODO Use correct values
		} else if ( 7 > station ){
			// check if hit is in station 5 or 6 (if so: use 2nd line in zx plane)
			result.SetXYZ(0., 0., 0.); // TODO Use correct values
		} else {
			Warning("getPforHit","LayerID %i is not known for FTS", index);
			result.SetXYZ(0., 0., 0.);
		}
	}

	if (fVerbose > 0) std::cout << "P-Vector for hit " << index << " : " << result.X() << " " << result.Y() << " " << result.Z() << std::endl;
	return result;
}


TVector3 PndFtsHoughTrackCand::getPositionForHit(UInt_t index) {
	TVector3 result;




	if (index > GetNHits())
	{
		Warning("getPositionForHit","Hit index %i is too big for track candidate. Position is set to (0,0,0)", index);
		result.SetXYZ(0., 0., 0.);
	}
	else
	{
		const PndFtsHit* myHit = getHit(index);
		// track model is assumed to be line+parabola+line in zx and line in zy
		Int_t station = myHit->GetChamberID();
		// I take the z value from the hit and calculate the point on the track based on the results from the Hough transforms using my track model
		Double_t zLabSys = myHit->GetZ();

		if ( 3 > station ){
			// if hit is in station 1 or 2 use 1st line in zx plane
			result.SetXYZ(0., 0., 0.); // TODO Use correct values
		} else if ( 5 > station ){
			// if hit is in station 3 or 4 use tangent to parabola in zx plane
			result.SetXYZ(0., 0., 0.); // TODO Use correct values
		} else if ( 7 > station ){
			// check if hit is in station 5 or 6 (if so: use 2nd line in zx plane)
			result.SetXYZ(0., 0., 0.); // TODO Use correct values
		} else {
			Warning("getPforHit","LayerID %i is not known for FTS", index);
			result.SetXYZ(0., 0., 0.);
		}
	}

	if (fVerbose > 0) std::cout << "P-Vector for hit " << index << " : " << result.X() << " " << result.Y() << " " << result.Z() << std::endl;
	return result;
}

Int_t PndFtsHoughTrackCand::getCharge() const {
	if (0 < getQoverPzx()){
		return 1;
	}
	else{
		return -1;
	}
}

// end for conversion to PndTrack
/////////////////////////////////
