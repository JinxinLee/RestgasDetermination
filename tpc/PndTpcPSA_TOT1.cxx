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

#define DEBUG 0

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
  
  for(unsigned int i=0;i<samples.size();++i){
    amp=samples.at(i)->amp();
    time=samples.at(i)->t();
    if(i>2){
      deriv_2=deriv_1;
    }
    if(i>1){
      deriv_1=deriv_0;
    }
    if(i>0){
      prevamp=samples.at(i-1)->amp();
      prevtime=samples.at(i-1)->t();
      if(time-prevtime!=1){
        deriv_0=amp; //if there were empty samples between
        fallingEdge=false;
      }else{
        deriv_0=amp-prevamp;
        if(deriv_0<0){
          fallingEdge=true;
        }
      }
    }
    if(i==0){ 
      deriv_0=amp; //setting the derivative for the first sample
      fallingEdge=false;
    }
    if(DEBUG) cout<<"Processing sample: "<<i<<", a:"<<amp<<", t:"<<time<<" der0,1,2: "<<deriv_0<<", "<<deriv_1<<", "<<deriv_2<<endl;
    if(!inpulse){
      if(deriv_0>0 && 
         deriv_1>=0 && 
         time-prevtime==1){
        if(DEBUG) cout<<" starting new pulse"<<endl;
        inpulse=true;
        mcid.ClearData();
        if(time-prevtime==1&&time>1){ //TODO get from config-file
          mcid.AddIDCollection(samples[i-1]->mcId(),1);
          samplesInPulse.push_back(samples[i-1]);
        }
        mcid.AddIDCollection(samples[i]->mcId(),1);
        samplesInPulse.push_back(samples[i]);
      }
    }else{
      if((deriv_0>0 && deriv_1<0) ||
         (fallingEdge && deriv_0==0 ) ||
          time-prevtime>1 ||
          i==samples.size()-1){
        if(time==510){  //TODO get from config-file
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
          if(time-prevtime==1){
            mcid.AddIDCollection(samples[i-1]->mcId(),1);
            samplesInPulse.push_back(samples[i-1]);
          }
          mcid.AddIDCollection(samples[i]->mcId(),1);
          samplesInPulse.push_back(samples[i]);
        }
      }else{
        mcid.AddIDCollection(samples[i]->mcId(),1);
        samplesInPulse.push_back(samples[i]);
      }
    }
  }
  if(DEBUG) cout<<endl;
}


void PndTpcPSA_TOT1::processPulse(std::vector<PndTpcSample*> samples,
                                  double& t0,double& A, double& length){
  A=0.;
  t0=0.;
  // find maximum
  if(DEBUG){
    cout<<"Pulse created from samples: ";
  }
  for(unsigned int i=0;i<samples.size();i++) {
    if(A<samples[i]->amp())A=samples[i]->amp();
    if(DEBUG){
      cout<<samples[i]->t()<<", ";
    }
  }
  length=samples.back()->t()-samples[0]->t();
  t0=samples[0]->t() + 0.5*length;
  if(DEBUG){
    cout<<"with t:"<<t0<<", a:"<<A<<endl;
  }

}
