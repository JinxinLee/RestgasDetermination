//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcLaserFitTask
//     
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//	Felix Boehmer    	TUM     (original author)
//
//-----------------------------------------------------------


// This Class' Header ------------------
#include "PndTpcLaserFitTask.h"


// C++ headers
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "assert.h"

// Collaborating Class Headers --------
#include "TClonesArray.h"
#include "FairRootManager.h"
#include "TVector3.h"
#include "PndTpcLaser.h"
#include "PndTpcLaserTrack.h"
#include "PndTpcLaserTask.h"
#include "PndTpcLaserStat.h"
#include "PndTpcCluster.h"


// Class Member definitions -----------

PndTpcLaserFitTask::PndTpcLaserFitTask()		//default constructor
  : FairTask("TPC Laser Fit"),
    _persistence(kFALSE),	
    _XYresidual(false),
    _maxDist(1.0),
    _openingAngle(0),
    _rMin(15.5),
    _rMax(41.5),           //TODO: implement parameter management
    _PI(std::acos( -1.0 ))
{
  _clusterBranchName = "PndTpcCluster";
}

PndTpcLaserFitTask::~PndTpcLaserFitTask()
{
delete _lasertask;
}

InitStatus
PndTpcLaserFitTask::Init()
{

 //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  if(ioman==0)
    {
      Error("PndTpcLaserFitTask::Init","RootManager not instantiated!");
      return kERROR;
    }

  // Get input collection
  _clusterArray=(TClonesArray*) ioman->GetObject(_clusterBranchName);
  
  if(_clusterArray==0)
    {
      Error("PndTpcLaserFitTask::Init","PndTpcCluster-Array not found!");
      return kERROR;
    }
  
 // create and register output array
  _statArray = new TClonesArray("PndTpcLaserStat"); 
  ioman->Register("PndTpcLaserStat","PndTpc",_statArray,_persistence);
  _trackArray = new TClonesArray("PndTpcLaserTrack"); 
  ioman->Register("PndTpcLaserTrack","PndTpc",_trackArray,_persistence);
   
  return kSUCCESS;
}

void
PndTpcLaserFitTask::buildLaserList(const char* filename, bool quiet)
{
  _lasertask = new PndTpcLaserTask();
  _lasertask->setLaserFile(filename, quiet);
  _laserList = _lasertask->getLaserList();
  if (_laserList!=0)
    std::cout<<"\n*** PndTpcLaserFitTask::sucessfully built Laser-Geometry from Grid-map ***"<<std::endl;
}
  

//TODO: redesign. This class should resolve error calculation and write into
//PndTpcLaserStat objects. Error in z has to be taken into account!!!

void
PndTpcLaserFitTask::Exec(Option_t* opt)
{
  // loop over all LaserTracks and look for corresponding clusters.
  unsigned int Nlasers = _laserList->size();
  unsigned int Nclusters = _clusterArray->GetEntriesFast();
  
  TVector3 pP;     //projected point on the theoretical laser track
  TVector3 res;    //residual
  TVector3 dist;  

  unsigned int ncl=_clusterArray->GetEntriesFast();

  for(unsigned int ilaser=0; ilaser<Nlasers; ilaser++)
  {
    PndTpcLaser* theLaser = _laserList->at(ilaser);
    PndTpcLaserTrack* theTrk = new PndTpcLaserTrack(ilaser,
						    theLaser->getStart(),
						    theLaser->getDir());

    unsigned int counter = 0;
      
    //loop over all clusters, find matching clusters in the cone
    for (unsigned int iclust=0; iclust<ncl; iclust++)
    {
      cl =(PndTpcCluster*)_clusterArray->At(iclust);
            
      //effective radius from laser starting point
      
      //NEEDS MORE THINKING: (_rMax-_rMin) only legal if the track lies in 
      //the x-y-plane
      //double rEff = (_rMax-_rMin) - (cl->pos().Mag());
      
      //calculate geometry and projection point 
      TVector3 start=theLaser->getStart();
      TVector3 end=theLaser->getEnd();
      TVector3 dir=theLaser->getDir();
      double length=(start-end).Mag();
      TVector3 clpos = cl->pos();
      TVector3 clvec = clpos - start;
      TVector3 clerr = cl->sig();
      
      double t = (clpos-start)*dir;
      
      pP = dir*t + start;   //projection point on the laser beam
      res = clpos - pP;     //residual
   
      TVector3 sig_res;     //final errors of laser-fit cluster
      TVector3 sig_pP;      //erros of the projection point
      
      //add the matching clusters to the track
      if(res.Mag()<=_maxDist && t<=length) {	
      	
      	if(_XYresidual == false) {
      		//error calculation:
      		double sig_t = sqrt(pow(dir.X()*clerr.X(),2)+pow(dir.Y()*clerr.Y(),2)+pow(dir.Z()*clerr.Z(),2));
      		sig_pP = dir * sig_t;
      		double sig_resX = sqrt(pow(clerr.X(),2) + pow(sig_pP.X(),2));
      		double sig_resY = sqrt(pow(clerr.Y(),2) + pow(sig_pP.Y(),2));
      		double sig_resZ = sqrt(pow(clerr.Z(),2) + pow(sig_pP.Z(),2));
      		sig_res = TVector3(sig_resX, sig_resY, sig_resZ);
      		
      	}
      	      	
      	//alternative residual calculation 
      	else {
      		double delta_z = clpos.Z() - start.Z();
      		double t_z = delta_z / dir.Z();
      		pP = dir*t_z + start;
      		sig_pP = dir * clerr.Z();
      		res = clpos - pP;
      		double sig_resX = sqrt(pow(clerr.X(),2) + pow(sig_pP.X(),2));
      		double sig_resY = sqrt(pow(clerr.Y(),2) + pow(sig_pP.Y(),2));
      		double sig_resZ = sqrt(pow(clerr.Z(),2) + pow(sig_pP.Z(),2));
      		sig_res = TVector3(sig_resX, sig_resY, sig_resZ);
      	}
	
      	//TODO: implement smart check for crossings. Use map or hash-map!
	
      	//look for double associations
      	// 	for(int i=0; i<_trackArray->GetEntriesFast(); i++) {
      	// 	  std::vector<PndTpcCluster*>* cl_temp;
      	// 	  cl_temp = ((PndTpcLaserTrack*)_trackArray->At(i))->getClusterList();
      	// 	  bool found = false;
      	// 	  for(int j=0; j<cl_temp->size(); j++) {
      	// 	    if(cl_temp->at(j) != cl)
      	// 	      continue;
      	// 	    else
      	// 	      found = true;
      	// 	  }
      	// 	  if(found != true)
      	// 	    theTrk->addCluster(cl, res, pP);  //add the cluster to the track
      	// 	}
      	counter++;
      	PndTpcLaserStat* stat = new PndTpcLaserStat(theTrk->getID(),cl,res,sig_res,pP,sig_pP);
      	theTrk->addStat(stat);  //add the cluster to the track
      	} 
    	}  // end loop over all clusters


    std::cout<<"PndTpcLaserFitTask::Exec(): Found "
	     <<counter<<" Clusters on Track "<<theTrk->getID()<<std::endl;
    std::cout.flush();
    std::vector<PndTpcLaserStat*>* statList = theTrk->getStatList();  
    unsigned int size = statList->size();
    for(unsigned int j=0; j<size; j++) {
      Int_t statsize = _statArray->GetEntriesFast();
      new((*_statArray)[statsize]) PndTpcLaserStat(*(statList->at(j)));
    }
    new((*_trackArray)[ilaser]) PndTpcLaserTrack(*theTrk);
  } // end loop over all lasers
      
}

ClassImp(PndTpcLaserFitTask)
