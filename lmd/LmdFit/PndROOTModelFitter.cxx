/*
 * PndLmdLumiModelFitter.cxx
 *
 *  Created on: Jan 21, 2013
 *      Author: steve
 */

#include "PndROOTModelFitter.h"

#include "Math/Factory.h"
#include "Math/Functor.h"

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
	// create function wrapper for minmizer  a IMultiGenFunction type
	ROOT::Math::Functor fc(this, &PndROOTModelFitter::chi2,
			getFreeParameterList().size());
	min->SetFunction(fc);

	// Set the free variables to be minimized!
	for (unsigned int i = 0; i < getFreeParameterList().size(); i++) {
		min->SetVariable(i, getFreeParameterList()[i]->getName(),
				getFreeParameterList()[i]->getValue(), 0.01 * getFreeParameterList()[i]->getValue());
	}
	std::cout << "Finished setting up fit!" << std::endl;

	std::cout << "Performing fit..." << std::endl;
	int error_code = 0;
	if(!min->Minimize())
		error_code = 1;
	std::cout << "Fit done!" << std::endl;
	return error_code;
}
