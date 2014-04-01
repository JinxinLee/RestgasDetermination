/*
 * PndLmdHistogramData.h
 *
 *  Created on: Mar 26, 2014
 *      Author: steve
 */

#ifndef PNDLMDHISTOGRAMDATA_H_
#define PNDLMDHISTOGRAMDATA_H_

#include "PndLmdAbstractData.h"
#include "PndLmdFitStorage.h"

#include "TH1D.h" // these includes I need for the dictionary generation
#include "TH2D.h"

class PndLmdHistogramData: public PndLmdAbstractData  {
	/** ROOT 1D histogram as the container of the data */
	TH1D* hist_1d;
	/** ROOT 2D histogram as the container of the data */
	TH2D* hist_2d;

	PndLmdFitStorage fit_storage;

	void init1DData();
	void init2DData();

public:
	PndLmdHistogramData();
	virtual ~PndLmdHistogramData();

	TH1D* get1DHistogram() const;
	TH2D* get2DHistogram() const;

	void cloneData(const PndLmdAbstractData &lmd_abs_data);

	void add(const PndLmdAbstractData &lmd_abs_data_addition);

	// histogram filling methods
	virtual void addData(double primary_value, double secondary_value = 0);

	const map<PndLmdLumiFitOptions, PndLmdLumiFitResult*>& getFitResults() const;
	PndLmdLumiFitResult* getFitResult(const PndLmdLumiFitOptions &fit_options) const;
	void addFitResult(const PndLmdLumiFitOptions *fit_options, PndLmdLumiFitResult* fit_result_);

	ClassDef(PndLmdHistogramData, 1);
};

#endif /* PNDLMDHISTOGRAMDATA_H_ */
