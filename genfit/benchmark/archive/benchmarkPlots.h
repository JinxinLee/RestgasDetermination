//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Jun 18 23:43:57 2008 by ROOT version 5.18/00
// from TTree t/SPtestTask output
// found on file: /afs/e18.ph.tum.de/panda/SIM/hoeppner/workdir.sarge/0.50GeV_0.025cmRes_45.00deg_3000ev.root
//////////////////////////////////////////////////////////

#ifndef benchmarkPlots_h
#define benchmarkPlots_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class benchmarkPlots {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Double_t        momRe;
   Double_t        momTr;
   Double_t        momSi;
   Double_t        momPu;
   Double_t        uRe;
   Double_t        uTr;
   Double_t        uSi;
   Double_t        uPu;
   Double_t        vRe;
   Double_t        vTr;
   Double_t        vSi;
   Double_t        vPu;
   Double_t        upRe;
   Double_t        upTr;
   Double_t        upSi;
   Double_t        upPu;
   Double_t        vpRe;
   Double_t        vpTr;
   Double_t        vpSi;
   Double_t        vpPu;
   Double_t        thTr;
   Double_t        thSt;
   Double_t        phiTr;
   Double_t        phiSt;

   // List of branches
   TBranch        *b_momRe;   //!
   TBranch        *b_momTr;   //!
   TBranch        *b_momSi;   //!
   TBranch        *b_momPu;   //!
   TBranch        *b_uRe;   //!
   TBranch        *b_uTr;   //!
   TBranch        *b_uSi;   //!
   TBranch        *b_uPu;   //!
   TBranch        *b_vRe;   //!
   TBranch        *b_vTr;   //!
   TBranch        *b_vSi;   //!
   TBranch        *b_vPu;   //!
   TBranch        *b_upRe;   //!
   TBranch        *b_upTr;   //!
   TBranch        *b_upSi;   //!
   TBranch        *b_upPu;   //!
   TBranch        *b_vpRe;   //!
   TBranch        *b_vpTr;   //!
   TBranch        *b_vpSi;   //!
   TBranch        *b_vpPu;   //!
   TBranch        *b_thTr;   //!
   TBranch        *b_thSt;   //!
   TBranch        *b_phiTr;   //!
   TBranch        *b_phiSt;   //!

   benchmarkPlots(TTree *tree);
   virtual ~benchmarkPlots();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef benchmarkPlots_cxx
benchmarkPlots::benchmarkPlots(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/afs/e18.ph.tum.de/panda/SIM/hoeppner/workdir.sarge/0.50GeV_0.025cmRes_45.00deg_3000ev.root");
      if (!f) {
         f = new TFile("/afs/e18.ph.tum.de/panda/SIM/hoeppner/workdir.sarge/0.50GeV_0.025cmRes_45.00deg_3000ev.root");
      }
      tree = (TTree*)gDirectory->Get("t");

   }
   Init(tree);
}

benchmarkPlots::~benchmarkPlots()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t benchmarkPlots::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t benchmarkPlots::LoadTree(Long64_t entry)
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

void benchmarkPlots::Init(TTree *tree)
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

   fChain->SetBranchAddress("momRe", &momRe, &b_momRe);
   fChain->SetBranchAddress("momTr", &momTr, &b_momTr);
   fChain->SetBranchAddress("momSi", &momSi, &b_momSi);
   fChain->SetBranchAddress("momPu", &momPu, &b_momPu);
   fChain->SetBranchAddress("uRe", &uRe, &b_uRe);
   fChain->SetBranchAddress("uTr", &uTr, &b_uTr);
   fChain->SetBranchAddress("uSi", &uSi, &b_uSi);
   fChain->SetBranchAddress("uPu", &uPu, &b_uPu);
   fChain->SetBranchAddress("vRe", &vRe, &b_vRe);
   fChain->SetBranchAddress("vTr", &vTr, &b_vTr);
   fChain->SetBranchAddress("vSi", &vSi, &b_vSi);
   fChain->SetBranchAddress("vPu", &vPu, &b_vPu);
   fChain->SetBranchAddress("upRe", &upRe, &b_upRe);
   fChain->SetBranchAddress("upTr", &upTr, &b_upTr);
   fChain->SetBranchAddress("upSi", &upSi, &b_upSi);
   fChain->SetBranchAddress("upPu", &upPu, &b_upPu);
   fChain->SetBranchAddress("vpRe", &vpRe, &b_vpRe);
   fChain->SetBranchAddress("vpTr", &vpTr, &b_vpTr);
   fChain->SetBranchAddress("vpSi", &vpSi, &b_vpSi);
   fChain->SetBranchAddress("vpPu", &vpPu, &b_vpPu);
   fChain->SetBranchAddress("thTr", &thTr, &b_thTr);
   fChain->SetBranchAddress("thSt", &thSt, &b_thSt);
   fChain->SetBranchAddress("phiTr", &phiTr, &b_phiTr);
   fChain->SetBranchAddress("phiSt", &phiSt, &b_phiSt);
   Notify();
}

Bool_t benchmarkPlots::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void benchmarkPlots::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t benchmarkPlots::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef benchmarkPlots_cxx
