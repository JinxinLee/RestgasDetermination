/*
 * SdsFE.cpp
 *
 *  Created on: Apr 4, 2011
 *      Author: esch
 */

#include "PndSdsFESimple.h"
#include <vector>
#include "Math/Interpolator.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TLegend.h"

PndSdsFESimple::PndSdsFESimple() {

	fFrontEndModel = new PndSdsFEAmpModelSimple();
	fFunctionRange = 22000;
	fFunction = new TF1("fFunction",fFrontEndModel,&PndSdsFEAmpModelSimple::Definition,0,fFunctionRange,3);
	fFunction->SetNpx(20000);

	fFunction->SetParName(0,"chargetime");
	fFunction->SetParName(1,"constantcurrent");
	fFunction->SetParName(2,"charge");

	fFunction->SetParameter("chargetime",100);
	fFunction->SetParameter("constcurrent",60);
	fFunction->SetParameter("threshold",1100);
	fFunction->SetParameter("frequency",150);

	fTimeStep = 1. / 150 * 1000.; // Dont forget to change the frequency here if you change it above

	fBaselineEpsilon = 1;
	GetTimeOffSet();

	fThreshold = 1100;

	GetInterpolatorList();
}

PndSdsFESimple::PndSdsFESimple(double chargetime, double constcurrent, double threshold, double frequency, int verbose) {

	fFrontEndModel = new PndSdsFEAmpModelSimple();
	fTimeStep = 1. / frequency * 1000.;
	fFunctionRange = 22000;
	fBaselineEpsilon = 1;
	fFunction = new TF1("fFunction",fFrontEndModel,&PndSdsFEAmpModelSimple::Definition,0,fFunctionRange,3);
	fFunction->SetNpx(20000);
	fFunction->SetParName(0,"chargetime");
	fFunction->SetParName(1,"constantcurrent");
	fFunction->SetParName(2,"charge");

	fFunction->SetParameter(0,chargetime);
	fFunction->SetParameter(1,constcurrent);

	GetTimeOffSet();

	fThreshold = threshold;

	//CreateInterpolatorList(threshold);

	GetInterpolatorList();
}

PndSdsFESimple::~PndSdsFESimple() {
	delete fFrontEndModel;
	delete fFunction;
}


ClassImp(PndSdsFESimple);


