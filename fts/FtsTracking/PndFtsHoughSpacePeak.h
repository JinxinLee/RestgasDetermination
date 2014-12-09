/**
 @class PndFtsHoughSpacePeak

 @author Martin J. Galuska <martin [dot] j [dot] galuska (at) physik [dot] uni (minus) giessen [dot] de>

 @brief Class for saving peaks of a Hough space.

 Saves height and globalbin numbers belonging to this peak.
 TODO Use set/ unordered_set or something else rather than vectors to not store duplicate entries

 Created: 25.06.2014
 */


#ifndef PndFtsHoughSpacePeak_H
#define PndFtsHoughSpacePeak_H

#include "PndFtsHoughTrackerTask.h"

// Root Class Headers ----------------
#include "PndTrackCand.h"
#include "Rtypes.h"                     // for Double_t, Int_t, etc
#include "FairLogger.h" // for FairLogger, MESSAGE_ORIGIN
#include <set>





class PndFtsHoughSpacePeak {
public:
	static const Int_t noVal = -1;

	// Constructors / Destructors ---------
	PndFtsHoughSpacePeak( Int_t height = noVal, Int_t firstBin = noVal, Int_t firstHitIdx = noVal );
	~PndFtsHoughSpacePeak();

	void resetBins(){
		fBins.clear();
		fHitIds.clear();
		fFinished = kFALSE;
		fHeight = noVal;
	};

	void replaceBins( Int_t height, Int_t firstBin, Int_t firstHitIdx ){
		resetBins();
		addBin(firstBin, firstHitIdx);
	};

	void addBin(Int_t binNumber, Int_t hitIdx){
		fBins.insert(binNumber);
		fHitIds.insert(hitIdx);
	};

	void setFinished(Bool_t newVal){ fFinished = newVal; };

	Bool_t isFinished() const{ return fFinished; };
	Int_t getHeight() const{ return fHeight; };

	const std::set<Int_t>& getBins() const { return fBins; };
	const std::set<Int_t>& getHitIds() const { return fHitIds; };

	Bool_t OverlapsWith(const PndFtsHoughSpacePeak& toAdd){
		// return kTRUE iif toAdd has bins in common with this
		const std::set< Int_t > binsToSearch = toAdd.getBins();
		// try to find elements from toAdd in this
		for (std::set< Int_t >::iterator it = binsToSearch.begin(); it != binsToSearch.end(); ++it){
			std::set< Int_t >::iterator itFind = fBins.find( *it );
			if ( itFind != fBins.end() ) return kTRUE; // element from binsToSearch was found in this
		}
		return kFALSE; // none of the elements from binsToSearch was found in this
	};

	void MergeWith(const PndFtsHoughSpacePeak& toAdd) {
		// merge bins
		const std::set< Int_t > binsToMerge = toAdd.getBins();
		fBins.insert( binsToMerge.begin(), binsToMerge.end() );
		// merge hit indices
		const std::set< Int_t > hitsToMerge = toAdd.getHitIds();
		fHitIds.insert(  hitsToMerge.begin(), hitsToMerge.end()  );
	};


private:
	std::set< Int_t > fBins; // globalbins belonging to the peak
	std::set< Int_t > fHitIds; // hit indices belonging to the peak
	Int_t fHeight; // height of peak
	Bool_t fFinished;

	ClassDef(PndFtsHoughSpacePeak, 1);

};

#endif
