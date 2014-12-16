#include "PndLmdSmearingConvolutionModel2D.h"

#include <cmath>
#include <iostream>

PndLmdSmearingConvolutionModel2D::PndLmdSmearingConvolutionModel2D(
		std::string name_, shared_ptr<Model2D> unsmeared_model_,
		shared_ptr<PndLmdSmearingModel2D> smearing_model_) :
		Model2D(name_) {

	unsmeared_model = unsmeared_model_;
	smearing_model = smearing_model_;

	addModelToList(unsmeared_model);
}

PndLmdSmearingConvolutionModel2D::~PndLmdSmearingConvolutionModel2D() {
}

void PndLmdSmearingConvolutionModel2D::initModelParameters() {
}

void PndLmdSmearingConvolutionModel2D::injectModelParameter(shared_ptr<ModelPar> model_param) {
	getModelParameterSet().addModelParameter(model_param);
}

double PndLmdSmearingConvolutionModel2D::eval(const double *x) const {
	double value = 0.0;
	const std::map<PndLmdLumiHelper::bin_dimension, double> &mc_element_contributors =
			smearing_model->getListOfContributors(x);

	std::map<PndLmdLumiHelper::bin_dimension, double>::const_iterator mc_element_it;
	for (mc_element_it = mc_element_contributors.begin();
			mc_element_it != mc_element_contributors.end(); mc_element_it++) {
		double xx[2];
		xx[0] = mc_element_it->first.x_range.getDimensionMean();
		xx[1] = mc_element_it->first.y_range.getDimensionMean();
		double integral_unsmeared_model = unsmeared_model->evaluate(xx);
		integral_unsmeared_model = integral_unsmeared_model
				* mc_element_it->first.x_range.getDimensionLength()
				* mc_element_it->first.y_range.getDimensionLength();
		value = value + integral_unsmeared_model * mc_element_it->second;
		/*std::cout << integral_unsmeared_model << " " << mc_element_it->second
		 << std::endl;*/
	}

	//std::cout << "value: " << value << std::endl;
	return value;
}

void PndLmdSmearingConvolutionModel2D::updateDomain() {

}
