#include "PndMvdDetector.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairGeoMedia.h"
#include "FairGeoVolume.h"
#include "FairRunSim.h"
#include "FairVolume.h"

#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TObjArray.h"
#include "TList.h"
#include "TKey.h"
#include "TGeoManager.h"
#include "TGeoVoxelFinder.h"
#include "TGeoMatrix.h"

#include "PndStack.h"
#include "PndMvdMCPoint.h"
#include "PndMvdGeo.h"
#include "PndMvdGeoPar.h"
#include "PndMvdGeoHandling.h"

#include <iostream>
#include <string>
#include <sstream>

class FairVolume;

// -----   Default constructor   -------------------------------------------
PndMvdDetector::PndMvdDetector() : fUseRadDamOption(false) {
  fPndMvdCollection = new TClonesArray("PndMvdMCPoint");
  fPosIndex = 0;
  fListOfSensitives.push_back("Disk-Sensor");//Root_Test.root
  fListOfSensitives.push_back("Barrel-Sensor");//Root_Test.root
  fListOfSensitives.push_back("PixelActive");//Root_Test.root
  fListOfSensitives.push_back("StripSensor");//MVD14.root
  fListOfSensitives.push_back("SensorActiveArea");//MVD14.root
  fListOfSensitives.push_back("StripActive");//MVD_v1.0.root
  fListOfSensitives.push_back("PixelActive");//MVD_v1.0.root
  //if (fVerboseLevel>0) {
    std::cout<<"-I- PndMvdDetector: fListOfSensitives contains:";
    for(Int_t k=0;k<fListOfSensitives.size();k++)
      std::cout<<"\n\t"<<fListOfSensitives[k];
    std::cout<<std::endl;
  //}
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndMvdDetector::PndMvdDetector (const char* name, Bool_t active)
  : FairDetector(name, active), fUseRadDamOption(false) {
  fPndMvdCollection = new TClonesArray("PndMvdMCPoint");
  fPosIndex = 0;
  fListOfSensitives.push_back("Disk-Sensor");//Root_Test.root
  fListOfSensitives.push_back("Barrel-Sensor");//Root_Test.root
  fListOfSensitives.push_back("PixelActive");
  fListOfSensitives.push_back("StripActive");
  fListOfSensitives.push_back("StripSensor");//MVD14.root
  fListOfSensitives.push_back("SensorActiveArea");//MVD14.root
  fListOfSensitives.push_back("StripActive");//MVD_v1.0.root
  fListOfSensitives.push_back("PixelActive");//MVD_v1.0.root
  //if (fVerboseLevel>0) {
    std::cout<<"- I - PndMvdDetector: fListOfSensitives contains:";
    for(Int_t k=0;k<fListOfSensitives.size();k++)
      std::cout<<"\n\t"<<fListOfSensitives[k];
    std::cout<<std::endl;
  //}
}
// -------------------------------------------------------------------------




// -----   Destructor   ----------------------------------------------------
PndMvdDetector::~PndMvdDetector()
{
  if (fPndMvdCollection)
    {
      fPndMvdCollection->Delete();
      delete fPndMvdCollection;
    }
  delete fGeoH;
}
// -------------------------------------------------------------------------
void PndMvdDetector::Initialize()
{
  std::cout<<" -I- Initializing PndMvdDetector()"<<std::endl;
  FairDetector::Initialize();
  if(0==gGeoManager) {
    std::cout<<" -E- No gGeoManager in PndMvdDetector::Initialize()!"<<std::endl;
    abort();
  }
  fGeoH = new PndMvdGeoHandling(gGeoManager);
}


// -----   Public method ProcessHits  --------------------------------------
Bool_t  PndMvdDetector::ProcessHits(FairVolume* vol)
{
//	std::cout<<"-I- PndMvdDetector::ProcessHits() : called. Please remove this line soon."<<std::endl;
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


  // Create PndMvdMCPoint at exit of active volume

  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) {

      fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();

      if(0==fGeoH) {
        std::cout<<" -E- No PndMvdGeoHandling loaded."<<std::endl;
        abort();
      }
      if (fVerboseLevel > 1){
        std::cout << "*******  Info from gMC *************" << std::endl;
        std::cout << "Hit in " << gMC->CurrentVolPath() << " with MCiD: " << vol->getMCid() << " PixelDetectorID: " << fVolumeID << std::endl;
        std::cout<<"VolumeID: "<<fGeoH->GetID(gMC->CurrentVolPath())<<std::endl;
        std::cout << "PosIn: " << fPosIn.X() << " " << fPosIn.Y() << " " << fPosIn.Z() << " " << fELoss << std::endl;
      }

      gMC->TrackPosition(fPosOut);
      gMC->TrackMomentum(fMomOut);

      if (fUseRadDamOption == false){
    	  if (fELoss == 0.) return kFALSE;
      }

     TString detPath = gMC->CurrentVolPath();
     AddHit(fTrackID, kMVDPoint, fGeoH->GetID(detPath),
        TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
        TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
        TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
        TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
        fTime, fLength, fELoss);

      // Increment number of PndMvd points for TParticle
      PndStack* stack = (PndStack*) gMC->GetStack();
      stack->AddPoint(kMVD);
      ResetParameters();
    }

  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   Public method EndOfEvent   --------------------------------------
void PndMvdDetector::EndOfEvent()
{
  if (fVerboseLevel)
    Print();

  fPndMvdCollection->Delete();
  fPosIndex = 0;
}
// -------------------------------------------------------------------------

void PndMvdDetector::FinishRun()
{

}

// -----   Public method Register   ----------------------------------------
void PndMvdDetector::Register()
{
  FairRootManager::Instance()->Register("MVDPoint", "PndMvd", fPndMvdCollection, kTRUE);
}
// -------------------------------------------------------------------------



// -----   Public method GetCollection   -----------------------------------
TClonesArray* PndMvdDetector::GetCollection(Int_t iColl) const
{
  if (iColl == 0)
    return fPndMvdCollection;
  else
    return NULL;
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void PndMvdDetector::Print() const
{
  Int_t
    nHits = fPndMvdCollection->GetEntriesFast();

  std::cout << "-I- PndMvdDetector: " << nHits << " points registered in this event."  << std::endl;

  if (fVerboseLevel>1)
    for (Int_t i=0; i<nHits; i++)
      (*fPndMvdCollection)[i]->Print();
}
// -------------------------------------------------------------------------



// -----   Public method Reset   -------------------------------------------
void PndMvdDetector::Reset()
{
  fPndMvdCollection->Delete();
  ResetParameters();
}
// -------------------------------------------------------------------------



// -----   Public method CopyClones   --------------------------------------
void PndMvdDetector::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
  Int_t
    nEntries = cl1->GetEntriesFast();

  std::cout << "-I- PndMvdDetector: " << nEntries << " entries to add." << std::endl;

  TClonesArray& clref = *cl2;

  PndMvdMCPoint
    *oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++)
     {
       oldpoint = (PndMvdMCPoint*) cl1->At(i);

       Int_t
   index = oldpoint->GetTrackID() + offset;

       oldpoint->SetTrackID(index);
       new (clref[fPosIndex]) PndMvdMCPoint(*oldpoint);
       fPosIndex++;
     }
   std::cout << "-I- PndMvdDetector: " << cl2->GetEntriesFast() << " merged entries."
  << std::endl;
}

// -------------------------------------------------------------------------
void PndMvdDetector::ConstructGeometry()
{
  TString fileName=GetGeometryFileName();
        if(fileName.EndsWith(".geo")){
    ConstructASCIIGeometry();
  }else if(fileName.EndsWith(".root")){
    ConstructRootGeometry();
  }else{
    std::cout<< "Geometry format not supported " <<std::endl;
  }
}

// -------------------------------------------------------------------------
bool PndMvdDetector::CheckIfSensitive(std::string name)
{
  for (Int_t i = 0; i < fListOfSensitives.size(); i++){
    if (name.find(fListOfSensitives[i]) != std::string::npos)
    return true;
  }
  return false;
}




// -----   Public method ConstructGeometry   -------------------------------
void PndMvdDetector::ConstructASCIIGeometry()
{
  // get pointer to the instantons which interface
  // to monte carlo

  FairGeoLoader *geoLoad = FairGeoLoader::Instance();
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  PndMvdGeo *thePndMvdGeo  = new PndMvdGeo();

  thePndMvdGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(thePndMvdGeo);

  Bool_t rc = geoFace->readSet(thePndMvdGeo);

  if (rc)
    thePndMvdGeo->create(geoLoad->getGeoBuilder());

  TList* volList = thePndMvdGeo->getListOfVolumes();

  // store geo parameter
  FairRun *fRun = FairRun::Instance();

  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();

  PndMvdGeoPar *par= (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));

  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();

  TObjArray *fPassNodes = par->GetGeoPassiveNodes();

  TListIter iter(volList);

  FairGeoNode   *node = NULL;
  FairGeoVolume *aVol = NULL;

  while( (node = (FairGeoNode*)iter.Next()) ) {
      aVol = dynamic_cast<FairGeoVolume*> ( node );
       if ( node->isSensitive()  ) {
           fSensNodes->AddLast( aVol );
       }else{
           fPassNodes->AddLast( aVol );
       }
  }

  par->setChanged();
  par->setInputVersion(fRun->GetRunId(),1);

  ProcessNodes ( volList );
}
// -------------------------------------------------------------------------

// -----   Public method SetExclusiveSensorType   -------------------------------
void PndMvdDetector::SetExclusiveSensorType(const TString sens)
{
  //Set one exclusive sensor type for testing purposes
  fListOfSensitives.clear();
  fListOfSensitives.push_back(sens.Data());
  std::cout<<"-I- PndMvdDetector: Only active sensor type is set to \""<<sens.Data()<<"\","<<std::endl;
  std::cout<<"    this is not a default setting."<<std::endl;
}

// -------------------------------------------------------------------------



// -----   Private method AddHit   -----------------------------------------
PndMvdMCPoint* PndMvdDetector::AddHit(Int_t trackID, Int_t detID, TString detName, TVector3 posIn,              TVector3 posOut,TVector3 momIn, TVector3 momOut,
            Double_t time, Double_t length, Double_t eLoss) const
{
  TClonesArray&
    clref = *fPndMvdCollection;

  Int_t
    size = clref.GetEntriesFast();

    if (fVerboseLevel >= 2)
       std::cout << "-I- PndMvdDetector: Adding Point at (" << posIn.X() << ", " << posIn.Y()
      << ", " << posIn.Z() << ") cm, (" << posOut.X() << ", " << posOut.Y()
      << ", " << posOut.Z() << ") cm,  detector " << detName << " " << detID << ", track "
      << trackID << ", energy loss " << eLoss*1e06 << " keV" << std::endl;

  return new(clref[size]) PndMvdMCPoint(trackID, detID, detName, posIn, posOut,
                        momIn, momOut, time, length, eLoss);
}
// -------------------------------------------------------------------------



ClassImp(PndMvdDetector);
