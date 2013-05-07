/*
 * PndLmdROOTDataModel1D.cpp
 *
 *  Created on: Jan 20, 2013
 *      Author: steve
 */

#include "PndLmdROOTDataModel1D.h"

#include <iostream>

#include "TEfficiency.h"
#include "TGraphAsymmErrors.h"
#include "TCanvas.h"

PndLmdROOTDataModel1D::PndLmdROOTDataModel1D(std::string name_,
		TEfficiency *acceptance_, interpolation_type type_) :
		Model1D(name_), acceptance(acceptance_) {

	TCanvas can;
	acceptance->Draw();
	can.Update();
	graph = acceptance->GetPaintedGraph();

	intpol_type = type_;
	if (intpol_type == CONSTANT) {
		model_func = &PndLmdROOTDataModel1D::evaluateConstant;
	} else if (intpol_type == SPLINE) {
		model_func = &PndLmdROOTDataModel1D::evaluateSpline;
	} else {
		model_func = &PndLmdROOTDataModel1D::evaluateLinear;
	}

	determineAcceptanceBounds();
}

PndLmdROOTDataModel1D::~PndLmdROOTDataModel1D() {
	// TODO Auto-generated destructor stub
}

void PndLmdROOTDataModel1D::updateDomainFromPars(double *par) {

}

void PndLmdROOTDataModel1D::determineAcceptanceBounds() {
	// ok this procedure is not very foolproof... better would be an edge detection
	// so scanning once from left to right for the lower edge and then right to left for the upper edge!!
	//loop over bins and find out bins that are on the edge of the threshold
	int last_bin_index_at_zero = -10;
	double last_bin = 0.0;
	double y = 0.0;
	//std::cout << "Calculating acceptance bounds!!!!!!!!!!!!!" << std::endl;
	//std::cout << "Number of bins: " << graph->GetN() << std::endl;
	for (int i = 0; i < graph->GetN(); i++) {
		y = acceptance->GetEfficiency(i);
		//std::cout << "acceptance value at bin " << i << "(" << graph->GetX()[i]
		//		<< "): " << y << std::endl;
		if (y != 0.0) {
			if (last_bin_index_at_zero == i - 1) {
				acc_range_low = graph->GetX()[i - 1];
				//std::cout << "first bin: " << acc_range_low << std::endl;
			}
		} else {
			last_bin_index_at_zero = i;
			if (last_bin > 0.0) {
				acc_range_high = graph->GetX()[i];
				//std::cout << "last bin: " << acc_range_high << std::endl;
				break;
			}
		}
		last_bin = y;
	}
	if (acc_range_high < acc_range_low)
		acc_range_high = graph->GetX()[graph->GetN() - 1];
	//std::cout << "Calculated acceptance bounds to " << acc_range_low << " and "
	//		<< acc_range_high << std::endl;
	setDomain(acc_range_low, acc_range_high);
}

void PndLmdROOTDataModel1D::initModelParameters() {

}

double PndLmdROOTDataModel1D::evaluateConstant(double *x) const {
	return acceptance->GetEfficiency(acceptance->FindFixBin(x[0]));
}

double PndLmdROOTDataModel1D::evaluateLinear(double *x) const {
	return graph->Eval(x[0]);
}

double PndLmdROOTDataModel1D::evaluateSpline(double *x) const {
	return graph->Eval(x[0], 0, "S");
}

double PndLmdROOTDataModel1D::eval(double *x) const {
	if (acc_range_low > x[0] || acc_range_high < x[0])
		return 0.0;
	return (this->*model_func)(x);
}

void PndLmdROOTDataModel1D::updateDomain() {
	setDomain(acc_range_low, acc_range_high);
}
