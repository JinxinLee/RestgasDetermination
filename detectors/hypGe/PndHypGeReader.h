// -------------------------------------------------------------------------
// -----                     CbmHypGeReader header file                  -----
// ----- //  roginal Author: S.Spataro              
//------         modified 15/05/07  by A.Sanchez                  -----
// -------------------------------------------------------------------------

#ifndef PNDHYPGEREADER_HH
#define PNDHYPGEREADER_HH 1

#include <vector>
#include "TString.h"
#include "TVector3.h"
using namespace std;

class DataG4
{

public:
    DataG4(){}
    ~DataG4(){}
    
    int crystal;
    double theta, phi, tau;
    double posX, posY, posZ;
        
};

class PndHypGeReader
{

public:

  PndHypGeReader(TString name);
    ~PndHypGeReader();
    
    vector<DataG4> g4data;
    
    TString sName;
    void fill_vector();
    //int GetMaxModules();
    //int GetMaxRows(int module);
    //int GetMaxCrystals(int module, int row);
    DataG4 GetData(int crystal);
    
};

#endif //PNDHYPGEREADER_HH
