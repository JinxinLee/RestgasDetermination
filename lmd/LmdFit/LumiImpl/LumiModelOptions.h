#ifndef LUMIMODELOPTIONS_H_
#define LUMIMODELOPTIONS_H_

#include "LumiFitStructs.h"

class PndLmdAcceptance;
class PndLmdHistogramData;

namespace LumiFit {

struct PndLmdFitModelOptions: public TObject {
	// fields
	int fit_dimension;

	DPMElasticParts dpm_elastic_parts;
	bool momentum_transfer_active; // if this is enabled everything else cannot be used

	bool divergence_smearing_active;
	LumiFit::LmdDimension data_primary_dimension;
	LumiFit::LmdDimension data_secondary_dimension;

	bool fix_beam_tilts;
	double beam_tilt_x;
	double beam_tilt_y;

	bool acceptance_correction_active;
	const PndLmdAcceptance *acceptance;
	InterpolationType acceptance_interpolation;

	bool resolution_smearing_active;
	ModelType smearing_model;
	std::set<LmdDimension> selections;
	bool use_resolution_parameter_interpolation;
	std::string resolution_parametrization_file_url;
	std::string elastic_reco_data_file_url;

	ModelType vertex_model;

	/**
	 * Empty Constructor
	 */
	PndLmdFitModelOptions();

	PndLmdFitModelOptions(LmdTrackType track_type,
			LmdDimensionType dimension_type, bool divergence_smearing_active);

	bool lessThanBinaryOptions(const PndLmdFitModelOptions &rhs) const;

	bool equalBinaryOptions(const PndLmdFitModelOptions &rhs) const;

	bool operator<(const PndLmdFitModelOptions &rhs) const;

	bool operator>(const PndLmdFitModelOptions &rhs) const;

	bool operator==(const PndLmdFitModelOptions &rhs) const;

	bool operator!=(const PndLmdFitModelOptions &rhs) const;

	friend std::ostream & operator <<(std::ostream & os,
			const PndLmdFitModelOptions & model_opt);

ClassDef(PndLmdFitModelOptions ,4)
	;
};

}

#endif /* LUMIMODELOPTIONS_H_ */
