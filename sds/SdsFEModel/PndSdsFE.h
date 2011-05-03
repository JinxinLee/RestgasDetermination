/*
 * SdsFEAmpModel.h
 *
 *  Created on: Apr 4, 2011
 *      Author: esch
 *
 *      Class to calculate ToT, TimeWalk, TimeStamp and Time till the signal is back to baseline from a TF1 amplifier model.
 *
 *
 */

#ifndef PndSdsFE_H_
#define PndSdsFE_H_
#include "TObject.h"
#include "PndSdsFEAmpModelSimple.h"
#include "TMath.h"
#include "TF1.h"
#include "TRandom2.h"
#include "Math/Interpolator.h"
#include "Math/InterpolationTypes.h"
#include "PndSdsTotChargeConversion.h"

class PndSdsFE: public TObject {
public:
	PndSdsFE();
	PndSdsFE(double charsingtime, double constcurrent, double threshold, double frequency, int verbose);
	virtual ~PndSdsFE();

	double GetTotFromCharge(Double_t charge);			// Calculates the ToT value from the charge
	double GetChargeFromTot(double tot);				// Calculates the charge from the tot value
	double GetTimeWalkFromCharge(double charge);		// Calculates the TimeWalk from the charge
	double GetTimeBackToBaseline(double charge);		// Calculates the time from start of the signal till the signal is fBaselineEpsilon close to baseline
	double GetTimeStamp(double eventtime, double tof, double charge);  // Calculates the TimeStamp of the signal


private:
	double DigitizeTime(double time);
	double GetTimeOffSet();
	void InterpolatorList(double threshold);  // Creates the Interpolatorlist for TotFromCharge calculation
	int number_of_support_points;
	int number_of_max_electrons ;
	PndSdsFEAmpModelSimple *fFrontEndModel;
	TF1 *Function;
	double fMaximumAmplitude;      // Saves position of the maximum amplitude of the signal
	double fThreshold;				// Threshold for Signal from Parameter database
	double fBaselineEpsilon;		//
	double fToF;					// Time of Flight from MC
	double fEventTime;				// EventTime from MC
	double fTimeOffSet;				// Random offset
	double fTimeStep;
	double fFunctionRange; 			//Range of the function. max 100 fC => 624 150.9 e => max.tot 10503 ns (with thr.= 1100 e)
	TRandom2 fRand;
	int i;
	double stepsize;

	std::vector<double> x_value;
	std::vector<double> y_value;
	ROOT::Math::Interpolator *inter;


	ClassDef(PndSdsFE,1);
};

#endif /* PndSdsFE_H_ */
