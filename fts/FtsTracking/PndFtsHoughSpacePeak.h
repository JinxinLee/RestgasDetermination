/**
 @class PndFtsHoughSpacePeak

 @author Martin J. Galuska <martin [dot] j [dot] galuska (at) physik [dot] uni (minus) giessen [dot] de>

 @brief Class for saving peaks of a Hough space.

 Created: 25.06.2014
*/


#ifndef PndFtsHoughSpacePeak_H
#define PndFtsHoughSpacePeak_H

#include "PndFtsHoughTrackerTask.h"

// Root Class Headers ----------------
#include "PndTrackCand.h"
#include "Rtypes.h"                     // for Double_t, Int_t, etc
#include "FairLogger.h" // for FairLogger, MESSAGE_ORIGIN
#include <vector>

// For error reporting
#include "TString.h"
#include <stdexcept>




class PndFtsHoughSpacePeak {
public:
	static const Int_t noVal = -1;

	// Constructors/Destructors ---------
	PndFtsHoughSpacePeak( Int_t height=noVal, Int_t firstBin=noVal );
	~PndFtsHoughSpacePeak();

	void resetBins(){
		fBinVec.clear();
		fFinished = kFALSE;
		fHeight = noVal;
	};

	void replaceBins( Int_t height, Int_t firstBin ){
		resetBins();
		addBin(firstBin);
	};

	void addBin(Int_t binNumber){ fBinVec.push_back(binNumber); };

	void setFinished(Bool_t newVal){ fFinished = newVal; };

	Bool_t isFinished(){ return fFinished; };
	Int_t getHeight(){ return fHeight; };


private:
	std::vector< Int_t > fBinVec; //globalbins belonging to the peak
	Int_t fHeight; // height of peak
	Bool_t fFinished;

	ClassDef(PndFtsHoughSpacePeak,1);
};

#endif
