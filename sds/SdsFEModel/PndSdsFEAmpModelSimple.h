/*
 * SdsFEAmpModelSimple.h
 *
 *  Created on: Apr 4, 2011
 *      Author: esch
 *
 *      Class for the simple amplifier model definition
 *
 */

#ifndef PNDSDSFEAMPMODELSIMPLE_H_
#define PNDSDSFEAMPMODELSIMPLE_H_

#include "TF1.h"

class PndSdsFEAmpModelSimple {
public:
	PndSdsFEAmpModelSimple();
	virtual ~PndSdsFEAmpModelSimple();

	double Definition(double *x, double *params);

//	double fRisingTime;    		// Constant rising time from parameter database [ns]
//	double fConstantCurrent;  	// Current that unloads the capacitor [e/ns]
//	double fCharge; 			// [e]
//	double fPar; 				// parameter for function

private:

	TF1 fFunction;

};

#endif /* PNDSDSFEAMPMODELSIMPLE_H_ */
