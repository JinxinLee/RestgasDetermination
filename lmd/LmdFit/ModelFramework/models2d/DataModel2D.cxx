#include "DataModel2D.h"

#include <cmath>
#include <iostream>
#include <cstdlib>

DataModel2D::DataModel2D(std::string name_, interpolation_type type) :
		Model2D(name_), intpol_type(type), data(0) {
}

DataModel2D::DataModel2D(const DataModel2D &data_model_) :
		Model2D(data_model_.getName()), intpol_type(data_model_.intpol_type), data(
				new double[data_model_.cell_count[0] * data_model_.cell_count[1]]) {
	grid_spacing[0] = data_model_.grid_spacing[0];
	grid_spacing[1] = data_model_.grid_spacing[1];

	cell_count[0] = data_model_.cell_count[0];
	cell_count[1] = data_model_.cell_count[1];

	domain_low[0] = data_model_.domain_low[0];
	domain_low[1] = data_model_.domain_low[1];

	domain_high[0] = data_model_.domain_high[0];
	domain_high[1] = data_model_.domain_high[1];

	for (unsigned int i = 0; i < cell_count[0] * cell_count[1]; i++) {
		data[i] = data_model_.data[i];
	}
}

DataModel2D::~DataModel2D() {
	if (data)
		delete[] data;
}

std::pair<double, bool> DataModel2D::getCellSpacing(
		const std::set<double> &values) {
	double dist = -1;
	bool first_dist(true);
	bool first(true);
	bool success(true);
	double last_value;
	std::set<double>::const_iterator it;
	for (it = values.begin(); it != values.end(); it++) {
		if (first) {
			last_value = *it;
			first = false;
		} else {
			if (first_dist) {
				dist = *it - last_value;
				last_value = *it;
				first_dist = false;
			} else {
				if (fabs(1 - (*it - last_value) / dist) < 1e-5 * dist)
					last_value = *it;
				else {
					success = false;
					break;
				}
			}
		}
	}
	return std::make_pair(dist, success);
}

void DataModel2D::setData(
		const std::map<std::pair<double, double>, double> &data_) {
	// delete old data if existent
	if (data)
		delete[] data;

	std::set<double> x_values;
	std::set<double> y_values;

	// check if we have a unified grid and what the dimensions are
	bool first(true);
	std::map<std::pair<double, double>, double>::const_iterator it;
	for (it = data_.begin(); it != data_.end(); it++) {
		if (first) {
			first = false;
			domain_low[0] = it->first.first;
			domain_high[0] = it->first.first;
			domain_low[1] = it->first.second;
			domain_high[1] = it->first.second;
		} else {
			if (domain_low[0] > it->first.first)
				domain_low[0] = it->first.first;
			else if (domain_high[0] < it->first.first)
				domain_high[0] = it->first.first;
			if (domain_low[1] > it->first.second)
				domain_low[1] = it->first.second;
			else if (domain_high[1] < it->first.second)
				domain_high[1] = it->first.second;
		}
		x_values.insert(it->first.first);
		y_values.insert(it->first.second);
	}

	std::pair<double, bool> xspacing = getCellSpacing(x_values);
	std::pair<double, bool> yspacing = getCellSpacing(y_values);

	if (xspacing.second && yspacing.second) {
		std::vector<unsigned int> missing_indices;

		grid_spacing[0] = xspacing.first;
		grid_spacing[1] = yspacing.first;
		cell_count[0] = x_values.size();
		cell_count[1] = y_values.size();

		/*std::cout<<"detected grid spacing:"<<std::endl;
		 std::cout<<grid_spacing[0]<<" x "<<grid_spacing[1]<<std::endl;
		 std::cout<<"detected grid size:"<<std::endl;
		 std::cout<<cell_count[0]<<" x "<<cell_count[1]<<std::endl;*/

		domain_low[0] = domain_low[0] - 0.5 * grid_spacing[0];
		domain_high[0] = domain_high[0] + 0.5 * grid_spacing[0];
		domain_low[1] = domain_low[1] - 0.5 * grid_spacing[1];
		domain_high[1] = domain_high[1] + 0.5 * grid_spacing[1];

		data = new double[cell_count[0] * cell_count[1]];

		unsigned int idx_last(0);
		unsigned int idy_last(0);

		for (it = data_.begin(); it != data_.end(); it++) {
			unsigned int idx = (unsigned int) ((it->first.first - domain_low[0])
					/ grid_spacing[0]);
			unsigned int idy = (unsigned int) ((it->first.second - domain_low[1])
					/ grid_spacing[1]);

			// ok we address the data as idy+ycellcount*idx (so y before x) because the data
			// is presorted with x as a stronger variable, then its easier to find out the missing values

			bool missing(false);
			unsigned int missing_start_x = idx_last;
			unsigned int missing_end_x = idx;

			unsigned int diffx = abs(idx - idx_last) % (cell_count[0] - 1);
			if (diffx > 1) {
				missing = true;
				missing_start_x = idx_last + 1;
				missing_end_x = missing_start_x + diffx;
			}

			unsigned int missing_start_y = idy_last + 1;
			unsigned int missing_end_y = idy;

			unsigned int diffy = abs(idy - idy_last) % (cell_count[1] - 1);
			if (diffy > 1) {
				missing = true;
			}

			if (missing) {
				// translate into running index range
				unsigned int miss_id_low = missing_start_x * cell_count[1]
						+ missing_start_y;
				unsigned int miss_id_high = missing_end_x * cell_count[1]
						+ missing_end_y;
				for (unsigned int miss_id = miss_id_low; miss_id < miss_id_high;
						miss_id++) {
					missing_indices.push_back(miss_id);
				}
			}

			idx_last = idx;
			idy_last = idy;

			data[idx * cell_count[1] + idy] = it->second;
		}

		// now fix the missing values
		std::cout << "found " << missing_indices.size()
				<< " missing evaluation points. Fixing interpolation!" << std::endl;
		for (unsigned int i = 0; i < missing_indices.size(); i++) {
			data[missing_indices[i]] = 0.0;
		}
	}

	std::cout << "initialized interpolation model!" << std::endl;

	setVar1Domain(domain_low[0], domain_high[0]);
	setVar2Domain(domain_low[1], domain_high[1]);
}

void DataModel2D::initModelParameters() {
}

double DataModel2D::eval(const double *x) const {
	if (x[0] < domain_low[0] || x[0] > domain_high[0] || x[1] < domain_low[1]
			|| x[1] > domain_high[1])
		return 0.0;

	unsigned int idx = (unsigned int) ((x[0] - domain_low[0]) / grid_spacing[0]);
	unsigned int idy = (unsigned int) ((x[1] - domain_low[1]) / grid_spacing[1]);

	return data[idx * cell_count[1] + idy];
}

void DataModel2D::updateDomain() {

}

DataModel2D& DataModel2D::operator=(const DataModel2D &data_model_) {
	grid_spacing[0] = data_model_.grid_spacing[0];
	grid_spacing[1] = data_model_.grid_spacing[1];

	cell_count[0] = data_model_.cell_count[0];
	cell_count[1] = data_model_.cell_count[1];

	domain_low[0] = data_model_.domain_low[0];
	domain_low[1] = data_model_.domain_low[1];

	domain_high[0] = data_model_.domain_high[0];
	domain_high[1] = data_model_.domain_high[1];

	for (unsigned int i = 0; i < cell_count[0] * cell_count[1]; i++) {
		data[i] = data_model_.data[i];
	}

	return *this;
}
