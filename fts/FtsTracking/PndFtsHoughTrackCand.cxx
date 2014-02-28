#include "PndFtsHoughTrackCand.h"

#include <iostream>
#include "math.h"

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "PndFtsHit.h"
#include "PndTrack.h"
#include "FairTrackParP.h"
#include "TVector3.h"

ClassImp(PndFtsHoughTrackCand);

PndFtsHoughTrackCand::PndFtsHoughTrackCand(Int_t ftsBranchId, TClonesArray *ftsHitArray) :
														fFtsHitArray(ftsHitArray),
														fFtsBranchId(ftsBranchId),

														fVerbose(0),

														fZxLineParabola(0., fFtsBranchId, fFtsHitArray), // TODO: It could be a problem here that I set the z reference value to 0.

														fZxParabola(0., fFtsBranchId, fFtsHitArray),

														fZxParabolaLine(0., fFtsBranchId, fFtsHitArray),

														fZyLine(0., fFtsBranchId, fFtsHitArray),

														fZLineParabola(0.),
														fZParabolaLine(0.)
{
	if (0==fFtsHitArray){
		std::cout << "PndFtsHoughTrackCand FATAL ERROR Hit array not set.\n";
	}
}

PndFtsHoughTrackCand::~PndFtsHoughTrackCand()
{

}


void PndFtsHoughTrackCand::SetZxFirstLine(const PndFtsHoughTracklet zxLineParabola){
	fZxLineParabola = zxLineParabola;
	fZLineParabola = fZxLineParabola.getZRefLabSys();
	addUniqueTrackletHits(fZxLineParabola);
}
void PndFtsHoughTrackCand::SetZxParabola(const PndFtsHoughTracklet zxParabola){
	// warn if line before dipole field and parabola within are not calculated wrt the same z reference value
	if (zxParabola.getZRefLabSys() != fZLineParabola){
		std::cout << "WARNING from PndFtsHoughTrackCand: First line and parabola were not calculated wrt the same z position! Potentially FATAL ERROR!\n";
	}
	fZxParabola = zxParabola;
	addUniqueTrackletHits(fZxParabola);
}
void PndFtsHoughTrackCand::SetZxSecondLine(const PndFtsHoughTracklet zxParabolaLine){
	fZxParabolaLine = zxParabolaLine;
	fZParabolaLine = fZxParabolaLine.getZRefLabSys();
	addUniqueTrackletHits(fZxParabolaLine);
}
void PndFtsHoughTrackCand::SetZyLine(const PndFtsHoughTracklet zyLine){
	fZyLine=zyLine;
	addUniqueTrackletHits(fZyLine);
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



void PndFtsHoughTrackCand::addUniqueTrackletHits(PndFtsHoughTracklet inTracklet)
{
	// add all the hits from the tracklet which are not already in the hitId vector
	for (UInt_t iHit = 0; iHit < inTracklet.GetNHits(); ++iHit)
	{
		PndTrackCandHit inHit = inTracklet.GetSortedHit(iHit);
		const Int_t inHitId = inHit.GetHitId();
		const Int_t inDetId = inHit.GetDetId();
		// if hit is NOT in track -1 is returned by HitInTrack, otherwise the index (>=0) in the HitId vector is returned
		if (-1==HitInTrack(inDetId,inHitId)){
			// add hit to track cand
			const Double_t inRho = inHit.GetRho();
			AddHit(inDetId, inHitId, inRho);
		}
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
	// this method will sort the hitId vector
	// Make sure we have a complete track candidate before we try to access any hits
	if (!isComplete()) return 0;
	if (index < GetNHits()){
		//		TClonesArray *ftsHitArray= (TClonesArray *)FairRootManager::Instance()->GetObject("FTSHit");
		const PndFtsHit *myHit = (PndFtsHit*) fFtsHitArray->At(GetSortedHit(index).GetHitId());
		return myHit;
	} else {
		return 0;
	}
}

FairTrackParP PndFtsHoughTrackCand::getTrackParPForHit(UInt_t index) {
	// TODO: Check if all arguments are correct

	// position should NOT come from hit, it should come from the pattern recognition track model
	// position error can be large (like 1* or 2* tube size) as the Kalman filter will adjust it.
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
			Warning("getTrackParPForHit","Cannot get hit, probably the tracking has not finished.");
			return FairTrackParP();
		}
		//		myHit->Position(hitPos);


		// TODO: Set the error correctly
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
	TVector3 position;

	if (index > GetNHits())
	{
		Warning("getPositionForHit","Hit index %i is too big for track candidate. Position is set to (0,0,0)", index);
		position.SetXYZ(0., 0., 0.);
		return position;
	}

	if (kFALSE == isComplete())
	{
		Warning("getPositionForHit","Track candidate is not complete yet. Position is set to (0,0,0)", index);
		position.SetXYZ(0., 0., 0.);
		return position;
	}

	const PndFtsHit* myHit = getHit(index);
	// track model is assumed to be line+parabola+line in zx and line in zy
	Int_t station = myHit->GetChamberID();
	// I take the z value from the hit and calculate the point on the track based on the results from the Hough transforms using my track model
	const Double_t zLabSys = myHit->GetZ();
	const Double_t yLabSys = getXOrYLabForLine(zLabSys, &fZyLine);
	Double_t xLabSys;

	if ( 3 > station ){
		// if hit is in station 1 or 2 use 1st line in zx plane
		xLabSys = getXOrYLabForLine(zLabSys, &fZxLineParabola);

	} else if ( 5 > station ){
		// if hit is in station 3 or 4 use parabola in zx plane
		getXLabForParabola(zLabSys, &fZxParabola, &fZxLineParabola);
	} else if ( 7 > station ){
		// check if hit is in station 5 or 6 (if so: use 2nd line in zx plane)
		xLabSys = getXOrYLabForLine(zLabSys, &fZxParabolaLine);
	} else {
		Warning("getPforHit","LayerID %i is not known for FTS", index);
		position.SetXYZ(0., 0., 0.);
		return position;
	}

	position.SetXYZ(xLabSys, yLabSys, zLabSys);
	if (fVerbose > 0) std::cout << "P-Vector for hit " << index << " : " << position.X() << " " << position.Y() << " " << position.Z() << std::endl;
	return position;
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
