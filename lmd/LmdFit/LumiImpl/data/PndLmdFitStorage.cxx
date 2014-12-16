/*
 * PndLmdFitStorage.cxx
 *
 *  Created on: Mar 26, 2014
 *      Author: steve
 */

#include "PndLmdFitStorage.h"

PndLmdFitStorage::PndLmdFitStorage() {
	// TODO Auto-generated constructor stub

}

PndLmdFitStorage::~PndLmdFitStorage() {
	// TODO Auto-generated destructor stub
}

const map<PndLmdLumiFitOptions, PndLmdLumiFitResult>& PndLmdFitStorage::getFitResults() const {
	return fit_results;
}

PndLmdLumiFitResult PndLmdFitStorage::getFitResult(
		const PndLmdLumiFitOptions &fit_options) const {
	PndLmdLumiFitResult return_result;
	map<PndLmdLumiFitOptions, PndLmdLumiFitResult>::const_iterator fit_result =
			fit_results.find(fit_options);
	if (fit_result != fit_results.end())
		return_result = fit_result->second;

	return return_result;
}

void PndLmdFitStorage::addFitResult(const PndLmdLumiFitOptions &fit_options,
		const PndLmdLumiFitResult &fit_result_) {
	fit_results[fit_options] = fit_result_;
}
