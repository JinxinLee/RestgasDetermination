//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//     Implementation of PndTpcLaser.
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer        TUM            (original author)
//
//
//-----------------------------------------------------------


// This class' header
#include "PndTpcLaser.h"

// Collaborating class headers
#include "TError.h"
#include <cmath>
#include <iostream>
#include "PndTpcPoint.h"
#include "PndTpcPrimaryCluster.h"
#include "LinearInterpolPolicy.h"
#include "TRandom.h"

PndTpcLaser::PndTpcLaser(const TVector3& start, const TVector3& direction,
		   double iondens, double width, double time, bool dirisend)
{
  fstart=start;
  if(direction.Mag()!=0)
  {
    if(dirisend==false){
      fdir = direction *(1/(double)direction.Mag()); //normalize
      fend = TVector3(0,0,0);}
    else {
      fend = direction;
      fdir = TVector3(0,0,0); }
  }
  fionDens = iondens; 
  fwidth = width;
  ftime = time;
  frMin=15.5;
  frMax=41.5;
  fzMin=-39.5;
  fzMax=109.5;

  evalGeometry();
}

PndTpcLaser::~PndTpcLaser()
{;}

void
PndTpcLaser::evalGeometry()
{
  if(fdir.Mag() == 0. && fend.Mag() == 0)
    Error("PndTpcLaser::evalGeometry()", "geometry not set! supply direction or end");

  if(fend.Mag() == 0)    //direction is given, standard case
  {
    double stepsize = 0.1;  //1 mm, should be small enough
    fend = fstart;
    //determine the end-point
    //the correction +0.01 guarantees that we don't lie outside due to
    //rounding errors
    while((fend.X()*fend.X() + fend.Y()*fend.Y()) >= frMin*frMin &&
	  (fend.X()*fend.X() + fend.Y()*fend.Y()) <= (frMax*frMax+0.01))
    {
      fend = fend + (fdir*stepsize);
    }
  }  
    
  if(fdir.Mag() == 0)    //end-point is given
  {
    fdir = fend-fstart;
    fdir = fdir*(1/(double)fdir.Mag());  //normalize
  }
}

void
PndTpcLaser::activate()
{
  evalGeometry();
  ftrackLength = (fend-fstart).Mag();
  //total number of electrons
  totalNE = (unsigned int)std::floor(fionDens*ftrackLength);
   flaserArray = new TClonesArray("PndTpcPrimaryCluster");
  for (int i=0; i<totalNE; i++)
    new((*flaserArray)[i]) PndTpcPrimaryCluster(ftime, 1, TVector3(0,0,0), 1, 1);
  
  PndTpcPoint* startPoint = new PndTpcPoint(1, 1, fstart, TVector3(0,0,0),
				      ftime, 0, 0);
  PndTpcPoint* endPoint = new PndTpcPoint(1, 1, fend, TVector3(0,0,0),
				      ftime, 0, 0);

  //arrange the created "clusters" along a straight line, the beam
  LinearInterpolPolicy().Interpolate(startPoint,endPoint,flaserArray,0,totalNE);
  
  // Gauss-profile ... smear them
  double dx, dy, dz;
  PndTpcPrimaryCluster* cluster;
  TVector3 pos;
  int size = flaserArray->GetEntriesFast();
  
  for(int j=0; j<size; j++)
  {   
    cluster = (PndTpcPrimaryCluster*) flaserArray->At(j);
    pos = cluster->pos();
    //Remove Clusters that have been created
    //outside of TPC region due to nature of LinearInterpolPolicy
    if((pos.X()*pos.X() + pos.Y()*pos.Y()) < frMin*frMin ||
       (pos.X()*pos.X() + pos.Y()*pos.Y()) > frMax*frMax)
    {
      flaserArray->RemoveAt(j);
      continue;
    }
    dx = gRandom->Gaus(0,fwidth);
    dy = gRandom->Gaus(0,fwidth);
    dz = gRandom->Gaus(0,fwidth);
    cluster->setpos(pos.X()+dx, pos.Y()+dy, pos.Z()+dz);
  }
  
}

void
PndTpcLaser::print() 
{
  evalGeometry();
  std::cout<<"\nStart("<<fstart.X()<<", "<<fstart.Y()<<", "<<fstart.Z()<<"), ";
  std::cout<<"direction("<<fdir.X()<<", "<<fdir.Y()<<", "<<fdir.Z()<<"), ";
  std::cout<<"\nend("<<fend.X()<<", "<<fend.Y()<<", "<<fend.Z()<<")";
  std::cout<<"\nIon density: "<<fionDens<<", width: "<<fwidth<<", time: "
	   <<ftime<<std::endl;
}
  
  
