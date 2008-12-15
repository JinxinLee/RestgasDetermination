#ifndef PNDGPIDTASKFAST_HH
#define PNDGPIDTASKFAST_HH 1 

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
#include "PndMicroCandidate.h"

using namespace std;

class TObjectArray;
class TClonesArray;
class Reader;


class PndGpidTaskFast : public CbmTask
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
  PndGpidTaskFast();

  /** Destructor **/
  virtual ~PndGpidTaskFast();
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

  void Config();      // function to configure the Classifier with proper class names and variable names

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
  TClonesArray* fArrCand;
  TClonesArray* fPndTrackArr; 
 // TClonesArray* fArrMvd;
 // TClonesArray* fArrPid; 
ClassDef(PndGpidTaskFast,1);
};

#endif

