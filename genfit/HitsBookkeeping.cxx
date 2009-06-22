#include"HitsBookkeeping.h"

int Bookkeeping::getNumFailed(int detId){
  assert(_failedDetId.size()==_failedHitId.size());
  if(detId==-1){
    return _failedDetId.size();
  }
  int retVal=0;
  for(int i=0;i<_failedDetId.size();++i){
    if(detId==_failedDetId.at(i)) retVal+=1;
  }
  return retVal;
}

int Bookkeeping::getNumOutlier(int detId){
  assert(_outlierDetId.size()==_outlierHitId.size());
  if(detId==-1){
    return _outlierDetId.size();
  }
  int retVal=0;
  for(int i=0;i<_outlierDetId.size();++i){
    if(detId==_outlierDetId.at(i)) retVal+=1;
  }
  return retVal;
}


void Bookkeeping::print(){
  std::cout << "Bookkeeping::print()" << std::endl;
  std::cout << "failedHits ";
  assert(_failedDetId.size()==_failedHitId.size());
  for(unsigned int i=0;i<_failedDetId.size();++i){
    std::cout << _failedDetId.at(i)<<"|"<<_failedHitId.at(i)<<" ";
  }
  std::cout << std::endl;
  std::cout << "outlierHits ";
  assert(_outlierDetId.size()==_outlierHitId.size());
  for(unsigned int i=0;i<_outlierDetId.size();++i){
    std::cout << _outlierDetId.at(i)<<"|"<<_outlierHitId.at(i)<<" ";
  }
  std::cout << std::endl;
}


ClassImp(Bookkeeping)
