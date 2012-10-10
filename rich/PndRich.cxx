#include "PndRich.h"

#include "PndRichPoint.h"
#include "PndRichGeo.h"
#include "PndRichGeoPar.h"

#include "FairVolume.h"
#include "FairGeoVolume.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "PndDetectorList.h"
#include "PndStack.h"

#include "TClonesArray.h"
#include "TVirtualMC.h"

#include <iostream>
using std::cout;
using std::endl;

PndRich::PndRich()
  : FairDetector("PndRich", kTRUE, kRICH),
    fTrackID(-1),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1),
    fPndRichPointCollection(new TClonesArray("PndRichPoint"))
{
}

PndRich::PndRich(const char* name, Bool_t active)
  : FairDetector(name, active, kRICH),
    fTrackID(-1),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1),
    fPndRichPointCollection(new TClonesArray("PndRichPoint"))
{
}

PndRich::~PndRich()
{
  if (fPndRichPointCollection) {
    fPndRichPointCollection->Delete();
    delete fPndRichPointCollection;
  }
}

void PndRich::Initialize()
{
  FairDetector::Initialize();
  FairRuntimeDb* rtdb= FairRun::Instance()->GetRuntimeDb();
  PndRichGeoPar* par=(PndRichGeoPar*)(rtdb->getContainer("PndRichGeoPar"));
}

Bool_t  PndRich::ProcessHits(FairVolume* vol)
{
  /** This method is called from the MC stepping */

  //Set parameters at entrance of volume. Reset ELoss.
  if ( gMC->IsTrackEntering() ) {
    fELoss  = 0.;
    fTime   = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPos);
    gMC->TrackMomentum(fMom);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Create PndRichPoint at exit of active volume
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) {
    fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
    fVolumeID = vol->getMCid();
    if (fELoss == 0. ) { return kFALSE; }
    AddHit(fTrackID, fVolumeID, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()),
           TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength,
           fELoss);

    // Increment number of PndRich det points in TParticle
    PndStack* stack = (PndStack*) gMC->GetStack();
    stack->AddPoint(kRICH);
  }

  return kTRUE;
}

void PndRich::EndOfEvent()
{

  fPndRichPointCollection->Clear();

}



void PndRich::Register()
{

  /** This will create a branch in the output tree called
      PndRichPoint, setting the last parameter to kFALSE means:
      this collection will not be written to the file, it will exist
      only during the simulation.
  */

  FairRootManager::Instance()->Register("PndRichPoint", "PndRich",
                                        fPndRichPointCollection, kTRUE);

}


TClonesArray* PndRich::GetCollection(Int_t iColl) const
{
  if (iColl == 0) { return fPndRichPointCollection; }
  else { return NULL; }
}

void PndRich::Reset()
{
  fPndRichPointCollection->Clear();
}

void PndRich::ConstructGeometry()
{
  /** If you are using the standard ASCII input for the geometry
      just copy this and use it for your detector, otherwise you can
      implement here you own way of constructing the geometry. */

  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  PndRichGeo*  Geo  = new PndRichGeo();
  Geo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(Geo);

  Bool_t rc = geoFace->readSet(Geo);
  if (rc) { Geo->create(geoLoad->getGeoBuilder()); }
  TList* volList = Geo->getListOfVolumes();

  // store geo parameter
  FairRun* fRun = FairRun::Instance();
  FairRuntimeDb* rtdb= FairRun::Instance()->GetRuntimeDb();
  PndRichGeoPar* par=(PndRichGeoPar*)(rtdb->getContainer("PndRichGeoPar"));
  TObjArray* fSensNodes = par->GetGeoSensitiveNodes();
  TObjArray* fPassNodes = par->GetGeoPassiveNodes();

  TListIter iter(volList);
  FairGeoNode* node   = NULL;
  FairGeoVolume* aVol=NULL;

  while( (node = (FairGeoNode*)iter.Next()) ) {
    aVol = dynamic_cast<FairGeoVolume*> ( node );
    if ( node->isSensitive()  ) {
      fSensNodes->AddLast( aVol );
    } else {
      fPassNodes->AddLast( aVol );
    }
  }
  par->setChanged();
  par->setInputVersion(fRun->GetRunId(),1);

  ProcessNodes ( volList );
}

PndRichPoint* PndRich::AddHit(Int_t trackID, Int_t detID,
                                      TVector3 pos, TVector3 mom,
                                      Double_t time, Double_t length,
                                      Double_t eLoss)
{
  TClonesArray& clref = *fPndRichPointCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndRichPoint(trackID, detID, pos, mom,
         time, length, eLoss);
}

ClassImp(PndRich)
