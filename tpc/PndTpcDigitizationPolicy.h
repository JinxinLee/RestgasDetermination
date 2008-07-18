//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Policy class for the digitizition of the TPC in the ADC module
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

#ifndef TPCDIGITIZATIONPOLICY_HH
#define TPCDIGITIZATIONPOLICY_HH

// Collaborating Class Headers -------
#include <vector>

// Collaborating Class Declarations --
class PndTpcSample;
class PndTpcSignal;
class PndTpcFrontend;
class PndTpcAbsPulseshape;

struct PndTpcDigitizationPolicy {
  void Digitize(std::vector< PndTpcSignal* >* SignalVector,
		std::vector< PndTpcSample* >* SampleVector,
		const PndTpcFrontend* Frontend,
		const PndTpcAbsPulseshape* Pulseshape);
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
