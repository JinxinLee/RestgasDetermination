/*
 * PndLmdModelFactory.cxx
 *
 *  Created on: Dec 18, 2012
 *      Author: steve
 */

#include "PndLmdModelFactory.h"
#include "PndLmdLumiFitOptions.h"
#include "PndLmdAcceptance.h"
#include "PndLmdDPMMTModel1D.h"
#include "PndLmdDPMAngModel1D.h"
#include "PndLmdROOTDataModel1D.h"
#include "ProductModel1D.h"
#include "GaussianModel1D.h"
#include "DoubleGaussianModel1D.h"

#include "NumericConvolutionModel1D.h"
#include "PndLmdSmearingGaussianModelParametrization1D.h"
#include "PndLmdSmearingDoubleGaussianModelParametrization1D.h"
#include "PndLmdDPMModelParametrization.h"

PndLmdModelFactory::PndLmdModelFactory() {

}

PndLmdModelFactory::~PndLmdModelFactory() {

}

std::shared_ptr<Model1D> PndLmdModelFactory::generate1DResolutionModel(
    PndLmdLumiFitOptions *fit_options) {
  if (1 == fit_options->getSmearingModelType()) {
    std::shared_ptr<Model1D> gauss(new DoubleGaussianModel1D());
    // ok that part is a little different the following class will just do all
    // of the parametrization stuff for the double gaussian, instead of writing
    // it all down in here...
    PndLmdSmearingDoubleGaussianModelParametrization1D gauss_parametrization(
        gauss);
    return gauss;
  } else //(0 == fit_options->getSmearingModelType())
  {
    std::shared_ptr<Model1D> gauss(new GaussianModel1D());
    std::shared_ptr<Parametrization> smear_parametrization(
        new PndLmdSmearingGaussianModelParametrization1D(
            gauss->getModelParameterSet()));
    gauss->getModelParameterHandler().registerParametrizations(
        gauss->getModelParameterSet(), smear_parametrization);
    return gauss;
  }
}

std::shared_ptr<Model1D> PndLmdModelFactory::generate1DModel(
    PndLmdLumiFitOptions *fit_options, double plab,
    PndLmdAcceptance *acceptance) {
  std::shared_ptr<Model1D> current_model;
  if (fit_options->isFitRaw()) {
    current_model.reset(new PndLmdDPMMTModel1D());

  } else {
    current_model.reset(new PndLmdDPMAngModel1D());
  }

  std::shared_ptr<Parametrization> dpm_parametrization(
      new PndLmdDPMModelParametrization(current_model->getModelParameterSet()));
  current_model->getModelParameterHandler().registerParametrizations(
      current_model->getModelParameterSet(), dpm_parametrization);

  if (fit_options->isAcceptanceCorrOn()) { // with acceptance corr
    if (acceptance) {
      // translate acceptance interpolation option
      PndLmdROOTDataModel1D::interpolation_type intpol_type =
          PndLmdROOTDataModel1D::LINEAR;
      if (fit_options->getAcceptanceInterpolationType() == 0) {
        intpol_type = PndLmdROOTDataModel1D::CONSTANT;
      } else if (fit_options->getAcceptanceInterpolationType() == 2) {
        intpol_type = PndLmdROOTDataModel1D::SPLINE;
      }
      std::shared_ptr<Model1D> acc(
          new PndLmdROOTDataModel1D(
              acceptance->getAcceptance1D(fit_options->isFitRaw()),
              intpol_type));

      current_model.reset(new ProductModel1D(current_model, acc));
    } else {
      std::cout
          << "ERROR: requesting an acceptance corrected model without an acceptance object!"
          << std::endl;
    }
  }

  if (fit_options->isSmearingOn()) { // with resolution smearing
    // ok since we have smearing on, generate smearing model
    current_model.reset(
        new NumericConvolutionModel1D(current_model,
            generate1DResolutionModel(fit_options)));
  }

  // every model has superior parameters which have to be set by the user
  // if they are kept fixed
  // in this case its the lab momentum plab
  current_model->getModelParameterSet().setModelParameterValue("p_lab", plab);

  return current_model;
}

Model2D& PndLmdModelFactory::generate2DModel(
    PndLmdLumiFitOptions *fit_options) {
// TODO
}
