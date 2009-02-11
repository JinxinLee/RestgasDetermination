////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  PndDsk
//
//  Interface to virtual monte carlo
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// #include "FairRun.h"
#include "FairMCApplication.h"
#include "FairVolumeList.h"
#include "TGeoMCGeometry.h"
#include "TGeoMedium.h"

#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "PndGeoDsk.h"
#include "FairGeoRootBuilder.h"
#include "FairRuntimeDb.h"
#include "TObjArray.h"
#include "FairGeoVolume.h"
#include "FairGeoNode.h"
#include "FairRun.h"




#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoPhysicalNode.h"
#include "FairRun.h"
#include "PndGeoDskPar.h"
#include "FairRuntimeDb.h"

#include "TLorentzVector.h"
#include "TVirtualMC.h"
#include "TParticle.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairVolume.h"
#include "PndStack.h"
#include "PndDsk.h"
#include "PndDskCerenkov.h"
#include "PndGeoDsk.h"
#include <iostream>
using std::cout;
using std::endl;

ClassImp(PndDsk);


// default constructor
// -------------------------------------------------------------------------------------------------
PndDsk::PndDsk()
{
  cout << "---III---: PndDsk::PndDsk(void) was called!";
}


// standard constructor
// -------------------------------------------------------------------------------------------------
PndDsk::PndDsk(const char *name, Bool_t active)
  : FairDetector(name, active)
{
  // default mirror setup
  fMirrorTypes    = 3;
  fMirrorsPerEdge = 120;
  // create the Collections of PndDskPoints
  // this are all points created in the MC
  fDskCerenkovCollection = new TClonesArray("PndDskCerenkov");

  fPrimaryIsInside = false;
}


// default destructor
// -------------------------------------------------------------------------------------------------
PndDsk::~PndDsk()
{
  // destructor, needs to clean the PointCollections

  if ( 0 != fDskCerenkovCollection) {
    fDskCerenkovCollection->Delete();
    delete fDskCerenkovCollection;
  }
}


// Adds Cerenkov to collection
// -------------------------------------------------------------------------------------------------
PndDskCerenkov*
PndDsk::AddCerenkov(Int_t trackID, Int_t volID, TVector3 pos, TVector3 mom,
              Double_t tof, Double_t length, Double_t eLoss,
              Double_t energy, Int_t motherPdgCode,
              TVector3 motherHitMomentum)
{
  // get size of array
  TClonesArray& points = *fDskCerenkovCollection;
  Int_t         size   = points.GetEntriesFast();

  // create a new hit after the last one present in the collection
  return new(points[size]) PndDskCerenkov(trackID, volID, pos, mom, tof, length, eLoss,
                                          energy, motherPdgCode, motherHitMomentum);
}


// Construct the detector using the definitions in the geometry file
// -------------------------------------------------------------------------------------------------
void
PndDsk::ConstructGeometry()
{
  cout << "[--- III --- ] PndDsk::ConctructGeometry() was called." << endl;

  // make a new interface to geometry file
  PndGeoDsk *dskGeo = new PndGeoDsk();
  dskGeo->setGeomFile(GetGeometryFileName());

  // geo interface is the class that reads our geometry file
  FairGeoInterface *geoFace = FairGeoLoader::Instance()->getGeoInterface();
  // geo builder is the class that constructs the geometry
  FairGeoBuilder *geoBuild = FairGeoLoader::Instance()->getGeoBuilder();

  // Read in geometry file
  geoFace->addGeoModule(dskGeo);
  Bool_t rc = geoFace->readSet(dskGeo);
  // and create the geometry
  if (rc)
    dskGeo->create(geoBuild);
  else
    cout << "[ !! ] PndDsk::ConstructGeometry: geometry in file "
         << GetGeometryFileName()
         << " could not be read!" << endl;

  TList *volList = dskGeo->getListOfVolumes();

  // Set active/inactive
  ProcessNodes(volList);
}


// Construct optical surfaces of geant4
// -------------------------------------------------------------------------------------------------
void
PndDsk::ConstructOpGeometry()
{
  cout << "[ --- III --- ] PndDsk::ConctructOpGeometry() was called." << endl;

  // glass <-> air
  // the double is the "polish parameter in glisur model"
  // kUnified / kGlisur
  // kDielectric_dielectric
  gMC->DefineOpSurface("GlassSurface",
                        kUnified, kDielectric_dielectric, kPolished, 0.);

  gMC->SetBorderSurface("GlassSurface",
                        "dsk01", 0, "dsk02", 0, "GlassSurface");


//   const Int_t num = 2;
//   Double_t ephoton[num] = { 2.038e-09, 4.144e-09 };

  // OpticalWaterSurface
//   Double_t refractiveIndex[num] = { 1.35, 1.40 };
//   gMC->SetMaterialProperty("GlassSurface",
//                            "RINDEX", num, ephoton, refractiveIndex);


}


// Things that need to be done at the end of an event
// -----------------------------------------------------------------------------
void
PndDsk::EndOfEvent()
{
  Reset();
}


// Returns a hit collection
// -----------------------------------------------------------------------------
TClonesArray*
PndDsk::GetCollection(Int_t iColl) const
{
  // the assignment for iColl is just defined below:
  if (iColl == 0)
    return fDskCerenkovCollection;

  // as default, return null
  return NULL;
}

// React on a hit in an activ volume
// -----------------------------------------------------------------------------
Bool_t
PndDsk::ProcessHits(FairVolume *vol)
{
  // Defines the action to be taken when a cherenkov produced a hit
  // but there is nothing to do here as long as we dont get reflections

//   cout << "--- III --- OndDsk::ProcessHits() was called" << endl;

//   fPdgCode    = gMC->TrackPid();

//   if (fPdgCode == 50000050) {
//     cout << gMC->GetStack()->GetCurrentTrackNumber() <<  " " << vol->getName() << " "
//          << gMC->IsTrackEntering() << gMC->IsTrackExiting() << endl;
//   }

//   if (((vol->getName()).EndsWith("04")) && (fPdgCode == 50000050)) {
//     cout << "Hit in Mirror: " << gMC->GetStack()->GetCurrentTrackNumber() << " hit: " << 1.239842447e-6/fEnergy << "nm " << gMC->IsTrackEntering() << gMC->IsTrackExiting() << endl;
//   }



  return kTRUE;
}


// Registers the hit collection to the ROOT manager
// -----------------------------------------------------------------------------
void
PndDsk::Register()
{
  // Registers the hit collection to the ROOT manager

  // All entries in fDskPointCollection will be added to ROOTFILE->cbmsim->DskPoint
  FairRootManager::Instance()->Register("DskCerenkov", "Dsk", fDskCerenkovCollection, kTRUE);
}


// Reset the Collections
// -----------------------------------------------------------------------------
void
PndDsk::Reset()
{
  // Clear the hit collections

  fDskCerenkovCollection->Clear();
}


// Define mirror setup to use
// -----------------------------------------------------------------------------
void
PndDsk::SetMirrors(Int_t mirrorTypes, Int_t mirrorsPerEdge )
{
  // for diffrent wavelength intervals
  fMirrorTypes    = mirrorTypes;
  // total number of mirrors per edge
  fMirrorsPerEdge = mirrorsPerEdge;
}
