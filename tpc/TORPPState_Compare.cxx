//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class ppstate_compare
//      see ppstate_compare.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "TORPPState_Compare.h"

// C/C++ Headers ----------------------
#include "assert.h"
#include <iostream>

// Collaborating Class Headers --------
#include "TORPadProcessor.h"

// Class Member definitions -----------

ppstate_compare::ppstate_compare(padprocessor* pp)
  : ppstate(pp)
{}

std::string
ppstate_compare::heartbeat()
{
  int n=_parent->_neighbours.size();
  double largestamp=_parent->amp();
  bool lonely=true;
  _parent->_iscenter=true;
  for(int i=0;i<n;++i){
    if(_parent->_neighbours[i]->amp()!=0)lonely=false;
    if(largestamp<_parent->_neighbours[i]->amp()){
      // check if this neighbour is sending me data
      padprocessor* nei=_parent->_neighbours[i];
      if(nei->_dominant_neighb!=-1){
	if(nei->_neighbours[nei->_dominant_neighb]==_parent){
	  //assert(nei->amp()==_parent->amp());
	  std::cout<<"already getting data"<<std::endl;
	  continue;
	}
      }
      largestamp=_parent->_neighbours[i]->amp();
      _parent->_iscenter=false;
      _parent->_dominant_neighb=i;
    }
  } // end loop over neighbours
  //if(lonely && n<6){ // kill this one it's a boundary deffect
  //  _parent->reset();
  //  _parent->setState("initial");
  //return "initial";
  //}
  if(!_parent->_iscenter){
    _parent->setState("send");
    return "send";
  }
  else _parent->setState("wait");
  return "wait";
}

