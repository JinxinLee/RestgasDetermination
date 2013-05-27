/*
 * PndLmdLumiModelFitter.cxx
 *
 *  Created on: Jan 21, 2013
 *      Author: steve
 */

#include "PndROOTModelFitter.h"

#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TMath.h"

#include "TH1D.h"
#include "TH2D.h"

PndROOTModelFitter::PndROOTModelFitter() {
	std::cout << "Initializing Minuit Minimizer..." << std::endl;

	//Minimize = Migrad+Simplex
	min = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Minimize");
	// set tolerance , etc...
	min->SetMaxFunctionCalls(100000); // for Minuit/Minuit2
	min->SetMaxIterations(1000);
	min->SetTolerance(0.1);
	//min->SetPrecision(0.001);
	min->SetPrintLevel(5);
}

PndROOTModelFitter::~PndROOTModelFitter() {
	// TODO Auto-generated destructor stub
}

const ROOT::Math::Minimizer* PndROOTModelFitter::getROOTMinimizer() const {
	return min;
}

int PndROOTModelFitter::minimize() {
	std::cout << "Setting up fit..." << std::endl;
	min->Clear();
	// create function wrapper for minmizer  a IMultiGenFunction type
	std::cout << "Number of free parameters in fit: "
			<< getFreeParameterList().size() << std::endl;
	ROOT::Math::Functor fc(this, &PndROOTModelFitter::chi2,
			getFreeParameterList().size());
	min->SetFunction(fc);

	// Set the free variables to be minimized!
	for (unsigned int i = 0; i < getFreeParameterList().size(); i++) {
		double stepsize = TMath::Abs(0.01 * getFreeParameterList()[i]->getValue());
		if (0.0 == getFreeParameterList()[i]->getValue())
			stepsize = 0.001;
		min->SetVariable(i, getFreeParameterList()[i]->getName(),
				getFreeParameterList()[i]->getValue(), stepsize);
	}
	std::cout << "Finished setting up fit!" << std::endl;

	std::cout << "Performing fit..." << std::endl;
	int error_code = 0;
	if (!min->Minimize())
		error_code = 1;
	std::cout << "Fit done!" << std::endl;
	return error_code;
}

void PndROOTModelFitter::fillFitData1D(TH1D* hist_1d,
		std::pair<double, double> &fit_range, bool with_integral_scaling) {
	clearData();

	std::vector<std::pair<double, double> > bin_range;
	bin_range.push_back(std::make_pair(0.0, 0.0));
	for (int i = 1; i <= hist_1d->GetNbinsX(); i++) {
		ModelFitInterface::data_point datapoint;

		datapoint.x[0] = hist_1d->GetBinCenter(i);
		datapoint.z = hist_1d->GetBinContent(i);
		datapoint.z_error = hist_1d->GetBinError(i);

		if (datapoint.x[0] < fit_range.first || datapoint.x[0] > fit_range.second)
			continue;
		if (datapoint.z == 0.0)
			continue;

		double scale = 1.0;
		if (with_integral_scaling) {
			bin_range[0].first = hist_1d->GetBinLowEdge(i);
			bin_range[0].second = bin_range[0].first + hist_1d->GetBinWidth(i);

			double int_func_real = model->Integral(bin_range, 0.001);
			double int_func_approx = model->evaluate(datapoint.x)
					* (bin_range[0].second - bin_range[0].first);

			if (int_func_approx > 0.0 && int_func_real > 0.0) {
				scale = int_func_approx / int_func_real;
			}
		}
		datapoint.scale = scale;
		datapoint.z = datapoint.z * scale;

		insertData(datapoint);
	}
}

void fillFitData2D(TH2D* hist_2d, std::pair<double, double> &fit_range_x
		, std::pair<double, double> &fit_range_y) {

}
