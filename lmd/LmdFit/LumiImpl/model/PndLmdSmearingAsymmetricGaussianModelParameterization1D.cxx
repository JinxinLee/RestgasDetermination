/*
 * PndLmdSmearingAsymmetricGaussianModelParametrization1D.cxx
 *
 *  Created on: Jan 22, 2013
 *      Author: steve
 */

#include "PndLmdSmearingAsymmetricGaussianModelParameterization1D.h"
#include "Model1D.h"
#include "ModelPar.h"
#include "PolynomialModel1D.h"
#include "PndLmdROOTDataModel1D.h"
#include "PndLmdLumiFitOptions.h"
#include "PndLmdLumiHelper.h"

#include <string>

#include "TFile.h"
#include "TGraphAsymmErrors.h"

PndLmdSmearingAsymmetricGaussianModelParameterization1D::PndLmdSmearingAsymmetricGaussianModelParameterization1D(
		shared_ptr<Model1D> model, const PndLmdLumiFitOptions *fit_options) {

	if (fit_options->getSmearingModelType() < 9) {
		shared_ptr<Model1D> poly_model1(
				new PolynomialModel1D("asymm_gauss_sigma_left_poly_model_1d", 1));
		shared_ptr<ParametrizationModel> pm1(new ParametrizationModel(poly_model1));
		model->getModelParameterHandler().registerParametrizationModel(
				model->getModelParameterSet().getModelParameter(
						"asymm_gauss_sigma_left"), pm1);

		shared_ptr<Model1D> poly_model2(
				new PolynomialModel1D("asymm_gauss_sigma_right_poly_model_1d", 1));
		shared_ptr<ParametrizationModel> pm2(new ParametrizationModel(poly_model2));
		model->getModelParameterHandler().registerParametrizationModel(
				model->getModelParameterSet().getModelParameter(
						"asymm_gauss_sigma_right"), pm2);

		shared_ptr<Model1D> poly_model3(
				new PolynomialModel1D("asymm_gauss_mean_poly_model_1d", 1));
		shared_ptr<ParametrizationModel> pm3(new ParametrizationModel(poly_model3));
		model->getModelParameterHandler().registerParametrizationModel(
				model->getModelParameterSet().getModelParameter("asymm_gauss_mean"),
				pm3);
	} else {
		shared_ptr<PndLmdROOTDataModel1D> poly_model1(
				new PndLmdROOTDataModel1D("asymm_gauss_sigma_left_data_model_1d"));
		shared_ptr<ParametrizationModel> pm1(new ParametrizationModel(poly_model1));
		model->getModelParameterHandler().registerParametrizationModel(
				model->getModelParameterSet().getModelParameter(
						"asymm_gauss_sigma_left"), pm1);

		shared_ptr<PndLmdROOTDataModel1D> poly_model2(
				new PndLmdROOTDataModel1D("asymm_gauss_sigma_right_data_model_1d"));
		shared_ptr<ParametrizationModel> pm2(new ParametrizationModel(poly_model2));
		model->getModelParameterHandler().registerParametrizationModel(
				model->getModelParameterSet().getModelParameter(
						"asymm_gauss_sigma_right"), pm2);

		shared_ptr<PndLmdROOTDataModel1D> poly_model3(
				new PndLmdROOTDataModel1D("asymm_gauss_mean_data_model_1d"));
		shared_ptr<ParametrizationModel> pm3(new ParametrizationModel(poly_model3));
		model->getModelParameterHandler().registerParametrizationModel(
				model->getModelParameterSet().getModelParameter("asymm_gauss_mean"),
				pm3);

		if (fit_options->isResolutionParametrizationFileUrlActive()) {
			PndLmdLumiHelper lmd_helper;

			TFile f(fit_options->getResolutionParametrizationFileUrl(), "READ");

			std::vector<PndLmdLumiHelper::lmd_graph*> allgraphs =
					lmd_helper.getResolutionModelResultsFromFile(&f);
			std::vector<PndLmdLumiHelper::lmd_graph*> graphs =
					lmd_helper.filterLmdGraphs(allgraphs, "theta");

			for (unsigned int i = 0; i < graphs.size(); i++) {
				std::map<unsigned int, std::pair<std::string, std::string> >::const_iterator parameter_name =
						graphs[i]->parameter_name_stack.begin();
				std::cout << parameter_name->second.first << " "
						<< parameter_name->second.second << std::endl;
				if (0
						== parameter_name->second.second.compare(
								std::string("asymm_gauss_sigma_left"))) {
					TGraphAsymmErrors *gr = new TGraphAsymmErrors(
							graphs[i]->graph->GetN(), graphs[i]->graph->GetX(),
							graphs[i]->graph->GetY(), graphs[i]->graph->GetEXlow(),
							graphs[i]->graph->GetEXhigh(), graphs[i]->graph->GetEYlow(),
							graphs[i]->graph->GetEYhigh());
					poly_model1->setGraph(gr);
					poly_model1->setIntpolType(PndLmdROOTDataModel1D::LINEAR);

				} else if (0
						== parameter_name->second.second.compare(
								std::string("asymm_gauss_sigma_right"))) {
					TGraphAsymmErrors *gr = new TGraphAsymmErrors(
							graphs[i]->graph->GetN(), graphs[i]->graph->GetX(),
							graphs[i]->graph->GetY(), graphs[i]->graph->GetEXlow(),
							graphs[i]->graph->GetEXhigh(), graphs[i]->graph->GetEYlow(),
							graphs[i]->graph->GetEYhigh());
					poly_model2->setGraph(gr);
					poly_model2->setIntpolType(PndLmdROOTDataModel1D::LINEAR);

				} else if (0
						== parameter_name->second.second.compare(
								std::string("asymm_gauss_mean"))) {
					TGraphAsymmErrors *gr = new TGraphAsymmErrors(
							graphs[i]->graph->GetN(), graphs[i]->graph->GetX(),
							graphs[i]->graph->GetY(), graphs[i]->graph->GetEXlow(),
							graphs[i]->graph->GetEXhigh(), graphs[i]->graph->GetEYlow(),
							graphs[i]->graph->GetEYhigh());
					poly_model3->setGraph(gr);
					poly_model3->setIntpolType(PndLmdROOTDataModel1D::LINEAR);

				} else {
					std::cout
							<< "ERROR: Not able to obtain parametrization model for parameter "
							<< parameter_name->second.second << "!" << std::endl;
				}
			}
		}
	}
}

PndLmdSmearingAsymmetricGaussianModelParameterization1D::~PndLmdSmearingAsymmetricGaussianModelParameterization1D() {
// TODO Auto-generated destructor stub
}
