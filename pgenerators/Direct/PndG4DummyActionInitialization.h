#ifndef  PNDG4DUMMYACTIONINITIALIZATION_H
#define  PNDG4DUMMYACTIONINITIALIZATION_H 1

#include "globals.hh"
#include "G4VUserActionInitialization.hh"
class G4VSteppingVerbose;

class PndG4DummyActionInitialization :  public G4VUserActionInitialization
{
  public:
  PndG4DummyActionInitialization();
  virtual ~PndG4DummyActionInitialization();
  virtual void BuildForMaster() const;
  virtual void Build() const;
  virtual G4VSteppingVerbose* InitializeSteppingVerbose() const;
};

#endif
