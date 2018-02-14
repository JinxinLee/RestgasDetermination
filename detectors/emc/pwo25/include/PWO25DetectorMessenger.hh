#ifndef PWO25DetectorMessenger_h
#define PWO25DetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithABool.hh"

class PWO25DetectorConstruction;
class G4UIcmdWithoutParameter;

class PWO25DetectorMessenger: public G4UImessenger
{
  private:
    //static PWO25DetectorMessenger        *fInstance;

  public:
	PWO25DetectorMessenger(PWO25DetectorConstruction *);
	~PWO25DetectorMessenger();
	//static inline PWO25DetectorMessenger* GetInstance() {return fInstance;};

	void SetNewValue(G4UIcommand*, G4String);
	G4double getGap() const { return valGap; };
	G4bool getUseRevetment() const { return useRevetment; };

  private:
	  
	PWO25DetectorConstruction *PWO25Detector;

	G4UIdirectory*             PWO25Dir;
	G4UIdirectory*             detDir;
	G4UIcmdWithADoubleAndUnit* GapCmd;
	G4UIcmdWithABool* UseRevetmentCmd;
	G4UIcmdWithoutParameter*   UpdateCmd;	

	G4double valGap;
	G4bool   useRevetment;

};


#endif

