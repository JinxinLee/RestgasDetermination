#ifndef TPCTESTCHAMBEROUTPUTTASK_HH
#define TPCTESTCHAMBEROUTPUTTASK_HH

#include "FairTask.h"
#include"TTree.h"

class TClonesArray;


struct multitree_t{
   UInt_t          trigger;
  //Int_t           chan[128];   //[nchan]
  //Int_t           nslice[128];   //[nchan]
   Int_t           charge[128][150];   //[nchan]
  //Int_t           time[128][150];   //[nchan]
  Double_t MCTax;
  Double_t MCTbx;
  Double_t MCTay;
  Double_t MCTby;
};

class PndTpcTestChamberOutputTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcTestChamberOutputTask();
  ~PndTpcTestChamberOutputTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetSampleBranchName(const TString& name) {fsampleBranchName=name;}
  void SetOutfileName(const TString& fs){ffilename=fs;}
  // Operations ----------------------


  void writeToFile();
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  virtual void SetParContainers();

private:

  // Private Data Members ------------
  TString fsampleBranchName;
  
  TClonesArray* fsampleArray;
  TClonesArray* MCTarray;

  multitree_t outstruct;
  TTree *outtree;
  TFile *outfile;
  TString ffilename;

public:
  ClassDef(PndTpcTestChamberOutputTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
