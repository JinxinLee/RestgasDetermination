//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Jan 12 17:54:20 2009 by ROOT version 5.20/00
// from TTree Hits/COMPASS GEM hits tree
// found on file: run-122.001.root
//////////////////////////////////////////////////////////

#ifndef Hits_h
#define Hits_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <string>
#include <iostream>
#include <CsGEMPlane.h>

   const Int_t kMaxGM01X1__ = 1;
   const Int_t kMaxGM01Y1__ = 1;
   const Int_t kMaxGM02X1__ = 1;
   const Int_t kMaxGM02Y1__ = 1;
   const Int_t kMaxSI01X1__ = 1;
   const Int_t kMaxSI01Y1__ = 1;
   const Int_t kMaxSI02X1__ = 1;
   const Int_t kMaxSI02Y1__ = 1;

class Hits {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   //CsGEMPlane*     GM01X1__.;
   Int_t           GM01X1___fNhit;
   Int_t           GM01X1___fNcluster;
   Int_t           GM01X1___fPlaneHdr_fEvtNum;
   Int_t           GM01X1___fPlaneHdr_fRun;
   Int_t           GM01X1___fPlaneHdr_fDate;
   //   std::list<CsGEMHit*> GM01X1___fHits;
   //   std::list<CsGEMCluster*> GM01X1___fClusters;
   Bool_t          GM01X1___fIsHitSorted;
   Bool_t          GM01X1___fIsClusterSorted;
   Bool_t          GM01X1___fIsClusterized;
   //   std::string          GM01X1___fName;
   //CsGEMPlane      *GM01Y1__.;
   Int_t           GM01Y1___fNhit;
   Int_t           GM01Y1___fNcluster;
   Int_t           GM01Y1___fPlaneHdr_fEvtNum;
   Int_t           GM01Y1___fPlaneHdr_fRun;
   Int_t           GM01Y1___fPlaneHdr_fDate;
 //std::list<CsGEMHit*> GM01Y1___fHits;
 //std::list<CsGEMCluster*> GM01Y1___fClusters;
   Bool_t          GM01Y1___fIsHitSorted;
   Bool_t          GM01Y1___fIsClusterSorted;
   Bool_t          GM01Y1___fIsClusterized;
   std::string          GM01Y1___fName;
 //CsGEMPlane      *GM02X1__.;
   Int_t           GM02X1___fNhit;
   Int_t           GM02X1___fNcluster;
   Int_t           GM02X1___fPlaneHdr_fEvtNum;
   Int_t           GM02X1___fPlaneHdr_fRun;
   Int_t           GM02X1___fPlaneHdr_fDate;
 //std::list<CsGEMHit*> GM02X1___fHits;
 //std::list<CsGEMCluster*> GM02X1___fClusters;
   Bool_t          GM02X1___fIsHitSorted;
   Bool_t          GM02X1___fIsClusterSorted;
   Bool_t          GM02X1___fIsClusterized;
   std::string          GM02X1___fName;
 //CsGEMPlane      *GM02Y1__.;
   Int_t           GM02Y1___fNhit;
   Int_t           GM02Y1___fNcluster;
   Int_t           GM02Y1___fPlaneHdr_fEvtNum;
   Int_t           GM02Y1___fPlaneHdr_fRun;
   Int_t           GM02Y1___fPlaneHdr_fDate;
 //std::list<CsGEMHit*> GM02Y1___fHits;
 //std::list<CsGEMCluster*> GM02Y1___fClusters;
   Bool_t          GM02Y1___fIsHitSorted;
   Bool_t          GM02Y1___fIsClusterSorted;
   Bool_t          GM02Y1___fIsClusterized;
   std::string          GM02Y1___fName;
 //CsGEMPlane      *SI01X1__.;
   Int_t           SI01X1___fNhit;
   Int_t           SI01X1___fNcluster;
   Int_t           SI01X1___fPlaneHdr_fEvtNum;
   Int_t           SI01X1___fPlaneHdr_fRun;
   Int_t           SI01X1___fPlaneHdr_fDate;
 //std::list<CsGEMHit*> SI01X1___fHits;
 //std::list<CsGEMCluster*> SI01X1___fClusters;
   Bool_t          SI01X1___fIsHitSorted;
   Bool_t          SI01X1___fIsClusterSorted;
   Bool_t          SI01X1___fIsClusterized;
   std::string          SI01X1___fName;
 //CsGEMPlane      *SI01Y1__.;
   Int_t           SI01Y1___fNhit;
   Int_t           SI01Y1___fNcluster;
   Int_t           SI01Y1___fPlaneHdr_fEvtNum;
   Int_t           SI01Y1___fPlaneHdr_fRun;
   Int_t           SI01Y1___fPlaneHdr_fDate;
 //std::list<CsGEMHit*> SI01Y1___fHits;
 //std::list<CsGEMCluster*> SI01Y1___fClusters;
   Bool_t          SI01Y1___fIsHitSorted;
   Bool_t          SI01Y1___fIsClusterSorted;
   Bool_t          SI01Y1___fIsClusterized;
   std::string          SI01Y1___fName;
 //CsGEMPlane      *SI02X1__.;
   Int_t           SI02X1___fNhit;
   Int_t           SI02X1___fNcluster;
   Int_t           SI02X1___fPlaneHdr_fEvtNum;
   Int_t           SI02X1___fPlaneHdr_fRun;
   Int_t           SI02X1___fPlaneHdr_fDate;
 //std::list<CsGEMHit*> SI02X1___fHits;
 //std::list<CsGEMCluster*> SI02X1___fClusters;
   Bool_t          SI02X1___fIsHitSorted;
   Bool_t          SI02X1___fIsClusterSorted;
   Bool_t          SI02X1___fIsClusterized;
   std::string          SI02X1___fName;
 //CsGEMPlane      *SI02Y1__.;
   Int_t           SI02Y1___fNhit;
   Int_t           SI02Y1___fNcluster;
   Int_t           SI02Y1___fPlaneHdr_fEvtNum;
   Int_t           SI02Y1___fPlaneHdr_fRun;
   Int_t           SI02Y1___fPlaneHdr_fDate;
 //std::list<CsGEMHit*> SI02Y1___fHits;
 //std::list<CsGEMCluster*> SI02Y1___fClusters;
   Bool_t          SI02Y1___fIsHitSorted;
   Bool_t          SI02Y1___fIsClusterSorted;
   Bool_t          SI02Y1___fIsClusterized;
   std::string          SI02Y1___fName;

   // Std::List of branches
   TBranch        *b_GM01X1___fHits;
   TBranch        *b_GM01X1___fCluster;
   TBranch        *b_GM01X1__;//!
   TBranch        *b_GM01X1___fNhit;   //!
   TBranch        *b_GM01X1___fNcluster;   //!
   TBranch        *b_GM01X1___fPlaneHdr_fEvtNum;   //!
   TBranch        *b_GM01X1___fPlaneHdr_fRun;   //!
   TBranch        *b_GM01X1___fPlaneHdr_fDate;   //!
   TBranch        *b_GM01X1___fIsHitSorted;   //!
   TBranch        *b_GM01X1___fIsClusterSorted;   //!
   TBranch        *b_GM01X1___fIsClusterized;   //!
   TBranch        *b_GM01X1___fName;   //!
   TBranch        *b_GM01Y1___fNhit;   //!
   TBranch        *b_GM01Y1___fNcluster;   //!
   TBranch        *b_GM01Y1___fPlaneHdr_fEvtNum;   //!
   TBranch        *b_GM01Y1___fPlaneHdr_fRun;   //!
   TBranch        *b_GM01Y1___fPlaneHdr_fDate;   //!
   TBranch        *b_GM01Y1___fIsHitSorted;   //!
   TBranch        *b_GM01Y1___fIsClusterSorted;   //!
   TBranch        *b_GM01Y1___fIsClusterized;   //!
   TBranch        *b_GM01Y1___fName;   //!
   TBranch        *b_GM02X1___fNhit;   //!
   TBranch        *b_GM02X1___fNcluster;   //!
   TBranch        *b_GM02X1___fPlaneHdr_fEvtNum;   //!
   TBranch        *b_GM02X1___fPlaneHdr_fRun;   //!
   TBranch        *b_GM02X1___fPlaneHdr_fDate;   //!
   TBranch        *b_GM02X1___fIsHitSorted;   //!
   TBranch        *b_GM02X1___fIsClusterSorted;   //!
   TBranch        *b_GM02X1___fIsClusterized;   //!
   TBranch        *b_GM02X1___fName;   //!
   TBranch        *b_GM02Y1___fNhit;   //!
   TBranch        *b_GM02Y1___fNcluster;   //!
   TBranch        *b_GM02Y1___fPlaneHdr_fEvtNum;   //!
   TBranch        *b_GM02Y1___fPlaneHdr_fRun;   //!
   TBranch        *b_GM02Y1___fPlaneHdr_fDate;   //!
   TBranch        *b_GM02Y1___fIsHitSorted;   //!
   TBranch        *b_GM02Y1___fIsClusterSorted;   //!
   TBranch        *b_GM02Y1___fIsClusterized;   //!
   TBranch        *b_GM02Y1___fName;   //!
   TBranch        *b_SI01X1___fNhit;   //!
   TBranch        *b_SI01X1___fNcluster;   //!
   TBranch        *b_SI01X1___fPlaneHdr_fEvtNum;   //!
   TBranch        *b_SI01X1___fPlaneHdr_fRun;   //!
   TBranch        *b_SI01X1___fPlaneHdr_fDate;   //!
   TBranch        *b_SI01X1___fIsHitSorted;   //!
   TBranch        *b_SI01X1___fIsClusterSorted;   //!
   TBranch        *b_SI01X1___fIsClusterized;   //!
   TBranch        *b_SI01X1___fName;   //!
   TBranch        *b_SI01Y1___fNhit;   //!
   TBranch        *b_SI01Y1___fNcluster;   //!
   TBranch        *b_SI01Y1___fPlaneHdr_fEvtNum;   //!
   TBranch        *b_SI01Y1___fPlaneHdr_fRun;   //!
   TBranch        *b_SI01Y1___fPlaneHdr_fDate;   //!
   TBranch        *b_SI01Y1___fIsHitSorted;   //!
   TBranch        *b_SI01Y1___fIsClusterSorted;   //!
   TBranch        *b_SI01Y1___fIsClusterized;   //!
   TBranch        *b_SI01Y1___fName;   //!
   TBranch        *b_SI02X1___fNhit;   //!
   TBranch        *b_SI02X1___fNcluster;   //!
   TBranch        *b_SI02X1___fPlaneHdr_fEvtNum;   //!
   TBranch        *b_SI02X1___fPlaneHdr_fRun;   //!
   TBranch        *b_SI02X1___fPlaneHdr_fDate;   //!
   TBranch        *b_SI02X1___fIsHitSorted;   //!
   TBranch        *b_SI02X1___fIsClusterSorted;   //!
   TBranch        *b_SI02X1___fIsClusterized;   //!
   TBranch        *b_SI02X1___fName;   //!
   TBranch        *b_SI02Y1___fNhit;   //!
   TBranch        *b_SI02Y1___fNcluster;   //!
   TBranch        *b_SI02Y1___fPlaneHdr_fEvtNum;   //!
   TBranch        *b_SI02Y1___fPlaneHdr_fRun;   //!
   TBranch        *b_SI02Y1___fPlaneHdr_fDate;   //!
   TBranch        *b_SI02Y1___fIsHitSorted;   //!
   TBranch        *b_SI02Y1___fIsClusterSorted;   //!
   TBranch        *b_SI02Y1___fIsClusterized;   //!
   TBranch        *b_SI02Y1___fName;   //!

   virtual bool loadNext();
   Hits(TTree *tree=0);
   virtual ~Hits();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef Hits_cxx
Hits::Hits(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("run-122.001.root");
      if (!f) {
         f = new TFile("run-122.001.root");
      }
      tree = (TTree*)gDirectory->Get("Hits");

   }
   Init(tree);
}

Hits::~Hits()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Hits::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Hits::LoadTree(Long64_t entry)
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

void Hits::Init(TTree *tree)
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

   //   fChain->SetBranchAddress("GM01X1__.", &GM01X1__, &b_GM01X1__);
   fChain->SetBranchAddress("GM01X1__.fNhit", &GM01X1___fNhit, &b_GM01X1___fNhit);
   fChain->SetBranchAddress("GM01X1__.fNcluster", &GM01X1___fNcluster, &b_GM01X1___fNcluster);
   fChain->SetBranchAddress("GM01X1__.fPlaneHdr.fEvtNum", &GM01X1___fPlaneHdr_fEvtNum, &b_GM01X1___fPlaneHdr_fEvtNum);
   fChain->SetBranchAddress("GM01X1__.fPlaneHdr.fRun", &GM01X1___fPlaneHdr_fRun, &b_GM01X1___fPlaneHdr_fRun);
   fChain->SetBranchAddress("GM01X1__.fPlaneHdr.fDate", &GM01X1___fPlaneHdr_fDate, &b_GM01X1___fPlaneHdr_fDate);



   fChain->SetBranchAddress("GM01X1__.fIsHitSorted", &GM01X1___fIsHitSorted, &b_GM01X1___fIsHitSorted);
   fChain->SetBranchAddress("GM01X1__.fIsClusterSorted", &GM01X1___fIsClusterSorted, &b_GM01X1___fIsClusterSorted);
   fChain->SetBranchAddress("GM01X1__.fIsClusterized", &GM01X1___fIsClusterized, &b_GM01X1___fIsClusterized);
   // fChain->SetBranchAddress("GM01X1__.fName", &GM01X1___fName, &b_GM01X1___fName);
   fChain->SetBranchAddress("GM01Y1__.fNhit", &GM01Y1___fNhit, &b_GM01Y1___fNhit);
   fChain->SetBranchAddress("GM01Y1__.fNcluster", &GM01Y1___fNcluster, &b_GM01Y1___fNcluster);
   fChain->SetBranchAddress("GM01Y1__.fPlaneHdr.fEvtNum", &GM01Y1___fPlaneHdr_fEvtNum, &b_GM01Y1___fPlaneHdr_fEvtNum);
   fChain->SetBranchAddress("GM01Y1__.fPlaneHdr.fRun", &GM01Y1___fPlaneHdr_fRun, &b_GM01Y1___fPlaneHdr_fRun);
   fChain->SetBranchAddress("GM01Y1__.fPlaneHdr.fDate", &GM01Y1___fPlaneHdr_fDate, &b_GM01Y1___fPlaneHdr_fDate);
   fChain->SetBranchAddress("GM01Y1__.fIsHitSorted", &GM01Y1___fIsHitSorted, &b_GM01Y1___fIsHitSorted);
   fChain->SetBranchAddress("GM01Y1__.fIsClusterSorted", &GM01Y1___fIsClusterSorted, &b_GM01Y1___fIsClusterSorted);
   fChain->SetBranchAddress("GM01Y1__.fIsClusterized", &GM01Y1___fIsClusterized, &b_GM01Y1___fIsClusterized);
   fChain->SetBranchAddress("GM01Y1__.fName", &GM01Y1___fName, &b_GM01Y1___fName);
   fChain->SetBranchAddress("GM02X1__.fNhit", &GM02X1___fNhit, &b_GM02X1___fNhit);
   fChain->SetBranchAddress("GM02X1__.fNcluster", &GM02X1___fNcluster, &b_GM02X1___fNcluster);
   fChain->SetBranchAddress("GM02X1__.fPlaneHdr.fEvtNum", &GM02X1___fPlaneHdr_fEvtNum, &b_GM02X1___fPlaneHdr_fEvtNum);
   fChain->SetBranchAddress("GM02X1__.fPlaneHdr.fRun", &GM02X1___fPlaneHdr_fRun, &b_GM02X1___fPlaneHdr_fRun);
   fChain->SetBranchAddress("GM02X1__.fPlaneHdr.fDate", &GM02X1___fPlaneHdr_fDate, &b_GM02X1___fPlaneHdr_fDate);
   fChain->SetBranchAddress("GM02X1__.fIsHitSorted", &GM02X1___fIsHitSorted, &b_GM02X1___fIsHitSorted);
   fChain->SetBranchAddress("GM02X1__.fIsClusterSorted", &GM02X1___fIsClusterSorted, &b_GM02X1___fIsClusterSorted);
   fChain->SetBranchAddress("GM02X1__.fIsClusterized", &GM02X1___fIsClusterized, &b_GM02X1___fIsClusterized);
   fChain->SetBranchAddress("GM02X1__.fName", &GM02X1___fName, &b_GM02X1___fName);
   fChain->SetBranchAddress("GM02Y1__.fNhit", &GM02Y1___fNhit, &b_GM02Y1___fNhit);
   fChain->SetBranchAddress("GM02Y1__.fNcluster", &GM02Y1___fNcluster, &b_GM02Y1___fNcluster);
   fChain->SetBranchAddress("GM02Y1__.fPlaneHdr.fEvtNum", &GM02Y1___fPlaneHdr_fEvtNum, &b_GM02Y1___fPlaneHdr_fEvtNum);
   fChain->SetBranchAddress("GM02Y1__.fPlaneHdr.fRun", &GM02Y1___fPlaneHdr_fRun, &b_GM02Y1___fPlaneHdr_fRun);
   fChain->SetBranchAddress("GM02Y1__.fPlaneHdr.fDate", &GM02Y1___fPlaneHdr_fDate, &b_GM02Y1___fPlaneHdr_fDate);
   fChain->SetBranchAddress("GM02Y1__.fIsHitSorted", &GM02Y1___fIsHitSorted, &b_GM02Y1___fIsHitSorted);
   fChain->SetBranchAddress("GM02Y1__.fIsClusterSorted", &GM02Y1___fIsClusterSorted, &b_GM02Y1___fIsClusterSorted);
   fChain->SetBranchAddress("GM02Y1__.fIsClusterized", &GM02Y1___fIsClusterized, &b_GM02Y1___fIsClusterized);
   fChain->SetBranchAddress("GM02Y1__.fName", &GM02Y1___fName, &b_GM02Y1___fName);
   fChain->SetBranchAddress("SI01X1__.fNhit", &SI01X1___fNhit, &b_SI01X1___fNhit);
   fChain->SetBranchAddress("SI01X1__.fNcluster", &SI01X1___fNcluster, &b_SI01X1___fNcluster);
   fChain->SetBranchAddress("SI01X1__.fPlaneHdr.fEvtNum", &SI01X1___fPlaneHdr_fEvtNum, &b_SI01X1___fPlaneHdr_fEvtNum);
   fChain->SetBranchAddress("SI01X1__.fPlaneHdr.fRun", &SI01X1___fPlaneHdr_fRun, &b_SI01X1___fPlaneHdr_fRun);
   fChain->SetBranchAddress("SI01X1__.fPlaneHdr.fDate", &SI01X1___fPlaneHdr_fDate, &b_SI01X1___fPlaneHdr_fDate);
   fChain->SetBranchAddress("SI01X1__.fIsHitSorted", &SI01X1___fIsHitSorted, &b_SI01X1___fIsHitSorted);
   fChain->SetBranchAddress("SI01X1__.fIsClusterSorted", &SI01X1___fIsClusterSorted, &b_SI01X1___fIsClusterSorted);
   fChain->SetBranchAddress("SI01X1__.fIsClusterized", &SI01X1___fIsClusterized, &b_SI01X1___fIsClusterized);
   fChain->SetBranchAddress("SI01X1__.fName", &SI01X1___fName, &b_SI01X1___fName);
   fChain->SetBranchAddress("SI01Y1__.fNhit", &SI01Y1___fNhit, &b_SI01Y1___fNhit);
   fChain->SetBranchAddress("SI01Y1__.fNcluster", &SI01Y1___fNcluster, &b_SI01Y1___fNcluster);
   fChain->SetBranchAddress("SI01Y1__.fPlaneHdr.fEvtNum", &SI01Y1___fPlaneHdr_fEvtNum, &b_SI01Y1___fPlaneHdr_fEvtNum);
   fChain->SetBranchAddress("SI01Y1__.fPlaneHdr.fRun", &SI01Y1___fPlaneHdr_fRun, &b_SI01Y1___fPlaneHdr_fRun);
   fChain->SetBranchAddress("SI01Y1__.fPlaneHdr.fDate", &SI01Y1___fPlaneHdr_fDate, &b_SI01Y1___fPlaneHdr_fDate);
   fChain->SetBranchAddress("SI01Y1__.fIsHitSorted", &SI01Y1___fIsHitSorted, &b_SI01Y1___fIsHitSorted);
   fChain->SetBranchAddress("SI01Y1__.fIsClusterSorted", &SI01Y1___fIsClusterSorted, &b_SI01Y1___fIsClusterSorted);
   fChain->SetBranchAddress("SI01Y1__.fIsClusterized", &SI01Y1___fIsClusterized, &b_SI01Y1___fIsClusterized);
   fChain->SetBranchAddress("SI01Y1__.fName", &SI01Y1___fName, &b_SI01Y1___fName);
   fChain->SetBranchAddress("SI02X1__.fNhit", &SI02X1___fNhit, &b_SI02X1___fNhit);
   fChain->SetBranchAddress("SI02X1__.fNcluster", &SI02X1___fNcluster, &b_SI02X1___fNcluster);
   fChain->SetBranchAddress("SI02X1__.fPlaneHdr.fEvtNum", &SI02X1___fPlaneHdr_fEvtNum, &b_SI02X1___fPlaneHdr_fEvtNum);
   fChain->SetBranchAddress("SI02X1__.fPlaneHdr.fRun", &SI02X1___fPlaneHdr_fRun, &b_SI02X1___fPlaneHdr_fRun);
   fChain->SetBranchAddress("SI02X1__.fPlaneHdr.fDate", &SI02X1___fPlaneHdr_fDate, &b_SI02X1___fPlaneHdr_fDate);
   fChain->SetBranchAddress("SI02X1__.fIsHitSorted", &SI02X1___fIsHitSorted, &b_SI02X1___fIsHitSorted);
   fChain->SetBranchAddress("SI02X1__.fIsClusterSorted", &SI02X1___fIsClusterSorted, &b_SI02X1___fIsClusterSorted);
   fChain->SetBranchAddress("SI02X1__.fIsClusterized", &SI02X1___fIsClusterized, &b_SI02X1___fIsClusterized);
   fChain->SetBranchAddress("SI02X1__.fName", &SI02X1___fName, &b_SI02X1___fName);
   fChain->SetBranchAddress("SI02Y1__.fNhit", &SI02Y1___fNhit, &b_SI02Y1___fNhit);
   fChain->SetBranchAddress("SI02Y1__.fNcluster", &SI02Y1___fNcluster, &b_SI02Y1___fNcluster);
   fChain->SetBranchAddress("SI02Y1__.fPlaneHdr.fEvtNum", &SI02Y1___fPlaneHdr_fEvtNum, &b_SI02Y1___fPlaneHdr_fEvtNum);
   fChain->SetBranchAddress("SI02Y1__.fPlaneHdr.fRun", &SI02Y1___fPlaneHdr_fRun, &b_SI02Y1___fPlaneHdr_fRun);
   fChain->SetBranchAddress("SI02Y1__.fPlaneHdr.fDate", &SI02Y1___fPlaneHdr_fDate, &b_SI02Y1___fPlaneHdr_fDate);
   fChain->SetBranchAddress("SI02Y1__.fIsHitSorted", &SI02Y1___fIsHitSorted, &b_SI02Y1___fIsHitSorted);
   fChain->SetBranchAddress("SI02Y1__.fIsClusterSorted", &SI02Y1___fIsClusterSorted, &b_SI02Y1___fIsClusterSorted);
   fChain->SetBranchAddress("SI02Y1__.fIsClusterized", &SI02Y1___fIsClusterized, &b_SI02Y1___fIsClusterized);
   fChain->SetBranchAddress("SI02Y1__.fName", &SI02Y1___fName, &b_SI02Y1___fName);
   Notify();
}

Bool_t Hits::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Hits::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Hits::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef Hits_cxx
