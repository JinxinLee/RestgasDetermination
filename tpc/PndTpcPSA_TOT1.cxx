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

#define fC1 0
#define fC2 0
#define fC3 0
#define fC4 0
#define fPULSEDELAYFACTOR 0.3

using std::max;

// Collaborating Class Headers --------
#include "PndTpcSample.h"
#include "PndTpcDigi.h"

// Class Member definitions -----------
PndTpcPSA_TOT1::PndTpcPSA_TOT1()
  : PndTpcAbsPSAStrategy(), fcurrentPadID(0), finprogress(false),
  famp(0),ft(0),fthreshold(10)
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

  int time=0;
  int prevtime=0;

  int deriv_0=0;  
  int deriv_1=0;  
  int deriv_2=0;  

  bool inpulse=false;
  std::vector<PndTpcSample*> samplesInPulse;

  //  std::cout << _FILE__ << " " << f_LINE__ << std::endl;

  for(int i=0;i<samples.size();i++) {
	//	std::cout << _FILE__ << " " << f_LINE__ << std::endl;

    //std::cout << "samplt="<<samples[i]->t()<< std::endl;
    //std::cout << "padid="<<samples[i]->padId()<< std::endl;

	amp=samples[i]->amp();
	time=samples[i]->t();
	if(i>2) {
	  deriv_2=deriv_1;
	}
	if(i>1) {
	  deriv_1=deriv_0;
	}
	if(i>0) {	//true for second sample
	  //	  std::cout << _FILE__ << " " << _LINE__ << std::endl;
	  prevamp=samples[i-1]->amp();
	  prevtime=samples[i-1]->t();
	  deriv_0=amp-prevamp;
	}

	

	//	std::cout << amp << " " << prevamp << " " << deriv_0 << " " << deriv_1 << std::endl;
	
	///////////////////////////
	if(!inpulse) {
	  if(deriv_0>fC1 && deriv_1>=fC2 && amp>padThreshold && time-prevtime==1) {//start new pulse	//false for first sample (deriv_0>fC1)
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
	  if((deriv_0>=fC3 && deriv_1<fC4) || amp<padThreshold || time-prevtime>1 || i==samples.size()-1) {//done with pulse
		inpulse=false;
		double t0,A,length;
		processPulse(samplesInPulse,t0,A,length);
		mcid.Renormalize();
		PndTpcDigi* digi=new PndTpcDigi(A,t0,samples[i]->padId(),mcid);
		digi->tlength(length);
		for (int yi=0;yi<samplesInPulse.size();++yi)
		  digi->addSample(*samplesInPulse.at(yi));
		digis.push_back(digi);
		samplesInPulse.clear();

		if(deriv_0>fC1 && amp>padThreshold) {//start new pulse
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

	if(finprogress){
	  if(newPadID==fcurrentPadID && newamp>=fthreshold){
		if(newamp>famp){
		  famp=newamp;
		}
		fmcid.AddID(samples[i]->mcId());
	  }
	  else{
		PndTpcDigi* digi=new PndTpcDigi(famp,ft,fcurrentPadID,fmcid);
		finprogress=false;

		digis.push_back(digi);
	  }
	}
	else if(newamp>fthreshold){ // start new pulse!
	  fcurrentPadID=newPadID;
	  famp=newamp;
	  ft=samples[i]->t();
	  fmcid.ClearData();
	  fmcid.AddID(samples[i]->mcId());
	  startIndex = i;
	  finprogress=true;
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
  t0=samples[0]->t() + fPULSEDELAYFACTOR*length;

}

