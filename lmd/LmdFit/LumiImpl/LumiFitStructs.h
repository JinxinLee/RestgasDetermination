/*
 * LumiFitStructs.h
 *
 *  Created on: Jul 9, 2013
 *      Author: steve
 */

#ifndef LUMIFITSTRUCTS_H_
#define LUMIFITSTRUCTS_H_

#include "ModelFramework/fit/data/DataStructs.h"
#include "ModelFramework/core/ModelStructs.h"
#include "ModelFramework/fit/EstimatorOptions.h"

#include <iostream>
#include <string>
#include <sstream>

#include "TObject.h"
#include "TString.h"

class PndLmdAcceptance;

namespace LumiFit {

enum LmdEstimatorType {
	CHI2, LOG_LIKELIHOOD
};

enum LmdDataType {
	HISTOGRAM, EFFICIENCY
};

enum LmdDimensionType {
	X, Y, Z, T, THETA, PHI
};

enum LmdTrackType {
	MC, MC_ACC, RECO
};

enum LmdTrackParamType {
	IP, LMD
};

enum ModelType {
	GAUSSIAN, DOUBLE_GAUSSIAN, ASYMMETRIC_GAUSSIAN, UNIFORM
};

enum InterpolationType {
	CONSTANT, LINEAR, SPLINE
};

enum DPMElasticParts {
	COUL, INT, HAD, HAD_RHO_B_SIGTOT, ALL_RHO_B_SIGTOT, ALL
};

enum LmdDimensionUnitPrefix {
	PICO, NANO, MICRO, MILLI, CENTI, NONE, KILO, MEGA, GIGA
};

struct LmdDimensionOptions: public TObject {
	LmdDimensionType dimension_type;

	LmdTrackType track_type;

	LmdTrackParamType track_param_type;

	LmdDimensionOptions() {
		dimension_type = THETA;
		track_param_type = IP;
		track_type = RECO;
	}

	bool operator<(const LmdDimensionOptions &rhs) const {
		// check binary options first
		if (dimension_type < rhs.dimension_type)
			return true;
		else if (dimension_type > rhs.dimension_type)
			return false;
		if (track_type < rhs.track_type)
			return true;
		else if (track_type > rhs.track_type)
			return false;
		if (track_param_type < rhs.track_param_type)
			return true;
		else if (track_param_type > rhs.track_param_type)
			return false;

		return false;
	}

	bool operator>(const LmdDimensionOptions &rhs) const {
		return (rhs < *this);
	}

	/**
	 * This operator contains redundancy since it can be defined
	 * also from < (and >), but for reasons of speed this separate
	 * implementation exists.
	 */
	bool operator==(const LmdDimensionOptions &rhs) const {
		// check binary options first
		if (dimension_type != rhs.dimension_type)
			return false;
		else if (track_type != rhs.track_type)
			return false;
		else if (track_param_type != rhs.track_param_type)
			return false;

		return true;
	}

	bool operator!=(const LmdDimensionOptions &rhs) const {
		// check binary options first
		return !(*this == rhs);
	}

ClassDef(LmdDimensionOptions, 1)
	;
};

struct LmdDimensionUnitFactor {
	static std::string getUnitNamePrefix(LmdDimensionUnitPrefix unit_prefix) {
		if (PICO == unit_prefix)
			return "p";
		else if (NANO == unit_prefix)
			return "n";
		else if (MICRO == unit_prefix)
			return "#mu";
		else if (MILLI == unit_prefix)
			return "m";
		else if (CENTI == unit_prefix)
			return "cm";
		else if (NONE == unit_prefix)
			return "";
		else if (KILO == unit_prefix)
			return "k";
		else if (MEGA == unit_prefix)
			return "M";
		else if (GIGA == unit_prefix)
			return "G";
		return "";
	}

	static double getUnitFactor(LmdDimensionUnitPrefix unit_prefix) {
		if (PICO == unit_prefix)
			return 1e-12;
		else if (NANO == unit_prefix)
			return 1e-9;
		else if (MICRO == unit_prefix)
			return 1e-6;
		else if (MILLI == unit_prefix)
			return 1e-3;
		else if (CENTI == unit_prefix)
			return 1e-2;
		else if (NONE == unit_prefix)
			return 1.0;
		else if (KILO == unit_prefix)
			return 1e3;
		else if (MEGA == unit_prefix)
			return 1e6;
		else if (GIGA == unit_prefix)
			return 1e9;
		return 1.0;
	}
};

class LmdDimensionRange: public TObject {
private:
	// lower bound on this axis/dimension
	double range_low;
	// upper bound on this axis/dimension
	double range_high;

	LmdDimensionUnitPrefix unit_prefix;

public:
	LmdDimensionRange() :
			range_low(0.0), range_high(0.0), unit_prefix(NONE) {
	}

	double getDimensionLength() const {
		return (getRangeHigh() - getRangeLow());
	}

	double getDimensionMean() const {
		return (getRangeHigh() + getRangeLow()) / 2.0;
	}

	double getUnitFactor() const {
		return LmdDimensionUnitFactor::getUnitFactor(unit_prefix);
	}

	double getRangeLow() const {
		return range_low * getUnitFactor();
	}

	double getRangeHigh() const {
		return range_high * getUnitFactor();
	}

	void setRangeLow(double range_low_) {
		range_low = range_low_;
	}

	void setRangeHigh(double range_high_) {
		range_high = range_high_;
	}

	void setUnitPrefix(LmdDimensionUnitPrefix unit_prefix_) {
		unit_prefix = unit_prefix_;
	}

	bool isDataWithinRange(double data_value) const {
		if (data_value < getRangeLow()) {
			return false;
		}
		if (data_value > getRangeHigh()) {
			return false;
		}
		return true;
	}

	bool operator<(const LmdDimensionRange &lmd_dim_range) const {
		if (range_low < lmd_dim_range.range_low)
			return true;
		else if (range_low > lmd_dim_range.range_low)
			return false;
		if (range_high < lmd_dim_range.range_high)
			return true;
		else if (range_high > lmd_dim_range.range_high)
			return false;

		return false;
	}

	bool operator>(const LmdDimensionRange &lmd_dim_range) const {
		return (lmd_dim_range < *this);
	}

	bool operator==(const LmdDimensionRange &lmd_dim_range) const {
		if (range_low != lmd_dim_range.range_low)
			return false;
		if (range_high != lmd_dim_range.range_high)
			return false;
		if (unit_prefix != lmd_dim_range.unit_prefix)
			return false;

		return true;
	}

	bool operator!=(const LmdDimensionRange &lmd_dim_range) const {
		return !(*this == lmd_dim_range);
	}

ClassDef(LmdDimensionRange, 1)
	;
};

/**
 * Struct which defines a dimension of data.
 */
struct LmdDimension: public TObject {
	/**
	 * flag stating whether this dimension should be used or not.
	 * In its default initialization state it is set to false,
	 * either the user should set this flag manually to true or via
	 * #calculateBinSize() if this dimension should be used.
	 */
	bool is_active;
	/** the number of bins on this axis/dimension */
	int bins;

	/**
	 * bin size = (range_high-range_low)/bins. This will be automatically
	 * calculated. Is required when using a binned fit to make fit result
	 * independent of the binning.
	 */
	double bin_size;

	LmdDimensionOptions dimension_options;

	LmdDimensionRange dimension_range;

	/**
	 * Label of this dimension which will be printed on the corresponding axis as
	 * labels. Default value will be automatically generated from the
	 * #LmdDimensionType and #LmdTrackParamType.
	 */
	TString label;

	LmdDimension() {
		is_active = false;
		bins = 0;
		bin_size = 1.0;
		label = "";
	}

	void calculateBinSize() {
		is_active = true;
		bin_size = (dimension_range.getDimensionLength()) / bins;
	}

	LmdDimension clone() const {
		LmdDimension clone_object;
		clone_object.bins = bins;
		clone_object.dimension_range = dimension_range;
		clone_object.dimension_options = dimension_options;
		clone_object.calculateBinSize();
		return clone_object;
	}

	bool operator<(const LmdDimension &lmd_dim) const {
		if (bins < lmd_dim.bins)
			return true;
		else if (bins > lmd_dim.bins)
			return false;
		if (dimension_range < lmd_dim.dimension_range)
			return true;
		else if (dimension_range > lmd_dim.dimension_range)
			return false;
		if (dimension_options < lmd_dim.dimension_options)
			return true;

		return false;
	}

	bool operator>(const LmdDimension &lmd_dim) const {
		return (lmd_dim < *this);
	}

	bool operator==(const LmdDimension &lmd_dim) const {
		if (bins != lmd_dim.bins)
			return false;
		if (dimension_range != lmd_dim.dimension_range)
			return false;
		if (dimension_options != lmd_dim.dimension_options)
			return false;

		return true;
	}

	bool operator!=(const LmdDimension &lmd_dim) const {
		return !(*this == lmd_dim);
	}

ClassDef(LmdDimension, 1)
	;
};

struct PndLmdFitModelOptions: public TObject {
	// fields
	int fit_dimension;

	DPMElasticParts dpm_elastic_parts;
	bool momentum_transfer_active; // if this is enabled everything else cannot be used

	bool acceptance_correction_active;
	PndLmdAcceptance *acceptance;
	InterpolationType acceptance_interpolation;

	bool resolution_smearing_active;
	ModelType smearing_model;
	bool use_resolution_parameter_interpolation;
	std::string resolution_parametrization_file_url;

	ModelType vertex_model;

	/**
	 * Empty Constructor
	 */
	PndLmdFitModelOptions() :
			momentum_transfer_active(false), acceptance_correction_active(false), resolution_smearing_active(
					false), fit_dimension(1), acceptance(0), resolution_parametrization_file_url(
					""), smearing_model(GAUSSIAN), acceptance_interpolation(SPLINE), dpm_elastic_parts(
					ALL), vertex_model(GAUSSIAN) {
	}

	PndLmdFitModelOptions(LmdTrackType track_type,
			LmdDimensionType dimension_type) {
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

	bool lessThanBinaryOptions(const PndLmdFitModelOptions &rhs) const {
		if (momentum_transfer_active < rhs.momentum_transfer_active)
			return true;
		else if (momentum_transfer_active > rhs.momentum_transfer_active)
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

	bool equalBinaryOptions(const PndLmdFitModelOptions &rhs) const {
		return (lessThanBinaryOptions(rhs) == rhs.lessThanBinaryOptions(*this));
	}

	bool operator<(const PndLmdFitModelOptions &rhs) const {
		if (lessThanBinaryOptions(rhs) == true)
			return true;
		else if (rhs.lessThanBinaryOptions(*this) == true)
			return false;

		if (smearing_model < rhs.smearing_model)
			return true;
		else if (smearing_model > rhs.smearing_model)
			return false;
		if (acceptance_interpolation < rhs.acceptance_interpolation)
			return true;
		else if (acceptance_interpolation > rhs.acceptance_interpolation)
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

		ModelStructs::string_comp strcomp;
		return strcomp(resolution_parametrization_file_url,
				rhs.resolution_parametrization_file_url);
	}

	bool operator>(const PndLmdFitModelOptions &rhs) const {
		return (rhs < *this);
	}

	bool operator==(const PndLmdFitModelOptions &rhs) const {
		return ((*this < rhs) == (*this > rhs));
	}

	bool operator!=(const PndLmdFitModelOptions &rhs) const {
		return !(*this == rhs);
	}

	friend std::ostream & operator <<(std::ostream & os,
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

ClassDef(PndLmdFitModelOptions ,1)
	;
};

struct LmdSimIPParameters: public TObject {
	double offset_x_mean;
	double offset_x_width;
	double offset_y_mean;
	double offset_y_width;
	double offset_z_mean;
	double offset_z_width;

	double tilt_x_mean;
	double tilt_x_width;
	double tilt_y_mean;
	double tilt_y_width;

	void print() {
		std::cout << "simulation beam properties are:" << std::endl;
		std::cout << "mean X pos: \t" << offset_x_mean << std::endl;
		std::cout << "width X pos: \t" << offset_x_width << std::endl;
		std::cout << "mean Y pos: \t" << offset_y_mean << std::endl;
		std::cout << "width Y pos: \t" << offset_y_width << std::endl;
		std::cout << "mean Z pos: \t" << offset_z_mean << std::endl;
		std::cout << "width Z pos: \t" << offset_z_width << std::endl;
		std::cout << "--------------------------------------------------"
				<< std::endl;
		std::cout << "mean X tilt: \t" << tilt_x_mean << std::endl;
		std::cout << "width X tilt: \t" << tilt_x_width << std::endl;
		std::cout << "mean Y tilt: \t" << tilt_y_mean << std::endl;
		std::cout << "width Y tilt: \t" << tilt_y_width << std::endl;
	}

	LmdSimIPParameters() {
		reset();
	}

	void reset() {
		offset_x_mean = 0.0;
		offset_x_width = 0.0;
		offset_y_mean = 0.0;
		offset_y_width = 0.0;
		offset_z_mean = 0.0;
		offset_z_width = 0.0;

		tilt_x_mean = 0.0;
		tilt_x_width = 0.0;
		tilt_y_mean = 0.0;
		tilt_y_width = 0.0;
	}

	std::string getLabel() const {
		std::stringstream ss;
		ss << "IP_pos_" << offset_x_mean << "_" << offset_x_width << "_"
				<< offset_y_mean << "_" << offset_y_width << "_" << offset_z_mean << "_"
				<< offset_z_width << "-beam_tilt_" << tilt_x_mean << "_" << tilt_x_width
				<< "_" << tilt_y_mean << "_" << tilt_y_width;
		return ss.str();
	}

	std::string getDependencyLabel() const {
		if (1.0 == offset_x_mean)
			return std::string("offset_x");
		if (1.0 == offset_x_width)
			return std::string("width_x");
		if (1.0 == offset_y_mean)
			return std::string("offset_y");
		if (1.0 == offset_y_width)
			return std::string("width_y");
		if (1.0 == offset_z_mean)
			return std::string("offset_z");
		if (1.0 == offset_z_width)
			return std::string("width_z");

		if (1.0 == tilt_x_mean)
			return std::string("tilt_x");
		if (1.0 == tilt_x_width)
			return std::string("div_x");
		if (1.0 == tilt_y_mean)
			return std::string("tilt_y");
		if (1.0 == tilt_y_width)
			return std::string("div_y");

		return std::string("");
	}

	double getDependencyValue(const LmdSimIPParameters &param_template) const {
		if (1.0 == param_template.offset_x_mean)
			return offset_x_mean;
		if (1.0 == param_template.offset_x_width)
			return offset_x_width;
		if (1.0 == param_template.offset_y_mean)
			return offset_y_mean;
		if (1.0 == param_template.offset_y_width)
			return offset_y_width;
		if (1.0 == param_template.offset_z_mean)
			return offset_z_mean;
		if (1.0 == param_template.offset_z_width)
			return offset_z_width;

		if (1.0 == param_template.tilt_x_mean)
			return tilt_x_mean;
		if (1.0 == param_template.tilt_x_width)
			return tilt_x_width;
		if (1.0 == param_template.tilt_y_mean)
			return tilt_y_mean;
		if (1.0 == param_template.tilt_y_width)
			return tilt_y_width;

		return 0.0;
	}

	std::string getDependencyName(
			const LmdSimIPParameters &param_template) const {
		std::string return_str("");
		if (1.0 == param_template.offset_x_mean)
			return_str = "#mu_{x}";
		else if (1.0 == param_template.offset_x_width)
			return_str = "#sigma_{x}";
		else if (1.0 == param_template.offset_y_mean)
			return_str = "#mu_{y}";
		else if (1.0 == param_template.offset_y_width)
			return_str = "#sigma_{y}";
		else if (1.0 == param_template.offset_z_mean)
			return_str = "#mu_{z}";
		else if (1.0 == param_template.offset_z_width)
			return_str = "#sigma_{z}";

		else if (1.0 == param_template.tilt_x_mean)
			return_str = "#mu_{x}";
		else if (1.0 == param_template.tilt_x_width)
			return_str = "#sigma_{x}";
		else if (1.0 == param_template.tilt_y_mean)
			return_str = "#mu_{x}";
		else if (1.0 == param_template.tilt_y_width)
			return_str = "#sigma_{x}";

		return return_str.append(" [cm]");
	}

	bool operator<(const LmdSimIPParameters &rhs) const {
		if (offset_x_mean < rhs.offset_x_mean)
			return true;
		else if (offset_x_mean > rhs.offset_x_mean)
			return false;
		if (offset_x_width < rhs.offset_x_width)
			return true;
		else if (offset_x_width > rhs.offset_x_width)
			return false;
		if (offset_y_mean < rhs.offset_y_mean)
			return true;
		else if (offset_y_mean > rhs.offset_y_mean)
			return false;
		if (offset_y_width < rhs.offset_y_width)
			return true;
		else if (offset_y_width > rhs.offset_y_width)
			return false;
		if (offset_z_mean < rhs.offset_z_mean)
			return true;
		else if (offset_z_mean > rhs.offset_z_mean)
			return false;
		if (offset_z_width < rhs.offset_z_width)
			return true;
		else if (offset_z_width > rhs.offset_z_width)
			return false;

		if (tilt_x_mean < rhs.tilt_x_mean)
			return true;
		else if (tilt_x_mean > rhs.tilt_x_mean)
			return false;
		if (tilt_x_width < rhs.tilt_x_width)
			return true;
		else if (tilt_x_width > rhs.tilt_x_width)
			return false;
		if (tilt_y_mean < rhs.tilt_y_mean)
			return true;
		else if (tilt_y_mean > rhs.tilt_y_mean)
			return false;
		if (tilt_y_width < rhs.tilt_y_width)
			return true;
		else if (tilt_y_width > rhs.tilt_y_width)
			return false;

		return false;
	}

	bool operator>(const LmdSimIPParameters &rhs) const {
		return (rhs < *this);
	}

ClassDef(LmdSimIPParameters ,1)
	;
};

}

#endif /* LUMIFITSTRUCTS_H_ */
