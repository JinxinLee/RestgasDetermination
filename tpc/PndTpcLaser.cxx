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
  _start=start;
  if(direction.Mag()!=0)
  {
    if(dirisend==false){
      _dir = direction *(1/(double)direction.Mag()); //normalize
      _end = TVector3(0,0,0);}
    else {
      _end = direction;
      _dir = TVector3(0,0,0); }
  }
  _ionDens = iondens; 
  _width = width;
  _time = time;
  _rMin=15.5;
  _rMax=41.5;
  _zMin=-39.5;
  _zMax=109.5;

  evalGeometry();
}

PndTpcLaser::~PndTpcLaser()
{;}

void
PndTpcLaser::evalGeometry()
{
  if(_dir.Mag() == 0. && _end.Mag() == 0)
    Error("PndTpcLaser::evalGeometry()", "geometry not set! supply direction or end");

  if(_end.Mag() == 0)    //direction is given, standard case
  {
    double stepsize = 0.1;  //1 mm, should be small enough
    _end = _start;
    //determine the end-point
    //the correction +0.01 guarantees that we don't lie outside due to
    //rounding errors
    while((_end.X()*_end.X() + _end.Y()*_end.Y()) >= _rMin*_rMin &&
	  (_end.X()*_end.X() + _end.Y()*_end.Y()) <= (_rMax*_rMax+0.01))
    {
      _end = _end + (_dir*stepsize);
    }
  }  
    
  if(_dir.Mag() == 0)    //end-point is given
  {
    _dir = _end-_start;
    _dir = _dir*(1/(double)_dir.Mag());  //normalize
  }
}

void
PndTpcLaser::activate()
{
  evalGeometry();
  _trackLength = (_end-_start).Mag();
  //total number of electrons
  totalNE = (unsigned int)std::floor(_ionDens*_trackLength);
   _laserArray = new TClonesArray("PndTpcPrimaryCluster");
  for (int i=0; i<totalNE; i++)
    new((*_laserArray)[i]) PndTpcPrimaryCluster(_time, 1, TVector3(0,0,0), 1, 1);
  
  PndTpcPoint* startPoint = new PndTpcPoint(1, 1, _start, TVector3(0,0,0),
				      _time, 0, 0);
  PndTpcPoint* endPoint = new PndTpcPoint(1, 1, _end, TVector3(0,0,0),
				      _time, 0, 0);

  //arrange the created "clusters" along a straight line, the beam
  LinearInterpolPolicy().Interpolate(startPoint,endPoint,_laserArray,0,totalNE);
  
  // Gauss-profile ... smear them
  double dx, dy, dz;
  PndTpcPrimaryCluster* cluster;
  TVector3 pos;
  int size = _laserArray->GetEntriesFast();
  
  for(int j=0; j<size; j++)
  {   
    cluster = (PndTpcPrimaryCluster*) _laserArray->At(j);
    pos = cluster->pos();
    //Remove Clusters that have been created
    //outside of TPC region due to nature of LinearInterpolPolicy
    if((pos.X()*pos.X() + pos.Y()*pos.Y()) < _rMin*_rMin ||
       (pos.X()*pos.X() + pos.Y()*pos.Y()) > _rMax*_rMax)
    {
      _laserArray->RemoveAt(j);
      continue;
    }
    dx = gRandom->Gaus(0,_width);
    dy = gRandom->Gaus(0,_width);
    dz = gRandom->Gaus(0,_width);
    cluster->setpos(pos.X()+dx, pos.Y()+dy, pos.Z()+dz);
  }
  
}

void
PndTpcLaser::print() 
{
  evalGeometry();
  std::cout<<"\nStart("<<_start.X()<<", "<<_start.Y()<<", "<<_start.Z()<<"), ";
  std::cout<<"direction("<<_dir.X()<<", "<<_dir.Y()<<", "<<_dir.Z()<<"), ";
  std::cout<<"\nend("<<_end.X()<<", "<<_end.Y()<<", "<<_end.Z()<<")";
  std::cout<<"\nIon density: "<<_ionDens<<", width: "<<_width<<", time: "
	   <<_time<<std::endl;
}
  
  
