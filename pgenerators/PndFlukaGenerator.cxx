// -------------------------------------------------------------------------
// -----                PndFlukaGenerator source file                  -----
// -------------------------------------------------------------------------


#include <iostream>
#include "TClonesArray.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "TVector3.h"
#include "TParticle.h"
#include "PndFlukaGenerator.h"
#include "FairPrimaryGenerator.h"

#include "TF1.h"
#include "TRandom.h"


#define MAX 200


using std::cout;
using std::endl;
//using std::max;


// -----   Default constructor   ------------------------------------------
PndFlukaGenerator::PndFlukaGenerator() {
  iEvent     = 0;
  iEntry     = 0;
  fInputRootFile = NULL;
  fInputTree = NULL;
}
// ------------------------------------------------------------------------

// -----   Standard constructor   -----------------------------------------
PndFlukaGenerator::PndFlukaGenerator(const Char_t* fileName) {
  
  fFileName  = TString(fileName);
  Init();
  
}

// -----   Destructor   ---------------------------------------------------
PndFlukaGenerator::~PndFlukaGenerator() {
  CloseInput();

}
// ------------------------------------------------------------------------

Bool_t PndFlukaGenerator::Init()
{
  iEvent     = 0;
  iEntry     = 0;  

  cout << "-I PndFlukaGenerator: Opening input file " << fFileName.Data() << endl;
  
  // open ROOT input file
  
  fInputRootFile = new TFile(fFileName,"READ");
  if (fInputRootFile->IsZombie())
    Fatal("PndFlukaGenerator","Cannot open ROOT input file.");
  
  fInputTree = (TTree*) fInputRootFile->Get("ResultsTree");
  if (!SetBranchAddresses()) 
    Fatal("PndFlukaGenerator","Incompatible ROOT input file!");
  return kTRUE;  
}

// ------------------------------------------------------------------------

Bool_t PndFlukaGenerator::SetBranchAddresses() 
{
    if (0==fInputTree) return false;
    
    fInputTree->SetMakeClass(1);

    fInputTree->SetBranchStatus("*",0);
    
    fInputTree->SetBranchStatus("Npart",1);
    //    fInputTree->SetBranchStatus("Weight",1);
    fInputTree->SetBranchStatus("pid",1);
    //    fInputTree->SetBranchStatus("NofInt",1);
    //    fInputTree->SetBranchStatus("etot",1);
    //    fInputTree->SetBranchStatus("ptot",1);
    fInputTree->SetBranchStatus("px",1);
    fInputTree->SetBranchStatus("py",1);
    fInputTree->SetBranchStatus("pz",1);
    //    fInputTree->SetBranchStatus("time",1);
    fInputTree->SetBranchStatus("xx",1);
    fInputTree->SetBranchStatus("yy",1);
    fInputTree->SetBranchStatus("zz",1);

    
    fInputTree->SetBranchAddress("Npart",&fnpart);
    fInputTree->SetBranchAddress("pid",&fpid);
    fInputTree->SetBranchAddress("px",&fpx);
    fInputTree->SetBranchAddress("py",&fpy);
    fInputTree->SetBranchAddress("pz",&fpz);
    fInputTree->SetBranchAddress("xx",&fvx);
    fInputTree->SetBranchAddress("yy",&fvy);
    fInputTree->SetBranchAddress("zz",&fvz);

   
    return true;
}

// -----   Public method ReadEvent   --------------------------------------
Bool_t PndFlukaGenerator::ReadEvent(FairPrimaryGenerator* primGen) 
{
  if ( ! fInputRootFile ) {
    cout << "-E PndFlukaGenerator: Input ROOT file not open!" << endl;
    return kFALSE;
  }
  
  return ReadRootEvent(primGen);
  

}
  
  
// ------------------------------------------------------------------------

Bool_t PndFlukaGenerator::ReadRootEvent(FairPrimaryGenerator* primGen)
{
  // Check for number of events in input file

  Int_t nentries = fInputTree->GetEntries();

  if ( iEntry > fInputTree->GetEntries() ) {
    cout << "-E PndFlukaGenerator: No more events in input file!" << endl;
    CloseInput();
    return kFALSE;
  }
  
  // preserve orginal TDirectory
  TFile  *g=gFile;
  fInputRootFile->cd();
  g->cd();
  
  Int_t evmul=0;

  do 
    { 
      fInputTree->GetEntry(iEntry);

      evmul++;
      
      if ( fnpart < evmul ){
	break;
      }

//      cout << "Pid= " << fpid << endl;
//      cout << "momentum " << fpx << " " << fpy << " " << fpz <<endl;
//      cout << "vertex   " << fvx << " " << fvy << " " << fvz <<endl;

      primGen->AddTrack(fpid,fpx,fpy,fpz,fvx,fvy,fvz);

      iEntry++;

    } while (evmul <= fnpart);

  return kTRUE;
  
}
							       
							       
// -----   Private method CloseInput   ------------------------------------
void PndFlukaGenerator::CloseInput() {
  if (fInputRootFile) 
    {
      cout << "-I PndFlukaGenerator: Closing ROOT input file " << fFileName.Data() << endl;
      fInputRootFile->Close();
      delete fInputRootFile;
      fInputRootFile = NULL;
    }
}

// ------------------------------------------------------------------------

ClassImp(PndFlukaGenerator)
