/*
 *  
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
 * Classification purpose. 
 *
 *                  S.Vanniarajan  01-08-08
 */
#pragma once
#ifndef PNDGPIDTASK_H
#define PNDGPIDTASK_H

#include "CbmTask.h"
#include "PndLhePidTrack.h"
#include "TVector3.h"
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include "TClonesArray.h"
#include "CbmRootManager.h"
#include "TMVA/Reader.h"
#include "PndMvdMCPoint.h"
#include "PndTpcPoint.h"
#include "PndPidCand.h"

using namespace std;

class TObjectArray;
class TClonesArray;
class Reader;


class PndGpidTask : public CbmTask
{
 public:
 // enum type for the type of classifier 
 // task is going to use
  enum MVAType{
       KNN = 1,
       BDT = 2,
       MLP = 3,
       };


  /** Default constructor **/  
  PndGpidTask();

  /** Destructor **/
  virtual ~PndGpidTask();
  // Data modifiers
  void SetAPPNAME(TString anaName)  { fAPPNAME = anaName; }
  void SetDIR(string dir) {fDIR = dir;}
  void SetMVA(MVAType mode) { fMVAmode = mode; } 

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

private:
  // Private fuction members called by init during Initialization

  // function to configure the Classifier with proper class names and variable names
  void Config();
  
  void AddVar();      // function adding the variables for classification 
  
  void BookingMVA();  // Book the MVA Set by the usr.
  
  // MVAType booked 
  MVAType fMVAmode;
  
  // Name of the application for which one does the classification used to pickup
  // the correct configuration file and weight file.
  std::string fAPPNAME;
  
  // locate the directory for the weight files
  std::string fDIR;
  
  // string internally used to select the methed
  std::string fClassifier;
  
  // Array of Variable names for classification names 
  vector<string> fVarNameArray;
  
  // Array of Class names for classification names 
  vector<string> fClassNameArray;
  
  TMVA::Reader reader[10]; //! 
  Int_t fNVAR;
  Int_t fNCLASS;
  float varArray[100];
  //  std::vector <float> varArray;
  //  std::vector <float>::iterator FlIt;  
  // TClonesArray* fPidTrackCand;  
  TClonesArray* fArrTpc; 
  TClonesArray* fArrMvd;
  TClonesArray* fArrPid; 
  ClassDef(PndGpidTask,1);
};

#endif
