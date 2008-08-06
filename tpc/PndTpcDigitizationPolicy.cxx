//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcDigitizationPolicy
//      see PndTpcDigitizationPolicy.h for details
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
#include "PndTpcDigitizationPolicy.h"

// Collaborating Class Headers --------
#include "TError.h"
#include "PndTpcSignal.h"
#include "PndTpcSignalAge.h"
#include "PndTpcSample.h"
#include "McIdCollection.h"
#include "PndTpcFrontend.h"
#include "PndTpcAbsPulseshape.h"

// C/C++ Headers ----------------------
#include "assert.h"
#include <limits>
#include <iostream>
#include <algorithm>
using std::numeric_limits;
using std::vector;

// Class Member definitions -----------
void
PndTpcDigitizationPolicy::Digitize(vector< PndTpcSignal* >* Signals,
				vector< PndTpcSample* >* Samples,
				const PndTpcFrontend* Frontend,
				const PndTpcAbsPulseshape* pulseshape){

  // Digitize
  // TODO: Refactorize the PadID out of this function!
  // Sort Signals according to PadID while preserving time order in one Pad
  
  std::sort(Signals->begin(),Signals->end(),PndTpcSignalComparePadID()); 
  // when using PndTpcElectronicsTask this should not be necessary

  double tstep=Frontend->dt();
  //std::cout << "tstep: " << tstep << std::endl;
  double t=0;
  double nextsignalt=std::numeric_limits<double>::max();
  
  assert(pulseshape!=0);

  std::vector<PndTpcSignal*>::iterator it_Padbegin=Signals->begin();
  std::vector<PndTpcSignal*>::iterator it_Padend=Signals->begin();
  unsigned int iPadbegin=0;
  unsigned int iPadend=0;
  unsigned int nSignals=Signals->size();

  unsigned int PadID=(*it_Padbegin)->padId();
  double tint=Frontend->tint();				//use for a rough estimate on how many samples should be at least tried 
  const int nMinSamplesTrys = (int)(tint/tstep);		// necessary because if only one signal is there, the time of this signal
  										// will be to early to get something else from pulseshape->value than zero
  //std::cout << "nMinSamplesTrys: " << nMinSamplesTrys << std::endl;
  while(iPadend<nSignals){
    //std::cout<<"iPadend:"<<iPadend<<"   iPadbegin:"<<iPadbegin<<std::endl;
    unsigned int newPadID=(*Signals)[iPadend]->padId();
    if(newPadID!=PadID || iPadend+1==nSignals){
      // process the last pad
      // sort the signals on this pad in time
	 //std::cout << "PadIDs: " << newPadID << " " << PadID << std::endl;
      sort(it_Padbegin+iPadbegin,it_Padend+(iPadend),PndTpcSignalAge());

      t=Frontend->TimeAtClock((*Signals)[iPadbegin]->t());
      unsigned int iSig=iPadbegin;
      while(iSig<=iPadend){//loop over signals on one pad
	if(iSig<iPadend)nextsignalt=Frontend->TimeAtClock((*Signals)[iSig+1]->t());
	int nSamplesTrys=0;
	while(t<nextsignalt || iSig==iPadend ){
	  //add all valid pulseshapes
	  double amp=0;
	  unsigned int i;
	  McIdCollection mcid;
	  for(i=iPadbegin; i<=iSig; ++i){
	    PndTpcSignal* sig=(*Signals)[i];
	    double ampcontrib=pulseshape->value(t,sig->amp(),sig->t());
	    amp+=ampcontrib;
	    mcid.AddID(sig->mcId(),ampcontrib);
	  }
	  if(amp>Frontend->adcThreshold()){
	    mcid.Renormalize();
	    Samples->push_back(new PndTpcSample(Frontend->Clock(t),
						Frontend->A2D(amp),
						PadID,
						mcid));
	  }
	  else if (nSamplesTrys > nMinSamplesTrys) {
	    t=nextsignalt;
	    break;
	  }
	  t+=tstep;
	  nSamplesTrys++;
	}
	++iSig;
	
      }// end loop over Signals on one pad
      // cleanup signals on current pad
      //iSig=iPadbegin;
      //while(iSig<iPadend){
      //if((*Signals)[iSig]!=NULL){
      //  delete(*Signals)[iSig];
      //  (*Signals)[iSig]=NULL;
      //}
      //++iSig;
      //}
	
      // start new signalform
      PadID=newPadID;
      iPadbegin=iPadend;
    }
    ++iPadend;
  } //end loop over signals
}



