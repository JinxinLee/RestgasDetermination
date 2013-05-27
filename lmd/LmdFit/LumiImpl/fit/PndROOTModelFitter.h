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

class TH1D;
class TH2D;

class PndROOTModelFitter: public ModelFitInterface {
private:
	ROOT::Math::Minimizer* min;

	int minimize();

public:
	PndROOTModelFitter();
	virtual ~PndROOTModelFitter();

	const ROOT::Math::Minimizer* getROOTMinimizer() const;

	void fillFitData1D(TH1D* hist_1d, std::pair<double, double> &fit_range, bool with_integral_scaling = true);

	void fillFitData2D(TH2D* hist_2d, std::pair<double, double> &fit_range_x
			, std::pair<double, double> &fit_range_y);
};

#endif /* PNDLMDMODELFITTER_H_ */
