// -------------------------------------------------------------------------
// -----                      PndDchTrack source file                  -----
// -----                  Created 27/01/08  by A. Bubak                -----
// -------------------------------------------------------------------------

// Pnd includes
#include "PndDchTrack.h"
#include "PndDchHit.h"
#include "PndDchCylinderHit.h"
#include "PndDchDigi.h"

// C++ includes
#include <iostream>
using std::cout;
using std::endl;
using std::pair;

// -----   Default constructor   -------------------------------------------
PndDchTrack::PndDchTrack() {
	fFlag     = 0;
	fChi2     = 0.;
	fNDF      = 0;
	fELoss    = 0.;
	fDchHits  = 0;
	fDchCylinderHits = 0;
	fDchDigis = 0;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndDchTrack::~PndDchTrack() {
	fDchHitMultimap.clear();
	fDchCylinderHitMultimap.clear();
	fDchDigiMultimap.clear();
}
// -------------------------------------------------------------------------


// -----   Public method AddHit   ------------------------------------------
void PndDchTrack::AddHit(PndDchHit* hit, Int_t hitID) {
	// hitID - index of hit in global array of hits
	fDchHitMultimap.insert(pair<Double_t, Int_t>(hit->GetZ(),hitID));
}
// -------------------------------------------------------------------------

// -----   Public method AddCylinderHit   ------------------------------------------
void PndDchTrack::AddCylinderHit(PndDchCylinderHit* chit, Int_t chitID) {
	// chitID - index of cylinder hit in global array of cylinder hits
	fDchCylinderHitMultimap.insert(pair<Double_t, Int_t>(chit->GetWireZcoordGlobal(), chitID));
}
// -------------------------------------------------------------------------

// -----   Public method AddDigi   -----------------------------------------
void PndDchTrack::AddDigi(PndDchDigi* digi, Int_t digiID) {
	// digiID - index of digi in global array of digis
	fDchDigiMultimap.insert(pair<Double_t, Int_t>(digi->GetChamber(),digiID));
}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndDchTrack::Print() {
  cout << " Number of attached Dch hits:  "<< fDchHits.GetSize() << endl;
  cout << " Number of attached Dch chits: "<< fDchCylinderHits.GetSize() << endl;
  cout << " Number of attached Dch digis: "<< fDchDigis.GetSize() << endl;
  cout << "Printing hit indices of cyl hits:" << endl;
  for(unsigned int i=0;i<fDchCylinderHits.GetSize();++i){
    std::cout << fDchCylinderHits[i] << " " ;
  }
  std::cout << std::endl;
}
// -------------------------------------------------------------------------


// -----   Public method SortDigis  ----------------------------------------
void PndDchTrack::SortDigis() {

	Int_t index = 0;
	multimap<Double_t, Int_t>::iterator it;

	fDchDigis.Reset();
	fDchDigis.Set(fDchDigiMultimap.size());

	for(it = fDchDigiMultimap.begin(); it != fDchDigiMultimap.end(); it++) {

		//cout << (*it).first << endl;		
		fDchDigis[index] = it->second;
		index++;		
	}	
	//cout <<"fDchDigis: "<< fDchDigis.GetSize() << endl;
}
// -------------------------------------------------------------------------


// -----   Public method SortHits   ----------------------------------------
void PndDchTrack::SortHits() {

	Int_t index = 0;
	multimap<Double_t, Int_t>::iterator it;

	fDchHits.Reset();
	fDchHits.Set(fDchHitMultimap.size());

	for(it = fDchHitMultimap.begin(); it != fDchHitMultimap.end(); it++) {
		fDchHits[index] = it->second;
		index++;
	}
	//  cout <<"fDchHits: "<< fDchHits.GetSize() << endl;
}
// -------------------------------------------------------------------------


// -----   Public method SortCylinderHits   ----------------------------------------
void PndDchTrack::SortCylinderHits() {

	Int_t index = 0;
	multimap<Double_t, Int_t>::iterator it;

	fDchCylinderHits.Reset();
	fDchCylinderHits.Set(fDchCylinderHitMultimap.size());

	for(it = fDchCylinderHitMultimap.begin(); it != fDchCylinderHitMultimap.end(); it++) {
		fDchCylinderHits[index] = it->second;
		index++;
	}
	//  cout <<"fDchHits: "<< fDchHits.GetSize() << endl;
}
// -------------------------------------------------------------------------


ClassImp(PndDchTrack)
