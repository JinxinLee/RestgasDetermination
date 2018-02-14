#include "PWO25DetectorMessenger.hh"
#include "PWO25DetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "globals.hh"

//PWO25DetectorMessenger* PWO25DetectorMessenger::fInstance = 0;

PWO25DetectorMessenger::PWO25DetectorMessenger(PWO25DetectorConstruction *PWO25Det):
	PWO25Detector(PWO25Det)
{
	PWO25Dir = new G4UIdirectory("/PWO25/");
	PWO25Dir->SetGuidance("UI commands specific to this example.");
	
	detDir = new G4UIdirectory("/PWO25/det/");
	detDir->SetGuidance("detector control");
	
	GapCmd = new G4UIcmdWithADoubleAndUnit("/PWO25/det/setGap",this);
	GapCmd->SetGuidance("Define gap between crystals");
	GapCmd->SetParameterName("gap",false);
	GapCmd->SetDefaultUnit("mm");
	GapCmd->SetUnitCategory("Length");
	GapCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	UseRevetmentCmd = new G4UIcmdWithABool("/PWO25/det/setUseRevetment",this);
	UseRevetmentCmd->SetGuidance("Define if revetment around crystals is used");
	UseRevetmentCmd->SetParameterName("useRevetment",false);
	UseRevetmentCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	UpdateCmd = new G4UIcmdWithoutParameter("/PWO25/det/update",this);
	UpdateCmd->SetGuidance("Update calorimeter geometry.");
	UpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
	UpdateCmd->SetGuidance("if you changed geometrical value(s).");
	UpdateCmd->AvailableForStates(G4State_Idle);
	
	valGap=0.5*mm;
	useRevetment=false;
}

PWO25DetectorMessenger::~PWO25DetectorMessenger()
{
	delete GapCmd;
	delete UseRevetmentCmd;
	delete detDir;
	delete PWO25Dir;
}

void PWO25DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
	if( command == GapCmd )	{
		 PWO25Detector->SetGap(GapCmd->GetNewDoubleValue(newValue));
	}
	if( command == UseRevetmentCmd )	{
		 PWO25Detector->SetUseRevetment(UseRevetmentCmd->GetNewBoolValue(newValue));
	}
	
	if( command == UpdateCmd ) {
		 PWO25Detector->UpdateGeometry();
	}

}

