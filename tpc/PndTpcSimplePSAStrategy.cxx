//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcSimplePSAStrategy
//      see PndTpcSimplePSAStrategy.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// This Class' Header ------------------
#include "PndTpcSimplePSAStrategy.h"

// C/C++ Headers ----------------------
#include "assert.h"
#include <algorithm>
#include <iostream>


using std::max;

// Collaborating Class Headers --------
#include "PndTpcSample.h"
#include "PndTpcDigi.h"

// Class Member definitions -----------
PndTpcSimplePSAStrategy::PndTpcSimplePSAStrategy(const unsigned int threshold)
  : PndTpcAbsPSAStrategy(), _currentPadID(0), _inprogress(false),
    _amp(0),_t(0),_threshold(threshold)
{}


PndTpcDigi*
PndTpcSimplePSAStrategy::ProcessNext(const PndTpcSample* sample)
{
  unsigned int newPadID=sample->padId();
  unsigned int newamp=sample->amp();
  //  std::cout << newPadID << " " << newamp << std::endl;
  if(_inprogress){
    if(newPadID==_currentPadID && newamp>=_threshold){
      if(newamp>_amp){
	_amp=newamp;
	//_t=sample->t();
      }
      _mcid.AddIDCollection(sample->mcId(),newamp);
    }
    else{
      PndTpcDigi* digi=new PndTpcDigi(_amp,_t,_currentPadID,_mcid);
      if(newamp>=_threshold){ // start new pulse on different pad
		_currentPadID=newPadID;
		_amp=sample->amp();
		_t=sample->t();
      }
      else _inprogress=false;
      return digi;
    }
  }
  else if(newamp>_threshold){ // start new pulse!
    _currentPadID=newPadID;
    _amp=newamp;
    _t=sample->t();
    _mcid.ClearData();
    _mcid.AddIDCollection(sample->mcId(),newamp);
    _inprogress=true;
  }
  return 0;
}

void PndTpcSimplePSAStrategy::Process(const std::vector<PndTpcSample*> & samples, 
								   std::vector<PndTpcDigi*>& digis,
								   double padThreshold) {

  /*
  for(int i=0;i<samples.size();i++) {
	std::cout << samples[i]->padId() << " " << samples[i]->amp() << std::endl;
  }
  */
	std::cout << "Using Simple PSA strategy!" << std::endl;
  int startIndex;
  
  for(int i=0;i<samples.size();i++) {
	unsigned int newPadID=samples[i]->padId();
	unsigned int newamp=samples[i]->amp();

	if(_inprogress){
	  if(newPadID==_currentPadID && newamp>=_threshold){
		if(newamp>_amp){
		  _amp=newamp;
		}
		_mcid.AddIDCollection(samples[i]->mcId(),newamp);
	  }
	  else{
		PndTpcDigi* digi=new PndTpcDigi(_amp,_t,_currentPadID,_mcid);
		digi->tlength(samples[i]->t()-_t);
		_inprogress=false;

		digis.push_back(digi);
	  }
	}
	else if(newamp>_threshold){ // start new pulse!
	  _currentPadID=newPadID;
	  _amp=newamp;
	  _t=samples[i]->t();
	  _mcid.ClearData();
	  _mcid.AddIDCollection(samples[i]->mcId(),newamp);
	  startIndex = i;
	  _inprogress=true;
	}
	
	
  }

}

