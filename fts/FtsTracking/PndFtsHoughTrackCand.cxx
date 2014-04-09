#include "PndFtsHoughTrackCand.h"

#include "PndFtsHoughTrackerTask.h"


#include <iostream>
#include "math.h"

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "PndFtsHit.h"
#include "PndTrack.h"
#include "FairTrackParP.h"
#include "TVector3.h"

ClassImp(PndFtsHoughTrackCand);

PndFtsHoughTrackCand::PndFtsHoughTrackCand(PndFtsHoughTrackerTask *trackerTask) :
				fTrackerTask(trackerTask),

				fFtsHitArray(0),
				fFtsBranchId(0),
				fVerbose(0),

				fZxLineParabola(0., trackerTask), // TODO: It could be a problem here that I set the z reference value to 0.

				fZxParabola(0., trackerTask),

				fZxParabolaLine(0., trackerTask),

				fZyLine(0., trackerTask),

				fZLineParabola(0.),
				fZParabolaLine(0.)
{
	if (0==fTrackerTask){
		std::cout << "PndFtsHoughTrackCand FATAL ERROR Tracker task pointer not set.\n";
	} else {
		fVerbose = fTrackerTask->GetVerbose();
		if(3<fVerbose) std::cout << "PndFtsHoughTrackCand called with tracker ptr " << fTrackerTask << '\n';
		fFtsHitArray = fTrackerTask->getFtsHitArrayPtr();
		fFtsBranchId = fTrackerTask->getFtsBranchId();
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

PndTrackCand PndFtsHoughTrackCand::getPndTrackCand() {
	// TODO: Maybe this is not necessary because PndFtsHoughTrackCand is derived from PndTrackCand
	PndTrackCand myCand;
	// copy all the hits from PndFtsHoughTrackCand *this to PndTrackCand myCand
	for (UInt_t iHit = 0; iHit < GetNHits(); ++iHit)
	{
		PndTrackCandHit inHit = GetSortedHit(iHit);
		const Int_t inHitId = inHit.GetHitId();
		const Int_t inDetId = inHit.GetDetId();
		const Double_t inRho = inHit.GetRho();
		myCand.AddHit(inDetId, inHitId, inRho);
	}
	return myCand;
}



// for conversion to PndTrack, compare to PndTools/riemannfit/PndRiemannTrack.cxx, line 1104
/////////////////////////////////////////////////////////////////////////////////////////////
// getNumHits is called getNHits (because my track cand class is derived from PndTrackCand and that's how it's called there)
// I don't need the B field to calculate the parameters
PndTrack PndFtsHoughTrackCand::getPndTrack() {
	// calculates first and last parameter, but uses an empty PndTrackCand which has to be set lateron using SetTrackCandRef
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
	// Warn if we do not have a complete track candidate
	if (!isComplete()) Warning("getHit","You try to access hits before we have a complete track candidate.");
	if (index < GetNHits()){
		//		TClonesArray *ftsHitArray= (TClonesArray *)FairRootManager::Instance()->GetObject("FTSHit");
		const PndFtsHit *myHit = (PndFtsHit*) fFtsHitArray->At(GetSortedHit(index).GetHitId());
		return myHit;
	} else {
		return 0;
	}
}

FairTrackParP PndFtsHoughTrackCand::getTrackParPForHit(const UInt_t index) {
	// TODO: Check if all arguments are correct

	// get position of hit with index in track candidate
	const PndFtsHit *myHit = getHit(index);
	if (0==myHit){
		Warning("getTrackParPForHit","Cannot get hit, probably the tracking has not finished or the index is too large.");
		return FairTrackParP();
	}
	// Take z of hit and calculate the position and momentum for that z using the results from the Hough transforms and my track model
	Double_t zLabSys = myHit->GetZ();

	// position should NOT come from hit, it should come from the pattern recognition track model
	// position error can be large (like 1* or 2* tube size) as the Kalman filter will adjust it.
	TVector3 hitPos = getPos(zLabSys);
	TVector3 hitPosError;
	myHit->PositionError(hitPosError); // TODO: Set the error correctly

	// momentum comes from the pattern recognition track model
	TVector3 mom = getP(zLabSys);
	TVector3 momError = 0.1*mom; // TODO: add correct values here

	// set plane as detector plane in which the hit is, FTS planes are parallel to xy plane
	TVector3 origin; // set origin of plane = position of hit
	myHit->Position(origin);
	TVector3 dj(1,0,0); // unit vector along x // TODO: Check if that is set correctly for FTS
	TVector3 dk(0,1,0); // unit vector along y // TODO: Check if that is set correctly for FTS

	// -----   Constructor with track parameters in LAB -----------------------------------
	// FairTrackParP::FairTrackParP(TVector3 pos, TVector3 Mom, TVector3 posErr, TVector3 MomErr, Int_t Q, TVector3 o, TVector3 dj, TVector3 dk)
	FairTrackParP result(hitPos, mom, hitPosError, momError, getCharge(), origin, dj, dk);
	return result;
}

TVector3 PndFtsHoughTrackCand::getP(const Double_t zLabSys) const{
	TVector3 mom;

	if (kFALSE == isComplete())
	{
		Warning("getPforHit","Track cand. is not complete yet. Momentum will be calculated for incomplete track cand.");
	}

	Double_t pZLabSys;
	Double_t pXLabSys;
	std::pair<Double_t, Double_t> pZPXLabSys;

	// track model is assumed to be line+parabola+line in zx and line in zy
	if ( zLabSys <= fZLineParabola ){
		// use 1st line in zx plane
		pZPXLabSys = getPZPXLabLine(zLabSys, &fZxLineParabola);
	} else if ( zLabSys < fZParabolaLine ){
		// use "tangent" to parabola in zx plane
		pZPXLabSys = getPZPXLabParabola(zLabSys);
	} else {
		// use 2nd line in zx plane
		pZPXLabSys = getPZPXLabLine(zLabSys, &fZxParabolaLine);
	}

	pZLabSys = pZPXLabSys.first;
	pXLabSys = pZPXLabSys.second;
	const Double_t pYLabSys = getPYLab();
	mom.SetXYZ(pXLabSys, pYLabSys, pZLabSys);
	if (fVerbose > 0) std::cout << "P-Vector for z=" << zLabSys << " : " << mom.X() << " " << mom.Y() << " " << mom.Z() << std::endl;
	return mom;
}


TVector3 PndFtsHoughTrackCand::getPos(const Double_t zLabSys) const{
	// calculates the point on the track based on the results from the Hough transforms using my track model for the given z

	if (kFALSE == isComplete())
	{
		Warning("getPositionForHit","Track cand. is not complete yet. Position will be calculated for incomplete track cand.");
	}

	TVector3 position;
	// track model is assumed to be line+parabola+line in zx and line in zy
	const Double_t yLabSys = getXOrYLabForLine(zLabSys, &fZyLine);
	Double_t xLabSys;

	if ( zLabSys <= fZLineParabola ){
		// use 1st line in zx plane
		xLabSys = getXOrYLabForLine(zLabSys, &fZxLineParabola);

	} else if ( zLabSys < fZParabolaLine ){
		// use parabola in zx plane
		getXLabForParabola(zLabSys);
	} else {
		// use 2nd line in zx plane
		xLabSys = getXOrYLabForLine(zLabSys, &fZxParabolaLine);
	}

	position.SetXYZ(xLabSys, yLabSys, zLabSys);
	if (fVerbose > 0) std::cout << "Pos-Vector for z=" << zLabSys << " : " << position.X() << " " << position.Y() << " " << position.Z() << std::endl;
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
