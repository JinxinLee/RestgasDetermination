/*
 * PndLmdComparisonStructs.h
 *
 *  Created on: Jul 14, 2014
 *      Author: steve
 */

#ifndef PNDLMDCOMPARISONSTRUCTS_H_
#define PNDLMDCOMPARISONSTRUCTS_H_

#include "LumiFitStructs.h"
#include "fit/PndLmdLumiFitOptions.h"
#include "data/PndLmdAbstractData.h"

namespace LumiFit {

namespace Comparisons {

class AbstractLmdDataFilter {
public:
	virtual ~AbstractLmdDataFilter() {
	}
	virtual bool check(PndLmdAbstractData &lmd_abs_data) =0;
};

struct fit_options_compare {
	bool operator()(const PndLmdLumiFitOptions& lhs,
			const PndLmdLumiFitOptions& rhs) const {
		LumiFit::PndLmdFitModelOptions fit_model_opt_lhs = lhs.getFitModelOptions();
		fit_model_opt_lhs.acceptance_correction_active = false;
		fit_model_opt_lhs.momentum_transfer_active = false;
		fit_model_opt_lhs.resolution_smearing_active = false;
		LumiFit::PndLmdFitModelOptions fit_model_opt_rhs = rhs.getFitModelOptions();
		fit_model_opt_rhs.acceptance_correction_active = false;
		fit_model_opt_rhs.momentum_transfer_active = false;
		fit_model_opt_rhs.resolution_smearing_active = false;

		if (fit_model_opt_lhs < fit_model_opt_rhs)
			return true;
		else if (fit_model_opt_lhs > fit_model_opt_rhs) {
			return false;
		}
		return (lhs.getEstimatorOptions() < rhs.getEstimatorOptions());
	}
};

struct data_primary_dimension_options_filter: public AbstractLmdDataFilter {
	LumiFit::LmdDimensionOptions lmd_dim_opt;
public:
	data_primary_dimension_options_filter(
			LumiFit::LmdDimensionOptions lmd_dim_opt_) :
			lmd_dim_opt(lmd_dim_opt_) {
	}
	bool check(PndLmdAbstractData &lmd_abs_data) {
		return (lmd_abs_data.getPrimaryDimension().dimension_options == lmd_dim_opt);
	}
	void setDimensionOptions(LumiFit::LmdDimensionOptions &lmd_dim_opt_) {
		lmd_dim_opt = lmd_dim_opt_;
	}
};

struct data_primary_dimension_track_type_filter: public AbstractLmdDataFilter {
	LumiFit::LmdTrackType lmd_track_type;
public:
	data_primary_dimension_track_type_filter(
			LumiFit::LmdTrackType lmd_track_type_) :
			lmd_track_type(lmd_track_type_) {
	}
	bool check(PndLmdAbstractData &lmd_abs_data) {
		return (lmd_abs_data.getPrimaryDimension().dimension_options.track_type
				== lmd_track_type);
	}
	void setDimensionTrackType(LumiFit::LmdTrackType &lmd_track_type_) {
		lmd_track_type = lmd_track_type_;
	}
};

}
}

#endif /* PNDLMDCOMPARISONSTRUCTS_H_ */
