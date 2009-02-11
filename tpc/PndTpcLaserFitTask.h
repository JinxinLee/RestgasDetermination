//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Simple Fitting Task for the Laser Calibration System   
//	
//      
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer           TUM      (original author)  
//
//-----------------------------------------------------------

#ifndef TPCLASERFITTASK_H
#define TPCLASERFITTASK_H

//Base class
#include "FairTask.h"

#include "TClonesArray.h"
#include "TString.h"
#include <vector>

// Collaborating Class Declarations --------------------------------------

class PndTpcLaser;
class PndTpcLaserTrack;
class PndTpcLaserStat;
class PndTpcLaserTask;
class PndTpcCluster;


class PndTpcLaserFitTask : public FairTask {

public:

// Constructor -----------------------------------------------------------

PndTpcLaserFitTask();			//default constructor
~PndTpcLaserFitTask();

virtual InitStatus Init();		//needs to be overidden as usual
virtual void Exec(Option_t* opt);	//...

// Modifiers -------------------------------------------------------------

//TODO: implement parameter management
void setOpeningAngle(double);            //Opening angle of the thought cone
                                         //that contains the track 
	                                       //(not yet implemented)
void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}

void setXYresiduals(Bool_t xyres) {_XYresidual = xyres;}

void buildLaserList(const char*, bool quiet);        //creates the _laserList 
                                         //needs to be called before Exec()! 



private:

 TString _clusterBranchName;
 Bool_t _persistence;


 Bool_t _XYresidual;            // flag for residual calculation
                                // set true if z component should be ignored.

 //fitting parameters
 double _maxDist;
 double _openingAngle;          // not implemented
 double _rMin;
 double _rMax;
 double _PI;

 PndTpcLaserTask* _lasertask;
 PndTpcCluster* cl;

 std::vector<PndTpcLaser*>* _laserList;
 std::vector<PndTpcLaserTrack*>* _trackList;

 TClonesArray* _clusterArray;
 TClonesArray* _statArray;
 TClonesArray* _trackArray;

public:
  ClassDef(PndTpcLaserFitTask,1)
	
};

#endif
