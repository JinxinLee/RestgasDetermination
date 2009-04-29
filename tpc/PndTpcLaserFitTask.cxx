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

// Collaborating Class Headers --------
#include "TClonesArray.h"
#include "FairRootManager.h"
#include "TVector3.h"
#include "TVectorD.h"
#include "TArrayD.h"
#include "TF2.h"
#include "TCanvas.h"
#include "PndTpcDigiPar.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "PndTpcLaser.h"
#include "PndTpcLaserTrack.h"
#include "PndTpcLaserTask.h"
#include "PndTpcLaserStat.h"
#include "PndTpcCluster.h"
#include "BiCubSpline.h"
#include "BiCubSplineFitter.h"
#include "SplineTF2Interface.h"
#include "PndTpcLaserFitTaskStat.h"

#include "TPolyMarker3D.h"


// Class Member definitions -----------

PndTpcLaserFitTask::PndTpcLaserFitTask()		//default constructor
  : FairTask("TPC Laser Fit"),
    _persistence(kFALSE),
    _plot(kFALSE),
    _gridFile("LaserGridTest.dat"),
    _clusterBranchName("PndTpcCluster"),
    _XYresidual(false),
    _maxDist(0.8),         //this is not the final value!! check Exec()
    _openingAngle(0.),     //divergence of the lasers
    _NknotsZ(6),
    _NknotsR(3),
    _devMapR(NULL),
    _devMapPerp(NULL),
    _recoMapR(NULL),
    _recoMapPerp(NULL)
{;}



PndTpcLaserFitTask::~PndTpcLaserFitTask()
{
  /*dirty: need to get rid of having both the TClonesArray and the vector  */
  for(unsigned int i=0; i<_laserList->size(); ++i)
    delete _laserList->at(i);
  delete _laserList;
  for(unsigned int i=0; i<_trackList->size(); ++i)
    delete _trackList->at(i);
  delete _trackList;
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
  
  //read in parameters
  _zMin=_par->getZGem();
  _zMax=_par->getZMax();
  _rMin=_par->getRMin();
  _rMax=_par->getRMax();

  std::cout<<"PndTpcLaserFitTask::Init(): parameters initialized: \n"
	   <<"_zMin = "<<_zMin
	   <<"\n_zMax = "<<_zMax
	   <<"\n_rMin = "<<_rMin
	   <<"\n_rMax = "<<_rMax<<std::endl;
  
    
  
  // create and register output array
  _statArray = new TClonesArray("PndTpcLaserStat"); 
  ioman->Register("PndTpcLaserStat","PndTpc",_statArray,_persistence);
  _trackArray = new TClonesArray("PndTpcLaserTrack"); 
  ioman->Register("PndTpcLaserTrack","PndTpc",_trackArray,_persistence);
  _fitStatArray = new TClonesArray("PndTpcLaserFitTaskStat"); 
  ioman->Register("PndTpcLaserFitTaskStat","PndTpc",_fitStatArray,_persistence);
  
  buildLaserList(true);

  // initialize mesh for Spline fitting
  _NlamdaZ = _NknotsZ + 8;   //4 knots on each side outside the data area
  _NlamdaR = _NknotsR + 8;

  //stretch the range a bit to make sure we have 4 knots on each side 
  //safely OUTSIDE the data area
    
  double lengthZ = (_zMax-_zMin);
  double lengthR = (_rMax-_rMin);
  double _zMax_mod = _zMax + 0.02*lengthZ;
  double _zMin_mod = _zMin - 0.02*lengthZ;
  double _rMax_mod = _rMax + 0.02*lengthR;
  double _rMin_mod = _rMin - 0.02*lengthR;

  double stepZ = (_zMax_mod-_zMin_mod)/(_NknotsZ-1); 
  double stepR = (_rMax_mod-_rMin_mod)/(_NknotsR-1); 

  
  
  for(int i=0; i<_NlamdaZ; ++i)
    _knotsZ.push_back(_zMin_mod + (i-4)*stepZ);
  
  for(int i=0; i<_NlamdaR; ++i)
    _knotsR.push_back(_rMin_mod + (i-4)*stepR);
  
  _devMapR = new BiCubSpline(&_knotsZ, &_knotsR);
  _devMapPerp = new BiCubSpline(&_knotsZ, &_knotsR);
  _recoMapR = new BiCubSpline(&_knotsZ, &_knotsR);
  _recoMapPerp = new BiCubSpline(&_knotsZ, &_knotsR);

  std::cout<<"\n--------- PndTpcLaserFitTask::Init(): Spline mesh initialized -----------"
	   <<std::endl<<"Knots in Z: ";
  for(unsigned int i=0; i<_knotsZ.size(); i++) {
    if(i%4==0 && i>0)
      std::cout<<"\n            " ;
    std::cout<<_knotsZ[i]<<",  ";
  }
  std::cout<<std::endl;
  std::cout<<std::endl<<"Knots in R: ";
  for(unsigned int i=0; i<_knotsR.size(); i++) {
    if(i%4==0 && i>0)
      std::cout<<"\n            " ;
    std::cout<<_knotsR[i]<<",  ";
  }
  std::cout<<"\n-------------------------------------------------------------------------\n"<<std::endl;
    
  return kSUCCESS;
}


void 
PndTpcLaserFitTask::SetParContainers() {

  std::cout<<"PndTpcLaserFitTask::SetParContainers"<<std::endl;
  std::cout.flush();

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get PndTpc digitisation parameter container
  _par= (PndTpcDigiPar*) db->getContainer("PndTpcDigiPar");
  if (! _par ) Fatal("SetParContainers", "PndTpcDigiPar not found");
}



//needs redesign, ugly
void
PndTpcLaserFitTask::buildLaserList(bool quiet)
{
  _lasertask = new PndTpcLaserTask();
  _lasertask->setLaserFile(_gridFile.c_str(), quiet);
  _laserList = _lasertask->getLaserList();
  if (_laserList!=NULL)
    std::cout<<"\nPndTpcLaserFitTask::buildLaserList(): success! "
	     <<"\n                                      "
	     <<"Laserlist has "<<_laserList->size()<<" entries"<<std::endl;
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
    unsigned int size = _trackArray->GetEntriesFast();
    new((*_trackArray)[size]) PndTpcLaserTrack(*(laserTrackList[(laserTrackList.size()-1)]));
  } 
    
  std::cout<< "\nPndTpcLaserFitTask::Exec(): Built Laser list ...\n"<<std::endl;

   
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

	
	
	bool in_region_of_interest = false;
	//in this case, do we have a faulty matching?
	bool abort = false;
	
	if(clpos.Z() > 0. && clpos.Perp() < 25.)
	  in_region_of_interest = true;
	
	if(in_region_of_interest) {
	  double phi = pP.Phi();
	  pP.RotateZ(-phi);
	  clpos.RotateZ(-phi);
	  double r_pP=pP.X();
	  double r_cl=clpos.X();
	  double perp_pP=pP.Y();        // = 0
	  double perp_cl=clpos.Y();   
	  
	  double dr = r_cl-r_pP;
	  double dperp = perp_cl-perp_pP;
	  pP.RotateZ(phi);
	  clpos.RotateZ(phi);
	  if(dr < 0. || dperp < 0.)
	    abort=true;
	}
	  
	//store distance for this track in distance map:
	if(!abort)
	  dist_map[ilaser]=res.Mag();
	      
      }	//end loop over lasers



    //TODO: think of smart algorithm for track matching
    
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

    
    //fill reconstruction data into Spline objects ---------------------------------------
    
    //TODO: error calculation (see testmacro)

    double phi = pP.Phi();
    pP.RotateZ(-phi);
    clpos.RotateZ(-phi);
    double r_pP=pP.X();
    double r_cl=clpos.X();
    double perp_pP=pP.Y();        // = 0
    double perp_cl=clpos.Y();   
    double z_pP=pP.Z();
    double z_cl=clpos.Z();

    double dr = r_cl-r_pP;
    double dperp = perp_cl-perp_pP;

    
    pP.RotateZ(phi);
    clpos.RotateZ(phi);

    std::vector<double>* mapR = new std::vector<double>(4,0.);
    std::vector<double>* mapPerp = new std::vector<double>(4,0.);
    std::vector<double>* recoR = new std::vector<double>(4,0.);
    std::vector<double>* recoPerp = new std::vector<double>(4,0.);

    
    mapR->at(0)=z_pP; mapR->at(1)=r_pP; mapR->at(2)=dr;
    mapPerp->at(0)=z_pP; mapPerp->at(1)=r_pP; mapPerp->at(2)=dperp;
    //TODO: resolve z
    recoR->at(0)=z_pP; recoR->at(1)=clpos.Perp(); recoR->at(2)=dr;
    recoPerp->at(0)=z_pP; recoPerp->at(1)=clpos.Perp(); recoPerp->at(2)=dperp;

    _devMapR_data.push_back(mapR);
    _devMapPerp_data.push_back(mapPerp);
    _recoMapR_data.push_back(recoR);
    _recoMapPerp_data.push_back(recoPerp);
 
  }  // end loop over all clusters
  
  
  for(int i=0; i<Nlasers; i++) {
    std::vector<PndTpcLaserStat*>* statList = (laserTrackList[i])->getStatList();  
    unsigned int size = statList->size();
    for(unsigned int j=0; j<size; j++) {
      Int_t statsize = _statArray->GetEntriesFast();
      new((*_statArray)[statsize]) PndTpcLaserStat(*(statList->at(j)));
    }
  }
  
  // Fit using BiCubic Splines

  std::cout<<"\n\nPndTpcLaserFitTask::Exec(): starting fits ..............."<<std::endl;

  BiCubSplineFitter* fit_devMapR = new BiCubSplineFitter(_devMapR, &_devMapR_data);
  BiCubSplineFitter* fit_devMapPerp = new BiCubSplineFitter(_devMapPerp, &_devMapPerp_data);
  BiCubSplineFitter* fit_recoMapR = new BiCubSplineFitter(_recoMapR, &_recoMapR_data);
  BiCubSplineFitter* fit_recoMapPerp = new BiCubSplineFitter(_recoMapPerp, &_recoMapPerp_data);

  // Perform QR decomposition
  fit_devMapR->decompose();
  fit_devMapPerp->decompose();
  fit_recoMapR->decompose();
  fit_recoMapPerp->decompose();

  // Obtain solution
  TVectorD sol_devMapR = fit_devMapR->solve();
  TVectorD sol_devMapPerp = fit_devMapPerp->solve();
  TVectorD sol_recoMapR = fit_recoMapR->solve();
  TVectorD sol_recoMapPerp = fit_recoMapPerp->solve();

  //extract the right number of elements
  TArrayD res_devMapR = TArrayD((_NlamdaZ-4)*(_NlamdaR-4));
  TArrayD res_devMapPerp = TArrayD((_NlamdaZ-4)*(_NlamdaR-4));
  TArrayD res_recoMapR = TArrayD((_NlamdaZ-4)*(_NlamdaR-4));
  TArrayD res_recoMapPerp = TArrayD((_NlamdaZ-4)*(_NlamdaR-4));
  
  for(int i=0; i<(_NlamdaZ-4)*(_NlamdaR-4); ++i) {
    res_devMapR[i] = sol_devMapR[i];
    res_devMapPerp[i] = sol_devMapPerp[i];
    res_recoMapR[i] = sol_recoMapR[i];
    res_recoMapPerp[i] = sol_recoMapPerp[i];
  }
  
  // Convert to standard array and finally set coefficients of Spline
  
  double* par_devMapR = res_devMapR.GetArray();
  double* par_devMapPerp = res_devMapPerp.GetArray();  
  double* par_recoMapR = res_recoMapR.GetArray();
  double* par_recoMapPerp = res_recoMapPerp.GetArray();

  std::cout<<"\nPndTpcLaserFitTask::Exec(): transporting fit results to Splines ....."<<std::endl;

  _devMapR->setCoeffsByArray(par_devMapR);
  _devMapPerp->setCoeffsByArray(par_devMapPerp);
  _recoMapR->setCoeffsByArray(par_recoMapR);
  _recoMapPerp->setCoeffsByArray(par_recoMapPerp);

  
  if(_plot) {    
    std::cout<<"\nPndTpcLaserFitTask::Exec(): creating plots .........."<<std::endl;

    TPolyMarker3D* poly_devMapR = new TPolyMarker3D(_devMapR_data.size());
    TPolyMarker3D* poly_devMapPerp = new TPolyMarker3D(_devMapPerp_data.size());
    TPolyMarker3D* poly_recoMapR = new TPolyMarker3D(_recoMapR_data.size());
    TPolyMarker3D* poly_recoMapPerp = new TPolyMarker3D(_recoMapPerp_data.size());
  
    for(int i=0; i<_devMapR_data.size(); ++i) {
      std::vector<double>* temp = _devMapR_data[i];
      poly_devMapR->SetNextPoint(temp->at(0), temp->at(1), temp->at(2));
    }
    
    for(int i=0; i<_devMapPerp_data.size(); ++i) {
      std::vector<double>* temp = _devMapPerp_data[i];
      poly_devMapPerp->SetNextPoint(temp->at(0), temp->at(1), temp->at(2));
    }
    
    for(int i=0; i<_recoMapR_data.size(); ++i) {
      std::vector<double>* temp = _recoMapR_data[i];
      poly_recoMapR->SetNextPoint(temp->at(0), temp->at(1), temp->at(2));
    }
    
    for(int i=0; i<_recoMapPerp_data.size(); ++i) {
      std::vector<double>* temp = _recoMapPerp_data[i];
      poly_recoMapPerp->SetNextPoint(temp->at(0), temp->at(1), temp->at(2));
    }
    
    TCanvas* canv = new TCanvas();
    canv->Divide(2,2);
    canv->cd(1);
    poly_devMapR->Draw();
    canv->cd(2);
    poly_devMapPerp->Draw();
    canv->cd(3);
    poly_recoMapR->Draw();
    canv->cd(4);
    poly_recoMapPerp->Draw();

    TCanvas* canv2 = new TCanvas();
    canv2->Divide(2,2);
    
    canv2->cd(1);
    (_devMapR->getTF2(_zMin,_zMax,_rMin,_rMax))->Draw("SURF1");
    canv2->cd(3);
    (_devMapPerp->getTF2(_zMin,_zMax,_rMin,_rMax))->Draw("SURF1");
    canv2->cd(2);
    (_recoMapR->getTF2(_zMin,_zMax,_rMin,_rMax))->Draw("SURF1");
    canv2->cd(4);
    (_recoMapPerp->getTF2(_zMin,_zMax,_rMin,_rMax))->Draw("SURF1");
  }


  //create FitStat object

  std::vector<BiCubSpline*> splineList;
  splineList.push_back(_devMapR);
  splineList.push_back(_devMapPerp);
  splineList.push_back(_recoMapR);
  splineList.push_back(_recoMapPerp);

  std::vector<std::vector<std::vector<double>*>*> datalist;
  datalist.push_back(&_devMapR_data);
  datalist.push_back(&_devMapPerp_data);
  datalist.push_back(&_recoMapR_data);
  datalist.push_back(&_recoMapPerp_data);
  

  std::cout<<"\nPndTpcLaserFitTask::Exec(): booking fit result container ..."
	   <<std::endl;
  
  //write fitStat container
  unsigned int sizeS = _fitStatArray->GetEntriesFast();
  //LaserFitTask gives away BiCubSpline ownerships
  new((*_fitStatArray)[sizeS]) PndTpcLaserFitTaskStat(splineList, datalist);

}

ClassImp(PndTpcLaserFitTask)
