//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Feb 16 12:51:33 2010 by ROOT version 5.22/00
// from TTree photon/kBarList_100000_center.root
// found on file: kBarList_100000_center.root
//////////////////////////////////////////////////////////

#ifndef KBarAnalysis_hh
#define KBarAnalysis_hh

#include <iostream>
#include <vector>
using namespace std;

#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "TSelector.h"
#include "TString.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TLine.h"
#include "TPaveText.h"
#include "TRandom3.h"

// const Int_t kMaxPosSize = 14; // 1 start + 3 volume transition (+ tiny shifts) + detector + 1 exceed + 5 reflections (limit)

class KBarAnalysis : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // Declaration of leaf types
   Double_t        wavelength;
   Double_t        kBarX;
   Double_t        kBarY;
   Double_t        kBarZ;
   Double_t        hitPosX;
   Double_t        hitPosY;
   Double_t        hitPosZ;
   Double_t        hitDirX;
   Double_t        hitDirY;
   Double_t        hitDirZ;
   Bool_t          measured;
   Double_t        posX[14];
   Double_t        posY[14];
   Double_t        posZ[14];
   Int_t           index_pos;

   // List of branches
   TBranch        *b_wavelength;   //!
   TBranch        *b_kBarX;   //!
   TBranch        *b_kBarY;   //!
   TBranch        *b_kBarZ;   //!
   TBranch        *b_hitPosX;   //!
   TBranch        *b_hitPosY;   //!
   TBranch        *b_hitPosZ;   //!
   TBranch        *b_hitDirX;   //!
   TBranch        *b_hitDirY;   //!
   TBranch        *b_hitDirZ;   //!
   TBranch        *b_measured;   //!
   TBranch        *b_posX;   //!
   TBranch        *b_posY;   //!
   TBranch        *b_posZ;   //!
   TBranch        *b_index_pos;   //!


   KBarAnalysis(TTree * /*tree*/ =0);
   virtual ~KBarAnalysis() { }
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
   Bool_t _mcpMode;
   Bool_t _effiMode;

   Double_t _effi[70];

   TRandom3 _rand;

   TString _gap_str;

   Int_t _n_mcp;

   Double_t _mcp_dim;
   Double_t _mcp_active;

   Double_t _minX_dim[4];
   Double_t _minY_dim[4];
   Double_t _maxX_dim[4];
   Double_t _maxY_dim[4];

   Double_t _minX_active[4];
   Double_t _minY_active[4];
   Double_t _maxX_active[4];
   Double_t _maxY_active[4];

   Double_t _px_kBarX[6];
   Double_t _px_kBarY[6];
   Double_t _px_kBarZ[6];

   Double_t _px_kBarXerr[6];
   Double_t _px_kBarYerr[6];
   Double_t _px_kBarZerr[6];

   Int_t _px_freq[6];

   Double_t _fishtank_width;
   Double_t _fishtank_height;
   Double_t _airgap;

   Double_t _resolution;

   TString _outDirectory;
   TString _outFileCore;

   Int_t _x_bins;
   Int_t _y_bins;

   vector< vector< vector<TH1F> > > _pixelX;
   vector< vector< vector<TH1F> > > _pixelY;
   vector< vector< vector<TH1F> > > _pixelZ;

   vector< vector< vector<TString> > > _pixelX_str;
   vector< vector< vector<TString> > > _pixelY_str;
   vector< vector< vector<TString> > > _pixelZ_str;

   vector< vector< vector<Double_t> > > _kBarXsum;
   vector< vector< vector<Double_t> > > _kBarYsum;
   vector< vector< vector<Double_t> > > _kBarZsum;

   vector< vector< vector<Int_t> > > _freq;

   TH2F *_screen;

   TLine _line_mcpCase[4][4]; // MCP position case
   TLine _line_mcpArea[4][4]; // MCP position avtive area

   TPaveText *_mcpLabel[4]; // need it as pointer due to AddText()

   vector< vector<TH2F> > _kBarX;
   vector< vector<TH2F> > _kBarY;
   vector< vector<TH2F> > _kBarZ;

   vector< vector<TString> > _kBarX_str;
   vector< vector<TString> > _kBarY_str;
   vector< vector<TString> > _kBarZ_str;

   vector< vector<TH2F> > _kBarX_screen;
   vector< vector<TH2F> > _kBarY_screen;
   vector< vector<TH2F> > _kBarZ_screen;

   vector< vector<TString> > _kBarX_screen_str;
   vector< vector<TString> > _kBarY_screen_str;
   vector< vector<TString> > _kBarZ_screen_str;


   enum reflType
   {
       all    = 0,
       direct = 1,
       left   = 2,
       right  = 3,
       up     = 4,
       down   = 5,
       size   = 6
   };

   TString type_str[6];


   ClassDef(KBarAnalysis,0);
};

#endif

#ifdef KBarAnalysis_cxx
void KBarAnalysis::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree ) return;
   fChain = tree;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("wavelength", &wavelength, &b_wavelength);
   fChain->SetBranchAddress("kBarX", &kBarX, &b_kBarX);
   fChain->SetBranchAddress("kBarY", &kBarY, &b_kBarY);
   fChain->SetBranchAddress("kBarZ", &kBarZ, &b_kBarZ);
   fChain->SetBranchAddress("hitPosX", &hitPosX, &b_hitPosX);
   fChain->SetBranchAddress("hitPosY", &hitPosY, &b_hitPosY);
   fChain->SetBranchAddress("hitPosZ", &hitPosZ, &b_hitPosZ);
   fChain->SetBranchAddress("hitDirX", &hitDirX, &b_hitDirX);
   fChain->SetBranchAddress("hitDirY", &hitDirY, &b_hitDirY);
   fChain->SetBranchAddress("hitDirZ", &hitDirZ, &b_hitDirY);
   fChain->SetBranchAddress("measured", &measured, &b_measured);
   fChain->SetBranchAddress("posX[14]", posX, &b_posX);
   fChain->SetBranchAddress("posY[14]", posY, &b_posY);
   fChain->SetBranchAddress("posZ[14]", posZ, &b_posZ);
   fChain->SetBranchAddress("index_pos", &index_pos, &b_index_pos);
}

Bool_t KBarAnalysis::Notify()
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
        TString file = fChain->GetCurrentFile()->GetName();
        cout << file << endl;
    }
    else
        _notify = true;


   return kTRUE;
}

#endif // #ifdef KBarAnalysis_cxx
