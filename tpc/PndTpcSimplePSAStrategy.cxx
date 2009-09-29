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
  : PndTpcAbsPSAStrategy(), fcurrentPadID(0), finprogress(false),
    famp(0),ft(0),fthreshold(threshold)
{}


PndTpcDigi*
PndTpcSimplePSAStrategy::ProcessNext(const PndTpcSample* sample)
{
  unsigned int newPadID=sample->padId();
  unsigned int newamp=sample->amp();
  //  std::cout << newPadID << " " << newamp << std::endl;
  if(finprogress){
    if(newPadID==fcurrentPadID && newamp>=fthreshold){
      if(newamp>famp){
	famp=newamp;
	//ft=sample->t();
      }
      fmcid.AddIDCollection(sample->mcId(),newamp);
    }
    else{
      PndTpcDigi* digi=new PndTpcDigi(famp,ft,fcurrentPadID,fmcid);
      if(newamp>=fthreshold){ // start new pulse on different pad
		fcurrentPadID=newPadID;
		famp=sample->amp();
		ft=sample->t();
      }
      else finprogress=false;
      return digi;
    }
  }
  else if(newamp>fthreshold){ // start new pulse!
    fcurrentPadID=newPadID;
    famp=newamp;
    ft=sample->t();
    fmcid.ClearData();
    fmcid.AddIDCollection(sample->mcId(),newamp);
    finprogress=true;
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

	if(finprogress){
	  if(newPadID==fcurrentPadID && newamp>=fthreshold){
		if(newamp>famp){
		  famp=newamp;
		}
		fmcid.AddIDCollection(samples[i]->mcId(),newamp);
	  }
	  else{
		PndTpcDigi* digi=new PndTpcDigi(famp,ft,fcurrentPadID,fmcid);
		digi->tlength(samples[i]->t()-ft);
		finprogress=false;

		digis.push_back(digi);
	  }
	}
	else if(newamp>fthreshold){ // start new pulse!
	  fcurrentPadID=newPadID;
	  famp=newamp;
	  ft=samples[i]->t();
	  fmcid.ClearData();
	  fmcid.AddIDCollection(samples[i]->mcId(),newamp);
	  startIndex = i;
	  finprogress=true;
	}
	
	
  }

}

