//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcLaserTask
//      see PndTpcLaserTask.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer        TUM            (original author)
//
//
//-----------------------------------------------------------


// This Class' Header ---------------------------------------
#include "PndTpcLaserTask.h"

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "PndTpcPrimaryCluster.h"
#include "PndTpcLaser.h"
#include "TVector3.h"
#include "TError.h"
#include <iostream>
#include <fstream>
#include <string>


PndTpcLaserTask::PndTpcLaserTask()
  : FairTask("TPC Laser")
{
  _laserList = new std::vector<PndTpcLaser*>;
  _is_end = false;
  addCounter = 0;
  NEcount = 0;
}

PndTpcLaserTask::~PndTpcLaserTask()
{
  for(unsigned int j=0;j<_laserList->size();j++)
    delete (PndTpcLaser*)_laserList->at(j);
  delete _laserList;
  delete _currentLaser;
  delete _laserArray;
}

InitStatus
PndTpcLaserTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcLaserTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  // Get input collection
  _primArray=(TClonesArray*) ioman->GetObject("PndTpcPrimaryCluster");
  
  if(_primArray==0)
    {
      Error("PndTpcDriftTask::Init","PrimaryElectron-array not found!");
      return kERROR;
    }
  return kSUCCESS;
}

void
PndTpcLaserTask::Exec(Option_t* opt)
{
  // _laserArray = new TClonesArray("PndTpcPrimaryCluster"); 
  int size = _laserList->size(); //comes from "readFromFile()"
 
  for(int i=0;i<size;i++)
  {
    _currentLaser = _laserList->at(i);
    _currentLaser->activate();
    _laserArray = _currentLaser->getTrack();
    if(_currentLaser->getNE()>0)
      NEcount++;
    Int_t nPrim = _primArray->GetEntriesFast();
    Int_t nLaser = _laserArray->GetEntriesFast();
    PndTpcPrimaryCluster* test = (PndTpcPrimaryCluster*) _primArray->At(0);
    unsigned int trackID = test->mcTrackId();
    unsigned int hitID = test->mcHitId();
    if(nLaser>0)
      addCounter++;
    
    for(Int_t j=0; j<nLaser; j++)
    { 
      PndTpcPrimaryCluster* cl=(PndTpcPrimaryCluster*) _laserArray->At(j);
      if(cl!=NULL){
	new((*_primArray)[_primArray->GetEntriesFast()]) PndTpcPrimaryCluster(cl->t(),cl->q(),
							 cl->pos(),trackID,hitID);
      }
    }
  }
  std::cout<<"\n***LaserList had "<<size<<" entries***"<<std::endl;
  std::cout.flush();
  std::cout<<NEcount<<" Lasers initialized with NEcount > 0"<<std::endl; 
  std::cout<<addCounter<<" Laser-Arrays have been added.\n"<<std::endl;
  std::cout.flush();
  
  return;
}

void
PndTpcLaserTask::addLaser(double startX,double startY, double startZ,
		       double dirX, double dirY, double dirZ,
		       double iondens, double width, double time, bool is_end)
{
  TVector3 start = TVector3(startX, startY, startZ);
  TVector3 direction = TVector3(dirX, dirY, dirZ);
  _laserList->push_back(new PndTpcLaser(start,direction,iondens,width,time,is_end));
}

void
PndTpcLaserTask::setLaserFile(const char* filename, bool quiet)
{
  _filename = filename;
  readFromFile(quiet);
}

void
PndTpcLaserTask::readFromFile(bool quiet) //ugly, needs a shapeup
{
  std::ifstream infile(_filename, std::fstream::in);
  if (!infile.good()) 
  {
    Fatal("PndTpcLaserTask::readFromFile", "Laser-File not found!");
    return;
  }   
  bool init = false;
  while (infile.good())
  {
    //initialisation
    while (infile.good() && !init)
    {
      char line[256];
      infile.getline(line, 256);
      //ignore blank lines
      if(strcmp(line, "\0") == 0){
	//std::cout<<"\nIgnoring blank line"<<std::endl;
	  continue;}
      //ignore comments
      char* comment = strchr(line, '#');
      if (comment != 0)
	continue;
	
      //find the line that tells us if we use dir or end values
      char* marker = strchr(line,'!');
      if (marker != 0)
      {
	strncpy(_dir_or_end, line+1,12);
	if(strcmp(_dir_or_end,"direction")==0){
	  std::cout<<"\n\nPndTpcLaserTask::readFromFile : Found string after marker: "
		   <<_dir_or_end<<std::endl;
	  init=true;
	  continue;
	}
	if(strcmp(_dir_or_end,"end")==0){
	  std::cout<<"\n\nPndTpcLaserTask::readFromFile : Found string after marker: "
		   <<_dir_or_end<<std::endl;
	  _is_end=true;
	  init=true;
	  continue;
	}
	Fatal("PndTpcLaserTask::readFromFile", "Invalid string after '!' marker");
      } 
    }//end of initialisation

    //the standard read-in
    infile>>_startX>>_startY>>_startZ>>_dirX>>_dirY>>_dirZ>>_iondens>>_width
	  >>_time;
    PndTpcLaser* laser = new PndTpcLaser(TVector3(_startX,_startY,_startZ),
				    TVector3(_dirX,_dirY,_dirZ),
				    _iondens,_width,_time,_is_end);
    _laserList->push_back(laser);
    if(!quiet){
      std::cout<<"\nAdded Laser:";
      laser->print();
    }
  }
  infile.close();
}
    
ClassImp(PndTpcLaserTask)



