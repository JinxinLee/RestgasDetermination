#include "PndEmcPSAFPGADigitalFilterAnalyser.h"

#include "PndEmcPSAFPGAFilterCF.h"
#include "PndEmcPSAFPGAFilterMA.h"
#include "PndEmcPSAFPGAFilterMWD.h"
#include "PndEmcPSAFPGAFilterDelay.h"
#include "PndEmcPSAFPGALinFitter.h"

#include <iostream>
#include <fstream>
#include <string>

PndEmcPSAFPGADigitalFilterAnalyser::PndEmcPSAFPGADigitalFilterAnalyser() :
  PndEmcPSAFPGASampleAnalyser() {
  MWD_filter = new PndEmcPSAFPGAFilterMWD();
  analysis_start_position = 32;
  baseline_stop_position = 132;
}

PndEmcPSAFPGADigitalFilterAnalyser::PndEmcPSAFPGADigitalFilterAnalyser(const std::string &fname) :
  PndEmcPSAFPGASampleAnalyser(fname) {
  MWD_filter = new PndEmcPSAFPGAFilterMWD();
  analysis_start_position = 32;
  baseline_stop_position = 132;
}

PndEmcPSAFPGADigitalFilterAnalyser::~PndEmcPSAFPGADigitalFilterAnalyser() {
  delete MWD_filter;
}

void PndEmcPSAFPGADigitalFilterAnalyser::init(PndEmcPSAFPGASampleAnalyser::SampleAnalyserParams &params) {
  PndEmcPSAFPGASampleAnalyser::init(params);
  MWD_filter->set(analyserParams.mwd_tau, analyserParams.mwd_length);
  return;
}

void PndEmcPSAFPGADigitalFilterAnalyser::setBaselineInterval(int anl_start, int bl_stop) {
  analysis_start_position = anl_start;
  baseline_stop_position = bl_stop;
  return;
}

float PndEmcPSAFPGADigitalFilterAnalyser::baseline() {
  return baseline_value_calculated / (baseline_stop_position - analysis_start_position);
}

void PndEmcPSAFPGADigitalFilterAnalyser::reset() {
  local_time = 0;
  Number_of_hits = 0;
  baseline_value_calculated = 0.0;
  CF_filter->resetToZero();
  MA_triggering->resetToZero();
  Signal_delay->resetToZero();
  CF_delay->resetToZero();
  pulse_detected = false;
  return;
}

void PndEmcPSAFPGADigitalFilterAnalyser::put(float valueToStore) {
  float mwd_val = MWD_filter->put(valueToStore);
  if (local_time < analysis_start_position) {
    local_time++;
    return;
  }
  if (local_time < baseline_stop_position) {
    baseline_value_calculated += mwd_val;
    //
    // uncomment lines below for automatic baseline calculation; does not work for high rates
    baseline_value = baseline_value_calculated / (baseline_stop_position - analysis_start_position);
    local_time++;
    return;
  }
  float signal = mwd_val - baseline_value;
  float signal_delayed = Signal_delay->put(signal);
  float cf_value = CF_filter->put(signal);
  float cf_value_delayed = CF_delay->put(cf_value);
  float ma_value = MA_triggering->put(signal);

  int dT = local_time - rough_pulse_timing;
  if (!pulse_detected) {
    if((cf_value_delayed < 0.0) && (cf_value > 0.0) && (ma_value > analyserParams.hit_threshold)) {
      // hit detected
      // CF
      CF_Fitter->reset();
      CF_Fitter->putPoint(local_time, cf_value_delayed);
      cf_zero_crossing_points = 1;
      pulse_detected = true;
      rough_pulse_timing = local_time;
      integral[Number_of_hits] = 0.0;
      amplitude[Number_of_hits] = 0.0;
    }
  } else {
    // CF timing
    if(cf_zero_crossing_points < analyserParams.cf_fitter_length) {
      CF_Fitter->putPoint(local_time, cf_value_delayed);
      cf_zero_crossing_points++;
    }
    // Integral
    integral[Number_of_hits] += signal_delayed;
    // Amplitude
    if( (dT < 2*analyserParams.cf_delay) && (signal_delayed > amplitude[Number_of_hits]) ) {
      amplitude[Number_of_hits] = signal_delayed;
    }
    // Finalize pulse detection
    if ((dT > analyserParams.cf_delay) && (signal_delayed < analyserParams.hit_threshold/analyserParams.ma_trig_M)) {
      pulse_detected = false;
      CF_Fitter->fit();
      double a = CF_Fitter->offset();
      double k = CF_Fitter->slope();
      time[Number_of_hits] = 0.0;
      if(k > 0) 
	time[Number_of_hits] = -a/k;
      Number_of_hits++;
    }
  }

  local_time++;
}

ClassImp(PndEmcPSAFPGADigitalFilterAnalyser);
