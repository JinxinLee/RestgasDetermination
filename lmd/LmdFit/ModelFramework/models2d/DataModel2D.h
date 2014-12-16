#ifndef DATAMODEL2D_H_
#define DATAMODEL2D_H_

#include "core/Model2D.h"

class DataModel2D: public Model2D {
private:
	double grid_spacing[2];
	int cell_count[2];
	double *data;

	double domain_low[2];
	double domain_high[2];

	enum interpolation_type {
		CONSTANT, LINEAR, SPLINE3
	} intpol_type;

	std::pair<double, bool> getCellSpacing(
			const std::set<double> &values);
public:
	DataModel2D(std::string name_, interpolation_type type = LINEAR);
	virtual ~DataModel2D();

	void setData(const std::map<std::pair<double, double>, double> &data_);

	double eval(const double *x) const;

	void initModelParameters();

	void updateDomain();
};

#endif /* DATAMODEL2D_H_ */
