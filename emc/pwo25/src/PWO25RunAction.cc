#include "G4Run.hh"
#include "G4UImanager.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"

#include "PWO25RunAction.hh"

PWO25SimEvent* PWO25RunAction::fPWO25Event = NULL;
PWO25RunAction* PWO25RunAction::fInstance = NULL;

PWO25RunAction::PWO25RunAction()
{
	fInstance = this;
	fPWO25Event = new PWO25SimEvent();
}

PWO25RunAction::~PWO25RunAction()
{}


void PWO25RunAction::BeginOfRunAction(const G4Run* aRun)
{
	rootFile = new TFile("pwo25.root","RECREATE","GEANT4 output file");
	rootDataTree = new TTree("Events","Geant4 simulation output");
	rootDataTree->Branch("crystal","PWO25SimEvent",&fPWO25Event);
}


void PWO25RunAction::EndOfRunAction(const G4Run* aRun)
{
	rootDataTree->Write();
	rootFile->Write();
	rootFile->Close();
	delete rootFile;
}

