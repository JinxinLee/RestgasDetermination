//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class ppstate_send
//      see ppstate_send.hh for details
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
#include "TORPPState_Send.h"

// C/C++ Headers ----------------------
#include <iostream>

// Collaborating Class Headers --------
#include "TORPadProcessor.h"

// Class Member definitions -----------

ppstate_send::ppstate_send(padprocessor* pp)
  : ppstate(pp), _c(0)
{}

std::string
ppstate_send::heartbeat()
{
  unsigned int ndata=_parent->ndata();
  if(ndata==0){
    if(_c>5){
      // check 
      int n=_parent->_neighbours.size();
      for(int i=0;i<n;++i){
	if(_parent->_neighbours[i]->_data.size()!=0)
	  std::cout<<"Pad"<<_parent->_myid
		   <<" Neighb still has data("
		   <<_parent->_neighbours[i]->_data.size()
		   <<") and is in state "
		   <<_parent->_neighbours[i]->getCurrentState()<<std::endl;
      }
      
      _parent->setState("end");
      _c=0;
      return "end";
    }
    else {
      ++_c;
      _parent->setState("send");
      return "send";
    }
  }
  else {
    for(unsigned int idata=0; idata<ndata; ++idata){
      _parent->_neighbours[_parent->_dominant_neighb]->put(_parent->_data[idata]);
    }
    --_c;
    _parent->_data.clear();
    _parent->_mydata=NULL;
    _parent->setState("send");
    return "send";
  }
}

