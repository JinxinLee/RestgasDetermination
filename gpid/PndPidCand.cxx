#include "PndPidCand.h"
ClassImp(PndPidCand);

PndPidCand::PndPidCand()
{
}

PndPidCand::~PndPidCand()
{
  destroy();
}

void PndPidCand::destroy()
{
  fParam.clear();
  fVarName.clear(); 
}


void PndPidCand::Set(const string key, const double value)
{
  fParam[key] = value;
}

double PndPidCand::Get(const string key)
{
  return fParam.find(key)->second;
}


vector<string> PndPidCand::GetVarName()
{
  std::map<string,double>::iterator fIter;
  for(fIter = fParam.begin(); fIter != fParam.end();fIter++)
    {
      fVarName.push_back((*fIter).first); 
    }
  return fVarName;
}

