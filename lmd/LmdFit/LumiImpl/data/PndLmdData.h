/*
 * PndLmdAngularData.h
 *
 *  Created on: Jun 27, 2012
 *      Author: steve
 */

#ifndef PNDLMDDATA_H_
#define PNDLMDDATA_H_

#include "PndLmdAbstractData.h"
#include "../LumiFitStructs.h"

#include "TH1D.h"
#include "TH2D.h"

/**
 * \brief Class used to describe 1D, 2D and unbinned LMD data, using ROOT
 * histograms and trees.
 * Important is that the user has to set the dimension variables first,
 * before any storage will automatically be allocated.
 */
class PndLmdData: public PndLmdAbstractData {
private:
	/** ROOT 1D histogram as the container of the data */
	TH1D* hist_1d;
	/** ROOT 2D histogram as the container of the data */
	TH2D* hist_2d;

	/**
	 * In case this is a simulation a reference value for the luminosity
	 * can/should be used. This value corresponds to the luminosity per event,
	 * so multiplying by the number of events results in the actual integrated
	 * luminosity.
	 */
	double reference_luminosity_per_event;

	void init1DData();
	void init2DData();

public:
	PndLmdData();

	virtual ~PndLmdData();
	// getter methods

	TH1D* get1DHistogram() const;
	TH2D* get2DHistogram() const;

	void cloneData(const PndLmdAbstractData &lmd_abs_data);

	void add(const PndLmdAbstractData &lmd_abs_data_addition);

	// histogram filling methods
	virtual void addData(double primary_value, double secondary_value = 0);

	double getReferenceLuminosity() const;
	double getReferenceLuminosityPerEvent() const;
	void setReferenceLuminosityPerEvent(double reference_luminosity_per_event_);

	bool operator<(const PndLmdData &lmd_data) const;
	bool operator>(const PndLmdData &lmd_data) const;
	bool operator==(const PndLmdData &lmd_data) const;
	bool operator!=(const PndLmdData &lmd_data) const;

ClassDef(PndLmdData,1)
};

#endif /* PNDLMDDATA_H_ */
