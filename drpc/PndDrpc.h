#ifndef PNDDRPC_H
#define PNDDRPC_H

#include "FairDetector.h"

class TClonesArray;
class TGeoVolume;

class PndDrpc : public FairDetector
{
public:
  /** Default constructor. Don't use! **/
  PndDrpc();
  /** Standard constructor. active --- sensitivity flag **/
  PndDrpc(const char* name, Bool_t active);
  virtual ~PndDrpc();
  virtual void Initialize();
  virtual Bool_t ProcessHits(FairVolume* vol=NULL);
  virtual void EndOfEvent();
  virtual void BeginEvent();
  virtual void Register();
  virtual TClonesArray* GetCollection(Int_t iColl) const;
  virtual void Print();
  virtual void Reset();
  virtual void CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset);
  virtual void ConstructGeometry();
private:
  Int_t InitMedium(const char* name);
  void LoadParam();
  TGeoVolume* BuildCamera();
  TGeoVolume* BuildHolder();
  TGeoVolume* BuildInner();
  TGeoVolume* BuildModule();
  TGeoVolume* BuildTubes();

  Double_t fCameraSizeX;
  Double_t fCameraSizeZ;
  Double_t fCameraAnodTh;
  Double_t fCameraCathodTh;
  Double_t fCameraGasTh;
  Double_t fCameraSpacerSize;
  Double_t fCameraLayers;
  Double_t fCameraTh;
  Double_t fCameraDeadZone;
  Double_t fHolderTh;
  Double_t fHolderScrewR;
  Double_t fHolderScrewX;
  Double_t fHolderScrewZ;
  Double_t fHolderSizeX;
  Double_t fHolderSizeX2;
  Double_t fHolderLen1;
  Double_t fHolderLen2;
  Double_t fHolderLen3;
  Double_t fPCBTh;
  Double_t fCameraOverlap;
  Double_t fPCBHoleR;
  Double_t fPCBScrewR;
  Double_t fPCBScrewTh;
  Double_t fPCBScrewX;
  Double_t fPCBScrewZ;
  Double_t fInnerGap;
  Double_t fInnerGap2;
  Double_t fCoverTh;
  TString fCoverMat;
  Double_t fCoverSizeX;
  Double_t fCoverSizeZ;
  Double_t fPCBSizeZ;
  Double_t fCoverScrewD;
  Double_t fCoverHoleR;
  Double_t fCoverScrewR;
  Double_t fCoverScrewTh;
  Double_t fCoverAddHoleD;
  Int_t fCoverHolesX;
  Int_t fCoverHolesZ;
  Int_t fNCameras;
  Double_t fContainerR1;
  Double_t fContainerR2;
  Double_t fContainerR3;
  Double_t fModuleR1;
  Double_t fModuleR2;
  Int_t fNModules;
  Int_t fBeauty;

  Int_t fPosIndex;

  Int_t fWallId;
  Int_t fCameraId;

  TGeoVolume* fCamera;			//! Camera
  TGeoVolume* fHolder;			//! Camera+Holder
  TGeoVolume* fInner;			//! Inner part of module
  TGeoVolume* fModule;			//! Tof Module 
  TGeoVolume* fTof;			//! Tof main  volume
  TClonesArray* fPoints;		//!
  TClonesArray* fMCPoints;		//!
  ClassDef(PndDrpc, 1);
};

#endif

