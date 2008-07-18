//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Jan  3 18:55:43 2007 by ROOT version 5.11/02
// from TTree statTree/tree created by Fitter::makeStatTree()
// found on file: statTree.root
//////////////////////////////////////////////////////////

#ifndef statTreeAnalyzer_auto_h
#define statTreeAnalyzer_auto_h

#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"

#include<iostream>
#include "TMatrixT.h"

class statTreeAnalyzer_auto {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leave types
   TMatrixT<double> *state;
   TMatrixT<double> *residMCtruth;
   TMatrixT<double> *residStart;
   TMatrixT<double> *covFit;
   Double_t        chi2;

   // List of branches
   TBranch        *b_state;   //!
   TBranch        *b_residMCtruth;   //!
   TBranch        *b_residStart;   //!
   TBranch        *b_covFit;   //!
   TBranch        *b_chi2;   //!

   statTreeAnalyzer_auto(std::string treeName, std::string fileName);
   virtual ~statTreeAnalyzer_auto();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef statTreeAnalyzer_auto_cxx
statTreeAnalyzer_auto::statTreeAnalyzer_auto(std::string treeName,
					     std::string fileName)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
  TTree* tree = NULL;
  //TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("statTree.root");
  TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(fileName.c_str());
  if (!f) {
    //f = new TFile("statTree.root");
    f = new TFile(fileName.c_str());
  }
  tree = (TTree*)gDirectory->Get(treeName.c_str());
  if(tree == NULL) {
    std::cerr << "Error in statTreeAnalyzer-packet:" << std::endl 
	      << "tree\"" << treeName << "\" not found in file \""
	      << fileName << "\" -> abort" << std::endl;
    throw;
  }
  Init(tree);
}

statTreeAnalyzer_auto::~statTreeAnalyzer_auto()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t statTreeAnalyzer_auto::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t statTreeAnalyzer_auto::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->IsA() != TChain::Class()) return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void statTreeAnalyzer_auto::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses of the tree
   // will be set. It is normaly not necessary to make changes to the
   // generated code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running with PROOF.

   // Set object pointer
 
   state = 0;
   residMCtruth = 0;
   residStart = 0;
   covFit = 0;
   // Set branch addresses
   if (tree == 0) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("state",&state);
   fChain->SetBranchAddress("residMCtruth",&residMCtruth);
   fChain->SetBranchAddress("residStart",&residStart);
   fChain->SetBranchAddress("covFit",&covFit);
   fChain->SetBranchAddress("chi2",&chi2);
   Notify();
}

Bool_t statTreeAnalyzer_auto::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. Typically here the branch pointers
   // will be retrieved. It is normaly not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed.

   // Get branch pointers
   b_state = fChain->GetBranch("state");
   b_residMCtruth = fChain->GetBranch("residMCtruth");
   b_residStart = fChain->GetBranch("residStart");
   b_covFit = fChain->GetBranch("covFit");
   b_chi2 = fChain->GetBranch("chi2");

   return kTRUE;
}

void statTreeAnalyzer_auto::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t statTreeAnalyzer_auto::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef statTreeAnalyzer_auto_cxx
