
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
   fMdtCollection->Clear(); 
   fPosIndex = 0;
}
// ----------------------------------------------------------------------------


// -----   Public method CopyClones   -----------------------------------------
void PndMdt::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset ) 
{
  Int_t nEntries = cl1->GetEntriesFast();
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
}
// ----------------------------------------------------------------------------


// -----   Public method AddHit   ---------------------------------------------
PndMdtPoint* PndMdt::AddHit() 
{
  TClonesArray& clref = *fMdtCollection;
  Int_t size = clref.GetEntriesFast();
  
  return new(clref[size]) PndMdtPoint(fEventID,fTrackID,fTrackParentID,fDetectorID,fPDG,fPos,fMom);
}
// ----------------------------------------------------------------------------


// -----   Public method ResetParameters   ------------------------------------
void PndMdt::ResetParameters() 
{
    fEventID = -999;
    fTrackID = -999;
    fTrackParentID = -999;
    fDetectorID = -999;
    fPDG = -999;
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


// -----   Public method ConstructGeometry   ----------------------------------
void PndMdt::ConstructGeometry() 
{
    TString vname = "cave";

    SetParFile(GetGeometryFileName());
    vname = vname.Strip();
    TGeoVolume* vcave = gGeoManager->FindVolumeFast(vname.Data());

//ArCO2
    Int_t kMatArCO2 = 40;
    Float_t aP[3] = {39.948, 12.01, 16.00};
    Float_t zP[3] = {18., 6., 8.} ;
    Float_t wP[3] = {1., 1., 2.} ;
    Float_t dP = 1.636 ;
    Int_t   nP = 3;
    Float_t sumWeight = 0.0;
    for (Int_t i=0; i<nP; i++) sumWeight += aP[i]*wP[i];
    for (Int_t i=0; i<nP; i++) wP[i] *= aP[i]/sumWeight;
    TGeoMaterial* matArCO2 = gGeoManager->Mixture("ArCO2",aP,zP,dP,nP,wP,kMatArCO2);
    
    Int_t kMedArCO2 = 40;
    TGeoMedium* medArCO2 = gGeoManager->Medium("ArCO2", kMedArCO2, kMatArCO2, 1, 1, 30., 10.0, 0.1, 0.1, 0.1, 0.1);

// iron    
    Int_t kMatIron = 26;
    Float_t aP1[1] = {55.845};
    Float_t zP1[1] = {26.0} ;
    Float_t wP1[3] = {1.0} ;
    Float_t dP1 = 7.86;
    Int_t   nP1 = 1;
    TGeoMaterial* matIron = gGeoManager->Mixture("Iron",aP1,zP1,dP1,nP1,wP1,kMatIron);
    
    Int_t kMedIron = 26;
    TGeoMedium* medIron = gGeoManager->Medium("Iron", kMedIron, kMatIron, 1, 1, 30., 10.0, 0.1, 0.1, 0.1, 0.1);

// muon barrel
    TGeoVolume* sbVa = new TGeoVolumeAssembly("va_sb");
    TGeoVolume* mdtBarrel = new TGeoVolumeAssembly("MdtBarrel");

//muon layer (box)   
    Double_t mpx;
    Double_t mpy;
    Double_t mpz;
    Double_t mx0;
    Double_t my0;
    Double_t mz0;

    Double_t angle;
    Double_t lx,ly;
    Double_t lz = displacement;

    Text_t buffer[30];
    TFile* muof = new TFile(ffn,"READONLY");
    TTree* muom = (TTree*)muof->Get("box");
    (muom->GetBranch("mpx"))->SetAddress(&mpx);
    (muom->GetBranch("mpy"))->SetAddress(&mpy);
    (muom->GetBranch("mpz"))->SetAddress(&mpz);
    (muom->GetBranch("mx0"))->SetAddress(&mx0);
    (muom->GetBranch("my0"))->SetAddress(&my0);
    (muom->GetBranch("mz0"))->SetAddress(&mz0);

    Int_t nEntries = (Int_t)muom->GetEntries();
    for(Int_t i=0; i<nEntries; i++)
    {
	(muom->GetBranch("mpx"))->GetEntry(i);
	(muom->GetBranch("mpy"))->GetEntry(i);
	(muom->GetBranch("mpz"))->GetEntry(i);
	(muom->GetBranch("mx0"))->GetEntry(i);
	(muom->GetBranch("my0"))->GetEntry(i);
	(muom->GetBranch("mz0"))->GetEntry(i);
	sprintf(buffer,"muon%i",(int)(i+1));
	TGeoBBox* box = new TGeoBBox(mpx/10.0,mpy/10.0,mpz/10.0,0);
	TGeoVolume* volume = new TGeoVolume(buffer,box,gGeoManager->GetMedium("ArCO2"));
	AddSensitiveVolume(volume);
	volume->SetLineColor(1);
	sbVa->AddNode(volume,i,new TGeoCombiTrans(mx0/10.0,my0/10.0,mz0/10.0,new TGeoRotation()));
    };
    
    for(Int_t i=0; i<8; i++)
    {
	angle = ((Double_t)i)*TMath::ACos(-1.0)/4.0;
	lx = TMath::Cos(angle)*be.z0;
	ly = TMath::Sin(angle)*be.z0;
	angle *= 180.0/TMath::ACos(-1.0);
	TGeoRotation tRot;
	tRot.RotateY(90.0);
	tRot.RotateZ(angle);
	mdtBarrel->AddNode(sbVa,i,new TGeoCombiTrans(lx,ly,lz,new TGeoRotation(tRot)));
    };
        

// muon endcap        
    TGeoVolume* seVa = new TGeoVolumeAssembly("va_se");
    TGeoVolume* mdtEndcap = new TGeoVolumeAssembly("MdtEndcap");

    Double_t dx1, dx2, dy1, dy2, dz;

    dx1 = 1.0;
    dx2 = 1.0;
    dy1 = 0.0;
    dy2 = be.dy1;
    dz  = (be.z0-be.dz)/2.0;
    lz  = displacement + be.dx1 - 1.0;
    sprintf(buffer,"muon51");
    TGeoTrd2* trd = new TGeoTrd2(dx1,dx2,dy1,dy2,dz);
    TGeoVolume* volume = new TGeoVolume(buffer,trd,gGeoManager->GetMedium("ArCO2"));
    AddSensitiveVolume(volume);
    for(Int_t i=0; i<8; i++)
    {
	angle = ((Double_t)i)*TMath::ACos(-1.0)/4.0;
	lx = TMath::Cos(angle)*(be.z0-be.dz)/2.0;
	ly = TMath::Sin(angle)*(be.z0-be.dz)/2.0;
	angle *= 180.0/TMath::ACos(-1.0);
	TGeoRotation tRot;
	tRot.RotateY(90.0);
	tRot.RotateZ(angle);
	mdtEndcap->AddNode(volume,i+1,new TGeoCombiTrans(lx,ly,lz,new TGeoRotation(tRot)));
    };


    lx = 0.0;
    ly = 0.0;
    lz = 0.0;
    for(Int_t i=0; i<6; i++)
    {
	dx1 = 1.0;
	dx2 = 1.0;
	sprintf(buffer,"muon%i",(int)(i+52));
	dy1 = 0.0;
	dy2 = be.dy2;
	dz  = (be.z0+be.dz)/2.0;
	TGeoTrd2* trd = new TGeoTrd2(dx1,dx2,dy1,dy2,dz);
	if(i==0) lx -= 6.0;
	else lx -= 8.0;
	TGeoVolume* volume = new TGeoVolume(buffer,trd,gGeoManager->GetMedium("ArCO2"));
	AddSensitiveVolume(volume);
	seVa->AddNode(volume,i+52,new TGeoCombiTrans(lx,ly,lz,new TGeoRotation()));
    };

    lz  = displacement + be.dx1 - 1.0;
    for(Int_t i=0; i<8; i++)
    {
	angle = ((Double_t)i)*TMath::ACos(-1.0)/4.0;
	lx = TMath::Cos(angle)*(be.z0+be.dz)/2.0;
	ly = TMath::Sin(angle)*(be.z0+be.dz)/2.0;
	angle *= 180.0/TMath::ACos(-1.0);
	TGeoRotation tRot;
	tRot.RotateY(90.0);
	tRot.RotateZ(angle);
	mdtEndcap->AddNode(seVa,i+52,new TGeoCombiTrans(lx,ly,lz,new TGeoRotation(tRot)));
    };


    vcave->AddNode(mdtBarrel,1);
    vcave->AddNode(mdtEndcap,1);
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
  TString name = gMC->CurrentVolName();
  if (gMC->IsTrackEntering()==kFALSE) return kTRUE;
  
  TLorentzVector lPos, lMom;

  if (!(name.BeginsWith("muon"))) 
    cout << "Error <PndMdt::ProcessHits> : " << name << " not MDT volume" << endl;
  else {
    lEventID = gMC->CurrentEvent();
    lTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
    SetTrackParentID(gMC->GetStack()->GetCurrentParentTrackNumber());
    
    int ilayer;
    sscanf(name,"muon%i",&ilayer);
    SetDetectorID((Int_t)ilayer);
    
    SetPDG(gMC->TrackPid());
    
    gMC->TrackPosition(lPos); // cm
    gMC->TrackMomentum(lMom); // GeV
    
    SetPos(lPos);
    SetMom(lMom);
    
    if(name!=fVolumeName || lEventID != GetEventID() || lTrackID != GetTrackID()) 
    {
	fVolumeName = name;
	SetEventID(lEventID);
	SetTrackID(lTrackID);
	AddHit();
	if ( ( (GetModule()==1) && (TMath::Even(GetDetectorID())) ) ||
	     (GetModule()==2)  )
	  { // Set the correct MCTrack->GetMdtPoints()
	    Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
	    Int_t nMdtPoints = (points & (15<<6)) >> 6;
	    nMdtPoints ++;
	    if (nMdtPoints <= 15)
	      {
		points = ( points & ( ~ (15<< 6) ) ) | (nMdtPoints << 6);
		gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);
	      }
	  }
    };
  };
  
  //  ResetParameters();
  
  return kTRUE;
  
}
// ----------------------------------------------------------------------------


// -----   Public method EndOfEvent   -----------------------------------------
void PndMdt::EndOfEvent() 
{
  if (fVerboseLevel)  Print();
  Reset();
}
// ----------------------------------------------------------------------------


ClassImp(PndMdt)
