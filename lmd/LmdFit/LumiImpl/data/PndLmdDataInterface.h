/*
 * PndLmdDataInterface.h
 *
 *  Created on: Apr 17, 2013
 *      Author: steve
 */

#ifndef PNDLMDDATAINTERFACE_H_
#define PNDLMDDATAINTERFACE_H_

#include <set>
#include <vector>

#include "TString.h"
#include "TObject.h"
#include "TFile.h"

using std::set;

namespace PndLmdFit {
/**
 * Struct which contains all of the values that are read in from data files
 * and are required for the filling of the lmd data objects.
 */
struct lmd_values {
	double t;
	double theta;
	double phi;

	bool reconstructed;
};

/**
 * Struct which defines a dimension of data.
 */
struct lmd_dimension {
	/** the number of bins on this axis/dimension */
	int bins;
	/** lower bound on this axis/dimension */
	double range_low;
	/** upper bound on this axis/dimension */
	double range_high;
	/**
	 * bin size = (range_high-range_low)/bins. This will be automatically
	 * calculated. Is required when using a binned fit to make fit result
	 * independent of the binning.
	 */
	double bin_size;
	/**
	 * Type this dimension which will be used later on for the labeling of
	 * the axis in plots (0 = normal, 1 = difference).
	 */
	unsigned int label;

	lmd_dimension(int bins_, double range_low_, double range_high_) {
		bins = bins_;
		range_low = range_low_;
		range_high = range_high_;
		bin_size = (range_high - range_low) / bins;
		label = 0;
	}
	lmd_dimension() {
		bins = 0;
		range_low = 0.0;
		range_high = 0.0;
		bin_size = 0.0;
		label = 0;
	}
};
}

class PndLmdDataInterface: public TObject {
public:

private:
	/** The number of events filled to the histogram. Events may be outside of specified range (under/overflow)*/
	int num_events;
	/** The lab momentum of the antiproton beam*/
	double p_lab;

	/** Name of the object. This will be automatically generated. */
	TString name;

	/**
	 * List of filepaths that contain the data that was read in. Only used for
	 * bookkeeping/logging so later on it can be identified which data was used
	 * for this data object.
	 */
	set<TString> filepath_list;

protected:
	/** defines the theta dimension (can also be a theta difference) */
	PndLmdFit::lmd_dimension th_dimension;
	/** defines the phi dimension (can also be a phi difference) */
	PndLmdFit::lmd_dimension phi_dimension;
	/** defines the t (4 momentum transfer) dimension */
	PndLmdFit::lmd_dimension t_dimension;

	/** Root file where data is written to. */
	TFile *f;

public:
	PndLmdDataInterface();
	PndLmdDataInterface(double plab);
	PndLmdDataInterface(double plab, PndLmdFit::lmd_dimension th_dimension_,
			PndLmdFit::lmd_dimension phi_dimension_);
	virtual ~PndLmdDataInterface();

	// getter methods
	int getNumEvents() const;
	double getLabMomentum() const;
	TString& getName();
	PndLmdFit::lmd_dimension getTDimension() const;
	PndLmdFit::lmd_dimension getThetaDimension() const;
	PndLmdFit::lmd_dimension getPhiDimension() const;

	void setNumEvents(int num_events_);
	void setLabMomentum(double p_lab_);

	int addFileToList(TString filepath);

	// histogram filling methods
	virtual void fillHistograms(
			std::vector<std::pair<PndLmdFit::lmd_values, PndLmdFit::lmd_values> > &event_data) =0;

ClassDef(PndLmdDataInterface,1)
};

#endif /* PNDLMDDATAINTERFACE_H_ */
