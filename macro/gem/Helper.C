//make a library of that with  "root -q Helper.C+"
//load inside macros with gSystem->Load("Helper_C.so")
#include <vector>
#include <map>
#ifdef __MAKECINT__
#pragma link C++ class vector<bool>+;
#pragma link C++ class vector<int>+;
#pragma link C++ class vector<double>+;

#pragma link C++ class vector<vector<double> >+;
#pragma link C++ class vector<vector<int> >+;

#pragma link C++ class map<int,bool>+;
#pragma link C++ class map<int,int>+;
#pragma link C++ class map<int,double>+;

#pragma link C++ class map<int,vector<int> >+;
#endif

int Helper(){return 0;}
