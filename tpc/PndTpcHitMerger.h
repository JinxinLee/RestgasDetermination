//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      PndTpcHitMerger merges hits so that for example 10 Hits along 
//      along a track segment of 5 cm becomes 1 Hit
//	  
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      DerMenschMitDen 17 Armen    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef PNDTPCHITMERGER_H
#define PNDTPCHITMERGER_H

#include <vector>
#include "PndTpcDEDXStorageHelper.h"

class PndTpcPoint;


class PndTpcHitMerger  
{
public:
	PndTpcHitMerger():fSegmentLength(0.5),fCatchRemaining(false){}
	~PndTpcHitMerger();
	void CatchRemaining(bool opt=true)	{ fCatchRemaining=opt;	};
	
	void CombineHitsInTrack(double SegmentLength, const std::vector<PndTpcDEDXStorageHelper> &Hits, std::vector<PndTpcDEDXStorageHelper> &CombinedHits);
	void CombineHitsInTrack2(double SegmentLength, const std::vector<PndTpcDEDXStorageHelper> &Hits, std::vector<PndTpcDEDXStorageHelper> &CombinedHits);
	
	void CombineHits(unsigned int nHits, const std::vector<PndTpcDEDXStorageHelper> &Hits, std::vector<PndTpcDEDXStorageHelper> &CombinedHits);
	
	void ConvertHits(const std::vector<PndTpcPoint*> &Hits, std::vector<PndTpcDEDXStorageHelper> &ConvertedHits) const;
private:
	std::vector<PndTpcPoint*> fCombinedHits;
	double fSegmentLength;
	bool fCatchRemaining;
	
};


#endif
