//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//     FairTask for creation of laser-beams in the TPC
//     See "PndTpcLaser.h" for information on the Lasers
//     Lasers can be added manually or read in from a file
//     This file has to be of of a special format
//     (see below for more information)
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer        TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCLASERTASK_H
#define TPCLASERTASK_H

// Base Class Headers ---------------------------------------
#include "FairTask.h"

// External Headers -----------------------------------------
#include <vector> 
#include "TVector3.h"


class PndTpcLaser;
class TClonesArray;

class PndTpcLaserTask : public FairTask {

 public:

// Constructors ---------------------------------------------
  PndTpcLaserTask();
  ~PndTpcLaserTask();

// ----------------------------------------------------------
  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);

// Accessors ------------------------------------------------

  PndTpcLaser* getLaser(const TVector3& start){;} //TODO: implement
  std::vector<PndTpcLaser*>* getLaserList() {return _laserList;}

// Modifiers ------------------------------------------------

  //sets the filename of the file with the laser array info
  //and reads it in automatically. Run in macro before Exec()
  //
  //FILE FORMAT has to be like this:
  //#comments 
  //"!end" or "!direction" once to determine how to init the lasers
  //and then one line for every laser in the format
  //startX startY startZ vecX vecY vecZ ionDens beamWidth fireTime
  //where 'vec' stands for direction or end vector 
  void setLaserFile(const char* file, bool quiet=false); 

  void addLaser(double,double,double,double,double,double,
		double iondens, double width, double time, bool);


  

 private:

  TClonesArray* _primArray;
  TClonesArray* _laserArray;
  std::vector<PndTpcLaser*>* _laserList;  
  PndTpcLaser* _currentLaser;
  const char* _filename;
  char _dir_or_end[15];
  bool _is_end;
  double _startX,_startY,_startZ,_dirX,_dirY,_dirZ,_iondens,_width,_time;
  unsigned int addCounter, NEcount;

  void readFromFile(bool); 
  
  
public:
  ClassDef(PndTpcLaserTask,1)

};

#endif



