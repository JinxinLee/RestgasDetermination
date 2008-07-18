#ifndef PWO25DetectorConstruction_H
#define PWO25DetectorConstruction_H 1

class G4LogicalVolume;
class G4VPhysicalVolume;
class PWO25DetectorMessenger;
class PWO25SD;

#include "G4VUserDetectorConstruction.hh"
#include "G4SDManager.hh"

class PWO25DetectorConstruction : public G4VUserDetectorConstruction
{
  public:

	PWO25DetectorConstruction();
	~PWO25DetectorConstruction();
	
	void SetUseRevetment(G4bool);
	void SetGap(G4double);

	G4VPhysicalVolume* Construct();
	void UpdateGeometry();

  private:
	G4Material* teflon;
	G4Material* PWO;
	G4Material* Air;
	
	PWO25SD* crystal_sd;
			
	G4bool useRevetment;
	G4double gap;
	// Logical volumes
	//
	G4SDManager* SDMan;
	G4LogicalVolume* experimentalHall_log;
	G4LogicalVolume* cristal_log;
	G4LogicalVolume* space1_log;
	G4LogicalVolume* space2_log;
	// Physical volumes
	//
	G4VPhysicalVolume* experimentalHall_phys;
	G4VPhysicalVolume* cristal_phys;
	G4VPhysicalVolume* space1_phys;
	G4VPhysicalVolume* space2_phys;
	
	PWO25DetectorMessenger* detectorMessenger;  //pointer to the Messenger
	G4VPhysicalVolume* ConstructCalorimeter();
	void DefineMaterials();
};

#endif

