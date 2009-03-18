// -----------------------------------------------------------------
// $Id: 
//
// Description:
//      Driver Class for Runge-Kutta-Method
//      User specifies output binning/accuracy in r and z
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
class PndTpcEFieldCyl;
class FairField;
class PndTpcDigiPar;

class PndTpcInhFieldDrifter {

public:

  // ---------------- Constructors --------------------------------

  /*standard constructor */
  PndTpcInhFieldDrifter(PndTpcEFieldCyl* eField,
			PndMultiField* bField,
			const char* outFile,
			const char* paramFile,   /* "[...]/tpc/tpc.par" */
			const int rBins, 
			const int zBins,
			const int split=-1);     /* single bin in r to process */
  
  /*alternative version, using PndTpcEFieldCyl as E and B field internally */
  PndTpcInhFieldDrifter(const char* eFieldFile,
			const char* bFieldFile,
			const char* outFile,
			const char* paramFile,
			const int rBins,
			const int zBins,
			const int split=-1);
  
  ~PndTpcInhFieldDrifter();
  

  //---- Methods --------------------------------------------------------

  void run();	//start
  
  /*Easy way to set constant fields without changing input files */
  void setConstE(TVector3 cE) {_runKut->setConstE(cE);} 
  void setConstB(TVector3 cB) {_runKut->setConstB(cB);}


 
private:
 
  int    _rBins,
         _zBins,
         _split;  /*split the task for evaluation on cluster? 
		    -1: no split
		    any other int: number of row (in r) to process */
  
  double _tpcMinR, 
         _tpcMaxR,
         _tpcMinZ,
         _tpcMaxZ,
         _rBinWidth,
         _zBinWidth,
         _sc,
         _friction; 	

  const char* _outFile;
  const char* _paramFile;

  
  void writeToFile(const char* bla);
  void initParams();   //create parameter environment and read in
  
  
  PndTpcRungeKutta* _runKut;
  PndTpcDigiPar* _par;
  
  std::vector< std::vector<double> > _devX;        //x-deviation
  std::vector< std::vector<double> > _devY;        //y-deviation
  std::vector< std::vector<double> > _time;        //total RK-time
  std::vector< std::vector<int> > _stepCount;      //amount of RK-steps
  std::vector< std::vector<double> > _pathLength;  //total real drift-path 
  
  //control vector for the evolution of the drift velocity.
  //contains vectors of 3 components of the vector v and the Runge Kutta time
  std::vector<std::vector<double>*>* _velocity_control;
  
};

#endif

