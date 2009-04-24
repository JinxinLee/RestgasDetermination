


#ifndef FAILEDHITS_H
#define FAILEDHITS_H

#include"TObject.h"
#include<vector>
#include<cassert>
#include<iostream>

class FailedHits : public TObject {
 private:
  std::vector<int> _detId;
  std::vector<int> _hitId;
 public:
  FailedHits(){}
  void add(unsigned int hit,unsigned int detId){
    _detId.push_back(detId);
    _hitId.push_back(hit);
  }
  void clear(){
    _detId.clear();
    _hitId.clear();
  }
  int getNum(int detId=-1){
    assert(_detId.size()==_hitId.size());
    if(detId==-1){
      return _detId.size();
    }
    int retVal=0;
    for(int i=0;i<_detId.size();++i){
      if(detId==_detId.at(i)) retVal+=1;
    }
    return retVal;
  }

 public:
  ClassDef(FailedHits,1)

};

#endif
