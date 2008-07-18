//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Abstract Interface for the Nystrom system equation
//       u''=g(u,u')
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

#ifndef ABSNYSTROMEQM_HH
#define ABSNYSTROMEQM_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
#include "TVectorT.h"

class AbsNystromEQM {
public:

  // Constructors/Destructors ---------
  AbsNystromEQM(){;}
  virtual ~AbsNystromEQM(){;}

  virtual TVectorT<double> eval(const TVectorT<double>&u,
				const TVectorT<double>& uprim, 
				const TVectorT<double>& par) = 0; 

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
