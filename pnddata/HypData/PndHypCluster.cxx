#include "PndHypCluster.h"

PndHypCluster::PndHypCluster(std::vector<Int_t> list) :
	fClusterList(list),fSide(kTOP)
{
}

void PndHypCluster::Print()
{
	std::cout << *this;
}

bool PndHypCluster::DigiBelongsToCluster(Int_t digiIndex)
{
  for (int i = 0; i < fClusterList.size(); i++)
    if (fClusterList[i] == digiIndex) return true;
  
  return false;
}

ClassImp(PndHypCluster)
