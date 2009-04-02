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
#include <map>
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
    _maxDist(0.8),         //this is not the final value!! check Exec()
    _openingAngle(0),
    _zMin(-39.5),
    _zMax(109.5),
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
  

//TODO: Error in z has to be taken into account!!!

void
PndTpcLaserFitTask::Exec(Option_t* opt) 
{
  
  std::cout<< "\nPndTpcLaserFitTask::Exec(): Starting Laser Reconstruction ...\n"<<std::endl;
  
  unsigned int Nlasers = _laserList->size();
  unsigned int Nclusters = _clusterArray->GetEntriesFast();

  TVector3 pP;     //projected point on the theoretical laser track
  TVector3 res;    //residual
  TVector3 dist;  

  unsigned int ncl=_clusterArray->GetEntriesFast();

  double slope = _maxDist/(_zMax-_zMin); //needed for effective cut based on zPos

  std::vector<PndTpcLaserTrack*> laserTrackList;
  for(int i=0; i<Nlasers; i++) {
    PndTpcLaser* theLaser = _laserList->at(i);
    laserTrackList.push_back(new PndTpcLaserTrack(i, theLaser->getStart(),theLaser->getDir()));
  } 

    
  //loop over all clusters, find matching clusters in the cone
  for (unsigned int iclust=0; iclust<ncl; iclust++) {

    cl =(PndTpcCluster*)_clusterArray->At(iclust);

    std::map<int, double> dist_map;  //store cluster-distance from every laser
    unsigned int found_count=0;   //how many tracks match?


    TVector3 start, end, dir, clpos, clvec, clerr;
    double length, t;
    
    //loop over all laser tracks, calculate distances
    for(unsigned int ilaser=0; ilaser<Nlasers; ilaser++)
      {
	PndTpcLaser* theLaser = _laserList->at(ilaser);
	
	//calculate geometry and projection point 
	start=theLaser->getStart();
	end=theLaser->getEnd();
	dir=theLaser->getDir();
	length=(start-end).Mag();
	clpos = cl->pos();
	clvec = clpos - start;
	clerr = cl->sig();
	
	t = (clpos-start)*dir;
	
	pP = dir*t + start;   //projection point on the laser beam
	res = clpos - pP;     //residual

	double eff_maxDist = (clpos.Z() - _zMin) * slope + 0.2;
	
	if(res.Mag()>eff_maxDist || t>length) 
	  continue;

	//store distance for this track in distance map:
	dist_map[ilaser]=res.Mag();
	      
      }	//end loop over lasers

    //look for the shortest distance and book
    double min;
    unsigned int match;  //index of the best matching laser track
    unsigned int count=0;  //debug variable
    for(int i=0; i<Nlasers; ++i)  {
      if(dist_map.count(i)){
	if(count>0) {
	  count++;
	  if(dist_map[i]<min) {
	    min=dist_map[i];
	    match=i;
	  }
	}
	else {
	  match=i;
	  count=1;
	  min = dist_map[i];
	}
      }
    } //finished searching for the best laser track...


    if(count==0)//the cluster could not be associated with a track
      continue;   

    
    //redo calculation for the best track: ----------------------------------------------
    
    PndTpcLaser* theLaser = _laserList->at(match);
    PndTpcLaserTrack* theTrk = laserTrackList[match];
    
    
    start=theLaser->getStart();
    end=theLaser->getEnd();
    dir=theLaser->getDir();
    length=(start-end).Mag();
    clpos = cl->pos();
    clvec = clpos - start;
    clerr = cl->sig();
    
    t = (clpos-start)*dir;
	
    pP = dir*t + start;   //projection point on the laser beam
    res = clpos - pP;     //residual
    
    
    //begin error calculation: -----------------------------------------------------------
    
    TVector3 sig_res;     //final errors of laser-fit cluster
    TVector3 sig_pP;      //erros of the projection point
	
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
    
    PndTpcLaserStat* stat = new PndTpcLaserStat(match,cl,res,sig_res,pP,sig_pP);
    theTrk->addStat(stat);  //add the cluster to the track

  }  // end loop over all clusters
  
  for(int i=0; i<Nlasers; i++) {
    std::vector<PndTpcLaserStat*>* statList = (laserTrackList[i])->getStatList();  
    unsigned int size = statList->size();
    for(unsigned int j=0; j<size; j++) {
      Int_t statsize = _statArray->GetEntriesFast();
      new((*_statArray)[statsize]) PndTpcLaserStat(*(statList->at(j)));
    }
  }
}

ClassImp(PndTpcLaserFitTask)
