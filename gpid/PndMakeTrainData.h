#ifndef PND_MAKE_TRAIN_DATA_HH
#define PND_MAKE_TRAIN_DATA_HH 1

#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

class PndMakeTrainData  {
  private:
    int fNCLASS;
    map <string, vector<pair<string,string> > > fInFileNameArray;
    vector <string> fClassNameArray;
  public:
    PndMakeTrainData();
    ~PndMakeTrainData();
    void GenerateTree();
    void AddInFile(string className,string simFileName,string recoFileName);
};
#endif

