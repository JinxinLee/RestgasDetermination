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
 * Modified:
 * M. Babai
 */
#pragma once
#ifndef PNDGPIDTASKLHE_H
#define PNDGPIDTASKLHE_H

#ifndef NUM_TMVA_READERS
#define NUM_TMVA_READERS 10
#endif

//C++ includes
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>

//ROOT and PandaRoot includes
#include "FairTask.h"
#include "PndLhePidTrack.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "FairRootManager.h"
#include "TMVA/Reader.h"
#include "PndMvdMCPoint.h"
#include "PndTpcPoint.h"
#include "PndPidCand.h"

// Gpid data types
#include "PndGpidTypes.h"
#include "PndLVQClassify.h"//LVQ1 implementation

using namespace std;

class TObjectArray;
class TClonesArray;
class Reader;

class PndGpidTaskLhe : public FairTask
{
 public:

  /** Default constructor **/  
  PndGpidTaskLhe();
  
  /** Destructor **/
  virtual ~PndGpidTaskLhe();
  
  // Data modifiers
  void SetAPPNAME(TString anaName)  { fAPPNAME = anaName; };
  void SetDIR(string dir) {fDIR = dir;};
  void SetMVA(MVAType mode) {fMVAmode = mode;};

  void SetInFileName(char* InPut) {M_InFileName = InPut;} ;
  
  /** Virtual method Init **/
  virtual InitStatus Init();
  
  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  protected:
  inline   void printResult(std::map<std::string,float>& res){
    std::cout << "\n===== LVQ output For debugging ========== \n";
    for( std::map<std::string,float>::iterator ii=res.begin(); 
	 ii != res.end(); ++ii){
      std::cout << (*ii).first << " => " << (*ii).second << std::endl;
    }
    std::cout << "===== LVQ output For debugging ========== \n";
  }

 private:
  // Private fuction members called by init during Initialization

  // function to configure the Classifier with proper class names and
  // variable names
  void Config();

  void AddVar(); // function adding the variables for classification 
  
  void BookingMVA();  // Book the MVA Set by the usr.
  
  // MVAType booked 
  MVAType fMVAmode;
  
  // Name of the application for which one does the classification
  // used to pickup the correct configuration file and weight file.
  std::string fAPPNAME;
  
  // locate the directory for the weight files
  std::string fDIR;
  
  // string internally used to select the methed
  std::string fClassifier;
  
  // Array of Variable names for classification names 
  std::vector<std::string> fVarNameArray;
  
  // Array of Class names for classification names 
  std::vector<std::string> fClassNameArray;
  
  TMVA::Reader reader[NUM_TMVA_READERS];
  int fNVAR;
  int fNCLASS;

  TString M_InFileName;
  std::vector<float> m_varVec;
  PndLVQClassify* m_lvq;
  
  TClonesArray* fPidTrackCand;  
  TClonesArray* fArrPid; 
  ClassDef(PndGpidTaskLhe,1);
};
#endif
