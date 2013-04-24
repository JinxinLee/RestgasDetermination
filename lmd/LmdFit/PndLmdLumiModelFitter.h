/*
 * PndLmdLumiModelFitter.h
 *
 *  Created on: Jan 21, 2013
 *      Author: steve
 */

#ifndef PNDLMDLUMIMODELFITTER_H_
#define PNDLMDLUMIMODELFITTER_H_

#include "Model1D.h"

#include <vector>

class PndLmdModelFactory;
class PndLmdData;
class PndLmdAcceptance;
class PndLmdLumiFitOptions;
class PndLmdLumiFitResult;

class PndLmdLumiModelFitter {
private:
	//this are some vectors to store the information for the chi2 calculation
	std::vector<double*> x;
	std::vector<double> z;
	std::vector<double> errorz;

	Model1D *model;

	PndLmdModelFactory *signal_model_fac;

	// the chisquare function
	double chi2(const double *par);

	void fillVectors(PndLmdData *lmd_data, Model1D &model1d, PndLmdLumiFitOptions *fit_options);

	//void probeChi2(double *par);

	void doFit(PndLmdData *lmd_data, PndLmdLumiFitOptions *fit_options,
			PndLmdLumiFitResult* fit_result);

public:
	PndLmdLumiModelFitter();
	virtual ~PndLmdLumiModelFitter();

	PndLmdLumiFitResult* fitTo(PndLmdData *lmd_data, PndLmdAcceptance *lmd_acc,
			PndLmdLumiFitOptions *fit_options);
};

#endif /* PNDLMDLUMIMODELFITTER_H_ */
