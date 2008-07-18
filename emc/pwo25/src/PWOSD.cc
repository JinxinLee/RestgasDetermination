#include "PWO25SD.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ios.hh"
#include "G4VPhysicalVolume.hh"

#include <iostream>

PWO25SD::PWO25SD(G4String name, G4int modules)
:G4VSensitiveDetector(name), fModules(modules)
{
    fCollID = -1;
    collectionName.insert("PWO25");
    // maybe one should use here something STL like
    fHitList = new PWO25Hit*[fModules];
    for(G4int i=0;i<fModules;fHitList[i++]=NULL);
}

PWO25SD::~PWO25SD(){;}

void PWO25SD::Initialize(G4HCofThisEvent*HCE)
{

	fHitCollection = new PWO25HitsCollection
							("Crystal set",collectionName[0]);
	for (int i=0;i<=10000;i++)
	if (fCollID<0) {
		fCollID = GetCollectionID(0);
	}
	
	HCE->AddHitsCollection(fCollID,fHitCollection);
}

G4bool PWO25SD::ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist)
{
	// if there is no energy deposit, return
	// (however, a comparison edep==0. is nothing one should do, I know)
	G4double edep   = aStep->GetTotalEnergyDeposit();
	if(edep==0.) return false;
	
	// index to mc truth track
	G4int trackId = aStep->GetTrack()->GetTrackID();
	// get volume and copt number (i.e. module number in ring)
	G4VPhysicalVolume *pVol  = aStep->GetPreStepPoint()->GetPhysicalVolume();
	G4int             modnum = pVol->GetCopyNo();
	if (modnum>=fModules) {
		std::cerr << "PWO25SD: Module number " << modnum << " in "
			<< collectionName[0] << " exceeds maximum ("
			<< fModules << ")" << std::endl;
		return false;
	}
	
	// if there is already a PWOHit in the list, just add the energy and
	// check the time
	if (fHitList[modnum]) {
		fHitList[modnum]->AddDE(edep/MeV);
	}
	else {
		// allocate memory and store pointer in the list and in collection
		PWO25Hit* aHit = new PWO25Hit(aStep);
		fHitList[modnum] = aHit;
		fHitCollection->insert( aHit );
		// store initial information
		aHit->SetDE(edep/MeV);
		aHit->SetModule(modnum);
		aHit->SetPVolume(pVol);
		aHit->SetTrackId(trackId);
	}
	
	return true;
}

void PWO25SD::EndOfEvent(G4HCofThisEvent*HCE)
{
	for(G4int i=0; i<fModules; fHitList[i++]=NULL);
}

void PWO25SD::clear()
{
}

void PWO25SD::DrawAll()
{
}

void PWO25SD::PrintAll()
{
}

