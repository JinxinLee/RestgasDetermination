/*
 * PndLmdFitStorage.h
 *
 *  Created on: Mar 26, 2014
 *      Author: steve
 */

#ifndef PNDLMDFITSTORAGE_H_
#define PNDLMDFITSTORAGE_H_

#include "../fit/PndLmdLumiFitResult.h"
#include "../fit/PndLmdLumiFitOptions.h"

#include <map>

using std::map;

class PndLmdFitStorage {
	map<PndLmdLumiFitOptions, PndLmdLumiFitResult*> fit_results;

public:
	PndLmdFitStorage();
	virtual ~PndLmdFitStorage();

	const map<PndLmdLumiFitOptions, PndLmdLumiFitResult*>& getFitResults() const;
	PndLmdLumiFitResult* getFitResult(const PndLmdLumiFitOptions &fit_options) const;
	void addFitResult(const PndLmdLumiFitOptions *fit_options, PndLmdLumiFitResult* fit_result_);
};

#endif /* PNDLMDFITSTORAGE_H_ */
