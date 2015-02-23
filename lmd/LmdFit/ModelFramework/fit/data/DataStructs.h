/*
 * DataStructs.h
 *
 *  Created on: Jun 16, 2013
 *      Author: steve
 */

#ifndef DATASTRUCTS_H_
#define DATASTRUCTS_H_

#include <string>

namespace DataStructs {

// binned data structure
struct binned_data_point {
	double bin_center_value[2]; // variables
	double bin_widths[2];
	double z;
	double z_error;
	double scale;
	binned_data_point() :
			z(0.0), z_error(0.0), scale(1.0) {
	}
};

struct unbinned_data_point {
	double x[2]; // variables
};

struct DimensionRange {
	// lower bound on this axis/dimension
	double range_low;
	// upper bound on this axis/dimension
	double range_high;

	bool is_active;

	DimensionRange() :
			range_low(0.0), range_high(0.0), is_active(false) {
	}

	DimensionRange(double low, double high) :
			range_low(low), range_high(high), is_active(true) {
	}

	double getDimensionLength() const {
		return (range_high - range_low);
	}

	double getDimensionMean() const {
		return (range_high + range_low) / 2.0;
	}

	bool isDataWithinRange(double data_value) const {
		if (data_value < range_low) {
			return false;
		}
		if (data_value > range_high) {
			return false;
		}
		return true;
	}

	bool operator<(const DimensionRange &dim_range) const {
		if (range_low < dim_range.range_low)
			return true;
		else if (range_low > dim_range.range_low)
			return false;
		if (range_high < dim_range.range_high)
			return true;
		else if (range_high > dim_range.range_high)
			return false;

		return false;
	}

	bool operator>(const DimensionRange &dim_range) const {
		return (dim_range < *this);
	}

	bool operator==(const DimensionRange &dim_range) const {
		if (range_low != dim_range.range_low)
			return false;
		if (range_high != dim_range.range_high)
			return false;

		return true;
	}

	bool operator!=(const DimensionRange &lmd_dim_range) const {
		return !(*this == lmd_dim_range);
	}
};

}

#endif /* DATASTRUCTS_H_ */
