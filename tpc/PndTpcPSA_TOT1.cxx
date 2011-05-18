//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcPSA_TOT1
//      see PndTpcPSA_TOT1.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Christian Hoeppner   TUM            (original author)
//      Sverre Doerheim    TUM            
//
//
//-----------------------------------------------------------

// This Class' Header ------------------
#include "PndTpcPSA_TOT1.h"
#include "McIdCollection.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <vector>

// Collaborating Class Headers --------
#include "PndTpcSample.h"
#include "PndTpcDigi.h"

#define DEBUG 1

using std::cout; using std::endl;

PndTpcPSA_TOT1::PndTpcPSA_TOT1()
  : PndTpcAbsPSAStrategy(), fcurrentPadID(0), finprogress(false),
    famp(0),ft(0),fthreshold(10), fNbEmptySampleAllowed(1)
{}

PndTpcDigi*
PndTpcPSA_TOT1::ProcessNext(const PndTpcSample* sample)
{
  return NULL;
}

void PndTpcPSA_TOT1::Process(const std::vector<PndTpcSample*> & samples, 
              std::vector<PndTpcDigi*>& digis,
              double padThreshold) {

  McIdCollection mcid;
  mcid.ClearData();

  unsigned int nsamples = samples.size();

  int amp=0;
  int prevamp=0;

  int time=0;
  int prevtime=0;

  bool inpulse=false;

  int deriv_0=0;  
  int deriv_1=0;  
  int deriv_2=0;  
  bool fallingEdge = false;

  std::vector<PndTpcSample*> samplesInPulse;
  
  for(unsigned int i=0;i<nsamples;++i){ // loop over samples
    amp=samples.at(i)->amp();
    time=samples.at(i)->t();
    deriv_2=deriv_1;
    deriv_1=deriv_0;

    if(i==0){ // first sample
      deriv_0=amp; //setting the derivative for the first sample
      fallingEdge=false;
    }else{ // i>0
      prevamp=samples.at(i-1)->amp();
      prevtime=samples.at(i-1)->t();
      if(time-prevtime!=1){ //if there were empty samples between
        deriv_0=amp;
        fallingEdge=false;
      }else{
        deriv_0=amp-prevamp;
        if(deriv_0<0) fallingEdge=true;
      }
    }
    if(DEBUG) cout<<"Processing sample: "<<i
      <<", a:"<<amp<<", t:"<<time
      <<" der0,1,2: "<<deriv_0<<", "<<deriv_1<<", "<<deriv_2<<endl;

    if(!inpulse){
      // check for starting pulse
      if( ((deriv_0>0 && deriv_1>=0)|| // _/ or //
           (deriv_0>=0 && deriv_1>0)|| // /_ or //
           (deriv_0<0 && deriv_1>0 && prevamp>0)) //  /\       //
           && time-prevtime==1){
        if(DEBUG) cout<<" starting new pulse"<<endl;
        inpulse=true;
        mcid.ClearData();
        if(i>0){ // add previous sample to pulse
          mcid.AddIDCollection(samples[i-1]->mcId(),1);
          samplesInPulse.push_back(samples[i-1]);
        }
        // add sample to pulse
        mcid.AddIDCollection(samples[i]->mcId(),1);
        samplesInPulse.push_back(samples[i]);
      }
    }else{ // sample is in pulse
      //checking for the end of a pulse
      if((deriv_0>0 && deriv_1<0) ||     //local minimum
         (fallingEdge && deriv_0==0) ||  //flat valley, on falling edge
         time-prevtime>1 ||              //gap in the samples
         i==nsamples-1){                 //last sample

        if(i==nsamples-1 && time-prevtime==1){ // add last sample if there is no gap
          mcid.AddIDCollection(samples[i]->mcId(),1);
          samplesInPulse.push_back(samples[i]);
        }

        inpulse=false;
        fallingEdge=false;
        double t0,A,length;
        processPulse(samplesInPulse,t0,A,length);
       
        mcid.Renormalize();
        if(A>padThreshold){ // create Digi only when over threshold!
          PndTpcDigi* digi=new PndTpcDigi(A,t0,samples[i]->padId(),mcid);
          digi->tlength(length);
          digis.push_back(digi);
          if(DEBUG) cout<<"Digi Amp over threshold ("<<padThreshold<<")! Push back Digi"<<endl;
        }
        else if(DEBUG) cout<<"Digi Amp below threshold!"<<endl;

        samplesInPulse.clear();
      
        if(deriv_0>0){
          if(DEBUG) cout<<"starting new pulse"<<endl;
          inpulse = true;
          mcid.ClearData();
          if(time-prevtime==1 && i>0){ // add previous sample to pulse; this sample will then be in two pulses
            mcid.AddIDCollection(samples[i-1]->mcId(),1);
            samplesInPulse.push_back(samples[i-1]);
          }
          // add sample to pulse
          mcid.AddIDCollection(samples[i]->mcId(),1);
          samplesInPulse.push_back(samples[i]);
        }
      }else{ // not at the end of pulse
        // add sample to pulse
        mcid.AddIDCollection(samples[i]->mcId(),1);
        samplesInPulse.push_back(samples[i]);
      }
    }
  } // end loop over samples
  if(DEBUG) cout<<endl;
}


void PndTpcPSA_TOT1::processPulse(std::vector<PndTpcSample*> samples,
                                  double& t0,double& A, double& length){
  A=0.;
  t0=0.;
  double tA;
  // find maximum
  if(DEBUG) cout<<"Pulse created from samples: ";
  for(unsigned int i=0;i<samples.size();i++) {
    if(A<samples[i]->amp()){
      A=samples[i]->amp();
      tA = samples[i]->t();
    }
    if(DEBUG) cout<<samples[i]->t()<<", ";
  }
  length=samples.back()->t()-samples[0]->t();
  t0=samples[0]->t() + 0.5*length;
  // take mean time of center between minima and time of maximum amplitude
  t0*=0.5;
  t0+=0.5*tA;

  if(DEBUG) cout<<"with t:"<<t0<<", a:"<<A<<endl;
}
