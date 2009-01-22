#include "Event.h"

Event::Event()
{
fParam.clear();
}

Event::~Event()
{
 fParam.clear();
 fVarName.clear();  
 //destroy();
}
/*
void Event::destroy()
{
}
*/

void Event::Set(const string key,double value)
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
vector<string> Event::GetVarName()
{
    std::map<string,double>::iterator fIter;
    for(fIter = fParam.begin(); fIter != fParam.end();fIter++)
     {
      fVarName.push_back((*fIter).first);
      cout<<(*fIter).first<<" =>"<<(*fIter).second<<endl; 
     }
     return fVarName;
}

ClassImp(Event)
