#pragma once
#ifndef EVENT_H
#define EVENT_H

//C++ includes
#include <map>
#include <string>
#include <vector>
#include <iostream> 

//ROOT includes
#include "TObject.h"

using namespace std;

typedef std::map<string,double> Param;

class Event : public TObject
{
 public:
  Event();
  ~Event();
  void Set(const string key, const double value);
  double Get(const string key);
  
  Param GetParam();
  
  //vector<string> GetVarName();
  void GetVarName(vector<string>& outPut);
  
  //protected:
 private:
  void destroy();
  Param fParam;
  std::vector<string> fVarName;  
  
  ClassDef(Event,1);
};
#endif
