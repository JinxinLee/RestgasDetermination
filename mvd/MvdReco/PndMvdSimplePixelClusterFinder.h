#ifndef PNDMVDSIMPLEPIXELCLUSTERFINDER_H
#define PNDMVDSIMPLEPIXELCLUSTERFINDER_H

#include "PndMvdPixelClusterFinder.h"
/// PndMvdSimplePixelClusterFinder.h
/// Simple cluster finder for pixel detectors.
///
/// It takes the first hit in the DigiArray and moves it into a new cluster.
/// Then it looks through the remaining hits in the DigiArray if there are hits which are in the area around the first hit and moves them into the cluster.
/// The radius is given by the first parameter.
/// Then it takes the new hits in the cluster and looks if there are other hits which are in the range of these hits and moves them into the cluster, too.
/// This process is repeated until no hits around the cluster are found
/// The the first hit of the remaining DigiHits is taken to create a new hit and the cluster search process is started again.
///
/// @param param[0] Range as Int_t at which a pixel belongs to a cluster or not
/// @param param[1] Number of columns in a front-end
/// @param param[2] Number of rows in a front-end

class PndMvdSimplePixelClusterFinder : public PndMvdPixelClusterFinder
{
	public : 	PndMvdSimplePixelClusterFinder():PndMvdPixelClusterFinder(){};
			PndMvdSimplePixelClusterFinder(std::vector<Double_t> params, std::vector<PndMvdDigiPixel> hits):
				PndMvdPixelClusterFinder(params, hits){
				if(fVerbose>0)std::cout << "Hits in array: " << fHits.size() << std::endl;
			};

			///Main method which searches for the clusters.
			///It returns a matrix of ints where a column corresponds to a cluster and an integer to a hit in the DigiArray
			std::vector< std::vector< Int_t> > GetClusters();
	private :
			//PndMvdDigiPixel MoveHit(std::vector<PndMvdDigiPixel>* hitVector, Int_t index);
			Int_t MoveHit(std::vector<Int_t>* hitVector, Int_t index) const;

			bool IsInRange(PndMvdDigiPixel hit1, PndMvdDigiPixel hit2) const;

};
#endif
