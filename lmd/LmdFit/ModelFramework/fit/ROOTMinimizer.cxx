/*
 * PndLmdLumiModelFitter.cxx
 *
 *  Created on: Jan 21, 2013
 *      Author: steve
 */

#include "ROOTMinimizer.h"

#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TMath.h"

ROOTMinimizer::ROOTMinimizer(ModelControlParameter &control_param_) :
		ModelMinimizer(control_param_) {
	std::cout << "Initializing Minuit Minimizer..." << std::endl;

	//Minimize = Migrad+Simplex
	min = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Combined");
	// set tolerance , etc...
	min->SetMaxFunctionCalls(100000); // for Minuit/Minuit2
	min->SetMaxIterations(1000);
	min->SetTolerance(0.001);
	//min->SetPrecision(0.001);
	min->SetPrintLevel(5);
}

ROOTMinimizer::~ROOTMinimizer() {
	// TODO Auto-generated destructor stub
}

const ROOT::Math::Minimizer* ROOTMinimizer::getROOTMinimizer() const {
	return min;
}

int ROOTMinimizer::minimize() {
	std::cout << "Setting up fit..." << std::endl;
	min->Clear();
	// create function wrapper for minmizer  a IMultiGenFunction type
	std::cout << "Number of free parameters in fit: "
			<< control_param.getParameterList().size() << std::endl;
	ROOT::Math::Functor fc(&control_param, &ModelControlParameter::evaluate,
			control_param.getParameterList().size());
	min->SetFunction(fc);

	// Set the free variables to be minimized!
	for (unsigned int i = 0; i < control_param.getParameterList().size(); i++) {
		double stepsize = TMath::Abs(
				0.01 * control_param.getParameterList()[i].value);
		if (0.0 == control_param.getParameterList()[i].value)
			stepsize = 0.001;
		min->SetVariable(i, control_param.getParameterList()[i].name,
				control_param.getParameterList()[i].value, stepsize);
	}
	std::cout << "Finished setting up fit!" << std::endl;

	std::cout << "Performing fit..." << std::endl;
	int error_code = 0;
	if (!min->Minimize())
		error_code = 1;
	std::cout << "Fit done!" << std::endl;
	return error_code;
}
