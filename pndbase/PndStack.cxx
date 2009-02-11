// -------------------------------------------------------------------------
// -----                       PndStack source file                    -----
// -----             Created 10/08/04  by D. Bertini / V. Friese       -----
// -------------------------------------------------------------------------

#include "PndStack.h"

#include "FairDetector.h"
#include "FairMCPoint.h"
#include "PndMCTrack.h"
#include "FairRootManager.h"

#include "TError.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TRefArray.h"

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndStack::PndStack() {
  fParticles = new TClonesArray("TParticle", 100);
  fTracks    = new TClonesArray("PndMCTrack", 100);
  fCurrentTrack = -1;
  fNPrimaries = fNParticles = fNTracks = 0;
  fIndex = 0;
  fStoreSecondaries = kTRUE;
  fMinPoints        = 1;
  fEnergyCut        = 0.;
  fStoreMothers     = kTRUE;
}

// -------------------------------------------------------------------------
// -----   Constructor with estimated array dimension   --------------------
PndStack::PndStack(Int_t size) {
  fParticles = new TClonesArray("TParticle", size);
  fTracks    = new TClonesArray("PndMCTrack", size);
  fCurrentTrack = -1;
  fNPrimaries = fNParticles = fNTracks = 0;
  fIndex = 0;
  fStoreSecondaries = kTRUE;
  fMinPoints        = 1;
  fEnergyCut        = 0.;
  fStoreMothers     = kTRUE;
}
// -------------------------------------------------------------------------

  

// -----   Destructor   ----------------------------------------------------
PndStack::~PndStack() {
  if (fParticles) {
    fParticles->Delete();
    delete fParticles;
  }
  if (fTracks) {
    fTracks->Delete();
    delete fTracks;
  }
}
// -------------------------------------------------------------------------

  

// -----   Virtual method PushTrack   --------------------------------------
void PndStack::PushTrack(Int_t toBeDone, Int_t parentID, Int_t pdgCode,
			 Double_t px, Double_t py, Double_t pz,
			 Double_t e, Double_t vx, Double_t vy, Double_t vz, 
			 Double_t time, Double_t polx, Double_t poly,
			 Double_t polz, TMCProcess proc, Int_t& ntr, 
			 Double_t weight, Int_t is) {

  // --> Get Stack
  TClonesArray& partArray = *fParticles;

  // --> Create new TParticle and add it to the TParticle array
  Int_t trackID = fNParticles;
  Int_t nPoints = 0;
  Int_t daughter1ID = -1;
  Int_t daughter2ID = -1;
  TParticle* particle = 
    new(partArray[fNParticles++]) TParticle(pdgCode, trackID, parentID, 
					    nPoints, daughter1ID, 
					    daughter2ID, px, py, pz, e, 
					    vx, vy, vz, time);
  particle->SetPolarisation(polx, poly, polz);
  particle->SetWeight(weight);
  particle->SetUniqueID(proc);

  // --> Increment counter
  if (parentID < 0) fNPrimaries++;

  // --> Set argument variable
  ntr = trackID;

  // --> Push particle on the stack if toBeDone is set
  if (toBeDone == 1) fStack.push(particle);

}
// -------------------------------------------------------------------------

  

// -----   Virtual method PopNextTrack   -----------------------------------
TParticle* PndStack::PopNextTrack(Int_t& iTrack) {

  // If end of stack: Return empty pointer
  if (fStack.empty()) {
    iTrack = -1;
    return NULL;
  }

  // If not, get next particle from stack
  TParticle* thisParticle = fStack.top();
  fStack.pop();

  if ( !thisParticle) {
    iTrack = 0;
    return NULL;
  }

  fCurrentTrack = thisParticle->GetStatusCode();
  iTrack = fCurrentTrack;

  return thisParticle;

}
// -------------------------------------------------------------------------

  

// -----   Virtual method PopPrimaryForTracking   --------------------------
TParticle* PndStack::PopPrimaryForTracking(Int_t iPrim) {

  // Get the iPrimth particle from the fStack TClonesArray. This
  // should be a primary (if the index is correct).

  // Test for index
  if (iPrim < 0 || iPrim >= fNPrimaries) {
    cout << "-E- PndStack: Primary index out of range! " << iPrim << endl;
    Fatal("PndStack::PopPrimaryForTracking", "Index out of range");
  }

  // Return the iPrim-th TParticle from the fParticle array. This should be
  // a primary.
  TParticle* part = (TParticle*)fParticles->At(iPrim);
  if ( ! (part->GetMother(0) < 0) ) {
    cout << "-E- PndStack:: Not a primary track! " << iPrim << endl;
    Fatal("PndStack::PopPrimaryForTracking", "Not a primary track");
  }

  return part;

}
// -------------------------------------------------------------------------


  
// -----   Public method AddParticle   -------------------------------------
void PndStack::AddParticle(TParticle* oldPart) {
  TClonesArray& array = *fParticles;
  TParticle* newPart = new(array[fIndex]) TParticle(*oldPart);
  newPart->SetWeight(oldPart->GetWeight());
  newPart->SetUniqueID(oldPart->GetUniqueID());
  fIndex++;
}
// -------------------------------------------------------------------------



// -----   Public method FillTrackArray   ----------------------------------
void PndStack::FillTrackArray() {

  if(gDebug!=0) cout << "-I- PndStack: Filling MCTrack array..." << endl;

  // --> Reset index map and number of output tracks
  fIndexMap.clear();
  fNTracks = 0;

  // --> Check tracks for selection criteria
  SelectTracks();

  // --> Loop over fParticles array and copy selected tracks
  for (Int_t iPart=0; iPart<fNParticles; iPart++) {

    fStoreIter = fStoreMap.find(iPart);
    if (fStoreIter == fStoreMap.end() ) {
      cout << "-E- PndStack: Particle " << iPart 
	   << " not found in storage map!" << endl;
      Fatal("PndStack::FillTrackArray",
	    "Particle not found in storage map.");
    }
    Bool_t store = (*fStoreIter).second;

    if (store) {
      new( (*fTracks)[fNTracks]) PndMCTrack(GetParticle(iPart));
      fIndexMap[iPart] = fNTracks;
      fNTracks++;
    }
    else fIndexMap[iPart] = -2;

  }

  // --> Map index for primary mothers
  fIndexMap[-1] = -1;

}
// -------------------------------------------------------------------------



// -----   Public method UpdateTrackIndex   --------------------------------
void PndStack::UpdateTrackIndex(TRefArray* detList) {

  if(gDebug!=0)cout << "-I- PndStack: Updating track indizes...";
  Int_t nColl = 0;

  // First update mother ID in MCTracks
  for (Int_t i=0; i<fNTracks; i++) {
    PndMCTrack* track = (PndMCTrack*)fTracks->At(i);
    Int_t iMotherOld = track->GetMotherID();
    fIndexIter = fIndexMap.find(iMotherOld);
    if (fIndexIter == fIndexMap.end() && gDebug!=0 ) {
      cout << "-E- PndStack: Particle index " << iMotherOld 
	   << " not found in dex map! " << endl;
      Fatal("PndStack::UpdateTrackIndex",
		"Particle index not found in map");
    }
    track->SetMotherID( (*fIndexIter).second );
  }

  // Now iterate through all active detectors
  TIterator* detIter = detList->MakeIterator();
  detIter->Reset();
  FairDetector* det = NULL;
  while( (det = (FairDetector*)detIter->Next() ) ) {

    // --> Get hit collections from detector
    Int_t iColl = 0;
    TClonesArray* hitArray;
    while ( (hitArray = det->GetCollection(iColl++)) ) {
      nColl++;
      Int_t nPoints = hitArray->GetEntriesFast();
      
      // --> Update track index for all MCPoints in the collection
      for (Int_t iPoint=0; iPoint<nPoints; iPoint++) {
	FairMCPoint* point = (FairMCPoint*)hitArray->At(iPoint);
	Int_t iTrack = point->GetTrackID();

	fIndexIter = fIndexMap.find(iTrack);
	if (fIndexIter == fIndexMap.end()) {
	  cout << "-E- PndStack: Particle index " << iTrack 
	       << " not found in dex map! " << endl;
	  Fatal("PndStack::UpdateTrackIndex",
		"Particle index not found in map");
	}
	point->SetTrackID((*fIndexIter).second);
      }

    }   // Collections of this detector
  }     // List of active detectors
  delete  detIter;
  if(gDebug!=0) cout << "...stack and " << nColl << " collections updated." << endl;

}
// -------------------------------------------------------------------------



// -----   Public method Reset   -------------------------------------------
void PndStack::Reset() {
  fIndex = 0;
  fCurrentTrack = -1;
  fNPrimaries = fNParticles = fNTracks = 0;
  while (! fStack.empty() ) fStack.pop();
  fParticles->Clear();
  fTracks->Clear();
}
// -------------------------------------------------------------------------



// -----   Public method Register   ----------------------------------------
void PndStack::Register() {
  FairRootManager::Instance()->Register("MCTrack", "Stack", fTracks,kTRUE);
}
// -------------------------------------------------------------------------



// -----   Public method Print  --------------------------------------------
void PndStack::Print(Int_t iVerbose) const {
  cout << "-I- PndStack: Number of primaries        = " 
       << fNPrimaries << endl;
  cout << "              Total number of particles  = " 
       << fNParticles << endl;
  cout << "              Number of tracks in output = "
       << fNTracks << endl;
  if (iVerbose) {
    for (Int_t iTrack=0; iTrack<fNTracks; iTrack++) 
      fTracks->At(iTrack)->Print();
  }
}
// -------------------------------------------------------------------------



// -----   Virtual method SetCurrentTrack   --------------------------------
void PndStack::SetCurrentTrack(Int_t iTrack) { 
  fCurrentTrack = iTrack;
}
// -------------------------------------------------------------------------

  

// -----   Virtual method GetNtrack   --------------------------------------
Int_t PndStack::GetNtrack() const {
  return fNParticles;
}
// -------------------------------------------------------------------------

  

// -----   Virtual method GetNprimary   ------------------------------------
Int_t PndStack::GetNprimary() const {
  return fNPrimaries;
}
// -------------------------------------------------------------------------



// -----   Virtual method GetCurrentTrack   --------------------------------
TParticle* PndStack::GetCurrentTrack() const {
  TParticle* currentPart = GetParticle(fCurrentTrack);
  if ( ! currentPart) {
    cout << "-W- PndStack: Current track not found in stack!" << endl;
    Warning("PndStack::GetCurrentTrack", "Track not found in stack");
  }
  return currentPart;
}
// -------------------------------------------------------------------------


  
// -----   Virtual method GetCurrentTrackNumber   --------------------------
Int_t PndStack::GetCurrentTrackNumber() const {
  return fCurrentTrack;
}
// -------------------------------------------------------------------------



// -----   Virtual method GetCurrentParentTrackNumber   --------------------
Int_t PndStack::GetCurrentParentTrackNumber() const {
  TParticle* currentPart = GetCurrentTrack();
  if ( currentPart ) return currentPart->GetFirstMother();
  else               return -1;
}
// -------------------------------------------------------------------------

  

// -----   Private method SelectTracks   -----------------------------------
void PndStack::SelectTracks() {

  // --> Clear storage map
  fStoreMap.clear();

  // --> Check particles in the fParticle array
  for (Int_t i=0; i<fNParticles; i++) {

    TParticle* thisPart = GetParticle(i);
    Bool_t store = kTRUE;

    // --> Get track parameters
    Int_t iMother   = thisPart->GetMother(0);
    Int_t points    = thisPart->GetMother(1);
    TLorentzVector p;
    thisPart->Momentum(p);
    Double_t energy = p.E();
    Double_t mass   = thisPart->GetMass();
    Double_t eKin = energy - mass;

    // --> Calculate number of points
    Int_t nPoints = 0;
    for (Int_t j=0; j<7; j++)
      nPoints += ( ( points & (15 << (4*j)) ) >> (4*j) );

    // --> Check for cuts (store primaries in any case)
    if (iMother < 0)            store = kTRUE;
    else {
      if (!fStoreSecondaries)   store = kFALSE;
      if (nPoints < fMinPoints) store = kFALSE;
      if (eKin < fEnergyCut)    store = kFALSE;
    }

    // --> Set storage flag
    fStoreMap[i] = store;

  }

  // --> If flag is set, flag recursively mothers of selected tracks
  if (fStoreMothers) {
    for (Int_t i=0; i<fNParticles; i++) {
      if (fStoreMap[i]) {
	Int_t iMother = GetParticle(i)->GetMother(0);
	while(iMother >= 0) {
	  fStoreMap[iMother] = kTRUE;
	  iMother = GetParticle(iMother)->GetMother(0);
	}
      }
    }
  }

}
// -------------------------------------------------------------------------

TParticle* PndStack::GetParticle(Int_t trackID) const {
  if (trackID < 0 || trackID >= fNParticles) {
    cout << "-E- PndStack: Particle index " << trackID 
	 << " out of range." << endl;
    Fatal("PndStack::GetParticle", "Index out of range");
  }
  return (TParticle*)fParticles->At(trackID);
}


ClassImp(PndStack)
