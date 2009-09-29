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
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "TClonesArray.h"
#include "PndTpcPrimaryCluster.h"
#include "PndTpcLaser.h"
#include "TVector3.h"
#include "TError.h"
#include "PndTpcDigiPar.h"
#include <iostream>
#include <fstream>
#include <string>


PndTpcLaserTask::PndTpcLaserTask()
  : FairTask("TPC Laser")
{
  flaserList = new std::vector<PndTpcLaser*>;
  fis_end = false;
  addCounter = 0;
  NEcount = 0;
}

PndTpcLaserTask::~PndTpcLaserTask()
{
  for(unsigned int j=0;j<flaserList->size();j++)
    delete (PndTpcLaser*)flaserList->at(j);
  delete flaserList;
  delete fcurrentLaser;
  delete flaserArray;
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
  fprimArray=(TClonesArray*) ioman->GetObject("PndTpcPrimaryCluster");
  
  if(fprimArray==0)
    {
      Error("PndTpcLaserTask::Init","PrimaryElectron-array not found!");
      return kERROR;
    }

  //clear input array of any real "physical" hits
  fprimArray->Delete();

   
  //read in parameters
  fzMin=fpar->getZGem();
  fzMax=fpar->getZMax();
  frMin=fpar->getRMin();
  frMax=fpar->getRMax();
  
  return kSUCCESS;
}



void 
PndTpcLaserTask::SetParContainers() {

  std::cout<<"PndTpcLaserTask::SetParContainers"<<std::endl;
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
PndTpcLaserTask::Exec(Option_t* opt)
{
  // flaserArray = new TClonesArray("PndTpcPrimaryCluster"); 
  int size = flaserList->size(); //comes from "readFromFile()"
 
  for(int i=0;i<size;i++)
  {
    fcurrentLaser = flaserList->at(i);
    fcurrentLaser->activate();
    flaserArray = fcurrentLaser->getTrack();
    if(fcurrentLaser->getNE()>0)
      NEcount++;
    Int_t nPrim = fprimArray->GetEntriesFast();
    Int_t nLaser = flaserArray->GetEntriesFast();
    PndTpcPrimaryCluster* test = (PndTpcPrimaryCluster*) fprimArray->At(0);
    //unsigned int trackID = test->mcTrackId();
    //unsigned int hitID = test->mcHitId();
    if(nLaser>0)
      addCounter++;
    
    for(Int_t j=0; j<nLaser; j++)
    { 
      PndTpcPrimaryCluster* cl=(PndTpcPrimaryCluster*) flaserArray->At(j);
         
      if(cl!=NULL){
	TVector3 cl_pos = cl->pos();
	if(cl_pos.Perp()<frMin || cl_pos.Perp()>frMax)
	  continue;
	if(cl_pos.Z()<fzMin || cl_pos.Z()>fzMax)
	  continue;   
	
	//new((*fprimArray)[fprimArray->GetEntriesFast()]) PndTpcPrimaryCluster(cl->t(),cl->q(),
	//					 cl->pos(),trackID,hitID);
	new((*fprimArray)[fprimArray->GetEntriesFast()]) PndTpcPrimaryCluster(cl->t(),cl->q(),
							     cl->pos(),1,1);
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
  flaserList->push_back(new PndTpcLaser(start,direction,iondens,width,time,is_end));
}

void
PndTpcLaserTask::setLaserFile(const char* filename, bool quiet)
{
  ffilename = filename;
  readFromFile(quiet);
}

void
PndTpcLaserTask::readFromFile(bool quiet) //ugly, needs a shapeup
{
  std::ifstream infile(ffilename, std::fstream::in);
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
	strncpy(fdir_or_end, line+1,12);
	if(strcmp(fdir_or_end,"direction")==0){
	  std::cout<<"\n\nPndTpcLaserTask::readFromFile : Found string after marker: "
		   <<fdir_or_end<<std::endl;
	  init=true;
	  continue;
	}
	if(strcmp(fdir_or_end,"end")==0){
	  std::cout<<"\n\nPndTpcLaserTask::readFromFile : Found string after marker: "
		   <<fdir_or_end<<std::endl;
	  fis_end=true;
	  init=true;
	  continue;
	}
	Fatal("PndTpcLaserTask::readFromFile", "Invalid string after '!' marker");
      } 
    }//end of initialisation

    //the standard read-in
    infile>>fstartX>>fstartY>>fstartZ>>fdirX>>fdirY>>fdirZ>>fiondens>>fwidth
	  >>ftime;
    PndTpcLaser* laser = new PndTpcLaser(TVector3(fstartX,fstartY,fstartZ),
				    TVector3(fdirX,fdirY,fdirZ),
				    fiondens,fwidth,ftime,fis_end);
    flaserList->push_back(laser);
    if(!quiet){
      std::cout<<"\nAdded Laser:";
      laser->print();
    }
  }
  infile.close();
}
    
ClassImp(PndTpcLaserTask)



