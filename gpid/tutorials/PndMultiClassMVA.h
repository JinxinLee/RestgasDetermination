/*
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
 * Classification purpose. This part is the learning  part of 
 * classifier. The learning will be done for the methods KNN,
 * MLP and BDT.This Implementation has function calls
 * to do the configuration for different algorithem. 
 *
 *                  S.Vanniarajan  01-08-08
 */

#ifndef MULTI_CLASS_MVA_H
#define MULTI_CLASS_MVA_H 1 

#include <iostream>
#include <sstream>
#include "TCut.h"
#include "TFile.h"
#include "TString.h"
#include "TSystem.h"
#include "TTree.h"
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "Factory.h" 
using namespace std;

class PndMultiClassMVA
{
 private:
       Int_t fNCLASS;
       Int_t fNVAR;
       TString fNSigTrain;
       TString fNSigTest;
       TString fNBkgTrain;
       TString fNBkgTest;
       TString fPruneStrengthBDT;
       TString fBoostTypeBDT;
       TString fNCutsBDT;
       TString fNTreeBDT;
       TString fINFILENAME;
       TString fNKNN;
       TString fAPPNAME;
       TString fConfigFileName;
       vector <string> fVarNameArray;
       vector <string> fClassNameArray;
       map <string, vector<pair<string,string> > > fInFileNameArray;
       TFile fINPUT;
 public:
       PndMultiClassMVA();
       ~PndMultiClassMVA();
       void AddVar(string varName); 
       void AddClass(string className);
//       void GenerateTree();
//       void AddInFile(string className,string simFileName,string recoFileName);
       void WriteConfigFile();
//       void SetNCLASS(Int_t NoClass ) { fNCLASS = NoClass;}
//       void SetNVAR(Int_t NoPar ) { fNVAR = NoPar;}
       void SetINFILENAME(TString fname)  { fINFILENAME = fname; }
       void SetConfigFileName(TString fname)  { fConfigFileName = fname; }
       void SetAPPNAME(TString anaName)  { fAPPNAME = anaName; }
       void SetNSigTrain(TString sigTrain) { fNSigTrain = sigTrain; }
       void SetNSigTest(TString sigTest) { fNSigTest = sigTest; }
       void SetNBkgTrain(TString bkgTrain) { fNBkgTrain = bkgTrain; }
       void SetNBkgTest(TString bkgTest) { fNBkgTest = bkgTest; }
       void SetPruneStrengthBDT(TString PruneStrength) { fPruneStrengthBDT = PruneStrength; }
       void SetNTreeBDT(TString nTree) { fNTreeBDT = nTree; }
       void SetBoostTypeBDT(TString boostType) {fBoostTypeBDT = boostType; }
       void SetNCutsBDT(TString nCuts) { fNCutsBDT = nCuts; }
       void SetNKNN(TString kNN) { fNKNN = kNN; }
       Int_t GetNCLASS() {return fNCLASS; }
       Int_t GetNVAR() {return fNVAR; }
       void TrainTest();       
};

#endif
