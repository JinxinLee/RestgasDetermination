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
#include "FairTask.h"
#include <vector>
// Collaborating Class Declarations --------------------------------------
class TClonesArray;
class PndTpcGas;
class PndTpcDigiPar;


class PndTpcSpaceChargeTask : public FairTask {

public:

// Constructor -----------------------------------------------------------

PndTpcSpaceChargeTask();			//default constructor
~PndTpcSpaceChargeTask();

virtual InitStatus Init();		//needs to be overidden as usual
virtual void Exec(Option_t* opt);	//...

virtual void SetParContainers();

void writeToFile(const char*);

// Modifiers -------------------------------------------------------------

// these have to be called BEFORE the Init() function is called in the macro!

void setMinR(double rmin) {ftpcMinR = rmin;}
void setMaxR(double rmax) {ftpcMaxR = rmax;}
void setMinZ(double zmin) {ftpcMinZ = zmin;}
void setMaxZ(double zmax) {ftpcMaxZ = zmax;}

void setRBins(int r) {frBinCount=r;}
void setZBins(int z) {fzBinCount=z;}

void setBins(int rbins, int zbins) {frBinCount = rbins; fzBinCount = zbins;}
void setRate(double rate) {frate = rate;}
void setSupression(double sup) {fsupression = sup;}
void setGain(double gain) {fgemGain = gain;}
void setPrimChargeMode(bool opt) {fprimChargeOnly = opt;}

void setAliceMode(bool opt) {fALICEmode = opt;} //experimental, should not be used

private:

 TString fpointBranchName;
 TClonesArray* fpointArray;
 const PndTpcGas* fgas;
 
 double fwriteDownTime;   	//timestep to write file [ns]
 double frate; 			//inverse time between 2 events [1/ns]
 double fionDriftVelocity;	//[cm/ns]
 double fsupression;		//supression-factor of ion-creation at the gems
 double ftpcMinR;		//starting radius for the Space-Charge-Map [cm]
 double ftpcMaxR;
 double ftpcMinZ;		//Z geometry
 double ftpcMaxZ;
 int frBinCount;		//number of segments in r direction
 int fzBinCount;
 double frBinWidth;		//size of one Bin
 double fzBinWidth;
 
 double fangle;			//opening angle between the 2 tpc volumes [rad]	
 
 int ftime;			//controls Ion-drift. Steered by frate
 int ferrorCount;                //keep track of # of hits outside the volume
 double fdistPerTime;		//Ion-travel-distance each time-step
 double fgemGain;		//gain factor of the gem. Set by hand atm
 double fgemCharge;		//net charge produced at the gems
 double fWGas;			//obtained from PndTpcGas-Object

 bool fprimChargeOnly;          //no Charge is created at gems if true
 bool fALICEmode;
 
 std::vector< std::vector<int> > fchargeMap;	//segmented map of net charge
 
 PndTpcDigiPar* fpar;
 
 public:
  ClassDef(PndTpcSpaceChargeTask,1)
	
};

#endif
