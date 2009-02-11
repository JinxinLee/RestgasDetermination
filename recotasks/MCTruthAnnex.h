//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Adds information to the MCtruth objects
//      like how many hits in a detector
//      has this this track been reconstructed
//      etc...
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef MCTRUTHANNEX_HH
#define MCTRUTHANNEX_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --
class PndMCTrack;



class MCTruthAnnex : public TObject {
public:

  // Constructors/Destructors ---------
  MCTruthAnnex();
  virtual ~MCTruthAnnex();

  

  // Accessors -----------------------
  double getPndTpcLength() const {return _tpclength;}
  int getPndTpcHits() const {return _tpchits;}
  bool isReco() const {return _isReco;}

  // Modifiers -----------------------
  void setPndTpcLength(double l){_tpclength=l;}
  void setPndTpcHits(int n){_tpchits=n;}
  void setReco(bool flag=true){_isReco=flag;}

  // Operations ----------------------


private:

  // Private Data Members ------------
  double _tpclength;
  int _tpchits;
  bool _isReco;
  

  // Private Methods -----------------

public:
  ClassDef(MCTruthAnnex,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
