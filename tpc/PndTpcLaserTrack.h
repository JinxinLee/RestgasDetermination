//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//    Laser-Track in the PndTpc, built by PndTpcLaserFitTask
//     
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//	Felix Boehmer    	TUM     (original author)
//
//-----------------------------------------------------------

#ifndef PNDTPCLASERTRACK_H
#define PNDTPCLASERTRACK_H


// External Headers -----------------------------------------
#include <vector> 
#include "TVector3.h"
#include "TObject.h"

class PndTpcLaser;
class PndTpcLaserStat;
class PndTpcCluster;
class TClonesArray;


class PndTpcLaserTrack : public TObject {

 public:

// Constructors ---------------------------------------------

  PndTpcLaserTrack() {;}
  PndTpcLaserTrack(unsigned int ID, const TVector3&, const TVector3&);
  ~PndTpcLaserTrack();


// Modifiers ------------------------------------------------

  void addStat(PndTpcLaserStat*);
  

// Acessors -------------------------------------------------

  std::vector<PndTpcLaserStat*>* getStatList() {return _statList;}
  std::vector<PndTpcCluster*>* getClusterList();
  unsigned int getID() {return _ID;}
  const TVector3& getStart() const {return _start;}
  const TVector3& getDir() const {return _dir;}

 private:

  std::vector<PndTpcLaserStat*>* _statList;  //! //statistic objects
  TVector3 _start;
  TVector3 _dir;
  TVector3 _end;
  unsigned int _ID;

public:
  ClassDef(PndTpcLaserTrack,1)

};

#endif
