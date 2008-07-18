//=====================================================================
//
//  $Id: $
//
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

PndEmcWaveform::PndEmcWaveform(){}



PndEmcWaveform::PndEmcWaveform(int trackId, long detId, Double_t tau1, Double_t tau2, Double_t sampleRate,Double_t firstADCTime, 
			 Double_t pePerMeV, Double_t tauCrystal, long waveform_length, Double_t excessNoiseFactor,bool use_photon_statistic, Int_t hitIndex):
	fTrackId(trackId),
	fDetectorId(detId),
	fTauCrystal(tauCrystal),
	fTau1(tau1),
	fTau2(tau2),
	fSampleRate(sampleRate),
	fFirstADCBinTime ( firstADCTime),
	fPePerMeV(pePerMeV),
	fExcessNoiseFactor(excessNoiseFactor),
	fUsePhotonStatistic(use_photon_statistic),
	fWaveformLength(waveform_length),
	fSignal(waveform_length,0.),
	fPulseshape(tau1,tau2,tauCrystal),
	fHitIndex(hitIndex)
{
	PndEmcMapper *fEmcMap=PndEmcMapper::Instance(0);
	fTCI=fEmcMap->GetTCI(detId);
}

PndEmcWaveform::PndEmcWaveform(const PndEmcWaveform &copy)
{
		fTrackId=copy.fTrackId;
		fDetectorId = copy.fDetectorId;
		fTCI=copy.fTCI;
		fTau1=copy.fTau1;
		fTau2=copy.fTau2;
		fSampleRate=copy.fSampleRate;
		fTauCrystal=copy.fTauCrystal;
		fFirstADCBinTime =copy.fFirstADCBinTime;
		fPePerMeV=copy.fPePerMeV;
		fExcessNoiseFactor=copy.fExcessNoiseFactor;
		fUsePhotonStatistic=copy.fUsePhotonStatistic;
		fWaveformLength=copy.fWaveformLength;
		fHitIndex=copy.fHitIndex;
		fSignal=copy.fSignal;
		fPulseshape=copy.fPulseshape;
		fHitIndex=copy.fHitIndex;
}

PndEmcWaveform&
PndEmcWaveform::operator=(const PndEmcWaveform &copy){
  if (this != &copy){
		fTrackId=copy.fTrackId;
		fDetectorId = copy.fDetectorId;
		fTCI=copy.fTCI;
		fTau1=copy.fTau1;
		fTau2=copy.fTau2;
		fSampleRate=copy.fSampleRate;
		fTauCrystal=copy.fTauCrystal;
		fFirstADCBinTime =copy.fFirstADCBinTime;
		fPePerMeV=copy.fPePerMeV;
		fExcessNoiseFactor=copy.fExcessNoiseFactor;
		fUsePhotonStatistic=copy.fUsePhotonStatistic;
		fWaveformLength=copy.fWaveformLength;
		fHitIndex=copy.fHitIndex;
		fSignal=copy.fSignal;
		fPulseshape=copy.fPulseshape;
		fHitIndex=copy.fHitIndex;
  }
  return *this;
}

//--------------
// Destructor --
//--------------

PndEmcWaveform::~PndEmcWaveform()
{
}

    
//-------------
// Selectors --
//-------------

Double_t 
PndEmcWaveform::get_scale() const
{
	return getNormalisation();
}

Double_t 
PndEmcWaveform::getNormalisation() const
{
  // Function to return the equivalent pulse height for a 1 GeV pulse
  // Used  in EmcHitsToWaveform to determine electronics noise scale

	PndEmcWaveform newWaveform(*this);
	newWaveform.clearAndReset();
	
	PndEmcHit *gevHit=new PndEmcHit();
	gevHit->SetEnergy(1.0);
	gevHit->SetTime(0.);
	newWaveform.update_waveform(gevHit);
 	delete gevHit;

	Double_t max=newWaveform.max();
	return max;
}

    
//-------------
// Modifiers --
//-------------

void 
PndEmcWaveform::update_waveform(PndEmcHit *hit)
{

	Double_t energy=(Double_t)hit->GetEnergy();
	Double_t time=(Double_t)hit->GetTime();
	
	make_waveform(energy, time);

}

void 
PndEmcWaveform::make_waveform(Double_t energy, Double_t time)
{
	Double_t amplitude;
	Double_t photonStatFactor;
	if (fUsePhotonStatistic)
	{
		Double_t crystalPhotonsMeV = 1.0e3 * energy * fPePerMeV;
		photonStatFactor = gRandom->Gaus(1,sqrt(fExcessNoiseFactor/crystalPhotonsMeV));
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
		t= time+fFirstADCBinTime+i/fSampleRate;
		fSignal[i]+=(fPulseshape.value(t,amplitude,time_offset));
	}
}

void 
PndEmcWaveform::add_elec_noise(Double_t width)
{
   
	for (int i=0;i<fWaveformLength;i++)
	{
		Double_t ran_noise;
		ran_noise=gRandom->Gaus(0,1)*width;
		fSignal[i]+=ran_noise;
	}
}


void 
PndEmcWaveform::digitise(Double_t oneBitResolution)
{
	for (  int i=0;i<fWaveformLength;i++)
	{
		fSignal[i]=(Double_t) ( long (fSignal[i]/oneBitResolution+64)-64 ) * oneBitResolution;
	}
}

void 
PndEmcWaveform::add_elec_noise_and_digitise(Double_t noise_width,
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
PndEmcWaveform::add_shaped_elec_noise_and_digitise(Double_t noise_width,
						Double_t oneBitResolution)
{
  // Do both e_noise and digitisation.

	// "0" corresponds to timing constant of signal
	// for noise can be assumed 0 (it is taken tauInt*1e-5 to avoid nan)
	PndEmcCRRCPulseshape* fPulseshape2= new PndEmcCRRCPulseshape(fTau1,fTau2,fTau1*1e-5);

	Double_t t;
	for (  int i=0;i<fWaveformLength;i++)
	{
			t= fFirstADCBinTime+i/fSampleRate;
			Double_t ran_noise=gRandom->Gaus(0,1)*noise_width;
			fSignal[i]+=fPulseshape2->value(t,ran_noise,0);
	}    
	
	for (  int i=0;i<fWaveformLength;i++)
	{
		fSignal[i]=(Double_t) ( long (fSignal[i]/oneBitResolution+64)-64 ) * oneBitResolution;
	}
	delete fPulseshape2;
}

void PndEmcWaveform::fitPeak(Double_t& ampl, Double_t& pos,
			  int peakBin) const
{
// parabolic fit
	ampl=pos=-1.;
	if (peakBin>0 && peakBin<fWaveformLength-1)
	{
		long theBin(peakBin);
	
		Double_t pValue = fSignal[peakBin];
		Double_t pPosition = Double_t(peakBin);
	
		Double_t leftValue = fSignal[theBin-1];
		Double_t rightValue = fSignal[theBin+1];
		if (leftValue<pValue && rightValue<pValue) {
			Double_t d = 0.25*(rightValue-leftValue);
			Double_t b = pValue-0.5*(leftValue+rightValue);
			pValue += d*d/b;
			pPosition += d/b;
		}
		ampl=pValue;
		pos=pPosition;
  }
}

void PndEmcWaveform::fitPeak(Double_t& ampl, Double_t& pos, int start, int end) const
{
	std::vector<Double_t>::const_iterator p;
	p=max_element(fSignal.begin()+start,fSignal.begin()+end);
	int pPosition = distance(fSignal.begin(),p);
	fitPeak(ampl,pos,pPosition);
}
  
void PndEmcWaveform::fitPeak(Double_t& ampl, Double_t& pos) const
{
	std::vector<Double_t>::const_iterator p;
	p=max_element(fSignal.begin(),fSignal.end());
	int pPosition = distance(fSignal.begin(),p);
	fitPeak(ampl,pos,pPosition);
}

Double_t 
PndEmcWaveform::max()
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

void PndEmcWaveform::Streamer(TBuffer &R__b)
{
   // Stream an object of class PndEmcWaveform.

   if (R__b.IsReading()) {
      PndEmcWaveform::Class()->ReadBuffer(R__b, this);
		PndEmcMapper *fEmcMap=PndEmcMapper::Instance(0);
		if (fEmcMap!=0) {
			fTCI=fEmcMap->GetTCI(fDetectorId);
		} else {
			fTCI=0;
		}

   } else {
      PndEmcWaveform::Class()->WriteBuffer(R__b, this);
   }
}

ClassImp(PndEmcWaveform)
