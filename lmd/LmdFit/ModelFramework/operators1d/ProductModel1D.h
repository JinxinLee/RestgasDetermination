/*
 * ProductModel1D.h
 *
 *  Created on: Jan 10, 2013
 *      Author: steve
 */

#ifndef PRODUCTMODEL1D_H_
#define PRODUCTMODEL1D_H_

#include "core/Model1D.h"

class ProductModel1D: public Model1D {
private:
	shared_ptr<Model1D> first, second;
public:
	ProductModel1D(std::string name_, shared_ptr<Model1D> first_,
			shared_ptr<Model1D> second_);

	void initModelParameters();

	double eval(const double *x) const;

	void updateDomain();

	virtual std::pair<double, double> getUncertaincy(const double *x) const;
};

#endif /* PRODUCTMODEL1D_H_ */
