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
  PndTpcSample(const int T, 
	    const int Amp, 
	    const unsigned int PadID, 
	    const McIdCollection& mcid); 
  PndTpcSample(const int T,                       
            const int Amp,
            const unsigned int PadID);
  ~PndTpcSample();

  // Operators
  friend bool operator== (const PndTpcSample& lhs, const PndTpcSample& rhs);
  friend bool operator< (const PndTpcSample& lhs, const PndTpcSample& rhs);
  friend std::ostream& operator<< (std::ostream& s, const PndTpcSample& me);

  // Accessors -----------------------
  int amp() const {return famp;}
  int   t() const {return ft;}
  int padId() const {return fpadId;}
  const McIdCollection& mcId() const {return fmcId;}
  //*** next four functions added by x. zhang on 08.10.2009 ****//
  unsigned int  sourceId() const { return fsourceId; }
  unsigned int     adcId() const { return fadcId; }
  unsigned int    chipId() const { return fchipId; }
  unsigned int channelId() const { return fchannelId; }

  // Modifiers -----------------------
  //*** next four functions added by x. zhang on 08.10.2009 ****//
  void setAmp(unsigned int Amp) { famp = Amp; }
  void setSample(unsigned int s) { ft = s; }
  void setPadId(int pi) { fpadId = pi; }
  void setSourceId(unsigned int sId) { fsourceId = sId; }
  void setADCId(unsigned int aId) { fadcId = aId; }
  void setChipId(unsigned int cId) { fchipId = cId; }
  void setChannelId(unsigned int chId) { fchannelId = chId; }

  // Operations ----------------------
  void reset();

private:

  // Private Data Members ------------
  int famp;
  int ft;
  int fpadId;
  unsigned int fsourceId;
  unsigned int fadcId;
  unsigned int fchipId;
  unsigned int fchannelId;       // added by X. Zhang, Oct. 29, 2009               

  McIdCollection fmcId;
  // Private Methods -----------------

public:
  ClassDef(PndTpcSample,2)

};

#endif

//--------------------------------------------------------------
// $Log: PndTpcSample.hh,v $
// Revision 1.9 2009/12/03 12:12 x.zhang
//   delete the electronic Id
//
// Revision 1.8 2009/11/02 15:41 X.Zhang
//   add operator, reset().
//
// Revision 1.7 2009/10/29 09.24 X.Zhang
//   add the sourceId, adcId, chipId, and channelId data \
//   and corresponding functions.
// Revision 1.6 2009/10/08 20:51 x.zhang
// add four set member functions
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
