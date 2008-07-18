//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Monte carlo ID which can be used by event mixing applications
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

#ifndef MCID_HH
#define MCID_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --



class McId : public TObject{
public:

  // Constructors/Destructors ---------
  McId();

  McId(const unsigned int eventid,
       const unsigned int trackid,
       const double weight=1);
  virtual ~McId();

  // Operators
  friend bool operator== (const McId& lhs, const McId& rhs);
  friend bool operator< (const McId& lhs, const McId& rhs);
  friend std::ostream& operator<< (std::ostream& s, const McId& me);

  // Accessors -----------------------
  unsigned int mceventID() const {return _mceventID;}
  unsigned int mctrackID() const {return _mctrackID;}
  double weight() const {return _weight;}

  // Modifiers -----------------------
  void setweight(double w) {_weight=w;}

  // Operations ----------------------
  double IncWeight(double w=1) {return _weight+=w;}

private:

  // Private Data Members ------------
  unsigned int _mceventID;
  unsigned int _mctrackID;
  double _weight;
  
  // Private Methods -----------------

public: 
  ClassDef(McId,2)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
