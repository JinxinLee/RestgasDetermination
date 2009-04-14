// -------------------------------------------------------------------------
// -----                      PndGemTrack source file                  -----
// -----                  Created 19.03.2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

// Pnd includes
#include "PndGemTrack.h"
#include "PndGemHit.h"
#include "PndGemDigi.h"

// C++ includes
#include <iostream>
using std::cout;
using std::endl;
using std::pair;

// -----   Default constructor   -------------------------------------------
PndGemTrack::PndGemTrack() {
  fFlag     = 0;
  fChi2     = 0.;
  fNDF      = 0;
  fELoss    = 0.;
  fGemHits  = 0;
  fGemDigis = 0;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndGemTrack::~PndGemTrack() {
  fGemHitMultimap.clear();
  fGemDigiMultimap.clear();
}
// -------------------------------------------------------------------------


// -----   Public method AddHit   ------------------------------------------
void PndGemTrack::AddHit(PndGemHit* hit, Int_t hitID) {
  // hitID - index of hit in global array of hits
  fGemHitMultimap.insert(pair<Double_t, Int_t>(hit->GetZ(),hitID));
}
// -------------------------------------------------------------------------

// -----   Public method AddDigi   -----------------------------------------
void PndGemTrack::AddDigi(PndGemDigi* digi, Int_t digiID) {
  // digiID - index of digi in global array of digis
  fGemDigiMultimap.insert(pair<Double_t, Int_t>(digi->GetDetectorId(),digiID));
}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndGemTrack::Print() {
  cout << " Number of attached Gem hits:  "<< fGemHits.GetSize() << endl;
  cout << " Number of attached Gem digis: "<< fGemDigis.GetSize() << endl;
}
// -------------------------------------------------------------------------


// -----   Public method SortDigis  ----------------------------------------
void PndGemTrack::SortDigis() {
  
  Int_t index = 0;
  multimap<Double_t, Int_t>::iterator it;
  
  fGemDigis.Reset();
  fGemDigis.Set(fGemDigiMultimap.size());
  
  for(it = fGemDigiMultimap.begin(); it != fGemDigiMultimap.end(); it++) {
    
    //cout << (*it).first << endl;		
    fGemDigis[index] = it->second;
    index++;		
  }	
  //cout <<"fGemDigis: "<< fGemDigis.GetSize() << endl;
}
// -------------------------------------------------------------------------


// -----   Public method SortHits   ----------------------------------------
void PndGemTrack::SortHits() {
  
  Int_t index = 0;
  multimap<Double_t, Int_t>::iterator it;
  
  fGemHits.Reset();
  fGemHits.Set(fGemHitMultimap.size());
  
  for(it = fGemHitMultimap.begin(); it != fGemHitMultimap.end(); it++) {
    fGemHits[index] = it->second;
    index++;
  }
  //  cout <<"fGemHits: "<< fGemHits.GetSize() << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndGemTrack)
