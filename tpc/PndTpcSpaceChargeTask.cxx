
//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcSpaceChargeTask
//     
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Cristoforo Simonetto  	TUM         (original author)
//	Felix Boehmer    	TUM
//
//-----------------------------------------------------------


// This Class' Header ------------------
#include "PndTpcSpaceChargeTask.h"

// Collaborating Class Headers --------
#include "PndTpcGas.h"
#include "TClonesArray.h"
#include "CbmRootManager.h"
#include "PndTpcPoint.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cmath>

// Class Member definitions -----------

PndTpcSpaceChargeTask::PndTpcSpaceChargeTask()		//default constructor
  : 	CbmTask("TPC Space Charge"),
	_rate(2e-2),
	_ionDriftVelocity(1.766e-6),
	_supression(1e-3),
	_tpcMinR(15.5),		//TODO: PARAMETER MANAGEMENT
	_tpcMaxR(41.5),         //these are all default values
	_tpcMinZ(-39.5),        //change with set-functions individually
	_tpcMaxZ(109.5),
	_rBinCount(26),
	_zBinCount(75),
	_angle(0.17453),
	_time(0),               
	_errorCount(0),
				//This should be received from PndTpcGem later, 
	_gemGain(4000.0),	//but PndTpcGem does not read it in itself atm
 
        _pointBranchName("PndTpcPoint")
{}

PndTpcSpaceChargeTask::~PndTpcSpaceChargeTask()
{}

InitStatus
PndTpcSpaceChargeTask::Init()
{

  _rBinWidth = ((_tpcMaxR - _tpcMinR) / _rBinCount);
  _zBinWidth = ((_tpcMaxZ - _tpcMinZ) / _zBinCount);

 //Get ROOT Manager
  CbmRootManager* ioman= CbmRootManager::Instance();
  
  if(ioman==0)
    {
      Error("PndTpcSpaceChargeTask::Init","RootManager not instantiated!");
      return kERROR;
    }

  // Get input collection
  _pointArray=(TClonesArray*) ioman->GetObject(_pointBranchName);
  
  if(_pointArray==0)
    {
      Error("PndTpcSpaceChargeTask::Init","MC-Point-array not found!");
      return kERROR;
    }

  _gas = new PndTpcGas("NEON-90_CO2-10_B2_PRES1013.asc",400);
  _WGas = _gas->W();
  delete _gas;
  _gemCharge = _supression * _gemGain;
  _distPerTime = _ionDriftVelocity / _rate; 	//Ion travel distance per event
  

// Initialize the charge-map (empty at the start...)
  for (int nr = 0; nr < _rBinCount; nr++)
	_chargeMap.push_back(std::vector<int>(_zBinCount, 0));

  return kSUCCESS;
}

void
PndTpcSpaceChargeTask::Exec(Option_t* opt)
{
  PndTpcPoint* currentPoint;
  int np = _pointArray->GetEntriesFast();	//number of MC points 
  
  for(int n=0; n<np; n++)			//Loop over all MC points
  {
    currentPoint = (PndTpcPoint*) _pointArray->At(n);
    unsigned int qPoint = (unsigned int)std::floor(currentPoint->GetEnergyLoss()*1e9 / _WGas); // [eV]
    
    double posX = currentPoint->GetX();
    double posY = currentPoint->GetY();
    double r = sqrt(posX*posX + posY*posY);

    int rBin = (int)std::floor(std::fabs(r - _tpcMinR) / _rBinWidth);
    int zBin = (int)std::floor(std::fabs(currentPoint->GetZ() - _tpcMinZ) / _zBinWidth);

    if(rBin > _rBinCount-1 || zBin > _zBinCount-1)
    {
	Error("PndTpcSpaceChargeTask::Exec", "Hit occured outside the PndTpc-Volume!");
	_errorCount++;
	continue;
    }

    //Now fill in the point-charge into the charge-map
    _chargeMap.at(rBin).at(zBin) += qPoint;

    //At the same x-y-coordinates Ions are created on the gem-plane by the 
    //impact of the drifted electrons (assuming instant electron drift) in 
    //the gems
    _chargeMap.at(rBin).at(0) += (int)(qPoint*_gemCharge);

  } 		//end Loop over all Points of this track/event

  _time++;	//ticks every event

  //Drift all the charge one bin in z-direction when enough "time" has passed
  //This takes a lot of time. Normally not used, instead done by a macro
  
  if( fmod((double)_time * _distPerTime, _zBinWidth) < _distPerTime)
  {
    std::cout<<"\n**** Drifting all the charge after  "<<_time<<" events ****"
             <<std::endl;

    for(int nr=0; nr<_rBinCount; nr++)
	for(int nz=_zBinCount-1; nz>0; nz--)
	   _chargeMap.at(nr).at(nz) = _chargeMap.at(nr).at(nz-1);

    for(int nr=0; nr<_rBinCount; nr++)
	_chargeMap.at(nr).at(0) = 0;
  }
  return;
}

void PndTpcSpaceChargeTask::writeToFile(const char* filename)
{  
  
  std::cout<< "\n\n**** Writing Charge Density to File " << filename 
           << " ****" << std::endl;
  std::cout<<"\nAll done. There have been "<<_errorCount<<" hits outside the volume."<<std::endl;
  double innerR, outerR, xy_area, volume;
  double pi = std::acos( -1.0 );
  double e_charge = 1.602176462e-19;

  //calculate the charge density from _chargeMap and geometry
  std::vector< std::vector <double> > _chargeDens;
  for (int nr=0; nr<_rBinCount; nr++)
    _chargeDens.push_back(std::vector<double>(_zBinCount, 0));

  for (int nr=0; nr<_rBinCount; nr++)
  {
    innerR = nr * _rBinWidth + _tpcMinR;	//inner radius of the bin
    outerR = (nr + 1) * _rBinWidth + _tpcMinR;
    xy_area = (pi - _angle) * (outerR*outerR - innerR*innerR);
    volume = xy_area*_zBinWidth;
    for (int nz=0; nz<_zBinCount; nz++)
	_chargeDens.at(nr).at(nz)=(double) _chargeMap.at(nr).at(nz)*e_charge/volume;                     //[C]
  }  
	
  //write file with 2D charge distribution density
 
  std::ofstream outfile(filename, std::fstream::out);
  outfile<<std::setprecision(6);
  outfile<<_tpcMinR<<" "<<_rBinWidth<<" "<<_rBinCount<<" "
	 <<_tpcMinZ<<" "<<_zBinWidth<<" "<<_zBinCount;
  //z runs first
  for (int nr=0; nr<_rBinCount; nr++)
    for (int nz=0; nz<_zBinCount; nz++)
      outfile<<"\n"<<_chargeDens.at(nr).at(nz);
  outfile<<std::endl;
  outfile.close();
}

ClassImp(PndTpcSpaceChargeTask)
