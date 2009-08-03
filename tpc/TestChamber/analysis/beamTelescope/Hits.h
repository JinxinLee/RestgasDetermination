//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Jul 23 11:27:59 2009 by ROOT version 5.24/00
// from TTree Hits/COMPASS GEM hits tree
// found on file: run-921.root
//////////////////////////////////////////////////////////

#ifndef Hits_h
#define Hits_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <string>

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
 //CsGEMPlane      *GM01X1__.;
   Int_t           GM01X1___fHeader_fEvtNum;
   Int_t           GM01X1___fHeader_fRun;
   Int_t           GM01X1___fHeader_fDate;
   Int_t           GM01X1___fPar_fClusMeth;
   Int_t           GM01X1___fPar_fClusConfigMask;
   Int_t           GM01X1___fPar_fShareHits;
   Float_t         GM01X1___fPar_fThrClus;
   Float_t         GM01X1___fPar_fThrHit;
   Float_t         GM01X1___fPar_fTimeCrossTalkR;
   Int_t           GM01X1___fPar_fSample;
 //list<CsGEMHit*> GM01X1___fHits;
 //list<CsGEMCluster*> GM01X1___fClusters;
   Bool_t          GM01X1___fIsHitSorted;
   Bool_t          GM01X1___fIsHitAmpSorted;
   Bool_t          GM01X1___fIsClusterSorted;
   Bool_t          GM01X1___fIsClusterized;
   std::string          GM01X1___fName;
 //CsGEMPlane      *GM01Y1__.;
   Int_t           GM01Y1___fHeader_fEvtNum;
   Int_t           GM01Y1___fHeader_fRun;
   Int_t           GM01Y1___fHeader_fDate;
   Int_t           GM01Y1___fPar_fClusMeth;
   Int_t           GM01Y1___fPar_fClusConfigMask;
   Int_t           GM01Y1___fPar_fShareHits;
   Float_t         GM01Y1___fPar_fThrClus;
   Float_t         GM01Y1___fPar_fThrHit;
   Float_t         GM01Y1___fPar_fTimeCrossTalkR;
   Int_t           GM01Y1___fPar_fSample;
 //list<CsGEMHit*> GM01Y1___fHits;
 //list<CsGEMCluster*> GM01Y1___fClusters;
   Bool_t          GM01Y1___fIsHitSorted;
   Bool_t          GM01Y1___fIsHitAmpSorted;
   Bool_t          GM01Y1___fIsClusterSorted;
   Bool_t          GM01Y1___fIsClusterized;
   std::string          GM01Y1___fName;
 //CsGEMPlane      *GM02X1__.;
   Int_t           GM02X1___fHeader_fEvtNum;
   Int_t           GM02X1___fHeader_fRun;
   Int_t           GM02X1___fHeader_fDate;
   Int_t           GM02X1___fPar_fClusMeth;
   Int_t           GM02X1___fPar_fClusConfigMask;
   Int_t           GM02X1___fPar_fShareHits;
   Float_t         GM02X1___fPar_fThrClus;
   Float_t         GM02X1___fPar_fThrHit;
   Float_t         GM02X1___fPar_fTimeCrossTalkR;
   Int_t           GM02X1___fPar_fSample;
 //list<CsGEMHit*> GM02X1___fHits;
 //list<CsGEMCluster*> GM02X1___fClusters;
   Bool_t          GM02X1___fIsHitSorted;
   Bool_t          GM02X1___fIsHitAmpSorted;
   Bool_t          GM02X1___fIsClusterSorted;
   Bool_t          GM02X1___fIsClusterized;
   std::string          GM02X1___fName;
 //CsGEMPlane      *GM02Y1__.;
   Int_t           GM02Y1___fHeader_fEvtNum;
   Int_t           GM02Y1___fHeader_fRun;
   Int_t           GM02Y1___fHeader_fDate;
   Int_t           GM02Y1___fPar_fClusMeth;
   Int_t           GM02Y1___fPar_fClusConfigMask;
   Int_t           GM02Y1___fPar_fShareHits;
   Float_t         GM02Y1___fPar_fThrClus;
   Float_t         GM02Y1___fPar_fThrHit;
   Float_t         GM02Y1___fPar_fTimeCrossTalkR;
   Int_t           GM02Y1___fPar_fSample;
 //list<CsGEMHit*> GM02Y1___fHits;
 //list<CsGEMCluster*> GM02Y1___fClusters;
   Bool_t          GM02Y1___fIsHitSorted;
   Bool_t          GM02Y1___fIsHitAmpSorted;
   Bool_t          GM02Y1___fIsClusterSorted;
   Bool_t          GM02Y1___fIsClusterized;
   std::string          GM02Y1___fName;
 //CsGEMPlane      *SI01X1__.;
   Int_t           SI01X1___fHeader_fEvtNum;
   Int_t           SI01X1___fHeader_fRun;
   Int_t           SI01X1___fHeader_fDate;
   Int_t           SI01X1___fPar_fClusMeth;
   Int_t           SI01X1___fPar_fClusConfigMask;
   Int_t           SI01X1___fPar_fShareHits;
   Float_t         SI01X1___fPar_fThrClus;
   Float_t         SI01X1___fPar_fThrHit;
   Float_t         SI01X1___fPar_fTimeCrossTalkR;
   Int_t           SI01X1___fPar_fSample;
 //list<CsGEMHit*> SI01X1___fHits;
 //list<CsGEMCluster*> SI01X1___fClusters;
   Bool_t          SI01X1___fIsHitSorted;
   Bool_t          SI01X1___fIsHitAmpSorted;
   Bool_t          SI01X1___fIsClusterSorted;
   Bool_t          SI01X1___fIsClusterized;
   std::string          SI01X1___fName;
 //CsGEMPlane      *SI01Y1__.;
   Int_t           SI01Y1___fHeader_fEvtNum;
   Int_t           SI01Y1___fHeader_fRun;
   Int_t           SI01Y1___fHeader_fDate;
   Int_t           SI01Y1___fPar_fClusMeth;
   Int_t           SI01Y1___fPar_fClusConfigMask;
   Int_t           SI01Y1___fPar_fShareHits;
   Float_t         SI01Y1___fPar_fThrClus;
   Float_t         SI01Y1___fPar_fThrHit;
   Float_t         SI01Y1___fPar_fTimeCrossTalkR;
   Int_t           SI01Y1___fPar_fSample;
 //list<CsGEMHit*> SI01Y1___fHits;
 //list<CsGEMCluster*> SI01Y1___fClusters;
   Bool_t          SI01Y1___fIsHitSorted;
   Bool_t          SI01Y1___fIsHitAmpSorted;
   Bool_t          SI01Y1___fIsClusterSorted;
   Bool_t          SI01Y1___fIsClusterized;
   std::string          SI01Y1___fName;
 //CsGEMPlane      *SI02X1__.;
   Int_t           SI02X1___fHeader_fEvtNum;
   Int_t           SI02X1___fHeader_fRun;
   Int_t           SI02X1___fHeader_fDate;
   Int_t           SI02X1___fPar_fClusMeth;
   Int_t           SI02X1___fPar_fClusConfigMask;
   Int_t           SI02X1___fPar_fShareHits;
   Float_t         SI02X1___fPar_fThrClus;
   Float_t         SI02X1___fPar_fThrHit;
   Float_t         SI02X1___fPar_fTimeCrossTalkR;
   Int_t           SI02X1___fPar_fSample;
 //list<CsGEMHit*> SI02X1___fHits;
 //list<CsGEMCluster*> SI02X1___fClusters;
   Bool_t          SI02X1___fIsHitSorted;
   Bool_t          SI02X1___fIsHitAmpSorted;
   Bool_t          SI02X1___fIsClusterSorted;
   Bool_t          SI02X1___fIsClusterized;
   std::string          SI02X1___fName;
 //CsGEMPlane      *SI02Y1__.;
   Int_t           SI02Y1___fHeader_fEvtNum;
   Int_t           SI02Y1___fHeader_fRun;
   Int_t           SI02Y1___fHeader_fDate;
   Int_t           SI02Y1___fPar_fClusMeth;
   Int_t           SI02Y1___fPar_fClusConfigMask;
   Int_t           SI02Y1___fPar_fShareHits;
   Float_t         SI02Y1___fPar_fThrClus;
   Float_t         SI02Y1___fPar_fThrHit;
   Float_t         SI02Y1___fPar_fTimeCrossTalkR;
   Int_t           SI02Y1___fPar_fSample;
 //list<CsGEMHit*> SI02Y1___fHits;
 //list<CsGEMCluster*> SI02Y1___fClusters;
   Bool_t          SI02Y1___fIsHitSorted;
   Bool_t          SI02Y1___fIsHitAmpSorted;
   Bool_t          SI02Y1___fIsClusterSorted;
   Bool_t          SI02Y1___fIsClusterized;
   std::string          SI02Y1___fName;

   // List of branches
   TBranch        *b_GM01X1___fHeader_fEvtNum;   //!
   TBranch        *b_GM01X1___fHeader_fRun;   //!
   TBranch        *b_GM01X1___fHeader_fDate;   //!
   TBranch        *b_GM01X1___fPar_fClusMeth;   //!
   TBranch        *b_GM01X1___fPar_fClusConfigMask;   //!
   TBranch        *b_GM01X1___fPar_fShareHits;   //!
   TBranch        *b_GM01X1___fPar_fThrClus;   //!
   TBranch        *b_GM01X1___fPar_fThrHit;   //!
   TBranch        *b_GM01X1___fPar_fTimeCrossTalkR;   //!
   TBranch        *b_GM01X1___fPar_fSample;   //!
   TBranch        *b_GM01X1___fIsHitSorted;   //!
   TBranch        *b_GM01X1___fIsHitAmpSorted;   //!
   TBranch        *b_GM01X1___fIsClusterSorted;   //!
   TBranch        *b_GM01X1___fIsClusterized;   //!
   TBranch        *b_GM01X1___fName;   //!
   TBranch        *b_GM01Y1___fHeader_fEvtNum;   //!
   TBranch        *b_GM01Y1___fHeader_fRun;   //!
   TBranch        *b_GM01Y1___fHeader_fDate;   //!
   TBranch        *b_GM01Y1___fPar_fClusMeth;   //!
   TBranch        *b_GM01Y1___fPar_fClusConfigMask;   //!
   TBranch        *b_GM01Y1___fPar_fShareHits;   //!
   TBranch        *b_GM01Y1___fPar_fThrClus;   //!
   TBranch        *b_GM01Y1___fPar_fThrHit;   //!
   TBranch        *b_GM01Y1___fPar_fTimeCrossTalkR;   //!
   TBranch        *b_GM01Y1___fPar_fSample;   //!
   TBranch        *b_GM01Y1___fIsHitSorted;   //!
   TBranch        *b_GM01Y1___fIsHitAmpSorted;   //!
   TBranch        *b_GM01Y1___fIsClusterSorted;   //!
   TBranch        *b_GM01Y1___fIsClusterized;   //!
   TBranch        *b_GM01Y1___fName;   //!
   TBranch        *b_GM02X1___fHeader_fEvtNum;   //!
   TBranch        *b_GM02X1___fHeader_fRun;   //!
   TBranch        *b_GM02X1___fHeader_fDate;   //!
   TBranch        *b_GM02X1___fPar_fClusMeth;   //!
   TBranch        *b_GM02X1___fPar_fClusConfigMask;   //!
   TBranch        *b_GM02X1___fPar_fShareHits;   //!
   TBranch        *b_GM02X1___fPar_fThrClus;   //!
   TBranch        *b_GM02X1___fPar_fThrHit;   //!
   TBranch        *b_GM02X1___fPar_fTimeCrossTalkR;   //!
   TBranch        *b_GM02X1___fPar_fSample;   //!
   TBranch        *b_GM02X1___fIsHitSorted;   //!
   TBranch        *b_GM02X1___fIsHitAmpSorted;   //!
   TBranch        *b_GM02X1___fIsClusterSorted;   //!
   TBranch        *b_GM02X1___fIsClusterized;   //!
   TBranch        *b_GM02X1___fName;   //!
   TBranch        *b_GM02Y1___fHeader_fEvtNum;   //!
   TBranch        *b_GM02Y1___fHeader_fRun;   //!
   TBranch        *b_GM02Y1___fHeader_fDate;   //!
   TBranch        *b_GM02Y1___fPar_fClusMeth;   //!
   TBranch        *b_GM02Y1___fPar_fClusConfigMask;   //!
   TBranch        *b_GM02Y1___fPar_fShareHits;   //!
   TBranch        *b_GM02Y1___fPar_fThrClus;   //!
   TBranch        *b_GM02Y1___fPar_fThrHit;   //!
   TBranch        *b_GM02Y1___fPar_fTimeCrossTalkR;   //!
   TBranch        *b_GM02Y1___fPar_fSample;   //!
   TBranch        *b_GM02Y1___fIsHitSorted;   //!
   TBranch        *b_GM02Y1___fIsHitAmpSorted;   //!
   TBranch        *b_GM02Y1___fIsClusterSorted;   //!
   TBranch        *b_GM02Y1___fIsClusterized;   //!
   TBranch        *b_GM02Y1___fName;   //!
   TBranch        *b_SI01X1___fHeader_fEvtNum;   //!
   TBranch        *b_SI01X1___fHeader_fRun;   //!
   TBranch        *b_SI01X1___fHeader_fDate;   //!
   TBranch        *b_SI01X1___fPar_fClusMeth;   //!
   TBranch        *b_SI01X1___fPar_fClusConfigMask;   //!
   TBranch        *b_SI01X1___fPar_fShareHits;   //!
   TBranch        *b_SI01X1___fPar_fThrClus;   //!
   TBranch        *b_SI01X1___fPar_fThrHit;   //!
   TBranch        *b_SI01X1___fPar_fTimeCrossTalkR;   //!
   TBranch        *b_SI01X1___fPar_fSample;   //!
   TBranch        *b_SI01X1___fIsHitSorted;   //!
   TBranch        *b_SI01X1___fIsHitAmpSorted;   //!
   TBranch        *b_SI01X1___fIsClusterSorted;   //!
   TBranch        *b_SI01X1___fIsClusterized;   //!
   TBranch        *b_SI01X1___fName;   //!
   TBranch        *b_SI01Y1___fHeader_fEvtNum;   //!
   TBranch        *b_SI01Y1___fHeader_fRun;   //!
   TBranch        *b_SI01Y1___fHeader_fDate;   //!
   TBranch        *b_SI01Y1___fPar_fClusMeth;   //!
   TBranch        *b_SI01Y1___fPar_fClusConfigMask;   //!
   TBranch        *b_SI01Y1___fPar_fShareHits;   //!
   TBranch        *b_SI01Y1___fPar_fThrClus;   //!
   TBranch        *b_SI01Y1___fPar_fThrHit;   //!
   TBranch        *b_SI01Y1___fPar_fTimeCrossTalkR;   //!
   TBranch        *b_SI01Y1___fPar_fSample;   //!
   TBranch        *b_SI01Y1___fIsHitSorted;   //!
   TBranch        *b_SI01Y1___fIsHitAmpSorted;   //!
   TBranch        *b_SI01Y1___fIsClusterSorted;   //!
   TBranch        *b_SI01Y1___fIsClusterized;   //!
   TBranch        *b_SI01Y1___fName;   //!
   TBranch        *b_SI02X1___fHeader_fEvtNum;   //!
   TBranch        *b_SI02X1___fHeader_fRun;   //!
   TBranch        *b_SI02X1___fHeader_fDate;   //!
   TBranch        *b_SI02X1___fPar_fClusMeth;   //!
   TBranch        *b_SI02X1___fPar_fClusConfigMask;   //!
   TBranch        *b_SI02X1___fPar_fShareHits;   //!
   TBranch        *b_SI02X1___fPar_fThrClus;   //!
   TBranch        *b_SI02X1___fPar_fThrHit;   //!
   TBranch        *b_SI02X1___fPar_fTimeCrossTalkR;   //!
   TBranch        *b_SI02X1___fPar_fSample;   //!
   TBranch        *b_SI02X1___fIsHitSorted;   //!
   TBranch        *b_SI02X1___fIsHitAmpSorted;   //!
   TBranch        *b_SI02X1___fIsClusterSorted;   //!
   TBranch        *b_SI02X1___fIsClusterized;   //!
   TBranch        *b_SI02X1___fName;   //!
   TBranch        *b_SI02Y1___fHeader_fEvtNum;   //!
   TBranch        *b_SI02Y1___fHeader_fRun;   //!
   TBranch        *b_SI02Y1___fHeader_fDate;   //!
   TBranch        *b_SI02Y1___fPar_fClusMeth;   //!
   TBranch        *b_SI02Y1___fPar_fClusConfigMask;   //!
   TBranch        *b_SI02Y1___fPar_fShareHits;   //!
   TBranch        *b_SI02Y1___fPar_fThrClus;   //!
   TBranch        *b_SI02Y1___fPar_fThrHit;   //!
   TBranch        *b_SI02Y1___fPar_fTimeCrossTalkR;   //!
   TBranch        *b_SI02Y1___fPar_fSample;   //!
   TBranch        *b_SI02Y1___fIsHitSorted;   //!
   TBranch        *b_SI02Y1___fIsHitAmpSorted;   //!
   TBranch        *b_SI02Y1___fIsClusterSorted;   //!
   TBranch        *b_SI02Y1___fIsClusterized;   //!
   TBranch        *b_SI02Y1___fName;   //!

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
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("run-921.root");
      if (!f) {
         f = new TFile("run-921.root");
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

   fChain->SetBranchAddress("GM01X1__.fHeader.fEvtNum", &GM01X1___fHeader_fEvtNum, &b_GM01X1___fHeader_fEvtNum);
   fChain->SetBranchAddress("GM01X1__.fHeader.fRun", &GM01X1___fHeader_fRun, &b_GM01X1___fHeader_fRun);
   fChain->SetBranchAddress("GM01X1__.fHeader.fDate", &GM01X1___fHeader_fDate, &b_GM01X1___fHeader_fDate);
   fChain->SetBranchAddress("GM01X1__.fPar.fClusMeth", &GM01X1___fPar_fClusMeth, &b_GM01X1___fPar_fClusMeth);
   fChain->SetBranchAddress("GM01X1__.fPar.fClusConfigMask", &GM01X1___fPar_fClusConfigMask, &b_GM01X1___fPar_fClusConfigMask);
   fChain->SetBranchAddress("GM01X1__.fPar.fShareHits", &GM01X1___fPar_fShareHits, &b_GM01X1___fPar_fShareHits);
   fChain->SetBranchAddress("GM01X1__.fPar.fThrClus", &GM01X1___fPar_fThrClus, &b_GM01X1___fPar_fThrClus);
   fChain->SetBranchAddress("GM01X1__.fPar.fThrHit", &GM01X1___fPar_fThrHit, &b_GM01X1___fPar_fThrHit);
   fChain->SetBranchAddress("GM01X1__.fPar.fTimeCrossTalkR", &GM01X1___fPar_fTimeCrossTalkR, &b_GM01X1___fPar_fTimeCrossTalkR);
   fChain->SetBranchAddress("GM01X1__.fPar.fSample", &GM01X1___fPar_fSample, &b_GM01X1___fPar_fSample);
   fChain->SetBranchAddress("GM01X1__.fIsHitSorted", &GM01X1___fIsHitSorted, &b_GM01X1___fIsHitSorted);
   fChain->SetBranchAddress("GM01X1__.fIsHitAmpSorted", &GM01X1___fIsHitAmpSorted, &b_GM01X1___fIsHitAmpSorted);
   fChain->SetBranchAddress("GM01X1__.fIsClusterSorted", &GM01X1___fIsClusterSorted, &b_GM01X1___fIsClusterSorted);
   fChain->SetBranchAddress("GM01X1__.fIsClusterized", &GM01X1___fIsClusterized, &b_GM01X1___fIsClusterized);
   fChain->SetBranchAddress("GM01X1__.fName", &GM01X1___fName, &b_GM01X1___fName);
   fChain->SetBranchAddress("GM01Y1__.fHeader.fEvtNum", &GM01Y1___fHeader_fEvtNum, &b_GM01Y1___fHeader_fEvtNum);
   fChain->SetBranchAddress("GM01Y1__.fHeader.fRun", &GM01Y1___fHeader_fRun, &b_GM01Y1___fHeader_fRun);
   fChain->SetBranchAddress("GM01Y1__.fHeader.fDate", &GM01Y1___fHeader_fDate, &b_GM01Y1___fHeader_fDate);
   fChain->SetBranchAddress("GM01Y1__.fPar.fClusMeth", &GM01Y1___fPar_fClusMeth, &b_GM01Y1___fPar_fClusMeth);
   fChain->SetBranchAddress("GM01Y1__.fPar.fClusConfigMask", &GM01Y1___fPar_fClusConfigMask, &b_GM01Y1___fPar_fClusConfigMask);
   fChain->SetBranchAddress("GM01Y1__.fPar.fShareHits", &GM01Y1___fPar_fShareHits, &b_GM01Y1___fPar_fShareHits);
   fChain->SetBranchAddress("GM01Y1__.fPar.fThrClus", &GM01Y1___fPar_fThrClus, &b_GM01Y1___fPar_fThrClus);
   fChain->SetBranchAddress("GM01Y1__.fPar.fThrHit", &GM01Y1___fPar_fThrHit, &b_GM01Y1___fPar_fThrHit);
   fChain->SetBranchAddress("GM01Y1__.fPar.fTimeCrossTalkR", &GM01Y1___fPar_fTimeCrossTalkR, &b_GM01Y1___fPar_fTimeCrossTalkR);
   fChain->SetBranchAddress("GM01Y1__.fPar.fSample", &GM01Y1___fPar_fSample, &b_GM01Y1___fPar_fSample);
   fChain->SetBranchAddress("GM01Y1__.fIsHitSorted", &GM01Y1___fIsHitSorted, &b_GM01Y1___fIsHitSorted);
   fChain->SetBranchAddress("GM01Y1__.fIsHitAmpSorted", &GM01Y1___fIsHitAmpSorted, &b_GM01Y1___fIsHitAmpSorted);
   fChain->SetBranchAddress("GM01Y1__.fIsClusterSorted", &GM01Y1___fIsClusterSorted, &b_GM01Y1___fIsClusterSorted);
   fChain->SetBranchAddress("GM01Y1__.fIsClusterized", &GM01Y1___fIsClusterized, &b_GM01Y1___fIsClusterized);
   fChain->SetBranchAddress("GM01Y1__.fName", &GM01Y1___fName, &b_GM01Y1___fName);
   fChain->SetBranchAddress("GM02X1__.fHeader.fEvtNum", &GM02X1___fHeader_fEvtNum, &b_GM02X1___fHeader_fEvtNum);
   fChain->SetBranchAddress("GM02X1__.fHeader.fRun", &GM02X1___fHeader_fRun, &b_GM02X1___fHeader_fRun);
   fChain->SetBranchAddress("GM02X1__.fHeader.fDate", &GM02X1___fHeader_fDate, &b_GM02X1___fHeader_fDate);
   fChain->SetBranchAddress("GM02X1__.fPar.fClusMeth", &GM02X1___fPar_fClusMeth, &b_GM02X1___fPar_fClusMeth);
   fChain->SetBranchAddress("GM02X1__.fPar.fClusConfigMask", &GM02X1___fPar_fClusConfigMask, &b_GM02X1___fPar_fClusConfigMask);
   fChain->SetBranchAddress("GM02X1__.fPar.fShareHits", &GM02X1___fPar_fShareHits, &b_GM02X1___fPar_fShareHits);
   fChain->SetBranchAddress("GM02X1__.fPar.fThrClus", &GM02X1___fPar_fThrClus, &b_GM02X1___fPar_fThrClus);
   fChain->SetBranchAddress("GM02X1__.fPar.fThrHit", &GM02X1___fPar_fThrHit, &b_GM02X1___fPar_fThrHit);
   fChain->SetBranchAddress("GM02X1__.fPar.fTimeCrossTalkR", &GM02X1___fPar_fTimeCrossTalkR, &b_GM02X1___fPar_fTimeCrossTalkR);
   fChain->SetBranchAddress("GM02X1__.fPar.fSample", &GM02X1___fPar_fSample, &b_GM02X1___fPar_fSample);
   fChain->SetBranchAddress("GM02X1__.fIsHitSorted", &GM02X1___fIsHitSorted, &b_GM02X1___fIsHitSorted);
   fChain->SetBranchAddress("GM02X1__.fIsHitAmpSorted", &GM02X1___fIsHitAmpSorted, &b_GM02X1___fIsHitAmpSorted);
   fChain->SetBranchAddress("GM02X1__.fIsClusterSorted", &GM02X1___fIsClusterSorted, &b_GM02X1___fIsClusterSorted);
   fChain->SetBranchAddress("GM02X1__.fIsClusterized", &GM02X1___fIsClusterized, &b_GM02X1___fIsClusterized);
   fChain->SetBranchAddress("GM02X1__.fName", &GM02X1___fName, &b_GM02X1___fName);
   fChain->SetBranchAddress("GM02Y1__.fHeader.fEvtNum", &GM02Y1___fHeader_fEvtNum, &b_GM02Y1___fHeader_fEvtNum);
   fChain->SetBranchAddress("GM02Y1__.fHeader.fRun", &GM02Y1___fHeader_fRun, &b_GM02Y1___fHeader_fRun);
   fChain->SetBranchAddress("GM02Y1__.fHeader.fDate", &GM02Y1___fHeader_fDate, &b_GM02Y1___fHeader_fDate);
   fChain->SetBranchAddress("GM02Y1__.fPar.fClusMeth", &GM02Y1___fPar_fClusMeth, &b_GM02Y1___fPar_fClusMeth);
   fChain->SetBranchAddress("GM02Y1__.fPar.fClusConfigMask", &GM02Y1___fPar_fClusConfigMask, &b_GM02Y1___fPar_fClusConfigMask);
   fChain->SetBranchAddress("GM02Y1__.fPar.fShareHits", &GM02Y1___fPar_fShareHits, &b_GM02Y1___fPar_fShareHits);
   fChain->SetBranchAddress("GM02Y1__.fPar.fThrClus", &GM02Y1___fPar_fThrClus, &b_GM02Y1___fPar_fThrClus);
   fChain->SetBranchAddress("GM02Y1__.fPar.fThrHit", &GM02Y1___fPar_fThrHit, &b_GM02Y1___fPar_fThrHit);
   fChain->SetBranchAddress("GM02Y1__.fPar.fTimeCrossTalkR", &GM02Y1___fPar_fTimeCrossTalkR, &b_GM02Y1___fPar_fTimeCrossTalkR);
   fChain->SetBranchAddress("GM02Y1__.fPar.fSample", &GM02Y1___fPar_fSample, &b_GM02Y1___fPar_fSample);
   fChain->SetBranchAddress("GM02Y1__.fIsHitSorted", &GM02Y1___fIsHitSorted, &b_GM02Y1___fIsHitSorted);
   fChain->SetBranchAddress("GM02Y1__.fIsHitAmpSorted", &GM02Y1___fIsHitAmpSorted, &b_GM02Y1___fIsHitAmpSorted);
   fChain->SetBranchAddress("GM02Y1__.fIsClusterSorted", &GM02Y1___fIsClusterSorted, &b_GM02Y1___fIsClusterSorted);
   fChain->SetBranchAddress("GM02Y1__.fIsClusterized", &GM02Y1___fIsClusterized, &b_GM02Y1___fIsClusterized);
   fChain->SetBranchAddress("GM02Y1__.fName", &GM02Y1___fName, &b_GM02Y1___fName);
   fChain->SetBranchAddress("SI01X1__.fHeader.fEvtNum", &SI01X1___fHeader_fEvtNum, &b_SI01X1___fHeader_fEvtNum);
   fChain->SetBranchAddress("SI01X1__.fHeader.fRun", &SI01X1___fHeader_fRun, &b_SI01X1___fHeader_fRun);
   fChain->SetBranchAddress("SI01X1__.fHeader.fDate", &SI01X1___fHeader_fDate, &b_SI01X1___fHeader_fDate);
   fChain->SetBranchAddress("SI01X1__.fPar.fClusMeth", &SI01X1___fPar_fClusMeth, &b_SI01X1___fPar_fClusMeth);
   fChain->SetBranchAddress("SI01X1__.fPar.fClusConfigMask", &SI01X1___fPar_fClusConfigMask, &b_SI01X1___fPar_fClusConfigMask);
   fChain->SetBranchAddress("SI01X1__.fPar.fShareHits", &SI01X1___fPar_fShareHits, &b_SI01X1___fPar_fShareHits);
   fChain->SetBranchAddress("SI01X1__.fPar.fThrClus", &SI01X1___fPar_fThrClus, &b_SI01X1___fPar_fThrClus);
   fChain->SetBranchAddress("SI01X1__.fPar.fThrHit", &SI01X1___fPar_fThrHit, &b_SI01X1___fPar_fThrHit);
   fChain->SetBranchAddress("SI01X1__.fPar.fTimeCrossTalkR", &SI01X1___fPar_fTimeCrossTalkR, &b_SI01X1___fPar_fTimeCrossTalkR);
   fChain->SetBranchAddress("SI01X1__.fPar.fSample", &SI01X1___fPar_fSample, &b_SI01X1___fPar_fSample);
   fChain->SetBranchAddress("SI01X1__.fIsHitSorted", &SI01X1___fIsHitSorted, &b_SI01X1___fIsHitSorted);
   fChain->SetBranchAddress("SI01X1__.fIsHitAmpSorted", &SI01X1___fIsHitAmpSorted, &b_SI01X1___fIsHitAmpSorted);
   fChain->SetBranchAddress("SI01X1__.fIsClusterSorted", &SI01X1___fIsClusterSorted, &b_SI01X1___fIsClusterSorted);
   fChain->SetBranchAddress("SI01X1__.fIsClusterized", &SI01X1___fIsClusterized, &b_SI01X1___fIsClusterized);
   fChain->SetBranchAddress("SI01X1__.fName", &SI01X1___fName, &b_SI01X1___fName);
   fChain->SetBranchAddress("SI01Y1__.fHeader.fEvtNum", &SI01Y1___fHeader_fEvtNum, &b_SI01Y1___fHeader_fEvtNum);
   fChain->SetBranchAddress("SI01Y1__.fHeader.fRun", &SI01Y1___fHeader_fRun, &b_SI01Y1___fHeader_fRun);
   fChain->SetBranchAddress("SI01Y1__.fHeader.fDate", &SI01Y1___fHeader_fDate, &b_SI01Y1___fHeader_fDate);
   fChain->SetBranchAddress("SI01Y1__.fPar.fClusMeth", &SI01Y1___fPar_fClusMeth, &b_SI01Y1___fPar_fClusMeth);
   fChain->SetBranchAddress("SI01Y1__.fPar.fClusConfigMask", &SI01Y1___fPar_fClusConfigMask, &b_SI01Y1___fPar_fClusConfigMask);
   fChain->SetBranchAddress("SI01Y1__.fPar.fShareHits", &SI01Y1___fPar_fShareHits, &b_SI01Y1___fPar_fShareHits);
   fChain->SetBranchAddress("SI01Y1__.fPar.fThrClus", &SI01Y1___fPar_fThrClus, &b_SI01Y1___fPar_fThrClus);
   fChain->SetBranchAddress("SI01Y1__.fPar.fThrHit", &SI01Y1___fPar_fThrHit, &b_SI01Y1___fPar_fThrHit);
   fChain->SetBranchAddress("SI01Y1__.fPar.fTimeCrossTalkR", &SI01Y1___fPar_fTimeCrossTalkR, &b_SI01Y1___fPar_fTimeCrossTalkR);
   fChain->SetBranchAddress("SI01Y1__.fPar.fSample", &SI01Y1___fPar_fSample, &b_SI01Y1___fPar_fSample);
   fChain->SetBranchAddress("SI01Y1__.fIsHitSorted", &SI01Y1___fIsHitSorted, &b_SI01Y1___fIsHitSorted);
   fChain->SetBranchAddress("SI01Y1__.fIsHitAmpSorted", &SI01Y1___fIsHitAmpSorted, &b_SI01Y1___fIsHitAmpSorted);
   fChain->SetBranchAddress("SI01Y1__.fIsClusterSorted", &SI01Y1___fIsClusterSorted, &b_SI01Y1___fIsClusterSorted);
   fChain->SetBranchAddress("SI01Y1__.fIsClusterized", &SI01Y1___fIsClusterized, &b_SI01Y1___fIsClusterized);
   fChain->SetBranchAddress("SI01Y1__.fName", &SI01Y1___fName, &b_SI01Y1___fName);
   fChain->SetBranchAddress("SI02X1__.fHeader.fEvtNum", &SI02X1___fHeader_fEvtNum, &b_SI02X1___fHeader_fEvtNum);
   fChain->SetBranchAddress("SI02X1__.fHeader.fRun", &SI02X1___fHeader_fRun, &b_SI02X1___fHeader_fRun);
   fChain->SetBranchAddress("SI02X1__.fHeader.fDate", &SI02X1___fHeader_fDate, &b_SI02X1___fHeader_fDate);
   fChain->SetBranchAddress("SI02X1__.fPar.fClusMeth", &SI02X1___fPar_fClusMeth, &b_SI02X1___fPar_fClusMeth);
   fChain->SetBranchAddress("SI02X1__.fPar.fClusConfigMask", &SI02X1___fPar_fClusConfigMask, &b_SI02X1___fPar_fClusConfigMask);
   fChain->SetBranchAddress("SI02X1__.fPar.fShareHits", &SI02X1___fPar_fShareHits, &b_SI02X1___fPar_fShareHits);
   fChain->SetBranchAddress("SI02X1__.fPar.fThrClus", &SI02X1___fPar_fThrClus, &b_SI02X1___fPar_fThrClus);
   fChain->SetBranchAddress("SI02X1__.fPar.fThrHit", &SI02X1___fPar_fThrHit, &b_SI02X1___fPar_fThrHit);
   fChain->SetBranchAddress("SI02X1__.fPar.fTimeCrossTalkR", &SI02X1___fPar_fTimeCrossTalkR, &b_SI02X1___fPar_fTimeCrossTalkR);
   fChain->SetBranchAddress("SI02X1__.fPar.fSample", &SI02X1___fPar_fSample, &b_SI02X1___fPar_fSample);
   fChain->SetBranchAddress("SI02X1__.fIsHitSorted", &SI02X1___fIsHitSorted, &b_SI02X1___fIsHitSorted);
   fChain->SetBranchAddress("SI02X1__.fIsHitAmpSorted", &SI02X1___fIsHitAmpSorted, &b_SI02X1___fIsHitAmpSorted);
   fChain->SetBranchAddress("SI02X1__.fIsClusterSorted", &SI02X1___fIsClusterSorted, &b_SI02X1___fIsClusterSorted);
   fChain->SetBranchAddress("SI02X1__.fIsClusterized", &SI02X1___fIsClusterized, &b_SI02X1___fIsClusterized);
   fChain->SetBranchAddress("SI02X1__.fName", &SI02X1___fName, &b_SI02X1___fName);
   fChain->SetBranchAddress("SI02Y1__.fHeader.fEvtNum", &SI02Y1___fHeader_fEvtNum, &b_SI02Y1___fHeader_fEvtNum);
   fChain->SetBranchAddress("SI02Y1__.fHeader.fRun", &SI02Y1___fHeader_fRun, &b_SI02Y1___fHeader_fRun);
   fChain->SetBranchAddress("SI02Y1__.fHeader.fDate", &SI02Y1___fHeader_fDate, &b_SI02Y1___fHeader_fDate);
   fChain->SetBranchAddress("SI02Y1__.fPar.fClusMeth", &SI02Y1___fPar_fClusMeth, &b_SI02Y1___fPar_fClusMeth);
   fChain->SetBranchAddress("SI02Y1__.fPar.fClusConfigMask", &SI02Y1___fPar_fClusConfigMask, &b_SI02Y1___fPar_fClusConfigMask);
   fChain->SetBranchAddress("SI02Y1__.fPar.fShareHits", &SI02Y1___fPar_fShareHits, &b_SI02Y1___fPar_fShareHits);
   fChain->SetBranchAddress("SI02Y1__.fPar.fThrClus", &SI02Y1___fPar_fThrClus, &b_SI02Y1___fPar_fThrClus);
   fChain->SetBranchAddress("SI02Y1__.fPar.fThrHit", &SI02Y1___fPar_fThrHit, &b_SI02Y1___fPar_fThrHit);
   fChain->SetBranchAddress("SI02Y1__.fPar.fTimeCrossTalkR", &SI02Y1___fPar_fTimeCrossTalkR, &b_SI02Y1___fPar_fTimeCrossTalkR);
   fChain->SetBranchAddress("SI02Y1__.fPar.fSample", &SI02Y1___fPar_fSample, &b_SI02Y1___fPar_fSample);
   fChain->SetBranchAddress("SI02Y1__.fIsHitSorted", &SI02Y1___fIsHitSorted, &b_SI02Y1___fIsHitSorted);
   fChain->SetBranchAddress("SI02Y1__.fIsHitAmpSorted", &SI02Y1___fIsHitAmpSorted, &b_SI02Y1___fIsHitAmpSorted);
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
