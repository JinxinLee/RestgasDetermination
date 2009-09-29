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

  std::vector<PndTpcLaserStat*>* getStatList() {return fstatList;}
  std::vector<PndTpcCluster*>* getClusterList();
  unsigned int getID() {return fID;}
  const TVector3& getStart() const {return fstart;}
  const TVector3& getDir() const {return fdir;}

 private:

  std::vector<PndTpcLaserStat*>* fstatList;  //! //statistic objects
  TVector3 fstart;
  TVector3 fdir;
  TVector3 fend;
  unsigned int fID;

public:
  ClassDef(PndTpcLaserTrack,1)

};

#endif
