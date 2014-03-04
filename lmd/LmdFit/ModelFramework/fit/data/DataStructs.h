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
	binned_data_point() {
		scale = 1.0;
	}
};

struct unbinned_data_point {
	double x[2]; // variables
};

enum dimension_unit_prefix {
	PICO, NANO, MICRO, MILLI, CENTI, NONE, KILO, MEGA, GIGA
};

struct dimension_unit_factor {
	static std::string getUnitNamePrefix(dimension_unit_prefix unit_prefix) {
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

	static double getUnitFactor(dimension_unit_prefix unit_prefix) {
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

struct dimension_range {
	std::pair<double, double> range;
};

}

#endif /* DATASTRUCTS_H_ */
