/*
 * PndLmdLumiFitResult.h
 *
 *  Created on: Jun 28, 2012
 *      Author: steve
 */

#ifndef PNDLMDLUMIFITRESULT_H_
#define PNDLMDLUMIFITRESULT_H_

#include <map>
#include <string>

#include "TObject.h"

class ModelFitResult;
class PndLmdLumiFitOptions;

/**
 * \brief This class contains the fit result information and most importantly the luminosity and its errors!!
 *
 * This class contains all of the information of the fit including the final parameter values
 * that were obtained by the fitting procedure.
 *
 */
class PndLmdLumiFitResult: public TObject {
private:
	double luminosity_sys_err;

	const PndLmdLumiFitOptions *fit_options;
	ModelFitResult *model_fit_result;


public:
	PndLmdLumiFitResult(const PndLmdLumiFitOptions *fit_options_);
	PndLmdLumiFitResult();
	~PndLmdLumiFitResult();

	double getLuminosity() const;
	double getLuminositySysError() const;
	double getLuminosityStatError() const;
	double getLuminosityError() const;

	ModelFitResult* getModelFitResult();

	double getRedChiSquare() const;

	void setFitStatus(int fit_status_);
	void setChiSquare(double chi_square_);
	void setNDF(double ndf_);
	void setLuminositySysError(double luminosity_sys_err_);

	void setModelFitResult(ModelFitResult &fit_result);

	bool checkFitOptions(const PndLmdLumiFitOptions *fit_options_) const;

	const PndLmdLumiFitOptions* getLumiFitOptions() const;

ClassDef(PndLmdLumiFitResult,1)
};

#endif /* PNDLMDLUMIFITRESULT_H_ */
