#ifndef EVENT_HH
#define EVENT_HH 1


#include <map>
#include <string>
#include <vector>
#include "TObject.h"
#include <iostream> 

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
