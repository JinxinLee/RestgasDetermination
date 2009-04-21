//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcPSA_TOT1
//      see PndTpcPSA_TOT1.hh for details
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
#include "PndTpcPSA_TOT1.h"
#include "McIdCollection.h"

// C/C++ Headers ----------------------
#include "assert.h"
#include <algorithm>
#include <iostream>
#include <vector>

#define _C1 0
#define _C2 0
#define _C3 0
#define _C4 0
#define _PULSEDELAYFACTOR 0.3

using std::max;

// Collaborating Class Headers --------
#include "PndTpcSample.h"
#include "PndTpcDigi.h"

// Class Member definitions -----------
PndTpcPSA_TOT1::PndTpcPSA_TOT1()
  : PndTpcAbsPSAStrategy(), _currentPadID(0), _inprogress(false),
  _amp(0),_t(0),_threshold(10)
{}


PndTpcDigi*
PndTpcPSA_TOT1::ProcessNext(const PndTpcSample* sample)
{
  return NULL;
}

void PndTpcPSA_TOT1::Process(const std::vector<PndTpcSample*> & samples, 
						  std::vector<PndTpcDigi*>& digis,
						  double padThreshold) {
  /*
  for(int i=0;i<samples.size();i++) {
	std::cout << samples[i]->padId() << " " << samples[i]->amp() << std::endl;
  }
  */
	//std::cout << "Using PSA_TOT strategy!!!!" << std::endl;
  McIdCollection mcid;
  mcid.ClearData();

  int amp=0;
  int prevamp=0;

  int deriv_0=0;  
  int deriv_1=0;  
  int deriv_2=0;  

  bool inpulse=false;
  std::vector<PndTpcSample*> samplesInPulse;

  //  std::cout << __FILE__ << " " << __LINE__ << std::endl;

  for(int i=0;i<samples.size();i++) {
	//	std::cout << __FILE__ << " " << __LINE__ << std::endl;

	amp=samples[i]->amp();
	if(i>2) {
	  deriv_2=deriv_1;
	}
	if(i>1) {
	  deriv_1=deriv_0;
	}
	if(i>0) {	//true for second sample
	  //	  std::cout << __FILE__ << " " << __LINE__ << std::endl;
	  prevamp=samples[i-1]->amp();
	  deriv_0=amp-prevamp;
	}


	//	std::cout << amp << " " << prevamp << " " << deriv_0 << " " << deriv_1 << std::endl;
	
	///////////////////////////
	if(!inpulse) {
	  if(deriv_0>_C1 && deriv_1>=_C2 && amp>padThreshold) {//start new pulse	//false for first sample (deriv_0>_C1)
		inpulse = true;											//true for second sample, if first sample amp
		mcid.ClearData();											//smaler second signal amp
		//mcid.AddID(samples[i-2]->mcId());
		//samplesInPulse.push_back(samples[i-2]);
		mcid.AddIDCollection(samples[i-1]->mcId(),1);
		samplesInPulse.push_back(samples[i-1]);
		mcid.AddIDCollection(samples[i]->mcId(),1);
		samplesInPulse.push_back(samples[i]);
	  }
	}
	else {// in active pulse
	  if((deriv_0>=_C3 && deriv_1<_C4) || amp<padThreshold) {//done with pulse
		inpulse=false;
		double t0,A,length;
		processPulse(samplesInPulse,t0,A,length);
		samplesInPulse.clear();
		mcid.Renormalize();
		PndTpcDigi* digi=new PndTpcDigi(A,t0,samples[i]->padId(),mcid);
		digi->tlength(length);
		digis.push_back(digi);

		if(deriv_0>_C1 && amp>padThreshold) {//start new pulse
		  inpulse = true;
		  mcid.ClearData();
		  //mcid.AddID(samples[i-2]->mcId());
		  //samplesInPulse.push_back(samples[i-2]);
		  mcid.AddIDCollection(samples[i-1]->mcId(),1);
		  samplesInPulse.push_back(samples[i-1]);
		  mcid.AddIDCollection(samples[i]->mcId(),1);
		  samplesInPulse.push_back(samples[i]);
		}

		
	  }
	  else {
		mcid.AddIDCollection(samples[i]->mcId(),1);
		samplesInPulse.push_back(samples[i]);
	  }
	}
	///////////////////////////

  }

  /*
  int startIndex;
  
  for(int i=0;i<samples.size();i++) {
	unsigned int newPadID=samples[i]->padId();
	unsigned int newamp=samples[i]->amp();

	if(_inprogress){
	  if(newPadID==_currentPadID && newamp>=_threshold){
		if(newamp>_amp){
		  _amp=newamp;
		}
		_mcid.AddID(samples[i]->mcId());
	  }
	  else{
		PndTpcDigi* digi=new PndTpcDigi(_amp,_t,_currentPadID,_mcid);
		_inprogress=false;

		digis.push_back(digi);
	  }
	}
	else if(newamp>_threshold){ // start new pulse!
	  _currentPadID=newPadID;
	  _amp=newamp;
	  _t=samples[i]->t();
	  _mcid.ClearData();
	  _mcid.AddID(samples[i]->mcId());
	  startIndex = i;
	  _inprogress=true;
	}
	
	
  }
  */
}

void PndTpcPSA_TOT1::processPulse(std::vector<PndTpcSample*> samples,
							   double& t0,double& A, double& length){
  A=0.;
  t0=0.;
  for(unsigned int i=0;i<samples.size();i++) {
	A+=samples[i]->amp();
  }
  length=samples[samples.size()-1]->t()-samples[0]->t();
  t0=samples[0]->t() + _PULSEDELAYFACTOR*length;

}

