//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Feb 16 12:51:33 2010 by ROOT version 5.22/00
// from TTree photon/kBarList_100000_center.root
// found on file: kBarList_100000_center.root
//////////////////////////////////////////////////////////

#ifndef KBarAnalysis_h
#define KBarAnalysis_h


//==========
// STD C/C++
//==========
#include <iostream>
#include <vector>

using namespace std;


//======
// ROOT
//======
#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "TSelector.h"
#include "TString.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TLine.h"
#include "TPaveText.h"


//==========
// my stuff
//==========
#include "DetEffi.h"


// const Int_t kMaxPosSize = 105; // 5 reflections (limit) + 100 backup (see prototype.cc)
const Int_t size_reflType = 6; // all, direct, left, right, up, down

class KBarAnalysis : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // Declaration of leaf types
   Double_t        wavelength;
   Double_t        kBarX;
   Double_t        kBarY;
   Double_t        kBarZ;
   Double_t        hitPosDetX;
   Double_t        hitPosDetY;
   Double_t        hitPosDetZ;
   Double_t        hitPosZ;
   Bool_t          measured;
   Double_t        posX[105];
   Double_t        posY[105];
   Double_t        posZ[105];
   Int_t           index_pos;

   // List of branches
   TBranch        *b_wavelength;   //!
   TBranch        *b_kBarX;   //!
   TBranch        *b_kBarY;   //!
   //TBranch        *b_time;   //!
   TBranch        *b_kBarZ;   //!
   TBranch        *b_hitPosDetX;   //!
   TBranch        *b_hitPosDetY;   //!
   TBranch        *b_hitPosDetZ;   //!
   TBranch        *b_hitPosZ;   //!
   TBranch        *b_measured;   //!
   TBranch        *b_posX;   //!
   TBranch        *b_posY;   //!
   TBranch        *b_posZ;   //!
   TBranch        *b_index_pos;   //!


//    KBarAnalysis(TTree * /*tree*/ =0);
   KBarAnalysis( Int_t n_mcp = 1 );
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

   // ROOT bug: it seems that data member changes by Notify() is saved somewhere else than
   // the data member; internal checks during opening a new file can delete the memory where
   // Notify() have saved the changes; files with complex structure are affected "
   static Bool_t _notify;


   Bool_t _mcpMode;
   Bool_t _effiMode;

   DetEffi *_effi;

   Int_t _n_mcp;

   Int_t _pixeltot;

   TString _gap_str;

   vector<TString> _mcpLabel_str;

   vector<Double_t> _mcp_dim;
   vector<Double_t> _mcp_act;
   vector<Double_t> _mcp_det;

   vector<Double_t> _minX_dim;
   vector<Double_t> _minY_dim;
   vector<Double_t> _maxX_dim;
   vector<Double_t> _maxY_dim;

   vector<Double_t> _minX_det;
   vector<Double_t> _minY_det;
   vector<Double_t> _maxX_det;
   vector<Double_t> _maxY_det;

   vector<Double_t> _shiftX;
   vector<Double_t> _shiftY;

   vector<Int_t> _x_bins;
   vector<Int_t> _y_bins;

   Double_t _px_kBarX[size_reflType];
   Double_t _px_kBarY[size_reflType];
   Double_t _px_kBarZ[size_reflType];

   Double_t _px_kBarXerr[size_reflType];
   Double_t _px_kBarYerr[size_reflType];
   Double_t _px_kBarZerr[size_reflType];

   Int_t _px_freq[size_reflType];

   Int_t _px_mcp;
   Int_t _px_col;
   Int_t _px_row;

   Double_t _fishtank_width;
   Double_t _fishtank_height;
   Double_t _airgap;

   Double_t _resolution;

   TString _outDirectory;
   TString _outFileCore;

   vector< vector< vector< vector<TH1F*> > > > _pixelX;
   vector< vector< vector< vector<TH1F*> > > > _pixelY;
   vector< vector< vector< vector<TH1F*> > > > _pixelZ;

   vector< vector< vector< vector<TString*> > > > _pixelX_str;
   vector< vector< vector< vector<TString*> > > > _pixelY_str;
   vector< vector< vector< vector<TString*> > > > _pixelZ_str;

   vector< vector< vector< vector<Double_t> > > > _kBarXsum;
   vector< vector< vector< vector<Double_t> > > > _kBarYsum;
   vector< vector< vector< vector<Double_t> > > > _kBarZsum;

   vector< vector< vector< vector<Int_t> > > > _freq;

   TH2F *_screen;

   Int_t _n_lines;

   vector< vector<TLine*> > _line_mcpCase;
   vector< vector<TLine*> > _line_mcpArea;

   vector<TPaveText*> _mcpLabel; // need it as pointer due to AddText()

   vector< vector<TH2F*> > _kBarX;
   vector< vector<TH2F*> > _kBarY;
   vector< vector<TH2F*> > _kBarZ;

   vector< vector<TString*> > _kBarX_str;
   vector< vector<TString*> > _kBarY_str;
   vector< vector<TString*> > _kBarZ_str;

   vector< vector<TH2F*> > _kBarX_screen;
   vector< vector<TH2F*> > _kBarY_screen;
   vector< vector<TH2F*> > _kBarZ_screen;

   vector< vector<TString*> > _kBarX_screen_str;
   vector< vector<TString*> > _kBarY_screen_str;
   vector< vector<TString*> > _kBarZ_screen_str;


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

   TString _type_str[size_reflType];


   enum magicNumber
   {
     unknown  = -666, // for initializing
     nokbar   = 666,  // for the combination nokbar/nofreq = unknown
     nofreq   = -1,
     n_lines  = 4     // lines to draw a square
   };


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
//    fChain->SetBranchAddress("time", &kBarY, &b_time); // kBarY is now time
   fChain->SetBranchAddress("kBarY", &kBarY, &b_kBarY);
   fChain->SetBranchAddress("kBarZ", &kBarZ, &b_kBarZ);
   fChain->SetBranchAddress("hitPosDetX", &hitPosDetX, &b_hitPosDetX);
   fChain->SetBranchAddress("hitPosDetY", &hitPosDetY, &b_hitPosDetY);
   fChain->SetBranchAddress("hitPosZ", &hitPosZ, &b_hitPosZ); // for tagging reflections
   fChain->SetBranchAddress("measured", &measured, &b_measured);
   fChain->SetBranchAddress("posX[105]", posX, &b_posX);
   fChain->SetBranchAddress("posY[105]", posY, &b_posY);
   fChain->SetBranchAddress("posZ[105]", posZ, &b_posZ);
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
