//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Data Representation of an Analog Signal created by the TPC
//      GEM readout. Signal shape after Preamp/Shaper 
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCSIGNAL_HH
#define TPCSIGNAL_HH

// Base Class Headers ----------------
#include "FairMultiLinkedData.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include "McId.h"

// Collaborating Class Declarations --
class PndTpcAvalanche;


class PndTpcSignal : public FairMultiLinkedData {
public:

  // Constructors/Destructors ---------
  PndTpcSignal();
  PndTpcSignal(double T, double Amp, 
	    unsigned int PadID, unsigned int MCEventID, PndTpcAvalanche* Mo);
  ~PndTpcSignal(){;}
  
  // Operators
  friend bool operator== (const PndTpcSignal& lhs, const PndTpcSignal& rhs);
  friend bool operator< (const PndTpcSignal& lhs, const PndTpcSignal& rhs);
  friend std::ostream& operator<< (std::ostream& s, const PndTpcSignal& me);

  // Accessors -----------------------
  double t() const {return ft;}
  double amp() const {return famp;}
  unsigned int padId() const {return fpadId;}
  unsigned int mcTrackId() const; 
  unsigned int mcSecId() const;
  unsigned int mcHitId() const; 
  unsigned int mcEventId() const {return fmcEventId;}
  McId mcId() const;
  PndTpcAvalanche* mother() const {return fmother;}
  
  // Modifiers -----------------------
  void sett(const double T) {ft=T;}
  void setamp(const double A) {famp=A;}
  void setpadId(const unsigned int PadID) {fpadId=PadID;}
  void setmcTrackId(const unsigned int ID) {fmcTrackId=ID;}
  void setmcSecId(const unsigned int ID) {fmcSecId=ID;}
  void setmcHitId(const unsigned int ID) {fmcHitId=ID;}
  void setmcEventId(const unsigned int ID) {fmcEventId=ID;}
 
  // Operations ----------------------

private:

  // Private Data Members ------------
  double ft;  // Start time
  double famp;  // Amplitude
  unsigned int fpadId;
  unsigned int fmcTrackId;
  unsigned int fmcSecId;
  unsigned int fmcHitId;
  unsigned int fmcEventId;
  PndTpcAvalanche* fmother;

  // Private Methods -----------------

public:
  ClassDef(PndTpcSignal,2)

};

// Sorting Functor
class PndTpcSignalComparePadID {
  public:
    PndTpcSignalComparePadID(){;}
    ~PndTpcSignalComparePadID(){;}
    bool operator()(PndTpcSignal* s1, PndTpcSignal* s2){return s1->padId()<s2->padId();}
  };


#endif

//--------------------------------------------------------------
// $Log: PndTpcSignal.hh,v $
// Revision 1.8  2006/02/24 14:39:34  sneubert
// moved McIds to package MciData
//
// Revision 1.7  2006/02/24 13:38:00  sneubert
// Code revision done
//
// Revision 1.6  2006/02/23 11:45:17  sneubert
// CodeRevision: revised PndTpcGHit
//
// Revision 1.5  2006/01/29 20:26:30  sneubert
// initial creation
//
// Revision 1.4  2005/09/14 22:29:30  sneubert
// added functor
//
// Revision 1.3  2005/08/22 09:33:41  sneubert
// modified for MCInfo
//
// Revision 1.2  2005/08/18 10:08:37  sneubert
// fixed missing constructor / destructor
//
// Revision 1.1  2005/08/18 09:34:15  sneubert
// initial import
//
//--------------------------------------------------------------
