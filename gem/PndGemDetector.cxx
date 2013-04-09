/////////////////////////////////////////////////////////////
// PndGemDetector
//
// Class for PndGemDetector
//
/////////////////////////////////////////////////////////////

#include "PndGemDetector.h"

#include "FairRootManager.h"
#include "FairVolume.h"

#include "PndDetectorList.h"
#include "PndGemMCPoint.h"
#include "PndStack.h"

#include "TClonesArray.h"
#include "TVirtualMC.h"
#include "TGeoManager.h"
#include "TGeoPhysicalNode.h"

#include <iostream>
#include <string>
#include <sstream>

class FairGeoLoader;
class FairGeoMedia;
class FairRunSim;
class FairGeoInterface;
class FairGeoRootBuilder;
class FairGeoNode;
class FairRun;
class FairRuntimeDb;
class FairGeoVolume;

class TGeoVoxelFinder;
class TObjArray;
class TList;
class TGeoMatrix;
class TParticle;
class TLorentzVector;
class TKey;


// -----   Default constructor   -------------------------------------------
PndGemDetector::PndGemDetector() : fUseRadDamOption(false) {
  fPndGemCollection = new TClonesArray("PndGemMCPoint");
  fPosIndex = 0;
  fListOfSensitives.push_back("Sensor");
  if (fVerboseLevel>0) {
    std::cout<<"-I- PndGemDetector: fListOfSensitives contains:";
    for(Int_t k=0;k<fListOfSensitives.size();k++)
      std::cout<<"\n\t"<<fListOfSensitives[k];
    std::cout<<std::endl;
  }
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndGemDetector::PndGemDetector (const char* name, Bool_t active)
  : FairDetector(name, active), fUseRadDamOption(false) {
  fPndGemCollection = new TClonesArray("PndGemMCPoint");
  fPosIndex = 0;
  fListOfSensitives.push_back("Sensor");
  if (fVerboseLevel>0) {
    std::cout<<"- I - PndGemDetector: fListOfSensitives contains:";
    for(Int_t k=0;k<fListOfSensitives.size();k++)
      std::cout<<"\n\t"<<fListOfSensitives[k];
    std::cout<<std::endl;
  }
}
// -------------------------------------------------------------------------




// -----   Destructor   ----------------------------------------------------
PndGemDetector::~PndGemDetector()
{
  if (fPndGemCollection)
    {
      fPndGemCollection->Delete();
      delete fPndGemCollection;
    }
//   delete fGeoH;
}
// -------------------------------------------------------------------------
void PndGemDetector::Initialize()
{
  std::cout<<" -I- Initializing PndGemDetector()"<<std::endl;
  FairDetector::Initialize();
  if(0==gGeoManager) {
    std::cout<<" -E- No gGeoManager in PndGemDetector::Initialize()!"<<std::endl;
    abort();
  }
//   fGeoH = new PndGemGeoHandling(gGeoManager);
}


// -----   Public method ProcessHits  --------------------------------------
Bool_t  PndGemDetector::ProcessHits(FairVolume* vol)
{

  if ( gMC->IsTrackEntering() )
  {
    // Set parameters at entrance of volume. Reset ELoss.
    fELoss  = 0.;
    fTime   = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPosIn);
    gMC->TrackMomentum(fMomIn);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();


  // Create PndGemMCPoint at exit of active volume

  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) {

      fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();

/*      if(0==fGeoH) {
        std::cout<<" -E- No PndGemGeoHandling loaded."<<std::endl;
        abort();
      }*/
      if (fVerboseLevel > 1){
        std::cout << "*******  Info from gMC *************" << std::endl;
        std::cout << "Hit in " << gMC->CurrentVolPath() << " with MCiD: " << vol->getMCid() << " PixelDetectorID: " << kGEM << std::endl;
//         std::cout<<"VolumeID: "<<fGeoH->GetID(gMC->CurrentVolPath())<<std::endl;
        std::cout << "PosIn: " << fPosIn.X() << " " << fPosIn.Y() << " " << fPosIn.Z() << " " << fELoss << std::endl;
      }

      gMC->TrackPosition(fPosOut);
      gMC->TrackMomentum(fMomOut);

      if (fUseRadDamOption == false){
    	  if (fELoss == 0.) return kFALSE;
      }

     TString detPath = gMC->CurrentVolPath();
     Int_t sensID = GetSensorId(detPath);
     AddHit(fTrackID, kGEM, sensID,
	    TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
	    TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
	    TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
	    TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
	    fTime, fLength, fELoss);
     
     // Increment number of PndGem points for TParticle
     PndStack* stack = (PndStack*) gMC->GetStack();
     stack->AddPoint(kGEM);
     
     ResetParameters();
  }
  
  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   Public method EndOfEvent   --------------------------------------
void PndGemDetector::EndOfEvent()
{
  if (fVerboseLevel)
    Print();

  fPndGemCollection->Delete();
  fPosIndex = 0;
}
// -------------------------------------------------------------------------

void PndGemDetector::FinishRun()
{

}

// -----   Public method Register   ----------------------------------------
void PndGemDetector::Register()
{
  FairRootManager::Instance()->Register("GEMPoint", "PndGem", fPndGemCollection, kTRUE);
}
// -------------------------------------------------------------------------



// -----   Public method GetCollection   -----------------------------------
TClonesArray* PndGemDetector::GetCollection(Int_t iColl) const
{
  if (iColl == 0)
    return fPndGemCollection;
  else
    return NULL;
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void PndGemDetector::Print() const
{
  Int_t
    nHits = fPndGemCollection->GetEntriesFast();

  std::cout << "-I- PndGemDetector: " << nHits << " points registered in this event."  << std::endl;

  if (fVerboseLevel>1)
    for (Int_t i=0; i<nHits; i++)
      (*fPndGemCollection)[i]->Print();
}
// -------------------------------------------------------------------------



// -----   Public method Reset   -------------------------------------------
void PndGemDetector::Reset()
{
  fPndGemCollection->Delete();
  ResetParameters();
}
// -------------------------------------------------------------------------



// -----   Public method CopyClones   --------------------------------------
void PndGemDetector::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
  Int_t
    nEntries = cl1->GetEntriesFast();

  std::cout << "-I- PndGemDetector: " << nEntries << " entries to add." << std::endl;

  TClonesArray& clref = *cl2;

  PndGemMCPoint
    *oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++)
     {
       oldpoint = (PndGemMCPoint*) cl1->At(i);

       Int_t
   index = oldpoint->GetTrackID() + offset;

       oldpoint->SetTrackID(index);
       new (clref[fPosIndex]) PndGemMCPoint(*oldpoint);
       fPosIndex++;
     }
   std::cout << "-I- PndGemDetector: " << cl2->GetEntriesFast() << " merged entries."
  << std::endl;
}

// -------------------------------------------------------------------------
void PndGemDetector::ConstructGeometry()
{
  TString fileName=GetGeometryFileName();
//   if(fileName.EndsWith(".geo")){
//     ConstructASCIIGeometry();
//   }else 
  if(fileName.EndsWith(".root")){
    ConstructRootGeometry();
  }else{
    std::cout<< "Geometry format not supported " <<std::endl;
  }
}

// -------------------------------------------------------------------------
void PndGemDetector::MisalignDetector()
{
  std::cout << "-----------------------------------" << std::endl;
  std::cout << " M I S A L I G N   D E T E C T O R " << std::endl;

  /*  TGeoPhysicalNode* pn1 = gGeoManager->MakePhysicalNode("/cave_1/Gem_Disks_0/Gem_Disk1_Volume_0/Gem_Disk1_Seg1_Gem1_Sensor_GEMmixture_0");
  cout << "got the node " << pn1 << endl;
  cout << "print the orig rot matrix:" << endl;
  pn1->GetOriginalMatrix()->Print();
  cout << "print the rot matrix:" << endl;
  pn1->GetMatrix()->Print();
  
  TGeoHMatrix* dummyRot = new TGeoHMatrix();
  //  dummyRot->RotateZ(90);
  dummyRot->RotateX(0.3);
  dummyRot->RotateY(0.1);
  Double_t trans[3] = {0.,-0.1,-1.123400};
  dummyRot->SetTranslation(trans);
 
  cout << "dummyRot ---> " << endl;
  dummyRot->Print();

  cout << "aligning to dummyRot" << endl;
  pn1->Align(dummyRot); // in Align, if (!newmat&&!newshape)return;*/

  Int_t nofSeg = 2;
  for ( Int_t ist = 0 ; ist < 3 ; ist++ ) {
    if ( ist == 2 ) nofSeg = 3;
    for ( Int_t isg = 0 ; isg < nofSeg ; isg++ ) {
      for ( Int_t isp = 0 ; isp < 2 ; isp++ ) {
	TString tName = Form("/cave_1/Gem_Disks_0/Gem_Disk%d_Volume_0/Gem_Disk%d_Seg%d_Gem%d_Sensor_GEMmixture_0",ist+1,ist+1,isg+1,isp*5+1);
	cout << tName.Data() << endl;	
	TGeoPhysicalNode* tgpn = gGeoManager->MakePhysicalNode(tName.Data());
	TGeoHMatrix* tghm = (TGeoHMatrix*)tgpn->GetOriginalMatrix();
	cout << " * * *  o r i g  * * *  o r i g  * * *  o r i g  * * *  o r i g  * * * " << endl;
	tghm->Print();
	tghm->RotateX(gRandom->Gaus(0.,.1));
	tghm->RotateY(gRandom->Gaus(0.,.1));
	tghm->RotateZ(gRandom->Gaus(0.,.1));
	Double_t* trans = tghm->GetTranslation();
	cout << "trans = " << trans[0] << " " << trans[1] << " " << trans[2] << endl;
	for ( Int_t ic = 0 ; ic < 3 ; ic++ ) 
	  trans[0] += gRandom->Gaus(0.,.03);
	tghm->SetTranslation(trans);
	cout << " * * * m o v e d * * * m o v e d * * * m o v e d * * * m o v e d * * * " << endl;
	tghm->Print();
	cout << " * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * " << endl;
	tgpn->Align(tghm);
      }
    }
  }
 
  std::cout << "-----------------------------------" << std::endl;
}

// -------------------------------------------------------------------------
bool PndGemDetector::CheckIfSensitive(std::string name)
{
  for (Int_t i = 0; i < fListOfSensitives.size(); i++){
    if (name.find(fListOfSensitives[i]) != std::string::npos)
    return true;
  }
  return false;
}

// -------------------------------------------------------------------------
Int_t PndGemDetector::GetSensorId(TString detName)
{
  //  std::cout << "name is " << detName.Data() << " -> " << std::flush;
  detName.Remove(0,detName.Last('/')+1);
  detName.Remove(0,detName.First("Disk")+4);
  Int_t stationNr = detName.Atoi();
  detName.Remove(0,detName.First("Seg")+3);
  Int_t segmentNr = detName.Atoi();
  detName.Remove(0,detName.First("Gem")+3);
  Int_t sensorNr  = detName.Atoi();
  if ( sensorNr == 6 ) sensorNr = 2;
//   std::cout << "stat " << stationNr << " sens " << sensorNr << " seg " << segmentNr << " > " 
// 	    << stationNr*256+sensorNr*16+segmentNr << std::endl;
  return stationNr*256+sensorNr*16+segmentNr;
}

// -----   Public method ConstructGeometry   -------------------------------
// void PndGemDetector::ConstructASCIIGeometry()
// {
//   // get pointer to the instantons which interface
//   // to monte carlo
// 
//   FairGeoLoader *geoLoad = FairGeoLoader::Instance();
//   FairGeoInterface *geoFace = geoLoad->getGeoInterface();
//   PndGemGeo *thePndGemGeo  = new PndGemGeo();
// 
//   thePndGemGeo->setGeomFile(GetGeometryFileName());
//   geoFace->addGeoModule(thePndGemGeo);
// 
//   Bool_t rc = geoFace->readSet(thePndGemGeo);
// 
//   if (rc)
//     thePndGemGeo->create(geoLoad->getGeoBuilder());
// 
//   TList* volList = thePndGemGeo->getListOfVolumes();
// 
//   // store geo parameter
//   FairRun *fRun = FairRun::Instance();
// 
//   FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
// 
//   PndGemGeoPar *par= (PndGemGeoPar*)(rtdb->getContainer("PndGemGeoPar"));
// 
//   TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
// 
//   TObjArray *fPassNodes = par->GetGeoPassiveNodes();
// 
//   TListIter iter(volList);
// 
//   FairGeoNode   *node = NULL;
//   FairGeoVolume *aVol = NULL;
// 
//   while( (node = (FairGeoNode*)iter.Next()) ) {
//       aVol = dynamic_cast<FairGeoVolume*> ( node );
//        if ( node->isSensitive()  ) {
//            fSensNodes->AddLast( aVol );
//        }else{
//            fPassNodes->AddLast( aVol );
//        }
//   }
// 
//   par->setChanged();
//   par->setInputVersion(fRun->GetRunId(),1);
// 
//   ProcessNodes ( volList );
// }
// -------------------------------------------------------------------------

// -----   Public method SetExclusiveSensorType   -------------------------------
void PndGemDetector::SetExclusiveSensorType(const TString sens)
{
  //Set one exclusive sensor type for testing purposes
  fListOfSensitives.clear();
  fListOfSensitives.push_back(sens.Data());
  std::cout<<"-I- PndGemDetector: Only active sensor type is set to \""<<sens.Data()<<"\","<<std::endl;
  std::cout<<"    this is not a default setting."<<std::endl;
}

// -------------------------------------------------------------------------



// -----   Private method AddHit   -----------------------------------------
PndGemMCPoint* PndGemDetector::AddHit(Int_t trackID, Int_t detID, Int_t sensID, 
				      TVector3 posIn, TVector3 posOut,TVector3 momIn, TVector3 momOut,
				      Double_t time, Double_t length, Double_t eLoss)
{
  TClonesArray&
    clref = *fPndGemCollection;
  
  Int_t
    size = clref.GetEntriesFast();

    if (fVerboseLevel >= 2)
       std::cout << "-I- PndGemDetector: Adding Point at (" << posIn.X() << ", " << posIn.Y()
      << ", " << posIn.Z() << ") cm, (" << posOut.X() << ", " << posOut.Y()
      << ", " << posOut.Z() << ") cm,  sensor " << sensID << " " << detID << ", track "
      << trackID << ", energy loss " << eLoss*1e06 << " keV" << std::endl;

  return new(clref[size]) PndGemMCPoint(trackID, detID, sensID, posIn, posOut,
                        momIn, momOut, time, length, eLoss);
}
// -------------------------------------------------------------------------



ClassImp(PndGemDetector);
