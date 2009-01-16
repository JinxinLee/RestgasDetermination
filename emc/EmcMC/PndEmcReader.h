// -------------------------------------------------------------------------
// -----                     PndEmcReader header file                  -----
// -----               Created 14/08/06  by S.Spataro                  -----
// -------------------------------------------------------------------------
#pragma once
#ifndef PNDEMCREADER_H
#define PNDEMCREADER_H

#include <vector>
#include "TString.h"

class DataG4
{

public:
    DataG4(){}
    ~DataG4(){}
    
    int crystal, row, module;
    double theta, phi, tau;
    double posX, posY, posZ;
    double pDz, pTheta, pPhi, pDy1, pDx1, pDx2, pAlp1, pDy2, pDx3, pDx4, pAlp2;
    
};

class PndEmcReader
{

public:

  PndEmcReader(TString name);
    ~PndEmcReader();
    
    std::vector<DataG4> g4data;
    
    TString sName;
    void fill_vector();
    int GetMaxModules();
    int GetMaxRows(int module);
    int GetMaxCrystals(int module, int row);
    int GetMinModules();
    int GetMinRows(int module);
    int GetMinCrystals(int module, int row);
    DataG4 GetData(int module, int row, int crystal);  
};
#endif //PNDEMCREADER_H
