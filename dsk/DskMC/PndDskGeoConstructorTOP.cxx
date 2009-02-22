// ----------------------------------------------------------------------------
// -----              PndDskGeoConstructorTOP source file                 -----
// -----                 Created 21/03/08  by P. Koch                     -----
// ----------------------------------------------------------------------------

#include <iostream>
using std::endl;
using std::cout;

#include "TClonesArray.h"
#include "TParticle.h"
#include "TVirtualMC.h"

#include "FairVolume.h"

#include "PndDetectorList.h"
#include "PndStack.h"

#include "PndDsk.h"
#include "PndDskCerenkov.h"
#include "PndDskParticle.h"

// -----   Private method CheckIfSensitiveTOP   -------------------------------
Bool_t
PndDsk::CheckIfSensitiveTOP(std::string name)
{
  return kTRUE;
//   if (name.compare("radiator") == 0) { // radiator
//     return kTRUE;
//   }
//   TString volName(name);
//   if (volName.BeginsWith("mcp")) { // mcps
//     return kTRUE;
//   }
}
// ----------------------------------------------------------------------------



// -----   Private method ProcessHitsCerenkovTOP   ----------------------------
Bool_t
PndDsk::ProcessHitsCerenkovTOP(FairVolume* vol)
{
//   cout << "-I- PndDsk::ProcessHitsCerenkov(): got called." << endl;


  // If a track is entering the detector (first step inside, just emitted a particle, ..)
  // we will search it in our collection.
  // If it is not found, we will apply the quantumefficiency and kill it if necessary.
  // Else we will add it to the collection.


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

  // it is entering the glass
  if ((gMC->IsTrackEntering()) && ((vol->getName()).BeginsWith("radiator"))) {

    // we need its TrackID
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();

    // find it in our collection
    Bool_t searching = kTRUE;
    PndDskCerenkov* pCerenkov;
    TIter iter(fDskCerenkovCollection);
    while ( searching && (pCerenkov = (PndDskCerenkov*)iter.Next()) ) {
      if ( pCerenkov->GetTrackID() == fTrackID )
      searching = kFALSE;
    }

    // this track is not yet in our collection
    if (searching) {

      PndStack* stack = (PndStack*) gMC->GetStack();
      stack->AddPoint(kDSK);

      // gather information that is needed in any case
      gMC->TrackMomentum(tmpLVec);
      fMomentum   = tmpLVec.Vect() * 1.e9;      // in [eV]
      fEnergy     = tmpLVec.E() * 1.e9;         // in [eV], photon energy
      fWavelength = 1.239841874e3/fEnergy;      // hc = 4.13566733e-15[eVs] * 299792458e+09[nm/s]
      // apply Quantum-Efficiency (reduces tracks as they wont be detected anyway)
      // if equal to one, 100% will be detected, no need to kill it here
      if (1. != fPDE) {
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
          // 0.004:  qeValue /= 100, cause it returns percent;
          //                 *= 0.4, cause faktor of 40% given by MCP
        // else we assume that efficiency is given as 0 < fQE < 1
        } else {
          pdeValue = fPDE;
        }
        // apply quantum efficiency
        if (gRandom->Uniform() > pdeValue) {
          // the Cerenkov wont be detected. So no need to track it.
          gMC->StopTrack();
          return kTRUE;
        }
      }

      // this Cerenkov is a new one, which will be detected, so add it to collection
      // other data of CmbMCPoint. Position and Time is the point of first appearance
      fDetectorID = vol->getMCid();
      gMC->TrackPosition(tmpLVec);
      fPosition   = tmpLVec.Vect();             // in [cm]
      fTime       = gMC->TrackTime() * 1.e9;    // in [ns], global time
      // gather needed information about mother
      fMotherTrackID = gMC->GetStack()->GetCurrentTrack()->GetFirstMother();
      if (fMotherTrackID>-1) {
        fMotherPdgCode = 0;
        fMotherPdgName = "unknown";
///         TParticle* mother         = dynamic_cast<PndStack*>(gMC->GetStack())->GetParticle(fMotherTrackID);
///                    fMotherPdgCode = mother->GetPdgCode();
///                    fMotherPdgName = mother->GetName();
      } else {
        fMotherPdgCode = 0;
        fMotherPdgName = "unknown";
      }

      // after we collected all information that is available on its creation
      // we will add it to the collection
      AddCerenkov(
        // data of first appearance (FairMCPoint)
        fTrackID, fDetectorID, fPosition, fMomentum, fTime,
        // (PndDskCerenkov)
        fEnergy, fWavelength,
        // data of mother
        fMotherTrackID, fMotherPdgCode, fMotherPdgName);

    } else {

    // we found the cerenkov in the collection, so its not the first step, so the IsEntering
    // actually means a reflection, that will be counted (for further usage)

      pCerenkov->AddReflection();

    }
  }

  // the other case is, that a Cerenkov reaches our MCPs.
  // first we will search it in the collection (it should be there)
  if ( (vol->getName()).BeginsWith("mcp") ) {

    // we need its TrackID
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();

    // find it in our collection
    Bool_t searching = kTRUE;
    PndDskCerenkov* pCerenkov;
    TIter iter(fDskCerenkovCollection);
    while ( searching && (pCerenkov = (PndDskCerenkov*)iter.Next()) ) {
      if ( pCerenkov->GetTrackID() == fTrackID )
      searching = kFALSE;
    }

    // we detected something in the MCP that wasnt registered before. Drop a warning!
    if (searching) {
      cout << "-W-  PndDskDetector::ProcessHitsCerenkov: Cerenkov with trackID = " << fTrackID
           << " reached the MCP without being in the collection! Something is badly wrong here." << endl;
    // we found it
    } else {
/**
      // what kind of detector got hit?
      fDetType = 1+(vol->getName()).Remove(0,4).Atoi(); // gives the X ind dsk4X

      // the detector number that got hit, starting with 1 from top left, counting clockwise watching in z-direction
      Int_t tmpCopy1, tmpCopy2;
      Int_t tmpVol1,  tmpVol2;
      tmpVol2  = gMC->CurrentVolOffID(2, tmpCopy2); // detector array
      tmpVol1  = gMC->CurrentVolOffID(1, tmpCopy1); // detector

      fDetNumber =   (tmpCopy2-1)*fDetectorsPerArray // all detectors on the edges bevor this one
                   + (tmpCopy1-1)*fDetectorTypes     // all group of detectors with one of ech type
                   + fDetType;                       // + the number of the type
**/

      // detector type, encoded in name: mcpX
      // i could need a faster code ...
      TString tmpName = vol->getName();
      if (tmpName.Contains("front")) {
        fDetType = 1+tmpName.Remove(0,8).Atoi(); // gives the X ind mcpfrontX
      } else if (tmpName.Contains("back")) {
        fDetType = 1+tmpName.Remove(0,7).Atoi(); // gives the X ind mcpbackX
      } else {
        fDetType = 1+tmpName.Remove(0,3).Atoi(); // gives the X ind mcpX
      }

      // detector number. They are counted as the createRootGeoFile script created them.
      // we need the copy number of the mother of the mcp, thats the copy number of the detector.
      gMC->CurrentVolOffID(1, fDetNumber); // detector
  
      // other data
      fDetTime = gMC->TrackTime() * 1.0e09;  // in [ns]

      // angle to the z-axis
      gMC->TrackMomentum(tmpLVec);
      fDetMomentum = tmpLVec.Vect() * 1.e9; // in [eV]

      // calculations for resolution study
      // this angle is between 0deg and 180deg!
      TVector3 tmpMom; pCerenkov->Momentum(tmpMom);
      fPrimaryAngleToCerenkov = TVector3(fPrimaryHitMomentum.Px(),fPrimaryHitMomentum.Py(),0.)
                                .Angle(TVector3(tmpMom.Px(),tmpMom.Py(),0.));

      // now store our values
      pCerenkov->SetFinalValues(fDetNumber, fDetType, fDetTime, fDetMomentum,
                                fPrimaryHitAngle, fPrimaryAngleToCerenkov);

      if (fDebugLevel > 79) {
        printf("C hit det %i of type %i\n", fDetNumber, fDetType);
      }

      // and finally: stopp the track as it got absorbed in mcp
      gMC->StopTrack();
    }
  }

  // the rest is not important: if the Cerenkov leaves the disk, if it gets
  // absorbed somewhere or whatever.

  return kTRUE;


}
// ----------------------------------------------------------------------------





// -----   Private method ProcessHitsParticleTOP   ----------------------------
Bool_t
PndDsk::ProcessHitsParticleTOP(FairVolume* vol)
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
///       fPdgName    = dynamic_cast<PndStack*>(gMC->GetStack())->GetCurrentTrack()->GetName();
      fPdgName    = "unknown";
      // gather needed information about mother
      fMotherTrackID = gMC->GetStack()->GetCurrentTrack()->GetFirstMother();
      if (fMotherTrackID>-1) {
///         TParticle* mother         = dynamic_cast<PndStack*>(gMC->GetStack())->GetParticle(fMotherTrackID);
///                    fMotherPdgCode = mother->GetPdgCode();
///                    fMotherPdgName = mother->GetName();
        fMotherPdgCode = 0;
        fMotherPdgName = "unknown";
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
  // criteria is taken from mvd
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

