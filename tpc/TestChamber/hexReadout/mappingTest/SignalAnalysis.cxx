#include"SignalAnalysis.h"
#include<cassert>

double pedestal(UInt_t* amp,int length){
  double sum=0.;
  for(int i=0;i<length;++i){
    sum+=amp[i];
  }
  return sum/length;
}

std::vector<pulse_t*> psa(UInt_t* amp,int length){
  static const int pedSamples=30;
  assert(length>pedSamples);
  double ped = pedestal(amp,30);
  double zAmp[length];
  for(int i=0;i<length;++i){
    zAmp[i]=amp[i]-ped;
  }
  double threshold = 10;
  int index = pedSamples+1;
  std::vector<pulse_t*> pulses;
  bool inPulse = false;
  int amp0;
  int amp1 = 20;
  int amp2 = 10;
  while(index<length){
    if(length-index>2)amp2=amp1;
    if(length-index>1)amp1=amp0;
    amp0=zAmp[index++];
    if(!inPulse){
      if(amp0>threshold){
	inPulse=true;
	pulse_t *p = new pulse_t;
	p->startTime = index-1;
	p->samples.push_back(amp0);
	pulses.push_back(p);
      }
      else{
	continue;
      }
    }
    else{//in pulse!
      if(amp0<threshold){
	inPulse=false;
      }
      else{//above threshold
	if(amp2<amp1){
	  pulses.at(pulses.size()-1)->samples.push_back(amp0);
	  continue;
	}
	else{
	  if(amp0<=amp1){//still falling edge
	    pulses.at(pulses.size()-1)->samples.push_back(amp0);
	    continue;	    
	  }
	  else{//we found local minimum
	    pulse_t *p = new pulse_t;
	    p->startTime = index-1;
	    p->samples.push_back(amp0);
	    pulses.push_back(p);	    
	  }
	}
      }
    }
    
  }
  return pulses;
}
