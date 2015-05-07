#ifndef DATAMODEL2D_H_
#define DATAMODEL2D_H_

#include "core/Model2D.h"
#include "LumiFitStructs.h"

class DataModel2D: public Model2D {
private:
	double grid_spacing[2];
	unsigned int cell_count[2];
	double *data;

	double domain_low[2];
	double domain_high[2];

	double grid_density;

	ModelStructs::InterpolationType intpol_type;

	shared_ptr<ModelPar> offset_x;
	shared_ptr<ModelPar> offset_y;

	// function pointer used to switch between different algorithms for interpolation
	typedef double (DataModel2D::*function)(const double *x) const;

	function model_func;

	std::pair<double, bool> getCellSpacing(
			const std::set<double> &values);
public:
	DataModel2D(std::string name_, ModelStructs::InterpolationType type = ModelStructs::LINEAR);
	DataModel2D(const DataModel2D &data_model_);
	virtual ~DataModel2D();

	void setData(const std::map<std::pair<double, double>, double> &data_);

	void setIntpolType(
			ModelStructs::InterpolationType intpol_type_);

	double evaluateConstant(const double *x) const;
	double evaluateLinear(const double *x) const;

	double eval(const double *x) const;

	void initModelParameters();

	void updateDomain();

	DataModel2D& operator=(const DataModel2D &data_model_);
};

#endif /* DATAMODEL2D_H_ */
