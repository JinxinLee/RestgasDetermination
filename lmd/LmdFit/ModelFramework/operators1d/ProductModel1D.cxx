/*
 * ProductModel1D.cxx
 *
 *  Created on: Jan 10, 2013
 *      Author: steve
 */

#include "ProductModel1D.h"

#include <iostream>

ProductModel1D::ProductModel1D(std::string name_, shared_ptr<Model1D> first_,
		shared_ptr<Model1D> second_) :
		Model1D(name_) {
	first = first_;
	second = second_;

	addModelToList(first);
	addModelToList(second);
}

void ProductModel1D::initModelParameters() {

}

double ProductModel1D::eval(const double *x) const {
	return multiply(first, second, x);
}

void ProductModel1D::updateDomain() {
	// first we need to check if user defined a domain for his models
	if (first->getDomainRange() == 0 || second->getDomainRange() == 0) {
		std::cout
				<< "Warning: Some of the models used for the multiplication have not"
						" defined any domains!" << std::endl;
	} else {
		setDomain(std::max(first->getDomain().first, second->getDomain().first),
				std::min(first->getDomain().second, second->getDomain().second));
	}
}
