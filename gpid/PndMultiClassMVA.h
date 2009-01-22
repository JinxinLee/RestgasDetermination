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
#pragma once
#ifndef MULTI_CLASS_MVA_H
#define MULTI_CLASS_MVA_H 
//C++ headers
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
//Root & PandaRoot headers
#include "TCut.h"
#include "TFile.h"
#include "TString.h"
#include "TSystem.h"
#include "TTree.h"
#include "Factory.h"

using namespace std;

class PndMultiClassMVA
{
 private:
       Int_t fNCLASS;        //  number of classes 
       Int_t fNVAR;          //  number of Variables
       TString fNSigTrain;   //  number of signals for training
       TString fNSigTest;    //  number of signals for testing
       TString fNBkgTrain;   //  number of background for Training
       TString fNBkgTest;    //  number of background for testing
       TString fPruneStrengthBDT; //  pruning strength for BDT (removing statistically insignificant nodes)
       TString fBoostTypeBDT;     //  boost type (Ada boost or bagging)   
       TString fNCutsBDT;         //  number of cuts to create a tree
       TString fNTreeBDT;         //  number of trees to create a forest
       TString fINFILENAME;       //  input file containing trees of all the signals
       TString fNKNN;             //  number of nearest neighbours
       TString fAPPNAME;          //  name of the application 
       TString fConfigFileName;   //  name of the configuration file
       vector <string> fVarNameArray;      // array of Variable names 
       vector <string> fClassNameArray;    // array of class names
     //  map <string, vector<pair<string,string> > > fInFileNameArray;  // map from class names the the corresponding 
       TFile fINPUT;
 public:
       PndMultiClassMVA();
       ~PndMultiClassMVA();
       void AddVar(string varName); 
       void AddClass(string className);
//       void GenerateTree();
//       void AddInFile(string className,string simFileName,string recoFileName);
       void WriteConfigFile();
//  data modifiers
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
//data accessers
       Int_t GetNCLASS() {return fNCLASS; }
       Int_t GetNVAR() {return fNVAR; }
       void TrainTest();       
};

#endif
