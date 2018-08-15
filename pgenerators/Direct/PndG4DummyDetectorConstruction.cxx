#include "PndG4DummyDetectorConstruction.h"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4SDManager.hh"
#include "G4Element.hh"
#include "G4SystemOfUnits.hh"
//#include ".hh"
//#include ".hh"
//#include ".hh"
//#include ".hh"



PndG4DummyDetectorConstruction::PndG4DummyDetectorConstruction()
{
  return;
}

PndG4DummyDetectorConstruction::~PndG4DummyDetectorConstruction()
{
  return;
}

G4VPhysicalVolume* PndG4DummyDetectorConstruction::Construct()
{
  G4Material* Vacuum = new G4Material("interGalactic", 1., 1.008*g/mole,1.e-25*g/cm3, kStateGas,2.73*kelvin, 3.e-18*pascal);
  G4Box* gExpHall = new G4Box("gExpHall",500,500,500);
  G4LogicalVolume* lExpHall = new G4LogicalVolume(gExpHall,Vacuum,"lExpHall",0,0,0);
  G4VPhysicalVolume* wExpHall  = new G4PVPlacement(0,G4ThreeVector(),lExpHall,"gExpHall",0,false,0);
  return wExpHall;
}

void PndG4DummyDetectorConstruction::ConstructSDandField()
{
  return;
}
