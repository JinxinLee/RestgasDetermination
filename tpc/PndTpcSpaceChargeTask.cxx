
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
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "PndTpcPoint.h"
#include "PndTpcDigiPar.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cmath>

// Class Member definitions -----------

PndTpcSpaceChargeTask::PndTpcSpaceChargeTask()		//default constructor
  : 	FairTask("TPC Space Charge"),
	//ftpcMinR(15.5),	
	//ftpcMaxR(41.5),         //these are all default values
	//ftpcMinZ(-39.5),        //change with set-functions individually
	//ftpcMaxZ(109.5),
	frBinCount(26),
	fzBinCount(75),
	fangle(0.17453),              //TODO: parameter management!
	fionDriftVelocity(1.766e-6),  // [cm/ns]
	ftime(0),               
	ferrorCount(0),
	fprimChargeOnly(false),
	fALICEmode(false),	 
        fpointBranchName("PndTpcPoint")
{}

PndTpcSpaceChargeTask::~PndTpcSpaceChargeTask()
{}

InitStatus
PndTpcSpaceChargeTask::Init()
{

  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  if(ioman==0)
    {
      Error("PndTpcSpaceChargeTask::Init","RootManager not instantiated!");
      return kERROR;
    }

  // Get input collection
  fpointArray=(TClonesArray*) ioman->GetObject(fpointBranchName);
  
  if(fpointArray==0)
    {
      Error("PndTpcSpaceChargeTask::Init","MC-Point-array not found!");
      return kERROR;
    }

  fsupression=fpar->getSupression();
  ftpcMaxR=fpar->getRMax();  //make sure these are the values for the ACTIVE detector region  
  ftpcMinR=fpar->getRMin();  //see geometry/tpc.geo      
  ftpcMaxZ=fpar->getZMax();  
  ftpcMinZ=fpar->getZGem();    
  			
  fgemGain=fpar->getGain();
  frate=fpar->getRate();              

  fgas=fpar->getGas();
  fWGas = fgas->W();
 
  fgemCharge = fsupression * fgemGain;
  fdistPerTime = fionDriftVelocity / frate; 	//Ion travel distance per event

  frBinWidth = ((ftpcMaxR - ftpcMinR) / frBinCount);
  fzBinWidth = ((ftpcMaxZ - ftpcMinZ) / fzBinCount);

  std::cout<<"\n\n--------- PndTpcSpaceChargeTask working parameters ----------"<<std::endl;
  std::cout<<"\nSupression: "<<fsupression<<";   Gain: "<<fgemGain
	   <<";   Rate: "<<frate<<"   WGas: "<<fWGas<<std::endl;
  std::cout<<"Geometry:\n"<<"MinR: "<<ftpcMinR<<";   MaxR: "<<ftpcMaxR<<std::endl;
  std::cout<<"MinZ: "<<ftpcMinZ<<";   MaxZ: "<<ftpcMaxZ<<std::endl;
  std::cout<<"-------------------------------------------------------------\n\n"<<std::endl;
  std::cout.flush();  
  

// Initialize the charge-map (empty at the start...)
  for (int nr = 0; nr < frBinCount; nr++)
	fchargeMap.push_back(std::vector<int>(fzBinCount, 0));  

  return kSUCCESS;
}

void 
PndTpcSpaceChargeTask::SetParContainers() {

  std::cout<<"PndTpcSpaceChargeTask::SetParContainers"<<std::endl;
  std::cout.flush();

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get PndTpc digitisation parameter container
  fpar= (PndTpcDigiPar*) db->getContainer("PndTpcDigiPar");
  if (! fpar ) Fatal("SetParContainers", "PndTpcDigiPar not found");
}

void
PndTpcSpaceChargeTask::Exec(Option_t* opt)
{
  //only needed for ALICE mode
  const Float_t poti = 20.77e-9; // first ionization potential for Ne/CO2
  const Float_t w_ion = 35.97e-9; // energy for the ion-electron pair creation 

  PndTpcPoint* currentPoint;
  int np = fpointArray->GetEntriesFast();	//number of MC points 

  if(ftime%500==0) {
    std::cout<<". ";
    std::cout.flush();
  }
  
  for(int n=0; n<np; n++)			//Loop over all MC points
  {
    currentPoint = (PndTpcPoint*) fpointArray->At(n);
    int qPoint=0;
    
    //misleading - has nothing to do with G3 "ALICE", just different method
    //TODO: make sure the DETECTOR class works right
    if(fALICEmode) {
      if(n==0 && time==0)
	std::cout<<"\n------- USING ALICE CHARGE CONVERSION MODE --------------------------"<<std::endl;
      if((currentPoint->GetEnergyLoss())-poti > 0.)
	qPoint = (int)(((currentPoint->GetEnergyLoss())-poti)/w_ion) + 1;	//imported from ALICE
      //qPoint = TMath::Min(qPoint,300); // 300 electrons corresponds to 10 keV ??
    }
    else
      qPoint = (int)(currentPoint->GetEnergyLoss()*1e9 / fWGas); // [eV]
    
    double posX = currentPoint->GetX();
    double posY = currentPoint->GetY();
    double r = sqrt(posX*posX + posY*posY);

    int rBin = (int)(std::fabs(r - ftpcMinR) / frBinWidth);
    int zBin = (int)(std::fabs(currentPoint->GetZ() - ftpcMinZ) / fzBinWidth);

    if(rBin >= frBinCount || zBin >= fzBinCount)
    {
      //Error("PndTpcSpaceChargeTask::Exec", "Hit occured outside the PndTpc-Volume!");
      ferrorCount++;
      continue;
    }

    //Now fill in the point-charge into the charge-map
    fchargeMap.at(rBin).at(zBin) += qPoint;

    //At the same x-y-coordinates Ions are created on the gem-plane by the 
    //impact of the drifted electrons (assuming instant electron drift) in 
    //the gems
    if(!fprimChargeOnly)
      fchargeMap.at(rBin).at(0) += (int)(qPoint*fgemCharge);

  } 		//end Loop over all Points of this track/event

  ftime++;	//ticks every event

  //Drift all the charge one bin in z-direction when enough "time" has passed
  //This takes a lot of time. Normally not used, instead done by a macro
  
  if( fmod((double)ftime * fdistPerTime, fzBinWidth) < fdistPerTime)
  {
    std::cout<<"\n**** Drifting all the charge after  "<<ftime<<" events ****"
             <<std::endl;

    for(int nr=0; nr<frBinCount; nr++)
	for(int nz=fzBinCount-1; nz>0; nz--)
	   fchargeMap.at(nr).at(nz) = fchargeMap.at(nr).at(nz-1);

    for(int nr=0; nr<frBinCount; nr++)
	fchargeMap.at(nr).at(0) = 0;
  }
  return;
}

void PndTpcSpaceChargeTask::writeToFile(const char* filename)
{  
  
  std::cout<< "\n\n**** Writing Charge Density to File " << filename 
           << " ****" << std::endl;
  std::cout<<"\nAll done. There have been "<<ferrorCount<<" hits outside the volume."<<std::endl;
  double innerR, outerR, xy_area, volume;
  double pi = std::acos( -1.0 );
  double e_charge = 1.602176462e-19;

  //calculate the charge density from fchargeMap and geometry
  std::vector< std::vector <double> > fchargeDens;
  for (int nr=0; nr<frBinCount; nr++)
    fchargeDens.push_back(std::vector<double>(fzBinCount, 0));

  for (int nr=0; nr<frBinCount; nr++)
  {
    innerR = nr * frBinWidth + ftpcMinR;	//inner radius of the bin
    outerR = (nr + 1) * frBinWidth + ftpcMinR;
    xy_area = (pi - fangle) * (outerR*outerR - innerR*innerR);
    volume = xy_area*fzBinWidth;
    for (int nz=0; nz<fzBinCount; nz++)
	fchargeDens.at(nr).at(nz)=(double) fchargeMap.at(nr).at(nz)*e_charge/volume;                     //[C]
  }  
	
  //write file with 2D charge distribution density
 
  std::ofstream outfile(filename, std::fstream::out);
  outfile<<std::setprecision(6);
  outfile<<ftpcMinR<<" "<<frBinWidth<<" "<<frBinCount<<" "
	 <<ftpcMinZ<<" "<<fzBinWidth<<" "<<fzBinCount;
  //z runs first
  for (int nr=0; nr<frBinCount; nr++)
    for (int nz=0; nz<fzBinCount; nz++)
      outfile<<"\n"<<fchargeDens.at(nr).at(nz);
  outfile<<std::endl;
  outfile.close();
}

ClassImp(PndTpcSpaceChargeTask)
