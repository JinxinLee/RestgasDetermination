#include "PndFtsHoughSpace.h"

#include <iostream>

#include "TMath.h"
#include <math.h>
#include <algorithm>

#include <set>
#include <vector>
#include <map>

// FTS
#include "PndGeoFtsPar.h"
#include "PndFtsMapCreator.h"
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


TString PndFtsHoughSpace::peakfinderOption="allPeaks>minHeight"; // see FindAllPeaks for available options


inline void PndFtsHoughSpace::AddHit(UInt_t hitId, Double_t rho)
{
	fHitId.push_back(PndTrackCandHit(fFtsBranchId, hitId, rho));
}


inline void PndFtsHoughSpace::AddHit(FairLink link, Double_t rho)
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

		Int_t ftsBranchId,
		TClonesArray *ftsHitArray,

		FairField *field
) :
fFtsHitArray(ftsHitArray),
fFtsBranchId(ftsBranchId),
fVerbose(3),

fZRefPos(zRefPos),
fInterceptZx(interceptZx),

fField(field),

TH2S(name,name,nbinsx,xlow,xup,nbinsy,ylow,yup)

{
	if (0==ftsHitArray){
		std::cout << "PndFtsHoughSpace FATAL ERROR Hit array not set.\n";
	}
	setParametersForHsOption();
	filterInputHits();
}

PndFtsHoughSpace::~PndFtsHoughSpace()
{

}


Bool_t PndFtsHoughSpace::setParametersForHsOption()
{
	fKeepBConstant = kTRUE; // kFALSE only for testing

	// set parameters according to the Hough transform I want to do
	if ("lineBeforeDipole" == fName)
	{
		// make sure hits are not shifted for line hough transform
		if (0!=fInterceptZx) {
			std::cout << "PndFtsHoughSpace: " << "fInterceptZx was set to " << fInterceptZx << " That is not correct for line HT! of stations before dipole field!\n";
		}
		if (0<fVerbose) {  std::cout << "Will set interceptZx to 0 for " << fName << std::endl; }
		fInterceptZx = 0.;

		fUseNonSkewedStraws = kTRUE;
		fUseSkewedStraws = kFALSE;

		// Line for stations 1+2
		fOnlyUseHitsFromZ = 100.; // Set = 100. if you want to use all FTS hits, higher if you want to exclude hits that are closer to the interaction point than the value
		fOnlyUseHitsUpToZ = 380.; // Set = 1000. if you want to use all FTS hits, lower if you want to exclude hits that are further away from the interaction point than the value
	}
	else if ("parabola" == fName)
	{
		fUseNonSkewedStraws = kTRUE;
		fUseSkewedStraws = kFALSE;

		// parabola for stations 3-5
		fOnlyUseHitsFromZ = 380.; // Set = 100. if you want to use all FTS hits, higher if you want to exclude hits that are closer to the interaction point than the value
		fOnlyUseHitsUpToZ = 700.; // Set = 1000. if you want to use all FTS hits, lower if you want to exclude hits that are further away from the interaction point than the value
	}
	else if ("parabolapz" == fName)
	{
		fUseNonSkewedStraws = kTRUE;
		fUseSkewedStraws = kFALSE;

		// parabola for stations 3-5
		fOnlyUseHitsFromZ = 380.; // Set = 100. if you want to use all FTS hits, higher if you want to exclude hits that are closer to the interaction point than the value
		fOnlyUseHitsUpToZ = 700.; // Set = 1000. if you want to use all FTS hits, lower if you want to exclude hits that are further away from the interaction point than the value
	}
	else
	{
		std::cout << "Error in PndFtsHoughSpace! option " << fName << " is not implemented!" << std::endl;
		return kFALSE;
	}

	if (0<fVerbose)
	{
		std::cout << "HoughSpace parameters successfully set for option " << fName << '\n';
		if (1<fVerbose)
		{
			std::cout << "fUseNonSkewedStraws " << fUseNonSkewedStraws << 'n';
			std::cout << "fUseSkewedStraws " << fUseSkewedStraws << 'n';
			std::cout << "fOnlyUseHitsFromZ " << fOnlyUseHitsFromZ << 'n';
			std::cout << "fOnlyUseHitsUpToZ " << fOnlyUseHitsUpToZ << 'n';

			std::cout << "fInterceptZx " << fInterceptZx << 'n';
		}
	}
}



Bool_t PndFtsHoughSpace::filterInputHits()
{
	if (1<fVerbose) {
		std::cout << "All FTS hits in event " << fFtsHitArray->GetEntriesFast() << "\n";
	}

	for (int iHit = 0; iHit < fFtsHitArray->GetEntriesFast(); iHit++)
	{
		PndFtsHit* myHit = (PndFtsHit*) fFtsHitArray->At(iHit);

		// Skip hits from skewed or non-skewed straws (if I wish not to use them)
		if (0 != myHit->GetSkewed())
		{
			// hit comes from skewed straw
			if (kFALSE == fUseSkewedStraws)
			{
				if (1<fVerbose) {std::cout << "Skipping hit with index " << iHit << " , because it comes from a skewed straw! LayerID = " << myHit->GetLayerID() << std::endl;}
				continue;
			}

		} else {
			// hit comes from non-skewed straw
			if (kFALSE == fUseNonSkewedStraws)
			{
				if (1<fVerbose) {std::cout << "Skipping hit with index " << iHit << " , because it comes from a non-skewed straw! LayerID = " << myHit->GetLayerID() << std::endl;}
				continue;
			}

		}



		// only hits with z component between fOnlyUseHitsFromZ and fOnlyUseHitsUpToZ will be used in the hough transform
		const Double_t hitZLabSys = myHit->GetZ();
		if ( (hitZLabSys < fOnlyUseHitsFromZ) || (hitZLabSys > fOnlyUseHitsUpToZ) ){
			if (1<fVerbose) {std::cout << "Skipping hit with index " << iHit << " , because its z " << hitZLabSys << " is not in [" << fOnlyUseHitsFromZ << ", " << fOnlyUseHitsUpToZ << "]\n";}
			continue;
		}

		// add surviving hits to the hit vector (z coordinate as sorting parameter chosen)
		if (1<fVerbose) {std::cout << "Ading hit with index " << iHit << " to the Hough space hit vector.\n";}
		AddHit(iHit,hitZLabSys);

	} // for loop over all hits
}






Bool_t PndFtsHoughSpace::FillHoles(
		Int_t lastBinX,
		Int_t lastBinY,
		Int_t currentBinY
)
{
	// Makes sure there are no holes in the Hough space
	// holes cannot appear in theta, we always go from lower theta to the next higher value
	// fill holes between (lastBinX, lastBinY) and (currentBinX, currentBinY) = (lastBinX+1, currentBinY) with a line
	const UInt_t nHolesToFill = abs(currentBinY-lastBinY);
	for (UInt_t iCorrect = 1; iCorrect < nHolesToFill; ++iCorrect)
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

		Int_t globalBin = GetBin(lastBinX+xCorrect,lastBinY+yCorrect);
		AddBinContent(globalBin);

		if (8<fVerbose)
		{
			std::cout << "I am filling hole number " << iCorrect << std::endl;
			std::cout << "globalbin = " << globalBin << std::endl;
			std::cout << "(lastBinX, lastBinY) = (" << lastBinX << ", " << lastBinY << ")" << std::endl;
			std::cout << "(lastBinX+1, currentBinY)     = (" << lastBinX+1 << ", " << currentBinY << ")" << std::endl;
			std::cout << "xCorrect = " << xCorrect << "  yCorrect = " << yCorrect << std::endl;
			std::cout << "(lastbinx+xCorrect, lastbiny+yCorrect) = (" << lastBinX+xCorrect << ", " << lastBinY+yCorrect << ")" << std::endl;
		}
	}// for iCorrect

}














Bool_t PndFtsHoughSpace::MakeHoughSpace()
{
	// function fills the Hough space using the equation corresponding to the name of the Hough space
	// If everything goes well, the function returns kTRUE, else kFALSE (probably Hough space name is set incorrectly or there are no FTS hits)

	// !!! WARNING The theta values are NOT the same as in the interaction point. They are always calculated relative to a shifted coordinate system and only 2-dimensional !!!

	// The angle (theta) to the z-axis in the z-x- or z-y-plane at a z reference position will be scanned
	// from theta corresponding to lowest bin to theta corresponding to highest bin of x-axis

	// y component of B field will be read from field maps if fKeepBConstant is kFALSE



	// make sure we have hits in the Hough space
	if (0==GetNHits()){
		Info("MakeHoughSpace","houghspace is not set.");
		return kFALSE;
	}


	// make Hough space according to the Hough transform I want to do
	const TString option = GetName();


	// for B field access
	Double_t By = 0.;




	// for storing the value to be calculated in Hough transform (yValue = offset for line, yValue = Q/pzx for parabola)
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
		const PndFtsHit* myHit = getHit(iHit);


		// get hit position
		TVector3 hitPos;
		myHit->Position(hitPos);

		Double_t hitXLabSys = hitPos.X();
		Double_t hitYLabSys = hitPos.Y();
		Double_t hitZLabSys = hitPos.Z();
		Double_t hitXShifted = hitXLabSys - fInterceptZx; // shifts all x positions of hits so that they go through x=0 at z=zOffset (for parabola)
		Double_t hitZShifted = hitZLabSys - fZRefPos; // z coordinate in local coordinate system (for parabola and for line)



		if (1<fVerbose) {
			std::cout << "Doing " << option << " Hough transform for hit (hitZLabSys, hitXLabSys) = (" << hitZLabSys << ", " << hitXLabSys << ") cm";
			if (kTRUE == fKeepBConstant)
			{
				std::cout << " ignoring B field maps\n";
			} else
			{
				std::cout << " reading B field maps\n";
			}
		}



		// get indices for first and last bins on x-axis
		Int_t iThetaFirst  = fXaxis.GetFirst();
		Int_t iThetaLast   = fXaxis.GetLast();



		// calculate Hough transform for hit iHit
		// for each hit a scan in theta is done
		// by going through the x-axis of the Hough space

		for (Int_t iTheta = iThetaFirst; iTheta < iThetaLast; ++iTheta)
		{
			// get corresponding theta value
			Double_t theta = fXaxis.GetBinCenter(iTheta);


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





			Double_t thetaRad = theta / 180. * meinpi;






			if ("parabola" == option)
			{
				// Use shifted x and shifted z for parabola
				yVal = equationParabola(thetaRad, hitZShifted, hitXShifted, By);

				if (9<fVerbose)	{ std::cout << "Q/pzx = " << yVal; }
			}
			else if ("parabolapz" == option)
			{
				yVal = equationParabolaPz(thetaRad, hitZShifted, hitXShifted, By);

				if (9<fVerbose)	{ std::cout << "pz/Q = " << yVal; }
			}
			else if ("lineBeforeDipole" == option)
			{
				// Use real x and shifted z for line

				yVal = equationLineZx(thetaRad, hitZShifted, hitXLabSys);

				if (9<fVerbose) { std::cout << "xLP = " << yVal; }
			}
			else
			{
				std::cout << "Error in MakeHoughSpace! option " << option << " is not implemented!" << std::endl;
				return kFALSE;
			}




			if (9<fVerbose)	{ std::cout << " for (theta, hitXreal) = (" << theta << ", " << hitXLabSys << ")" << std::endl; }






			globalBin = Fill(theta,yVal);
			if (5<fVerbose) { std::cout << "Hough point was filled into Hough space. globalbin = " << globalBin << " for option" << option <<std::endl; }
			// Find binx and biny for histogram from global bin number
			GetBinXYZ(globalBin, currentBinX, currentBinY, currentBinZ);






			// if Fill was into a real bin (and not into over-/underflow) remove holes in Hough space (by assuming a straight line in between neighboring points)
			// for each theta 1 yVal is calculated, so holes will only appear in yVal, not in theta
			if (globalBin>=0) // -1 would mean over- or underflow
			{
				if (5<fVerbose) { std::cout << "OK! Hough point was NOT written to over- or underflow of histogram. Setting firstEntry to kFALSE now. "<< option <<std::endl; }

				// TODO Remove the following check for optimization, it should always be true
				if (currentBinX != iTheta){
					std::cout << "\n\nError in MakeHoughSpace! Hough point was filled into xBin " << currentBinX << " and not in " << iTheta << "\n";
					std::cout << "iThetaFirst = " << iThetaLast << " iThetaLast = " << iThetaLast << "\n";
					std::cout << "Over- or underflow on y-axis or FATAL error!\n\n\n";
				}

				// fill holes if the current Hough point is not the first entry in Hough space for the hit
				if (kFALSE == firstEntry)
				{
					if (5<fVerbose)
					{
						std::cout << "This is not the first point of the hit in the histogram. I will fix all holes which might be between this entry and the last one in the histogram"<<std::endl;
					}

					FillHoles(lastBinX, lastBinY, currentBinY);

				} // if not first entry to be written into histogram
				else
				{
					//		++nHitsInHoughSpace; // count hits for making of Hough space (only once per hit)
					if (5<fVerbose) { std::cout << "This is the first point of the hit in the histogram. I will not try to fix any holes in the " << option << " histogram"<<std::endl;}
				}


				firstEntry = kFALSE;


			}
			else
			{
				if (9<fVerbose)	{ std::cout << "Watch out! Point was written to over- or underflow of histogram. firsttheta is set to kTRUE. "<< option <<std::endl; }
				firstEntry = kTRUE; // otherwise, algorithm connects first point which does not go into over-/underflow with (0,0)
			}

			if (9<fVerbose)	{ std::cout << "biny = " << currentBinY << "  lastbiny = " << lastBinY << std::endl; }
			lastBinX = currentBinX;
			lastBinY = currentBinY;


		} // for theta
	} // for iHit
	return kTRUE;
}






Bool_t PndFtsHoughSpace::FindAllPeaks(
		const UInt_t minHeight,
		std::vector<PndFtsHoughTracklet> &tracklets
)
{
	// finds all peaks that satisfy the minimum height requirement minHeight
	// returns kTRUE if at least one peak was found, kFALSE otherwise (Hough space name was not set correctly or Hough Space is empty / has too few hits or tracklets were not empty)


	// TODO: This information is obsolete and should be adjusted
	// tracklets should be empty at the beginning and will contain all values found for the peak in the Hough space
	//	peakTheta = theta for peak
	//	peakSecond = Q/pzx for peak (parabola HT)
	//	peakSecond = intercept for peak (line HT) (in z-x- or z-y-plane)

	// peakThetaHw = half width of peak in theta
	// peakSecondHw = half width of peak in second value (see above for what it stands for)

	// actualHeight height of the peak in the histogram (in counts)
	// the tracklets will contain the hitIds of all hits that contribute to the peak




	// make sure the output vector is empty
	if ( 0!=tracklets.size() ){
		std::cout << "FATAL error in PndFtsHoughSpace tracklet vector is not empty.\n";
		return kFALSE;
	}


	// check if Hough space has at least one entry
	if (1>GetEntries())
	{
		std::cout << "Hough Space is empty. No peak can be found. Return empty tracklet vector." << std::endl;
		return kFALSE;
	}



	if (("allPeaks>minHeight" == peakfinderOption) || ("allPeaksSearchWindow>minHeight" == peakfinderOption))
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
						binNumber = GetBin(binx+xVicinityBin,biny+yVicinityBin,binz);
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
					Double_t peakThetaHwVal = fXaxis.GetBinWidth(peakThetaVal);
					Double_t peakSecondHwVal = fYaxis.GetBinWidth(peakSecondVal);

					// create tracklet and push it back to output
					PndFtsHoughTracklet currentTracklet(fFtsBranchId, fFtsHitArray);
					currentTracklet.SetHoughTransformResults(peakThetaVal, peakSecondVal, currentHeight, peakThetaHwVal, peakSecondHwVal);
					tracklets.push_back(currentTracklet);
				}
			}
		}
		//		   }

		return kTRUE;
	}
	else if ("tspectrum2" == peakfinderOption)
	{
		// TODO: This peakfinder should be rechecked!
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
			Double_t peakThetaHwVal = fXaxis.GetBinWidth(peakThetaVal);
			Double_t peakSecondHwVal = fYaxis.GetBinWidth(peakSecondVal);


			Double_t currentHeight = GetBinContent(binmaxglobal);

			// create tracklet and push it back to output
			PndFtsHoughTracklet currentTracklet;
			currentTracklet.SetHoughTransformResults(peakThetaVal, peakSecondVal, currentHeight, peakThetaHwVal, peakSecondHwVal);
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


