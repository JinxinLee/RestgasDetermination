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

#include "Event.h"

Event::Event()
{
  fParam.clear();
}

Event::~Event()
{
  destroy();
}

void Event::destroy()
{
  fParam.clear();
  fVarName.clear();  
}

void Event::Set(const string key, const double value)
{
  fParam[key] = value;
}

double Event::Get(const string key)
{
  return fParam.find(key)->second;
}

inline Param Event::GetParam()
{
  return fParam;
}
/*
  Param* Event::GetParam()
  {
  return (new Param(fParam));
  }
*/
void Event::GetVarName(vector<string>& outPut)
{
  std::map<string,double>::iterator fIter;
  for(fIter = fParam.begin(); fIter != fParam.end();fIter++){
    outPut.push_back((*fIter).first);
    //cout<<(*fIter).first<<" =>"<<(*fIter).second<<endl; 
  }
}

ClassImp(Event);
