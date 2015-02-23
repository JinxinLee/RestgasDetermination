#include "LumiModelOptions.h"
#include "data/PndLmdAcceptance.h"

ClassImp(LumiFit::PndLmdFitModelOptions)

namespace LumiFit {

PndLmdFitModelOptions::PndLmdFitModelOptions() :
		momentum_transfer_active(false), use_theta_xy_coordinate_system(true), divergence_smearing_active(false), fix_beam_tilts(
				false), acceptance_correction_active(false), automatic_acceptance_boundary_finding_active(
				true), resolution_smearing_active(false), acceptance_bound_low(0.0), acceptance_bound_high(
				0.0), fit_dimension(1), acceptance(0), resolution_parametrization_file_url(
				""), smearing_model(GAUSSIAN), acceptance_interpolation(SPLINE), dpm_elastic_parts(
				ALL), with_background_model(false), vertex_model(GAUSSIAN) {
}

PndLmdFitModelOptions::PndLmdFitModelOptions(LmdTrackType track_type,
		LmdDimensionType dimension_type, bool divergence_smearing_active_) {
	divergence_smearing_active = divergence_smearing_active_;

	if (dimension_type == T) {
		momentum_transfer_active = true;
	} else {
		momentum_transfer_active = false;
	}
	if (track_type == MC) {
		acceptance_correction_active = false;
		resolution_smearing_active = false;
	} else if (track_type == MC_ACC) {
		acceptance_correction_active = true;
		resolution_smearing_active = false;
	} else if (track_type == RECO) {
		acceptance_correction_active = true;
		resolution_smearing_active = true;
	}
}

bool PndLmdFitModelOptions::lessThanBinaryOptions(
		const PndLmdFitModelOptions &rhs) const {
	if (momentum_transfer_active < rhs.momentum_transfer_active)
		return true;
	else if (momentum_transfer_active > rhs.momentum_transfer_active)
		return false;
	if (divergence_smearing_active < rhs.divergence_smearing_active)
		return true;
	else if (divergence_smearing_active > rhs.divergence_smearing_active)
		return false;
	if (acceptance_correction_active < rhs.acceptance_correction_active)
		return true;
	else if (acceptance_correction_active > rhs.acceptance_correction_active)
		return false;
	if (resolution_smearing_active < rhs.resolution_smearing_active)
		return true;
	else if (resolution_smearing_active > rhs.resolution_smearing_active)
		return false;

	return false;
}

bool PndLmdFitModelOptions::equalBinaryOptions(
		const PndLmdFitModelOptions &rhs) const {
	return (lessThanBinaryOptions(rhs) == rhs.lessThanBinaryOptions(*this));
}

bool PndLmdFitModelOptions::operator<(const PndLmdFitModelOptions &rhs) const {
	if (lessThanBinaryOptions(rhs) == true)
		return true;
	else if (rhs.lessThanBinaryOptions(*this) == true)
		return false;

	if (acceptance_correction_active) {
		if (acceptance != 0 && rhs.acceptance != 0) {
			const PndLmdAcceptance &acc1 = *acceptance;
			const PndLmdAcceptance &acc2 = *rhs.acceptance;
			if (acc1.operator <(acc2))
				return true;
			else if (acc1 > acc2)
				return false;
		}
	}

	if (use_theta_xy_coordinate_system < rhs.use_theta_xy_coordinate_system)
		return true;
	else if (use_theta_xy_coordinate_system > rhs.use_theta_xy_coordinate_system)
		return false;
	if (smearing_model < rhs.smearing_model)
		return true;
	else if (smearing_model > rhs.smearing_model)
		return false;
	if (acceptance_interpolation < rhs.acceptance_interpolation)
		return true;
	else if (acceptance_interpolation > rhs.acceptance_interpolation)
		return false;
	if (automatic_acceptance_boundary_finding_active
			< rhs.automatic_acceptance_boundary_finding_active)
		return true;
	else if (automatic_acceptance_boundary_finding_active
			> rhs.automatic_acceptance_boundary_finding_active)
		return false;
	if (dpm_elastic_parts < rhs.dpm_elastic_parts)
		return true;
	else if (dpm_elastic_parts > rhs.dpm_elastic_parts)
		return false;
	if (use_resolution_parameter_interpolation
			< rhs.use_resolution_parameter_interpolation)
		return true;
	else if (use_resolution_parameter_interpolation
			> rhs.use_resolution_parameter_interpolation)
		return false;
	if (vertex_model < rhs.vertex_model)
		return true;
	else if (vertex_model > rhs.vertex_model)
		return false;
	if (with_background_model < rhs.with_background_model)
		return true;
	else if (with_background_model > rhs.with_background_model)
		return false;

	ModelStructs::string_comp strcomp;
	return strcomp(resolution_parametrization_file_url,
			rhs.resolution_parametrization_file_url);
}

bool PndLmdFitModelOptions::operator>(const PndLmdFitModelOptions &rhs) const {
	return (rhs < *this);
}

bool PndLmdFitModelOptions::operator==(const PndLmdFitModelOptions &rhs) const {
	return ((*this < rhs) == (*this > rhs));
}

bool PndLmdFitModelOptions::operator!=(const PndLmdFitModelOptions &rhs) const {
	return !(*this == rhs);
}

std::ostream & operator<<(std::ostream & os,
		const PndLmdFitModelOptions & model_opt) {
	if (model_opt.momentum_transfer_active)
		os << "t";
	else
		os << "th";

	if (model_opt.acceptance_correction_active)
		os << "-acc_cor";
	if (model_opt.resolution_smearing_active)
		os << "-res_smear";

	return os;
}

}

