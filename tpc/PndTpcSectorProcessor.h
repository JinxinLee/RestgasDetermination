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
  PndTpcSectorProcessor(bool b=false, bool dm=false){fsaveRaw=b; fDataMode=dm;}
  ~PndTpcSectorProcessor();

  // Operators
  

  // Accessors -----------------------
  unsigned int getNPads() const {return fpproc.size();}
  const padprocessor* getPP(unsigned int id){return fpproc[id];}
  unsigned int getId() const {return fSectorId;}
  

  // Modifiers -----------------------
  void Init(PndTpcPadPlane* p,
	    unsigned int id,
	    std::vector<PndTpcCluster*>* ob,
	    double diffFactor=1.); // minimal relative difference for maximum
  void putDigi(PndTpcDigi* d){fdigi_buffer.push_back(d);}

  // Operations ----------------------
  void process();
  void reset();

private:

  // Private Data Members ------------
  std::map<unsigned int, padprocessor*> fpproc;
  std::map<unsigned int, padprocessor*> factivepads;
  
  std::vector<std::vector<PndTpcDigi*>*> fcluster_buffer;
  std::vector<PndTpcCluster*>* foutput_buffer;
  
  PndTpcPadPlane* fpadplane;
  unsigned int fSectorId;

  std::vector<PndTpcDigi*> fdigi_buffer;

  bool fsaveRaw;
  bool fDataMode;

  // Private Methods -----------------
  void cog(); // center of gravity

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
