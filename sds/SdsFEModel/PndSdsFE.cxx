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

PndSdsFE::PndSdsFE(double chargetime, double constcurrent, double threshold, double frequency, int verbose) {

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

PndSdsFE::~PndSdsFE() {
}

double PndSdsFE::GetTotFromCharge(Double_t Charge){

	fFunction->SetParameter("charge",Charge);
	fMaximumAmplitude = fFunction->GetMaximumX();

	if (fFunction->GetParameter("constantcurrent") <= 0){
		Error("PndSdsFE::GetTotFromCharge","const. current is less or equal zero -> now set to 60 e/ns");
		fFunction->SetParameter("constantcurrent",60);
	}

	if (fFunction->GetParameter("chargetime")<= 0){
		Error("PndSdsFE::GetTotFromCharge","charge time is less than zero -> now set to 100 ns");
		fFunction->SetParameter("chargetime",100);
	}

	if (fThreshold < 0){
		Error("PndSdsFE::GetTotFromCharge","Threshold is less than zero -> now set to 0 e");
		fThreshold = 0;
	}

	if (fFunction->GetParameter("charge")<= fThreshold){
		Warning("PndSdsFE::GetTotFromCharge","charge is equal or less than threshold -> zero TOT");
		return 0;
	}

	double start_exakt=fFunction->GetX(fThreshold,0,fMaximumAmplitude );
	double stop_exakt=fFunction->GetX(fThreshold,fMaximumAmplitude,fFunction->GetXmax());

	double start=DigitizeTime(fFunction->GetX(fThreshold,0,fMaximumAmplitude)+fTimeOffSet);
	double stop=DigitizeTime(fFunction->GetX(fThreshold,fMaximumAmplitude,fFunction->GetXmax())+fTimeOffSet);
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
	fFunction->SetParameter("charge",Charge);
	fMaximumAmplitude = fFunction->GetMaximumX();
	return fFunction->GetX(fThreshold,0,fMaximumAmplitude); //(xmin<x<xmax)
}

double PndSdsFE::GetTimeBackToBaseline(double Charge){
	fFunction->SetParameter("charge",Charge);
	fMaximumAmplitude = fFunction->GetMaximumX();
	return fFunction->GetX(fBaselineEpsilon,fMaximumAmplitude,fFunction->GetXmax());
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

void PndSdsFE::CreateInterpolatorList(){

	number_of_support_points = 2000;
	number_of_max_electrons = 630000;
	stepsize = number_of_max_electrons/number_of_support_points;

	printf("stepsize %f \n",stepsize);

	for( i=0; i < number_of_support_points; i++){
		if(i*stepsize<fThreshold){
				continue;
		}
		else{

		fTot_list.push_back(i*stepsize);
		fCharge_list.push_back(GetTotFromCharge(i*stepsize));

		printf("x-value %f ,y-value %f \n",fCharge_list.back(),fTot_list.back());
		//printf("x-value %f ,y-value %f \n",fCharge_list[],fTot_list.back());

		if(fTot_list.back()< fTot_list.back()-1) {
			printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++%d \n",i );
			printf("x-value[i-3] %f ,x-value[i-2] %f \n",fTot_list.back()-3,fTot_list.back()-2);

			printf("x-value[i] %f ,x-value[i-1] %f \n",fTot_list.back(),fTot_list.back());
			}
		}
	}

	SaveInterpolatorList(fCharge_list,fTot_list);
}

void PndSdsFE::SaveInterpolatorList(std::vector<double> charge, std::vector<double> tot){

	// To save an object to a root file it needs to inherit from TObject.
	// std::vector does not inherit from TObject, but TVectorD does.
	// so i convert the std::vector to a TVectorD and save them to a root file.

	int size_x = charge.size();
	int size_y = tot.size();

	TVectorD charge_root(size_x);
	TVectorD tot_root(size_y);

	for(int bx=0; bx < size_x;bx++)
	{
		charge_root[bx] = charge.at(bx);
	}

	for(int by=0; by < size_x;by++)
	{
		tot_root[by] = tot.at(by);
	}

	// in den namen der interpolatorliste sollte die threshold und das model hinein. sind diese beiden angaben identisch
	// ist die interpolatorliste eindeutig.

	char s[20];

	sprintf(s,"interpolatorlist-modelnumber_%i-thr_%g.root",fFrontEndModel->GetModelNumber(),fThreshold);

	TFile f1(s,"RECREATE");

	TGraph* graph = new TGraph(charge.size(), &charge.at(0),&tot.at(0));
	graph->Draw("AP");
	graph->Write();

	charge_root.Write("charge");
	tot_root.Write("tot");
}

void PndSdsFE::GetInterpolatorList(){
	// is list available?

	char s[20];
	sprintf(s,"interpolatorlist-modelnumber_%i-thr_%g.root",fFrontEndModel->GetModelNumber(),fThreshold);

	 TFile *fInFile = new TFile(s,"read");

	 if(fInFile->IsZombie()) {
		 std::cout << "No interpolator list to load, create one... " << std::endl;
		 CreateInterpolatorList();
		 LoadInterpolatorList();
		 return;
	 }
	 else
	 {
		 std::cout << "Loading Interpolator List " << std::endl;
		 LoadInterpolatorList();
		 return;
	 }

}

void PndSdsFE::LoadInterpolatorList(){

	char dd[20];
	sprintf(dd,"interpolatorlist-modelnumber_%i-thr_%g.root",fFrontEndModel->GetModelNumber(),fThreshold);

	 TFile *fInFile = new TFile(dd);

	 if(fInFile->IsZombie()) {
		 std::cout << "PndSdsFE::LoadInterpolatorList(): error loading root file " << std::endl;

		 return;
	 }
	 else
	 {
		 std::cout << "PndSdsFE::LoadInterpolatorList(): Loading list done " << std::endl;

	 }

	 TVectorD *charge = (TVectorD*)fInFile->Get("charge");
	 TVectorD *tot = (TVectorD*)fInFile->Get("tot");

	 int size_charge = charge->GetNoElements();
	 int size_tot = tot->GetNoElements();

	 fCharge_list.clear();
	 fCharge_list.clear();

		for(int bxx=0; bxx < size_charge;bxx++)
		{
			//fCharge_list.push_back(charge[bxx]);
			fCharge_list.push_back((*charge)[bxx]);

		}

		for(int byy=0; byy < size_tot;byy++)
		{
		//	fTot_list.push_back(tot);
				fTot_list.push_back((*tot)[byy]);
		}

		inter =  new ROOT::Math::Interpolator(fCharge_list,fTot_list, ROOT::Math::Interpolation::kCSPLINE);
}

ClassImp(PndSdsFE);


