//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//     This class describes a Laser beam in the tpc volume
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer        TUM            (original author)
//
//
//-----------------------------------------------------------


#ifndef TPCLASER_H
#define TPCLASER_H

#include "TVector3.h"
#include "TClonesArray.h"


class PndTpcLaser {

//Constructors ----------------------------------------------

 public:

  //either supply direction here, or set direction or endpoint 
  //manually via set-functions
  PndTpcLaser(const TVector3& start, const TVector3& direction,
	   double iondens=0, double width=0, double time=0, bool dirisend=false);


  ~PndTpcLaser();


//Modifiers -------------------------------------------------

  void setEndPos(TVector3 end) {_end = end;}
  void setDirection(TVector3 dir) {_dir = dir * (1/(double)dir.Mag());} 
  void setIonDens(double iondens) {_ionDens = iondens;}
  void setWidth(double width) {_width = width;}
  void setTime(double time) {_time = time;}

  void activate();       //fires the laser

//Acessors --------------------------------------------------

  TVector3 getStart() {return _start;} 
  TVector3 getDir() {return _dir;}
  TVector3 getEnd() {return _end;}
  double getIonDens() const {return _ionDens;}
  double getWidth() const {return _width;}
  double getTime() const {return _time;}
  unsigned int getNE() const {return totalNE;}
  
  void print();

  TClonesArray* getTrack() const {return _laserArray;}

// ----------------------------------------------------------


 private:

  TVector3 _start;     //starting position
  TVector3 _end;              //end point
  TVector3 _dir;              //direction, normalized

  double _ionDens;           //mean ion density on track 
                             //[electrons/cm]
  double _width;             //width of laser beam
  double _time;              //fire time
  double _trackLength;       //length of laser track in TPC

  unsigned int totalNE;      //total number of electrons
  

  double _rMin, _rMax, _zMin, _zMax;   //PndTpc geometry.

  TClonesArray* _laserArray;   //array of PndTpcPrimaryClusters
                               //with only 1 electron each

  void evalGeometry();   //evaluates endpoint or direction

};

#endif
