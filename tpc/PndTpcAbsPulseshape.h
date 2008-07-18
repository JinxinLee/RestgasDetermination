//-----------------------------------------------------------
// File and Version Information:
// $Id: PndTpcAbsPulseshape.hh,v 1.1 2005/09/22 15:46:09 sneubert Exp $
//
// Description:
//      TPC Digitization
//      abstract Base class for Pulseshapes
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCABSPULSESHAPE_HH
#define TPCABSPULSESHAPE_HH

// Collaborating Class Headers -------


// Collaborating Class Declarations --

class PndTpcAbsPulseshape {
public:

  // Constructors/Destructors ---------
  PndTpcAbsPulseshape(){;}
  virtual ~PndTpcAbsPulseshape(){;}

  // Operators
  
  virtual double operator() (const double t, 
			     const double amp, const double toffset) const = 0;
  virtual double value(const double t, 
		       const double amp, const double toffset) const 
  {return operator()(t,amp,toffset);}

};

#endif

//--------------------------------------------------------------
// $Log: PndTpcAbsPulseshape.hh,v $
// Revision 1.1  2005/09/22 15:46:09  sneubert
// *** empty log message ***
//
//--------------------------------------------------------------
