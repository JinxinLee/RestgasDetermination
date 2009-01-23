/*
 *  
 *  S.Vanniarajan  V.Suyam.Jothi@kvi.nl
 *
 *  This is the Mutivariate Event classification tool
 *  designed for PANDAROOT Analysis package for 
 *  PANDA Experiments.
 *       TMVA(Toolkit for MultiVariate Analysis) is a 
 *  two class classifier(signal and background). 
 *  available with ROOT package. mainly used for 
 *  Event Classification in High Energy Physics Experiments.
 *         
 *       This tool here is Designed from TMVA, for Multiclass 
 * Classification purpose. 
 *
 *                  S.Vanniarajan  01-08-08
 * Modified:
 * M. Babai
 */
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
  
  //We need to change this one.
  inline Param GetParam();
  //Param*  GetParam();
  
  void GetVarName(vector<string>& outPut);
  
  //protected:
  
 private:
  void destroy();
  Param fParam;
  std::vector<string> fVarName;  
  
  ClassDef(Event,1);
};
#endif
