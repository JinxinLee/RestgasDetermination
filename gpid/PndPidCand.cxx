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

#include "PndPidCand.h"

PndPidCand::PndPidCand()
{}

PndPidCand::~PndPidCand()
{
  destroy();
}

void PndPidCand::destroy()
{
  fParam.clear();
  fVarName.clear(); 
}

void PndPidCand::Set(const string key, const float value)
{
  fParam[key] = value;
}

double PndPidCand::Get(const string key)
{
  return fParam.find(key)->second;
}

void PndPidCand::GetVarName(vector<string>& varList)
{
  std::map<string, float>::iterator fIter;
  for(fIter = fParam.begin(); fIter != fParam.end();fIter++){
    varList.push_back((*fIter).first); 
  }
}
ClassImp(PndPidCand);
