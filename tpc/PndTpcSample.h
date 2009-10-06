//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      TPC digitization:
//      Output of ADC
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//	Xiaodong Zhang       TUM       (modify it on Oct. 06, 2009) 
//
//-----------------------------------------------------------

#ifndef TPCSAMPLE_HH
#define TPCSAMPLE_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include "McIdCollection.h"

// Collaborating Class Declarations --


class PndTpcSample : public TObject{
public:

  // Constructors/Destructors ---------
  PndTpcSample();
  PndTpcSample(const int t, 
	    const int Amp, 
	    const unsigned int PadID, 
	    const McIdCollection& mcid); 
  PndTpcSample(const int t,                       
            const int Amp,
            const unsigned int PadID,
            const unsigned int ElectronicChId);// added by X.Zhang, Oct. 06, 2009
  ~PndTpcSample();

  // Operators
  friend bool operator== (const PndTpcSample& lhs, const PndTpcSample& rhs);
  friend bool operator< (const PndTpcSample& lhs, const PndTpcSample& rhs);
  friend std::ostream& operator<< (std::ostream& s, const PndTpcSample& me);

  // Accessors -----------------------
  int amp() const {return famp;}
  int t() const {return ft;}
  unsigned int padId() const {return fpadId;}
  unsigned int electronicChId() const {return felectronicChId;}  //added by X.Zhang, Oct. 06, 2009 
  const McIdCollection& mcId() const {return fmcId;}

  // Modifiers -----------------------


  // Operations ----------------------

private:

  // Private Data Members ------------
  int famp;
  int ft;
  unsigned int fpadId;
  unsigned int felectronicChId;                // added by X. Zhang, Oct. 06, 2009 

  McIdCollection fmcId;
  // Private Methods -----------------

public:
  ClassDef(PndTpcSample,2)

};

#endif

//--------------------------------------------------------------
// $Log: PndTpcSample.hh,v $
//
// Revision 1.5 2009/10/06 10:47 X. Zhang
// add the felectronicChId member,
//	the new constructor, and
//	the electronicChId() function.
// 
// Revision 1.4  2006/02/24 14:39:34  sneubert
// moved McIds to package MciData
//
// Revision 1.3  2006/02/24 13:38:00  sneubert
// Code revision done
//
// Revision 1.2  2006/01/29 20:26:30  sneubert
// initial creation
//
// Revision 1.1  2005/09/14 22:31:35  sneubert
// added PndTpcSample
//
//--------------------------------------------------------------
