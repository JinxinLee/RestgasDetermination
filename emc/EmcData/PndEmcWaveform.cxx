//=====================================================================
//	PndEmcWaveform.cxx
//
// 	Class to hold waveforms created from Emc Hits
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//
// 	P.D.Strother 	Imperial College
//      Naveen Gunawardane  Imperial College
// Dima Melnichuk - adaption for PANDA		
//-----------------------


#include "PndEmcWaveform.h"
#include "PndEmcHit.h"
#include "PndEmcCRRCPulseshape.h"
#include "PndEmcCR2RCPulseshape.h"
#include "PndEmcMapper.h"
#include "PndDetectorList.h" 

#include "TRandom.h"
#include "TClonesArray.h"

#include "TClass.h"
#include "TBuffer.h"
#include "Riostream.h"

//---------------
// C++ Headers --
//---------------

#include <iostream>
#include <algorithm>
#include "assert.h"

using std::cout;
using std::endl;


//----------------
// Constructors --
//----------------

PndEmcWaveform::PndEmcWaveform():
  fTrackId(-1),
  fDetectorId(-1),
  fWaveformLength(0),
  fSignal(0,0.),
  fHitIndex(-1)
{
}



PndEmcWaveform::PndEmcWaveform(int trackId, long detId, long waveform_length, Int_t hitIndex):
	fTrackId(trackId),
	fDetectorId(detId),
	fWaveformLength(waveform_length),
	fSignal(waveform_length,0.),
	fHitIndex(hitIndex)
{
  SetLink(FairLink("EmcHit", hitIndex));
}

PndEmcWaveform::PndEmcWaveform(const PndEmcWaveform& copy):
  fTrackId(copy.fTrackId),
  fDetectorId ( copy.fDetectorId),
  fWaveformLength(copy.fWaveformLength),
  fHitIndex(copy.fHitIndex),
  fSignal(copy.fSignal)
{
  SetLink(FairLink("EmcHit", copy.fHitIndex));
}

PndEmcWaveform&
PndEmcWaveform::operator=(const PndEmcWaveform &copy){
  if (this != &copy){
		fTrackId=copy.fTrackId;
		fDetectorId = copy.fDetectorId;
		fWaveformLength=copy.fWaveformLength;
		fHitIndex=copy.fHitIndex;
		fSignal=copy.fSignal;  
		SetLink(FairLink("EmcHit", copy.fHitIndex));
  }
  return *this;
}

//--------------
// Destructor --
//--------------

PndEmcWaveform::~PndEmcWaveform()
{
	fSignal.clear();
}

    
//-------------
// Selectors --
//-------------

Double_t 
PndEmcWaveform::GetScale(Double_t sampleRate, PndEmcAbsPulseshape *pulseshape) const
{
	return GetNormalisation(sampleRate, pulseshape);
}

Double_t 
PndEmcWaveform::GetNormalisation(Double_t sampleRate, PndEmcAbsPulseshape *pulseshape) const
{
  // Function to return the equivalent pulse height for a 1 GeV pulse
  // Used  in EmcHitsToWaveform to determine electronics noise scale

	PndEmcWaveform newWaveform(*this);
	newWaveform.clearAndReset();
	
	PndEmcHit *gevHit=new PndEmcHit();
	gevHit->SetEnergy(1.0);
	gevHit->SetTime(0.);
	newWaveform.UpdateWaveform(gevHit, 0, false, 1., 0., sampleRate, pulseshape);
 	delete gevHit;

	Double_t maximum=newWaveform.Max();
	return maximum;
}

    
//-------------
// Modifiers --
//-------------

void 
PndEmcWaveform::UpdateWaveform(PndEmcHit *hit, Double_t pePerMeV, Bool_t usePhotonStatistic, Double_t excessNoiseFactor, Double_t firstADCBinTime, Double_t sampleRate, PndEmcAbsPulseshape *pulseshape)
{

	Double_t energy=(Double_t)hit->GetEnergy();
	Double_t time=(Double_t)hit->GetTime();
	
	MakeWaveform(energy, time, pePerMeV, usePhotonStatistic, excessNoiseFactor, firstADCBinTime, sampleRate, pulseshape);

}

void 
PndEmcWaveform::MakeWaveform(Double_t energy, Double_t time, Double_t pePerMeV, Bool_t usePhotonStatistic, Double_t excessNoiseFactor, Double_t firstADCBinTime, Double_t sampleRate, PndEmcAbsPulseshape *pulseshape)
{
	Double_t amplitude;
	Double_t photonStatFactor;
	if (usePhotonStatistic)
	{
		Double_t crystalPhotonsMeV = 1.0e3 * energy * pePerMeV;
		photonStatFactor = gRandom->Gaus(1,sqrt(excessNoiseFactor/crystalPhotonsMeV));
   }
	else
	{
		photonStatFactor=1.;
	}

	amplitude = energy*photonStatFactor;
	
	Double_t t;
	Double_t time_offset=time;
	for (  int i=0;i<fWaveformLength;i++)
	{
		t= time+firstADCBinTime+i/sampleRate;
		fSignal[i]+=(pulseshape->value(t,amplitude,time_offset));
	}
}

void 
PndEmcWaveform::AddElecNoise(Double_t width)
{
   
	for (int i=0;i<fWaveformLength;i++)
	{
		Double_t ran_noise;
		ran_noise=gRandom->Gaus(0,1)*width;
		fSignal[i]+=ran_noise;
	}
}


void 
PndEmcWaveform::Digitise(Double_t oneBitResolution)
{
	for (  int i=0;i<fWaveformLength;i++)
	{
		fSignal[i]=(Double_t) ( long (fSignal[i]/oneBitResolution+64)-64 ) * oneBitResolution;
	}
}

void 
PndEmcWaveform::AddElecNoiseAndDigitise(Double_t noise_width,
					 Double_t oneBitResolution)
{
	// Do both e_noise and digitisation.  
	
	for (  int i=0;i<fWaveformLength;i++)
	{
		Double_t ran_noise=gRandom->Gaus(0,1)*noise_width;
		fSignal[i]+=ran_noise;
		if (fSignal[i]<0) fSignal[i]=0;
		fSignal[i]=(Double_t) ( long (fSignal[i]/oneBitResolution+64)-64 ) * oneBitResolution;
	}    
}

void 
PndEmcWaveform::AddShapedElecNoiseAndDigitise(Double_t noise_width,
						Double_t oneBitResolution, PndEmcAbsPulseshape *pulseshape, Double_t firstADCBinTime, Double_t sampleRate)
{
  // Do both e_noise and digitisation.
	Double_t t;
	for (  int i=0;i<fWaveformLength;i++)
	{
			t= firstADCBinTime+i/sampleRate;
			Double_t ran_noise=gRandom->Gaus(0,1)*noise_width;
			fSignal[i]+=pulseshape->value(t,ran_noise,0);
	}    
	
	for (  int i=0;i<fWaveformLength;i++)
	{
		fSignal[i]=(Double_t) ( long (fSignal[i]/oneBitResolution+64)-64 ) * oneBitResolution;
	}
}

Double_t 
PndEmcWaveform::Max()
{
	Double_t _max;
	_max=*max_element(fSignal.begin(),fSignal.end());
	return _max;
}

void 
PndEmcWaveform::clearAndReset(){
	// reset values of fSignal to 0
	fill(fSignal.begin(),fSignal.end(),0);
}

PndEmcTwoCoordIndex* PndEmcWaveform::GetTCI() const
{
	PndEmcMapper *emcMap=PndEmcMapper::Instance();
	PndEmcTwoCoordIndex* tci=emcMap->GetTCI(fDetectorId);
	return tci;
};
	
ClassImp(PndEmcWaveform)
