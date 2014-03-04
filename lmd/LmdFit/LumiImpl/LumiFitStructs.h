/*
 * LumiFitStructs.h
 *
 *  Created on: Jul 9, 2013
 *      Author: steve
 */

#ifndef LUMIFITSTRUCTS_H_
#define LUMIFITSTRUCTS_H_

#include "../ModelFramework/fit/data/DataStructs.h"

#include "TObject.h"
#include "TString.h"

#include <iostream>

namespace LumiFit {

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

class LmdDimensionRange: public TObject {
	private:
		// lower bound on this axis/dimension
		double range_low;
		// upper bound on this axis/dimension
		double range_high;

		DataStructs::dimension_unit_prefix unit_prefix;

	public:
		LmdDimensionRange() :
				range_low(0.0), range_high(0.0) {
		}

		double getDimensionLength() const {
			return (getRangeHigh() - getRangeLow());
		}

		double getDimensionMean() const {
			return (getRangeHigh() + getRangeLow()) / 2.0;
		}

		double getUnitFactor() const {
			return DataStructs::dimension_unit_factor::getUnitFactor(unit_prefix);
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

		void setUnitPrefix(DataStructs::dimension_unit_prefix unit_prefix_) {
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

class LmdBinaryFitOptions: public TObject {
	private:
		/** This set of bits specifies all binary fit options will be used (so on-off type options)
		 * bit 0: 0 = no resolution smearing, 1 = with resolution smearing
		 * bit 1: 0 = no acceptance corr, 1 = with acceptance corr
		 * bit 2: 0 = 1d fit, 1 = 2d fit
		 * bit 3: 0 = use theta-phi data and fit function, 1 = use t (momentum transfer) data and fit function
		 * bit 4: 0 = ROOT, 1 = ROOFIT
		 **/
		unsigned long binary_options;

	public:
		/**
		 * Empty Constructor
		 */
		LmdBinaryFitOptions() :
				binary_options(0) {
		}
		/**
		 * Constructor
		 * @param bit_flag_options is a integer number, which is represented a set of binary flags:
		 *  bit 0: resolution smearing
		 *  bit 1: acceptance correction
		 *  bit 2: fit dimension
		 *  bit 3: use t instead of theta
		 *  bit 4: fitter type
		 *  The highest number can therefore be 31 and the lowest 0 (higher numbers than 31 are regarded as 31).
		 */
		LmdBinaryFitOptions(unsigned long bit_flag_options) :
				binary_options(bit_flag_options) {
		}

		unsigned long getBinaryOptions() const {
			return binary_options;
		}
		/**
		 * Get method for the fit dimension (1D or 2D)
		 * @returns fit dimension (0 = 1d fit, 1 = 2d fit)
		 */
		bool getFitDimension() const {
			return binary_options & (1 << (2));
		}
		/**
		 * Get method for the fitter type (ROOT or ROOFIT)
		 * @returns fitter type (0 = ROOT, 1 = ROOFIT)
		 */
		bool getFitterType() const {
			return binary_options & (1 << (4));
		}
		/**
		 * Checks if resolution smearing is on.
		 * @returns true if smearing is on, false if off
		 */
		bool isSmearingOn() const {
			return binary_options & 1;
		}
		/**
		 * Checks if acceptance correction is on
		 * @returns true if acceptance correction will be used, false otherwise
		 */
		bool isAcceptanceCorrOn() const {
			return binary_options & (1 << (1));
		}
		/**
		 * Checks if raw fit mode will be used
		 * @returns true if t spectrum and function will be used instead of theta, false otherwise
		 */
		bool isFitRaw() const {
			return binary_options & (1 << (3));
		}

		/**
		 * Setter method for switching between theta and t
		 * @param use_raw_ specifies if raw (momentum transfer) should be used
		 */
		void setFitAsRaw(bool use_raw_) {
			if (use_raw_)
				binary_options |= 1 << 3;
			else
				binary_options &= ~(1 << 3);
		}
		/**
		 * Setter method for the fit dimension (1D or 2D)
		 * @param fit_dimension_ is the new fit dimension that will be used
		 */
		void setFitDimension(bool fit_dimension_) {
			if (fit_dimension_)
				binary_options |= 1 << 2;
			else
				binary_options &= ~(1 << 2);
		}
		/**
		 * Setter method for the fitter type (ROOT or ROOFIT)
		 * @param fitter_type_ is the new fit type that will be used
		 */
		void setFitterType(bool fitter_type_) {
			if (fitter_type_)
				binary_options |= 1 << 4;
			else
				binary_options &= ~(1 << 4);
		}
		/**
		 * Setter method for the smearing mode (0 disabled, 1 enabled)
		 * @param with_smearing_ is the new fit dimension that will be used
		 */
		void setSmearingMode(bool with_smearing_) {
			if (with_smearing_)
				binary_options |= 1;
			else
				binary_options &= ~1;
		}
		/**
		 * Setter method for the acceptance correction mode (0 disabled, 1 enabled)
		 * @param with_acceptance_corr_ is the new fit dimension that will be used
		 */
		void setAcceptanceCorrMode(bool with_acceptance_corr_) {
			if (with_acceptance_corr_)
				binary_options |= 1 << 1;
			else
				binary_options &= ~(1 << 1);
		}

	ClassDef(LmdBinaryFitOptions ,1)
		;
};

struct LmdSimIPParameters {
		bool parameters_exist;

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
			parameters_exist = false;
		}
};

}

#endif /* LUMIFITSTRUCTS_H_ */
