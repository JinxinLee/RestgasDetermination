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
 * Modified:
 * M. Babai
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
 public:
 //Constructor
  PndMultiClassMVA();
  
  //Destructor
  ~PndMultiClassMVA();
  //Adds variables to be used as features
  void AddVar(const string varName); 
  //Add class names
  void AddClass(const string className);
  // void GenerateTree();
  // void AddInFile(string className,string simFileName,string recoFileName);
  //Write the configuration file, with the same nams as the chosen 
  // application
  void WriteConfigFile();
  //  data modifiers
  void SetINFILENAME(const TString fname)  { fINFILENAME = fname; }
  void SetConfigFileName(const TString fname)  { fConfigFileName = fname; }
  void SetAPPNAME(const TString anaName)  { fAPPNAME = anaName; }
  // Select number of signal and background events to be used for
  //training and testing
  void SetNSigTrain(const TString sigTrain) { fNSigTrain = sigTrain; }
  void SetNSigTest(const TString sigTest) { fNSigTest = sigTest; }
  void SetNBkgTrain(const TString bkgTrain) { fNBkgTrain = bkgTrain; }
  void SetNBkgTest(const TString bkgTest) { fNBkgTest = bkgTest; }
  // Set the classifier properties. Note that there are different 
  // functions and options for different classifiers. For the available
  // options see the TMVA manuals.
  // BDT Parameters
  void SetPruneStrengthBDT(const TString PruneStrength) { fPruneStrengthBDT = PruneStrength; }
  void SetNTreeBDT(const TString nTree) { fNTreeBDT = nTree; }
  void SetBoostTypeBDT(const TString boostType) {fBoostTypeBDT = boostType; }
  void SetNCutsBDT(const TString nCuts) { fNCutsBDT = nCuts; }
  // KNN Parameters
  void SetNKNN(const TString kNN) { fNKNN = kNN; }

   //MLP Parameters
   // "Normalise:H:!V:NeuronType=tanh:NCycles=200:HiddenLayers=N+1,N:TestRate=5"
   void SetMLPNeuronType(const TString NtMLP){
   	//mlpNeuTyp = NtMLP;
   	mlpNeuTyp = "tanh";//DEBUG
   	};
   void SetNuOfCycle(const int NumCycle){
   	std::stringstream out;
   	out << NumCycle;
   	//mlpCycle = out.str();
   	mlpCycle = "10";//DEBUG
   	};
   void SetNumOfHiddenLayers(const int NumHidLayer){
   	std::stringstream out;
   	out << NumHidLayer;
   	//mlpNumHidden = out.str();
   	mlpNumHidden = "N=1,N";//DEBUG
   	};
   void SetTestRate(const int TestR){
   	std::stringstream out;
   	out << TestR;
   	//mlpTestRate = out.str();
   	mlpTestRate = "5";//DEBUG
   	};
   
  //data accessers
  //Int_t GetNCLASS() {return fNCLASS; }
  Int_t GetNCLASS() {return fClassNameArray.size(); }
  //Int_t GetNVAR() {return fNVAR; }
  Int_t GetNVAR() {return fVarNameArray.size(); }
  void TrainTest();       
  
 private:
  //Classifier train and test parameters
  Int_t fNCLASS;        //  number of classes 
  Int_t fNVAR;          //  number of Variables
  TString fNSigTrain;   //  number of signals for training
  TString fNSigTest;    //  number of signals for testing
  TString fNBkgTrain;   //  number of background for Training
  TString fNBkgTest;    //  number of background for testing
  
  //  pruning strength for BDT (removing statistically insignificant nodes)
  TString fPruneStrengthBDT;
  TString fBoostTypeBDT;     //  boost type (Ada boost or bagging)   
  TString fNCutsBDT;         //  number of cuts to create a tree
  TString fNTreeBDT;         //  number of trees to create a forest
  //KNN
  TString fNKNN;             //  number of nearest neighbours
  //MLP
  TString mlpNeuTyp; //MLP neuron type
  TString mlpCycle;  //MLP number of cycles
  TString mlpNumHidden; //MLP number of hidden layers
  TString mlpTestRate; //MLP Test rate
  
  //Clasifier config params
  TString fINFILENAME;       //  input file containing trees of all the signals
  TString fAPPNAME;          //  name of the application 
  TString fConfigFileName;   //  name of the configuration file
  vector <string> fVarNameArray;      // array of Variable names 
  vector <string> fClassNameArray;    // array of class names

  // map <string, vector<pair<string,string> > > fInFileNameArray;  
  // map from class names the the corresponding 
  
  //TFile fINPUT;
};
#endif
