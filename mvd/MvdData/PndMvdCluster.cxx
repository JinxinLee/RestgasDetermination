#include "PndMvdCluster.h"

PndMvdCluster::PndMvdCluster(std::vector<Int_t> list)
{
	fClusterList = list;
}

void PndMvdCluster::Print()
{
	std::cout << *this;
}

bool PndMvdCluster::DigiBelongsToCluster(Int_t digiIndex)
{
	for (int i = 0; i < fClusterList.size(); i++)
		if (fClusterList[i] == digiIndex) return true;

	return false;
}


ClassImp(PndMvdCluster);

