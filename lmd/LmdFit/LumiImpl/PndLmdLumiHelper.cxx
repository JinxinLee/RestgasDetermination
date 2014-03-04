/*
 * PndLmdLumiHelper.cxx
 *
 *  Created on: Jun 27, 2012
 *      Author: steve
 */

#include "PndLmdLumiHelper.h"
#include "PndLmdResolution.h"
#include "PndLmdLumiFitOptions.h"
#include "ROOTMinimizer.h"
#include "PndLmdModelFactory.h"
#include "PndLmdDPMAngModel1D.h"
#include "PndLmdDPMModelParametrization.h"
#include "PndLmdLumiFitResult.h"
#include "ROOTDataHelper.h"
#include "Data.h"
#include "Chi2Estimator.h"

#include "ModelStructs.h"

#include <iostream>
#include <sstream>

#include "TFile.h"
#include "TMath.h"
#include "TRandom.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGraphErrors.h"
#include "TKey.h"

PndLmdLumiHelper::PndLmdLumiHelper() {
}

double PndLmdLumiHelper::getMomentumTransferFromTheta(double plab,
		double theta) {
	PndLmdDPMAngModel1D model("dpm_angular_1d", PndLmdDPMMTModel1D::ALL);
	shared_ptr<Parametrization> para(
			new PndLmdDPMModelParametrization(model.getModelParameterSet()));
	model.getModelParameterHandler().registerParametrizations(
			model.getModelParameterSet(), para);
	model.getModelParameterSet().setModelParameterValue("p_lab", plab);
	((Model1D*) &model)->init();
	return -model.getMomentumTransferFromTheta(theta);
}

double PndLmdLumiHelper::calcHistIntegral(const TH1D* hist,
		std::pair<double, double> range) {
	// just determine the bin range over which we have to calculate
	int bin_low = 1; // first bin is underflow
	int bin_high = hist->GetNbinsX();
	for (int i = 0; i < hist->GetNbinsX(); i++) {
		if (hist->GetBinCenter(i) - hist->GetBinWidth(i) / 2 < range.first
				&& hist->GetBinCenter(i) + hist->GetBinWidth(i) / 2 > range.first) {
			bin_low = i;
		}
		if (hist->GetBinCenter(i) - hist->GetBinWidth(i) / 2 < range.second
				&& hist->GetBinCenter(i) + hist->GetBinWidth(i) / 2 > range.second) {
			bin_high = i;
		}
	}
	return hist->Integral(bin_low, bin_high, "width");
}

std::vector<PndLmdLumiHelper::lmd_graph*> PndLmdLumiHelper::getResolutionModelResultsFromFile(
		TFile* f) {
	std::vector<PndLmdLumiHelper::lmd_graph*> return_vec;

	f->cd();
	TIter next(f->GetListOfKeys());
	PndLmdLumiHelper::lmd_graph* ptemp_lmd_graph;
	while ((ptemp_lmd_graph = (PndLmdLumiHelper::lmd_graph*) next())) {
		if (ptemp_lmd_graph) {
			f->GetObject(ptemp_lmd_graph->GetName(), ptemp_lmd_graph); // without getting it explicitly again it crashes...
			return_vec.push_back(new PndLmdLumiHelper::lmd_graph(*ptemp_lmd_graph));
		}
	}
	if (return_vec.size() == 0) {
		std::cout
				<< "WARNING: The given file does not contain any objects of the required type!"
				<< std::endl;
	}
	return return_vec;
}

void PndLmdLumiHelper::saveLmdGraphsToFile(
		std::vector<PndLmdLumiHelper::lmd_graph*>& graph_vec) {

	for (unsigned int i = 0; i < graph_vec.size(); i++) {
		TString name(graph_vec[i]->dependency);
		std::ostringstream strstream;
		strstream.precision(3);
		for (std::map<unsigned int, std::pair<std::string, std::string> >::const_iterator j =
				graph_vec[i]->parameter_name_stack.begin();
				j != graph_vec[i]->parameter_name_stack.end(); j++) {
			strstream << "_" << j->second.first << "-" << j->second.second;
		}
		for (std::map<std::string, double, ModelStructs::string_comp>::const_iterator dependency =
				graph_vec[i]->remaining_dependencies.begin();
				dependency != graph_vec[i]->remaining_dependencies.end();
				dependency++) {
			strstream << "_" << dependency->first << "-" << dependency->second;
		}
		name = name + strstream.str();
		graph_vec[i]->Write(name);
	}
}

void PndLmdLumiHelper::finalizeLmdGraphObjects(
		std::map<
				double,
				std::map<std::pair<std::string, std::string>
						, PndLmdLumiHelper::lmd_graph* , ModelStructs::stringpair_comp> > &graph_map
		, int mode) {

	bool is_dependent = (graph_map.size() > 0);
	bool make_absolute;
	for (std::map<
			double,
			std::map<std::pair<std::string, std::string>, PndLmdLumiHelper::lmd_graph*
					, ModelStructs::stringpair_comp> >::iterator graphs =
			graph_map.begin(); graphs != graph_map.end(); graphs++) {
		make_absolute = false;
		for (std::map<std::pair<std::string, std::string>
				, PndLmdLumiHelper::lmd_graph* , ModelStructs::stringpair_comp>::iterator graph =
				graphs->second.begin(); graph != graphs->second.end(); graph++) {

			if(graph->first.second.find("sigma") != -1)
				make_absolute = true;

			if (is_dependent) {
				if (0 == mode) {
					graph->second->remaining_dependencies["phi"] = graphs->first;
					graph->second->dependency = "theta";
				} else if (1 == mode) {
					graph->second->remaining_dependencies["theta"] = graphs->first;
					graph->second->dependency = "phi";
				}
			}

			graph->second->graph = new TGraphErrors(graph->second->data.size());
			for (unsigned int i = 0; i < graph->second->data.size(); i++) {
				double y = graph->second->data[i].second.value;
				if(make_absolute)
					y = fabs(y);
				graph->second->graph->SetPoint(i, graph->second->data[i].first,
						y);
				graph->second->graph->SetPointError(i, 0.0,
						graph->second->data[i].second.error);
			}
		}
	}
}

std::vector<PndLmdLumiHelper::lmd_graph*> PndLmdLumiHelper::generateLmdGraphs(
		std::vector<PndLmdResolution>& resolutions,
		LumiFit::LmdDimensionRange& fit_range) {

	std::map<
			double,
			std::map<std::pair<std::string, std::string>, PndLmdLumiHelper::lmd_graph*
					, ModelStructs::stringpair_comp> > phi_slice_map;
	std::map<
			double,
			std::map<std::pair<std::string, std::string>, PndLmdLumiHelper::lmd_graph*
					, ModelStructs::stringpair_comp> > theta_slice_map;

	for (unsigned int i = 0; i < resolutions.size(); i++) {
		const std::map<PndLmdLumiFitOptions, PndLmdLumiFitResult*> &fit_results =
				resolutions[i].getFitResults();
		for (std::map<PndLmdLumiFitOptions, PndLmdLumiFitResult*>::const_iterator fit_result =
				fit_results.begin(); fit_result != fit_results.end(); fit_result++) {
			// if fit was not successful, skip this fit result
			if (fit_result->second->getModelFitResult()->getFitStatus() != 0)
				continue;
			const std::set<ModelStructs::minimization_parameter> &fit_parameters =
					fit_result->second->getModelFitResult()->getFitParameters();
			for (std::set<ModelStructs::minimization_parameter>::const_iterator fit_param =
					fit_parameters.begin(); fit_param != fit_parameters.end();
					fit_param++) {
				if (phi_slice_map[resolutions[i].getSecondarySelectionDimension().dimension_range.getDimensionMean()][fit_param->name]
						== 0) {
					phi_slice_map[resolutions[i].getSecondarySelectionDimension().dimension_range.getDimensionMean()][fit_param->name] =
							new PndLmdLumiHelper::lmd_graph();
				}
				phi_slice_map[resolutions[i].getSecondarySelectionDimension().dimension_range.getDimensionMean()][fit_param->name]->data.push_back(
						std::make_pair(
								resolutions[i].getPrimarySelectionDimension().dimension_range.getDimensionMean(),
								*fit_param));
				phi_slice_map[resolutions[i].getSecondarySelectionDimension().dimension_range.getDimensionMean()][fit_param->name]->parameter_name_stack[1] =
						fit_param->name;
				PndLmdLumiFitOptions *fit_opt = new PndLmdLumiFitOptions(
						fit_result->first);
				fit_opt->setPrimaryDimensionFitRange(fit_range);
				phi_slice_map[resolutions[i].getSecondarySelectionDimension().dimension_range.getDimensionMean()][fit_param->name]->fit_options =
						fit_opt;

				if (theta_slice_map[resolutions[i].getPrimarySelectionDimension().dimension_range.getDimensionMean()][fit_param->name]
						== 0) {
					theta_slice_map[resolutions[i].getPrimarySelectionDimension().dimension_range.getDimensionMean()][fit_param->name] =
							new PndLmdLumiHelper::lmd_graph();
				}
				theta_slice_map[resolutions[i].getPrimarySelectionDimension().dimension_range.getDimensionMean()][fit_param->name]->data.push_back(
						std::make_pair(
								resolutions[i].getSecondarySelectionDimension().dimension_range.getDimensionMean(),
								*fit_param));
				theta_slice_map[resolutions[i].getPrimarySelectionDimension().dimension_range.getDimensionMean()][fit_param->name]->parameter_name_stack[1] =
						fit_param->name;
				fit_opt = new PndLmdLumiFitOptions(fit_result->first);
				fit_opt->setPrimaryDimensionFitRange(fit_range);
				theta_slice_map[resolutions[i].getPrimarySelectionDimension().dimension_range.getDimensionMean()][fit_param->name]->fit_options =
						fit_opt;
			}
			if (phi_slice_map[resolutions[i].getSecondarySelectionDimension().dimension_range.getDimensionMean()][std::make_pair(
					fit_parameters.begin()->name.first, "chi2")] == 0) {
				phi_slice_map[resolutions[i].getSecondarySelectionDimension().dimension_range.getDimensionMean()][std::make_pair(
						fit_parameters.begin()->name.first, "chi2")] =
						new PndLmdLumiHelper::lmd_graph();
			}
			phi_slice_map[resolutions[i].getSecondarySelectionDimension().dimension_range.getDimensionMean()][std::make_pair(
					fit_parameters.begin()->name.first, "chi2")]->data.push_back(
					std::make_pair(
							resolutions[i].getPrimarySelectionDimension().dimension_range.getDimensionMean(),
							ModelStructs::minimization_parameter(
									std::make_pair(fit_parameters.begin()->name.first, "chi2"),
									fit_result->second->getRedChiSquare(), 0.0)));
			phi_slice_map[resolutions[i].getSecondarySelectionDimension().dimension_range.getDimensionMean()][std::make_pair(
					fit_parameters.begin()->name.first, "chi2")]->parameter_name_stack[1] =
					std::make_pair(fit_parameters.begin()->name.first, "chi2");
			PndLmdLumiFitOptions *fit_opt = new PndLmdLumiFitOptions(
					fit_result->first);
			fit_opt->setPrimaryDimensionFitRange(fit_range);
			phi_slice_map[resolutions[i].getSecondarySelectionDimension().dimension_range.getDimensionMean()][std::make_pair(
					fit_parameters.begin()->name.first, "chi2")]->fit_options = fit_opt;

			if (theta_slice_map[resolutions[i].getPrimarySelectionDimension().dimension_range.getDimensionMean()][std::make_pair(
					fit_parameters.begin()->name.first, "chi2")] == 0) {
				theta_slice_map[resolutions[i].getPrimarySelectionDimension().dimension_range.getDimensionMean()][std::make_pair(
						fit_parameters.begin()->name.first, "chi2")] =
						new PndLmdLumiHelper::lmd_graph();
			}
			theta_slice_map[resolutions[i].getPrimarySelectionDimension().dimension_range.getDimensionMean()][std::make_pair(
					fit_parameters.begin()->name.first, "chi2")]->data.push_back(
					std::make_pair(
							resolutions[i].getSecondarySelectionDimension().dimension_range.getDimensionMean(),
							ModelStructs::minimization_parameter(
									std::make_pair(fit_parameters.begin()->name.first, "chi2"),
									fit_result->second->getRedChiSquare(), 0.0)));
			theta_slice_map[resolutions[i].getPrimarySelectionDimension().dimension_range.getDimensionMean()][std::make_pair(
					fit_parameters.begin()->name.first, "chi2")]->parameter_name_stack[1] =
					std::make_pair(fit_parameters.begin()->name.first, "chi2");
			fit_opt = new PndLmdLumiFitOptions(fit_result->first);
			fit_opt->setPrimaryDimensionFitRange(fit_range);
			theta_slice_map[resolutions[i].getPrimarySelectionDimension().dimension_range.getDimensionMean()][std::make_pair(
					fit_parameters.begin()->name.first, "chi2")]->fit_options = fit_opt;
		}
	}
	// go through map and make TGraph objects
	finalizeLmdGraphObjects(phi_slice_map, 0);
	finalizeLmdGraphObjects(theta_slice_map, 1);

	//construct return vector
	std::vector<PndLmdLumiHelper::lmd_graph*> return_vec;
	for (std::map<
			double,
			std::map<std::pair<std::string, std::string>, PndLmdLumiHelper::lmd_graph*
					, ModelStructs::stringpair_comp> >::iterator graphs =
			phi_slice_map.begin(); graphs != phi_slice_map.end(); graphs++) {
		for (std::map<std::pair<std::string, std::string>
				, PndLmdLumiHelper::lmd_graph* , ModelStructs::stringpair_comp>::iterator graph =
				graphs->second.begin(); graph != graphs->second.end(); graph++) {
			return_vec.push_back(graph->second);
		}

	}
	for (std::map<
			double,
			std::map<std::pair<std::string, std::string>, PndLmdLumiHelper::lmd_graph*
					, ModelStructs::stringpair_comp> >::iterator graphs =
			theta_slice_map.begin(); graphs != theta_slice_map.end(); graphs++) {
		for (std::map<std::pair<std::string, std::string>
				, PndLmdLumiHelper::lmd_graph* , ModelStructs::stringpair_comp>::iterator graph =
				graphs->second.begin(); graph != graphs->second.end(); graph++) {
			return_vec.push_back(graph->second);
		}

	}
	return return_vec;
}

std::vector<PndLmdLumiHelper::lmd_graph*> PndLmdLumiHelper::generateNewLmdGraphs(
		std::vector<PndLmdLumiHelper::lmd_graph*> &util_lmd_graphs) {

	std::map<std::pair<std::string, std::string>, PndLmdLumiHelper::lmd_graph*
			, ModelStructs::stringpair_comp> slice_map;

	for (unsigned int i = 0; i < util_lmd_graphs.size(); i++) {
		if (util_lmd_graphs[i]->fit_result) {
			const std::set<ModelStructs::minimization_parameter> &fit_parameters =
					util_lmd_graphs[i]->fit_result->getFitParameters();
			for (std::set<ModelStructs::minimization_parameter>::const_iterator fit_param =
					fit_parameters.begin(); fit_param != fit_parameters.end();
					fit_param++) {
				if (slice_map[fit_param->name] == 0) {
					slice_map[fit_param->name] = new PndLmdLumiHelper::lmd_graph();
				}
				slice_map[fit_param->name]->data.push_back(
						std::make_pair(
								util_lmd_graphs[i]->remaining_dependencies.begin()->second,
								*fit_param));
				slice_map[fit_param->name]->parameter_name_stack =
						util_lmd_graphs[i]->parameter_name_stack;
				slice_map[fit_param->name]->parameter_name_stack[util_lmd_graphs[i]->parameter_name_stack.size()
						+ 1] = fit_param->name;
				slice_map[fit_param->name]->fit_options =
						util_lmd_graphs[i]->fit_options;
				slice_map[fit_param->name]->dependency =
						util_lmd_graphs[i]->remaining_dependencies.begin()->first;

			}
			if (slice_map[std::make_pair(fit_parameters.begin()->name.first, "chi2")]
					== 0) {
				slice_map[std::make_pair(fit_parameters.begin()->name.first, "chi2")] =
						new PndLmdLumiHelper::lmd_graph();
			}

			slice_map[std::make_pair(fit_parameters.begin()->name.first, "chi2")]->data.push_back(
					std::make_pair(
							util_lmd_graphs[i]->remaining_dependencies.begin()->second,
							ModelStructs::minimization_parameter(
									std::make_pair(fit_parameters.begin()->name.first, "chi2"),
									util_lmd_graphs[i]->fit_result->getFinalEstimatorValue(),
									0.0)));
			slice_map[std::make_pair(fit_parameters.begin()->name.first, "chi2")]->parameter_name_stack =
					util_lmd_graphs[i]->parameter_name_stack;
			slice_map[std::make_pair(fit_parameters.begin()->name.first, "chi2")]->parameter_name_stack[util_lmd_graphs[i]->parameter_name_stack.size()
					+ 1] = std::make_pair(fit_parameters.begin()->name.first, "chi2");
			slice_map[std::make_pair(fit_parameters.begin()->name.first, "chi2")]->fit_options =
					util_lmd_graphs[i]->fit_options;
			slice_map[std::make_pair(fit_parameters.begin()->name.first, "chi2")]->dependency =
					util_lmd_graphs[i]->remaining_dependencies.begin()->first;

		}
	}

	// go through map and make TGraph objects
	for (std::map<std::pair<std::string, std::string>
			, PndLmdLumiHelper::lmd_graph* , ModelStructs::stringpair_comp>::iterator graph =
			slice_map.begin(); graph != slice_map.end(); graph++) {

		graph->second->graph = new TGraphErrors(graph->second->data.size());
		for (unsigned int i = 0; i < graph->second->data.size(); i++) {
			graph->second->graph->SetPoint(i, graph->second->data[i].first,
					graph->second->data[i].second.value);
			graph->second->graph->SetPointError(i, 0.0,
					graph->second->data[i].second.error);
		}
	}

	//construct return vector
	std::vector<PndLmdLumiHelper::lmd_graph*> return_vec;
	for (std::map<std::pair<std::string, std::string>
			, PndLmdLumiHelper::lmd_graph* , ModelStructs::stringpair_comp>::iterator graph =
			slice_map.begin(); graph != slice_map.end(); graph++) {
		return_vec.push_back(graph->second);
	}

	return return_vec;
}

std::vector<PndLmdLumiHelper::lmd_graph*> PndLmdLumiHelper::filterLmdGraphs(
		std::vector<PndLmdLumiHelper::lmd_graph*> &util_lmd_graphs, TString dep) {
	std::vector<PndLmdLumiHelper::lmd_graph*> return_vec;

	for (unsigned int i = 0; i < util_lmd_graphs.size(); i++) {
		if (util_lmd_graphs[i]->dependency.EqualTo(dep)) {
			return_vec.push_back(util_lmd_graphs[i]);
		}
	}
	std::cout << "shrunk from " << util_lmd_graphs.size() << " to "
			<< return_vec.size() << std::endl;
	return return_vec;
}

void PndLmdLumiHelper::fitParametrizationModelToData(
		PndLmdLumiHelper::lmd_graph *graph) {
	PndLmdModelFactory model_factory; // construct model factory
// specify which of type of smearing model we want to generate

// generate the model
	shared_ptr<Model> model = model_factory.generate1DResolutionModel(
			graph->fit_options);

// get the model that we have to fit to the data
	for (std::map<unsigned int, std::pair<std::string, std::string> >::const_iterator parameter_name =
			graph->parameter_name_stack.begin();
			parameter_name != graph->parameter_name_stack.end(); parameter_name++) {
		ParametrizationProxy par_proxy =
				model->getModelParameterHandler().getParametrizationProxyForModelParameter(
						parameter_name->second.second);
		if (par_proxy.hasParametrizationModel()) {
			model = par_proxy.getParametrizationModel()->getModel();
		} else {
			std::cout
					<< "ERROR: Not able to obtain parametrization model for parameter "
					<< parameter_name->second.second << "!" << std::endl;
			return;
		}
	}
	model->getModelParameterSet().getModelParameter("poly_poly_factor_0")->setValue(
			graph->graph->GetMean(2));
	model->getModelParameterSet().getModelParameter("poly_poly_factor_0")->setParameterFixed(
			false);
	model->getModelParameterSet().getModelParameter("poly_poly_factor_1")->setValue(
			1.0);
	model->getModelParameterSet().getModelParameter("poly_poly_factor_1")->setParameterFixed(
			false);

	model->getModelParameterSet().printInfo();

// create chi2 estimator
	shared_ptr<Chi2Estimator> chi2_est(new Chi2Estimator());

// set the resolution model as the active model of the fitter
	chi2_est->setModel(model);

// create and set data
	ROOTDataHelper data_helper;
	shared_ptr<Data> model_data(new Data(1));
	data_helper.fillBinnedData(model_data, graph->graph);
	chi2_est->setData(model_data);

// create estimator options

	EstimatorOptions est_opt;
	std::pair<double, double> fit_range = std::make_pair(
			graph->fit_options->getPrimaryDimensionFitRange().getRangeLow(),
			graph->fit_options->getPrimaryDimensionFitRange().getRangeHigh());
	est_opt.setFitRangeX(fit_range);
	est_opt.setWithIntegralScaling(true);

// apply estimator options
	chi2_est->applyEstimatorOptions(est_opt);

// now fit the resolution model to the data
// create Minuit Fitter
	ROOTMinimizer fitter;
	fitter.setControlParameter(chi2_est);

	int fit_status = fitter.doMinimization();
	std::cout << "fit status: " << fit_status << std::endl;
// call minimization procedure
	if (fit_status) {
		std::cout << "ERROR: Problem while performing fit. Returning NULL pointer!"
				<< std::endl;
		return;
	}

// store fit results
	ModelFitResult fit_result = fitter.createModelFitResult();
	fit_result.setFitStatus(fit_status);

	graph->fit_result = new ModelFitResult(fit_result);
}

void PndLmdLumiHelper::fitParametrizationModelToGraphs(
		std::vector<PndLmdLumiHelper::lmd_graph*> &lmd_graphs) {
// perform fits
	for (unsigned int graph_index = 0; graph_index < lmd_graphs.size();
			graph_index++) {
// TODO: we need some kind of check at which stage we are at and which graphs should be fitted...
// or if they exist at all...
		if (lmd_graphs[graph_index]->dependency.EqualTo("theta") == 1) {
			fitParametrizationModelToData(lmd_graphs[graph_index]);
		}
	}
}

std::map<double, ModelFitResult*> PndLmdLumiHelper::checkFitParameters(
		const std::map<PndLmdResolution*, ModelFitResult*> &fit_results) const {
// remove all points that are bogus
	std::map<double, ModelFitResult*> return_map;
// loop over all entries of the map
	for (std::map<PndLmdResolution*, ModelFitResult*>::const_iterator it =
			fit_results.begin(); it != fit_results.end(); it++) {
		const std::set<ModelStructs::minimization_parameter> &fit_parameters =
				it->second->getFitParameters();
		bool add = true;
		for (std::set<ModelStructs::minimization_parameter>::const_iterator fit_param =
				fit_parameters.begin(); fit_param != fit_parameters.end();
				fit_param++) {
			/*if (fit_param->name.find("red_chi2") < fit_param->name.size()) {
			 if (std::fabs(fit_param->value) > 1.35) {
			 add = false;
			 break;
			 }
			 }
			 if (fit_param->name.find("ratio_narrow_wide") < fit_param->name.size()) {
			 std::cout << fit_param->value << std::endl;
			 if (std::fabs(fit_param->value) < 0.0) {
			 add = false;
			 break;
			 }
			 }*/
		}
		if (add) {
			return_map.insert(
					std::make_pair(
							it->first->getPrimarySelectionDimension().dimension_range.getDimensionMean(),
							it->second));
		}
	}
	return return_map;
}

int PndLmdLumiHelper::initResolutionParametrizationFromFile(TFile *f,
		shared_ptr<Model> resolution_model) {
	// TODO: atm this only works for the 1D case
	std::vector<PndLmdLumiHelper::lmd_graph*> allgraphs =
			getResolutionModelResultsFromFile(f);
	std::vector<PndLmdLumiHelper::lmd_graph*> graphs = filterLmdGraphs(allgraphs,
			"theta");

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
	return 0;
}
