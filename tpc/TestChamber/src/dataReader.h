//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Jun 15 17:58:22 2007 by ROOT version 5.14/00
// from TTree datatree_m/Tree for multifit
// found on file: multi_tpc_data_01_Mar_2007_09_58_53.root
//////////////////////////////////////////////////////////

#ifndef dataReader_h
#define dataReader_h

#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"

#include<map>

class TCevt {
 public:
  UInt_t          trigger;
  //UInt_t          runtime;
  //Int_t           nchan;
  //Int_t           chan[128];   //[nchan]
  //Int_t           nslice[128];   //[nchan]
  Int_t           charge[128][150];   //[nchan]
  //Int_t           time[128][150];   //[nchan]
  //Float_t         mc_intercept_x;
  //Float_t         mc_slope_x;
  //Float_t         mc_intercept_z;
  //Float_t         mc_slope_z;


  void Print(); 
  void Print(int);


  int Nchan(std::map<int,double>&);

};

class dataReader {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leave types
   UInt_t          trigger;
   //UInt_t          runtime;
   //Int_t           nchan;
   //Int_t           chan[128];   //[nchan]
   //Int_t           nslice[128];   //[nchan]
   Int_t           charge[128][150];   //[nchan]
   //Int_t           time[128][150];   //[nchan]
   //Float_t         mc_intercept_x;
   //Float_t         mc_slope_x;
   //Float_t         mc_intercept_z;
   //Float_t         mc_slope_z;

   // List of branches
   TBranch        *b_trigger;   //!
   //TBranch        *b_runtime;   //!
   //TBranch        *b_nchan;   //!
   //TBranch        *b_chan;   //!
   //TBranch        *b_nslice;   //!
   TBranch        *b_charge;   //!
   //TBranch        *b_time;   //!
   //TBranch        *b_mc_intercept_x;   //!
   TBranch        *b_mc_slope_x;   //!
   //TBranch        *b_mc_intercept_z;   //!
   //TBranch        *b_mc_slope_z;   //!

   dataReader(TTree *tree=0);
   virtual ~dataReader();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   TCevt*   readEvent();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   
 private:
   int lastEvent;
};

#endif

#ifdef dataReader_cxx
dataReader::dataReader(TTree *tree)
{
  lastEvent = 0;
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("multi_tpc_data_01_Mar_2007_09_58_53.root");
      if (!f) {
         f = new TFile("multi_tpc_data_01_Mar_2007_09_58_53.root");
      }
      tree = (TTree*)gDirectory->Get("datatree_m");

   }
   Init(tree);
}

dataReader::~dataReader()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t dataReader::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t dataReader::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (!fChain->InheritsFrom(TChain::Class()))  return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void dataReader::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normaly not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("trigger", &trigger, &b_trigger);
   //fChain->SetBranchAddress("runtime", &runtime, &b_runtime);
   //fChain->SetBranchAddress("nchan", &nchan, &b_nchan);
   //fChain->SetBranchAddress("chan", chan, &b_chan);
   //fChain->SetBranchAddress("nslice", nslice, &b_nslice);
   fChain->SetBranchAddress("charge", charge, &b_charge);
   //fChain->SetBranchAddress("time", time, &b_time);
   //fChain->SetBranchAddress("mc_intercept_x", &mc_intercept_x, &b_mc_intercept_x);
   //fChain->SetBranchAddress("mc_slope_x", &mc_slope_x, &b_mc_slope_x);
   //fChain->SetBranchAddress("mc_intercept_z", &mc_intercept_z, &b_mc_intercept_z);
   //fChain->SetBranchAddress("mc_slope_z", &mc_slope_z, &b_mc_slope_z);
   Notify();
}

Bool_t dataReader::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normaly not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void dataReader::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t dataReader::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef dataReader_cxx
 
 
