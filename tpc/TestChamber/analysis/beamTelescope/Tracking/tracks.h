//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sun Aug 16 17:37:51 2009 by ROOT version 5.24/00
// from TTree at2/testBench analysis tree
// found on file: 2track002.root
//////////////////////////////////////////////////////////

#ifndef tracks_h
#define tracks_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
   const Int_t kMaxclusters = 1;

class tracks {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
 //TCevent         *event;
   UInt_t          fUniqueID;
   UInt_t          fBits;
 //vector<TCtrack*> tracks;
   Int_t           clusters_;
   UInt_t          clusters_fUniqueID[kMaxclusters];   //[clusters_]
   UInt_t          clusters_fBits[kMaxclusters];   //[clusters_]
   Int_t           clusters_detId[kMaxclusters];   //[clusters_]
   UInt_t          clusters_pos_fUniqueID[kMaxclusters];   //[clusters_]
   UInt_t          clusters_pos_fBits[kMaxclusters];   //[clusters_]
   Double_t        clusters_pos_fX[kMaxclusters];   //[clusters_]
   Double_t        clusters_pos_fY[kMaxclusters];   //[clusters_]
   Double_t        clusters_pos_fZ[kMaxclusters];   //[clusters_]
   UInt_t          clusters_err_fUniqueID[kMaxclusters];   //[clusters_]
   UInt_t          clusters_err_fBits[kMaxclusters];   //[clusters_]
   Double_t        clusters_err_fX[kMaxclusters];   //[clusters_]
   Double_t        clusters_err_fY[kMaxclusters];   //[clusters_]
   Double_t        clusters_err_fZ[kMaxclusters];   //[clusters_]
   UInt_t          clusters_res_fUniqueID[kMaxclusters];   //[clusters_]
   UInt_t          clusters_res_fBits[kMaxclusters];   //[clusters_]
   Double_t        clusters_res_fX[kMaxclusters];   //[clusters_]
   Double_t        clusters_res_fY[kMaxclusters];   //[clusters_]
   Double_t        clusters_res_fZ[kMaxclusters];   //[clusters_]
   Double_t        clusters_amp[kMaxclusters];   //[clusters_]
   Bool_t          clusters_fit[kMaxclusters];   //[clusters_]
   Double_t        clusters_pedestalRMS[kMaxclusters];   //[clusters_]
 //vector<TCcluster> clusters_raw[kMaxclusters];

   // List of branches
   TBranch        *b_event_fUniqueID;   //!
   TBranch        *b_event_fBits;   //!
   TBranch        *b_event_clusters_;   //!
   TBranch        *b_clusters_fUniqueID;   //!
   TBranch        *b_clusters_fBits;   //!
   TBranch        *b_clusters_detId;   //!
   TBranch        *b_clusters_pos_fUniqueID;   //!
   TBranch        *b_clusters_pos_fBits;   //!
   TBranch        *b_clusters_pos_fX;   //!
   TBranch        *b_clusters_pos_fY;   //!
   TBranch        *b_clusters_pos_fZ;   //!
   TBranch        *b_clusters_err_fUniqueID;   //!
   TBranch        *b_clusters_err_fBits;   //!
   TBranch        *b_clusters_err_fX;   //!
   TBranch        *b_clusters_err_fY;   //!
   TBranch        *b_clusters_err_fZ;   //!
   TBranch        *b_clusters_res_fUniqueID;   //!
   TBranch        *b_clusters_res_fBits;   //!
   TBranch        *b_clusters_res_fX;   //!
   TBranch        *b_clusters_res_fY;   //!
   TBranch        *b_clusters_res_fZ;   //!
   TBranch        *b_clusters_amp;   //!
   TBranch        *b_clusters_fit;   //!
   TBranch        *b_clusters_pedestalRMS;   //!

   tracks(TTree *tree=0);
   virtual ~tracks();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef tracks_cxx
tracks::tracks(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("2track002.root");
      if (!f) {
         f = new TFile("2track002.root");
      }
      tree = (TTree*)gDirectory->Get("at2");

   }
   Init(tree);
}

tracks::~tracks()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t tracks::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t tracks::LoadTree(Long64_t entry)
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

void tracks::Init(TTree *tree)
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

   fChain->SetBranchAddress("fUniqueID", &fUniqueID, &b_event_fUniqueID);
   fChain->SetBranchAddress("fBits", &fBits, &b_event_fBits);
   fChain->SetBranchAddress("clusters", &clusters_, &b_event_clusters_);
   fChain->SetBranchAddress("clusters.fUniqueID", &clusters_fUniqueID, &b_clusters_fUniqueID);
   fChain->SetBranchAddress("clusters.fBits", &clusters_fBits, &b_clusters_fBits);
   fChain->SetBranchAddress("clusters.detId", &clusters_detId, &b_clusters_detId);
   fChain->SetBranchAddress("clusters.pos.fUniqueID", &clusters_pos_fUniqueID, &b_clusters_pos_fUniqueID);
   fChain->SetBranchAddress("clusters.pos.fBits", &clusters_pos_fBits, &b_clusters_pos_fBits);
   fChain->SetBranchAddress("clusters.pos.fX", &clusters_pos_fX, &b_clusters_pos_fX);
   fChain->SetBranchAddress("clusters.pos.fY", &clusters_pos_fY, &b_clusters_pos_fY);
   fChain->SetBranchAddress("clusters.pos.fZ", &clusters_pos_fZ, &b_clusters_pos_fZ);
   fChain->SetBranchAddress("clusters.err.fUniqueID", &clusters_err_fUniqueID, &b_clusters_err_fUniqueID);
   fChain->SetBranchAddress("clusters.err.fBits", &clusters_err_fBits, &b_clusters_err_fBits);
   fChain->SetBranchAddress("clusters.err.fX", &clusters_err_fX, &b_clusters_err_fX);
   fChain->SetBranchAddress("clusters.err.fY", &clusters_err_fY, &b_clusters_err_fY);
   fChain->SetBranchAddress("clusters.err.fZ", &clusters_err_fZ, &b_clusters_err_fZ);
   fChain->SetBranchAddress("clusters.res.fUniqueID", &clusters_res_fUniqueID, &b_clusters_res_fUniqueID);
   fChain->SetBranchAddress("clusters.res.fBits", &clusters_res_fBits, &b_clusters_res_fBits);
   fChain->SetBranchAddress("clusters.res.fX", &clusters_res_fX, &b_clusters_res_fX);
   fChain->SetBranchAddress("clusters.res.fY", &clusters_res_fY, &b_clusters_res_fY);
   fChain->SetBranchAddress("clusters.res.fZ", &clusters_res_fZ, &b_clusters_res_fZ);
   fChain->SetBranchAddress("clusters.amp", &clusters_amp, &b_clusters_amp);
   fChain->SetBranchAddress("clusters.fit", &clusters_fit, &b_clusters_fit);
   fChain->SetBranchAddress("clusters.pedestalRMS", &clusters_pedestalRMS, &b_clusters_pedestalRMS);
   Notify();
}

Bool_t tracks::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void tracks::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t tracks::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef tracks_cxx
