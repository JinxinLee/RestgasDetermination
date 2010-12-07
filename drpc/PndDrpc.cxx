#include "PndDrpc.h"

#include "TClonesArray.h"
#include "TGeoBBox.h"
#include "TGeoSphere.h"
#include "TGeoTube.h"
#include "TGeoManager.h"
#include "TGeoMedium.h"
#include "TGeoMaterial.h"
#include "TVirtualMC.h"
#include "TGeoVolume.h"
#include "TGeoCompositeShape.h"
#include "TGeoMatrix.h"
#include "TLorentzVector.h"

#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairGeoMedia.h"
#include "FairGeoMedium.h"
#include "FairGeoBuilder.h"
#include "FairRootManager.h"

#include "PndDrpcParam.h"
#include "PndDrpcPointLite.h"


#include <iostream>

using namespace std;

/** Default constructor. Don't use! **/
PndDrpc::PndDrpc()
  : FairDetector("TOF", kTRUE)
{
  fPoints=new TClonesArray("PndDrpcPointLite");
  fMCPoints=new TClonesArray("PndDrpcPointLite");
  fPosIndex=0;
  fWallId=-1111;
  fCameraId=-1111;
}

/** Standard constructor. active --- sensitivity flag **/
PndDrpc::PndDrpc(const char* name, Bool_t active)
  : FairDetector(name, active)
{
  fPoints=new TClonesArray("PndDrpcPointLite");
  fMCPoints=new TClonesArray("PndDrpcPointLite");
  fPosIndex=0;
  fWallId=-1111;
  fCameraId=-1111;
}

PndDrpc::~PndDrpc()
{
  if (fPoints)
  {
    fPoints->Delete();
    delete fPoints;
    fPoints=NULL;
  }
  if (fMCPoints)
  {
    fMCPoints->Delete();
    delete fMCPoints;
    fMCPoints=NULL;
  }
}

void PndDrpc::Initialize()
{
  FairDetector::Initialize();
}

void PndDrpc::LoadParam()
{
  PndDrpcParam* p=new PndDrpcParam("TofGeometry",GetGeometryFileName());

  fCameraSizeX=p->GetDouble("CameraSizeX");
  fCameraSizeZ=p->GetDouble("CameraSizeZ");
  fCameraAnodTh=p->GetDouble("CameraAnodTh");
  fCameraCathodTh=p->GetDouble("CameraCathodTh");
  fCameraGasTh=p->GetDouble("CameraGasTh");
  fCameraSpacerSize=p->GetDouble("CameraSpacerSize");
  fCameraLayers=p->GetInteger("CameraLayers");
  fCameraDeadZone=p->GetDouble("CameraDeadZone");
  fCameraTh=(fCameraAnodTh+fCameraCathodTh+fCameraGasTh)*fCameraLayers;

  fHolderTh=p->GetDouble("HolderTh");
  fHolderScrewR=p->GetDouble("HolderScrewR");
  fHolderScrewX=p->GetDouble("HolderScrewX");
  fHolderScrewZ=p->GetDouble("HolderScrewZ");
  fHolderSizeX=p->GetDouble("HolderSizeX");
  fHolderSizeX2=p->GetDouble("HolderSizeX2");
  fHolderLen1=p->GetDouble("HolderLen1");
  fHolderLen2=p->GetDouble("HolderLen2");
  fHolderLen3=p->GetDouble("HolderLen3");

  fPCBTh=p->GetDouble("PCBTh");
  fCameraOverlap=p->GetDouble("CameraOverlap");
  fPCBHoleR=p->GetDouble("PCBHoleR");
  fPCBScrewR=p->GetDouble("PCBScrewR");
  fPCBScrewTh=p->GetDouble("PCBScrewTh");
  fPCBScrewX=p->GetDouble("PCBScrewX");
  fPCBScrewZ=p->GetDouble("PCBScrewZ");

  fInnerGap=p->GetDouble("InnerGap");
  fInnerGap2=p->GetDouble("InnerGap2");
  fNCameras=p->GetInteger("NCameras");

  fCoverTh=p->GetDouble("CoverTh");
  fCoverMat=p->GetString("CoverMat");
  fCoverSizeX=p->GetDouble("CoverSizeX");
  fPCBSizeZ=p->GetDouble("PCBSizeZ");
  fCoverSizeZ=p->GetDouble("CoverSizeZ");
  fCoverScrewD=p->GetDouble("CoverScrewD");
  fCoverHoleR=p->GetDouble("CoverHoleR");
  fCoverScrewR=p->GetDouble("CoverScrewR");
  fCoverScrewTh=p->GetDouble("CoverScrewTh");
  fCoverAddHoleD=p->GetDouble("CoverAddHoleD");
  fCoverHolesX=p->GetInteger("CoverHolesX");
  fCoverHolesZ=p->GetInteger("CoverHolesZ");

  fContainerR1=p->GetDouble("ContainerR1");
  fContainerR2=p->GetDouble("ContainerR2");
  fContainerR3=p->GetDouble("ContainerR3");
  fModuleR1=p->GetDouble("ModuleR1");
  fModuleR2=p->GetDouble("ModuleR2");
  fNModules=p->GetInteger("NModules");

  fBeauty=p->GetInteger("Beauty");
  
  if (fPCBSizeZ<fCoverSizeZ)
  {
    Info("LoadParam", "PCBSizeZ should be more than CoverSizeZ. Geometry will be corrupted.");
  }
//  delete p;
  if (fVerboseLevel>99)
  {
    cout << "CameraSizeX=" << fCameraSizeX << endl; 
    cout << "CameraSizeZ=" << fCameraSizeZ << endl; 
    cout << "CameraAnodTh=" << fCameraAnodTh << endl; 
    cout << "CameraCathodTh=" << fCameraCathodTh << endl; 
    cout << "CameraGasTh=" << fCameraGasTh << endl; 
    cout << "CameraSpacerSize=" << fCameraSpacerSize << endl; 
    cout << "CameraLayers=" << fCameraLayers << endl; 
    cout << "CameraDeadZone=" << fCameraDeadZone << endl; 

    cout << "HolderTh=" << fHolderTh << endl;
    cout << "HolderScrewR=" << fHolderScrewR << endl;
    cout << "HolderScrewX=" << fHolderScrewX << endl;
    cout << "HolderScrewZ=" << fHolderScrewZ << endl;
    cout << "HolderSizeX=" << fHolderSizeX << endl;
    cout << "HolderSizeX2=" << fHolderSizeX2 << endl;
    cout << "HolderLen1=" << fHolderLen1 << endl;
    cout << "HolderLen2=" << fHolderLen2 << endl;
    cout << "HolderLen3=" << fHolderLen3 << endl;

    cout << "PCBTh=" << fPCBTh << endl; 
    cout << "CameraOverlap=" << fCameraOverlap << endl; 
    cout << "PCBHoleR=" << fPCBHoleR << endl; 
    cout << "PCBScrewR=" << fPCBScrewR << endl; 
    cout << "PCBScrewTh=" << fPCBScrewTh << endl; 
    cout << "PCBScrewX=" << fPCBScrewX << endl; 
    cout << "PCBScrewZ=" << fPCBScrewZ << endl; 

    cout << "InnerGap=" << fInnerGap << endl;
    cout << "InnerGap2=" << fInnerGap2 << endl;
    cout << "NCameras=" << fNCameras << endl;

    cout << "CoverTh=" << fCoverTh << endl;
    cout << "CoverMat=" << fCoverMat << endl;
    cout << "CoverSizeX=" << fCoverSizeX << endl;
    cout << "CoverSizeZ=" << fCoverSizeZ << endl;
    cout << "PCBSizeZ=" << fPCBSizeZ << endl;
    cout << "CoverScrewD=" << fCoverScrewD << endl;
    cout << "CoverHoleR=" << fCoverHoleR << endl;
    cout << "CoverScrewR=" << fCoverScrewR << endl;
    cout << "CoverScrewTh=" << fCoverScrewTh << endl;
    cout << "CoverAddHoleD=" << fCoverAddHoleD << endl;
    cout << "CoverHolesX=" << fCoverHolesX << endl;
    cout << "CoverHolesZ=" << fCoverHolesZ << endl;

    cout << "ContainerR1=" << fContainerR1 << endl; 
    cout << "ContainerR2=" << fContainerR2 << endl; 
    cout << "ContainerR3=" << fContainerR3 << endl; 
    cout << "ModuleR1=" << fModuleR1 << endl;
    cout << "ModuleR2=" << fModuleR2 << endl;
    cout << "NModules=" << fNModules << endl;

    cout << "Beauty=" << fBeauty << endl; 
  }
}

Bool_t PndDrpc::ProcessHits(FairVolume* vol)
{
  Int_t cpy;
  Int_t vln=gMC->CurrentVolID(cpy);

  if (vln==fWallId)
  if (gMC->IsTrackEntering()||gMC->IsTrackExiting())
  {
    Int_t ex;
    if (gMC->IsTrackEntering()) ex=-1; else ex=-2;
    TLorentzVector pos; gMC->TrackPosition(pos);
    TLorentzVector mom; gMC->TrackMomentum(mom);
    Double_t len=gMC->TrackLength();
    Int_t pdg=gMC->TrackPid();
    Double_t t=gMC->TrackTime()*1e9;  pos.SetT(t);
    Int_t trackid=gMC->GetStack()->GetCurrentTrackNumber();

    TClonesArray& clref=*fMCPoints;
    Int_t size=clref.GetEntriesFast();
    new (clref[size]) PndDrpcPointLite(trackid, ex, pos, mom, 0.0, len, pdg);

    return kTRUE;
  }
  else
  {
    return kTRUE;
  }
/*
//  Debug information
  Int_t i;
  Int_t dta;
  for(i=0;i<7;i++)
  {
    gMC->CurrentVolOffID(i, dta);
    cout << dta << ": " << gMC->CurrentVolOffName(i) << ", ";
  }
  cout << endl;
*/

  Double_t eDep=gMC->Edep();

  if (eDep<=0) return kTRUE;
/*
// An old version
  Int_t gap; gMC->CurrentVolOffID(0, gap);
  Int_t det; gMC->CurrentVolOffID(2, det);
  Int_t rng; gMC->CurrentVolOffID(4, rng);
  Int_t mdl; gMC->CurrentVolOffID(5, mdl);
  Int_t detId=gap+10*(det+100*(mdl+100*rng));
  TLorentzVector pos; gMC->TrackPosition(pos);
  TLorentzVector mom; gMC->TrackMomentum(mom);
  Double_t len=gMC->TrackLength();
  Int_t pdg=gMC->TrackPid();
  Double_t t=gMC->TrackTime()*1e9;  pos.SetT(t);
  Int_t trackid=gMC->GetStack()->GetCurrentTrackNumber();

  TClonesArray& clref=*fPoints;
  Int_t size=clref.GetEntriesFast();
  new (clref[size]) PndDrpcPointLite(trackid, detId, pos, mom, eDep, len, pdg);
*/

  Int_t gap; gMC->CurrentVolOffID(0, gap);
  Int_t det; gMC->CurrentVolOffID(2, det);
  Int_t rng; gMC->CurrentVolOffID(4, rng);
  Int_t detId=gap+10*(det+100*rng);
  TLorentzVector pos; gMC->TrackPosition(pos);
  TLorentzVector mom; gMC->TrackMomentum(mom);
  Double_t len=gMC->TrackLength();
  Int_t pdg=gMC->TrackPid();
  Double_t t=gMC->TrackTime()*1e9;  pos.SetT(t);
  Int_t trackid=gMC->GetStack()->GetCurrentTrackNumber();

  TClonesArray& clref=*fPoints;
  Int_t size=clref.GetEntriesFast();
  new (clref[size]) PndDrpcPointLite(trackid, detId, pos, mom, eDep, len, pdg);


  return kTRUE;
}

void PndDrpc::EndOfEvent()
{
  Reset();
}

void PndDrpc::BeginEvent()
{
  ;
}

void PndDrpc::Register()
{
  FairRootManager::Instance()->Register("TofPoint","Tof", fPoints, kTRUE);
  FairRootManager::Instance()->Register("TofMCPoint","Tof", fMCPoints, kTRUE);
}

TClonesArray* PndDrpc::GetCollection(Int_t iColl) const
{
  if (iColl==0) return fPoints;
  if (iColl==1) return fMCPoints;
  return NULL;
}

void PndDrpc::Print()
{
  ;
}

void PndDrpc::Reset()
{
  fPoints->Delete();
  fMCPoints->Delete();

  fPosIndex=0;
}

void PndDrpc::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
  Int_t n=cl1->GetEntriesFast();
  TClonesArray& clref=*cl2;
  PndDrpcPointLite* oldpoint=NULL;
  for(Int_t i=0;i<n;i++)
  {
    oldpoint=(PndDrpcPointLite*) cl1->At(i);
    Int_t index=oldpoint->TrackId()+offset;
    oldpoint->SetTrackId(index);
    new (clref[fPosIndex]) PndDrpcPointLite(*oldpoint);
    fPosIndex++;
  }
  Info("CopyClones","%d merged entries.", cl2->GetEntriesFast());
}

void PndDrpc::ConstructGeometry()
{
/** ------------------ Loading media --------------------------------------- **/
  InitMedium("tof_polyethylene");
  InitMedium("tof_polypropylene");
  InitMedium("tof_aluminum");
  InitMedium("tof_tungsten");
  InitMedium("tof_sapphire");
  InitMedium("tof_g10");
  InitMedium("tof_gas");
  InitMedium("tof_gas_sens");
  InitMedium("tof_air");
  InitMedium("tof_air_sens");
/** ------------------ Loading parameters ---------------------------------- **/
  LoadParam();
/** ------------------ Building geometry ----------------------------------- **/
  fCamera=BuildCamera();
  fHolder=BuildHolder();
  fInner=BuildInner();
  fModule=BuildModule();
  fTof=BuildTubes();
}
/*
// An old version
TGeoVolume* PndDrpc::BuildTubes()
{
  TGeoTube* mt;
  TGeoVolume* mtv;
  TGeoTube* mt1;
  TGeoVolume* mt1v;
  TGeoVolume* mt1d;
  TGeoTube* mt2;
  TGeoVolume* mt2v;
  TGeoVolume* mt2d;
  Double_t d=0.001;
  TGeoRotation* tr2;
  TGeoRotation* tr1;
  Double_t phi;

  TGeoVolume* cave=gGeoManager->GetTopVolume();

//  tr2=new TGeoRotation(); tr2->RotateZ(360.0/fNModules/2.0);
  mt=new TGeoTube(fContainerR1-d, fContainerR3+d, fPCBSizeZ/2.0+d);
  mtv=new TGeoVolume("Tof", mt, gGeoManager->GetMedium("tof_air_sens"));
  mt1=new TGeoTube(fContainerR1, fContainerR2, fPCBSizeZ/2.0+d);
  mt1v=new TGeoVolume("TofT1", mt1, gGeoManager->GetMedium("tof_air"));
  gGeoManager->Division("TofT1Cell", "TofT1", 2, fNModules, 0.0, -1, 0, "N");
  mt1d=gGeoManager->GetVolume("TofT1Cell");
  phi=TMath::DegToRad()*180.0/fNModules;
  tr1=new TGeoRotation(); tr1->RotateZ(360.0/fNModules/2.0+90.0);
  mt1d->AddNode(fModule, 0, new TGeoCombiTrans(TMath::Cos(phi)*fModuleR1, TMath::Sin(phi)*fModuleR1, 0.0, tr1));
  mt2=new TGeoTube(fContainerR2, fContainerR3, fPCBSizeZ/2.0+d);
  mt2v=new TGeoVolume("TofT2", mt2, gGeoManager->GetMedium("tof_air"));
  gGeoManager->Division("TofT2Cell", "TofT2", 2, fNModules, 0.0, -1, 0, "N");
  mt2d=gGeoManager->GetVolume("TofT2Cell");
  mt2d->AddNode(fModule, 1, new TGeoCombiTrans(TMath::Cos(phi)*fModuleR2, TMath::Sin(phi)*fModuleR2, 0.0, tr1));
  mtv->AddNode(mt1v, 0);
  mtv->AddNode(mt2v, 0, tr2);

  cave->AddNode(mtv, 0);
  AddSensitiveVolume(mtv);
  fWallId=mtv->GetNumber();
  if (fVerboseLevel>99)
    Info("BuildTubes","Added Tof with number %d.", mtv->GetNumber()); 

  return mtv;
}
*/

TGeoVolume* PndDrpc::BuildTubes()
{
  TGeoTube* mt;
  TGeoVolume* mtv;
  Double_t d=0.001;
  TGeoRotation* tr2;
  TGeoRotation* tr1;
  Int_t i;
  Double_t phi;

  TGeoVolume* cave=gGeoManager->GetTopVolume();

//  tr2=new TGeoRotation(); tr2->RotateZ(360.0/fNModules/2.0);
  mt=new TGeoTube(fContainerR1, fContainerR3, fPCBSizeZ/2.0+d);
  mtv=new TGeoVolume("Tof", mt, gGeoManager->GetMedium("tof_air_sens"));
  for(i=0;i<fNModules;i++)
  {

    phi=TMath::DegToRad()*360.0/fNModules*i;
    tr1=new TGeoRotation(); tr1->RotateZ(360.0/fNModules*i+90.0);
    mtv->AddNode(fModule, i, new TGeoCombiTrans(TMath::Cos(phi)*fModuleR1, TMath::Sin(phi)*fModuleR1, 0.0, tr1));


    phi=TMath::DegToRad()*360.0/fNModules*(i+1.0/2.0);
    tr1=new TGeoRotation(); tr1->RotateZ(360.0/fNModules*(i+1.0/2.0)+90.0);
    mtv->AddNode(fModule, fNModules+i, new TGeoCombiTrans(TMath::Cos(phi)*fModuleR2, TMath::Sin(phi)*fModuleR2, 0.0, tr1));

  }

  cave->AddNode(mtv, 0);
  AddSensitiveVolume(mtv);
  fWallId=mtv->GetNumber();
  if (fVerboseLevel>99)
    Info("BuildTubes","Added Tof with number %d.", mtv->GetNumber()); 

  return mtv;
}

TGeoVolume* PndDrpc::BuildModule()
{
  TGeoVolumeAssembly* module;
  TGeoBBox* covertop;
  TGeoVolume* covertopv;
  TGeoBBox* coverside1;
  TGeoVolume* coverside1v;
  TGeoBBox* coverside2;
  TGeoVolume* coverside2v;
  TGeoTubeSeg* tube1[4];
  TGeoVolume* tube1v[4];
  TGeoTubeSeg* tube2[4];
  TGeoVolume* tube2v[4];
  TGeoTubeSeg* tube3[4];
  TGeoVolume* tube3v[4];
  TGeoSphere* sp[8];
  TGeoVolume* spv[8];
  TGeoRotation* tr1;
  TGeoRotation* tr2;
  TGeoRotation* tr3;
  TGeoRotation* tfr1[4];
  TGeoRotation* tfr2[4];
  TGeoCompositeShape* f;
  TGeoVolume* fv;
  TGeoTubeSeg* f1;
  TGeoTube* f2;
  TGeoRotation* tof_r2;
  TGeoTranslation* tof_r1;
  Double_t x;
  Double_t y;
  Double_t z;
  Double_t dx;
  Double_t dz;
  Double_t z0;
  Double_t x0;
  Int_t i;
  Int_t n;
  TString nm;

  TGeoBBox* cs1;
  TGeoBBox* cs1h;
  TGeoVolume* cs1v;
  TGeoVolume* cs1hv;
  TGeoTube* ch;
  TGeoVolume* chv;
  TGeoTube* cs;
  TGeoVolume* csv;
  TGeoBBox* cs2;
  TGeoBBox* cs2h;
  TGeoVolume* cs2v;
  TGeoVolume* cs2hv;
  TGeoRotation* chr;
  TGeoRotation* cs1r[4];

  tr1=new TGeoRotation();
  tr2=new TGeoRotation();
  tr2->RotateX(90);
  tr3=new TGeoRotation();
  tr3->RotateY(90);
  
  module=new TGeoVolumeAssembly("TofModule");
  covertop=new TGeoBBox(fCameraSizeX/2.0+fHolderLen2+fInnerGap, fCoverTh/2.0, (fCameraSizeZ-fCameraOverlap)/2.0*fNCameras+fCameraOverlap/2.0+fInnerGap);
  coverside1=new TGeoBBox(fCoverTh/2.0, fCameraTh/2.0+fHolderTh+fInnerGap2/2.0-fCoverTh/2.0, (fCameraSizeZ-fCameraOverlap)/2.0*fNCameras+fCameraOverlap/2.0+fInnerGap);
  coverside2=new TGeoBBox(fCameraSizeX/2.0+fHolderLen2+fInnerGap, fCameraTh/2.0+fHolderTh+fInnerGap2/2.0-fCoverTh/2.0, fCoverTh/2.0);
  covertopv=new TGeoVolume("TofCoverTop", covertop, gGeoManager->GetMedium(fCoverMat));
  coverside1v=new TGeoVolume("TofCoverSide1", coverside1, gGeoManager->GetMedium(fCoverMat));
  coverside2v=new TGeoVolume("TofCoverSide2", coverside2, gGeoManager->GetMedium(fCoverMat));
  for(i=0;i<4;i++)
  {
    tube1[i]=new TGeoTubeSeg(0.0, fCoverTh, (fCameraSizeZ-fCameraOverlap)/2.0*fNCameras+fCameraOverlap/2.0+fInnerGap, i*90, (i+1)*90);
    nm="TofCoverTube1_"; nm+=i;
    tube1v[i]=new TGeoVolume(nm, tube1[i],  gGeoManager->GetMedium(fCoverMat));

    tube2[i]=new TGeoTubeSeg(0.0, fCoverTh, fCameraTh/2.0+fHolderTh+fInnerGap2/2.0-fCoverTh/2.0, i*90, (i+1)*90);
    nm="TofCoverTube2_"; nm+=i;
    tube2v[i]=new TGeoVolume(nm, tube2[i],  gGeoManager->GetMedium(fCoverMat));

    tube3[i]=new TGeoTubeSeg(0.0, fCoverTh, fCameraSizeX/2.0+fHolderLen2+fInnerGap, i*90, (i+1)*90);
    nm="TofCoverTube3_"; nm+=i;
    tube3v[i]=new TGeoVolume(nm, tube3[i],  gGeoManager->GetMedium(fCoverMat));
  }
  for(i=0;i<4;i++)
  {
    sp[i]=new TGeoSphere(0.0, fCoverTh, 0, 90, i*90, (i+1)*90);
    nm="TofCoverSphere_"; nm+=i;
    spv[i]=new TGeoVolume(nm, sp[i],  gGeoManager->GetMedium(fCoverMat));
    
    sp[i+4]=new TGeoSphere(0.0, fCoverTh, 90, 180, i*90, (i+1)*90);
    nm="TofCoverSphere_"; nm+=i+4;
    spv[i+4]=new TGeoVolume(nm, sp[i+4],  gGeoManager->GetMedium(fCoverMat));
  }
  // Making cover
  y=fCameraTh+fPCBTh/2.0+2.0*fHolderTh+fInnerGap2+0.5*fCoverTh;
  module->AddNode(covertopv, 0, new TGeoTranslation(0.0, y, 0.0));
  module->AddNode(covertopv, 1, new TGeoTranslation(0.0,-y, 0.0));
  y=fCameraTh/2.0+fPCBTh/2.0+fHolderTh+fInnerGap2/2.0+fCoverTh/2.0;
  x=fCameraSizeX/2.0+fHolderLen2+fInnerGap+fCoverTh/2.0;
  module->AddNode(coverside1v, 0, new TGeoTranslation( x, y, 0.0));
  module->AddNode(coverside1v, 1, new TGeoTranslation(-x, y, 0.0));
  module->AddNode(coverside1v, 2, new TGeoTranslation( x,-y, 0.0));
  module->AddNode(coverside1v, 3, new TGeoTranslation(-x,-y, 0.0));
  z=(fCameraSizeZ-fCameraOverlap)/2.0*fNCameras+fCameraOverlap/2.0+fInnerGap+fCoverTh/2.0;
  module->AddNode(coverside2v, 0, new TGeoTranslation(0.0, y, z));
  module->AddNode(coverside2v, 1, new TGeoTranslation(0.0, y,-z));
  module->AddNode(coverside2v, 2, new TGeoTranslation(0.0,-y, z));
  module->AddNode(coverside2v, 3, new TGeoTranslation(0.0,-y,-z));


  y=fCameraTh+fPCBTh/2.0+2.0*fHolderTh+fInnerGap2;
  x=fCameraSizeX/2.0+fHolderLen2+fInnerGap;
  module->AddNode(tube1v[0], 0, new TGeoCombiTrans( x, y, 0.0, tr1));
  module->AddNode(tube1v[1], 0, new TGeoCombiTrans(-x, y, 0.0, tr1));
  module->AddNode(tube1v[2], 0, new TGeoCombiTrans(-x,-y, 0.0, tr1));
  module->AddNode(tube1v[3], 0, new TGeoCombiTrans( x,-y, 0.0, tr1));

  x=fCameraSizeX/2.0+fHolderLen2+fInnerGap+fCoverTh;
  y=fPCBTh/2.0+fCoverTh;
  module->AddNode(tube1v[0], 1, new TGeoCombiTrans(-x,-y, 0.0, tr1));
  module->AddNode(tube1v[1], 1, new TGeoCombiTrans( x,-y, 0.0, tr1));
  module->AddNode(tube1v[2], 1, new TGeoCombiTrans( x, y, 0.0, tr1));
  module->AddNode(tube1v[3], 1, new TGeoCombiTrans(-x, y, 0.0, tr1));

  y=fCameraTh/2.0+fPCBTh/2.0+fHolderTh+fInnerGap2/2.0+fCoverTh/2.0;
  z=(fCameraSizeZ-fCameraOverlap)/2.0*fNCameras+fCameraOverlap/2.0+fInnerGap;
  x=fCameraSizeX/2.0+fHolderLen2+fInnerGap;

  module->AddNode(tube2v[0], 0, new TGeoCombiTrans( x, y, z, tr2));
  module->AddNode(tube2v[1], 0, new TGeoCombiTrans(-x, y, z, tr2));
  module->AddNode(tube2v[2], 0, new TGeoCombiTrans(-x, y,-z, tr2));
  module->AddNode(tube2v[3], 0, new TGeoCombiTrans( x, y,-z, tr2));

  module->AddNode(tube2v[0], 1, new TGeoCombiTrans( x,-y, z, tr2));
  module->AddNode(tube2v[1], 1, new TGeoCombiTrans(-x,-y, z, tr2));
  module->AddNode(tube2v[2], 1, new TGeoCombiTrans(-x,-y,-z, tr2));
  module->AddNode(tube2v[3], 1, new TGeoCombiTrans( x,-y,-z, tr2));

  y=fCameraTh+fPCBTh/2.0+fHolderTh*2.0+fInnerGap2;
  z=(fCameraSizeZ-fCameraOverlap)/2.0*fNCameras+fCameraOverlap/2.0+fInnerGap;
  module->AddNode(tube3v[0], 0, new TGeoCombiTrans(0.0, y,-z, tr3));
  module->AddNode(tube3v[1], 0, new TGeoCombiTrans(0.0, y, z, tr3));
  module->AddNode(tube3v[2], 0, new TGeoCombiTrans(0.0,-y, z, tr3));
  module->AddNode(tube3v[3], 0, new TGeoCombiTrans(0.0,-y,-z, tr3));

  y=fPCBTh/2.0+fCoverTh;
  z=(fCameraSizeZ-fCameraOverlap)/2.0*fNCameras+fCameraOverlap/2.0+fInnerGap+fCoverTh;
  module->AddNode(tube3v[0], 1, new TGeoCombiTrans(0.0,-y, z, tr3));
  module->AddNode(tube3v[1], 1, new TGeoCombiTrans(0.0,-y,-z, tr3));
  module->AddNode(tube3v[2], 1, new TGeoCombiTrans(0.0, y,-z, tr3));
  module->AddNode(tube3v[3], 1, new TGeoCombiTrans(0.0, y, z, tr3));


  y=fCameraTh+fPCBTh/2.0+2.0*fHolderTh+fInnerGap2;
  x=fCameraSizeX/2.0+fHolderLen2+fInnerGap;
  z=(fCameraSizeZ-fCameraOverlap)/2.0*fNCameras+fCameraOverlap/2.0+fInnerGap;
  module->AddNode(spv[0], 0, new TGeoTranslation( x, y, z));
  module->AddNode(spv[1], 0, new TGeoTranslation(-x, y, z));
  module->AddNode(spv[2], 0, new TGeoTranslation(-x,-y, z));
  module->AddNode(spv[3], 0, new TGeoTranslation( x,-y, z));
  module->AddNode(spv[4], 0, new TGeoTranslation( x, y,-z));
  module->AddNode(spv[5], 0, new TGeoTranslation(-x, y,-z));
  module->AddNode(spv[6], 0, new TGeoTranslation(-x,-y,-z));
  module->AddNode(spv[7], 0, new TGeoTranslation( x,-y,-z));

/*
  // Shape if not 100% accurate
  y=fCoverTh+fPCBTh/2.0;
  x=fCameraSizeX/2.0+fHolderLen2+fInnerGap+fCoverTh;
  z=(fCameraSizeZ-fCameraOverlap)/2.0*fNCameras+fCameraOverlap/2.0+fInnerGap+fCoverTh;
  module->AddNode(spv[0], 1, new TGeoTranslation(-x,-y,-z));
  module->AddNode(spv[1], 1, new TGeoTranslation( x,-y,-z));
  module->AddNode(spv[2], 1, new TGeoTranslation( x, y,-z));
  module->AddNode(spv[3], 1, new TGeoTranslation(-x, y,-z));
  module->AddNode(spv[4], 1, new TGeoTranslation(-x,-y, z));
  module->AddNode(spv[5], 1, new TGeoTranslation( x,-y, z));
  module->AddNode(spv[6], 1, new TGeoTranslation( x, y, z));
  module->AddNode(spv[7], 1, new TGeoTranslation(-x, y, z));
*/

  // Should not be desperate !!!! 
  y=fCoverTh+fPCBTh/2.0;
  x=fCameraSizeX/2.0+fHolderLen2+fInnerGap+fCoverTh;
  z=(fCameraSizeZ-fCameraOverlap)/2.0*fNCameras+fCameraOverlap/2.0+fInnerGap+fCoverTh/2.0;
  f1=new TGeoTubeSeg(0.0, fCoverTh, fCoverTh/2.0, 0, 90);
  f1->SetName("tof_f1");
  f2=new TGeoTubeSeg(0.0, fCoverTh, fCoverTh/2.0, 0, 90);
  f2->SetName("tof_f2");
  tof_r1=new TGeoTranslation("tof_r1", 0.0, -fCoverTh/2.0, fCoverTh/2.0);
  tof_r2=new TGeoRotation("tof_r2");
  tof_r2->RotateX(90);
  gGeoManager->RegisterMatrix(tof_r1);
  gGeoManager->RegisterMatrix(tof_r2);
  f=new TGeoCompositeShape("tof_f","tof_f1:tof_r1+tof_f2:tof_r2");
  fv=new TGeoVolume("TofCoverF", f, gGeoManager->GetMedium(fCoverMat));
//  module->AddNode(fv, 0, new TGeoTranslation(-x, -y, -z));
  for(i=0;i<4;i++)
  {
    tfr1[i]=new TGeoRotation();
    tfr1[i]->RotateZ(90);
    tfr1[i]->RotateY(90*i);
    tfr2[i]=new TGeoRotation();
    tfr2[i]->RotateX(180);
    tfr2[i]->RotateZ(-90);
    tfr2[i]->RotateY(90*i);
  }

  module->AddNode(fv, 0, new TGeoCombiTrans( x,-y,  z, tfr1[3]));
  module->AddNode(fv, 1, new TGeoCombiTrans(-x,-y, -z, tfr1[1]));
  module->AddNode(fv, 2, new TGeoCombiTrans( (x-fCoverTh/2.0),-y, -(z+fCoverTh/2.0), tfr1[0]));
  module->AddNode(fv, 3, new TGeoCombiTrans(-(x-fCoverTh/2.0),-y,  (z+fCoverTh/2.0), tfr1[2]));

  module->AddNode(fv, 5, new TGeoCombiTrans( x, y, -z, tfr2[1]));
  module->AddNode(fv, 6, new TGeoCombiTrans(+(x-fCoverTh/2.0), y,  (z+fCoverTh/2.0), tfr2[0]));
  module->AddNode(fv, 7, new TGeoCombiTrans(-(x-fCoverTh/2.0), y, -(z+fCoverTh/2.0), tfr2[2]));
  module->AddNode(fv, 4, new TGeoCombiTrans(-x, y,  z, tfr2[3]));
  
  ch=new TGeoTube(0.0, fCoverHoleR, fCoverTh/2.0);
  chv=new TGeoVolume("TofCoverHole", ch, gGeoManager->GetMedium("tof_polyethylene"));
  chr=new TGeoRotation(); chr->RotateX(90);
  cs=new TGeoTube(0.0, fCoverScrewR, fCoverScrewTh/2.0);
  csv=new TGeoVolume("TofCoverHole", cs, gGeoManager->GetMedium("tof_polyethylene"));
  x=fCameraSizeX/2.0+fHolderLen2+fInnerGap+fCoverTh;
  y=fCoverTh/2.0;
  z=(fCameraSizeZ-fCameraOverlap)/2.0*fNCameras+fCameraOverlap/2.0+fInnerGap+fCoverTh;
  cs1h=new TGeoBBox(fCoverSizeX/4.0-x/2.0, y+fCoverScrewTh/2.0, z);
  cs1hv=new TGeoVolume("TofCoverSide1Holder", cs1h, gGeoManager->GetMedium("tof_air"));
  cs1=new TGeoBBox(fCoverSizeX/4.0-x/2.0, y, z);
  cs1v=new TGeoVolume("TofCoverSide1", cs1, gGeoManager->GetMedium(fCoverMat));
  dz=(fCoverSizeZ-fCoverScrewD*2.0)/(fCoverHolesZ-1);
  z0=-fCoverSizeZ/2.0+fCoverScrewD+dz;
  for(i=0;i<fCoverHolesZ-2;i++)
  {
    cs1v->AddNode(chv, i, new TGeoCombiTrans(fCoverSizeX/4.0-x/2.0-fCoverScrewD,0.0,z0+dz*i, chr));
    cs1hv->AddNode(csv, i, new TGeoCombiTrans(fCoverSizeX/4.0-x/2.0-fCoverScrewD,y,z0+dz*i, chr));
  }
  cs1v->AddNode(chv, i, new TGeoCombiTrans(fCoverSizeX/4.0-x/2.0-fCoverScrewD,0.0,fCoverAddHoleD, chr));
  cs1hv->AddNode(csv, i, new TGeoCombiTrans(fCoverSizeX/4.0-x/2.0-fCoverScrewD,y, fCoverAddHoleD, chr));
  cs1v->SetVisContainers(kTRUE);
  cs1hv->AddNode(cs1v, 0, new TGeoTranslation(0.0, -fCoverScrewTh/2.0, 0.0));
  cs1r[1]=new TGeoRotation(); cs1r[1]->RotateY(180);
  cs1r[2]=new TGeoRotation(); cs1r[2]->ReflectY(kTRUE);
  cs1r[3]=new TGeoRotation(); cs1r[3]->RotateY(180); cs1r[3]->ReflectY(kTRUE);
  module->AddNode(cs1hv, 0, new TGeoTranslation(fCoverSizeX/2.0-(fCoverSizeX/2.0-x)/2.0, y+fCoverScrewTh/2.0+fPCBTh/2.0,0.0));

  module->AddNode(cs1hv, 1, new TGeoCombiTrans(-(fCoverSizeX/2.0-(fCoverSizeX/2.0-x)/2.0), y+fCoverScrewTh/2.0+fPCBTh/2.0,0.0, cs1r[1]));
  module->AddNode(cs1hv, 2, new TGeoCombiTrans(fCoverSizeX/2.0-(fCoverSizeX/2.0-x)/2.0, -y-fCoverScrewTh/2.0-fPCBTh/2.0,0.0, cs1r[2]));
  module->AddNode(cs1hv, 3, new TGeoCombiTrans(-(fCoverSizeX/2.0-(fCoverSizeX/2.0-x)/2.0), -y-fCoverScrewTh/2.0-fPCBTh/2.0,0.0, cs1r[3]));

  cs2h=new TGeoBBox(fCoverSizeX/2.0, y+fCoverScrewTh/2.0, fCoverSizeZ/4.0-z/2.0);
  cs2hv=new TGeoVolume("TofCoverSide2Holder", cs2h, gGeoManager->GetMedium("tof_air"));
  cs2=new TGeoBBox(fCoverSizeX/2.0, y+fCoverScrewTh/2.0, fCoverSizeZ/4.0-z/2.0);
  cs2v=new TGeoVolume("TofCoverSide2", cs2, gGeoManager->GetMedium(fCoverMat));
  dx=(fCoverSizeX-fCoverScrewD*2.0)/(fCoverHolesX-1);
  x0=-fCoverSizeX/2.0+fCoverScrewD; n=fCoverHolesZ;
  for(i=0;i<fCoverHolesX;i++)
  {
    cs2v->AddNode(chv, n+i, new TGeoCombiTrans(x0+i*dx,0.0,fCoverSizeZ/4.0-z/2.0-fCoverScrewD, chr));
    cs2hv->AddNode(csv, n+i, new TGeoCombiTrans(x0+i*dx,y,fCoverSizeZ/4.0-z/2.0-fCoverScrewD, chr));
  }
  cs2v->SetVisContainers(kTRUE);
  cs2hv->AddNode(cs2v, 0, new TGeoTranslation(0.0, -fCoverScrewTh/2.0, 0.0));
  module->AddNode(cs2hv, 0, new TGeoTranslation(0.0,y+fCoverScrewTh/2.0+fPCBTh/2.0,fCoverSizeZ/2.0-(fCoverSizeZ/2.0-z)/2.0));
  module->AddNode(cs2hv, 1, new TGeoCombiTrans(0.0,y+fCoverScrewTh/2.0+fPCBTh/2.0,-fCoverSizeZ/2.0+(fCoverSizeZ/2.0-z)/2.0, cs1r[1]));
  module->AddNode(cs2hv, 2, new TGeoCombiTrans(0.0,-y-fCoverScrewTh/2.0-fPCBTh/2.0,+fCoverSizeZ/2.0-(fCoverSizeZ/2.0-z)/2.0, cs1r[2]));
  module->AddNode(cs2hv, 3, new TGeoCombiTrans(0.0,-y-fCoverScrewTh/2.0-fPCBTh/2.0,-fCoverSizeZ/2.0+(fCoverSizeZ/2.0-z)/2.0, cs1r[3]));

// Now PCB -------------------------------------------------------------------
  TGeoBBox* pcb1;
  TGeoVolume* pcb1v;
  TGeoBBox* pcb2;
  TGeoVolume* pcb2v;
  TGeoTube* pcbh;
  TGeoVolume* pcbhv;

  pcbh=new TGeoTube(0.0, fCoverHoleR, fPCBTh/2.0);
  pcbhv=new TGeoVolume("PCBCoverHole", pcbh, gGeoManager->GetMedium("tof_polyethylene"));

  x=fCameraSizeX/2.0+fHolderLen2+fInnerGap;
  y=fPCBTh/2.0;
  z=(fCameraSizeZ-fCameraOverlap)/2.0*fNCameras+fCameraOverlap/2.0+fInnerGap;
  pcb1=new TGeoBBox(fCoverSizeX/4.0-x/2.0, y, z);
  pcb1v=new TGeoVolume("TofCoverPCB1", pcb1, gGeoManager->GetMedium("tof_g10"));
  for(i=0;i<fCoverHolesZ-2;i++)
    pcb1v->AddNode(pcbhv, i, new TGeoCombiTrans(fCoverSizeX/4.0-x/2.0-fCoverScrewD,0.0,z0+dz*i, chr));
  pcb1v->AddNode(pcbhv, i, new TGeoCombiTrans(fCoverSizeX/4.0-x/2.0-fCoverScrewD,0.0,fCoverAddHoleD, chr));

  module->AddNode(pcb1v, 0, new TGeoTranslation(fCoverSizeX/2.0-(fCoverSizeX/2.0-x)/2.0, 0.0,0.0));
  module->AddNode(pcb1v, 1, new TGeoCombiTrans(-(fCoverSizeX/2.0-(fCoverSizeX/2.0-x)/2.0), 0.0,0.0, cs1r[1]));

  pcb2=new TGeoBBox(fCoverSizeX/2.0, y, fPCBSizeZ/4.0-z/2.0);
  pcb2v=new TGeoVolume("TofCoverPCB2", pcb2, gGeoManager->GetMedium("tof_g10"));
  for(i=0;i<fCoverHolesX;i++)
    pcb2v->AddNode(pcbhv, n+i, new TGeoCombiTrans(x0+i*dx,0.0,fCoverSizeZ/4.0-z/2.0-fCoverScrewD-(fPCBSizeZ/4.0-fCoverSizeZ/4.0), chr));

  module->AddNode(pcb2v, 0, new TGeoTranslation(0.0,0.0,fPCBSizeZ/2.0-(fPCBSizeZ/2.0-z)/2.0));
  module->AddNode(pcb2v, 1, new TGeoCombiTrans(0.0,0.0,-fPCBSizeZ/2.0+(fPCBSizeZ/2.0-z)/2.0, cs1r[1]));

  module->AddNode(fInner, 0);

  if (fBeauty==0) return module;
// TODO: Beauty

  return module;
}

TGeoVolume* PndDrpc::BuildInner()
{
  TGeoBBox* inner;
  TGeoVolume* innerv;
  TGeoBBox* pcbsmall;
  TGeoVolume* pcbsmallv;
  TGeoTube* pcbhole;
  TGeoVolume* pcbholev;
  TGeoTube* pcbscrew;
  TGeoVolume* pcbscrewv;
  TGeoBBox* screwholder;
  TGeoVolume* screwholderv;
  TGeoRotation* r=new TGeoRotation();
  TGeoRotation* r2=new TGeoRotation();
  TGeoRotation* r3=new TGeoRotation();
  TGeoBBox* pcbadd;
  TGeoVolume* pcbaddv;
  Double_t sizex=fCameraSizeZ+2.0*fHolderLen2;
  Double_t z;
  Double_t y;
  Double_t x;
  Int_t i;

  r->RotateX(90);
  r2->RotateY(90);
  r3->RotateY(90);
  r3->RotateX(180);
//Warning X and Z axis at this point are swapped
  pcbsmall=new  TGeoBBox(fCameraSizeX/2.0+fHolderLen2+fInnerGap, fPCBTh/2.0, (fCameraSizeZ-fCameraOverlap)/2.0);
  pcbsmallv=new TGeoVolume("TofPCBS", pcbsmall, gGeoManager->GetMedium("tof_g10"));
  pcbhole=new TGeoTube(0.0, fPCBHoleR, fPCBTh/2.0);
  pcbholev=new TGeoVolume("TofPCBHole", pcbhole, gGeoManager->GetMedium("tof_polyethylene"));
  pcbsmallv->AddNode(pcbholev, 0, new TGeoCombiTrans(-sizex/2.0+fPCBScrewX, 0.0, -fHolderSizeX/2.0+fPCBScrewZ, r));
  pcbsmallv->AddNode(pcbholev, 1, new TGeoCombiTrans(-sizex/2.0+fPCBScrewX, 0.0,  fHolderSizeX/2.0-fPCBScrewZ, r));
  pcbsmallv->AddNode(pcbholev, 2, new TGeoCombiTrans( sizex/2.0-fPCBScrewX, 0.0, -fHolderSizeX/2.0+fPCBScrewZ, r));
  pcbsmallv->AddNode(pcbholev, 3, new TGeoCombiTrans( sizex/2.0-fPCBScrewX, 0.0,  fHolderSizeX/2.0-fPCBScrewZ, r));

  pcbscrew=new TGeoTube(0.0, fPCBScrewR, fPCBScrewTh/2.0);
  pcbscrewv=new TGeoVolume("TofPCBScrew", pcbscrew, gGeoManager->GetMedium("tof_polyethylene"));
  screwholder=new TGeoBBox(sizex/2.0-fPCBScrewX+fPCBScrewR, fPCBScrewTh/2.0, fHolderSizeX/2.0-fPCBScrewZ+fPCBScrewR);
  screwholderv=new TGeoVolume("TofPCBScrewh", screwholder, gGeoManager->GetMedium("tof_gas"));
  screwholderv->AddNode(pcbscrewv, 0, new TGeoCombiTrans(-sizex/2.0+fPCBScrewX, 0.0, -fHolderSizeX/2.0+fPCBScrewZ, r));
  screwholderv->AddNode(pcbscrewv, 1, new TGeoCombiTrans(-sizex/2.0+fPCBScrewX, 0.0,  fHolderSizeX/2.0-fPCBScrewZ, r));
  screwholderv->AddNode(pcbscrewv, 2, new TGeoCombiTrans( sizex/2.0-fPCBScrewX, 0.0, -fHolderSizeX/2.0+fPCBScrewZ, r));
  screwholderv->AddNode(pcbscrewv, 3, new TGeoCombiTrans( sizex/2.0-fPCBScrewX, 0.0,  fHolderSizeX/2.0-fPCBScrewZ, r));

  inner=new TGeoBBox(fCameraSizeX/2.0+fHolderLen2+fInnerGap, fCameraTh+fPCBTh/2.0+fHolderTh+fInnerGap2, (fCameraSizeZ-fCameraOverlap)/2.0*fNCameras+fCameraOverlap/2.0+fInnerGap);
  innerv=new TGeoVolume("TofInner", inner, gGeoManager->GetMedium("tof_gas"));
  for(i=0;i<fNCameras;i++)
  {
    z=-((fCameraSizeZ-fCameraOverlap)/2.0*fNCameras+fCameraOverlap/2.0+fInnerGap);
    z+=(i+0.5)*(fCameraSizeZ-fCameraOverlap)+fCameraOverlap/2.0+fInnerGap;
    innerv->AddNode(pcbsmallv, i, new TGeoTranslation(0.0, 0.0, z));
    y=-fPCBTh/2.0-fPCBScrewTh/2.0;
    if (i%2==1) y=-y;
    innerv->AddNode(screwholderv, i, new TGeoTranslation(0.0, y, z));
    y=fPCBTh/2.0+fCameraTh/2.0+fHolderTh;
    if (i%2==1)
    {
      y=-y;
      innerv->AddNode(fHolder, i, new TGeoCombiTrans(0.0, y, z, r2));
    }
    else
      innerv->AddNode(fHolder, i, new TGeoCombiTrans(0.0, y, z, r3));
  }
  pcbadd=new TGeoBBox(fCameraSizeX/2.0+fHolderLen2+fInnerGap, fPCBTh/2.0, fCameraOverlap/4.0+fInnerGap/2.0);
  pcbaddv=new TGeoVolume("TofPCBA", pcbadd, gGeoManager->GetMedium("tof_g10"));
  z=-((fCameraSizeZ-fCameraOverlap)/2.0*fNCameras+fCameraOverlap/2.0+fInnerGap);
  z+=fCameraOverlap/4.0+fInnerGap/2.0;
  innerv->AddNode(pcbaddv, 0, new TGeoTranslation(0.0, 0.0, z));
  innerv->AddNode(pcbaddv, 1, new TGeoTranslation(0.0, 0.0, -z));

  return innerv; 
}

TGeoVolume* PndDrpc::BuildHolder()
{
  TGeoVolume* holderv;
  TGeoBBox* holder;
  TGeoBBox* hp1;
  TGeoBBox* hp2;
  TGeoBBox* hp3;
  TGeoTube* hsc;
  TGeoVolume* hp1v;
  TGeoVolume* hp2v;
  TGeoVolume* hp3v;
  TGeoVolume* hscv;
  Double_t* buf=NULL;
  Double_t sizez=fCameraSizeZ+2.0*fHolderLen2;
  TGeoRotation* r=new TGeoRotation();
  r->RotateX(90);

  holder=new TGeoBBox(fCameraSizeX/2.0, fCameraTh/2.0+fHolderTh, fCameraSizeZ/2.0+fHolderLen2);
  holderv=new TGeoVolume("TofHolder", holder, gGeoManager->GetMedium("tof_gas"));

  // Upper holder
  hp1=new TGeoBBox(fHolderSizeX/2.0, fHolderTh/2.0, (fHolderLen1-fHolderLen3)/2.0);
  // Middler holder
  hp2=new TGeoBBox(fHolderSizeX/2.0, fHolderTh/2.0+fCameraTh/2.0, fHolderLen2/2.0);
  // Part of the bottom holder
  hp3=new TGeoBBox(fHolderSizeX2/2.0, fHolderTh/2.0, (fHolderLen1-fHolderLen2)/2.0);
  hp1v=new TGeoVolume("Tofhp1v", hp1, gGeoManager->GetMedium("tof_polyethylene"));
  hp2v=new TGeoVolume("Tofhp2v", hp2, gGeoManager->GetMedium("tof_polyethylene"));
  hp3v=new TGeoVolume("Tofhp3v", hp3, gGeoManager->GetMedium("tof_polyethylene"));
  hsc=new TGeoTube(0.0, fHolderScrewR, fHolderTh/2.0);
  hscv=new TGeoVolume("TofHScrewv", hsc, gGeoManager->GetMedium("tof_polyethylene"));

  gGeoManager->Node("TofCamera", 0, "TofHolder", 0.0, 0.0, 0.0, 0, kTRUE, buf, 0);

  gGeoManager->Node("Tofhp1v", 0, "TofHolder", 0.0, -fCameraTh/2.0-fHolderTh/2.0, -sizez/2.0+fHolderLen3+(fHolderLen1-fHolderLen3)/2.0, 0, kTRUE, buf, 0);
  gGeoManager->Node("Tofhp2v", 0, "TofHolder", 0.0, fHolderTh/2.0, -sizez/2.0+fHolderLen2/2.0, 0, kTRUE, buf, 0);
  gGeoManager->Node("Tofhp3v", 0, "TofHolder", -(fHolderSizeX-fHolderSizeX2)/2.0,  fCameraTh/2.0+fHolderTh/2.0, -fCameraSizeZ/2.0+(fHolderLen1-fHolderLen2)/2.0, 0, kTRUE, buf, 0);
  gGeoManager->Node("Tofhp3v", 1, "TofHolder", +(fHolderSizeX-fHolderSizeX2)/2.0,  fCameraTh/2.0+fHolderTh/2.0, -fCameraSizeZ/2.0+(fHolderLen1-fHolderLen2)/2.0, 0, kTRUE, buf, 0);
  gGeoManager->Node("Tofhp3v", 2, "TofHolder", -fHolderSizeX2/2.0,  fCameraTh/2.0+fHolderTh/2.0, -fCameraSizeZ/2.0+(fHolderLen1-fHolderLen2)/2.0, 0, kTRUE, buf, 0);
  gGeoManager->Node("Tofhp3v", 3, "TofHolder",  fHolderSizeX2/2.0,  fCameraTh/2.0+fHolderTh/2.0, -fCameraSizeZ/2.0+(fHolderLen1-fHolderLen2)/2.0, 0, kTRUE, buf, 0);
  
//  gGeoManager->Node("TofHScrewv", 0, "TofHolder", -fHolderSizeX/2.0+fHolderScrewX, -fCameraTh/2.0-fHolderTh/2.0, -sizez/2.0+fHolderScrewZ, 0, kTRUE, buf, 0);
//  gGeoManager->Node("TofHScrewv", 0, "TofHolder", +fHolderSizeX/2.0-fHolderScrewX, -fCameraTh/2.0-fHolderTh/2.0, -sizez/2.0+fHolderScrewZ, 0, kTRUE, buf, 0);

  gGeoManager->Node("Tofhp1v", 1, "TofHolder", 0.0, -fCameraTh/2.0-fHolderTh/2.0, sizez/2.0-fHolderLen3-(fHolderLen1-fHolderLen3)/2.0, 0, kTRUE, buf, 0);
  gGeoManager->Node("Tofhp2v", 1, "TofHolder", 0.0, fHolderTh/2.0, sizez/2.0-fHolderLen2/2.0, 0, kTRUE, buf, 0);
  gGeoManager->Node("Tofhp3v", 5, "TofHolder", -(fHolderSizeX-fHolderSizeX2)/2.0,  fCameraTh/2.0+fHolderTh/2.0, +fCameraSizeZ/2.0-(fHolderLen1-fHolderLen2)/2.0, 0, kTRUE, buf, 0);
  gGeoManager->Node("Tofhp3v", 6, "TofHolder", +(fHolderSizeX-fHolderSizeX2)/2.0,  fCameraTh/2.0+fHolderTh/2.0, +fCameraSizeZ/2.0-(fHolderLen1-fHolderLen2)/2.0, 0, kTRUE, buf, 0);
  gGeoManager->Node("Tofhp3v", 7, "TofHolder", -fHolderSizeX2/2.0,  fCameraTh/2.0+fHolderTh/2.0, +fCameraSizeZ/2.0-(fHolderLen1-fHolderLen2)/2.0, 0, kTRUE, buf, 0);
  gGeoManager->Node("Tofhp3v", 8, "TofHolder",  fHolderSizeX2/2.0,  fCameraTh/2.0+fHolderTh/2.0, +fCameraSizeZ/2.0-(fHolderLen1-fHolderLen2)/2.0, 0, kTRUE, buf, 0);
//  gGeoManager->Node("TofHScrewv", 0, "TofHolder", -fHolderSizeX/2.0+fHolderScrewX, -fCameraTh/2.0-fHolderTh/2.0, +sizez/2.0-fHolderScrewZ, 0, kTRUE, buf, 0);
//  gGeoManager->Node("TofHScrewv", 0, "TofHolder", +fHolderSizeX/2.0-fHolderScrewX, -fCameraTh/2.0-fHolderTh/2.0, +sizez/2.0-fHolderScrewZ, 0, kTRUE, buf, 0);
  holderv->AddNode(hscv, 0, new TGeoCombiTrans(-fHolderSizeX/2.0+fHolderScrewX, -fCameraTh/2.0-fHolderTh/2.0, -sizez/2.0+fHolderScrewZ, r));
  holderv->AddNode(hscv, 1, new TGeoCombiTrans(+fHolderSizeX/2.0-fHolderScrewX, -fCameraTh/2.0-fHolderTh/2.0, -sizez/2.0+fHolderScrewZ, r));
  holderv->AddNode(hscv, 2, new TGeoCombiTrans(-fHolderSizeX/2.0+fHolderScrewX, -fCameraTh/2.0-fHolderTh/2.0,  sizez/2.0-fHolderScrewZ, r));
  holderv->AddNode(hscv, 3, new TGeoCombiTrans(+fHolderSizeX/2.0-fHolderScrewX, -fCameraTh/2.0-fHolderTh/2.0,  sizez/2.0-fHolderScrewZ, r));
  
  return holderv;
}

TGeoVolume* PndDrpc::BuildCamera()
{
  TGeoBBox* camera;
  TGeoVolume* camerav;
  TGeoBBox* cathod;
  TGeoVolume* cathodv;
  TGeoBBox* anod;
  TGeoVolume* anodv;
  TGeoBBox* spacer;
  TGeoVolume* spacerv;
  TGeoBBox* sens;
  TGeoVolume* sensv;
  Int_t i;
  Double_t y;
  Double_t th=fCameraCathodTh+fCameraAnodTh+fCameraGasTh;
  Double_t* buf=0;

  camera=new TGeoBBox(fCameraSizeX/2.0, fCameraTh/2.0, fCameraSizeZ/2.0);
  camerav=new TGeoVolume("TofCamera", camera, gGeoManager->GetMedium("tof_gas"));
  cathod=new TGeoBBox(fCameraSizeX/2.0, fCameraCathodTh/2.0, fCameraSizeZ/2.0);
  cathodv=new TGeoVolume("TofCameraCathod", cathod, gGeoManager->GetMedium("tof_sapphire"));
  anod=new TGeoBBox(fCameraSizeX/2.0, fCameraAnodTh/2.0, fCameraSizeZ/2.0);
  anodv=new TGeoVolume("TofCameraAnod", anod, gGeoManager->GetMedium("tof_sapphire"));
  sens=new TGeoBBox(fCameraSizeX/2.0-fCameraDeadZone, fCameraGasTh/2.0, fCameraSizeZ/2.0-fCameraDeadZone);
  sensv=new TGeoVolume("TofCameraSensGas", sens, gGeoManager->GetMedium("tof_gas_sens"));
//  spacer=new TGeoBBox(fCameraSpacerSize/2.0, fCameraGasTh/2.0, fCameraSizeZ/2.0);
  spacer=new TGeoBBox(fCameraSizeX/2.0, fCameraGasTh/2.0, fCameraSpacerSize/2.0);
  spacerv=new TGeoVolume("TofCameraSpacer", spacer, gGeoManager->GetMedium("tof_sapphire"));

  AddSensitiveVolume(sensv);
  fCameraId=sensv->GetNumber();
  if (fVerboseLevel>99)
    Info("BuildCamera","Added TofCameraSensGas with number %d.", sensv->GetNumber()); 
  for(i=0;i<fCameraLayers;i++)
  {
    if (i%2==0)
    {
      y=-fCameraTh/2.0+fCameraCathodTh/2.0+i*th;
      gGeoManager->Node("TofCameraCathod", i, "TofCamera", 0.0, y, 0.0, 0, kTRUE, buf, 0);
    }
    else
    {
      y=-fCameraTh/2.0+fCameraAnodTh/2.0+i*th;
      gGeoManager->Node("TofCameraAnod", i, "TofCamera", 0.0, y, 0.0, 0, kTRUE, buf, 0);
    }
    if (i%2==0)
      y=-fCameraTh/2.0+fCameraCathodTh+fCameraGasTh/2.0+i*th;
    else
      y=-fCameraTh/2.0+fCameraAnodTh+fCameraGasTh/2.0+i*th;
    gGeoManager->Node("TofCameraSensGas", i, "TofCamera", 0.0, y, 0.0, 0, kTRUE, buf, 0);
    gGeoManager->Node("TofCameraSpacer", 2*i, "TofCamera", 0.0, y, -fCameraSizeZ/2.0+fCameraSpacerSize/2.0, 0, kTRUE, buf, 0);
    gGeoManager->Node("TofCameraSpacer", 2*i+1, "TofCamera", 0.0, y, +fCameraSizeZ/2.0-fCameraSpacerSize/2.0, 0, kTRUE, buf, 0);
//    gGeoManager->Node("TofCameraSpacer", 2*i, "TofCamera", -fCameraSizeX/2.0+fCameraSpacerSize/2.0, y, 0.0, 0, kTRUE, buf, 0);
//    gGeoManager->Node("TofCameraSpacer", 2*i+1, "TofCamera", +fCameraSizeX/2.0-fCameraSpacerSize/2.0, y, 0.0, 0, kTRUE, buf, 0);
    if (i%2==0)
    {
      y=-fCameraTh/2.0+fCameraAnodTh/2.0+fCameraCathodTh+fCameraGasTh+i*th;
      gGeoManager->Node("TofCameraAnod", i, "TofCamera", 0.0, y, 0.0, 0, kTRUE, buf, 0);
    }
    else
    {
      y=-fCameraTh/2.0+fCameraCathodTh/2.0+fCameraAnodTh+fCameraGasTh+i*th;
      gGeoManager->Node("TofCameraCathod", i, "TofCamera", 0.0, y, 0.0, 0, kTRUE, buf, 0);
    }
  }
  return camerav;
}

Int_t PndDrpc::InitMedium(const char* name)
{
  static FairGeoLoader* geoload=FairGeoLoader::Instance();
  static FairGeoInterface* geoface=geoload->getGeoInterface();
  static FairGeoMedia* media=geoface->getMedia();
  static FairGeoBuilder* geobuild=geoload->getGeoBuilder();
  FairGeoMedium* medium=media->getMedium(name);
  Int_t res;
  Int_t i;
  if (medium==NULL)
  {
    Fatal("InitMedium","Material %s not defined in media file.", name);
    return -1111;
  }
  if (fVerboseLevel>99)
    medium->print();
  res=geobuild->createMedium(medium);  

  if (fVerboseLevel>99)
  {
    gGeoManager->GetMedium(name)->GetMaterial()->Print();
    for(i=0;i<10;i++)
      cout << gGeoManager->GetMedium(name)->GetParam(i) << " ";
    cout << endl;
  }
  return res;
}
  
ClassImp(PndDrpc);

