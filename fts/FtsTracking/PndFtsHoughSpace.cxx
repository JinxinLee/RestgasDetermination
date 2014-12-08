#include "PndFtsHoughSpace.h"

#include <iostream>

#include "TMath.h"
#include <math.h>
#include <algorithm>

#include <set>
#include <vector>
#include <map>

// FTS
#include "PndFtsHit.h"
#include "FairHit.h"

// magnetic field
#include "FairField.h"
#include "TVector3.h"

// (Hough) tracking
#include "PndTrackCand.h"
#include "PndTrack.h"
#include "FairTrackParP.h"
#include "PndFtsHoughTrackCand.h"

// histogramming / plotting
#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"

// peak finder
#include "TSpectrum2.h"

// root IO
#include "FairRunAna.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairTask.h"

#include "PndFtsHit.h"
#include "TString.h"
#include "FairHit.h"
#include "PndFtsHit.h"

#include "PndFtsTube.h"
#include "FairEventHeader.h"






ClassImp(PndFtsHoughSpace);


TString PndFtsHoughSpace::peakfinderOption="scanPaths"; // see FindAllPeaks for available options




std::ostream& operator <<(std::ostream& os, const ThetaYIdxPair& outPair){
	os << "(" << outPair.first << ", " << outPair.second << ")";
	return os;
}

std::ostream& operator <<(std::ostream& os, const IdxPath& outVector){
	os << "[";
	Int_t lastIdx = outVector.size()-1;
	if(lastIdx < 0){
		os << "]" ;
		return os;
	}

	for (UInt_t iVec = 0; iVec < lastIdx; ++iVec){
		os << outVector[iVec] << ", ";
	}
	os << outVector[lastIdx] << "]";
	return os;
}

std::ostream& operator <<(std::ostream& os, const HitIdxPathMap& outMap)
{
	os << '\n';
	if(outMap.begin() == outMap.end()){
		os << "{ , [] }";
		return os;
	}
	for (HitIdxPathMap::const_iterator itMap = outMap.begin(); itMap != outMap.end(); ++itMap){
		os << "{ "<< itMap->first << ", ";
		os << itMap->second;
		os << " }\n\n";
	}
	return os;
}


inline void PndFtsHoughSpace::AddHitToHS(UInt_t hitId, Double_t rho)
{
	fHitId.push_back(PndTrackCandHit(fFtsBranchId, hitId, rho));
}


inline void PndFtsHoughSpace::AddHitToHS(FairLink link, Double_t rho)
{
	fHitId.push_back(PndTrackCandHit(link.GetType(), link.GetIndex(), rho));
}




PndFtsHoughSpace::PndFtsHoughSpace(
		const char *name,

		Int_t nbinsx,
		Double_t xlow,
		Double_t xup,

		Int_t nbinsy,
		Double_t ylow,
		Double_t yup,

		Double_t zRefPos,
		Double_t interceptZx,

		PndFtsHoughTrackCand *associatedTrackCand,

		PndFtsHoughTrackerTask *trackerTask
) :
								fTrackerTask(trackerTask),

								fZRefPos(zRefPos),
								fInterceptZx(interceptZx),

								TH2S(name,name,nbinsx,xlow,xup,nbinsy,ylow,yup),

								// set from tracker task
								fFtsBranchId(0),
								fVerbose(0),
								fField(0),

								fAssociatedTrackCand(associatedTrackCand)
{
	if (0==fTrackerTask){
		std::cerr << "PndFtsHoughSpace FATAL ERROR Tracker task pointer not set.\n";
	} else {
		fVerbose = fTrackerTask->GetVerbose();
		if(3<fVerbose) std::cout << "PndFtsHoughSpace called with tracker ptr " << fTrackerTask << '\n';
		fFtsBranchId = fTrackerTask->getFtsBranchId();
		fField = fTrackerTask->getMagneticFieldPtr();

		setParametersForHsOption();
		filterInputHits();
	}
}

PndFtsHoughSpace::~PndFtsHoughSpace()
{

}


Bool_t PndFtsHoughSpace::setParametersForHsOption()
{
	// use option instead of GetName() or fName (even though it is the same)
	const TString option = GetName();

	fKeepBConstant = kTRUE; // kFALSE only for testing

	// set parameters according to the Hough transform I want to do
	if ("lineBeforeDipole" == option)
	{
		// make sure hits are not shifted for line hough transform
		if (0!=fInterceptZx) {
			std::cout << "PndFtsHoughSpace: " << "fInterceptZx was set to " << fInterceptZx << " That is not correct for line HT of stations before dipole field!\n";
			std::cout << "Will set interceptZx to 0 for " << option << '\n';
			fInterceptZx = 0.;
		}

		fUseNonSkewedStraws = kTRUE;
		fUseSkewedStraws = kFALSE;

		// Line for stations 1+2
		fOnlyUseHitsFromZ = 100.; // Set = 100. if you want to use all FTS hits, higher if you want to exclude hits that are closer to the interaction point than the value
		fOnlyUseHitsUpToZ = 380.; // Set = 1000. if you want to use all FTS hits, lower if you want to exclude hits that are further away from the interaction point than the value
	}
	else if ("parabola" == option)
	{
		fUseNonSkewedStraws = kTRUE;
		fUseSkewedStraws = kFALSE;

		// parabola for stations 3-5
		fOnlyUseHitsFromZ = 380.; // Set = 100. if you want to use all FTS hits, higher if you want to exclude hits that are closer to the interaction point than the value
		fOnlyUseHitsUpToZ = 630.; // Set = 1000. if you want to use all FTS hits, lower if you want to exclude hits that are further away from the interaction point than the value
	}
	else if ("parabolapz" == option)
	{
		fUseNonSkewedStraws = kTRUE;
		fUseSkewedStraws = kFALSE;

		// parabola for stations 3-5
		fOnlyUseHitsFromZ = 380.; // Set = 100. if you want to use all FTS hits, higher if you want to exclude hits that are closer to the interaction point than the value
		fOnlyUseHitsUpToZ = 630.; // Set = 1000. if you want to use all FTS hits, lower if you want to exclude hits that are further away from the interaction point than the value
	} else if ("lineBehindDipole" == option)
	{
		// make sure hits are not shifted for line hough transform
		if (0!=fInterceptZx) {
			std::cout << "PndFtsHoughSpace: " << "fInterceptZx was set to " << fInterceptZx << " That is not correct for line HT of stations after dipole field!\n";
			std::cout << "Will set interceptZx to 0 for " << option << '\n';
			fInterceptZx = 0.;
		}

		fUseNonSkewedStraws = kTRUE;
		fUseSkewedStraws = kFALSE;

		// Line for stations 5+6
		fOnlyUseHitsFromZ = 550.; // Set = 100. if you want to use all FTS hits, higher if you want to exclude hits that are closer to the interaction point than the value
		fOnlyUseHitsUpToZ = 1000.; // Set = 1000. if you want to use all FTS hits, lower if you want to exclude hits that are further away from the interaction point than the value
	} else if ("lineZy" == option)
	{
		// make sure hits are not shifted for line hough transform
		if (0!=fInterceptZx) {
			std::cout << "PndFtsHoughSpace: " << "fInterceptZx was set to " << fInterceptZx << " That is not correct for line HT of all stations in zy plane!\n";
			std::cout << "Will set interceptZx to 0 for " << option << '\n';
			fInterceptZx = 0.;
		}

		fUseNonSkewedStraws = kTRUE;
		fUseSkewedStraws = kFALSE;

		// Line for all FTS stations
		fOnlyUseHitsFromZ = 100.; // Set = 100. if you want to use all FTS hits, higher if you want to exclude hits that are closer to the interaction point than the value
		fOnlyUseHitsUpToZ = 1000.; // Set = 1000. if you want to use all FTS hits, lower if you want to exclude hits that are further away from the interaction point than the value
	}
	else
	{
		throwError("in PndFtsHoughSpace! option " + option + " is not implemented!");
	}

	if (0<fVerbose)
	{
		std::cout << "HoughSpace parameters successfully set for option " << option << '\n';
		if (1<fVerbose)
		{
			std::cout << "fUseNonSkewedStraws " << fUseNonSkewedStraws << '\n';
			std::cout << "fUseSkewedStraws " << fUseSkewedStraws << '\n';
			std::cout << "fOnlyUseHitsFromZ " << fOnlyUseHitsFromZ << '\n';
			std::cout << "fOnlyUseHitsUpToZ " << fOnlyUseHitsUpToZ << '\n';

			std::cout << "fInterceptZx " << fInterceptZx << '\n';
		}
	}
	return kTRUE;
}



void PndFtsHoughSpace::filterInputHits()
{
	if (1<fVerbose) {
		std::cout << "All FTS hits in event " << fTrackerTask->GetNFtsHits() << "\n";
	}

	for (int iHit = 0; iHit < fTrackerTask->GetNFtsHits(); iHit++)
	{
		const PndFtsHit *const myHit = fTrackerTask->GetFtsHit(iHit);

		// Skip hits from skewed or non-skewed straws (if I wish not to use them)
		if (0 != myHit->GetSkewed())
		{
			// hit comes from skewed straw
			if (kFALSE == fUseSkewedStraws)
			{
				if (1<fVerbose) {std::cout << "Skipping hit with index " << iHit << ", because it comes from a skewed straw! LayerID = " << myHit->GetLayerID() << '\n';}
				continue;
			}

		} else {
			// hit comes from non-skewed straw
			if (kFALSE == fUseNonSkewedStraws)
			{
				if (1<fVerbose) {std::cout << "Skipping hit with index " << iHit << ", because it comes from a non-skewed straw! LayerID = " << myHit->GetLayerID() << '\n';}
				continue;
			}

		}



		// only hits with z component between fOnlyUseHitsFromZ and fOnlyUseHitsUpToZ will be used in the Hough transform
		const Double_t hitZLabSys = myHit->GetZ();
		if ( (hitZLabSys < fOnlyUseHitsFromZ) || (hitZLabSys > fOnlyUseHitsUpToZ) ){
			if (1<fVerbose) {std::cout << "Skipping hit with index " << iHit << " , because its z " << hitZLabSys << " is not in [" << fOnlyUseHitsFromZ << ", " << fOnlyUseHitsUpToZ << "]\n";}
			continue;
		}

		// add surviving hits to the hit vector (z coordinate as sorting parameter chosen)
		if (1<fVerbose) {std::cout << "Ading hit with index " << iHit << " to the Hough space hit vector.\n";}
		AddHitToHS(iHit,hitZLabSys);

	} // for loop over all hits
}






Bool_t PndFtsHoughSpace::FillHoles(
		const Int_t lastBinX,
		const Int_t lastBinY,
		const Int_t currentBinY,
		IdxPath * ptrThetaYIdxPathVec
)
{
	// determine how many holes we have to fill
	const Int_t nHolesToFill = abs(currentBinY-lastBinY)-1;
	for (Int_t iCorrect = 1; iCorrect <= nHolesToFill; ++iCorrect)
	{
		const Int_t xCorrect = round(float(iCorrect)/float(nHolesToFill)); // gives 0 or 1
		Int_t yCorrect;
		if (currentBinY > lastBinY)
		{
			// we go up in the second value, therefore, we need to add to the yValue
			yCorrect = iCorrect;
		}
		else
		{
			yCorrect = -iCorrect;
		}


		const Int_t interpolateBinX = lastBinX+xCorrect;
		const Int_t interpolateBinY = lastBinY+yCorrect;

		const Int_t globalBin = GetBin(interpolateBinX,interpolateBinY);
		AddBinContent(globalBin);

		// save interpolated values into path vector
		ThetaYIdxPair idxPair(interpolateBinX,interpolateBinY);
		ptrThetaYIdxPathVec->push_back(idxPair);

		if (8<fVerbose)
		{
			std::cout << "I am filling hole " << iCorrect << " of " << nHolesToFill << '\n';
			std::cout << "globalbin = " << globalBin << '\n';
			std::cout << "(lastBinX, lastBinY) = (" << lastBinX << ", " << lastBinY << ")" << '\n';
			std::cout << "(lastBinX+1, currentBinY)     = (" << lastBinX+1 << ", " << currentBinY << ")" << '\n';
			std::cout << "xCorrect = " << xCorrect << "  yCorrect = " << yCorrect << '\n';
			std::cout << "(interpolateBinX, interpolateBinY) = (" << interpolateBinX << ", " << interpolateBinY << ")" << '\n';
		}
	}// for iCorrect
	return kTRUE;
}

void PndFtsHoughSpace::FillHoughSpace()
{
	// make sure we have hits in the Hough space
	if (0==GetNHits()){
		Info("FillHoughSpace","No hits in Hough space.");
		return;
	}


	// make Hough space according to the Hough transform I want to do
	const TString option = GetName();


	// for B field access
	Double_t By = 0.;




	// for storing the value to be calculated in Hough transform (yValue = offset for line, yValue = Q/p_{zx} for parabola)
	Double_t yVal = 0.;

	// store bin numbers for last and current entry (for making sure that there are no holes in the histogram)
	Int_t globalBin = 0;
	Int_t currentBinX = 0, currentBinY = 0, currentBinZ = 0;
	Int_t lastBinX = 0, lastBinY = 0;

	Bool_t firstEntry = kTRUE; // is used to indicate when holes in histogram have to be filled, set this to kTRUE for the first entry FOR EACH HIT



	// This produces the Hough space for a parabola or a line (with constant B field or with B field read from field maps)
	for (int iHit = 0; iHit < GetNHits(); iHit++)
	{
		firstEntry = kTRUE;
		const PndFtsHit *const myHit = getHitFromHS(iHit);


		// get hit position
		TVector3 hitPos = GetRawOrCalculatedHitPos(myHit);
		Double_t hitXLabSys = hitPos.X();
		Double_t hitYLabSys = hitPos.Y();
		Double_t hitZLabSys = hitPos.Z();
		Double_t hitXShifted = hitXLabSys - fInterceptZx; // shifts all x positions of hits so that they go through x=0 at z=zOffset (for parabola)
		Double_t hitZShifted = hitZLabSys - fZRefPos; // z coordinate in local coordinate system (for parabola and for line)



		if (1<fVerbose) {
			std::cout << "Doing " << option << " Hough transform for hit (hitZLabSys, hitXLabSys) = (" << hitZLabSys << ", " << hitXLabSys << ") cm";
			if (kTRUE == fKeepBConstant) { std::cout << " ignoring B field maps\n"; } else { std::cout << " reading B field maps\n"; }
		}

		// B field
		if (kTRUE == fKeepBConstant)
		{
			// do not take B field into account
			By = 1.;
		}
		else
		{
			// Use B field information
			Double_t po[3], BB[3];
			po[0] = hitXLabSys; // Use magnetic field at real (not shifted) x position
			po[1] = hitYLabSys;
			po[2] = hitZLabSys;
			fField->GetFieldValue(po, BB); //return value in KG (G3)
			By = BB[1] / 10.; // By is y-component of magnetic field in Tesla
		}


		//------------------
		// theta scan
		//------------------

		// get indices for first and last bins on x-axis
		Int_t iThetaFirst  = fXaxis.GetFirst();
		Int_t iThetaLast   = fXaxis.GetLast();

		// save path for each hit
		IdxPath thetaYIdxPathVec;

		// calculate Hough transform for hit iHit
		// for each hit a scan in theta is done by going through the x-axis of the Hough space from lower to higher values
		for (Int_t iTheta = iThetaFirst; iTheta < iThetaLast; ++iTheta)
		{
			// get theta value corresponding to iTheta
			Double_t thetaRad = fXaxis.GetBinCenter(iTheta); // theta has to be stored in rad
			if (9<fVerbose)	{ std::cout << " for thetaRad = " << thetaRad << '\n'; }

			// calculate yVal depending on which Hough transform we need
			if ("parabola" == option)
			{
				// Use shifted x and shifted z for parabola
				yVal = equationParabola(thetaRad, hitZShifted, hitXShifted, By);
				if (9<fVerbose)	{ std::cout << "Q/pzx = " << yVal; }
			}
			else if ("parabolapz" == option)
			{
				// Use shifted x and shifted z for parabola
				yVal = equationParabolaPz(thetaRad, hitZShifted, hitXShifted, By);
				if (9<fVerbose)	{ std::cout << "pz/Q = " << yVal; }
			}
			else if ( ("lineBeforeDipole" == option) || ("lineBehindDipole" == option) )
			{
				// Use real x and shifted z for line
				yVal = equationLineZxOrZy(thetaRad, hitZShifted, hitXLabSys);
				if (9<fVerbose) { std::cout << "xLP/PL = " << yVal; }
			}
			else if ("lineZy" == option)
			{
				// Use real x and shifted z for line
				yVal = equationLineZxOrZy(thetaRad, hitZShifted, hitYLabSys);
				if (9<fVerbose) { std::cout << "yLine = " << yVal; }
			}
			else
			{
				// should never happen
				throwError("in FillHoughSpace! option " + option + " is not implemented!");
			}





			// Insert "point" into Hough space and check in which bins we filled
			globalBin = Fill(thetaRad,yVal);
			if (5<fVerbose) { std::cout << "Hough point was filled into Hough space. globalbin = " << globalBin << " for option" << option <<'\n'; }
			// Find currentBinX(=iTheta) and currentBinY from globalBin
			GetBinXYZ(globalBin, currentBinX, currentBinY, currentBinZ);



			// if Fill was into a real bin (and not into over-/underflow) remove holes in Hough space (by assuming a straight line in between neighboring points)
			// for each theta 1 yVal is calculated, so holes will only appear in yVal, not in theta
			if (globalBin>=0) // -1 would mean over- or underflow
			{
				if (5<fVerbose) { std::cout << "OK! Hough point was NOT written to over- or underflow of histogram. Setting firstEntry to kFALSE now. "<< option <<'\n'; }

				// TODO Remove the following check for optimization, it should always be true
				if (currentBinX != iTheta){
					std::cerr << "\n\nError in FillHoughSpace! Hough point was filled into xBin " << currentBinX << " and not in " << iTheta << "\n";
					std::cerr << "iThetaFirst = " << iThetaLast << " iThetaLast = " << iThetaLast << "\n";
					std::cerr << "Over- or underflow on y-axis or FATAL error!\n\n\n";
				}

				// fill holes if the current Hough point is not the first entry in Hough space for the hit
				if (kFALSE == firstEntry)
				{
					if (5<fVerbose)
					{
						std::cout << "This is not the first point of the hit in the histogram. I will fix all holes which might be between this entry and the last one in the histogram"<<'\n';
					}

					FillHoles(lastBinX, lastBinY, currentBinY, &thetaYIdxPathVec);

				} // if not first entry to be written into histogram
				else
				{
					//		++nHitsInHoughSpace; // count hits for making of Hough space (only once per hit)
					if (5<fVerbose) { std::cout << "This is the first point of the hit in the histogram. I will not try to fix any holes in the " << option << " histogram"<<'\n';}
				}


				firstEntry = kFALSE;

				// save calculated values into path vector
				ThetaYIdxPair idxPair(currentBinX,currentBinY);
				thetaYIdxPathVec.push_back(idxPair);

			} // if NOT filled into over- or underflow
			else
			{
				if (9<fVerbose)	{ std::cout << "Watch out! Point was written to over- or underflow of histogram. firstEntry is set to kTRUE. "<< option <<'\n'; }
				firstEntry = kTRUE; // otherwise, algorithm connects first point which does not go into over-/underflow with (0,0)
			}

			if (9<fVerbose)	{ std::cout << "currentBinY = " << currentBinY << "  lastBinY = " << lastBinY << '\n'; }
			lastBinX = currentBinX;
			lastBinY = currentBinY;


		} // for theta
		//		std::cout << "map before iHit " << iHit << ": " << fHitThetaYIdxPath << '\n';
		// save the final path for the hit
		if ( 0 < thetaYIdxPathVec.size() ){
			HitIdxPathPair hitPathPair( iHit, thetaYIdxPathVec );
			fHitThetaYIdxPath.insert( hitPathPair );
		}
	} // for iHit
	if (1<fVerbose) std::cout << "map after all hits: " << fHitThetaYIdxPath << '\n';
}






Bool_t PndFtsHoughSpace::FindAllPeaks(
		const UInt_t minHeight,
		std::vector<PndFtsHoughTracklet> &tracklets
)
{
	// make sure the output vector is empty
	if ( 0!=tracklets.size() ) throwError("in PndFtsHoughSpace: tracklet vector is not empty.");


	// check if Hough space has at least one entry
	if (1>GetEntries())
	{
		std::cout << "Hough Space is empty. No peak can be found. Return empty tracklet vector." << '\n';
		return kFALSE;
	}



	if ("scanPaths" == peakfinderOption)
	{
		// this peak finder is only implemented for 2D histograms
		// for each hit scan the Hough space along the hit's path -> this reduces the 2d peak finding to nHits 1d problems.
		// require bin height >= minHeight
		// and that it is between a falling and a rising area


		// stores possible peaks for all hits
		HitIdxPeakVec hitIdxPeaks;
		// stores possible peaks for one hit
		PeakVec possiblePeaks;

		// hit loop
		for (HitIdxPathMap::const_iterator itMap = fHitThetaYIdxPath.begin(); itMap != fHitThetaYIdxPath.end(); ++itMap){
			Int_t hitIdx = itMap->first;
			IdxPath path = itMap->second;
			possiblePeaks.clear();

			// loop over bins along the path
			for (UInt_t iBinPair = 0; iBinPair < path.size(); ++iBinPair){
				// current bin
				ThetaYIdxPair binPair = path[iBinPair];
				const Int_t currBinX = binPair.first;
				const Int_t currBinY = binPair.second;
				// check height of current bin
				const Int_t currBinNumber = GetBin(currBinX,currBinY);
				const Int_t currHeight = GetBinContent(currBinNumber);

				if (minHeight <= currHeight) { // Bin could belong to a peak
					// check if we already found a peak candidate
					const Int_t nPeaksSoFar = possiblePeaks.size();
					if ( 0 < nPeaksSoFar ){
						PndFtsHoughSpacePeak& currPeak = possiblePeaks[nPeaksSoFar-1];
						if ( kFALSE == currPeak.isFinished() ){ // continue building up current peak
							if ( currHeight == currPeak.getHeight() ) currPeak.addBin(currBinNumber);
							if ( currHeight < currPeak.getHeight() ) currPeak.setFinished(kTRUE);
							if ( currHeight > currPeak.getHeight() ) currPeak.replaceBins(currHeight, currBinNumber);
							continue; // do not create a new peak
						}
					}
					// Add a new peak (only if we do not continue a nonfinished existing one)
					PndFtsHoughSpacePeak newPeak(currHeight, currBinNumber);
					possiblePeaks.push_back(newPeak);
				} // Bin could belong to a peak
			}// loop over bins along the path
			HitIdxPeakVecPair hitPeaksPair( hitIdx, possiblePeaks );
			hitIdxPeaks.insert( hitPeaksPair );
		}// hit loop

		// TODO Build up tracklets from peaks



		return kTRUE;
	} else
		if ("allPeaks>minHeight uses calculation instead of saved path" == peakfinderOption)
		{
			// peak finder only for 2D histograms implemented

			// the following is an adaptation of the TH1::GetMaximumBin() code


			// move search window over histogram
			// if a peak is >= minHeight check the neighboring bins and merge peaks

			// get values for first and last bins on each axis
			Int_t xFirstBin  = fXaxis.GetFirst();
			Int_t xLastBin   = fXaxis.GetLast();
			Int_t yFirstBin  = fYaxis.GetFirst();
			Int_t yLastBin   = fYaxis.GetLast();
			//		   Int_t zfirst  = fZaxis->GetFirst();
			//		   Int_t zlast   = fZaxis->GetLast();

			// find all peaks that have a height >= minHeight
			// go from low to high in theta <- SUPER IMPORTANT
			Int_t currBinNumber;
			Double_t currHeight;
			Int_t peakBinX = 0;
			Int_t peakBinY = 0;
			Int_t peakBinZ = 0; // location of maximum (x,y,z)

			for (Int_t currBinX=xFirstBin; currBinX<=xLastBin; ++currBinX) {
				for (Int_t currBinY=yFirstBin; currBinY<=yLastBin; ++currBinY) {
					//		   for (binz=zfirst;binz<=zlast;binz++) {

					currBinNumber = GetBin(currBinX,currBinY); // binz
					currHeight = GetBinContent(currBinNumber);
					if (minHeight > currHeight) {
						// currBinNumber is not high enough to be considered a peak
						SetBinContent(currBinNumber,0); // DEBUG: Uncomment this if you want to produce nice Hough space plots for demonstration
						continue;
					}
					// I found a potential peak, so I check the neighbors in next higher theta bins
					// I save the currently highest peak
					// I set the last visited peak = current peak and move to the next neighbor
					// if neighbor < highest peak and neighbor <= last peak, but above the minHeight, I set it to 0. I save its height as the last height.

					// if neighbor is higher than the last peak, I set the last visited bin to 0 and move on
					// if neighbor is the same height as last peak, I find the middle and set all others to 0 and I widen the errors of the middle bin

					Int_t iBinX = 0; // for moving to neighbors in x direction

					// save info of highest peak found while searching neighbors
					Int_t combinedPeakBinXLow=currBinX;  // saves in which x (thetaRad) bin a peak starts
					Int_t combinedPeakBinXHigh=currBinX; // saves in which x (thetaRad) bin a peak ends
					Double_t combinedPeakHeight=currHeight;

					// save info of last visited neighbor
					Int_t lastVisitedBinX = currBinX;
					Int_t lastVisitedBinNumber = currBinNumber;
					Double_t lastVisitedHeight = currHeight;

					// storing the current neighbor bin
					Int_t currNeighborBinX = currBinX;
					Int_t currNeighborBinNumber = currBinNumber;
					Double_t currNeighborHeight = currHeight;

					do{
						++iBinX; // to move along the x bins
						// set current neighbor
						currNeighborBinX = currBinX+iBinX;
						if (currNeighborBinX>xLastBin){
							break;
						}
						currNeighborBinNumber = GetBin(currNeighborBinX,currBinY); // binz
						currNeighborHeight = GetBinContent(currNeighborBinNumber);

						if (minHeight > currNeighborHeight){
							// end of peak region in x direction found, I can get out of the loop
							SetBinContent(currNeighborBinNumber,0); // DEBUG: Uncomment this if you want to produce nice Hough space plots for demonstration
							break;
						}

						if (currNeighborHeight > combinedPeakHeight){
							// actually peak starts with neighbor (or even later), but current bin is not in peak
							for (Int_t xBinToDel=combinedPeakBinXLow; xBinToDel<= combinedPeakBinXHigh; ++xBinToDel){
								Int_t BinNumberToDel = GetBin(xBinToDel,currBinY);
								SetBinContent(BinNumberToDel,0);
							}
							combinedPeakBinXLow = currNeighborBinX;
							combinedPeakBinXHigh = currNeighborBinX;
							combinedPeakHeight = currNeighborHeight;
						} else if (currNeighborHeight < combinedPeakHeight){
							if (currNeighborHeight <= lastVisitedHeight){
								// peak is dropping, delete current bin (which is at the falling edge of the peak)
								SetBinContent(currNeighborBinNumber,0);
							} else {
								// next peak starts rising, I can end the loop
								break;
							}
						} else if (currNeighborHeight == combinedPeakHeight){
							// peak is spread over several bins
							combinedPeakBinXHigh = currNeighborBinX;
						}
						lastVisitedBinX = currNeighborBinX;
						lastVisitedBinNumber = currNeighborBinNumber;
						lastVisitedHeight = currNeighborHeight;

					} while(kTRUE);


					// DEBUG: comment this out if you want to produce nice Hough space plots for demonstration
					// remove the peak which was found, otherwise it (or parts of it) might be found again
					for (Int_t xBinToDel=combinedPeakBinXLow; xBinToDel<= combinedPeakBinXHigh; ++xBinToDel){
						Int_t BinNumberToDel = GetBin(xBinToDel,currBinY);
						SetBinContent(BinNumberToDel,0);
					}




					// peak is in the middle
					peakBinX  = (combinedPeakBinXHigh+combinedPeakBinXLow)/2; // if sum is not an even number, I have to correct for that later
					peakBinY  = currBinY;
					// locmaz  = binz;
					// get values corresponding to the peak
					Double_t peakThetaVal = fXaxis.GetBinCenter(peakBinX);
					Double_t peakSecondVal = fYaxis.GetBinCenter(peakBinY);

					// correct peak value if mean for peak cannot be divided by 2 without remainder
					if (0!=(combinedPeakBinXHigh+combinedPeakBinXLow)%2){
						peakThetaVal+=fXaxis.GetBinWidth(peakThetaVal)/2.;
					}

					// get full width of peak = (highest bin + halfwidth) - (lowest bin - halfwidth)
					Double_t combinedPeakThetaLowEdge = fXaxis.GetBinCenter(combinedPeakBinXLow) - fXaxis.GetBinWidth(combinedPeakBinXLow)/2.;
					Double_t combinedPeakThetaHighEdge = fXaxis.GetBinCenter(combinedPeakBinXHigh) + fXaxis.GetBinWidth(combinedPeakBinXHigh)/2.;

					Double_t peakThetaHw = (combinedPeakThetaHighEdge-combinedPeakThetaLowEdge)/2.;
					Double_t peakSecondHw = fYaxis.GetBinWidth(peakSecondVal)/2.;

					// create tracklet and push it back to output
					PndFtsHoughTracklet currentTracklet(fZRefPos, fTrackerTask);
					currentTracklet.SetHoughTransformResults(peakThetaVal, peakSecondVal, currHeight, peakThetaHw, peakSecondHw);

					///////////////////////////////////////////
					// TODO this is messy, because the code is very similar to FillHoughSpace. Probably, I should find a way to merge it
					// add hits which are in the peak to the tracklet
					// 1 calculate the 2nd value for the next higher/lower theta bin of combined peak theta
					// 2 If peak 2nd value is within [min hit 2nd value - half width,  max hit 2nd value + half width] add the hit to the tracklet
					// 3 otherwise the hit is not in the peak

					// This is more complicated to check, but also true
					// A calculate the 2nd value for peak theta
					// B If hit 2nd value is within [peak 2nd value - peakSecondHw,  peak 2nd value + peakSecondHw] add the hit
					// C If hit 2nd value is < peak 2nd value - peakSecondHw, hit is in peak if the 2nd value of the next higher/lower theta bin is >= peak 2nd value
					// D If hit 2nd value is > peak 2nd value + peakSecondHw, hit is in peak if the 2nd value of the next lower/higher theta bin is <= peak 2nd value
					// E otherwise the hit is not in the peak

					// 1 calculate the 2nd value for the next higher/lower theta bin of peak theta
					UInt_t thetaBinLo = combinedPeakBinXLow-1;
					UInt_t thetaBinHi = combinedPeakBinXHigh+1;
					// special case if we are at the edge of the Hough space
					if (thetaBinLo>xFirstBin) {
						thetaBinLo=xFirstBin;
					}
					if (thetaBinHi>xLastBin) {
						thetaBinHi=xLastBin;
					}

					// get theta values
					const Double_t thetaRadLo = fXaxis.GetBinCenter(thetaBinLo);
					const Double_t thetaRadHi = fXaxis.GetBinCenter(thetaBinHi);

					// for storing the values to be calculated in Hough transform (yValue = offset for line, yValue = Q/pzx for parabola)
					Double_t yValLo = 0.;
					Double_t yValHi = 0.;

					// for B field access
					Double_t By = 0.;


					for (int iHit = 0; iHit < GetNHits(); iHit++)
					{
						const PndFtsHit* myHit = getHitFromHS(iHit);

						// get hit position
						const TVector3 hitPos = GetRawOrCalculatedHitPos(myHit);


						Double_t hitXLabSys = hitPos.X();
						Double_t hitYLabSys = hitPos.Y();
						Double_t hitZLabSys = hitPos.Z();
						Double_t hitXShifted = hitXLabSys - fInterceptZx; // shifts all x positions of hits so that they go through x=0 at z=zOffset (for parabola)
						Double_t hitZShifted = hitZLabSys - fZRefPos; // z coordinate in local coordinate system (for parabola and for line)

						if (kTRUE == fKeepBConstant)
						{
							// do not take B field into account
							By = 1.;
						}
						else
						{
							// Use B field information
							Double_t po[3], BB[3];
							po[0] = hitXLabSys; // Use magnetic field at real (not shifted) x position
							po[1] = hitYLabSys;
							po[2] = hitZLabSys;
							fField->GetFieldValue(po, BB); //return value in KG (G3)
							By = BB[1] / 10.; // By is y-component of magnetic field in Tesla
						}

						const TString option = GetName();
						if ("parabola" == option)
						{
							// Use shifted x and shifted z for parabola
							yValLo = equationParabola(thetaRadLo, hitZShifted, hitXShifted, By);
							yValHi = equationParabola(thetaRadHi, hitZShifted, hitXShifted, By);
							if (9<fVerbose)	{
								std::cout << "Q/pzx = " << yValLo << '\n';
								std::cout << "Q/pzx = " << yValHi << '\n';
							}
						}
						else if ("parabolapz" == option)
						{
							yValLo = equationParabolaPz(thetaRadLo, hitZShifted, hitXShifted, By);
							yValHi = equationParabolaPz(thetaRadHi, hitZShifted, hitXShifted, By);

							if (9<fVerbose)	{
								std::cout << "pz/Q = " << yValLo << '\n';
								std::cout << "pz/Q = " << yValHi << '\n';
							}
						}
						else if ( ("lineBeforeDipole" == option) || ("lineBehindDipole" == option) )
						{
							// Use real x and shifted z for line

							yValLo = equationLineZxOrZy(thetaRadLo, hitZShifted, hitXLabSys);
							yValHi = equationLineZxOrZy(thetaRadHi, hitZShifted, hitXLabSys);

							if (9<fVerbose) {
								std::cout << "xLP/PL = " << yValLo << '\n';
								std::cout << "xLP/PL = " << yValHi << '\n';
							}
						}
						else if ("lineZy" == option)
						{
							// Use real x and shifted z for line

							yValLo = equationLineZxOrZy(thetaRadLo, hitZShifted, hitYLabSys);
							yValHi = equationLineZxOrZy(thetaRadHi, hitZShifted, hitYLabSys);

							if (9<fVerbose) {
								std::cout << "xLP = " << yValLo << '\n';
								std::cout << "xLP = " << yValHi << '\n';
							}
						}
						else
						{
							std::cout << "Error in FillHoughSpace! option " << option << " is not implemented!" << '\n';
							return kFALSE;
						}


						// 2 If peak 2nd value is within [min hit 2nd value - half width,  max hit 2nd value + half width] add the hit to the tracklet
						const Double_t yMin = std::min(yValLo,yValHi);
						const Double_t yMax = std::max(yValLo,yValHi);
						const Double_t yMinHw = fYaxis.GetBinWidth(yMin)/2.;
						const Double_t yMaxHw = fYaxis.GetBinWidth(yMax)/2.;



						if ( (yMin-yMinHw <= peakSecondVal) && (yMax+yMaxHw >= peakSecondVal) ){
							currentTracklet.AddHit(fFtsBranchId, iHit, hitZLabSys);
						}

					} // loop over hits
					/////////////////////////////////////////////////
					tracklets.push_back(currentTracklet);
					// } // z loop
				} // y loop
			} // x loop



			return kTRUE;
		} else

			if (("allPeaks>minHeight Old" == peakfinderOption) || ("allPeaksSearchWindow>minHeight Old" == peakfinderOption))
			{
				// peak finder based on weighted means (only for 2D histograms implemented)


				// the following is an adaptation of the TH1::GetMaximumBin() code



				// define vicinityLength for search window which determines an effective height of the bin
				Int_t vicinityLength = 0; // set for not using a search window
				// search window has size (2*vicinityLength+1)^2
				if ("allPeaksSearchWindow>minHeight" == peakfinderOption)
				{
					vicinityLength = 2; // set for using a search window
				}


				// move search window over histogram and select highest value
				Int_t locmax, locmay, locmaz; // location of maximum (x,y,z)

				//   -*-*-*-*-*Return location of bin with maximum value in the range*-*
				//             ======================================================
				Int_t binNumber, binx, biny, binz=0;
				// get values for first and last bins on each axis and take into account that we might want a search window (make sure search window stays within histogram)
				Int_t xfirst  = fXaxis.GetFirst()+vicinityLength;
				Int_t xlast   = fXaxis.GetLast()-vicinityLength;
				Int_t yfirst  = fYaxis.GetFirst()+vicinityLength;
				Int_t ylast   = fYaxis.GetLast()-vicinityLength;
				//		   Int_t zfirst  = fZaxis->GetFirst()+vicinityLength;
				//		   Int_t zlast   = fZaxis->GetLast()-vicinityLength;

				// find all peaks that have a height >= minHeight
				Double_t currentHeight;
				locmax = locmay = locmaz = 0;
				//		   for (binz=zfirst;binz<=zlast;binz++) {
				for (biny=yfirst;biny<=ylast;biny++) {
					for (binx=xfirst;binx<=xlast;binx++) {
						currentHeight = 0.;
						// iterate over vicinity
						for (Int_t xVicinityBin = -vicinityLength; xVicinityBin <= vicinityLength; ++xVicinityBin)
						{
							for (Int_t yVicinityBin = -vicinityLength; yVicinityBin <= vicinityLength; ++yVicinityBin)
							{
								binNumber = GetBin(binx+xVicinityBin,biny+yVicinityBin); //,binz);
								currentHeight += 1./(1.+std::max(abs(xVicinityBin),abs(yVicinityBin)))*GetBinContent(binNumber); // linear, unendlich norm
								//							currentHeight += 1./(1.+abs(xVicinity) +abs(yVicinity))*houghspace->GetBinContent(binNumber); // linear
								// currentHeight += 1./(1.+pow(max(xVicinity,yVicinity),2))*houghspace->GetBinContent(binNumber); // quadratic, unendlich norm
							}
						}
						if (currentHeight >= minHeight) {
							locmax  = binx;
							locmay  = biny;
							//		               locmaz  = binz;
							// get values corresponding to the peak
							Double_t peakThetaVal = fXaxis.GetBinCenter(locmax);
							Double_t peakSecondVal = fYaxis.GetBinCenter(locmay);

							Int_t binmaxglobal = Fill(peakThetaVal, peakSecondVal, 0); // returns binnumber without modifying the histogram
							Double_t peakThetaHw = fXaxis.GetBinWidth(peakThetaVal)/2.;
							Double_t peakSecondHw = fYaxis.GetBinWidth(peakSecondVal)/2.;

							// create tracklet and push it back to output
							PndFtsHoughTracklet currentTracklet(fZRefPos, fTrackerTask);
							currentTracklet.SetHoughTransformResults(peakThetaVal, peakSecondVal, currentHeight, peakThetaHw, peakSecondHw);

							///////////////////////////////////////////
							// TODO this is messy, because the code is very similar to FillHoughSpace. Probably, I should find a way to merge it
							// add hits which are in the peak to the tracklet
							// 1 calculate the 2nd value for the next higher/lower theta bin of peak theta
							// 2 If peak 2nd value is within [min hit 2nd value - half width,  max hit 2nd value + half width] add the hit to the tracklet
							// 3 otherwise the hit is not in the peak

							// This is more complicated to check, but also true
							// A calculate the 2nd value for peak theta
							// B If hit 2nd value is within [peak 2nd value - peakSecondHw,  peak 2nd value + peakSecondHw] add the hit
							// C If hit 2nd value is < peak 2nd value - peakSecondHw, hit is in peak if the 2nd value of the next higher/lower theta bin is >= peak 2nd value
							// D If hit 2nd value is > peak 2nd value + peakSecondHw, hit is in peak if the 2nd value of the next lower/higher theta bin is <= peak 2nd value
							// E otherwise the hit is not in the peak

							// 1 calculate the 2nd value for the next higher/lower theta bin of peak theta
							UInt_t thetaBinLo = locmax-1;
							UInt_t thetaBinHi = locmax+1;
							// special case if we are at the edge of the Hough space
							if (thetaBinLo>xfirst) {
								thetaBinLo=xfirst;
							}
							if (thetaBinHi>xlast) {
								thetaBinHi=xlast;
							}

							// get theta values
							const Double_t thetaRadLo = fXaxis.GetBinCenter(thetaBinLo);
							const Double_t thetaRadHi = fXaxis.GetBinCenter(thetaBinHi);

							// for storing the values to be calculated in Hough transform (yValue = offset for line, yValue = Q/pzx for parabola)
							Double_t yValLo = 0.;
							Double_t yValHi = 0.;

							// for B field access
							Double_t By = 0.;


							for (int iHit = 0; iHit < GetNHits(); iHit++)
							{
								const PndFtsHit* myHit = getHitFromHS(iHit);

								// get hit position
								const TVector3 hitPos = GetRawOrCalculatedHitPos(myHit);

								Double_t hitXLabSys = hitPos.X();
								Double_t hitYLabSys = hitPos.Y();
								Double_t hitZLabSys = hitPos.Z();
								Double_t hitXShifted = hitXLabSys - fInterceptZx; // shifts all x positions of hits so that they go through x=0 at z=zOffset (for parabola)
								Double_t hitZShifted = hitZLabSys - fZRefPos; // z coordinate in local coordinate system (for parabola and for line)

								if (kTRUE == fKeepBConstant)
								{
									// do not take B field into account
									By = 1.;
								}
								else
								{
									// Use B field information
									Double_t po[3], BB[3];
									po[0] = hitXLabSys; // Use magnetic field at real (not shifted) x position
									po[1] = hitYLabSys;
									po[2] = hitZLabSys;
									fField->GetFieldValue(po, BB); //return value in KG (G3)
									By = BB[1] / 10.; // By is y-component of magnetic field in Tesla
								}

								const TString option = GetName();
								if ("parabola" == option)
								{
									// Use shifted x and shifted z for parabola
									yValLo = equationParabola(thetaRadLo, hitZShifted, hitXShifted, By);
									yValHi = equationParabola(thetaRadHi, hitZShifted, hitXShifted, By);
									if (9<fVerbose)	{
										std::cout << "Q/pzx = " << yValLo << '\n';
										std::cout << "Q/pzx = " << yValHi << '\n';
									}
								}
								else if ("parabolapz" == option)
								{
									yValLo = equationParabolaPz(thetaRadLo, hitZShifted, hitXShifted, By);
									yValHi = equationParabolaPz(thetaRadHi, hitZShifted, hitXShifted, By);

									if (9<fVerbose)	{
										std::cout << "pz/Q = " << yValLo << '\n';
										std::cout << "pz/Q = " << yValHi << '\n';
									}
								}
								else if ( ("lineBeforeDipole" == option) || ("lineBehindDipole" == option) )
								{
									// Use real x and shifted z for line

									yValLo = equationLineZxOrZy(thetaRadLo, hitZShifted, hitXLabSys);
									yValHi = equationLineZxOrZy(thetaRadHi, hitZShifted, hitXLabSys);

									if (9<fVerbose) {
										std::cout << "xLP/PL = " << yValLo << '\n';
										std::cout << "xLP/PL = " << yValHi << '\n';
									}
								}
								else if ("lineZy" == option)
								{
									// Use real x and shifted z for line

									yValLo = equationLineZxOrZy(thetaRadLo, hitZShifted, hitYLabSys);
									yValHi = equationLineZxOrZy(thetaRadHi, hitZShifted, hitYLabSys);

									if (9<fVerbose) {
										std::cout << "xLP = " << yValLo << '\n';
										std::cout << "xLP = " << yValHi << '\n';
									}
								}
								else
								{
									std::cout << "Error in FillHoughSpace! option " << option << " is not implemented!" << '\n';
									return kFALSE;
								}


								// 2 If peak 2nd value is within [min hit 2nd value - half width,  max hit 2nd value + half width] add the hit to the tracklet
								const Double_t yMin = std::min(yValLo,yValHi);
								const Double_t yMax = std::max(yValLo,yValHi);
								const Double_t yMinHw = fYaxis.GetBinWidth(yMin)/2.;
								const Double_t yMaxHw = fYaxis.GetBinWidth(yMax)/2.;



								if ( (yMin-yMinHw <= peakSecondVal) && (yMax+yMaxHw >= peakSecondVal) ){
									currentTracklet.AddHit(fFtsBranchId, iHit, hitZLabSys);
								}

							} // loop over hits
							/////////////////////////////////////////////////
							tracklets.push_back(currentTracklet);
						} // height is big enough
					} // x loop
				} // y loop
				//		   } // z loop

				return kTRUE;
			}
			else if ("tspectrum2" == peakfinderOption)
			{
				// TODO: This peakfinder should be rechecked!
				// TODO: This peakfinder does not assign hits to the tracklets (so far)
				Int_t maxpeaks = 20;
				// Finding the peaks (as in example macro)
				TSpectrum2 s(maxpeaks,3); // second argument: higher = peaks can be closer together (1 enforces 3 sigma seperation between peaks)
				s.SetAverageWindow(3); // standard is 3 (for Markov smoothing)
				s.SetDeconIterations(100); // standard is 3, more is better
				// 2nd parameter: sigma of searched peaks = 2 standard
				// 4th parameter: threshold: (default=0.05)  peaks with amplitude less than threshold*highest_peak are discarded.  0<threshold<1
				//		Int_t nfound = s.Search(houghspace, 2,"nobackground,nomarkov",0.5); // works ok for line and for parabola, kind of...
				Int_t nfound = s.Search(this, 2,"",0.5); // TODO: does this work for my derived class?!?
				Float_t *xpeaks = s.GetPositionX();
				Float_t *ypeaks = s.GetPositionY();
				s.Print();

				// for output
				for (UInt_t iPeak = 0; iPeak < nfound; ++iPeak){
					Double_t peakThetaVal = xpeaks[iPeak];
					Double_t peakSecondVal = ypeaks[iPeak];

					Int_t binmaxglobal = Fill(peakThetaVal, peakSecondVal, 0); // returns binnumber without modifying the histogram
					Double_t peakThetaHw = fXaxis.GetBinWidth(peakThetaVal)/2.;
					Double_t peakSecondHw = fYaxis.GetBinWidth(peakSecondVal)/2.;


					Double_t currentHeight = GetBinContent(binmaxglobal);

					// create tracklet and push it back to output
					PndFtsHoughTracklet currentTracklet(fZRefPos, fTrackerTask);
					currentTracklet.SetHoughTransformResults(peakThetaVal, peakSecondVal, currentHeight, peakThetaHw, peakSecondHw);
					tracklets.push_back(currentTracklet);
				}
				//		binmaxglobal = houghspace->Fill(peakTheta, peakSecond, 0); // returns binnumber without modifying the histogram
				return kTRUE;
			}
			else
			{
				std::cout << "PeakFinder error: Option " << peakfinderOption << " is not implemented!\n";
				return kFALSE;
			}
}
