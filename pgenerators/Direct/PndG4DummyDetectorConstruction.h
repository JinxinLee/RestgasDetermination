#ifndef  PNDG4DUMMYDETECTORCONSTRUCTION_H
#define  PNDG4DUMMYDETECTORCONSTRUCTION_H 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
class G4VPhysicalVolume;

class PndG4DummyDetectorConstruction :  public G4VUserDetectorConstruction
{
  public:
  PndG4DummyDetectorConstruction();
  ~PndG4DummyDetectorConstruction();
  virtual G4VPhysicalVolume* Construct();
  virtual void ConstructSDandField();
};

#endif
