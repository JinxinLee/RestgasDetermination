#include "PndLumiCluster.h"

PndLumiCluster::PndLumiCluster(std::vector<Int_t> list)
{
	fClusterList = list;
}

void PndLumiCluster::Print()
{
	std::cout << *this;
}

bool PndLumiCluster::DigiBelongsToCluster(Int_t digiIndex)
{
	for (int i = 0; i < fClusterList.size(); i++)
		if (fClusterList[i] == digiIndex) return true;

	return false;
}


ClassImp(PndLumiCluster);

