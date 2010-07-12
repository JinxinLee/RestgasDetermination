//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Apr 23 09:04:05 2010 by ROOT version 5.26/00
// from TTree Hits/coordinate of hits
// found on file: tmpOutput/Lumi_out_0.root
//////////////////////////////////////////////////////////

#ifndef ReadTree_h
#define ReadTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TVector3.h>
#include <sstream>
using namespace std; 

const double M_PI = TMath::Pi();
class ReadTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Double_t        fXv;
   Double_t        fYv;
   Double_t        fZv;
   Double_t        ferrXv;
   Double_t        ferrYv;
   Double_t        ferrZv;
   Double_t        fPx;
   Double_t        fPy;
   Double_t        fPz;
   Double_t        ferrPx;
   Double_t        ferrPy;
   Double_t        ferrPz;
   Double_t        fPxMC;
   Double_t        fPyMC;
   Double_t        fPzMC;
 
   TBranch        *b_Xv;   //!
   TBranch        *b_Yv;   //!
   TBranch        *b_Zv;   //!
   TBranch        *b_errXv;   //!
   TBranch        *b_errYv;   //!
   TBranch        *b_errZv;   //!
   TBranch        *b_Px;   //!
   TBranch        *b_Py;   //!
   TBranch        *b_Pz;   //!
   TBranch        *b_errPx;   //!
   TBranch        *b_errPy;   //!
   TBranch        *b_errPz;   //!
   TBranch        *b_PxMC;   //!
   TBranch        *b_PyMC;   //!
   TBranch        *b_PzMC;   //!

   TH2F *ThetaRecvsThetaMC;
   TH2F *PhiRecvsPhiMC;
   TH2F *dTheta;
   TH2F *dPhi;
   TH2F *dThetavsPhi;
   TH2F *ThetavsPhi;
   TH2F *ThetavsPhiMC;
   TH2F *dThetavsdPhi;
   TH2F *dMomMomMC;

   TH1F *vertexX; TH1F *vertexY; TH1F *vertexZ;
   TH1F *ErrvertexX; TH1F *ErrvertexY; TH1F *ErrvertexZ;
   TH1F *momX; TH1F *momY; TH1F *momZ;
   TH1F *ResmomX; TH1F *ResmomY; TH1F *ResmomZ;
   TH1F *ErrmomX; TH1F *ErrmomY; TH1F *ErrmomZ;
   TH2F *sigmaPxvsPhi;  TH2F *sigmaPxvsTheta;
   TH2F *sigmaXvsPhi;  TH2F *sigmaXvsTheta;
   TH2F *sigmaPxvssigmaX;
   TFile *fout;

   ReadTree(TTree *tree=0);
   virtual ~ReadTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef ReadTree_cxx
ReadTree::ReadTree(TTree *tree)
{
  
  // if parameter tree is not specified (or zero), connect the file
  // used to generate this class and read the Tree.
  if (tree == 0) {
    TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("tmpOutput/Lumi_out_tracks_0.root");
    if (!f) {
      f = new TFile("tmpOutput/Lumi_out_tracks_0.root");
    }
    tree = (TTree*)gDirectory->Get("Tracks");

   }
   Init(tree);

   dTheta = new TH2F("diffTheta","#theta_{REC}-#theta_{MC};#theta_{MC}, rad;#Delta #theta, rad;",
   		 50, 0., 0.01, 100, -3e-2, 3e-2);
   
   dPhi = new TH2F("diffPhi","#phi_{REC}-#phi_{MC};#phi_{MC}, rad;#Delta #phi, rad;",
   	       50, 0*M_PI-1e-5, 2*M_PI+1e-5, 1e3, -2e-1, 2e-1);
   dThetavsPhi = new TH2F("diffThetaPhi","#theta;#phi_{MC}, rad;#Delta #theta, rad;",
   			100, -1e-7, +1e-7, 1000, -2e-1, 2e-1);
   ThetavsPhi = new TH2F("ThetaPhi","#theta;#phi_{Rec}, rad;#theta_{Rec}, rad;",
   		     100, -1e-7, 1e-7, 1000,  5.235e-3, 5.236e-3);
   ThetavsPhiMC = new TH2F("ThetaPhiMC","#theta;#phi_{MC}, rad;#theta_{MC}, rad;",
   			100, -1e-7, 1e-7, 1000, 5.235e-3, 5.236e-3);
   dThetavsdPhi = new TH2F("diffThetadiffPhi","#Delta#theta vs. #Delta#phi;#Delta#phi, rad;#Delta #theta, rad;",
   		      1000, -1e-6, 1e-6, 1000, -5.5e-6, -5e-6);

   vertexX = new TH1F("VertexX","X; x, cm;",1e3,-5e-1,5e-1);
   vertexY = new TH1F("VertexY","Y; y, cm;",1e3,-5e-2,5e-1);
   vertexZ = new TH1F("VertexZ","Z; z, cm;",1e3,-5e-1,5e-1);
   ErrvertexX = new TH1F("ErrVertexX","ErrX; #sigma_{x}, cm;",1e4,-1e-1,1e-1);
   ErrvertexY = new TH1F("ErrVertexY","ErrY; #sigma_{y}, cm;",1e4,-1e-1,1e-1);
   ErrvertexZ = new TH1F("ErrVertexZ","ErrZ; #sigma_{z}, cm;",1e4,-1e-1,1e-1);
   momX = new TH1F("MomX","Px; Px, GeV/c;",200,-1e-1,1e-1);
   momY = new TH1F("MomY","Py; Py, GeV/c;",200,-1e-1,1e-1);
   momZ = new TH1F("MomZ","Pz; Pz, Gev/c;",200,8.9-1e-1,8.9+1e-1);
   ErrmomX = new TH1F("ErrMomX","ErrPx; #sigma_{Px}, GeV/c;",1e4,-1e-1,1e-1);
   ErrmomY = new TH1F("ErrMomY","ErrPy; #sigma_{Py}, GeV/c;",1e4,-1e-1,1e-1);
   ErrmomZ = new TH1F("ErrMomZ","ErrPz; #sigma_{Pz}, GeV/c;",1e4,-1e-1,1e-1);
   ResmomX = new TH1F("ResMomX","Px_{REC}-Px_{MC}; #DeltaPx, GeV/c;",1e4,-2e-1,2e-1);
   ResmomY = new TH1F("ResMomY","Py_{REC}-Py_{MC}; #DeltaPy, GeV/c;",1e4,-2e-1,2e-1);
   ResmomZ = new TH1F("ResMomZ","Pz_{REC}-Pz_{MC}; #DeltaPz, Gev/c;",1e4,-2e-1,2e-1);
   dMomMomMC = new TH2F("dMom","#Delta  P; P_{MC}, GeV/c;#Delta P, GeV/c",1000,8.8,9.,1000,-3e-2,3e-2);

   sigmaPxvsPhi = new TH2F("sPxPhi",";#phi_{rec}, rad;#sigma_{Px}, GeV/c",1e3,0*M_PI-1e-5, 2*M_PI+1e-5, 1e3,-1e-3,5e-3); 
   sigmaPxvsTheta = new TH2F("sPxTheta",";#theta_{rec}, rad;#sigma_{Px}, GeV/c", 50, 0., 0.01, 1e3, -1e-3,5e-3);
   sigmaXvsPhi = new TH2F("sXPhi",";#phi_{rec}, rad;#sigma_{X}, cm", 1e2,0*M_PI-1e-5, 2*M_PI+1e-5, 1e2,-1e-3,7e-2); 
   sigmaXvsTheta = new TH2F("sXTheta",";#theta_{rec}, rad;#sigma_{X}, cm", 50, 0., 0.01, 1e2, -1e-3,7e-2);
   sigmaPxvssigmaX = new TH2F("sPxX",";#sigma_{X}, cm;#sigma_{Px}, GeV/c", 1e2, -1e-3,7e-2, 1e3, -1e-3,5e-3);
   /* dTheta = new TH2F("diffTheta","#theta;#theta_{MC}, rad;#Delta #theta, rad;", */
   /* 		 100, 0., 0.01, 1000, -2e-9, 2e-9); */
   
   /* dPhi = new TH2F("diffPhi","#phi;#phi_{MC}, rad;#Delta #phi, rad;", */
   /* 	       100, 0*M_PI-1e-5, 2*M_PI+1e-5, 1000, -2e-7, 2e-7); */
   /* dThetavsPhi = new TH2F("diffThetaPhi","#theta;#phi_{MC}, rad;#Delta #theta, rad;", */
   /* 			100, -1e-7, +1e-7, 1000, -5.5e-6, -5e-6); */
   /* ThetavsPhi = new TH2F("ThetaPhi","#theta;#phi_{Rec}, rad;#theta_{Rec}, rad;", */
   /* 		     100, -1e-7, 1e-7, 1000,  5.235e-3, 5.236e-3); */
   /* ThetavsPhiMC = new TH2F("ThetaPhiMC","#theta;#phi_{MC}, rad;#theta_{MC}, rad;", */
   /* 			100, -1e-7, 1e-7, 1000, 5.235e-3, 5.236e-3); */
   /* dThetavsdPhi = new TH2F("diffThetadiffPhi","#Delta#theta vs. #Delta#phi;#Delta#phi, rad;#Delta #theta, rad;", */
   /* 		      1000, -1e-6, 1e-6, 1000, -5.5e-6, -5e-6); */

   /* vertexX = new TH1F("VertexX","X; x, cm;",1000,-1e-4,1e-4); */
   /* vertexY = new TH1F("VertexY","Y; y, cm;",1000,-1e-4,1e-4); */
   /* vertexZ = new TH1F("VertexZ","Z; z, cm;",100,-2e-4,2e-4); */
   /* ErrvertexX = new TH1F("ErrVertexX","ErrX; #sigma_{x}, cm;",1000,-1e-1,1e-1); */
   /* ErrvertexY = new TH1F("ErrVertexY","ErrY; #sigma_{y}, cm;",1000,-1e-1,1e-1); */
   /* ErrvertexZ = new TH1F("ErrVertexZ","ErrZ; #sigma_{z}, cm;",1000,-1e-3,1e-3); */
   /* momX = new TH1F("MomX","Px; Px, GeV/c;",400,-0.1,0.1); */
   /* momY = new TH1F("MomY","Py; Py, GeV/c;",400,-0.1,0.1); */
   /* momZ = new TH1F("MomZ","Pz; Pz, Gev/c;",1000,8.9-0.01,8.9+0.01); */
   /* ErrmomX = new TH1F("ErrMomX","ErrPx; #sigma_{Px}, GeV/c;",1000,-1e-3,1e-3); */
   /* ErrmomY = new TH1F("ErrMomY","ErrPy; #sigma_{Py}, GeV/c;",1000,-1e-3,1e-3); */
   /* ErrmomZ = new TH1F("ErrMomZ","ErrPz; #sigma_{Pz}, GeV/c;",1000,-1e-3,1e-3); */
   /* ResmomX = new TH1F("ResMomX","Px_{REC}-Px_{MC}; #DeltaPx, GeV/c;",100,-4e-7,4e-7); */
   /* ResmomY = new TH1F("ResMomY","Py_{REC}-Py_{MC}; #DeltaPy, GeV/c;",100,-4e-7,4e-7); */
   /* ResmomZ = new TH1F("ResMomZ","Pz_{REC}-Pz_{MC}; #DeltaPz, Gev/c;",100,-4e-7,4e-7); */
   /* dMomMomMC = new TH2F("dMom","#Delta  P; P_{MC}, GeV/c;#Delta P, GeV/c",1000,8.8999,8.9001,1000,-1e-4,1e-4); */

 
}

ReadTree::~ReadTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t ReadTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t ReadTree::LoadTree(Long64_t entry)
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

void ReadTree::Init(TTree *tree)
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
   fChain->SetBranchAddress("Xv", &fXv, &b_Xv);
   fChain->SetBranchAddress("Yv", &fYv, &b_Yv);
   fChain->SetBranchAddress("Zv", &fZv, &b_Zv);
   fChain->SetBranchAddress("errXv", &ferrXv, &b_errXv);
   fChain->SetBranchAddress("errYv", &ferrYv, &b_errYv);
   fChain->SetBranchAddress("errZv", &ferrZv, &b_errZv);
   fChain->SetBranchAddress("Px", &fPx, &b_Px);
   fChain->SetBranchAddress("Py", &fPy, &b_Py);
   fChain->SetBranchAddress("Pz", &fPz, &b_Pz);
   fChain->SetBranchAddress("PxMC", &fPxMC, &b_PxMC);
   fChain->SetBranchAddress("PyMC", &fPyMC, &b_PyMC);
   fChain->SetBranchAddress("PzMC", &fPzMC, &b_PzMC);
   fChain->SetBranchAddress("errPx", &ferrPx, &b_errPx);
   fChain->SetBranchAddress("errPy", &ferrPy, &b_errPy);
   fChain->SetBranchAddress("errPz", &ferrPz, &b_errPz);
   cout<<"INIT!"<<endl;
   Notify();
}

Bool_t ReadTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void ReadTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t ReadTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef ReadTree_cxx
