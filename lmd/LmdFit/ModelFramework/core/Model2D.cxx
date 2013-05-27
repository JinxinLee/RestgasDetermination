/*
 * Model2D.cxx
 *
 *  Created on: Jan 16, 2013
 *      Author: steve
 */

#include "Model2D.h"

Model2D::Model2D(std::string name_) : Model(name_, 2) {
	// TODO Auto-generated constructor stub
}

Model2D::~Model2D() {
	// TODO Auto-generated destructor stub
}

double Model2D::getVar1DomainRange() {
	return var1_domain_bounds.second - var1_domain_bounds.first;
}
double Model2D::getVar2DomainRange() {
	return var2_domain_bounds.second - var2_domain_bounds.first;
}

double Model2D::getVar1DomainLowerBound() {
	return var1_domain_bounds.first;
}
double Model2D::getVar2DomainLowerBound() {
	return var2_domain_bounds.first;
}
void Model2D::setVar1Domain(double lower_bound, double upper_bound) {
	var1_domain_bounds.first = lower_bound;
	var1_domain_bounds.second = upper_bound;
}
void Model2D::setVar2Domain(double lower_bound, double upper_bound) {
	var2_domain_bounds.first = lower_bound;
	var2_domain_bounds.second = upper_bound;
}
