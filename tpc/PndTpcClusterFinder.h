//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Cluster Finding in the TPC:
//      ClusterFinder administrating SectorProcessors
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCCLUSTERFINDER_HH
#define TPCCLUSTERFINDER_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include <vector>
#include <map>
//#include "PndTpcDigi.h"
//#include "PndTpcCluster.h"
#include "PndTpcAbsClusterFinder.h"

// Collaborating Class Declarations --
class PndTpcPadPlane;
class padprocessor;
class PndTpcSectorProcessor;
class PndTpcDigi;
class PndTpcCluster;

class PndTpcClusterFinder : public PndTpcAbsClusterFinder{
public:

  // Constructors/Destructors ---------
  PndTpcClusterFinder(PndTpcPadPlane* p,
		   std::vector<PndTpcCluster*>* output_buffer,
		   unsigned int timeslice, int mode=0, int sectorid=-1);
  ~PndTpcClusterFinder();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------

  // Mode==0 --> global time slices
  // Mode==1 --> sector wise time slices; pad cannot be hit twice
  void setMode(int mode) {_mode=mode;}


  // Operations ----------------------
  virtual void process(std::vector<PndTpcDigi*>& digis);
  virtual void reset();


  virtual  void checkConsistency();

private:
  void putDigi(PndTpcDigi* digi);

  // Private Data Members ------------
  std::map<unsigned int, PndTpcSectorProcessor*> _sproc;
  std::map<unsigned int, std::vector<PndTpcDigi*>* > _sectormap;
  PndTpcPadPlane* _padplane;
  std::vector<PndTpcCluster*>* _output_buffer;

  unsigned int _dt; // time slice in units of sample time
  int _mode;

  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
