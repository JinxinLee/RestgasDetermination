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
private:
 Param fParam;
 std::vector<string> fVarName;  

public:
  Event();
  ~Event();
//void destroy();
 void Set(const string key, double value);
 double Get(const string key);
 Param GetParam();
 vector<string> GetVarName();
 ClassDef(Event,1);
 };
#endif
