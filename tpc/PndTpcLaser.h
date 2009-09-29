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

  void setEndPos(TVector3 end) {fend = end;}
  void setDirection(TVector3 dir) {fdir = dir * (1/(double)dir.Mag());} 
  void setIonDens(double iondens) {fionDens = iondens;}
  void setWidth(double width) {fwidth = width;}
  void setTime(double time) {ftime = time;}

  void activate();       //fires the laser

//Acessors --------------------------------------------------

  TVector3 getStart() {return fstart;} 
  TVector3 getDir() {return fdir;}
  TVector3 getEnd() {return fend;}
  double getIonDens() const {return fionDens;}
  double getWidth() const {return fwidth;}
  double getTime() const {return ftime;}
  unsigned int getNE() const {return totalNE;}
  
  void print();

  TClonesArray* getTrack() const {return flaserArray;}

// ----------------------------------------------------------


 private:

  TVector3 fstart;     //starting position
  TVector3 fend;              //end point
  TVector3 fdir;              //direction, normalized

  double fionDens;           //mean ion density on track 
                             //[electrons/cm]
  double fwidth;             //width of laser beam
  double ftime;              //fire time
  double ftrackLength;       //length of laser track in TPC

  unsigned int totalNE;      //total number of electrons
  

  double frMin, frMax, fzMin, fzMax;   //PndTpc geometry.

  TClonesArray* flaserArray;   //array of PndTpcPrimaryClusters
                               //with only 1 electron each

  void evalGeometry();   //evaluates endpoint or direction

};

#endif
