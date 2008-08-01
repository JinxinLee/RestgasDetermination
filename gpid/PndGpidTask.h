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

#ifndef PNDGPIDTASK_HH
#define PNDGPIDTASK_HH 1 

#include "CbmTask.h"
#include "TVector3.h"
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include "TClonesArray.h"
#include "CbmRootManager.h"
#include "TMVA/Reader.h"
using namespace std;

class TObjectArray;
class TClonesArray;
class Reader;


class PndGpidTask : public CbmTask
{
 public:

  enum MVAType{
       KNN = 1,
       BDT = 2,
       MLP = 3,
       };

  void SetMVA(MVAType mode) { fMVAmode = mode; } 
  /** Default constructor **/  
  PndGpidTask();

  /** Destructor **/
  virtual ~PndGpidTask();
  void SetAPPNAME(TString anaName)  { fAPPNAME = anaName; }
  void SetDIR(string dir) {fDIR = dir;}

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
private:
  void Config();
  void AddVar();
  void BookingMVA();
  MVAType fMVAmode;
  std::string fAPPNAME;
  std::string fDIR;
  std::string fClassifier;
  vector<string> fVarNameArray;
  vector<string> fClassNameArray;
  
  TMVA::Reader reader[10]; 
  Int_t fNVAR;
  Int_t fNCLASS;
  float varArray[100];
//  std::vector <float> varArray;
//  std::vector <float>::iterator FlIt;  
  TClonesArray* fEvtArray;   
ClassDef(PndGpidTask,1);
};

#endif
