//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcPSA_AD1
//      see PndTpcPSA_AD1.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Maxence Vandenbroucke    TUM            (original author)
//
//
//-----------------------------------------------------------

// This Class' Header ------------------
#include "PndTpcPSA_AD1.h"
#include "McIdCollection.h"


// C/C++ Headers ----------------------
#include "assert.h"
#include <algorithm>
#include <iostream>
#include <vector>

//TODO : implement init of the factor for whatever shape
//#define fPULSEDELAYFACTOR 63//time between the maxamp and 50% of rising egde in ns
#define fPULSEDELAYFACTOR 63//time between the maxamp and 50% of rising egde in ns

using std::max;

// Collaborating Class Headers --------
#include "PndTpcSample.h"
#include "PndTpcDigi.h"

// Class Member definitions -----------
PndTpcPSA_AD1::PndTpcPSA_AD1()//const PndTpcAbsPulseshape* pulseshape)
  : PndTpcAbsPSAStrategy(), fcurrentPadID(0), finprogress(false),
    famp(0),ft(0),fthreshold(10), fdtc(-1),fdtp(-1.0), fdamp(-1.0), fN(2), fTAIL(true), NEWPULSE(true), fFreq(20)
{  //fpulseShape=pulseshape;
}
//

PndTpcDigi*
PndTpcPSA_AD1::ProcessNext(const PndTpcSample* sample)
{
  return NULL;
}

void PndTpcPSA_AD1::Process(const std::vector<PndTpcSample*> & samples, 
			    std::vector<PndTpcDigi*>& digis,
			    double padThreshold) 
{
  NEWPULSE=false;
  fLastPulse = 0;

  std::vector<PndTpcSample*> noConstSamples;
  for (int uo = 0; uo<samples.size(); uo++)
    {PndTpcSample * _s = new PndTpcSample(samples[uo]->t(),
					  samples[uo]->amp(),
					  samples[uo]->padId(),
					  samples[uo]->mcId(),
					  samples[uo]->ct());
    
      noConstSamples.push_back(_s);
    }

  //  std::vector<PndTpcSample*> noConstSamples
  //  =const_cast<std::vector<PndTpcSample*>&>(samples);//forunconsttailsuppr
  for(int i=3;i<noConstSamples.size();i++) {

    //->(-)-----|detectSig|--------->tc,tp.amp
    //   |                        |
    //   |______|tailSuppr|_______|
    
    //    std::cout << "ci " <<samples[i]->amp() << "ci_1 " <<samples[i-fN]->amp() << std::endl;
    if(fLastPulse>=290) break;

      if (fTAIL&&fLastPulse)
            tailSuppr(i,noConstSamples);
    

    detectSigCF(i,noConstSamples);
    
    if(NEWPULSE)
      {
      saveSig(noConstSamples,digis);
      std::cout << "NEWPULSE "<< fLastPulse << std::endl;
      }
  }
}

void PndTpcPSA_AD1::tailSuppr(int i ,std::vector<PndTpcSample*> & samples)
{

  //TODO check if ok with every pulseShape and see if it creates signals
  double tail=fpulseShape->value(samples[i]->t()*fFreq,fdamp,(fdtp+fdtc)*fFreq+142.5-77.5);
  //std::cout << "TAIL " <<samples[i]->t()-samples[fLastPulse]->t()  <<' ' << tail<< ' ' << fdamp << std::endl;
  if (tail<samples[i]->amp()&&tail>0)
    samples[i]->setAmp((int)(samples[i]->amp()-tail));
  else 
    samples[i]->setAmp(0);
}

void PndTpcPSA_AD1::saveSig(std::vector<PndTpcSample*> & samples,std::vector<PndTpcDigi*>& digis)
  //saveSignal from last signal to local max
{  
  McIdCollection mcid;
  mcid.ClearData();
  std::vector<PndTpcSample*> samplesInPulse;

  int t0,t1,t2,maxAmp;


 
  while(fLastPulse+2<samples.size())
    {//add samples to digi from fLastPulse to next local maxima
      mcid.AddIDCollection(samples[fLastPulse]->mcId(),1);
      samplesInPulse.push_back(samples[fLastPulse]);

      //local maxima finding
      t0=samples[fLastPulse]->amp();
      t1=samples[fLastPulse+1]->amp();
      t2=samples[fLastPulse+2]->amp();

      if(t0<=t1 && t1>t2)
	{
	  maxAmp=t1;
	  fLastPulse++;
	  break;
	}
      
      fLastPulse++;
    }

  //TODO saving small pulse ?
  if (fLastPulse+2>samples.size()) return;

  fdamp=maxAmp;
  double length=samplesInPulse[samplesInPulse.size()-1]->t()-samplesInPulse[0]->t();
  mcid.Renormalize();
  PndTpcDigi* digi=new PndTpcDigi(maxAmp,fdtc+fdtp,samples[fLastPulse-1]->padId(),mcid);
  digi->tlength(length);
  for (int yi=0;yi<samplesInPulse.size();++yi)
    digi->addSample(*samplesInPulse.at(yi));
  digis.push_back(digi);
  samplesInPulse.clear();
  NEWPULSE = false;
}




  void    PndTpcPSA_AD1::detectSigCF(int i ,std::vector<PndTpcSample*> & samples)
{ 

  //Looking at 50% of the signal with
  //0=Si-2*S(i-n)
  double ci_1=samples[i-1]->amp()-2.0*samples[i-1-fN]->amp();
  double ci = samples[i]->amp()-2.0*samples[i-fN]->amp();
  if(samples[i-1]->amp()<samples[i]->amp())//rising edge
    if (ci_1>0)
      if (ci<0)
	{
	  fdtc = samples[i-1]->t();
	  //EXTRAPOLATION FOR PHASE
	  fdtp=ci_1/(ci_1-ci);
	  NEWPULSE = true;
	}
    if (NEWPULSE) 
    {
      fLastPulse=fdtc-4;
      std::cout << i<<"\tci " << ci << "\tci_1 " << ci_1 << "\tfd "<< fdtp << "\tfc "<<fdtc<< std::endl;}
    
}
