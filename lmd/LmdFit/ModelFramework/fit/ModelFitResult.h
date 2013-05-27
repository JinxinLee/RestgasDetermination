/*
 * ModelFitResult.h
 *
 *  Created on: May 23, 2013
 *      Author: steve
 */

#ifndef MODELFITRESULT_H_
#define MODELFITRESULT_H_

#include <set>
#include <string>

class ModelFitResult {
public:
	struct fit_parameter {
		std::string name;
		double value;
		double error;
		fit_parameter() {
			name = "";
			value = 0.0;
			error = 0.0;
		}
		fit_parameter(std::string name_) {
			name = name_;
			value = 0.0;
			error = 0.0;
		}
		fit_parameter(std::string name_, double value_, double error_) {
			name = name_;
			value = value_;
			error = error_;
		}
		bool operator<(const fit_parameter &rhs) const {
			if (name.compare(rhs.name) < 0) {
				return true;
			}
			return false;
		}
	};

private:
	int fit_status;
	unsigned int ndf;
	std::set<fit_parameter> fit_parameters;
	double chi2;

public:
	ModelFitResult();
	virtual ~ModelFitResult();

	double getChiSquare() const;
	unsigned int getNDF() const;
	void setChiSquare(double chi2_);
	void setNDF(unsigned int ndf_);

	void addFitParameter(std::string name_, double value_, double error_);
	const fit_parameter& getFitParameter(std::string name_) const;
	int getFitStatus() const;
	void setFitStatus(int fit_status_);
};

#endif /* MODELFITRESULT_H_ */
