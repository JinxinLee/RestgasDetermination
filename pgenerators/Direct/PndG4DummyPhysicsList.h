#ifndef  PNDG4DUMMYPHYSICSLIST_H
#define  PNDG4DUMMYPHYSICSLIST_H 1

#include "globals.hh"
#include "G4VUserPhysicsList.hh"


class PndG4DummyPhysicsList : public G4VUserPhysicsList
{
  public:
  PndG4DummyPhysicsList();
  ~PndG4DummyPhysicsList();
  virtual void ConstructParticle();      // construction of particles
  virtual void ConstructProcess();       // construct processes and register them to particles
};

#endif
