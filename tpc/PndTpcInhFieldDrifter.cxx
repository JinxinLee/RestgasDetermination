//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcInhFieldDrifter
//     
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//     	Felix Boehmer           TUM         (original author)
//	
//-----------------------------------------------------------


// This Class' Header ------------------
#include "PndTpcInhFieldDrifter.h"

// Collaborating Class Headers --------

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <assert.h>
#include <sstream>
#include "PndTpcEFieldCyl.h"
#include "PndTpcGas.h"
#include "FairParAsciiFileIo.h"
#include "FairRunAna.h"
#include "FairParSet.h"
#include "FairRuntimeDb.h"
#include "PndTpcDigiPar.h"
#include "PndTpcRungeKutta.h"


// Class Member definitions -----------

PndTpcInhFieldDrifter::
PndTpcInhFieldDrifter(PndTpcEFieldCyl* eField,
		      PndMultiField* bField,
		      const char* outFile,
		      const char* paramFile,
		      const int rBins, const int zBins, 
		      const int split)
  :
	_outFile(outFile),
	_paramFile(paramFile),
	_rBins(rBins),
        _zBins(zBins),
	_split(split)
{
  if(_split >= 0 && split >= _rBins) {   //in the case split was manually set...
    std::cout << "PndTpcInhFieldDrifter::PndTpcInhFieldDrifter : "
	      <<"invalid row number requested! ABORTING"<<std::endl;
    throw 1;
  }
  
  //init parameters
  initParams();
  
  double vDrift = ((PndTpcGas*)_par->getGas())->VDrift();
  double eNomZ = (eField->nominal()).Z();
  _friction = 1e-5 * eNomZ / vDrift;  // friction term, see Rolandi Blum    
    
  _runKut = new PndTpcRungeKutta(1e-16, 1e-6, 1e-2, 1e-3,
			      _sc, eField,bField, _friction);
  
}


PndTpcInhFieldDrifter::
PndTpcInhFieldDrifter(const char* eFieldFile,
		      const char* bFieldFile,
		      const char* outFile,
		      const char* paramFile,
		      const int rBins, const int zBins, 
		      const int split)
  :
	_outFile(outFile),
	_paramFile(paramFile),
	_rBins(rBins),
        _zBins(zBins),
	_split(split)
{
  if(_split >= 0 && split >= _rBins) {   //in the case split was manually set...
    std::cout << "PndTpcInhFieldDrifter::PndTpcInhFieldDrifter : "
	      <<"invalid row number requested! ABORTING"<<std::endl;
    throw 1;
  }
  
  //init parameters
  initParams();
  
  double vDrift = ((PndTpcGas*)_par->getGas())->VDrift();
  
  PndTpcEFieldCyl* efield = new PndTpcEFieldCyl(eFieldFile);
  double eNomZ = (efield->nominal()).Z();
  _friction = 1e-5 * eNomZ / vDrift;  // friction term, see Rolandi Blum    
    
  _runKut = new PndTpcRungeKutta(1e-16, 1e-6, 1e-2, 1e-3,
			      _sc, eFieldFile,bFieldFile, _friction);
  delete efield;
}



PndTpcInhFieldDrifter::~PndTpcInhFieldDrifter()
{
  delete _runKut;
  int size = _velocity_control->size();
  for (int i=0; i<size; i++)
    delete _velocity_control->at(i);
  delete _velocity_control;
}


void 
PndTpcInhFieldDrifter::run()
{
  // Calculate the Binning
  _rBinWidth = (double)(_tpcMaxR - _tpcMinR) / _rBins;
  _zBinWidth = (double)(_tpcMaxZ - _tpcMinZ) / _zBins;

  // Init the fields
  
  //TODO: dangerous!!! change to pointers inside 
  for (int nr = 0; nr < _rBins; nr++)
  {
    _devX.push_back(std::vector<double>(_zBins, 0));
    _devY.push_back(std::vector<double>(_zBins, 0));
    _time.push_back(std::vector<double>(_zBins, 0));
    _stepCount.push_back(std::vector<int>(_zBins, 0));
    _pathLength.push_back(std::vector<double>(_zBins, 0));
    
  }

  _velocity_control = new std::vector<std::vector<double>*>;
  _velocity_control->push_back(new std::vector<double>(4, 0)); //first position

  std::cout << "\nCalculating the Deviation Map with Runge-Kutta "<<std::endl;
  int errorcount = 0;
  
  
  //Calculate the Deviation-Map with Runge Kutta
  for (int nr = 0; nr < _rBins; nr++)
  {
    if(_split != -1) {  //we want to split: only calculate one row in r
      if(nr != _split)
	continue;
    }
    for (int nz = 0; nz < _zBins; nz++)
    {
      double rPos, zPos, dx, dy, dz;

      rPos = (double)(_tpcMinR + (nr+0.5)*_rBinWidth)/100.; // [m]
      zPos = (double)(_tpcMinZ + (nz+0.5)*_zBinWidth)/100.; 

      // Create the speed/position-vector for RungeKutta
      double x[6]; // x[0-2] = speed, x[3-5] = pos. (SI)

      x[3] = rPos;  // == x coordinate. symmetry!
      x[4] = 0.;    // == y
      x[5] = zPos;
      x[0] = 0.;
      x[1] = 0.;
      x[2] = -0.0027314 * 1e7;

     
      double xCopy[6]; //store the starting position
      for (int n=0; n<=5; n++)
        xCopy[n] = x[n];
      
      _runKut->setTimeStep(1e-16);
      double time_temp;

      for (int i=0; ; i++)  // get serious
      {
        if (x[5]>(_tpcMaxZ)/100. || x[5]<(_tpcMinZ/100.))	   
	  break;   	// finished when we reach the "Pads"
	
	double xTemp[3]; //needed for the drift path monitoring
        for (int n=0; n<3; n++)
          xTemp[n] = x[n+3];

        for (int j=0; ; j++)	//one step of RungeKutta
	{
	  time_temp = _runKut->getTimeStep();//store BEFORE step 
	  if (_runKut->getTimeStep() == 0)
	    _runKut->setTimeStep(1e-16);
	  if (_runKut->stepForwards(x))  //try until successful
	    break;
	}
	_time.at(nr).at(nz) += time_temp;;
	_stepCount.at(nr).at(nz) ++;
	
        dx = x[3] - xTemp[0];
	dy = x[4] - xTemp[1]; //coordinate changes of the last RK step
        dz = x[5] - xTemp[2];
        _pathLength.at(nr).at(nz) += sqrt(dx*dx + dy*dy + dz*dz);
	
	//build the velocity-control array (in just one bin)
	if (nr == (int) std::floor((double)_rBins/2) 
	    && nz == (int) std::floor((double)_zBins/2))
	{
	  if ((_stepCount.at(nr).at(nz))%100 == 0) 
	  {
	    std::vector<double>* temp_vec = new std::vector<double>(4);
	    for(int i = 0; i <= 2; i++)
	      temp_vec->at(i) = x[i];    //fill vector with components of V
	    temp_vec->at(3) = _runKut->getTimeStep();
	    _velocity_control->push_back(temp_vec);
	
	  }
	}  
      }
      

      if(x[3]==xCopy[3] && x[4]==xCopy[4])
        errorcount++;

      // Now fill the 2 deviation fields with the values
      _devX.at(nr).at(nz) = (x[3]-xCopy[3]) * 100;    //[cm]
      _devY.at(nr).at(nz) = (x[4]-xCopy[4]) * 100;
    }
  std::cout<<"* "<<std::endl;
  } 	// Finish Loop over all bins

  std::cout << "\n\nAll done! \nThere have been " << errorcount 
       << " unprocessed bins..." << std::endl;

  writeToFile(_outFile);
}

void
PndTpcInhFieldDrifter::writeToFile(const char* out_file)
{
  std::cout << "\n\n**** Writing Deviation Data to File " << out_file 
	    << " ****" << std::endl;

  std::string filename(out_file);

  if(_split >= 0) {
    std::string s;
    std::stringstream stream;
    stream << _split;
    stream >> s;
    filename += s;
  }
  
  std::ofstream outfile(filename.c_str(), std::fstream::out);

  if(_split == 0 || _split == -1) {
    outfile<<std::setprecision(6);
    outfile<<_rBins<<"   "<<_zBins<<"   "<<_tpcMinR<<"   "<<_tpcMaxR
	   <<"   "<<_tpcMinZ<<"   "<<_tpcMaxZ<<std::endl;
  }

  for (int nr=0; nr<_rBins; nr++)
  {
    if (_split>=0) {
      if (nr != _split)
	continue;
    }
      
    for (int nz=0; nz<_zBins; nz++) 
    {
      outfile<<std::setprecision(8)<<std::setw(8);
      outfile<<_devX.at(nr).at(nz)<<"   "
	     <<_devY.at(nr).at(nz)<<"   "<<_time.at(nr).at(nz)*1e9<<"   " 
	     <<_stepCount.at(nr).at(nz)<<"   "<<_pathLength.at(nr).at(nz)*100
	     <<std::endl;
    }
  }

  if(_split == _rBins-1 || _split == -1) {
  outfile<<std::endl;
  outfile<<"\n\n#Format: First Line - Bins in r, Bins in z, minR, maxR, minZ, maxZ" 
	 <<"\n#Each Following Line: x-deviation [cm], y-deviation, total Runge-Kutta-Time [ns],"
         <<"\n#number of Runge-Kutta steps, real total RK-drift path [cm]" 
 	 <<"\n#This File was created by class PndTpcInhFieldDrifter"
	 <<std::endl;
  }

  outfile.close();

//   std::ofstream outfile2("DEVMAP_VELOCITY_FILE.dat", std::fstream::out);
//   outfile<<std::setprecision(8);

//   long int length = _velocity_control->size();

//   for (int a=0; a<length; a++)
//   {
//     outfile2<<std::setprecision(8)<<std::setw(8);
//     outfile2<<"\n"<<_velocity_control->at(a)->at(0)*1e-7<<"   "
//             <<_velocity_control->at(a)->at(1)*1e-7<<"   "
//             <<_velocity_control->at(a)->at(2)*1e-7<<"   "
//             <<_velocity_control->at(a)->at(3)*1e9;
//   }
//   outfile2<<std::endl;
//   outfile2<<"#Format: v_x, v_y, v_z, corresponding Runge-Kutta-time"<<std::endl;
//   outfile2.close();
}
    


void
PndTpcInhFieldDrifter::initParams() {
  
  FairParAsciiFileIo* input = new FairParAsciiFileIo();
  
  //ugly test for valid param file
  std::ifstream paramf(_paramFile, std::fstream::in);
  if(!paramf.good()) {
    std::cout<<"PndTpcInhFieldDrifter::initParams() "
	     <<"- parameter file IO error - ABORTING"<<std::endl;
    throw 1;
  }
    
  input->open(_paramFile, "in");
  
  //create dummy run manager and load database
  FairRunAna* fRun = new FairRunAna();   
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();  
  rtdb->setFirstInput(input);
  _par = (PndTpcDigiPar*) rtdb->getContainer("PndTpcDigiPar");
  
  _par->FairParSet::init();
  
  //read in parameters
  _tpcMinR = _par->getRMin();
  _tpcMaxR = _par->getRMax();
  _tpcMinZ = _par->getZGem();
  _tpcMaxZ = _par->getZMax();

  std::cout<<"\n\nPndTpcInhFieldDrifter::initParams()\n"
	   <<"---------- Used Parameters ---------------------------"<<std::endl;
  std::cout<<"MinR: "<<_tpcMinR<<",   MaxR: "<<_tpcMaxR<<std::endl;
  std::cout<<"MinZ: "<<_tpcMinZ<<",   MaxZ: "<<_tpcMaxZ<<std::endl;std::cout<<std::endl;
  std::cout<<"Bins in R: "<<_rBins<<",   Bins in Z: "<<_zBins<<std::endl;
  
  std::cout<<"------------------------------------------------------"<<std::endl;
  std::cout<<std::endl;  
  _sc = -1e-6*299792458*299792458/0.510998902;

  
}
