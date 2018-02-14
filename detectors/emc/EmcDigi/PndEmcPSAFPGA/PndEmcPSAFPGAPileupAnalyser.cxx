#include "PndEmcPSAFPGAPileupAnalyser.h"

#include "PndEmcPSAFPGAFilterCF.h"
#include "PndEmcPSAFPGAFilterMWD.h"
#include "PndEmcPSAFPGAFilterMA.h"
#include "PndEmcPSAFPGAFilterDelay.h"
#include "PndEmcPSAFPGALinFitter.h"

#include "TF1.h"

#include <iostream>

PndEmcPSAFPGAPileupAnalyser::PndEmcPSAFPGAPileupAnalyser() :
  PndEmcPSAFPGASampleAnalyser(), fVerbose(0), Int_thres(NULL), Int_mean(NULL), MWD_filter1(NULL), MWD_filter2(NULL), MWD_filter3(NULL), CF_prev(NULL), func_defined(false) {
}

PndEmcPSAFPGAPileupAnalyser::~PndEmcPSAFPGAPileupAnalyser() {
	if(Int_thres!=NULL) delete Int_thres;
	if(Int_mean!=NULL) delete Int_mean;
	if(MWD_filter1!=NULL) delete MWD_filter1;
	if(MWD_filter2!=NULL) delete MWD_filter2;
	if(MWD_filter3!=NULL) delete MWD_filter3;
	if(CF_prev!=NULL) delete CF_prev;
}

void PndEmcPSAFPGAPileupAnalyser::reset() {	//reset for every waveform to process
  local_time = 0;
  rough_pulse_timing = 0;
  Number_of_hits = 0;
  CF_filter->resetToZero();
  MA_triggering->resetToZero();
  MWD_filter1->resetToZero();
  MWD_filter2->resetToZero();
  MWD_filter3->resetToZero();
  CF_delay->resetToZero();
  pulse_detected = false;
  BaselineSum = 0;
  return;
}


void PndEmcPSAFPGAPileupAnalyser::InitParameters(const std::vector<Double_t> &params) {

	//define parameter initialization, otherwise conflict with clock_unit and iafactor parameters, which have been added to PndEmcPSAFPGASampleAnalyser
	//
	const Int_t size = params.size();
	if(size!=14) {
		std::cerr << "-W PndEmcPSAFPGAPileupAnalyser::InitParameters Mismatch in number of parameters (" << size << "!=14). Filling missing parameter values up with zeros..." << std::endl;
	}

	SampleAnalyserParams newParams;
	newParams.ma_trig_M = (size>0) ? (int) params.at(0): 0;
	newParams.hit_threshold = (size>1) ? params.at(1): 0;
	newParams.cf_delay = (size>2) ? (int) params.at(2): 0;
	newParams.cf_ratio = (size>3) ? params.at(3): 0;
	newParams.cf_fitter_length = (size>4) ? (int) params.at(4): 0;
	newParams.cf_fit_offset = (size>5) ? (int) params.at(5): 0;
	newParams.mwd_length = (size>6) ? (int) params.at(6): 0;
	newParams.mwd_tau = (size>7) ? params.at(7): 0;
	newParams.mwd2_length = (size>8) ? (int) params.at(8): 0;
	newParams.mwd2_tau = (size>9) ? params.at(9): 0;
	newParams.mwd3_length = (size>10) ? (int) params.at(10): 0;
	newParams.mwd3_tau = (size>11) ?  params.at(11): 0;
	newParams.sig_delay = (size>12) ?  params.at(12): 0;
	newParams.rough_timing_corr = (size>13) ?  params.at(13): 0;
	
	PndEmcPSAFPGASampleAnalyser::init(newParams);	
}

void PndEmcPSAFPGAPileupAnalyser::Init(const std::vector<Double_t> &params, TF1* r_thres, TF1* r_mean, float extBaselineValue) {
	Init2(params, r_thres, r_mean);
	setBaseline(extBaselineValue);
}

void PndEmcPSAFPGAPileupAnalyser::Init(const std::vector<Double_t> &params, TF1* r_thres, TF1* r_mean, unsigned int baselineStartSample, unsigned int baselineStopSample) {
	Init2(params, r_thres, r_mean);
	setBaselineWindow(baselineStartSample, baselineStopSample);
}


void PndEmcPSAFPGAPileupAnalyser::Init2(const std::vector<Double_t> &params, TF1* r_thres, TF1* r_mean) {

	InitParameters(params);
	Signal_delay->set(analyserParams.sig_delay);

	if(MWD_filter1!=NULL) delete MWD_filter1;
	MWD_filter1 = new PndEmcPSAFPGAFilterMWD();
	if(MWD_filter2!=NULL) delete MWD_filter2;
	MWD_filter2 = new PndEmcPSAFPGAFilterMWD();
	if(MWD_filter3!=NULL) delete MWD_filter3;
	MWD_filter3 = new PndEmcPSAFPGAFilterMWD();
	if(CF_prev!=NULL) delete CF_prev;
	CF_prev = new PndEmcPSAFPGAFilterDelay();

	MWD_filter1->set(analyserParams.mwd_tau, analyserParams.mwd_length);
	MWD_filter2->set(analyserParams.mwd2_tau, analyserParams.mwd2_length);
	MWD_filter3->set(analyserParams.mwd3_tau, analyserParams.mwd3_length);
	CF_prev->set(1);

	if(r_mean!=NULL && r_thres!=NULL) {
		func_defined = true;

		if(Int_thres!=NULL) delete Int_thres;
		Int_thres = new TF1("intThres", "(" +r_thres->GetExpFormula() + ")*x", r_thres->GetXmin(), r_thres->GetXmax());
		Int_thres->SetParameters(r_thres->GetParameters());
		Int_thres->SetNpx(1000);

		if(Int_mean!=NULL) delete Int_mean;
		Int_mean = new TF1("intMean", "(" +r_mean->GetExpFormula() + ")*x", r_mean->GetXmin(), r_mean->GetXmax());
		Int_mean->SetParameters(r_mean->GetParameters());
		Int_mean->SetNpx(1000);

	} else {
		std::cerr << "-W PndEmcPSAFPGAPileupAnalyser:Init2 no pileup separation function defined. All hits will be treated as single pulses" << std::endl;	
	}
}

//TODO: Baseline follower (baselineMode = kFollow)

void PndEmcPSAFPGAPileupAnalyser::setBaseline(float newBaseline) {
	PndEmcPSAFPGASampleAnalyser::setBaseline(newBaseline);
	baselineMode = kExtern;
}

void PndEmcPSAFPGAPileupAnalyser::setBaselineWindow(unsigned int startSample, unsigned int stopSample) {
	BaselineStartSample = startSample;
	BaselineStopSample = stopSample;
	baselineMode = kWindow;
}


void PndEmcPSAFPGAPileupAnalyser::GetHit(Int_t i, Double_t &Energy, Double_t& Time) {
	Energy = sampleAmplitude(i);
	Time = sampleTime(i);
}


void PndEmcPSAFPGAPileupAnalyser::GetHit(Int_t i, Double_t &Energy, Double_t& Time, pileup_t& PileupType) {
	GetHit(i, Energy, Time);
	if(i < nHits() && i >=0) {
		PileupType = pileups[i];
	} else {
		PileupType = kInvalid;
	}
}


void PndEmcPSAFPGAPileupAnalyser::GetEnergyMeasures(Int_t i, Double_t &Amplitude, Double_t &Integral) {
	Amplitude = sampleAmplitude(i);
	Integral = sampleIntegral(i);
}

void PndEmcPSAFPGAPileupAnalyser::put(float valueToStore) {

	if(baselineMode==kWindow && local_time <= BaselineStopSample) {
		if(local_time >= BaselineStartSample) {
			BaselineSum += valueToStore;
		} 
		if(local_time == BaselineStopSample) {
			baseline_value = BaselineSum / (BaselineStopSample-BaselineStartSample+1);
		}
	} else {

		float signal = valueToStore - baseline_value;
		float signal_delayed = (analyserParams.sig_delay>0) ? Signal_delay->put(signal) : signal;

		float mwd_value = MWD_filter1->put(signal);
		float mwd2_value = MWD_filter2->put(mwd_value);
		float mwd3_value = MWD_filter3->put(mwd2_value);

		float cf_value = CF_filter->put(mwd3_value);
		float cf_value_delayed = (analyserParams.cf_fit_offset>0) ? CF_delay->put(cf_value) : cf_value;
		float cf_value_prev = CF_prev->put(cf_value_delayed);

		status = kUndefined;

		if (!pulse_detected) {
			if(signal > analyserParams.hit_threshold) { 	// new pulse detected..reset pulse specific quantities
				status = kPulseDetected;
				energy_finished = false;
				timing_finished = false;
				pulse_detected = true;
				rough_pulse_timing = local_time;
				cf_crossing = 0;
				in_cfRise = false;
				integral[Number_of_hits] = 0.0;
				amplitude[Number_of_hits] = 0.0;

				if(fVerbose>=5) std::cout << "first time over thres: " << local_time << std::endl;	
			} else {
				status = kBaseline;
			}
		} 

		int dT = local_time - rough_pulse_timing;

		if(pulse_detected) {		 							// over thres

			if(status!=kPulseDetected) status = kInPulse;

			if( (!(dT<analyserParams.sig_delay)) && (!energy_finished) ) {			// delayed over thres
				if(fVerbose>=5) std::cout << local_time << " ";	
				// Integral
				integral[Number_of_hits] += signal_delayed;
				// Amplitude
				if(cf_crossing==0 && (signal_delayed > amplitude[Number_of_hits])) {
					amplitude[Number_of_hits] = signal_delayed;
				}
				if(signal_delayed < analyserParams.hit_threshold) {
					energy_finished = true;
					if(fVerbose>=5) std::cout << local_time << std::endl;;	
				}
			}


			if( (!(dT<analyserParams.cf_delay)) && (!timing_finished) ) {	  
				if((cf_value_prev < 0.0) && (cf_value_delayed >= 0.0)) { 		// new zero crossing
					CF_Fitter->reset();
					CF_Fitter->putPoint(local_time-1, cf_value_prev);
					CF_Fitter->putPoint(local_time, cf_value_delayed);
					CF_Fitter->fit();
					double a = CF_Fitter->offset();
					double k = CF_Fitter->slope();
					cfZero[cf_crossing++] = (k>0) ? -a/k : 0;
					in_cfRise = true;
				}

				if(cf_value_delayed< cf_value_prev) {
					if(in_cfRise) {
						cfRise[cf_crossing-1] = cf_value_prev - cfRise[cf_crossing-1];
						in_cfRise = false;
					} else {
						cfRise[cf_crossing] = cf_value_delayed;
					}
				}
				if(energy_finished) {
					timing_finished = true;
				}
			}

			if(energy_finished && timing_finished) {

				time[Number_of_hits] = (cf_crossing>0) ? cfZero[0] : rough_pulse_timing-analyserParams.rough_timing_corr;

				if(fVerbose>=2) {
					std::cout << "Finaliyzing " << Number_of_hits << " hit" << std::endl;	
					std::cout << "  time: " <<  time[Number_of_hits] << std::endl;
					std::cout << "  ampitude: " << amplitude[Number_of_hits] << "\t";
					std::cout << "  integral: " << integral[Number_of_hits] << std::endl;
				}

				if(fVerbose >= 3) {
					std::cout << "all zeroCrossings t/rise" << std::endl;
					for(unsigned int i=0; i<cf_crossing; ++i) {
						std::cout << cfZero[i] << "/" << cfRise[i] << std::endl;
					}
				}


				if( (!func_defined) || (integral[Number_of_hits] < Int_thres->Eval(amplitude[Number_of_hits]))) { 	//single pulse
					status = kPulseFinished;
					if(fVerbose>=2) std::cout << "single hit" << std::endl;
					pileups[Number_of_hits] = kSingle;
					Number_of_hits++;
				} else {									//pileup pulse
					status = kPileupFinished;
					if(fVerbose>=2) std::cout << "pileup hit" << std::endl;
					//i2 = i_ges - i1
					integral[Number_of_hits+1] = integral[Number_of_hits] - Int_mean->Eval(amplitude[Number_of_hits]);	
					//i1 = i_ges - i2
					integral[Number_of_hits]-= integral[Number_of_hits+1];
					amplitude[Number_of_hits+1] = Int_mean->GetX(integral[Number_of_hits+1], 0, 0, 5.e-4, 100, kTRUE); 

					if(cf_crossing>1) {
						float max_rise = 0;
						for(unsigned int i=1; i<cf_crossing; ++i) {
							if(cfRise[i] > max_rise) {
								max_rise = cfRise[i];
								time[Number_of_hits+1] = cfZero[i];
							}
						}
					} else {
						time[Number_of_hits+1] = rough_pulse_timing-analyserParams.rough_timing_corr;
					}
					pileups[Number_of_hits] = kPileup1;
					pileups[Number_of_hits+1] = kPileup2;


					if(fVerbose>=2) {
						std::cout << "amp1: " << amplitude[Number_of_hits] << "\tint1: " << integral[Number_of_hits] << "\tamp2: " << amplitude[Number_of_hits+1] << "\tint2: " << integral[Number_of_hits+1] << std::endl;
					}

					Number_of_hits+=2;
				}
				pulse_detected = false;
			}
		}

		if(fVerbose>=9) {
			using namespace std;
			cout << "#" << local_time <<" ";
			cout << "mwd: " << mwd_value <<" ";
			cout << "mwd 2: " << mwd2_value <<" ";
			cout << "mwd 3: " << mwd3_value <<" ";
			cout << "cf delay: " << cf_value_delayed <<" ";
			if(pulse_detected) cout << "detec ";
			if(!energy_finished) cout << "energy method running ";
			if(!timing_finished) cout << "timing method running ";
			cout << endl;
		}
	}	
	local_time++;
}

ClassImp(PndEmcPSAFPGAPileupAnalyser)
