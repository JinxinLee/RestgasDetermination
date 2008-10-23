#ifndef PNDMVDIDEALPIXELCLUSTERFINDER_H
#define PNDMVDIDEALPIXELCLUSTERFINDER_H

#include "PndMvdPixelClusterFinder.h"
#include "PndMvdDigiPixel.h"
#include "TClonesArray.h"
#include <vector>
/// PndMvdIdealPixelClusterFinder.h
/// Ideal cluster finder using MCtruth information.

class PndMvdIdealPixelClusterFinder : public PndMvdPixelClusterFinder
{
	public : 	PndMvdIdealPixelClusterFinder():PndMvdPixelClusterFinder(){};
			PndMvdIdealPixelClusterFinder(std::vector<Double_t> params, std::vector<PndMvdDigiPixel> hits):
				PndMvdPixelClusterFinder(params, hits){
				if(fVerbose>0) std::cout << "Hits in array: " << fHits.size() << std::endl;
			};

			std::vector< std::vector< Int_t> > GetClusters();
			Int_t MoveHit(std::vector<Int_t>* hitVector, Int_t index) const;
			bool IsInRange(PndMvdDigiPixel hit1, PndMvdDigiPixel hit2) const;

};
#endif
