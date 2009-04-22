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
#include "BiCubSpline.h"
#include <vector>
#include <string>

// Collaborating Class Declarations --------------------------------------

class PndTpcLaser;
class PndTpcLaserTrack;
class PndTpcLaserStat;
class PndTpcLaserTask;
class PndTpcCluster;
class PndTpcDigiPar;


class PndTpcLaserFitTask : public FairTask {

public:

// Constructor -----------------------------------------------------------

PndTpcLaserFitTask();			//default constructor
~PndTpcLaserFitTask();

virtual InitStatus Init();		//needs to be overidden as usual
virtual void Exec(Option_t* opt);	//...

virtual void SetParContainers();

// Modifiers -------------------------------------------------------------


void setOpeningAngle(double);            //Opening angle of the thought cone
                                         //that contains the track 
	                                       //(not yet implemented)

void SetPersistence(Bool_t opt=kTRUE) {_persistence=kTRUE;} 

void setXYresiduals(Bool_t xyres) {_XYresidual = xyres;}

void setSplineMeshKnots(unsigned int nZ, unsigned int nR) 
{_NknotsZ=nZ; _NknotsR=nR;}

void setLaserGridFile(std::string gridfile) {_gridFile=gridfile;}

void setPlotSplineFits(Bool_t opt) {_plot=opt;}


private:

 TString _clusterBranchName;
 
 std::string _gridFile;

 Bool_t _persistence;            
 Bool_t _plot;                   
 Bool_t _XYresidual;              //flag for residual calculation
                                  //set true if z component should be ignored.

 //fitting parameters
 double _maxDist;                 //max allowed distance at the ENDPLATE (z=109.5)
 double _openingAngle;            //not implemented
 double _rMin;                   
 double _rMax;                   
 double _zMin;                   
 double _zMax;                   
 unsigned int _NknotsZ;         
 unsigned int _NknotsR;          
 unsigned int _NlamdaZ;          
 unsigned int _NlamdaR;           
 
 PndTpcLaserTask* _lasertask;
 PndTpcCluster* cl;

 std::vector<PndTpcLaser*>* _laserList;            
 std::vector<PndTpcLaserTrack*>* _trackList;       

 std::vector<double> _knotsZ;
 std::vector<double> _knotsR;  


 /*2D cubic Splines and associated data vectors for the fits */
 BiCubSpline* _devMapR;
 BiCubSpline* _devMapPerp;
 BiCubSpline* _recoMapR;
 BiCubSpline* _recoMapPerp;

 //reconstructed deviation maps: (dr, dperp) at (r,perp)
 std::vector<std::vector<double>*> _devMapR_data;
 std::vector<std::vector<double>*> _devMapPerp_data;
 //maps FOR reconstruction of MEASURED points: (dr, dperp) at (r+dr, perp+dperp)
 std::vector<std::vector<double>*> _recoMapR_data;     
 std::vector<std::vector<double>*> _recoMapPerp_data;
 
 
 TClonesArray* _clusterArray;
 TClonesArray* _statArray;
 TClonesArray* _trackArray;
 TClonesArray* _fitStatArray;

 void buildLaserList(bool quiet);   //creates the _laserList 
                                                
 PndTpcDigiPar* _par;                 

 

public:
  ClassDef(PndTpcLaserFitTask,1)
	
};

#endif
