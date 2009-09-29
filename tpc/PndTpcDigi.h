//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      PndTpc Digitization
//      Persisted data. Raw data
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


#ifndef TPCDIGI_HH
#define TPCDIGI_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include "McIdCollection.h"
#include <iostream>

// Collaborating Class Declarations --


class PndTpcDigi : public TObject {
public:

  // Constructors/Destructors ---------
  PndTpcDigi();
  PndTpcDigi(const PndTpcDigi&);
  PndTpcDigi(const double Amp,
		  const double t,
		  const unsigned int PadID,
		  const McIdCollection& mcid);
  ~PndTpcDigi(){;}

  // Operators
  friend bool operator== (const PndTpcDigi& lhs, const PndTpcDigi& rhs);
  friend bool operator< (const PndTpcDigi& lhs, const PndTpcDigi& rhs);
  friend std::ostream& operator<< (std::ostream& s, const PndTpcDigi& me);

  // Accessors -----------------------
  double amp() const {return famp;}
  double t() const {return ft;}
  unsigned int padId() const {return fpadID;}
  const McIdCollection& mcId() const {return fmcid;}
  double tlength() const {return ftlength;}
  // Modifiers -----------------------
  void amp(double val)  {famp=val;}
  void t(double val) { ft=val;}
  void padId(unsigned int val) {fpadID=val;}
  void mcId(const McIdCollection& val) {fmcid=val;}
  unsigned int nMcIds() const {return fmcid.nIDs();}
  double maxMcWeight() const {return fmcid.MaxRelWeight();}
  void tlength(double dt){ftlength=dt;}


  // Operations ----------------------
  void Print() const {
    std::cout << "===== PndTpcDigi::print() =====" << std::endl;
    std::cout << "padId " << padId() << " | amp " << amp() << " | t " << t() << std::endl;
  }
private:

  // Private Data Members ------------
  double famp;
  double ft;
  unsigned int fpadID;

  double ftlength; // optional: range in t from which digi was constructed //TODO: Initalize correctly?

  McIdCollection fmcid;
  // Private Methods -----------------

public:
  ClassDef(PndTpcDigi,2)

};

#endif

//--------------------------------------------------------------
// $Log: PndTpcDigi.hh,v $
// Revision 1.4  2006/02/24 14:39:34  sneubert
// moved McIds to package MciData
//
// Revision 1.3  2006/02/24 13:38:00  sneubert
// Code revision done
//
// Revision 1.2  2006/01/29 20:26:30  sneubert
// initial creation
//
// Revision 1.1  2005/10/14 12:28:35  sneubert
// added PndTpcDigi
//
//--------------------------------------------------------------
