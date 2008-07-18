//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Cluster Finding in the TPC:
//      SectorProcessor using PadProcessor algorithm
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

#ifndef TPCSECTORPROCESSOR_HH
#define TPCSECTORPROCESSOR_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include <vector>
#include <map>
#include "PndTpcDigi.h"
#include "PndTpcCluster.h"

// Collaborating Class Declarations --
class PndTpcPadPlane;
class padprocessor;


class PndTpcSectorProcessor {
public:

  // Constructors/Destructors ---------
  PndTpcSectorProcessor(bool b=false){_saveRaw=b;}
  ~PndTpcSectorProcessor();

  // Operators
  

  // Accessors -----------------------
  unsigned int getNPads() const {return _pproc.size();}
  const padprocessor* getPP(unsigned int id){return _pproc[id];}
  unsigned int getId() const {return _SectorId;}
  

  // Modifiers -----------------------
  void Init(PndTpcPadPlane* p,
	    unsigned int id,
	    std::vector<PndTpcCluster*>* ob);
  void putDigi(PndTpcDigi* d){_digi_buffer.push_back(d);}

  // Operations ----------------------
  void process();
  void reset();

private:

  // Private Data Members ------------
  std::map<unsigned int, padprocessor*> _pproc;
  std::map<unsigned int, padprocessor*> _activepads;
  
  std::vector<std::vector<PndTpcDigi*>*> _cluster_buffer;
  std::vector<PndTpcCluster*>* _output_buffer;
  
  PndTpcPadPlane* _padplane;
  unsigned int _SectorId;

  std::vector<PndTpcDigi*> _digi_buffer;

  bool _saveRaw;

  // Private Methods -----------------
  void cog(); // center of gravity

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
