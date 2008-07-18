#include "PndMvdIdealPixelClusterFinder.h"
#include "TMath.h"

std::vector< std::vector<Int_t> > PndMvdIdealPixelClusterFinder::GetClusters()
{
  if (fVerbose > 1)
    Print();
  std::vector<Int_t> posHits; // stores the position of the hits in the TClonesArray (only this information is stored)
  for (Int_t i = 0; i < fHits.size(); i++) posHits.push_back(i);
  std::vector< std::vector< Int_t> > result;
  Int_t sizeTempHits = posHits.size();
  Int_t actHit = 0;
  
  while (sizeTempHits != 0){
    std::vector<Int_t> tempInt;
    if (fHits[posHits[0]].GetCharge() == 0){  //if the hit has no charge it is deleted out of posHits
      if (fVerbose > 2){
        std::cout << "Charge too low for pixel: " << std::endl;
        fHits[posHits[0]].Print();
      }
      MoveHit(&posHits,0);
    }
    else {
      tempInt.push_back(MoveHit(&posHits,0));   //Move the first hit of the remaining hits into the new cluster
      result.push_back(tempInt);
      
      sizeTempHits = posHits.size();
      for (Int_t j = 0; j < sizeTempHits; j++){
        if (fHits[posHits[j]].GetCharge() == 0){
          MoveHit(&posHits,j);
          if (fVerbose > 2){
            std::cout << "Charge too low for pixel: " << std::endl;
            fHits[posHits[0]].Print();
          }
          j--;
        }
        else{
          if (fHits[(result.end()-1)->at(0)].GetDetName() == fHits[posHits[j]].GetDetName()){
            if (IsInRange(fHits[(result.end()-1)->at(0)], fHits[posHits[j]])) {
                (result.end()-1)->push_back(MoveHit(&posHits,j)); // the hit position is added to the current cluster
                j--;
                if (fVerbose > 2)
                  std::cout << "Hit added to cluster: " << result.size()-1 << std::endl;
            }
          }
          
        }
        sizeTempHits = posHits.size();
      }
    }
  }
  if (fVerbose > 1)
    PrintResult(result);
  return result;
}

Int_t PndMvdIdealPixelClusterFinder::MoveHit(std::vector<Int_t>* hitVector, Int_t index)
{
  Int_t result;
  if (index < hitVector->size()){
    result = hitVector->at(index);
    hitVector->erase(hitVector->begin()+index);
  }
  return result;
}

bool PndMvdIdealPixelClusterFinder::IsInRange(PndMvdDigiPixel hit1, PndMvdDigiPixel hit2)
{
  return (hit1.GetIndex(0) == hit2.GetIndex(0)); // two hits are in range if they come from the same MC hit
}
