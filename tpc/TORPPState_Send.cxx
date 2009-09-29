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
  : ppstate(pp), fc(0)
{}

std::string
ppstate_send::heartbeat()
{
  unsigned int ndata=fparent->ndata();
  if(ndata==0){
    if(fc>5){
      // check 
      int n=fparent->fneighbours.size();
      for(int i=0;i<n;++i){
	if(fparent->fneighbours[i]->fdata.size()!=0)
	  std::cout<<"Pad"<<fparent->fmyid
		   <<" Neighb still has data("
		   <<fparent->fneighbours[i]->fdata.size()
		   <<") and is in state "
		   <<fparent->fneighbours[i]->getCurrentState()<<std::endl;
      }
      
      fparent->setState("end");
      fc=0;
      return "end";
    }
    else {
      ++fc;
      fparent->setState("send");
      return "send";
    }
  }
  else {
    for(unsigned int idata=0; idata<ndata; ++idata){
      fparent->fneighbours[fparent->fdominant_neighb]->put(fparent->fdata[idata]);
    }
    --fc;
    fparent->fdata.clear();
    fparent->fmydata=NULL;
    fparent->setState("send");
    return "send";
  }
}

