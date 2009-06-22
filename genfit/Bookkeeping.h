


#ifndef BOOKKEEPING_H
#define BOOKKEEPING_H

#include"TObject.h"
#include<vector>
#include<cassert>
#include<iostream>

class Bookkeeping : public TObject {
 private:
  std::vector<int> _failedDetId;
  std::vector<int> _failedHitId;
  std::vector<int> _outlierDetId;
  std::vector<int> _outlierHitId;
 public:
  Bookkeeping(){}
  void addFailed(unsigned int hit,unsigned int detId){
    _failedDetId.push_back(detId);
    _failedHitId.push_back(hit);
  }
  void addOutlier(unsigned int hit,unsigned int detId){
    _outlierDetId.push_back(detId);
    _outlierHitId.push_back(hit);
  }
  void clear(){
    _failedDetId.clear();
    _failedHitId.clear();
    _outlierDetId.clear();
    _outlierHitId.clear();
  }
  int getNumFailed(int detId=-1);

  int getNumOutlier(int detId=-1);

  void print();

 public:
  ClassDef(Bookkeeping,1)

};

#endif
