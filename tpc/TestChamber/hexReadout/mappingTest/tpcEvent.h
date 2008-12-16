//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Dec 15 19:42:26 2008 by ROOT version 5.18/00
// from TTree datatree/Tree with TPC data
// found on file: tpc_data_2008_12_15_15_09_48.root
//////////////////////////////////////////////////////////

#ifndef tpcEvent_h
#define tpcEvent_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class tpcEvent {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   UInt_t          tTriggerInRun;
   UInt_t          tNumberOfSamples;
   UInt_t          tEntries;
   UInt_t          tChipId[19380];   //[tNumberOfSamples]
   UInt_t          tChannelId[19380];   //[tNumberOfSamples]
   UInt_t          tChannelNb[19380];   //[tNumberOfSamples]
   UInt_t          tAmp[19380];   //[tNumberOfSamples]
   UInt_t          tTime[19380];   //[tNumberOfSamples]

   // List of branches
   TBranch        *b_tTriggerInRun;   //!
   TBranch        *b_tNumberOfSamples;   //!
   TBranch        *b_tEntries;   //!
   TBranch        *b_tChipId;   //!
   TBranch        *b_tChannelId;   //!
   TBranch        *b_tChannelNb;   //!
   TBranch        *b_tAmp;   //!
   TBranch        *b_tTime;   //!

   tpcEvent(TTree *tree=0);
   virtual ~tpcEvent();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual bool     loadNext();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef tpcEvent_cxx
tpcEvent::tpcEvent(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("tpc_data_2008_12_15_15_09_48.root");
      if (!f) {
         f = new TFile("tpc_data_2008_12_15_15_09_48.root");
      }
      tree = (TTree*)gDirectory->Get("datatree");

   }
   Init(tree);
}

tpcEvent::~tpcEvent()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t tpcEvent::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t tpcEvent::LoadTree(Long64_t entry)
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

void tpcEvent::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("tTriggerInRun", &tTriggerInRun, &b_tTriggerInRun);
   fChain->SetBranchAddress("tNumberOfSamples", &tNumberOfSamples, &b_tNumberOfSamples);
   fChain->SetBranchAddress("tEntries", &tEntries, &b_tEntries);
   fChain->SetBranchAddress("tChipId", tChipId, &b_tChipId);
   fChain->SetBranchAddress("tChannelId", tChannelId, &b_tChannelId);
   fChain->SetBranchAddress("tChannelNb", tChannelNb, &b_tChannelNb);
   fChain->SetBranchAddress("tAmp", tAmp, &b_tAmp);
   fChain->SetBranchAddress("tTime", tTime, &b_tTime);
   Notify();
}

Bool_t tpcEvent::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void tpcEvent::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t tpcEvent::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef tpcEvent_cxx
