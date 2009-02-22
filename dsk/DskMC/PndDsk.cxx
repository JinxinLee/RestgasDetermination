// ----------------------------------------------------------------------------
// -----                        PndDsk source file                        -----
// -----                   Created 23/10/07 by P. Koch                    -----
// ----------------------------------------------------------------------------

#include <iostream>
using std::endl;
using std::cout;

#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TVirtualMC.h"

#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "PndDsk.h"
#include "PndDskCerenkov.h"
#include "PndDskParticle.h"



// -----   Default constructor   ----------------------------------------------
PndDsk::PndDsk()
  : fStoreCerenkovs(kTRUE),
    fStoreParticles(kTRUE),
    fPDE(1.),
    fDetectorTypes(2),
    fDetectorsPerArray(120),
    fUsingMirrors(kTRUE),
    fGeoVersion("TOP"),
    fDebugLevel(0)
{
  fDskCerenkovCollection = new TClonesArray("PndDskCerenkov");
  fDskParticleCollection = new TClonesArray("PndDskParticle");
}
// ----------------------------------------------------------------------------



// -----   Standard constructor   ---------------------------------------------
PndDsk::PndDsk(const char* name, Bool_t active)
  : FairDetector(name, active),
    fStoreCerenkovs(kTRUE),
    fStoreParticles(kTRUE),
    fPDE(1.),
    fDetectorTypes(2),
    fDetectorsPerArray(120),
    fUsingMirrors(kTRUE),
    fGeoVersion("TOP"),
    fDebugLevel(0)
{
  fDskCerenkovCollection = new TClonesArray("PndDskCerenkov");
  fDskParticleCollection = new TClonesArray("PndDskParticle");
}
// ----------------------------------------------------------------------------



// -----   Destructor   -------------------------------------------------------
PndDsk::~PndDsk()
{
  if (0 != fDskCerenkovCollection) {
    fDskCerenkovCollection->Delete();
    delete fDskCerenkovCollection;
  }
  if (0 != fDskParticleCollection) {
    fDskParticleCollection->Delete();
    delete fDskParticleCollection;
  }
}
// ----------------------------------------------------------------------------



// -----   Public method Intialize   ------------------------------------------
void
PndDsk::Initialize()
{
  FairDetector::Initialize();
  FairRun       *sim  = FairRun::Instance();
  FairRuntimeDb *rtdb = sim->GetRuntimeDb();
}
// ----------------------------------------------------------------------------



// -----   Public method ProcessHits  -----------------------------------------
Bool_t
PndDsk::ProcessHits(FairVolume* vol)
{
  fPdgCode = gMC->TrackPid();
  if (fGeoVersion == "TOP") {
    if (fPdgCode == 50000050 ) {
      return ProcessHitsCerenkovTOP(vol);
    } else {
      return ProcessHitsParticleTOP(vol);
    }
  } else if (fGeoVersion == "LG") {
    if (fPdgCode == 50000050 ) {
      return ProcessHitsCerenkovLG(vol);
    } else {
      return ProcessHitsParticleLG(vol);
    }
  }
}
// ----------------------------------------------------------------------------



// -----   Public method EndOfEvent   -----------------------------------------
void
PndDsk::EndOfEvent()
{
  Reset();
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void
PndDsk::Register()
{
  FairRootManager::Instance()->Register("DskCerenkov","Dsk", fDskCerenkovCollection, fStoreCerenkovs);
  FairRootManager::Instance()->Register("DskParticle","Dsk", fDskParticleCollection, fStoreParticles);
}
// ----------------------------------------------------------------------------



// -----   Public method SetDetectors   ---------------------------------------
void
PndDsk::SetDetectors(Int_t detectorTypes, Int_t detectorsPerArray, Bool_t usingMirrors)
{
  fDetectorTypes     = detectorTypes;
  fDetectorsPerArray = detectorsPerArray;
  fUsingMirrors      = usingMirrors;
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray*
PndDsk::GetCollection(Int_t iColl) const
{
  if (iColl == 0) return fDskCerenkovCollection;
  if (iColl == 1) return fDskParticleCollection;

  return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void
PndDsk::Print() const
{
  cout << "-I- PndDsk::Print() was called, but is not yet implemented." << endl;
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void
PndDsk::Reset()
{
  fDskCerenkovCollection->Clear();
  fDskParticleCollection->Clear();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void
PndDsk::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
  cout << "-I- PndDsk::CopyClones() was called, but is not yet implemented." << endl;
}
// ----------------------------------------------------------------------------



// -----   Public method ConstructGeometry   ----------------------------------
void
PndDsk::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();
  if (fileName.EndsWith(".root")) {
    ConstructRootGeometry();
  } else {
    cout << "-E- PndDsk::ConstructGeometry(): Geometry format not supported." << endl;
  }
}
// ----------------------------------------------------------------------------



// -----   Public method CheckIfSensitive   -----------------------------------
Bool_t
PndDsk::CheckIfSensitive(std::string name)
{
  if (fGeoVersion == "TOP") {
    return CheckIfSensitiveTOP(name);
  }
  if (fGeoVersion == "LG") {
    return CheckIfSensitiveLG(name);
  }
}
// ----------------------------------------------------------------------------



// -----   Public method AddCerenkov   ----------------------------------------
PndDskCerenkov*
PndDsk::AddCerenkov(Int_t trackID, Int_t detectorID, TVector3 position,
        TVector3 momentum, Double_t time, Double_t energy, Double_t wavelength,
        Int_t motherTrackID, Int_t motherPdgCode, TString motherPdgName)
{
  TClonesArray& clRef = *fDskCerenkovCollection;
  Int_t         size  = clRef.GetEntriesFast();

  return new(clRef[size]) PndDskCerenkov(trackID, detectorID,
          position, momentum, time, energy, wavelength,
          motherTrackID, motherPdgCode, motherPdgName);
}
// ----------------------------------------------------------------------------



// -----   Public method AddParticle   ----------------------------------------
PndDskParticle*
PndDsk::AddParticle(Int_t trackID, Int_t detectorID,
        TVector3 position, TVector3 momentum, Double_t time,
        Int_t pdgCode, TString pdgName, Double_t energy,
        Int_t motherTrackID, Int_t motherPdgCode, TString motherPdgName)
{
  TClonesArray& clRef = *fDskParticleCollection;
  Int_t         size  = clRef.GetEntriesFast();

  return new(clRef[size]) PndDskParticle(trackID, detectorID,
          position, momentum, time, pdgCode, pdgName, energy,
          motherTrackID, motherPdgCode, motherPdgName);
}
// ----------------------------------------------------------------------------


ClassImp(PndDsk)
