//-----------------------------------------------------------
// File and Version Information:
// $Id: PndTpcFrontend.hh,v 1.6 2006/03/10 15:57:17 sneubert Exp $
//
// Description:
//      TPC Digitization
//      Environment class for TPC frontend (Amplifier/Shaper/ADC/DSP)
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

#ifndef TPCFRONTEND_HH
#define TPCFRONTEND_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --

class PndTpcFrontend {
public:

  // Constructors/Destructors ---------
  PndTpcFrontend(const double tint,
	      const double tdiff,
	      const double tsig,
	      const double adcThreshold, 
	      const double adcmax,
	      const int adcbits,
	      const double samplingfreq_MHz,
	      const double t0,
	      const int timebits=16,
	      const unsigned int PSAthreshold=5); 
  ~PndTpcFrontend();

  // Operators
  friend std::ostream& operator<< (std::ostream& s, const PndTpcFrontend& me);

  // Accessors -----------------------
  double adcThreshold() const {return _adcThreshold;}
  double samplingFrequency() const {return 1000./_dt;}
  double t0() const {return _t0;}
  double dt() const {return _dt;}
  double adcbits() const {return _adcbits;}
  double adcmax() const {return _adcmax;}
  double adcstep() const {return _adcstep;}
  unsigned int psaThreshold() const {return _psaThreshold;}
  double tdiff() const {return _tdiff;}
  double tint() const {return _tint;}
  double tsig() const {return _tsig;}
  // Modifiers -----------------------
  void setAdcThreshold(double newThreshold) { _adcThreshold=newThreshold;	} 

  // Operations ----------------------
  unsigned int Clock(double const t) const;
  double ClockFine(double const t) const;
  double TimeAtClock(double const t) const;
  double Clock2Time(unsigned int const clock) const;
  unsigned int A2D(double const amp) const;

private:

  // Private Data Members ------------
  double _adcThreshold;
  double _adcmax;         // value at adc input which gives full count
  int _adcbits;           // number of adc bits
  double _adcstep;        // range between two adc counts
  unsigned int _maxcounts;
  double _dt;              // time between two samples
  double _t0;
  int _timebits;          // number of bits in timestamp
  unsigned int _maxsamples;
  unsigned int _psaThreshold;
  // preamp time constants
  double _tint;
  double _tdiff;
  double _tsig;

  // Private Methods -----------------
  // disallow
  PndTpcFrontend& operator=(const PndTpcFrontend&);
  PndTpcFrontend(const PndTpcFrontend&);
};

#endif

//--------------------------------------------------------------
// $Log: PndTpcFrontend.hh,v $
// Revision 1.6  2006/03/10 15:57:17  sneubert
// dropped pulseshape out of PndTpcFrontend
//
// Revision 1.5  2005/12/16 15:49:39  sneubert
// Some refactorisation done.
//
// Revision 1.4  2005/10/17 16:16:38  sneubert
// added PSAThreshold
//
// Revision 1.3  2005/10/13 07:45:53  sneubert
// event mixing bug fixed, sampling working
//
// Revision 1.2  2005/09/22 15:46:09  sneubert
// *** empty log message ***
//
// Revision 1.1  2005/09/14 22:27:19  sneubert
// added frontend to package
//
//--------------------------------------------------------------
