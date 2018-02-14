#include "PWO25DetectorConstruction.hh"
#include "PWO25DetectorMessenger.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4RunManager.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"
#include "PWO25SD.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

PWO25DetectorConstruction::PWO25DetectorConstruction()
 :  experimentalHall_log(0), cristal_log(0)
{
	useRevetment=false;
	gap=-1.;
	DefineMaterials();
	detectorMessenger = new PWO25DetectorMessenger(this);
}

PWO25DetectorConstruction::~PWO25DetectorConstruction()
{
	delete detectorMessenger;
}

G4VPhysicalVolume* PWO25DetectorConstruction::Construct()
{
	SDMan = G4SDManager::GetSDMpointer();
	crystal_sd = new PWO25SD("PWO25",25);
	SDMan->AddNewDetector(crystal_sd);

  return ConstructCalorimeter();
}

void PWO25DetectorConstruction::DefineMaterials()
{ 
	G4double a,z,density;
	G4int natoms;
	G4String name,symbol;
	
	//Air
	G4Element* N = new G4Element("Nitrogen", "N", z=7., a= 14.01*g/mole);
	G4Element* O = new G4Element("Oxygen"  , "O", z=8., a= 16.00*g/mole);
	
	Air = new G4Material("Air", density= 1.29*mg/cm3, 2);
	Air->AddElement(N, 70*perCent);
	Air->AddElement(O, 30*perCent);
	
	// PWO
	G4Element* elO = new G4Element(name="Oxygen", symbol="O", z=8.,a = 16.00*g/mole);
	G4Element* elPb = new G4Element(name="Lead", symbol="Pb", z=82.,a = 207.19*g/mole);
	G4Element* elW = new G4Element(name="Tungsten", symbol="W", z=74.,a = 183.85*g/mole);
	
	density = 8.28*g/cm3;
	PWO = new G4Material(name="PbWO4", density, 3);
	PWO->AddElement(elPb, natoms=1);
	PWO->AddElement(elW,  natoms=1);
	PWO->AddElement(elO,  natoms=4);
	
	// Teflon
	G4Element* elC = new G4Element(name="Carbon", symbol="C", z=6.,a = 12.0*g/mole);
	G4Element* elF = new G4Element(name="Fluorine", symbol="F", z=9.,a = 19.0*g/mole);
	
	density = 2.15*g/cm3;
	teflon = new G4Material(name="teflon", density, 2);
	teflon->AddElement(elC, natoms=1);
	teflon->AddElement(elF,  natoms=1);
	
}


G4VPhysicalVolume* PWO25DetectorConstruction::ConstructCalorimeter()
{
	// Clean old geometry, if any
	//
	G4GeometryManager::GetInstance()->OpenGeometry();
	G4PhysicalVolumeStore::GetInstance()->Clean();
	G4LogicalVolumeStore::GetInstance()->Clean();
	G4SolidStore::GetInstance()->Clean();
	
	if (useRevetment==true){
		std::cout<<"Crystals with revetment. Width = "<<gap<<" mm"<<std::endl;
	}
	
	G4double expHall_x = 1.0*m;
	G4double expHall_y = 1.0*m;
	G4double expHall_z = 1.0*m;
	G4Box* experimentalHall_box
		= new G4Box("expHall_box",expHall_x,expHall_y,expHall_z);
	experimentalHall_log = new G4LogicalVolume(experimentalHall_box,
																Air,"expHall_log",0,0,0);
	experimentalHall_phys = new G4PVPlacement(0,G4ThreeVector(),
													experimentalHall_log,"expHall",0,false,0);

	G4double cristal_x = 20.0*cm;
	G4double cristal_y = 2.0*cm;
	G4double cristal_z = 2.0*cm;
	G4Box* cristal_box
		= new G4Box("cristal",0.5*cristal_x,0.5*cristal_y,0.5*cristal_z);
	cristal_log = new G4LogicalVolume(cristal_box, PWO,"cristal_log",0,0,0);
	cristal_log->SetSensitiveDetector(crystal_sd);

	if (useRevetment==true)
	{
		// Space between crystals filled with teflon
		G4double space1_x = 20.0*cm;
		G4double space1_y = 2.0*cm;
		G4double space1_z = gap;
		G4Box* space1
			= new G4Box("space1",0.5*space1_x,0.5*space1_y,0.5*space1_z);
		space1_log = new G4LogicalVolume(space1, teflon,"space1_log",0,0,0);
		
		// Space between crystals filled with teflon
		G4double space2_x = 20.0*cm;
		G4double space2_y = gap;
		G4double space2_z = 5*cristal_z+4*gap;
		G4Box* space2
			= new G4Box("space2",0.5*space2_x,0.5*space2_y,0.5*space2_z);
		space2_log = new G4LogicalVolume(space2, teflon,"space2_log",0,0,0);
	} else if (gap>0) {
			// Space between crystals filled with air
		G4double space1_x = 20.0*cm;
		G4double space1_y = 2.0*cm;
		G4double space1_z = gap;
		G4Box* space1
			= new G4Box("space1",0.5*space1_x,0.5*space1_y,0.5*space1_z);
		space1_log = new G4LogicalVolume(space1, Air,"space1_log",0,0,0);
		
		// Space between crystals filled with air
		G4double space2_x = 20.0*cm;
		G4double space2_y = gap;
		G4double space2_z = 5*cristal_z+4*gap;
		G4Box* space2
			= new G4Box("space2",0.5*space2_x,0.5*space2_y,0.5*space2_z);
		space2_log = new G4LogicalVolume(space2, Air,"space2_log",0,0,0);
	}

	G4int k,l,m;
	G4double crystal_y_pos, crystal_z_pos;
	if (gap>0)
	{

		G4double space1_y_pos, space1_z_pos, space2_y_pos, space2_z_pos;
		for (G4int i=0;i<5;i++)
		{
			for (G4int j=0;j<5;j++)
			{
				k=i*5+j; //index of crystal (0-24) from the left top corner
				crystal_y_pos=cristal_y*(2-i)+gap*(2-i);  // vertical direction
				crystal_z_pos=cristal_z*(j-2)+gap*(j-2); // horizontal direction
		
				cristal_phys = new G4PVPlacement(0,G4ThreeVector(0,crystal_y_pos,crystal_z_pos),  cristal_log,"cristal",experimentalHall_log,false,k);
		
				if (j!=4)
				{
					l=i*4+j; //index of space1 (0-19)
					space1_y_pos=cristal_y*(2-i)+gap*(2-i);
					space1_z_pos=cristal_z*(j-1.5)+gap*(j-1.5);
		
					space1_phys = new G4PVPlacement(0,G4ThreeVector(0,space1_y_pos,space1_z_pos),
													space1_log,"space1_phys",experimentalHall_log,false,l);
				}
			}
		
			if (i!=4)
			{
				m=i; //index of space2 (0-3)
				space2_y_pos=cristal_y*(i-1.5)+gap*(i-1.5);
				space2_z_pos=0;
		
				space2_phys = new G4PVPlacement(0,G4ThreeVector(0,space2_y_pos,space2_z_pos),
											space2_log,"space2_phys",experimentalHall_log,false,m);
			}
		}
	}
	else 
	{
		for (G4int i=0;i<5;i++)
		{
			for (G4int j=0;j<5;j++)
			{
				k=i*5+j; //index of crystal (0-24) from the left top corner
				crystal_y_pos=cristal_y*(2-i);  // vertical direction
				crystal_z_pos=cristal_z*(j-2); // horizontal direction
		
				cristal_phys = new G4PVPlacement(0,G4ThreeVector(0,crystal_y_pos,crystal_z_pos),  cristal_log,"cristal",experimentalHall_log,false,k);
			}
		}
	}

	//--------- Visualization attributes -------------------------------
	
	experimentalHall_log->SetVisAttributes(G4VisAttributes::Invisible);
	
	G4VisAttributes* VisAtt1= new G4VisAttributes(G4Colour(1.0,1.0,0.0));
	cristal_log  ->SetVisAttributes(VisAtt1);
	
	if (useRevetment==true)
	{
		G4VisAttributes* VisAtt2= new G4VisAttributes(G4Colour(0.5,1.0,0.5));
		space1_log  ->SetVisAttributes(VisAtt2);
		
		G4VisAttributes* VisAtt3= new G4VisAttributes(G4Colour(1.0,0.5,0.5));
		space2_log  ->SetVisAttributes(VisAtt3);
	}
	
	return experimentalHall_phys;
}

void PWO25DetectorConstruction::SetUseRevetment(G4bool val)
{
	useRevetment=val;
}

void PWO25DetectorConstruction::SetGap(G4double val)
{
	gap=val;
}

void PWO25DetectorConstruction::UpdateGeometry()
{
  G4RunManager::GetRunManager()->DefineWorldVolume(ConstructCalorimeter());
}
