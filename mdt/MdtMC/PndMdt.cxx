
#include <iostream>

#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TParticle.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoArb8.h"
#include "TGeoTrd2.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include "TVirtualMC.h"

#include "CbmVolume.h"
// add on for debug
#include "CbmGeoG3Builder.h"
#include "CbmRuntimeDb.h"
#include "CbmRun.h"
#include "CbmModule.h"

#include "PndMdt.h"


using namespace std;


// -----   Default constructor   -------------------------------------------
PndMdt::PndMdt() 
{
    fMdtCollection        = new TClonesArray("PndMdtPoint");
    fPosIndex   = 0;
    ResetParameters();
    displacement = 148.;
    SetVerbosity(kFALSE);
    fVolumeName = "kk";
}
// -------------------------------------------------------------------------


// -----   Inherited constructor   -----------------------------------------
PndMdt::PndMdt(const char* name, Bool_t active) : CbmDetector(name,active)
{
    fMdtCollection        = new TClonesArray("PndMdtPoint");
    fPosIndex   = 0;
    ResetParameters();
    displacement = 148.;
    SetVerbosity(kFALSE);
    fVolumeName = "kk";
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMdt::~PndMdt() 
{
    if (fMdtCollection) {
	fMdtCollection->Delete();
	delete fMdtCollection;
    };
}
// -------------------------------------------------------------------------


// -----   Public method Print   ----------------------------------------------
void PndMdt::Print() const 
{
    Int_t nHits = fMdtCollection->GetEntriesFast();
    for (Int_t i=0; i<nHits; i++) (*fMdtCollection)[i]->Print();
}
// ----------------------------------------------------------------------------


// -----   Public method Reset   ----------------------------------------------
void PndMdt::Reset() 
{
   fMdtCollection->Delete(); 
   fPosIndex = 0;
}
// ----------------------------------------------------------------------------


// -----   Public method CopyClones   -----------------------------------------
void PndMdt::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset ) 
{
/*  Int_t nEntries = cl1->GetEntriesFast();
  TClonesArray& clref = *cl2;
  PndMdtPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++) {
    oldpoint = (PndMdtPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) PndMdtPoint(*oldpoint);
    fPosIndex++;
  }
  cout << " -I- PndMdt: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
*/
}
// ----------------------------------------------------------------------------

// -----   Public method ResetParameters   ------------------------------------
void PndMdt::ResetParameters() 
{
  /*  fEventID = -999;
    fTrackID = -999;
    fTrackParentID = -999;
    fDetectorID = -999;
    fPDG = -999;
    */
    fPos.SetXYZT(0., 0., 0., 0.);
    fMom.SetXYZT(0., 0., 0., 0.) ;
}
// ----------------------------------------------------------------------------


// -----   Public method SetParFile   --------------------------------------
void PndMdt::SetParFile(TString filename)
{
    ffn = filename;
    SetBE();
}
// -------------------------------------------------------------------------

// -----   Private method SetBE   ------------------------------------------
void PndMdt::SetBE()
{
//envelope (trd)
    Double_t edx1;
    Double_t edx2;
    Double_t edy1;
    Double_t edy2;
    Double_t edz;
    Double_t ex0;
    Double_t ey0;
    Double_t ez0;

    TFile* muof = new TFile(ffn,"READONLY");
    TTree* muoe = (TTree*)muof->Get("env");
    (muoe->GetBranch("edx1"))->SetAddress(&edx1);
    (muoe->GetBranch("edx2"))->SetAddress(&edx2);
    (muoe->GetBranch("edy1"))->SetAddress(&edy1);
    (muoe->GetBranch("edy2"))->SetAddress(&edy2);
    (muoe->GetBranch("edz"))->SetAddress(&edz);
    (muoe->GetBranch("ex0"))->SetAddress(&ex0);
    (muoe->GetBranch("ey0"))->SetAddress(&ey0);
    (muoe->GetBranch("ez0"))->SetAddress(&ez0);

    (muoe->GetBranch("edx1"))->GetEntry(0);
    (muoe->GetBranch("edx2"))->GetEntry(0);
    (muoe->GetBranch("edy1"))->GetEntry(0);
    (muoe->GetBranch("edy2"))->GetEntry(0);
    (muoe->GetBranch("edz"))->GetEntry(0);
    (muoe->GetBranch("ex0"))->GetEntry(0);
    (muoe->GetBranch("ey0"))->GetEntry(0);
    (muoe->GetBranch("ez0"))->GetEntry(0);
    
    be.dx1 = edx1/10.0;
    be.dx2 = edx2/10.0;
    be.dy1 = edy1/10.0;
    be.dy2 = edy2/10.0;
    be.dz  = edz/10.0;
    be.x0  = ex0/10.0;
    be.y0  = ey0/10.0;
    be.z0  = ez0/10.0;
    
}
// -------------------------------------------------------------------------

void PndMdt::SetMdtVersion(TString location)
{
    version = location;
    cout<<endl<<endl<<"MDT version used: "<<location<<endl<<endl;

    return;
}

// -----   Public method ConstructGeometry   ----------------------------------
void PndMdt::ConstructGeometry() 
{
    if(version=="torino" || version=="Torino") ConstructGeometryTo();
    else if(version=="dubna" || version=="Dubna") ConstructGeometryDu();
    else {cout<<"Error in PndMdt::ConstructGeometry: Specify the version and run again!"<<endl; exit(0);};
    
    return;
}
// ----------------------------------------------------------------------------


// -----   Public method Intialize   ---------------------------------------
void PndMdt::Initialize() 
{
  CbmDetector::Initialize();
  CbmRun* sim = CbmRun::Instance();
  CbmRuntimeDb* rtdb=sim->GetRuntimeDb();
  par=(PndGeoMdtPar*)(rtdb->getContainer("PndGeoMdtPar"));
  
  TObjArray *fSensNodes = par->GetSensitiveNodes();
}
// -------------------------------------------------------------------------


// -----   Public method BeginEvent   --------------------------------------
void PndMdt::BeginEvent()
{
}
// -------------------------------------------------------------------------


// -----   Public method ProcessHits  --------------------------------------
Bool_t PndMdt::ProcessHits(CbmVolume* vol) 
{
    Bool_t ph = kFALSE;

    if(version=="torino" || version=="Torino") ph = ProcessHitsTo(vol);
    else if(version=="dubna" || version=="Dubna") ph = ProcessHitsDu(vol);
    else {cout<<"Error in PndMdt::ConstructGeometry: Specify the version and run again!"<<endl; exit(0);};
  
  ResetParameters();
  
  return ph;
  
}

// ----------------------------------------------------------------------------

TClonesArray* PndMdt::GetCollection(Int_t iColl) const 
{ 

  if(iColl==0) {
	   cout << "PndMdt::GetCollection" << endl;
           return fMdtCollection; 
  }else{ 
     return NULL; 
  }

}


// -----   Public method EndOfEvent   -----------------------------------------
void PndMdt::EndOfEvent() 
{
  if (fVerboseLevel)  Print();
  Reset();
}
// ----------------------------------------------------------------------------


ClassImp(PndMdt)
