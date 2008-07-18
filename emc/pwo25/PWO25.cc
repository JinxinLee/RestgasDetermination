#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"

#include "PWO25DetectorConstruction.hh"
#include "PWO25DetectorMessenger.hh"
#include "PWO25PhysicsList.hh"
#include "PWO25PrimaryGeneratorAction.hh"
#include "PWO25RunAction.hh"
#include "PWO25EventAction.hh"

#include "PWO25VisManager.hh"

int main()
{

	//new PWO25DetectorMessenger;
	// Construct the default run manager
	G4RunManager* runManager = new G4RunManager;
	
	// set mandatory initialization classes
	runManager->SetUserInitialization(new PWO25DetectorConstruction);
	runManager->SetUserInitialization(new PWO25PhysicsList);
	
	// set mandatory user action class
	runManager->SetUserAction(new PWO25PrimaryGeneratorAction);
	runManager->SetUserAction(new PWO25RunAction);
	runManager->SetUserAction(new PWO25EventAction);
	
	
	
	G4VisManager* visManager = new PWO25VisManager;
	visManager->Initialize();
	
	// Initialize G4 kernel
	runManager->Initialize();
	
	
	//    session = new G4UIterminal(new G4UItcsh);
	
	// get the pointer to the UI manager and set verbosities
	G4UImanager* UI = G4UImanager::GetUIpointer();
	UI->ApplyCommand("/run/verbose 1");
	//UI->ApplyCommand("/event/verbose 1");
	//UI->ApplyCommand("/tracking/verbose 1");
	
	G4UIsession * session = 0;
	session = new G4UIterminal(new G4UItcsh);
	session->SessionStart();
	delete session;
	
	// start a run
	//int numberOfEvent = 3;
	//runManager->BeamOn(numberOfEvent);
	
	// job termination
	delete visManager;
	delete runManager;
	return 0;
}


