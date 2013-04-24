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

/**
 * \brief This class contains the fit result information and most importantly the luminosity and its errors!!
 *
 * This class contains all of the information of the fit including the final parameter values
 * that were obtained by the fitting procedure.
 *
 */
class PndLmdLumiFitResult : public TObject {
private:
	double luminosity, luminosity_stat_err, luminosity_sys_err;

	std::map<std::string, std::pair<double, double> > parameters;

	PndLmdLumiFitOptions *fit_options;

	double chi2;
	unsigned int ndf;

public:
	PndLmdLumiFitResult(PndLmdLumiFitOptions *fit_options_);
	PndLmdLumiFitResult();
	~PndLmdLumiFitResult();

	void init();

	double getLuminosity() const;
	double getLuminositySysError() const;
	double getLuminosityStatError() const;
	double getLuminosityError() const;

	double getChiSquare() const;
	unsigned int getNDF() const;
	double getParameterValue(std::string name_) const;

	void setChiSquare(double chi2_);
	void setNDF(unsigned int ndf_);
	void addParameterValue(std::string name_, double value_, double value_error_);

	void setLuminosity(double luminosity_);
	void setLuminositySysError(double luminosity_sys_err_);
	void setLuminosityStatError(double luminosity_stat_err_);

	bool checkFitOptions(PndLmdLumiFitOptions *fit_options_) const;

	PndLmdLumiFitOptions* getLumiFitOptions() const;

	ClassDef(PndLmdLumiFitResult,1)
};

#endif /* PNDLMDLUMIFITRESULT_H_ */
