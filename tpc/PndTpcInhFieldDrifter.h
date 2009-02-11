// -----------------------------------------------------------------
// $Id: 
//
// Description:
//      Driver Class for the Runge-Kutta-Method
//      User specifies binning/accuracy in r and z
//	Output is the deviation map file
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer             TUM           (original author)
//
// -----------------------------------------------------------------


#ifndef TPCINHFIELDDRIFTER_H
#define TPCINHFIELDDRIFTER_H

#include <vector>
#include <string>
#include "TVector3.h"

#include "PndTpcRungeKutta.h" 

class PndFieldMap;
class PndMultiField;
class FairField;

class PndTpcInhFieldDrifter {

public:

PndTpcInhFieldDrifter(const char* eFieldFile,
		      //PndMultiField* bField
		      const char*  bFieldFile,
		      std::string outFile,
		      int rBins, int zBins,
		      int split=-1);

~PndTpcInhFieldDrifter();

void run();	//starts the thing

private:

int    _rBins,
       _zBins,
       _split;  //split the task for evaluation on cluster?

double _tpcMinR, 
       _tpcMaxR,
       _tpcMinZ,
       _tpcMaxZ,
       _rBinWidth,
       _zBinWidth,
       _sc,
       _friction; 	

std::string _outFile;

void writeToFile(std::string);

PndTpcRungeKutta* _runKut;

std::vector< std::vector<double> > _devX;        //x-deviation
std::vector< std::vector<double> > _devY;        //y-deviation
std::vector< std::vector<double> > _time;     //total RK-time
std::vector< std::vector<int> > _stepCount;      //amount of RK-steps
std::vector< std::vector<double> > _pathLength;  //total real drift-path 

//control vector for the evolution of the drift velocity.
//contains vectors of 3 components of the vector v and the Runge Kutta time
std::vector<std::vector<double>*>* _velocity_control;

};

#endif

