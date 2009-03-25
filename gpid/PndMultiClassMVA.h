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

// C++ headers
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

// Root & PandaRoot headers
#include "TCut.h"
#include "TFile.h"
#include "TString.h"
#include "TSystem.h"
#include "TTree.h"
#include "Factory.h"

// Global PID data types definitions
#include "PndGpidTypes.h"
#include "PndLVQTrain.h"//LVQ1 implementation

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
  
  // Write the configuration file, with the same nams as the chosen
  // application
  void WriteConfigFile();
  
  //  data modifiers
  void SetINFILENAME(const TString fname)  { fINFILENAME = fname; }
  void SetConfigFileName(const TString fname)  { fConfigFileName = fname; }
  void SetAPPNAME(const TString anaName)  { fAPPNAME = anaName; }

  void SetOutFileName(const TString OutName){ m_OutFileName = OutName; }
  void SetNumLvqProto(const int protos){ m_numLVQProto = protos; }

  // Select number of signal and background events to be used for
  //training and testing
  void SetNSigTrain(const TString sigTrain) { fNSigTrain = sigTrain; }
  void SetNSigTest(const TString sigTest) { fNSigTest = sigTest; }
  void SetNBkgTrain(const TString bkgTrain) { fNBkgTrain = bkgTrain; }
  void SetNBkgTest(const TString bkgTest) { fNBkgTest = bkgTest; }
  
  // Set the classifier properties. Note that there are different 
  // functions and options for different classifiers. For the available
  // options see the TMVA manuals.


  //set the cuts on the parameter
  void SetVarCut(const TCut varCut)
  { fVarCuts = varCut; }

  
  // BDT Parameters
  void SetPruneStrengthBDT(const TString PruneStrength)
  { fPruneStrengthBDT = PruneStrength; }
  void SetNTreeBDT(const TString nTree) { fNTreeBDT = nTree; }
  void SetBoostTypeBDT(const TString boostType) {fBoostTypeBDT = boostType; }
  void SetNCutsBDT(const TString nCuts) { fNCutsBDT = nCuts; }
  
  // KNN Parameters
  //Set number of neighbors
  void SetNKNN(const TString kNN) { fNKNN = kNN; }
  
  //Set the depth of the tree that holds the examples
  void SetKNNTreeOptDepth(const int depth){
    std::stringstream out;
    out << depth;
    mKnnDepth = out.str();
  };
  //Set the scaler fraction
  void SetKNNScaleFrac(const int frac){
    std::stringstream out;
    out << frac;
    mKnnscalefrac = out.str();
  };
  //Set the selection option
  void SetKNNSelOpt(const TString sel){
    mKnnSelOpt = sel;
  };

  //MLP Parameters
  //Sets the type of the neuron of the MLP.
  void SetMLPNeuronType(const TString NtMLP){
    mlpNeuTyp = NtMLP;
  };
  //Set the number of cycles during the training phase.
  void SetNuOfCycle(const int NumCycle){
    std::stringstream out;
    out << NumCycle;
    mlpCycle = out.str();
  };
  //Set the number of hidden layers.
  void SetNumOfHiddenLayers(const TString NumHidLayer){
    mlpNumHidden = NumHidLayer;
  };
  //Set the Test rate of the MLP classifier.
  void SetTestRate(const int TestR){
    std::stringstream out;
    out << TestR;
    mlpTestRate = out.str();
  };
  // #################### End of classifier parameters ######### 
  //data accessers
  Int_t GetNCLASS() {return fClassNameArray.size(); }
  Int_t GetNVAR() {return fVarNameArray.size(); }
  
  //void TrainTest();       

  /* 
   * Select which MVA to train, From TMVA or other implementaions.
   * @param mva: Defines the MVA type to be trained.
   */
  void TrainClassifier(MVAType mva);

  /* This method is implemented because of the fact that the current
   * implementation of TMVA does not support multi class MVA's. Thus we
   * need to train a classifier for each class of objects. This method
   * might disappear if the newer versions of this package support multi
   * class properties.
   * @param mva: Defines the MVA type to be trained.
   * @param config: Defines the configuration string to be used by TMVA.
   */
  void TrainTestTM(MVAType mva, const std::string config);


 private:
  // Classifier train and test parameters
  Int_t fNCLASS;        //  number of classes 
  Int_t fNVAR;          //  number of Variables
  TString fNSigTrain;   //  number of signals for training
  TString fNSigTest;    //  number of signals for testing
  TString fNBkgTrain;   //  number of background for Training
  TString fNBkgTest;    //  number of background for testing
  
  // Pruning strength for BDT (removing statistically insignificant nodes)
  TString fPruneStrengthBDT;
  TString fBoostTypeBDT;     //  boost type (Ada boost or bagging)   
  TString fNCutsBDT;         //  number of cuts to create a tree
  TString fNTreeBDT;         //  number of trees to create a forest
 
  TCut fVarCuts;
  
  // KNN
  TString fNKNN;             //  number of nearest neighbours
  TString mKnnDepth;
  TString mKnnscalefrac;
  TString mKnnSelOpt;

  // MLP
  TString mlpNeuTyp; //MLP neuron type
  TString mlpCycle;  //MLP number of cycles
  TString mlpNumHidden; //MLP number of hidden layers
  TString mlpTestRate; //MLP Test rate
  
  // Clasifier config params
  TString fINFILENAME;  //  input file containing trees of all the signals
  TString fAPPNAME;     //  name of the application 
  TString fConfigFileName;   //  name of the configuration file

  TString m_OutFileName;  // Output file for writing proto type coordinates
  int m_numLVQProto;

  vector <string> fVarNameArray;      // array of Variable names 
  vector <string> fClassNameArray;    // array of class names
};
#endif
