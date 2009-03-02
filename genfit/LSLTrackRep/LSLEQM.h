//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      equation of motion for local straight line track rep
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

#ifndef LSLEQM_HH
#define LSLEQM_HH

// Base Class Headers ----------------
#include "AbsNystromEQM.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class AbsBFieldIfc;

class LSLEQM : public AbsNystromEQM {
public:

  // Constructors/Destructors ---------
  LSLEQM(AbsBFieldIfc* f):_field(f){;}
  ~LSLEQM(){;}

  TVectorT<double> eval(const TVectorT<double>&u,const TVectorT<double>& uprim, const TVectorT<double>& par); 

private:
  AbsBFieldIfc* _field;
  
};


#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
