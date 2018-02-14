#ifndef PWO25PrimaryGeneratorAction_h
#define PWO25PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"

class G4ParticleGun;
class G4Event;

class PWO25PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PWO25PrimaryGeneratorAction();
    ~PWO25PrimaryGeneratorAction();

  public:
    void GeneratePrimaries(G4Event* anEvent);

  private:
    G4ParticleGun* particleGun;
};

#endif


