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
    fpersistence(kFALSE),
    fplot(kFALSE),
    fgridFile("LaserGridTest.dat"),
    fclusterBranchName("PndTpcCluster"),
    fXYresidual(false),
    fmaxDist(0.8),         //this is not the final value!! check Exec()
    fopeningAngle(0.),     //divergence of the lasers
    fNknotsZ(6),
    fNknotsR(3),
    fdevMapR(NULL),
    fdevMapPerp(NULL),
    frecoMapR(NULL),
    frecoMapPerp(NULL)
{;}



PndTpcLaserFitTask::~PndTpcLaserFitTask()
{
  /*dirty: need to get rid of having both the TClonesArray and the vector  */
  for(unsigned int i=0; i<flaserList->size(); ++i)
    delete flaserList->at(i);
  delete flaserList;
  for(unsigned int i=0; i<ftrackList->size(); ++i)
    delete ftrackList->at(i);
  delete ftrackList;
  delete flasertask;
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
  fclusterArray=(TClonesArray*) ioman->GetObject(fclusterBranchName);
  
  if(fclusterArray==0)
    {
      Error("PndTpcLaserFitTask::Init","PndTpcCluster-Array not found!");
      return kERROR;
    }
  
  //read in parameters
  fzMin=fpar->getZGem();
  fzMax=fpar->getZMax();
  frMin=fpar->getRMin();
  frMax=fpar->getRMax();

  std::cout<<"PndTpcLaserFitTask::Init(): parameters initialized: \n"
	   <<"fzMin = "<<fzMin
	   <<"\n_zMax = "<<fzMax
	   <<"\n_rMin = "<<frMin
	   <<"\n_rMax = "<<frMax<<std::endl;
  
    
  
  // create and register output array
  fstatArray = new TClonesArray("PndTpcLaserStat"); 
  ioman->Register("PndTpcLaserStat","PndTpc",fstatArray,fpersistence);
  ftrackArray = new TClonesArray("PndTpcLaserTrack"); 
  ioman->Register("PndTpcLaserTrack","PndTpc",ftrackArray,fpersistence);
  ffitStatArray = new TClonesArray("PndTpcLaserFitTaskStat"); 
  ioman->Register("PndTpcLaserFitTaskStat","PndTpc",ffitStatArray,fpersistence);
  
  buildLaserList(true);

  // initialize mesh for Spline fitting
  fNlamdaZ = fNknotsZ + 8;   //4 knots on each side outside the data area
  fNlamdaR = fNknotsR + 8;

  //stretch the range a bit to make sure we have 4 knots on each side 
  //safely OUTSIDE the data area
    
  double lengthZ = (fzMax-fzMin);
  double lengthR = (frMax-frMin);
  double fzMax_mod = fzMax + 0.02*lengthZ;
  double fzMin_mod = fzMin - 0.02*lengthZ;
  double frMax_mod = frMax + 0.05*lengthR;
  double frMin_mod = frMin - 0.05*lengthR;

  double stepZ = (fzMax_mod-fzMin_mod)/(fNknotsZ-1); 
  double stepR = (frMax_mod-frMin_mod)/(fNknotsR-1); 

  
  
  for(int i=0; i<fNlamdaZ; ++i)
    fknotsZ.push_back(fzMin_mod + (i-4)*stepZ);
  
  for(int i=0; i<fNlamdaR; ++i)
    fknotsR.push_back(frMin_mod + (i-4)*stepR);
  
  fdevMapR = new BiCubSpline(&fknotsZ, &fknotsR);
  fdevMapPerp = new BiCubSpline(&fknotsZ, &fknotsR);
  frecoMapR = new BiCubSpline(&fknotsZ, &fknotsR);
  frecoMapPerp = new BiCubSpline(&fknotsZ, &fknotsR);

  std::cout<<"\n--------- PndTpcLaserFitTask::Init(): Spline mesh initialized -----------"
	   <<std::endl<<"Knots in Z: ";
  for(unsigned int i=0; i<fknotsZ.size(); i++) {
    if(i%4==0 && i>0)
      std::cout<<"\n            " ;
    std::cout<<fknotsZ[i]<<",  ";
  }
  std::cout<<std::endl;
  std::cout<<std::endl<<"Knots in R: ";
  for(unsigned int i=0; i<fknotsR.size(); i++) {
    if(i%4==0 && i>0)
      std::cout<<"\n            " ;
    std::cout<<fknotsR[i]<<",  ";
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
  fpar= (PndTpcDigiPar*) db->getContainer("PndTpcDigiPar");
  if (! fpar ) Fatal("SetParContainers", "PndTpcDigiPar not found");
}



//needs redesign, ugly
void
PndTpcLaserFitTask::buildLaserList(bool quiet)
{
  flasertask = new PndTpcLaserTask();
  flasertask->setLaserFile(fgridFile.c_str(), quiet);
  flaserList = flasertask->getLaserList();
  if (flaserList!=NULL)
    std::cout<<"\nPndTpcLaserFitTask::buildLaserList(): success! "
	     <<"\n                                      "
	     <<"Laserlist has "<<flaserList->size()<<" entries"<<std::endl;
}
  

//TODO: Error in z has to be taken into account!!!

void
PndTpcLaserFitTask::Exec(Option_t* opt) 
{
  
  std::cout<< "\nPndTpcLaserFitTask::Exec(): Starting Laser Reconstruction ...\n"<<std::endl;
  
  unsigned int Nlasers = flaserList->size();
  unsigned int Nclusters = fclusterArray->GetEntriesFast();

  TVector3 pP;     //projected point on the theoretical laser track
  TVector3 res;    //residual
  TVector3 dist;  

  unsigned int ncl=fclusterArray->GetEntriesFast();
  
  double slope = fmaxDist/(fzMax-fzMin); //needed for effective cut based on zPos
  
  std::vector<PndTpcLaserTrack*> laserTrackList;
  for(int i=0; i<Nlasers; i++) {
    PndTpcLaser* theLaser = flaserList->at(i);
    laserTrackList.push_back(new PndTpcLaserTrack(i, theLaser->getStart(),theLaser->getDir()));
    unsigned int size = ftrackArray->GetEntriesFast();
    new((*ftrackArray)[size]) PndTpcLaserTrack(*(laserTrackList[(laserTrackList.size()-1)]));
  } 
    
  std::cout<< "\nPndTpcLaserFitTask::Exec(): Built Laser list ...\n"<<std::endl;

   
  //loop over all clusters, find matching clusters in the cone
  for (unsigned int iclust=0; iclust<ncl; iclust++) {

    cl =(PndTpcCluster*)fclusterArray->At(iclust);

    std::map<int, double> dist_map;  //store cluster-distance from every laser
    unsigned int found_count=0;   //how many tracks match?


    TVector3 start, end, dir, clpos, clvec, clerr;
    double length, t;
    
    //loop over all laser tracks, calculate distances
    for(unsigned int ilaser=0; ilaser<Nlasers; ilaser++)
      {
	PndTpcLaser* theLaser = flaserList->at(ilaser);
	
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

	double eff_maxDist = (clpos.Z() - fzMin) * slope + 0.2;
	
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
    
    PndTpcLaser* theLaser = flaserList->at(match);
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
	
    if(fXYresidual == false) {
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

    fdevMapR_data.push_back(mapR);
    fdevMapPerp_data.push_back(mapPerp);
    frecoMapR_data.push_back(recoR);
    frecoMapPerp_data.push_back(recoPerp);
 
  }  // end loop over all clusters
  
  
  for(int i=0; i<Nlasers; i++) {
    std::vector<PndTpcLaserStat*>* statList = (laserTrackList[i])->getStatList();  
    unsigned int size = statList->size();
    for(unsigned int j=0; j<size; j++) {
      Int_t statsize = fstatArray->GetEntriesFast();
      new((*fstatArray)[statsize]) PndTpcLaserStat(*(statList->at(j)));
    }
  }
  
  // Fit using BiCubic Splines

  std::cout<<"\n\nPndTpcLaserFitTask::Exec(): starting fits ..............."<<std::endl;

  BiCubSplineFitter* fit_devMapR = new BiCubSplineFitter(fdevMapR, &fdevMapR_data);
  BiCubSplineFitter* fit_devMapPerp = new BiCubSplineFitter(fdevMapPerp, &fdevMapPerp_data);
  BiCubSplineFitter* fit_recoMapR = new BiCubSplineFitter(frecoMapR, &frecoMapR_data);
  BiCubSplineFitter* fit_recoMapPerp = new BiCubSplineFitter(frecoMapPerp, &frecoMapPerp_data);

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
  TArrayD res_devMapR = TArrayD((fNlamdaZ-4)*(fNlamdaR-4));
  TArrayD res_devMapPerp = TArrayD((fNlamdaZ-4)*(fNlamdaR-4));
  TArrayD res_recoMapR = TArrayD((fNlamdaZ-4)*(fNlamdaR-4));
  TArrayD res_recoMapPerp = TArrayD((fNlamdaZ-4)*(fNlamdaR-4));
  
  for(int i=0; i<(fNlamdaZ-4)*(fNlamdaR-4); ++i) {
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

  fdevMapR->setCoeffsByArray(par_devMapR);
  fdevMapPerp->setCoeffsByArray(par_devMapPerp);
  frecoMapR->setCoeffsByArray(par_recoMapR);
  frecoMapPerp->setCoeffsByArray(par_recoMapPerp);

  
  if(fplot) {    
    std::cout<<"\nPndTpcLaserFitTask::Exec(): creating plots .........."<<std::endl;

    TPolyMarker3D* poly_devMapR = new TPolyMarker3D(fdevMapR_data.size());
    TPolyMarker3D* poly_devMapPerp = new TPolyMarker3D(fdevMapPerp_data.size());
    TPolyMarker3D* poly_recoMapR = new TPolyMarker3D(frecoMapR_data.size());
    TPolyMarker3D* poly_recoMapPerp = new TPolyMarker3D(frecoMapPerp_data.size());
  
    for(int i=0; i<fdevMapR_data.size(); ++i) {
      std::vector<double>* temp = fdevMapR_data[i];
      poly_devMapR->SetNextPoint(temp->at(0), temp->at(1), temp->at(2));
    }
    
    for(int i=0; i<fdevMapPerp_data.size(); ++i) {
      std::vector<double>* temp = fdevMapPerp_data[i];
      poly_devMapPerp->SetNextPoint(temp->at(0), temp->at(1), temp->at(2));
    }
    
    for(int i=0; i<frecoMapR_data.size(); ++i) {
      std::vector<double>* temp = frecoMapR_data[i];
      poly_recoMapR->SetNextPoint(temp->at(0), temp->at(1), temp->at(2));
    }
    
    for(int i=0; i<frecoMapPerp_data.size(); ++i) {
      std::vector<double>* temp = frecoMapPerp_data[i];
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
    (fdevMapR->getTF2(fzMin,fzMax,frMin,frMax))->Draw("SURF1");
    canv2->cd(3);
    (fdevMapPerp->getTF2(fzMin,fzMax,frMin,frMax))->Draw("SURF1");
    canv2->cd(2);
    (frecoMapR->getTF2(fzMin,fzMax,frMin,frMax))->Draw("SURF1");
    canv2->cd(4);
    (frecoMapPerp->getTF2(fzMin,fzMax,frMin,frMax))->Draw("SURF1");
  }


  //create FitStat object

  std::vector<BiCubSpline*> splineList;
  splineList.push_back(fdevMapR);
  splineList.push_back(fdevMapPerp);
  splineList.push_back(frecoMapR);
  splineList.push_back(frecoMapPerp);

  std::vector<std::vector<std::vector<double>*>*> datalist;
  datalist.push_back(&fdevMapR_data);
  datalist.push_back(&fdevMapPerp_data);
  datalist.push_back(&frecoMapR_data);
  datalist.push_back(&frecoMapPerp_data);
  

  std::cout<<"\nPndTpcLaserFitTask::Exec(): booking fit result container ..."
	   <<std::endl;
  
  //write fitStat container
  unsigned int sizeS = ffitStatArray->GetEntriesFast();
  //LaserFitTask gives away BiCubSpline ownerships
  new((*ffitStatArray)[sizeS]) PndTpcLaserFitTaskStat(splineList, datalist);

}

ClassImp(PndTpcLaserFitTask)
