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
  m_clsVal.clear();
}


void PndPidCand::SetVarVal(const std::string key, const float value)
{
  fParam[key] = value;
}

void PndPidCand::SetClsVal(const std::string key, const float value)
{
  m_clsVal[key] = value;
}

float PndPidCand::GetVarVal(const std::string key)
{
  return fParam.find(key)->second;
}

float PndPidCand::GetClsVal(const std::string key)
{
  return m_clsVal.find(key)->second;
}

void PndPidCand::GetVarName(std::vector<std::string>& varList)
{
  varList.clear();
  std::map<string, float>::iterator fIter;
  for(fIter = fParam.begin(); fIter != fParam.end();fIter++){
    varList.push_back((*fIter).first); 
  }
}

void PndPidCand::GetClsName(std::vector<std::string>& clsList)
{
  clsList.clear();
  std::map<string, float>::iterator fIter;
  for(fIter = m_clsVal.begin(); fIter != m_clsVal.end(); fIter++){
    clsList.push_back((*fIter).first);
  }
}
ClassImp(PndPidCand);
