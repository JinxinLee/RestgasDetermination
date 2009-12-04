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


//void setOpeningAngle(double);            //Opening angle of the thought cone
                                         //that contains the track 
	                                       //(not yet implemented)

void SetPersistence(Bool_t opt=kTRUE) {fpersistence=kTRUE;} 

void setXYresiduals(Bool_t xyres) {fXYresidual = xyres;}

void setSplineMeshKnots(unsigned int nZ, unsigned int nR) 
{fNknotsZ=nZ; fNknotsR=nR;}

void setLaserGridFile(std::string gridfile) {fgridFile=gridfile;}

void setPlotSplineFits(Bool_t opt) {fplot=opt;}


private:

 TString fclusterBranchName;
 
 std::string fgridFile;

 Bool_t fpersistence;            
 Bool_t fplot;                   
 Bool_t fXYresidual;              //flag for residual calculation
                                  //set true if z component should be ignored.

 //fitting parameters
 double fmaxDist;                 //max allowed distance at the ENDPLATE (z=109.5)
 double fopeningAngle;            //not implemented yet
 double frMin;                   
 double frMax;                   
 double fzMin;                   
 double fzMax;                    
 unsigned int fNknotsZ;           
 unsigned int fNknotsR;          
 unsigned int fNlamdaZ;          
 unsigned int fNlamdaR;           
 
 PndTpcLaserTask* flasertask;
 PndTpcCluster* cl;

 std::vector<PndTpcLaser*>* flaserList;            
 std::vector<PndTpcLaserTrack*>* ftrackList;       

 std::vector<double> fknotsZ;
 std::vector<double> fknotsR;  


 /*2D cubic Splines and associated data vectors for the fits */
 BiCubSpline* fdevMapR;
 BiCubSpline* fdevMapPerp;
 BiCubSpline* frecoMapR;
 BiCubSpline* frecoMapPerp;

 //reconstructed deviation maps: (dr, dperp) at (r,perp)
 std::vector<std::vector<double>*> fdevMapR_data;
 std::vector<std::vector<double>*> fdevMapPerp_data;
 //maps FOR reconstruction of MEASURED points: (dr, dperp) at (r+dr, perp+dperp)
 std::vector<std::vector<double>*> frecoMapR_data;     
 std::vector<std::vector<double>*> frecoMapPerp_data;
 
 
 TClonesArray* fclusterArray;
 TClonesArray* fstatArray;
 TClonesArray* ftrackArray;
 TClonesArray* ffitStatArray;

 void buildLaserList(bool quiet);   //creates the flaserList 
                                                
 PndTpcDigiPar* fpar;                 

 

public:
  ClassDef(PndTpcLaserFitTask,1)
	
};

#endif
