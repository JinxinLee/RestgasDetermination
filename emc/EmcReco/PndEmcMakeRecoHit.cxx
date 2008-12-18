//---------------------------------------------------------------------
// File and Version Information:
// 	$Id: $
//
// Description:
// This task convert PndEmcBumps to PndEmcRecoHit objects
// At the moment PndEmcBump and PndEmcRecoHit have alsmost identically structure
// The main difference that for PndEmcBump position() nad energy() calculate corresponding values but for PndEmcRecoHit just return corresponding data member
//
// Environment:
//	Software developed for the PANDA Detector at GSI.
//
//------------------------------------------------------------------------

#include "PndEmcMakeRecoHit.h"

#include "PndEmcRecoHit.h"
#include "PndEmcBump.h"
#include "PndEmcRecoPar.h"

#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"

#include "TClonesArray.h"
#include "TStopwatch.h"
#include "TROOT.h"
#include "TVector3.h"

#include <iostream>
#include <vector>
#include <set>

using std::cout;
using std::endl;

PndEmcMakeRecoHit::PndEmcMakeRecoHit(Int_t verbose, Bool_t storerecohits) 
{
	fVerbose=verbose;
	fStoreRecoHits=storerecohits;
}

//--------------
// Destructor --
//--------------

PndEmcMakeRecoHit::~PndEmcMakeRecoHit()
{ 
}

// -----   Public method Init   -------------------------------
InitStatus PndEmcMakeRecoHit::Init() {
 
  	// Get RootManager
	CbmRootManager* ioman = CbmRootManager::Instance();
	if ( ! ioman )
	{
		cout << "-E- PndEmcMakeRecoHit::Init: "
		<< "RootManager not instantiated!" << endl;
		return kFATAL;
	}
	
	// Get input array
	fBumpArray = (TClonesArray*) ioman->GetObject("EmcBump");
	if ( ! fBumpArray ) {
		cout << "-W- PndEmcMakeRecoHit::Init: "
		<< "No PndEmcBump array!" << endl;
		return kERROR;
	}
	
	// Create and register output array
	fRecoHitArray = new TClonesArray("PndEmcRecoHit");
	
	ioman->Register("EmcRecoHit","Emc",fRecoHitArray,fStoreRecoHits);
	
	cout << "-I- PndEmcMakeRecoHit: Intialization successfull" << endl;

	return kSUCCESS;
}


void PndEmcMakeRecoHit::Exec(Option_t* opt) 
{
	// Reset output array
	if ( ! fRecoHitArray ) Fatal("Exec", "No Cluster Array");
	fRecoHitArray->Delete();

	Int_t nBumps = fBumpArray->GetEntriesFast();

	//loop to build Cluster
	for (Int_t iBump=0; iBump<nBumps; iBump++)
	{
		PndEmcBump* theBump = (PndEmcBump*) fBumpArray->At(iBump);
		Double_t energy=theBump->energy();
		TVector3 position=theBump->position();
		new((*fRecoHitArray)[iBump]) PndEmcRecoHit(energy,position);
		
	}
}

void PndEmcMakeRecoHit::SetParContainers() {

  // Get run and runtime database
  CbmRun* run = CbmRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  CbmRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get Emc reconstruction parameter container
  fRecoPar = (PndEmcRecoPar*) db->getContainer("PndEmcRecoPar");
}

void PndEmcMakeRecoHit::SetStorageOfRecoHits(Bool_t val)
{
  fStoreRecoHits=val;
  return;
}
  

ClassImp(PndEmcMakeRecoHit)
