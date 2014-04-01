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

const map<PndLmdLumiFitOptions, PndLmdLumiFitResult*>& PndLmdFitStorage::getFitResults() const {
	return fit_results;
}

PndLmdLumiFitResult* PndLmdFitStorage::getFitResult(
		const PndLmdLumiFitOptions &fit_options) const {
	map<PndLmdLumiFitOptions, PndLmdLumiFitResult*>::const_iterator fit_result =
			fit_results.find(fit_options);
	if (fit_result != fit_results.end())
		return fit_result->second;
	else
		return 0;
}

void PndLmdFitStorage::addFitResult(const PndLmdLumiFitOptions *fit_options,
		PndLmdLumiFitResult* fit_result_) {
	fit_results[*fit_options] = fit_result_;
}
