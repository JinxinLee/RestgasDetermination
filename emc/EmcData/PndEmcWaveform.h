//==========================================================================
//	PndEmcWaveform.h
//
// 	Class to hold waveforms created from Emc Hits
//	
//	Hits will be converted to waveforms using the standard
//	formula for an exponential decay convoluted with CR-RC (or CR-2RC)
//	shaping.
//
// Public functions:
//	
//      AddElecNoise(double)
//                                      Adds gaussian noise with width
//                                      given by the double parameter.
//
//      Digitise(double)                Simple digitisation, given
//                                      the value of one bit in energy 
//                                      equivalent units.
//      
//     AddElecNoiseAndDigitse(double, double)  Do both
//
//     AddShapedElecNoiseAndDigitse(double, double)  Do both, but with 
//            noise tyhat is not just plain gaussian, but passed through the
//            shaping.  Only use for detailed studies- this is like
//            adding a Hit to *every bin* and is very slow if you are doing
//            a calorimiter's worth.
//
//     GetScale()  Return the maximum peak of a 1 GeV hit.
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
#include "FairMultiLinkedData.h"

class PndEmcHit;

class PndEmcWaveform: public FairMultiLinkedData
{

public:

  //  Constructors
	PndEmcWaveform();
	
	PndEmcWaveform(int	trackId,long detId, long waveform_length=128, Int_t hitIndex =-1); 

  // Destructor:

  virtual ~PndEmcWaveform();

  // Copy:
  PndEmcWaveform(const PndEmcWaveform& copy);

  // Operators
  
  virtual PndEmcWaveform& operator=(const PndEmcWaveform& copy);

  // Selectors 

	long   GetDetectorId() const                    {return fDetectorId;}
	int    GetTrackId() const                       {return fTrackId;}
	Short_t GetModule()      const { return (fDetectorId/100000000);};
	PndEmcTwoCoordIndex* GetTCI() const;
	virtual	std::vector<double> GetSignal() const { return fSignal ;};
	double GetScale(Double_t sampleRate, PndEmcAbsPulseshape *pulseshape) const;
	double GetNormalisation(Double_t sampleRate, PndEmcAbsPulseshape *pulseshape) const;
   int    GetWaveformLength() const {return fWaveformLength;};
	
	Int_t GetHitIndex() {return fHitIndex;}
  

    // Modifiers

  void UpdateWaveform(PndEmcHit *hit, Double_t pePerMeV, Bool_t usePhotonStatistic, Double_t excessNoiseFactor, Double_t firstADCBinTime, Double_t sampleRate, PndEmcAbsPulseshape *pulseshape);
  void MakeWaveform(Double_t energy, Double_t time, Double_t pePerMeV, Bool_t usePhotonStatistic, Double_t excessNoiseFactor, Double_t firstADCBinTime, Double_t sampleRate, PndEmcAbsPulseshape *pulseshape);
  
  void AddElecNoise(double);
  void Digitise(double);
  void AddElecNoiseAndDigitise(double,double);
  // Both add noise and digitise.  The first double is the noise width (GeV),
  // the second is the one bit resolution

  void AddShapedElecNoiseAndDigitise(Double_t noise_width,Double_t oneBitResolution, PndEmcAbsPulseshape *pulseshape, Double_t firstADCBinTime, Double_t sampleRate);
  // Add shaped noise and digitise.  

  void SetWaveform(std::vector<Double_t>&signal,Int_t length){fSignal = signal; fWaveformLength=length;};
	double Max();
  
	virtual void clearAndReset();
	virtual void Clear(Option_t *option=""){fSignal.clear();};

//private:
protected: 
	Int_t fTrackId;	
	Int_t fDetectorId;
	Int_t fWaveformLength;
	Int_t fHitIndex;

	std::vector<Double_t>  fSignal; // Signal after FADC

ClassDef(PndEmcWaveform,5)
};
#endif
