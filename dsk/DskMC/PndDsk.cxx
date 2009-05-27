// ----------------------------------------------------------------------------
// -----                        PndDsk source file                        -----
// -----                   Created 23/10/07 by P. Koch                    -----
// ----------------------------------------------------------------------------

#include <iostream>
using std::endl;
using std::cout;

#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVector3.h"
#include "TVirtualMC.h"

#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairVolume.h"

#include "PndDetectorList.h"
#include "PndStack.h"

#include "PndDsk.h"
#include "PndDskCerenkov.h"
#include "PndDskParticle.h"
#include "PndDskTrackPoint.h"




// -----   Default constructor   ----------------------------------------------
PndDsk::PndDsk()
  : fStoreCerenkovs(kTRUE),
    fStoreParticles(kTRUE),
    fStoreTrackPoints(kFALSE),
    fCalcPWay(kFALSE),
    fMeasureTotalRefAngle(kFALSE),
    fPDE(1.),
    fDebugLevel(0)
{
  fDskCerenkovCollection = new TClonesArray("PndDskCerenkov");
  fDskParticleCollection = new TClonesArray("PndDskParticle");
  fDskTrackPointCollection = new TClonesArray("PndDskTrackPoint");
}
// ----------------------------------------------------------------------------



// -----   Standard constructor   ---------------------------------------------
PndDsk::PndDsk(const char* name, Bool_t active)
  : FairDetector(name, active),
    fStoreCerenkovs(kTRUE),
    fStoreParticles(kTRUE),
    fStoreTrackPoints(kFALSE),
    fCalcPWay(kFALSE),
    fMeasureTotalRefAngle(kFALSE),
    fPDE(1.),
    fDebugLevel(0)
{
  fDskCerenkovCollection = new TClonesArray("PndDskCerenkov");
  fDskParticleCollection = new TClonesArray("PndDskParticle");
  fDskTrackPointCollection = new TClonesArray("PndDskTrackPoint");
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
  if (0 != fDskTrackPointCollection) {
    fDskTrackPointCollection->Delete();
    delete fDskTrackPointCollection;
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
  if (fPdgCode == 50000050 ) {
    return ProcessHitsCerenkov(vol);
  } else {
    return ProcessHitsParticle(vol);
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
  FairRootManager::Instance()->Register("DskCerenkov",   "Dsk", fDskCerenkovCollection,   fStoreCerenkovs);
  FairRootManager::Instance()->Register("DskParticle",   "Dsk", fDskParticleCollection,   fStoreParticles);
  FairRootManager::Instance()->Register("DskTrackPoints","Dsk", fDskTrackPointCollection, fStoreTrackPoints);
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray*
PndDsk::GetCollection(Int_t iColl) const
{
  if (iColl == 0) return fDskCerenkovCollection;
  if (iColl == 1) return fDskParticleCollection;
  if (iColl == 2) return fDskTrackPointCollection;
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
  fDskTrackPointCollection->Clear();
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
//   } else if (fileName.EndsWith(".geo")) {
//     ConstructASCIIGeometry();
  } else {
    cout << "-E- PndDsk::ConstructGeometry(): Geometry format in file " << fileName.Data() << " not supported." << endl;
  }
}
// ----------------------------------------------------------------------------



// -----   Public method CheckIfSensitive   -----------------------------------
Bool_t
PndDsk::CheckIfSensitive(std::string name)
{
  return kTRUE;
//   if (name.compare("radiator") == 0) { // "radiator"
//     return kTRUE;
//   }
//   if (name.find("mcp") == 0) { // "mcp*"
//     return kTRUE;
//   }
//   if (name.find("mirror") == 0) { // "mirror*"
//     return kTRUE;
//   }
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



// -----   Public method AddTrackPoint   ----------------------------------------
PndDskTrackPoint*
PndDsk::AddTrackPoint(Int_t trackID, Int_t detectorID, TVector3 position, TVector3 momentum,
    Double_t time, Double_t length, Double_t eLoss)
{
  TClonesArray& clRef = *fDskTrackPointCollection;
  Int_t         size  = clRef.GetEntriesFast();

  return new(clRef[size]) PndDskTrackPoint(trackID, detectorID,
          position, momentum, time, length, eLoss);
}
// ----------------------------------------------------------------------------



// -----   Private method ProcessHitsCerenkov   -------------------------------
Bool_t
PndDsk::ProcessHitsCerenkov(FairVolume* vol)
{

/// Debug Tracks stepps
// fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
// cout << "New Step @ " << gMC->TrackTime() * 1.e9 << endl;
// if (gMC->IsNewTrack()) {
//   cout << "Track " << fTrackID << " is new,      called from volume: " << vol->GetName() << endl;
// }
// if (gMC->IsTrackOut()) {
//   cout << "Track " << fTrackID << " is out,      called from volume: " << vol->GetName() << endl;
// }
// if (gMC->IsTrackDisappeared()) {
//   cout << "Track " << fTrackID << " disappeared, called from volume: " << vol->GetName() << endl;
// }
// if (gMC->IsTrackStop()) {
//   cout << "Track " << fTrackID << " stopped,     called from volume: " << vol->GetName() << endl;
// }
// if (! gMC->IsTrackInside()) {
//   cout << "Track " << fTrackID << " on boundary, called from volume: " << vol->GetName() << endl;
// }
// if (gMC->IsTrackInside()) {
//   cout << "Track " << fTrackID << " is inside,   called from volume: " << vol->GetName() << endl;
// }
// if (gMC->IsTrackExiting()) {
//   cout << "Track " << fTrackID << " is exiting,  called from volume: " << vol->GetName() << endl;
// }
// if (gMC->IsTrackEntering()) {
//   cout << "Track " << fTrackID << " is entering, called from volume: " << vol->GetName() << endl;
// }


  // gather information
  fTrackID = gMC->GetStack()->GetCurrentTrackNumber();

  // search the photon in our collection
  Bool_t searching = kTRUE;
  PndDskCerenkov* pCerenkov = 0;
  TIter iter(fDskCerenkovCollection);
  while ( searching && (pCerenkov = (PndDskCerenkov*)iter.Next()) ) {
    if ( pCerenkov->GetTrackID() == fTrackID )
    searching = kFALSE;
  }

  // if fStoreTrackPoints is set gather all necessary data now
  if (fStoreTrackPoints) {
    fDetectorID = vol->getMCid();
    gMC->TrackPosition(tmpLVec);
    fPosition   = tmpLVec.Vect();             // in [cm]
    fTime       = gMC->TrackTime() * 1.e9;    // in [ns], global time
    gMC->TrackMomentum(tmpLVec);
    fMomentum   = tmpLVec.Vect() * 1.e9;      // in [eV]
    fLength     = gMC->TrackLength();         // in [cm]
    fELoss      = gMC->Edep();                // in [GeV]
    AddTrackPoint(fTrackID, fDetectorID, fPosition, fMomentum, fTime, fLength, fELoss);
  }

  // if it is a new photon
  if (searching) {

    // gather informations, if not already done
    if (!fStoreTrackPoints) {
      fDetectorID = vol->getMCid();
      gMC->TrackPosition(tmpLVec);
      fPosition   = tmpLVec.Vect();             // in [cm]
      fTime       = gMC->TrackTime() * 1.e9;    // in [ns], global time
      gMC->TrackMomentum(tmpLVec);
      fMomentum   = tmpLVec.Vect() * 1.e9;      // in [eV]
    }
    fEnergy     = tmpLVec.E() * 1.e9;           // in [eV], photon energy
    fWavelength = 1.239841874e3/fEnergy;        // hc = 4.13566733e-15[eVs] * 299792458e+09[nm/s]

    fMotherTrackID = gMC->GetStack()->GetCurrentTrack()->GetFirstMother();
    if (fMotherTrackID>-1) {
      TParticle* mother         = ((PndStack*)(gMC->GetStack()))->GetParticle(fMotherTrackID);
                 fMotherPdgCode = mother->GetPdgCode();
                 fMotherPdgName = mother->GetName();
    } else {
      fMotherPdgCode = 0;
      fMotherPdgName = "unknown";
    }

    PndStack* stack = (PndStack*)gMC->GetStack();
    stack->AddPoint(kDSK);

    // after we collected all information that is available on its creation
    // we will add it to the collection
    AddCerenkov(
      // data of first appearance (FairMCPoint)
      fTrackID, fDetectorID, fPosition, fMomentum, fTime,
      // (PndDskCerenkov)
      fEnergy, fWavelength,
      // data of mother
      fMotherTrackID, fMotherPdgCode, fMotherPdgName
    );

    // if the cerenkov doesnt meet our criteria, add it to collection (already done)
    // but stopp the track imediately. no need to track it further.

    if (DoNotTrackCerenkov()) {
      gMC->StopTrack();
      return kTRUE;
    }


  // is Cerenkov is in our collection
  } else {

    // gather informations, if not already done
    if (!fStoreTrackPoints) {
      gMC->TrackPosition(tmpLVec);
      fPosition = tmpLVec.Vect();             // in [cm]
    }

    // add projected way
    if (fCalcPWay) {
      pCerenkov->AddPWay(fPosition);
    }
    // and numer of reflections if IsTrackEntering() in the radiator
    /// @todo needs to take glue, mirrors and diffrent media into account!
    if ((gMC->IsTrackEntering()) && ((vol->getName()).BeginsWith("radiator"))) {
      pCerenkov->AddReflection();

      // get simulated total reflection angle
      if (fMeasureTotalRefAngle) {
        if (pCerenkov->GetNofReflections() == 5) {
          pCerenkov->Set5RefPosition(fPosition);
          /// calculate total reflection angle
          gMC->TrackMomentum(tmpLVec);
          fMomentum = tmpLVec.Vect();
          pCerenkov->SetTotalRefAngle(TMath::PiOver2() - TVector3(0.,0.,-1.).Angle(fMomentum));
        }
      }
    }


    // the dichroic mirrors need to be simulated inhere as it cant be done
    // with material definitions.
    // unfortunately there is no way to tell a Cerenkov to be reflected or not.
    // At the moment, ever Cerenkov is reflected (ideal mirror) if its not stopped
    // in here (already in the mirror, not the mcp ...).
    // So the following code will give a simple MC, but wont work for a more
    // more complicated setup behind the mirrors!


// this is what it should be:
//     // if cerenkov reaches mcp: detect it
//     if ((vol->getName()).BeginsWith("mcp")) {
// and this is what is is at the moment:
    if ((vol->getName()).BeginsWith("mirror")) {

//       sscanf((vol->getName()).Data(),"mcp%hu",&fDetType);
      sscanf((vol->getName()).Data(),"mirror%hu",&fDetType);

      
      // decide if photon is detected
      if (DichroicMirrorTransmitted(pCerenkov->GetWavelength(),fDetType)) {
      
        // detector number. They are counted as the createRootGeoFile script created them.
        // we need the copy number of the mother of the mcp, thats the copy number of the detector.
        gMC->CurrentVolOffID(1, fDetNumber); // detector

        // other data
        /// @todo data probably already collected! needs improvement
        fDetTime = gMC->TrackTime() * 1.0e09;  // in [ns]
        gMC->TrackMomentum(tmpLVec);
        fDetMomentum = tmpLVec.Vect() * 1.e9; // in [eV]
        // calculations for resolution study
        // this angle is between 0deg and 180deg!
        TVector3 tmpMom; pCerenkov->Momentum(tmpMom);
        fPrimaryAngleToCerenkov = TVector3(fPrimaryHitMomentum.Px(),fPrimaryHitMomentum.Py(),0.)
                                  .Angle(TVector3(tmpMom.Px(),tmpMom.Py(),0.));
        // and keep the track length
        fLength = gMC->TrackLength();  // [cm]
        // now store our values
        pCerenkov->SetFinalValues(fDetNumber, fDetType, fDetTime, fDetMomentum, fLength,
                                  fPrimaryHitAngle, fPrimaryAngleToCerenkov);
        // and finally: stopp the track as it got absorbed in mcp
        gMC->StopTrack();
      }
    }
  }

  return kTRUE;
}
// ----------------------------------------------------------------------------


// -----   Private method DichroicMirrorTransmitted   -------------------------
Bool_t
PndDsk::DichroicMirrorTransmitted(Double_t wl, Int_t det)
{
  // this is a VERY simple implementation, but better a lot better than 
  // using media definitions
  if (  ( (det==0) && ((wl>=400.) && (wl<=500.)) )
      ||( (det==1) && ((wl> 500.) && (wl<=700.)) )
     )
  {
    return kTRUE;
  } else {
    return kFALSE;
  }
}
// ----------------------------------------------------------------------------


// -----   Private method ProcessHitsParticle   -------------------------------
Bool_t
PndDsk::ProcessHitsParticle(FairVolume* vol)
{

  // it is entering any sensitive volume
  if ( gMC->IsTrackEntering() ) {

    // we need its TrackID
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();

    // find it in our collection
    Bool_t searching = true;
    TIter iter(fDskParticleCollection);
    PndDskParticle *pParticle;
    while ( searching && (pParticle = (PndDskParticle *)iter.Next()) ) {
      if ( pParticle->GetTrackID() == fTrackID )
      searching = false;
    }

    // this track is not yet in out collection
    if (searching) {

      PndStack* stack = (PndStack*) gMC->GetStack();
      stack->AddPoint(kDSK);

      // this particle is a new one, so store its values, as we want to know which particle
      // created the Cerenkovs.
      // other data of CmbMCPoint. Position and Time is the point of first appearance
      fDetectorID = vol->getMCid();
      gMC->TrackPosition(tmpLVec);
      fPosition   = tmpLVec.Vect();             // in [cm]
      fTime       = gMC->TrackTime() * 1.e9;    // in [ns], global time
      gMC->TrackMomentum(tmpLVec);
      fMomentum   = tmpLVec.Vect();             // in [GeV]
      fEnergy     = tmpLVec.E();                // in [GeV], particle energy
      // PDG code was already stored, but we want the name too
      fPdgName    = ((PndStack*)(gMC->GetStack()))->GetCurrentTrack()->GetName();
      // gather needed information about mother
      fMotherTrackID = gMC->GetStack()->GetCurrentTrack()->GetFirstMother();
      if (fMotherTrackID>-1) {
        TParticle* mother         = ((PndStack*)(gMC->GetStack()))->GetParticle(fMotherTrackID);
                   fMotherPdgCode = mother->GetPdgCode();
                   fMotherPdgName = mother->GetName();
      } else {
        fMotherPdgCode = 0;
        fMotherPdgName = "unknown";
      }

      // if this is the primary particle (TrackID == 0), save its momentum for further use
      // these values need to be initialised in constructor, cause we need to know if the primary
      // ever reaches the disk:
      // if fPrimaryHitAngle is 0, the primary never reached the disk, as a value of 0 is impossible
      // for the disk (would be along the beampipe)
      // As the Cerenkovs will use the fPrimaryHitMomentum the primary needs to call ProcessHits
      // before the first Cerenkov does, but i assume thats this will always be the case.
      if (fTrackID == 0) {
        fPrimaryHitMomentum = fMomentum;
        // angle between momentum and z-axis
        fPrimaryHitAngle = fMomentum.Angle(TVector3(0.,0.,1.));
      }

      // now add particle
      AddParticle(
            // data of first appearance (FairMCPoint)
            fTrackID, fDetectorID, fPosition, fMomentum, fTime,
            // (PndDskCerenkov)
            fPdgCode, fPdgName, fEnergy,
            // data of mother
            fMotherTrackID, fMotherPdgCode, fMotherPdgName);
    }
  }

  // The other case we are interested in is when the particles leaves the disk or is stopped in some way
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) {

    // we need its TrackID
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();

    // find it in our collection
    Bool_t searching = true;
    TIter iter(fDskParticleCollection);
    PndDskParticle *pParticle;
    while ( searching && (pParticle = (PndDskParticle *)iter.Next()) ) {
      if ( pParticle->GetTrackID() == fTrackID )
      searching = false;
    }

    if (searching) {
      cout << "-W-  PndDskDetector::ProcessHitsParticle: Particle with trackID = " << fTrackID
           << "disappeared without being in the collection! Something is badly wrong here." << endl;
    // we found it
    } else {
      gMC->TrackPosition(tmpLVec);
      fEndPosition  = tmpLVec.Vect();           // in [cm]
      fEndTime      = tmpLVec.T() * 1.e9;       // in [ns]
      gMC->TrackMomentum(tmpLVec);
      fEndMomentum  = tmpLVec.Vect();           // in [GeV]
      fEndEnergy    = tmpLVec.E();              // in [GeV]
      pParticle->SetFinalValues(fEndPosition, fEndMomentum, fEndTime, fEndEnergy);
    }
  }

  return kTRUE;

}
// ----------------------------------------------------------------------------


// -----   Private method TrackCerenkov   -----------------------------------
Bool_t
PndDsk::DoNotTrackCerenkov()
{
  //available data:
  // fDetectorID
  // fPosition         in [cm]
  // fTime             in [ns], global time
  // fMomentum         in [eV]
  // fEnergy           in [eV]
  // fWavelength       in [nm]

  // in our interval?
  if ((fWavelength < 400.) || (fWavelength > 700.)) {
    return kTRUE;
  }

  // detect every Cerenkov
  if (1. == fPDE) {
    return kFALSE;
  }

  Double_t pdeValue;

  // if equal to 2, we will use formula: fit for BINP_MCP, done by Avetik
  if (2. == fPDE) {
    pdeValue = (
    fWavelength * (
    fWavelength * (
    fWavelength * (
    fWavelength * (
    fWavelength * (
    fWavelength * (
        0.24980E-13
    ) - 0.10697E-09
    ) + 0.18451E-06
    ) - 0.16338E-03
    ) + 0.77842E-01
    ) - 18.806
    ) + 1806.4 ) * 0.004;
    // 0.004:  pdeValue /= 100, cause it returns percent;
    //                  *= 0.4, cause faktor of 40% given by MCP
  }

  // apply quantum efficiency
  if (gRandom->Uniform() > pdeValue) {
    // the Cerenkov wont be detected. So no need to track it.
    return kTRUE;
  }

  // in general we detect
  return kFALSE;
}
// ----------------------------------------------------------------------------


ClassImp(PndDsk)
