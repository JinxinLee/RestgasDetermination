#ifndef SMEARINGCONVOLUTIONMODEL2D_H_
#define SMEARINGCONVOLUTIONMODEL2D_H_

#include "core/Model2D.h"

class SmearingConvolutionModel2D: public Model2D {
private:
	unsigned int divisions_var1;
	unsigned int divisions_var2;

	shared_ptr<Model2D> first, second;

public:
	SmearingConvolutionModel2D(std::string name_, shared_ptr<Model2D> first_,
			shared_ptr<Model2D> second_);
	virtual ~SmearingConvolutionModel2D();

	void initModelParameters();

	void calculateLookupTable();

	double eval(const double *x) const;

	void updateDomain();
};

#endif /* SMEARINGCONVOLUTIONMODEL2D_H_ */
