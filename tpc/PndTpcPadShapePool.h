//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Owns the PndTpcPadShapes, can read them from a file and returns the 
//      right shape after getting its ID.
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Cristoforo Simonetto    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCPADSHAPEPOOL_HH
#define TPCPADSHAPEPOOL_HH

// Base Class Headers ----------------

// Collaborating Class Headers -------
#include <vector>
#include <ostream>
#include <Rtypes.h>
// Collaborating Class Declarations --
class PndTpcAbsPadShape;
class PndTpcGem;

class PndTpcPadShapePool {
public:

  // Constructors/Destructors ---------
  PndTpcPadShapePool();
  PndTpcPadShapePool(const char* const filename,
		    const PndTpcGem& gem, 
		    const double frange,      
		    const double fstep,
		    const double fintStep);
  PndTpcPadShapePool(const char* const filename);
  ~PndTpcPadShapePool();

  // Operators
  friend std::ostream& operator<< (std::ostream& s, const PndTpcPadShapePool& me);
  

  // Accessors -----------------------
  PndTpcAbsPadShape* GetPadShape(const unsigned int fID) const;

  // Modifiers -----------------------
  //PadShapePool will take ownership
  // The user is responsible for Building the PRLookupTable
  void AddPadShape(PndTpcAbsPadShape* padshape);
  void RemovePadShape(const unsigned int fID);//deletes it
  //PadShapePool will take ownership
  // PadShapePool will build the PRLookupTables
  void ReadFromFile(const char* const filename);
  void BuildLookupTable(const PndTpcGem& gem,
			const double frange,      
			const double fstep,
			const double fintStep);
private:

  // Private Data Members ------------
  std::vector<PndTpcAbsPadShape*> references;

  // Private Methods -----------------
  bool TryAddingPadShape(PndTpcAbsPadShape* padshape);
  bool ReadLine(std::ifstream& infile);
  PndTpcAbsPadShape* ReadShape(const char* const line);
  PndTpcAbsPadShape* ReadPolygon(const char* const line);

public:

  ClassDef(PndTpcPadShapePool,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
