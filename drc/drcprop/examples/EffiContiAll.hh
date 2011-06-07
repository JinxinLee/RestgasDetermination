//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon May 30 13:54:43 2011 by ROOT version 5.26/00
// from TTree photon//d/panda02/rhohler/beamtest1106/test/beamtest1106_f250_lens9_gap20_test_FSoil_theta0.root
// found on file: beamtest1106_f250_lens9_gap20_test_FSoil_theta0.root
//////////////////////////////////////////////////////////

#ifndef EffiContiAll_h
#define EffiContiAll_h

#include <iostream>
using namespace std;

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include "TString.h"
#include "TTree.h"
#include "TRandom3.h"

class EffiContiAll : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // Declaration of leaf types
   Double_t        wavelength;
   Int_t           color;
   Double_t        kBarX;
   Double_t        kBarY;
   Double_t        kBarZ;
   Double_t        hitPosX;
   Double_t        hitPosY;
   Double_t        hitPosZ;
   Double_t        hitPosDetX;
   Double_t        hitPosDetY;
   Double_t        hitPosDetZ;
   Double_t        hitDirX;
   Double_t        hitDirY;
   Double_t        hitDirZ;
   Double_t        posX[1100];
   Double_t        posY[1100];
   Double_t        posZ[1100];
   Int_t           index_pos;
   Int_t           particleID;
   Double_t        thetaC;
   Double_t        phiC;
   Bool_t          measured;
   Bool_t          absorbed;
   Bool_t          lost;
   Double_t        time;
   Int_t           nRefl;

   // List of branches
   TBranch        *b_wavelength;   //!
   TBranch        *b_color;   //!
   TBranch        *b_kBarX;   //!
   TBranch        *b_kBarY;   //!
   TBranch        *b_kBarZ;   //!
   TBranch        *b_hitPosX;   //!
   TBranch        *b_hitPosY;   //!
   TBranch        *b_hitPosZ;   //!
   TBranch        *b_hitPosDetX;   //!
   TBranch        *b_hitPosDetY;   //!
   TBranch        *b_hitPosDetZ;   //!
   TBranch        *b_hitDirX;   //!
   TBranch        *b_hitDirY;   //!
   TBranch        *b_hitDirZ;   //!
   TBranch        *b_posX;   //!
   TBranch        *b_posY;   //!
   TBranch        *b_posZ;   //!
   TBranch        *b_index_pos;   //!
   TBranch        *b_particleID;   //!
   TBranch        *b_thetaC;   //!
   TBranch        *b_phiC;   //!
   TBranch        *b_measured;   //!
   TBranch        *b_absorbed;   //!
   TBranch        *b_lost;   //!
   TBranch        *b_time;   //!
   TBranch        *b_nRefl;   //!

//    EffiContiAll(TTree * /*tree*/ =0) { }
   EffiContiAll();
   virtual ~EffiContiAll() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();


//==========
// my stuff
//==========
   virtual void ClearOutTree();

   Bool_t _notify;

   TFile *_outFile;
   TTree *_outTree;

   Double_t _effi[501]; // index is the wavelength in nm + 200 (min_wave)

   TRandom3 _rand;

   Double_t _tiny;

   TString _outFilename;
   TString _outDirectory;
   TString _outFileCore;

   Double_t _frontLens_thickness;
   Double_t _frontLens_diameter;
   Double_t _airgap;
   Double_t _slab_width;
   Double_t _slab_height;
   Double_t _fishtank_width;
   Double_t _fishtank_height;

   Int_t _refl_limit;

   Bool_t _b_frontLens;
   Bool_t _b_airgap;

   Bool_t _b_effi;

   Bool_t _b_lost_refl;
   Bool_t _b_lost_bar;
   Bool_t _b_lost_gap;
   Bool_t _b_lost_lens;
   Bool_t _b_lost_airBox;
   Bool_t _b_lost_tank;

   Bool_t _b_abs_limit;
   Bool_t _b_abs_bar;
   Bool_t _b_abs_lens;
   Bool_t _b_abs_tank;
   Bool_t _b_abs_sides;

   Bool_t _b_back;
   Bool_t _b_back_lens;
   Bool_t _b_back_tank;

   Double_t _theta;


   enum magicNumber
   {
     min_wave = 200,  // _effi index + min_wave = wavelength
     unknown  = -666, // for initializing
   };

   ClassDef(EffiContiAll,0);
};

#endif

#ifdef EffiContiAll_cxx
void EffiContiAll::Init(TTree *tree)
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
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("wavelength", &wavelength, &b_wavelength);
   fChain->SetBranchAddress("color", &color, &b_color);
   fChain->SetBranchAddress("kBarX", &kBarX, &b_kBarX);
   fChain->SetBranchAddress("kBarY", &kBarY, &b_kBarY);
   fChain->SetBranchAddress("kBarZ", &kBarZ, &b_kBarZ);
   fChain->SetBranchAddress("hitPosX", &hitPosX, &b_hitPosX);
   fChain->SetBranchAddress("hitPosY", &hitPosY, &b_hitPosY);
   fChain->SetBranchAddress("hitPosZ", &hitPosZ, &b_hitPosZ);
   fChain->SetBranchAddress("hitPosDetX", &hitPosDetX, &b_hitPosDetX);
   fChain->SetBranchAddress("hitPosDetY", &hitPosDetY, &b_hitPosDetY);
   fChain->SetBranchAddress("hitPosDetZ", &hitPosDetZ, &b_hitPosDetZ);
   fChain->SetBranchAddress("hitDirX", &hitDirX, &b_hitDirX);
   fChain->SetBranchAddress("hitDirY", &hitDirY, &b_hitDirY);
   fChain->SetBranchAddress("hitDirZ", &hitDirZ, &b_hitDirY);
   fChain->SetBranchAddress("posX[1100]", posX, &b_posX);
   fChain->SetBranchAddress("posY[1100]", posY, &b_posY);
   fChain->SetBranchAddress("posZ[1100]", posZ, &b_posZ);
   fChain->SetBranchAddress("index_pos", &index_pos, &b_index_pos);
   fChain->SetBranchAddress("particleID", &particleID, &b_particleID);
   fChain->SetBranchAddress("thetaC", &thetaC, &b_thetaC);
   fChain->SetBranchAddress("phiC", &phiC, &b_phiC);
   fChain->SetBranchAddress("measured", &measured, &b_measured);
   fChain->SetBranchAddress("absorbed", &absorbed, &b_absorbed);
   fChain->SetBranchAddress("lost", &lost, &b_lost);
   fChain->SetBranchAddress("time", &time, &b_time);
   fChain->SetBranchAddress("nRefl", &nRefl, &b_nRefl);
}

Bool_t EffiContiAll::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.


//==========
// my stuff
//==========
    // second call of Notify() then first file is opened
  if(_notify)
  {
    TString filename = fChain->GetCurrentFile()->GetName();
    cout << filename << endl;

    TFile *curFile = new TFile( filename );
    TTree *infoTree = (TTree*) curFile->Get("info");

    infoTree->SetBranchAddress( "incidence_theta", &_theta );
    infoTree->GetEntry( 0 );

    curFile->Close();
  }
  else
    _notify = true;


   return kTRUE;
}

#endif // #ifdef EffiContiAll_cxx
