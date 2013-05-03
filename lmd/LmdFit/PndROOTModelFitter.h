/*
 * PndROOTModelFitter.h
 *
 *  Created on: Jan 21, 2013
 *      Author: steve
 */

#ifndef PNDLMDMODELFITTER_H_
#define PNDLMDMODELFITTER_H_

#include "ModelFitInterface.h"
#include "Model1D.h"

#include "Math/Minimizer.h"

class PndLmdModelFactory;
class PndLmdData;
class PndLmdAcceptance;
class PndLmdLumiFitOptions;
class PndLmdLumiFitResult;

class PndROOTModelFitter : public ModelFitInterface {
private:
	ROOT::Math::Minimizer* min;

	int minimize();

public:
	PndROOTModelFitter();
	virtual ~PndROOTModelFitter();

	const ROOT::Math::Minimizer* getROOTMinimizer() const;
};

#endif /* PNDLMDMODELFITTER_H_ */
