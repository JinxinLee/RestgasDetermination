// -------------------------------------------------------------------------
// -----                PndMdtDigiProducer source file                  -----
// -----                  Created 29/03/11  by  S.Spataro              -----
// -------------------------------------------------------------------------

#include "PndMdtDigiProducer.h"

#include "PndMdtDigi.h"
#include "PndMdtPoint.h"

#include "FairRootManager.h"
#include "FairDetector.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoNode.h"
#include "TGeoBBox.h"
#include "TGeoMatrix.h"
#include "TVector3.h"
#include "TRandom.h"

#include <iostream>

using std::map;
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndMdtDigiProducer::PndMdtDigiProducer() :
  FairTask(" MDT Digi Producer") { 
  fStripMode = kFALSE;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMdtDigiProducer::~PndMdtDigiProducer() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndMdtDigiProducer::Init() {
  
  cout << "-I- PndMdtDigiProducer::Init: "
       << "INITIALIZATION *********************" << endl;
  
  FairRun* sim = FairRun::Instance();
  FairRuntimeDb* rtdb=sim->GetRuntimeDb();
    
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndMdtDigiProducer::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  
  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject("MdtPoint");
  if ( ! fPointArray ) {
    cout << "-W- PndMdtDigiProducer::Init: "
	 << "No MdtPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fDigiBoxArray = new TClonesArray("PndMdtDigi");
  ioman->Register("MdtDigiBox","Mdt",fDigiBoxArray,kTRUE);

  if (fStripMode)
    {
      fDigiStripArray = new TClonesArray("PndMdtDigi");
      ioman->Register("MdtDigiStrip","Mdt",fDigiStripArray,kTRUE);
    }
 
  TGeoVolume *volume = (TGeoVolume*)gGeoManager->FindVolumeFast("MdtBarrel");
  TGeoBBox *box = (TGeoBBox*)volume->GetShape();
  const Double_t *origin = box->GetOrigin();
  
  fBarrelStartZ = origin[2]+box->GetDZ();
  cout <<  fBarrelStartZ << endl;
  
  cout << "-I- PndMdtDigiProducer: Intialization successfull" << endl;
  
  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndMdtDigiProducer::Exec(Option_t* opt) 
{ 
  // Reset output array
  fDigiBoxArray->Delete();
  if (fStripMode) fDigiStripArray->Delete();

  map<Int_t, std::vector <Int_t> > mapStripPoint; // PointIds with same DetId - strip 
  map<Int_t, std::vector <Int_t> > mapBoxPoint;   // PointIds with same DetId - box
  map<Int_t, TVector3> mapStripPos;               // Position for strip
  map<Int_t, TVector3> mapBoxPos;                 // Position for box
  
  mapStripPoint.clear();
  mapBoxPoint.clear();
  mapStripPos.clear();
  mapBoxPos.clear();
  
  TVector3 mdtSize(0.5, 0.5, 0.5);
  
  // Loop over MdtPoints
  Int_t nPoints = fPointArray->GetEntriesFast();
  PndMdtPoint *point = 0;
  TVector3 inPos, outPos, meanPos;
  
  for (Int_t iPoint=0; iPoint<nPoints; iPoint++) {
    point  = (PndMdtPoint*) fPointArray->At(iPoint);
    if (point->GetEnergyLoss()==0) continue;
    
    point->Position(inPos);
    outPos = point->GetPosIn();
    
    if ((inPos-outPos).Mag()<0.2) continue; // skipped points
    
    meanPos = 0.5*(inPos+outPos);
    TGeoNode *mdtNode = (TGeoNode*)gGeoManager->FindNode(meanPos.X(), meanPos.Y(), meanPos.Z());
    TGeoMatrix *mdtMat = (TGeoMatrix*)gGeoManager->GetCurrentMatrix();
    const Double_t *matM = mdtMat->GetTranslation();
    TVector3 tubePos(matM[0], matM[1], matM[2]);
    TVector3 stripPos;
    Int_t stripNum = -10, stripId = -10;
    if (point->GetModule()==1)
      {
	stripNum = (Int_t)(fBarrelStartZ - point->GetZ());
	stripPos.SetXYZ(matM[0], matM[1], fBarrelStartZ - stripNum - 0.5);
	stripId = stripNum + 1000*point->GetLayerID() + 100000*point->GetSector() + 1000000*point->GetModule();
      }
    
    if (fStripMode)
      {
	mapBoxPoint[point->GetDetectorID()].push_back(iPoint);
	mapBoxPos[point->GetDetectorID()] = tubePos;
	mapStripPoint[stripId].push_back(iPoint);
	mapStripPos[stripId] = stripPos;
      }
    else
      {
	mapBoxPoint[point->GetDetectorID()].push_back(iPoint);
	mapBoxPos[point->GetDetectorID()] = stripPos;
      }
    
  } // Loop over MdtPoints
  
  map<Int_t, std::vector <Int_t> >::const_iterator boxIter;
  for( boxIter = mapBoxPoint.begin(); boxIter != mapBoxPoint.end(); ++boxIter)
    {
      AddDigiBox((*boxIter).first, 	mapBoxPos[(*boxIter).first], (*boxIter).second);
    } 
  map<Int_t, std::vector <Int_t> >::const_iterator stripIter;
  for( stripIter = mapStripPoint.begin(); stripIter != mapStripPoint.end(); ++stripIter)
    {
      AddDigiStrip((*stripIter).first, 	mapStripPos[(*stripIter).first], (*stripIter).second);
    }
  
  
  
}
// -------------------------------------------------------------------------


// -----   Private method AddDigi   --------------------------------------------
PndMdtDigi* PndMdtDigiProducer::AddDigiBox(Int_t detID, TVector3& pos, std::vector<Int_t> pointList)
{
  // It fills the PndMdtDigi category
  TClonesArray& clref = *fDigiBoxArray;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndMdtDigi(detID, pos, pointList);
}
// ----


// -----   Private method AddDigi   --------------------------------------------
PndMdtDigi* PndMdtDigiProducer::AddDigiStrip(Int_t detID, TVector3& pos, std::vector<Int_t> pointList)
{
  // It fills the PndMdtDigi category
 
  TClonesArray& clref = *fDigiStripArray;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndMdtDigi(detID, pos, pointList);
}
// ----


ClassImp(PndMdtDigiProducer)
