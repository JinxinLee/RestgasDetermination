/*
 * PndLmdResolution.h
 *
 *  Created on: Aug 25, 2013
 *      Author: steve
 */

#ifndef PNDLMDRESOLUTION_H_
#define PNDLMDRESOLUTION_H_

#include "PndLmdHistogramData.h"

class PndLmdResolution: public PndLmdHistogramData {
private:
	LumiFit::LmdDimension primary_select_dimension;
	LumiFit::LmdDimension secondary_select_dimension;

public:
	PndLmdResolution();
	virtual ~PndLmdResolution();

  const LumiFit::LmdDimension& getPrimarySelectionDimension() const;
	void setPrimarySelectionDimension(
			LumiFit::LmdDimension primary_select_dimension_);
	const LumiFit::LmdDimension& getSecondarySelectionDimension() const;
	void setSecondarySelectionDimension(
			LumiFit::LmdDimension secondary_select_dimension_);

	bool operator<(const PndLmdResolution &lmd_res) const;
	bool operator>(const PndLmdResolution &lmd_res) const;
	bool operator==(const PndLmdResolution &lmd_res) const;
	bool operator!=(const PndLmdResolution &lmd_res) const;

ClassDef(PndLmdResolution,1)
};

#endif /* PNDLMDRESOLUTION_H_ */
