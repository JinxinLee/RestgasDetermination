/*
 * PndLmdResolutionModelParameterizationHelper.cxx
 *
 *  Created on: Jul 30, 2014
 *      Author: steve
 */

#include <model/PndLmdResolutionModelParameterizationHelper.h>
#include "core/Model2D.h"
#include "core/Model1D.h"
#include "core/ModelPar.h"
#include "models1d/PolynomialModel1D.h"
#include "model/PndLmdROOTDataModel1D.h"
#include "models2d/DataModel2D.h"
#include "PndLmdLumiHelper.h"

#include "TGraphAsymmErrors.h"

PndLmdResolutionModelParameterizationHelper::PndLmdResolutionModelParameterizationHelper(
		shared_ptr<Model> resolution_model_,
		const LumiFit::PndLmdFitModelOptions& model_options_) :
		model_options(model_options_), resolution_model(resolution_model_) {
}

PndLmdResolutionModelParameterizationHelper::~PndLmdResolutionModelParameterizationHelper() {
	// TODO Auto-generated destructor stub
}

void PndLmdResolutionModelParameterizationHelper::createAndInitializeParametrizationModelsForParameters(
		const std::vector<std::string> &parameter_names) {
	if (!model_options.use_resolution_parameter_interpolation) {
		for (unsigned int i = 0; i < parameter_names.size(); i++) {
			addPolynomialParametrizationModelToResolutionModelParameter(
					parameter_names[i]);
		}
		initParametrizationModelParametersFromFile();
	} else {
		for (unsigned int i = 0; i < parameter_names.size(); i++) {
			addAndInitDataParametrizationModelForResolutionModelParameter(
					parameter_names[i]);
		}
	}
}

void PndLmdResolutionModelParameterizationHelper::addPolynomialParametrizationModelToResolutionModelParameter(
		const std::string &parameter_name) {
	if (model_options.fit_dimension == 1) {
		shared_ptr<Model1D> poly_model(
				new PolynomialModel1D(parameter_name + "_poly_model_1d", 1));
		shared_ptr<ParametrizationModel> pm(new ParametrizationModel(poly_model));
		resolution_model->getModelParameterHandler().registerParametrizationModel(
				resolution_model->getModelParameterSet().getModelParameter(
						parameter_name), pm);
	}
}

void PndLmdResolutionModelParameterizationHelper::addAndInitDataParametrizationModelForResolutionModelParameter(
		const std::string &parameter_name) {
	if (model_options.fit_dimension == 1) {
		shared_ptr<PndLmdROOTDataModel1D> interpolation_model(
				new PndLmdROOTDataModel1D(parameter_name + "_data_model_1d"));
		shared_ptr<ParametrizationModel> pm(
				new ParametrizationModel(interpolation_model));
		resolution_model->getModelParameterHandler().registerParametrizationModel(
				resolution_model->getModelParameterSet().getModelParameter(
						parameter_name), pm);

		if (model_options.resolution_smearing_active) {
			PndLmdLumiHelper lmd_helper;

			std::vector<PndLmdLumiHelper::lmd_graph*> allgraphs =
					lmd_helper.getResolutionModelResultsFromFile(
							model_options.resolution_parametrization_file_url);
			std::set<LumiFit::LmdDimensionOptions> dependencies;
			LumiFit::LmdDimensionOptions dim_opt;
			dim_opt.dimension_type = LumiFit::THETA;
			dim_opt.track_type = LumiFit::MC;
			dim_opt.track_param_type = LumiFit::IP;
			dependencies.insert(dim_opt);
			std::vector<PndLmdLumiHelper::lmd_graph*> graphs =
					lmd_helper.filterLmdGraphs(allgraphs, dependencies);
			graphs = lmd_helper.filterLmdGraphs(graphs, model_options.selections);

			bool found(false);
			for (unsigned int i = 0; i < graphs.size(); i++) {
				std::map<unsigned int, std::pair<std::string, std::string> >::const_iterator parameter_name_pair =
						graphs[i]->parameter_name_stack.begin();
				if (0 == parameter_name_pair->second.second.compare(parameter_name)) {
					std::cout << "Found parameterization for "
							<< parameter_name_pair->second.first << " "
							<< parameter_name_pair->second.second << std::endl;

					TGraphAsymmErrors *gr = new TGraphAsymmErrors(
							graphs[i]->graph->GetN(), graphs[i]->graph->GetX(),
							graphs[i]->graph->GetY(), graphs[i]->graph->GetEXlow(),
							graphs[i]->graph->GetEXhigh(), graphs[i]->graph->GetEYlow(),
							graphs[i]->graph->GetEYhigh());
					interpolation_model->setGraph(gr);
					interpolation_model->setIntpolType(LumiFit::LINEAR);
					found = true;
					break;
				}
			}
			if (!found) {
				std::cout
						<< "ERROR: Not able to obtain parametrization model for parameter "
						<< parameter_name << "!" << std::endl;
			}
		}
	} else if (model_options.fit_dimension == 2) {
		if (model_options.resolution_smearing_active) {
			shared_ptr<DataModel2D> interpolation_model(
					new DataModel2D(parameter_name + "_data_model_2d"));
			shared_ptr<ParametrizationModel> pm(
					new ParametrizationModel(interpolation_model));
			resolution_model->getModelParameterHandler().registerParametrizationModel(
					resolution_model->getModelParameterSet().getModelParameter(
							parameter_name), pm);

			PndLmdLumiHelper lmd_helper;

			std::vector<PndLmdLumiHelper::lmd_graph*> allgraphs =
					lmd_helper.getResolutionModelResultsFromFile(
							model_options.resolution_parametrization_file_url);
			std::set<LumiFit::LmdDimensionOptions> dependencies;
			LumiFit::LmdDimensionOptions dim_opt;
			dim_opt.dimension_type = LumiFit::THETA;
			dim_opt.track_type = LumiFit::MC;
			dim_opt.track_param_type = LumiFit::IP;
			dependencies.insert(dim_opt);
			dim_opt.dimension_type = LumiFit::PHI;
			dependencies.insert(dim_opt);
			std::vector<PndLmdLumiHelper::lmd_graph*> graphs =
					lmd_helper.filterLmdGraphs(allgraphs, dependencies);
			graphs = lmd_helper.filterLmdGraphs(graphs, model_options.selections);

			bool found(false);
			for (unsigned int i = 0; i < graphs.size(); i++) {
				std::map<unsigned int, std::pair<std::string, std::string> >::const_iterator parameter_name_pair =
						graphs[i]->parameter_name_stack.begin();
				if (0 == parameter_name_pair->second.second.compare(parameter_name)) {
					std::cout << "Found parameterization for "
							<< parameter_name_pair->second.first << " "
							<< parameter_name_pair->second.second << std::endl;

					/*TGraph2DErrors *graph2d = graphs[i]->graph2d;
					 TGraph2DErrors *gr(0);
					 if (graph2d)
					 gr = new TGraph2DErrors(graph2d->GetN(), graph2d->GetX(),
					 graph2d->GetY(), graph2d->GetZ(), graph2d->GetEX(),
					 graph2d->GetEY(), graph2d->GetEZ());

					 interpolation_model->setGraph(gr);
					 interpolation_model->setIntpolType(LumiFit::CONSTANT);*/

					TGraph2DErrors *graph2d = graphs[i]->graph2d;
					std::map<std::pair<double, double>, double> datamap;
					for (unsigned int gi = 0; gi < graph2d->GetN(); gi++)
						datamap[std::make_pair(graph2d->GetX()[gi], graph2d->GetY()[gi])] =
								graph2d->GetZ()[gi];

					interpolation_model->setData(datamap);

					found = true;
					break;
				}
			}
			if (!found) {
				std::cout
						<< "ERROR: Not able to obtain parametrization model for parameter "
						<< parameter_name << "!" << std::endl;
			}
		}
	}
}

void PndLmdResolutionModelParameterizationHelper::initParametrizationModelParametersFromFile() {
	PndLmdLumiHelper lmd_helper;

	std::vector<PndLmdLumiHelper::lmd_graph*> allgraphs =
			lmd_helper.getResolutionModelResultsFromFile(
					model_options.resolution_parametrization_file_url);

	std::set<LumiFit::LmdDimensionOptions> dependencies;
	LumiFit::LmdDimensionOptions dim_opt;
	dim_opt.dimension_type = LumiFit::THETA;
	dim_opt.track_type = LumiFit::MC;
	dim_opt.track_param_type = LumiFit::IP;
	dependencies.insert(dim_opt);
	if (model_options.fit_dimension == 2) {
		dim_opt.dimension_type = LumiFit::PHI;
		dependencies.insert(dim_opt);
	}

	std::vector<PndLmdLumiHelper::lmd_graph*> graphs = lmd_helper.filterLmdGraphs(
			allgraphs, dependencies);

	for (unsigned int i = 0; i < graphs.size(); i++) {
		shared_ptr<Model> temp_resolution_param_model = resolution_model;
		bool model_retrieve_success = true;
		// get the model that we have to fit to the data
		for (std::map<unsigned int, std::pair<std::string, std::string> >::const_iterator parameter_name =
				graphs[i]->parameter_name_stack.begin();
				parameter_name != graphs[i]->parameter_name_stack.end();
				parameter_name++) {
			ParametrizationProxy par_proxy =
					temp_resolution_param_model->getModelParameterHandler().getParametrizationProxyForModelParameter(
							parameter_name->second.second);
			if (par_proxy.hasParametrizationModel()) {
				temp_resolution_param_model =
						par_proxy.getParametrizationModel()->getModel();
			} else {
				std::cout
						<< "ERROR: Not able to obtain parametrization model for parameter "
						<< parameter_name->second.second << "!" << std::endl;
				model_retrieve_success = false;
				break;
			}
		}
		if (model_retrieve_success) {
			resolution_model->getModelParameterHandler().initModelParametersFromFitResult(
					*graphs[i]->fit_result);
		}
	}
}
