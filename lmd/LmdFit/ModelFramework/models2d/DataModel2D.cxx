#include "DataModel2D.h"

#include <cmath>
#include <iostream>
#include <cstdlib>

DataModel2D::DataModel2D(std::string name_,
		ModelStructs::InterpolationType type) :
		Model2D(name_), data(0), grid_density(1.0), intpol_type(type) {
	setIntpolType(type);
	initModelParameters();
}

DataModel2D::DataModel2D(const DataModel2D &data_model_) :
		Model2D(data_model_.getName()), data(
				new double[data_model_.cell_count[0] * data_model_.cell_count[1]]), grid_density(
				1.0), intpol_type(data_model_.intpol_type) {
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

		int idx_last(0);
		int idy_last(0);

		for (it = data_.begin(); it != data_.end(); it++) {
			int idx = ((it->first.first - domain_low[0]) / grid_spacing[0]);
			int idy = ((it->first.second - domain_low[1]) / grid_spacing[1]);

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

	grid_density = 1.0 / grid_spacing[0] / grid_spacing[1];

	std::cout << "initialized interpolation model!" << std::endl;

	setVar1Domain(domain_low[0], domain_high[0]);
	setVar2Domain(domain_low[1], domain_high[1]);
}

void DataModel2D::initModelParameters() {
	offset_x = getModelParameterSet().addModelParameter("offset_x");
	offset_x->setValue(0.0);
	offset_y = getModelParameterSet().addModelParameter("offset_y");
	offset_y->setValue(0.0);
}

void DataModel2D::setIntpolType(ModelStructs::InterpolationType intpol_type_) {
	intpol_type = intpol_type_;
	if (intpol_type == ModelStructs::CONSTANT) {
		model_func = &DataModel2D::evaluateConstant;
	} else
		model_func = &DataModel2D::evaluateLinear;
}

double DataModel2D::evaluateConstant(const double *x) const {
	unsigned int idx = (unsigned int) ((x[0] - domain_low[0]) / grid_spacing[0]);
	unsigned int idy = (unsigned int) ((x[1] - domain_low[1]) / grid_spacing[1]);

	return data[idx * cell_count[1] + idy];
}

double DataModel2D::evaluateLinear(const double *x) const {
	double dx = (x[0] - domain_low[0]) / grid_spacing[0];
	double dy = (x[1] - domain_low[1]) / grid_spacing[1];
	unsigned int idx = (unsigned int) dx;
	unsigned int idy = (unsigned int) dy;
	int idx_low(idx);
	int idx_high(idx);
	int idy_low(idy);
	int idy_high(idy);
	if (dx - idx > 0.5)
		++idx_high;
	else
		--idx_low;
	if (dy - idy > 0.5)
		++idy_high;
	else
		--idy_low;

	double p11(0.0), p12(0.0), p21(0.0), p22(0.0);
	if (idx_low > 0 && idy_low > 0 && idx_high < (int)cell_count[0] - 1
			&& idy_high < (int)cell_count[1] - 1) {
		p11 = data[idx_low * cell_count[1] + idy_low];
		p12 = data[idx_low * cell_count[1] + idy_high];
		p21 = data[idx_high * cell_count[1] + idy_low];
		p22 = data[idx_high * cell_count[1] + idy_high];
	} else if (idx_low < 0) {
		p21 = data[idx_high * cell_count[1] + idy_low];
		p22 = data[idx_high * cell_count[1] + idy_high];
	} else if (idx_high > (int)cell_count[0] - 1) {
		p11 = data[idx_low * cell_count[1] + idy_low];
		p12 = data[idx_low * cell_count[1] + idy_high];
	} else if (idy_low < 0) {
		p11 = data[idx_low * cell_count[1] + idy_low];
		p12 = data[idx_low * cell_count[1] + idy_high];
	} else if (idy_high > (int)cell_count[1] - 1) {
		p12 = data[idx_low * cell_count[1] + idy_high];
		p22 = data[idx_high * cell_count[1] + idy_high];
	}

	//double value = (p11 + (p21 - p11) * dx + (p12 - p11) * dy
	//		+ (p22 + p11 - p21 - p12) * dx * dy);
	double dx2x(domain_low[0] + (0.5 + idx_high) * grid_spacing[0] - x[0]);
	double dxx1(x[0] - (domain_low[0] + (0.5 + idx_low) * grid_spacing[0]));
	double dy2y(domain_low[1] + (0.5 + idy_high) * grid_spacing[1] - x[1]);
	double dyy1(x[1] - (domain_low[1] + (0.5 + idy_low) * grid_spacing[1]));
	double value = (p11 * dx2x * dy2y + p21 * dxx1 * dy2y + p12 * dx2x * dyy1
			+ p22 * dxx1 * dyy1) * grid_density;
	//std::cout << value << std::endl;
	return value;
}

double DataModel2D::eval(const double *x) const {
	double shifted_x[2];
	shifted_x[0] = x[0] - offset_x->getValue();
	shifted_x[1] = x[1] - offset_y->getValue();

	if (shifted_x[0] < domain_low[0] || shifted_x[0] > domain_high[0]
			|| shifted_x[1] < domain_low[1] || shifted_x[1] > domain_high[1])
		return 0.0;
	return (this->*model_func)(shifted_x);
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
