#include "PndMvdPixelClusterFinder.h"

ClassImp(PndMvdPixelClusterFinder);

void PndMvdPixelClusterFinder::Print(std::vector<PndMvdDigiPixel> hits)
{
	std::cout << "--------------" << std::endl;
	for (Int_t i = 0; i < hits.size(); i++){
		std::cout << "DetName: " << hits[i].GetDetName() << std::endl;
		std::cout << "FE col/row: " << hits[i].GetFE() << " / " << hits[i].GetPixelColumn() << " / " << hits[i].GetPixelRow() << std::endl;
	}
}

void PndMvdPixelClusterFinder::Print()
{
	Print(fHits);
}

void PndMvdPixelClusterFinder::PrintResult(std::vector<std::vector< Int_t> > clusters)
{
	for (Int_t i = 0; i < clusters.size(); i++){
		std::cout << "PndMvdPixelClusterFinder::PrintResult(): Cluster " << i << ": " << std::endl;
		for (Int_t j = 0; j < clusters[i].size();j++)
			fHits[clusters[i][j]].Print();
	}
}
