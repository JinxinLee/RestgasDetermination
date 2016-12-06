#include "PndMdtCluster.h"

// -----   Default constructor   -------------------------------------------
PndMdtCluster::PndMdtCluster() : fDigiList()
{
}


PndMdtCluster::PndMdtCluster(std::vector<Int_t> list) :
  fDigiList(list)
{
}

/** Destructor **/
PndMdtCluster::~PndMdtCluster() 
{
} 

Bool_t PndMdtCluster::DigiBelongsToCluster(Int_t digiIndex)
{
  for (unsigned int i = 0; i < fDigiList.size(); i++)
    if (fDigiList[i] == digiIndex) return true;
  
  return false;
}

void PndMdtCluster::Print(const Option_t* opt)
{
  std::cout << *this;
}

ClassImp(PndMdtCluster)
