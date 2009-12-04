//==========================================================================
// File and Version Information:
//   $Id:$
// 
//	PndEmcWaveform.h
//
// 	Class to hold waveforms created from Emc GHits
//	
//	GHits will be converted to waveforms using the standard
//	formula for an exponential decay convoluted with CR-RC
//	shaping.
//
// Public functions:
//	
//      add_elec_noise(double)
//                                      Adds gaussian noise with width
//                                      given by the double parameter.
//
//      digitise(double)                Simple digitisation, given
//                                      the value of one bit in energy 
//                                      equivalent units.
//      
//     add_elec_noise_and_digitse(double, double)  Do both
//
//     add_shaped_elec_noise_and_digitse(double, double)  Do both, but with 
//            noise tyhat is not just plain gaussian, but passed through the
//            CR-RC shaping.  Only use for detailed studies- this is like
//            adding a GHit to *every bin* and is very slow if you are doing
//            a calorimiter's worth.
//
//     get_scale()  Return the maximum peak of a 1 GeV hit.
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//
// 	P.D.Strother 	Imperial College
// Dima Melnichuk - adaption for PANDA		
//=======================================================================
#pragma once
#ifndef PNDEMCWAVEFORM_H
#define PNDEMCWAVEFORM_H

#include <vector>
#include "TObject.h"
#include "PndEmcTwoCoordIndex.h"
#include "PndEmcCRRCPulseshape.h"
#include "PndEmcCR2RCPulseshape.h"
#include "PndEmcHit.h"

class PndEmcHit;

class PndEmcWaveform: public TObject
{

public:

  //  Constructors
	PndEmcWaveform();
	
	PndEmcWaveform(int	trackId,long detId, double tau1, double tau2, double sampleRate, double firstBinTime, double pePerMev=0.0, double tauCrystal=0.9e-6, long waveform_length=128,double _excessNoiseFactor=1.0,bool usePhotonStatistic=true, Int_t hitIndex =-1); 

  // Destructor:

  virtual ~PndEmcWaveform();

  // Copy:
  PndEmcWaveform(const PndEmcWaveform& copy);

  // Operators
  
  virtual PndEmcWaveform& operator=(const PndEmcWaveform& copy);

  // Selectors 

	double GetShapingTime1() const                  {return fTau1;}
	double GetShapingTime2() const                  {return fTau2;}
	double GetTauCrystal() const                    {return fTauCrystal;}
	double GetSampleRate() const                    {return fSampleRate;}
	long   GetDetectorId() const                    {return fDetectorId;}
	int    GetTrackId() const                       {return fTrackId;}
	double GetFirstBinTime() const                  {return fFirstADCBinTime;}
	double GetPePerMeV() const                      {return fPePerMeV;}
	double GetExcessNoiseFactor() const             {return fExcessNoiseFactor;}
	PndEmcTwoCoordIndex* GetTCI() const { return fTCI ;};
	std::vector<double> GetSignal() const { return fSignal ;};
	double get_scale() const;
	double getNormalisation() const;
    //int    getWaveformLength() const;  // not implemented
	
	Int_t GetHitIndex() {return fHitIndex;}
  

    // Modifiers

  void update_waveform(PndEmcHit *);
  void make_waveform(double energy, double time);
  
  void add_elec_noise(double);
  void digitise(double);
  void add_elec_noise_and_digitise(double,double);
  // Both add noise and digitise.  The first double is the noise width (GeV),
  // the second is the one bit resolution

  void add_shaped_elec_noise_and_digitise(double,double);
  // Add shaped noise and digitise.  The first double is the noise width (GeV),
  // the second is the one bit resolution

	void fitPeak(double& ampl, double& pos, int peakBin) const;
	
	void fitPeak(double& ampl, double& pos, int start, int end) const;
	
	void fitPeak(double& ampl, double& pos) const;

	double max();
  
	void clearAndReset();

private:
  
	int fTrackId;	
	long fDetectorId;
	double fTauCrystal;
	double fTau1; //diff
	double fTau2; //int
	double fSampleRate;
	double fFirstADCBinTime;
	double fPePerMeV;
	double fExcessNoiseFactor;
	bool fUsePhotonStatistic;
	int fWaveformLength;
	PndEmcTwoCoordIndex* fTCI; //! transient data
	
	Int_t fHitIndex;

	std::vector<double>  fSignal; // Signal after FADC
	PndEmcCRRCPulseshape fPulseshape;

ClassDef(PndEmcWaveform,1)
};
#endif
