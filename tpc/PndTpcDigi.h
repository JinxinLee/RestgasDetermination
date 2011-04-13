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
//-----------------------------------------------------------


#ifndef TPCDIGI_HH
#define TPCDIGI_HH

// Base Class Headers ----------------
#include "FairMultiLinkedData.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include "McIdCollection.h"
#include <iostream>
#include <assert.h>

// Collaborating Class Declarations --
#include "PndTpcSample.h"


class PndTpcDigi : public FairMultiLinkedData {
public:

  // Constructors/Destructors ---------
  PndTpcDigi();
  PndTpcDigi(const double Amp,
		  const double T,
		  const unsigned int PadID,
		  const McIdCollection& mcid, const unsigned int index = 0);
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
  void shiftEventIds(int shift){fmcid.ShiftEventIds(shift);}
  double tlength() const {return ftlength;}
  unsigned int index() const {return findex;}
  // Modifiers -----------------------
  void amp(double val)  {famp=val;}
  void t(double val) { ft=val;}
  void padId(unsigned int val) {fpadID=val;}
  void mcId(const McIdCollection& val) {fmcid=val;}
  unsigned int nMcIds() const {return fmcid.nIDs();}
  double maxMcWeight() const {return fmcid.MaxRelWeight();}
  void tlength(double dt){ftlength=dt;}
  void index(unsigned int caindex){findex = caindex;}

  //Samples Vector-------------------
 
  /*
  unsigned int nSample() const {
    return samples.size();
  }
  
  void addSample(const PndTpcSample* d){
    samples.push_back(d);
  }
  
  const PndTpcSample* getSample(int i) const{
    assert (i<samples.size());
    return samples[i];
  }
  */
  
  // Operations ----------------------
  void Print() const {
    std::cout << "===== PndTpcDigi::print() =====" << std::endl;
    std::cout << "padId " << padId() << " | amp " << amp() << " | t " 
	      << t() << std::endl;
  }
  
 private:

  // Private Data Members ------------
  double famp;         /// Amplitude in ADC channels, can be fractional from PSA
  double ft;           /// Time in clock ticks ("samples"), can be fractional from PSA
  unsigned int fpadID;
  unsigned int findex; /// Position of Digi in TClonesArray (needed for FairLinks to work with PndTpcCluster)

  double ftlength; /// optional: range in t from which digi was constructed //TODO: Initalize correctly?

  McIdCollection fmcid;  /// AWESOME neubert invention needed for Event Mixing and not understood by collaboration for past 5 years (as of date spring 2011, lets see how long they take)
  // Private Methods -----------------

  //for optional saving of raw info that went into the cluster
  //std::vector<const PndTpcSample*> samples;


public:
  ClassDef(PndTpcDigi,5)

};

#endif

//--------------------------------------------------------------
// $Log: PndTpcDigi.hh,v $
// 
// Revision 1.5  2010/04/20 11:42:32  mvandenb
// Integration of sample vector
//
// Revision 1.4  2006/02/24 14:39:34  sneubert
// moved McIds to package MciData
///
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
