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
#include <cmath>
#include <iostream>

// Collaborating Class Headers --------
#include "TORPadProcessor.h"

// Class Member definitions -----------

//#define DEBUG 1 

ppstate_compare::ppstate_compare(padprocessor* pp)
  : ppstate(pp), fDiffFactor(1.), fTimeCut(0)
{}

std::string
ppstate_compare::heartbeat()
{
  int n=fparent->fneighbours.size();
  double largestamp=fparent->amp()*fDiffFactor;
  bool lonely=true;
  fparent->fiscenter=true;
  for(int i=0;i<n;++i){
    if(fparent->fneighbours[i]->amp()!=0)lonely=false;
    if(largestamp<=fparent->fneighbours[i]->amp()){
      // check if this neighbour is sending me data
      padprocessor* nei=fparent->fneighbours[i];
      if(nei->fdominant_neighb!=-1){
	if(nei->fneighbours[nei->fdominant_neighb]==fparent){
	  //assert(nei->amp()==fparent->amp());
	  std::cout<<"already getting data"<<std::endl;
	  continue;
	}
      }
      // check if neighbouring time is far away
      if(fabs(nei->t()-fparent->t())>fTimeCut){
#ifdef DEBUG
	  std::cout<<"TORPPState_Compare :: applying time cut" << std::endl;
	  std::cout<< nei->t() << "  " 
		   << fparent->t() << "    cut=" 
		     << fTimeCut << std::endl;
#endif
	continue;
      }
      largestamp=nei->amp();
      fparent->fiscenter=false;
      fparent->fdominant_neighb=i;
    }
  } // end loop over neighbours
  //if(lonely && n<6){ // kill this one it's a boundary deffect
  //  fparent->reset();
  //  fparent->setState("initial");
  //return "initial";
  //}
  if(!fparent->fiscenter){
    fparent->setState("send");
    return "send";
  }
  else fparent->setState("wait");
  return "wait";
}

