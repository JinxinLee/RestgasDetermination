/*
 * SdsFE.cpp
 *
 *  Created on: Apr 4, 2011
 *      Author: esch
 */

#include "PndSdsFE.h"
#include <vector>
#include "Math/Interpolator.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TLegend.h"
PndSdsFE::PndSdsFE() {

	fFrontEndModel = new PndSdsFEAmpModelSimple();
	fFunctionRange = 12000;
	Function = new TF1("fFunction",fFrontEndModel,&PndSdsFEAmpModelSimple::Definition,0,fFunctionRange,3);
	Function->SetNpx(10000);

	Function->SetParName(0,"chargetime");
	Function->SetParName(1,"constantcurrent");
	Function->SetParName(2,"charge");

	Function->SetParameter("chargetime",100);
	Function->SetParameter("constcurrent",60);
	Function->SetParameter("threshold",1100);
	Function->SetParameter("frequency",150);

	fTimeStep = 1. / 150 * 1000.; // Dont forget to change the frequency here if you change it above

	fBaselineEpsilon = 1;
	GetTimeOffSet();
}

PndSdsFE::PndSdsFE(double chargetime, double constcurrent, double threshold, double frequency, int verbose) {

	fFrontEndModel = new PndSdsFEAmpModelSimple();
	fTimeStep = 1. / frequency * 1000.;
	fFunctionRange = 12000;
	fBaselineEpsilon = 1;
	Function = new TF1("fFunction",fFrontEndModel,&PndSdsFEAmpModelSimple::Definition,0,fFunctionRange,3);
	Function->SetNpx(10000);
	Function->SetParName(0,"chargetime");
	Function->SetParName(1,"constantcurrent");
	Function->SetParName(2,"charge");

	Function->SetParameter(0,chargetime);
	Function->SetParameter(1,constcurrent);

	GetTimeOffSet();

	fThreshold = threshold;

	InterpolatorList(threshold);
}

PndSdsFE::~PndSdsFE() {
}

double PndSdsFE::GetTotFromCharge(Double_t Charge){

	Function->SetParameter("charge",Charge);
	fMaximumAmplitude = Function->GetMaximumX();

	if (Function->GetParameter("constantcurrent") <= 0){
		Error("PndSdsFE::GetTotFromCharge","const. current is less or equal zero -> now set to 60 e/ns");
		Function->SetParameter("constantcurrent",60);
	}

	if (Function->GetParameter("chargetime")<= 0){
		Error("PndSdsFE::GetTotFromCharge","charge time is less than zero -> now set to 100 ns");
		Function->SetParameter("chargetime",100);
	}

	if (fThreshold < 0){
		Error("PndSdsFE::GetTotFromCharge","Threshold is less than zero -> now set to 0 e");
		fThreshold = 0;
	}

	if (Function->GetParameter("charge")<= fThreshold){
		Warning("PndSdsFE::GetTotFromCharge","charge is equal or less than threshold -> zero TOT");
		return 0;
	}

	double start_exakt=Function->GetX(fThreshold,0,fMaximumAmplitude );
	double stop_exakt=Function->GetX(fThreshold,fMaximumAmplitude,Function->GetXmax());

	double start=DigitizeTime(Function->GetX(fThreshold,0,fMaximumAmplitude)+fTimeOffSet);
	double stop=DigitizeTime(Function->GetX(fThreshold,fMaximumAmplitude,Function->GetXmax())+fTimeOffSet);
//	printf("start exakt:%f   \n",start_exakt);
//	printf("start 	   :%f   \n",start);
//	printf("stop exakt:%f    \n",stop_exakt);
//	printf("stop 	   :%f    \n",stop);

		//return stop-start;		// with digitization
		return stop_exakt - start_exakt;  // without digitization
}

double PndSdsFE::GetChargeFromTot(double tot){
	return   inter->Eval(tot);
}

double PndSdsFE::GetTimeWalkFromCharge(double Charge){
	Function->SetParameter("charge",Charge);
	fMaximumAmplitude = Function->GetMaximumX();
	return Function->GetX(fThreshold,0,fMaximumAmplitude); //(xmin<x<xmax)
}

double PndSdsFE::GetTimeBackToBaseline(double Charge){
	Function->SetParameter("charge",Charge);
	fMaximumAmplitude = Function->GetMaximumX();
	return Function->GetX(fBaselineEpsilon,fMaximumAmplitude,Function->GetXmax());
}

double PndSdsFE::DigitizeTime(double time){

		return TMath::Floor(( time / fTimeStep )) * fTimeStep + fTimeStep;
}

double PndSdsFE::GetTimeStamp(double eventtime, double tof, double charge){
	//GetChargeToTimeWalk(charge);
	return DigitizeTime(GetTimeWalkFromCharge(charge)+eventtime+tof);

}
double PndSdsFE::GetTimeOffSet(){
	fTimeOffSet = fRand.Uniform(fTimeStep);
}

void PndSdsFE::InterpolatorList(double threshold){

	number_of_support_points = 2000;
	number_of_max_electrons = 630000;
	stepsize = number_of_max_electrons/number_of_support_points;

	printf("stepsize %f \n",stepsize);

	for( i=0; i < number_of_support_points; i++){
		if(i*stepsize<fThreshold){
				continue;
		}
		else{

		y_value.push_back(i*stepsize);
		x_value.push_back(GetTotFromCharge(i*stepsize));

		printf("x-value %f ,y-value %f \n",x_value.back(),y_value.back());

		if(x_value.back()< x_value.back()-1) {
			printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++%d \n",i );
			printf("x-value[i-3] %f ,x-value[i-2] %f \n",x_value.back()-3,x_value.back()-2);

			printf("x-value[i] %f ,x-value[i-1] %f \n",x_value.back(),x_value.back());
			}
		}
	}

	TCanvas *t = new TCanvas("blubb2");

	TGraph* graph = new TGraph(x_value.size(), &x_value.at(0),&y_value.at(0));
	graph->Draw("AP");
	printf("Blubb \n");


	inter =  new ROOT::Math::Interpolator(x_value,y_value, ROOT::Math::Interpolation::kCSPLINE);
}

ClassImp(PndSdsFE);


