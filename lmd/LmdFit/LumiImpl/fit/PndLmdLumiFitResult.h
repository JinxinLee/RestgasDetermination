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

class PndLmdLumiFitOptions;
class ModelFitResult;


/**
 * \brief This class contains the fit result information and most importantly the luminosity and its errors!!
 *
 * This class contains all of the information of the fit including the final parameter values
 * that were obtained by the fitting procedure.
 *
 */
class PndLmdLumiFitResult : public TObject {
private:
	double luminosity_sys_err;

	PndLmdLumiFitOptions *fit_options;
	ModelFitResult *model_fit_result;

public:
	PndLmdLumiFitResult(PndLmdLumiFitOptions *fit_options_);
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
	void addFitParameter(std::string name_, double value_, double error_);
	void setNDF(double ndf_);
	void setLuminositySysError(double luminosity_sys_err_);

	bool checkFitOptions(PndLmdLumiFitOptions *fit_options_) const;

	PndLmdLumiFitOptions* getLumiFitOptions() const;

	ClassDef(PndLmdLumiFitResult,1)
};

#endif /* PNDLMDLUMIFITRESULT_H_ */
