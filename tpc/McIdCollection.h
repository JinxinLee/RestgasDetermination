//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Class that stores multiple Monte Carlo IDs which can arise
//      Through event mixing and pileup
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

#ifndef MCIDCOLLECTION_HH
#define MCIDCOLLECTION_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include <vector>

#include "McId.h"

// Collaborating Class Declarations --


class McIdCollection : public TObject {
public:

  // Constructors/Destructors ---------
  McIdCollection();
  McIdCollection(const McIdCollection&);
  virtual ~McIdCollection();

  // Operators
  //McIdCollection& operator=(const McIdCollection&);

  friend std::ostream& operator<< (std::ostream& s, const McIdCollection& me);

  // Accessors -----------------------
  unsigned int nIDs() const {return fMCIDs.size();}
  const McId& ID(unsigned int i) const {return fMCIDs[i];}

  // Modifiers -----------------------

  // this will ensure, that each entry will be there only once!
  // if weight is set to false additional ids do not increase the weight!
  bool AddID(const McId& id, double weight=1); // return true if added, false if already in collection
  bool AddIDCollection(const McIdCollection& coll,double weight=1);
  // Operations ----------------------

  void ClearData(){fMCIDs.clear();}
  void ShiftEventIds(int shift); // used for eventmixing

  double Renormalize(); // renormalize weights
  double TotalWeight() const;
  double MaxRelWeight() const;
  McId DominantID() const;
private:

  // Private Data Members ------------
  std::vector<McId> fMCIDs;
  
  // Private Methods -----------------

public:
  ClassDef(McIdCollection,2)

};

#endif

//--------------------------------------------------------------
// $Log: McIdCollection.hh,v $
// Revision 1.1.1.1  2006/02/24 14:37:16  sneubert
// initial import
//
// Revision 1.1  2006/01/29 20:26:30  sneubert
// initial creation
//
//--------------------------------------------------------------
