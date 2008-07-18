//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      A Framework Task for   
//	creating  the Space-Charge-Map
//      
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Cristoforo Simonetto    TUM         (original author)
//      Felix Boehmer           TUM
//
//-----------------------------------------------------------

#ifndef TPCSPACECHARGETASK_HH
#define TPCSPACECHARGETASK_HH

//Base class
#include "CbmTask.h"
#include <vector>
// Collaborating Class Declarations --------------------------------------
class TClonesArray;
class PndTpcGas;


class PndTpcSpaceChargeTask : public CbmTask {

public:

// Constructor -----------------------------------------------------------

PndTpcSpaceChargeTask();			//default constructor
~PndTpcSpaceChargeTask();

virtual InitStatus Init();		//needs to be overidden as usual
virtual void Exec(Option_t* opt);	//...

void writeToFile(const char*);

// Modifiers -------------------------------------------------------------

// these have to be called BEFORE the Init() function is called in the macro!

void setMinR(double rmin) {_tpcMinR = rmin;}
void setMaxR(double rmax) {_tpcMaxR = rmax;}
void setMinZ(double zmin) {_tpcMinZ = zmin;}
void setMaxZ(double zmax) {_tpcMaxZ = zmax;}

void setBins(int rbins, int zbins) {_rBinCount = rbins; _zBinCount = zbins;}
void setRate(double rate) {_rate = rate;}
void setSupression(double sup) {_supression = sup;}
void setGain(double gain) {_gemGain = gain;}

private:

TString _pointBranchName;
TClonesArray* _pointArray;
PndTpcGas* _gas;
	
double _writeDownTime;   	//timestep to write file [ns]
double _rate; 			//inverse time between 2 events [1/ns]
double _ionDriftVelocity;	//[cm/ns]
double _supression;		//supression-factor of ion-creation at the gems
double _tpcMinR;		//starting radius for the Space-Charge-Map [cm]
double _tpcMaxR;
double _tpcMinZ;		//Z geometry
double _tpcMaxZ;
int _rBinCount;			//number of segments in r direction
int _zBinCount;
double _rBinWidth;		//size of one Bin
double _zBinWidth;

double _angle;			//opening angle between the 2 tpc volumes [rad]	

int _time;			//controls Ion-drift. Steered by _rate
int _errorCount;                //keep track of # of hits outside the volume
double _distPerTime;		//Ion-travel-distance each time-step
double _gemGain;		//gain factor of the gem. Set by hand atm
double _gemCharge;		//net charge produced at the gems
double _WGas;			//obtained from PndTpcGas-Object

std::vector< std::vector<int> > _chargeMap;	//segmented map of net charge

public:
  ClassDef(PndTpcSpaceChargeTask,1)
	
};

#endif
