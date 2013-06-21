/*
 * DataStructs.h
 *
 *  Created on: Jun 16, 2013
 *      Author: steve
 */

#ifndef DATASTRUCTS_H_
#define DATASTRUCTS_H_

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

}


#endif /* DATASTRUCTS_H_ */
