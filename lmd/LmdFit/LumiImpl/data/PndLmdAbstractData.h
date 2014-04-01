/*
 * PndLmdAbstractData.h
 *
 *  Created on: Apr 17, 2013
 *      Author: steve
 */

#ifndef PNDLMDABSTRACTDATA_H_
#define PNDLMDABSTRACTDATA_H_

#include "../LumiFitStructs.h"

#include <set>

#include "TString.h"
#include "TObject.h"

using std::set;


class PndLmdAbstractData: public TObject {
private:
	/**
	 * The number of events requested to be filled to the histogram. Events may
	 * be outside of specified range (under/overflow). This is mainly used only
	 * for the histogram filling part (to know when to stop).
	 */
	int num_events;

	/** The lab momentum of the antiproton beam */
	double p_lab;

	/**
	 * List of filepaths that contain the data that was read in. Only used for
	 * bookkeeping/logging so later on it can be identified which data was used
	 * for this data object.
	 */
	set<TString> filepath_list;

protected:
	/** Name of the object used for saving the object in a root file */
	TString name;

	// we only allow 1d or 2d data objects here
	/** defines the primary dimension */
	LumiFit::LmdDimension primary_dimension;
	/** defines the secondary dimension (optional) */
	LumiFit::LmdDimension secondary_dimension;

	virtual void init1DData() =0;
	virtual void init2DData() =0;

public:
	PndLmdAbstractData();
	virtual ~PndLmdAbstractData();

	// getter methods
	int getNumEvents() const;
	double getLabMomentum() const;
	const TString& getName() const;

	const LumiFit::LmdDimension& getPrimaryDimension() const;
	const LumiFit::LmdDimension& getSecondaryDimension() const;

	double getBinningFactor() const;

	// setter methods
	void setNumEvents(int num_events_);
	void setLabMomentum(double p_lab_);
	void setName(TString name_);

	void setPrimaryDimension(LumiFit::LmdDimension primary_dimension_);
	void setSecondaryDimension(LumiFit::LmdDimension secondary_dimension_);

	virtual void saveToRootFile();
	int addFileToList(TString filepath);

	virtual void cloneData(const PndLmdAbstractData &lmd_abs_data) =0;
	virtual void add(const PndLmdAbstractData &lmd_abs_data_addition) =0;

	virtual bool operator<(const PndLmdAbstractData &lmd_data_int) const;
	virtual bool operator>(const PndLmdAbstractData &lmd_data_int) const;
	virtual bool operator==(const PndLmdAbstractData &lmd_data_int) const;
	virtual bool operator!=(const PndLmdAbstractData &lmd_data_int) const;

ClassDef(PndLmdAbstractData,1)
};

#endif /* PNDLMDABSTRACTDATA_H_ */
