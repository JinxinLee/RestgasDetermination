#include <iostream>
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"
#include "G4SDManager.hh"

#include "PWO25DetectorConstruction.hh"
#include "PWO25RunAction.hh"
#include "PWO25EventAction.hh"
#include "PWO25Hit.hh"

//#include "PWO25SimEvent.hh"
#include "TTree.h"

PWO25EventAction::PWO25EventAction()
{
}

PWO25EventAction::~PWO25EventAction()
{}

void PWO25EventAction::BeginOfEventAction(const G4Event*)
{
	PWO25RunAction::GetPWO25Event()->Clear();
}

void PWO25EventAction::EndOfEventAction(const G4Event* evt)
{
	// sensitive detector manager
	G4SDManager* SDman = G4SDManager::GetSDMpointer();
	
	// hit collections of this event (if no, return)
	G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
	
	if (!HCE) return;
	
	// this is the place where the data have to go
	PWO25SimEvent* Event = PWO25RunAction::GetPWO25Event();
	
	G4int hit=0;	// Flag, if the detector has seen anything
		
	// find the hit collection from name -> id -> collection
	G4int CollID = SDman->GetCollectionID("PWO25");
	PWO25HitsCollection* PHC = (PWO25HitsCollection*) (HCE->GetHC(CollID));//CollID
	
	if (PHC) { // is there any collection ?
		G4int n = PHC->entries();
		for (G4int j=0; j<n ;j++) {
			Event->Add(
				(*PHC)[j]->GetModule(),  // module number
				(*PHC)[j]->GetDE(),      // energy
				(*PHC)[j]->TrackId());// G4Track
			// flag hits
			hit = 1;
		} // end modules in ring
	}
	
	if (hit) PWO25RunAction::GetInstance()->GetRootTree()->Fill();
}

