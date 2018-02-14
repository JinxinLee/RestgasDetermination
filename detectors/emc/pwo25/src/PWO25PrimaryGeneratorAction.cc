#include "PWO25PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"

PWO25PrimaryGeneratorAction::PWO25PrimaryGeneratorAction()
{
	G4int n_particle = 1;
	particleGun = new G4ParticleGun(n_particle);
	
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName;
	particleGun->SetParticleDefinition(particleTable->FindParticle(particleName="gamma"));
	particleGun->SetParticleEnergy(1.0*GeV);
	particleGun->SetParticlePosition(G4ThreeVector(-10.0*cm, 0.0, 0.0));
}

PWO25PrimaryGeneratorAction::~PWO25PrimaryGeneratorAction()
{
	delete particleGun;
}

void PWO25PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	G4ThreeVector v(1.0,0.0,0.0);
	particleGun->SetParticleMomentumDirection(v);
	particleGun->GeneratePrimaryVertex(anEvent);
}


