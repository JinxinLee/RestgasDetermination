/*
 * PndLmdLumiHelper.cxx
 *
 *  Created on: Jun 27, 2012
 *      Author: steve
 */

#include "PndLmdLumiHelper.h"
#include "fit/data/ROOT/ROOTDataHelper.h"
#include "fit/data/Data.h"
#include "fit/estimatorImpl/Chi2Estimator.h"
#include "fit/minimizerImpl/ROOT/ROOTMinimizer.h"
#include "data/PndLmdHistogramData.h"
#include "fit/PndLmdLumiFitOptions.h"
#include "model/PndLmdModelFactory.h"
#include "model/PndLmdDPMAngModel1D.h"
#include "model/PndLmdDPMModelParametrization.h"
#include "fit/PndLmdLumiFitResult.h"
#include "fit/PndLmdFitFacade.h"
#include "data/PndLmdAngularData.h"

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
	PndLmdDPMAngModel1D model("dpm_angular_1d", LumiFit::ALL);
	shared_ptr<Parametrization> para(
			new PndLmdDPMModelParametrization(model.getModelParameterSet()));
	model.getModelParameterHandler().registerParametrizations(
			model.getModelParameterSet(), para);
	model.getModelParameterSet().setModelParameterValue("p_lab", plab);
	((Model1D*) &model)->init();
	return -model.getMomentumTransferFromTheta(theta);
}

double PndLmdLumiHelper::calcHistIntegral(const TH1D* hist,
		std::vector<DataStructs::DimensionRange> range) {
	// just determine the bin range over which we have to calculate
	if (range.size() < 1)
		return 0.0;
	int bin_low = 1; // first bin is underflow
	int bin_high = hist->GetNbinsX();
	for (int i = 0; i < hist->GetNbinsX(); i++) {
		if (hist->GetBinCenter(i) - hist->GetBinWidth(i) / 2 < range[0].range_low
				&& hist->GetBinCenter(i) + hist->GetBinWidth(i) / 2
						> range[0].range_low) {
			bin_low = i;
		}
		if (hist->GetBinCenter(i) - hist->GetBinWidth(i) / 2 < range[0].range_high
				&& hist->GetBinCenter(i) + hist->GetBinWidth(i) / 2
						> range[0].range_high) {
			bin_high = i;
		}
	}
	return hist->Integral(bin_low, bin_high, "width");
}

double PndLmdLumiHelper::calcHistIntegral(const TH2D* hist,
		std::vector<DataStructs::DimensionRange> range) {
	// just determine the bin range over which we have to calculate
	if (range.size() < 2)
		return 0.0;
	int bin_low_x = 1; // first bin is underflow
	int bin_high_x = hist->GetNbinsX();
	for (int i = 0; i < hist->GetNbinsX(); i++) {
		if (hist->GetXaxis()->GetBinCenter(i) - hist->GetXaxis()->GetBinWidth(i) / 2
				< range[0].range_low
				&& hist->GetXaxis()->GetBinCenter(i)
						+ hist->GetXaxis()->GetBinWidth(i) / 2 > range[0].range_low) {
			bin_low_x = i;
		}
		if (hist->GetXaxis()->GetBinCenter(i) - hist->GetXaxis()->GetBinWidth(i) / 2
				< range[0].range_high
				&& hist->GetXaxis()->GetBinCenter(i)
						+ hist->GetXaxis()->GetBinWidth(i) / 2 > range[0].range_high) {
			bin_high_x = i;
		}
	}
	int bin_low_y = 1; // first bin is underflow
	int bin_high_y = hist->GetNbinsY();
	for (int i = 0; i < hist->GetNbinsY(); i++) {
		if (hist->GetYaxis()->GetBinCenter(i) - hist->GetYaxis()->GetBinWidth(i) / 2
				< range[1].range_low
				&& hist->GetYaxis()->GetBinCenter(i)
						+ hist->GetYaxis()->GetBinWidth(i) / 2 > range[1].range_low) {
			bin_low_y = i;
		}
		if (hist->GetYaxis()->GetBinCenter(i) - hist->GetYaxis()->GetBinWidth(i) / 2
				< range[1].range_high
				&& hist->GetYaxis()->GetBinCenter(i)
						+ hist->GetYaxis()->GetBinWidth(i) / 2 > range[1].range_high) {
			bin_high_y = i;
		}
	}
	return hist->Integral(bin_low_x, bin_high_x, bin_low_y, bin_high_y, "width");
}

std::vector<PndLmdLumiHelper::lmd_graph*> PndLmdLumiHelper::getResolutionModelResultsFromFile(
		const std::string &file_url) const {
	TFile f(file_url.c_str(), "READ");

	std::vector<PndLmdLumiHelper::lmd_graph*> return_vec;

	f.cd();
	TIter next(f.GetListOfKeys());
	PndLmdLumiHelper::lmd_graph* ptemp_lmd_graph;
	while ((ptemp_lmd_graph = (PndLmdLumiHelper::lmd_graph*) next())) {
		if (ptemp_lmd_graph) {
			f.GetObject(ptemp_lmd_graph->GetName(), ptemp_lmd_graph); // without getting it explicitly again it crashes...
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
		std::stringstream strstream;
		strstream.precision(3);
		strstream << graph_vec[i]->getDependencyString();
		for (std::map<unsigned int, std::pair<std::string, std::string> >::const_iterator j =
				graph_vec[i]->parameter_name_stack.begin();
				j != graph_vec[i]->parameter_name_stack.end(); j++) {
			strstream << "_" << j->second.first << "-" << j->second.second;
		}
		for (std::set<LumiFit::LmdDimension>::iterator dependency =
				graph_vec[i]->remaining_selections.begin();
				dependency != graph_vec[i]->remaining_selections.end(); dependency++) {
			strstream << "_" << *dependency;
		}
		graph_vec[i]->Write(strstream.str().c_str());
	}
}

void PndLmdLumiHelper::finalizeLmdGraphObjects(
		std::map<std::set<LumiFit::LmdDimension>,
				std::map<std::pair<std::string, std::string>,
						PndLmdLumiHelper::lmd_graph*, ModelStructs::stringpair_comp> > &graph_map) {

	bool is_dependent = (graph_map.size() > 0);
	bool make_absolute;
	for (std::map<std::set<LumiFit::LmdDimension>,
			std::map<std::pair<std::string, std::string>,
					PndLmdLumiHelper::lmd_graph*, ModelStructs::stringpair_comp> >::iterator graphs =
			graph_map.begin(); graphs != graph_map.end(); graphs++) {
		make_absolute = false;
		for (std::map<std::pair<std::string, std::string>,
				PndLmdLumiHelper::lmd_graph*, ModelStructs::stringpair_comp>::iterator graph =
				graphs->second.begin(); graph != graphs->second.end(); graph++) {

			if (graph->first.second.find("sigma") != -1)
				make_absolute = true;

			graph->second->remaining_selections = graphs->first;

			graph->second->graph = new TGraphErrors(graph->second->data.size());
			graph->second->graph2d = new TGraph2DErrors(graph->second->data.size());

			for (unsigned int i = 0; i < graph->second->data.size(); i++) {
				lmd_graph_data_point dp = graph->second->data[i];
				double value = dp.parameter.value;
				if (make_absolute)
					value = fabs(value);
				graph->second->graph->SetPoint(i, dp.theta, value);
				graph->second->graph->SetPointError(i, 0.0, dp.parameter.error);

				graph->second->graph2d->SetPoint(i, dp.theta, dp.phi, value);
				graph->second->graph2d->SetPointError(i, 0.0, 0.0, dp.parameter.error);
			}
		}
	}
}

double PndLmdLumiHelper::calculateBinOverlap(bin_dimension &reco_bin,
		bin_dimension &reso_bin) {
	bin_dimension overlap;

	if (reco_bin.x_range.range_low > reso_bin.x_range.range_low)
		overlap.x_range.range_low = reco_bin.x_range.range_low;
	else
		overlap.x_range.range_low = reso_bin.x_range.range_low;

	if (reco_bin.y_range.range_low > reso_bin.y_range.range_low)
		overlap.y_range.range_low = reco_bin.y_range.range_low;
	else
		overlap.y_range.range_low = reso_bin.y_range.range_low;

	if (reco_bin.x_range.range_high < reso_bin.x_range.range_high)
		overlap.x_range.range_high = reco_bin.x_range.range_high;
	else
		overlap.x_range.range_high = reso_bin.x_range.range_high;

	if (reco_bin.y_range.range_high < reso_bin.y_range.range_high)
		overlap.y_range.range_high = reco_bin.y_range.range_high;
	else
		overlap.y_range.range_high = reso_bin.y_range.range_high;

	double reso_bin_area = (reso_bin.x_range.range_high
			- reso_bin.x_range.range_low)
			* (reso_bin.y_range.range_high - reso_bin.y_range.range_low);

	//reco_bin.printInfo();
	//reso_bin.printInfo();
	//overlap.printInfo();

	return (overlap.x_range.range_high - overlap.x_range.range_low)
			* (overlap.y_range.range_high - overlap.y_range.range_low) / reso_bin_area;
}

std::map<std::pair<double, double>,
		std::map<PndLmdLumiHelper::bin_dimension, double> > PndLmdLumiHelper::generate2DResolutionMap(
		const std::vector<PndLmdHistogramData>& resolutions,
		const PndLmdAngularData &data) {
	std::map<std::pair<double, double>,
			std::map<PndLmdLumiHelper::bin_dimension, double> > resolution_map;

	TH2D* recohist2d = data.get2DHistogram();

	int prim_dim_bins = data.getPrimaryDimension().bins;
	double prim_dim_binsize = data.getPrimaryDimension().bin_size;
	double prim_dim_start = data.getPrimaryDimension().dimension_range.getRangeLow();

	int sec_dim_bins = data.getSecondaryDimension().bins;
	double sec_dim_binsize = data.getSecondaryDimension().bin_size;
	double sec_dim_start = data.getSecondaryDimension().dimension_range.getRangeLow();

	double binning_factor = 1 / sec_dim_binsize / prim_dim_binsize;

	// loop over all resolution objects
	for (unsigned int i = 0; i < resolutions.size(); i++) {
		//std::cout << "processing resolution " << i << std::endl;
		if (resolutions[i].getSecondaryDimension().is_active) {
			// try to find the selction dimension (mc values)
			LumiFit::LmdDimension prim_dim;
			LumiFit::LmdDimension sec_dim;

			bool found_primary_selection(false);
			bool found_secondary_selection(false);

			const std::set<LumiFit::LmdDimension>& selection_set =
					resolutions[i].getSelectorSet();
			std::set<LumiFit::LmdDimension>::const_iterator selection_set_it;
			for (selection_set_it = selection_set.begin();
					selection_set_it != selection_set.end(); selection_set_it++) {
				if (selection_set_it->dimension_options.dimension_type
						== data.getPrimaryDimension().dimension_options.dimension_type) {
					prim_dim = *selection_set_it;
					found_primary_selection = true;
				} else if (selection_set_it->dimension_options.dimension_type
						== data.getSecondaryDimension().dimension_options.dimension_type) {
					sec_dim = *selection_set_it;
					found_secondary_selection = true;
				}
			}

			if (found_primary_selection && found_secondary_selection) {
				// ok we found the selection dimensions
				TH2D* hist2d = resolutions[i].get2DHistogram();
				// calculated normalization integral
				double hist_integral = hist2d->Integral();
				if (hist_integral <= 0.0)
					continue;

				double total_smearing_probability = 0.0;
				double total_smearing_probability_noweight = 0.0;
				int counter = 0;

				// loop over the resolution histogram bins
				for (unsigned int ibinx = 1; ibinx < hist2d->GetNbinsX() - 1; ibinx++) {
					for (unsigned int ibiny = 1; ibiny < hist2d->GetNbinsY() - 1;
							ibiny++) {
						// ok so now we just need to find the reco bins (there could be some overlap with
						// multiple reco bins) for each of the resolution bins and weight the full bin
						// probability according to the overlap
						double smearing_probability = hist2d->GetBinContent(ibinx, ibiny)
								/ hist_integral * binning_factor;

						total_smearing_probability_noweight += smearing_probability;

						if (smearing_probability > 0.0) {

							/*std::cout << "smearing probability: " << smearing_probability
									<< std::endl;*/

							bin_dimension reso_bin;
							reso_bin.x_range.range_low =
									(prim_dim.dimension_range.getDimensionMean()
											+ hist2d->GetXaxis()->GetBinLowEdge(ibinx));
							reso_bin.x_range.range_high =
									(prim_dim.dimension_range.getDimensionMean()
											+ hist2d->GetXaxis()->GetBinUpEdge(ibinx));
							reso_bin.y_range.range_low =
									(sec_dim.dimension_range.getDimensionMean()
											+ hist2d->GetYaxis()->GetBinLowEdge(ibiny));
							reso_bin.y_range.range_high =
									(sec_dim.dimension_range.getDimensionMean()
											+ hist2d->GetYaxis()->GetBinUpEdge(ibiny));

							bin_dimension reco_bin;

							// calculated boundaries
							int recbinx_min = (reso_bin.x_range.range_low - prim_dim_start)
									/ prim_dim_binsize + 1;
							int recbinx_max = (reso_bin.x_range.range_high - prim_dim_start)
									/ prim_dim_binsize + 1;
							int recbiny_min = (reso_bin.y_range.range_low - sec_dim_start)
									/ sec_dim_binsize + 1;
							int recbiny_max = (reso_bin.y_range.range_high - sec_dim_start)
									/ sec_dim_binsize + 1;

							//in case the resolution bin is further down the lower bound of the reco hist
							//just skip
							if (recbinx_max <= 0 || recbinx_min >= prim_dim_bins)
								continue;
							if (recbiny_max <= 0 || recbiny_min >= sec_dim_bins)
								continue;

							/*std::cout << theta_start << " asdf " << phi_start << std::endl;
							 std::cout << recbinx_min << " = " << reso_bin.x_min << " "
							 << theta_start << " " << theta_binsize << std::endl;
							 std::cout << recbinx_max << " = " << reso_bin.x_max << " "
							 << theta_start << " " << theta_binsize << std::endl;
							 std::cout << recbiny_min << " = " << reso_bin.y_min << " "
							 << phi_start << " " << phi_binsize << std::endl;
							 std::cout << recbiny_max << " = " << reso_bin.y_max << " "
							 << phi_start << " " << phi_binsize << std::endl;*/

							for (unsigned int irecbinx = recbinx_min; irecbinx <= recbinx_max;
									irecbinx++) {
								for (unsigned int irecbiny = recbiny_min;
										irecbiny <= recbiny_max; irecbiny++) {
									// calculate overlap with elastic data reco bins

									reco_bin.x_range.range_low =
											recohist2d->GetXaxis()->GetBinLowEdge(irecbinx);
									reco_bin.x_range.range_high =
											recohist2d->GetXaxis()->GetBinUpEdge(irecbinx);
									reco_bin.y_range.range_low =
											recohist2d->GetYaxis()->GetBinLowEdge(irecbiny);
									reco_bin.y_range.range_high =
											recohist2d->GetYaxis()->GetBinUpEdge(irecbiny);

									double weight = calculateBinOverlap(reco_bin, reso_bin);

									double prim_var_rec = recohist2d->GetXaxis()->GetBinCenter(
											irecbinx);
									double sec_var_rec = recohist2d->GetYaxis()->GetBinCenter(
											irecbiny);

									//std::cout << "weight: " << weight << std::endl;

									if (resolution_map[std::make_pair(prim_var_rec, sec_var_rec)].find(
											bin_dimension(prim_dim.dimension_range,
													sec_dim.dimension_range))
											== resolution_map[std::make_pair(prim_var_rec,
													sec_var_rec)].end()) {
										resolution_map[std::make_pair(prim_var_rec, sec_var_rec)][bin_dimension(
												prim_dim.dimension_range, sec_dim.dimension_range)] =
												weight * smearing_probability;
									} else {
										resolution_map[std::make_pair(prim_var_rec, sec_var_rec)][bin_dimension(
												prim_dim.dimension_range, sec_dim.dimension_range)] +=
												weight * smearing_probability;
									}

									total_smearing_probability += weight * smearing_probability;
									counter++;
								}
							}
						}
					}
				}
				/*std::cout << "probability for "
				 << theta_dim.dimension_range.getDimensionMean() << ", "
				 << phi_dim.dimension_range.getDimensionMean() << std::endl;
				 std::cout << "total smearing probs: " << total_smearing_probability
				 << " " << total_smearing_probability_noweight << std::endl;
				 std::cout << counter << std::endl;*/
			}
		}
	}

	/*std::map<PndLmdLumiHelper::bin_dimension, std::pair<double, int> > test_map;

	std::map<std::pair<double, double>,
			std::map<PndLmdLumiHelper::bin_dimension, double> >::iterator it;

	for (it = resolution_map.begin(); it != resolution_map.end(); it++) {
		std::map<PndLmdLumiHelper::bin_dimension, double>::iterator it2;
		for (it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			if (test_map.find(it2->first) == test_map.end())
				test_map[it2->first] = std::make_pair(it2->second, 1);
			else {
				test_map[it2->first].first += it2->second;
				test_map[it2->first].second++;
			}
		}
	}

	std::map<PndLmdLumiHelper::bin_dimension, std::pair<double, int> >::iterator it2;
	for (it2 = test_map.begin(); it2 != test_map.end(); it2++) {
		std::cout << "probability for " << it2->first.x_range.range_low << ", "
				<< it2->first.x_range.range_high << ": " << it2->second.second << " "
				<< it2->second.first << std::endl;
	}*/

	return resolution_map;
}

std::vector<PndLmdLumiHelper::lmd_graph*> PndLmdLumiHelper::generateLmdGraphs(
		std::vector<PndLmdHistogramData>& resolutions,
		DataStructs::DimensionRange& fit_range) {

	EstimatorOptions est_opt;
	est_opt.setFitRangeX(fit_range);

	std::map<std::set<LumiFit::LmdDimension>,
			std::map<std::pair<std::string, std::string>,
					PndLmdLumiHelper::lmd_graph*, ModelStructs::stringpair_comp> > selection_dim_graph_map;

	for (unsigned int i = 0; i < resolutions.size(); i++) {
		const std::map<PndLmdLumiFitOptions, PndLmdLumiFitResult> &fit_results =
				resolutions[i].getFitResults();

		std::set<LumiFit::LmdDimension> selection_dimensions =
				resolutions[i].getSelectorSet();

		// remove phi dimension
		std::set<LumiFit::LmdDimension>::iterator phi_selection = std::find_if(
				selection_dimensions.begin(), selection_dimensions.end(),
				findByDimensionType(LumiFit::PHI));

		LumiFit::LmdDimension phi_selection_dimension;
		phi_selection_dimension.dimension_range.setRangeLow(-TMath::Pi());// standard value
		phi_selection_dimension.dimension_range.setRangeHigh(TMath::Pi());// standard value
		std::set<LumiFit::LmdDimensionOptions> dependencies;

		if (phi_selection != selection_dimensions.end()) {
			phi_selection_dimension = *phi_selection;
			selection_dimensions.erase(phi_selection);
			dependencies.insert(phi_selection->dimension_options);
		}

		// remove theta dimension
		std::set<LumiFit::LmdDimension>::iterator theta_selection = std::find_if(
				selection_dimensions.begin(), selection_dimensions.end(),
				findByDimensionType(LumiFit::THETA));

		if (theta_selection != selection_dimensions.end()) {
			LumiFit::LmdDimension theta_selection_dimension = *theta_selection;
			selection_dimensions.erase(theta_selection);
			dependencies.insert(theta_selection->dimension_options);

			for (std::map<PndLmdLumiFitOptions, PndLmdLumiFitResult>::const_iterator fit_result =
					fit_results.begin(); fit_result != fit_results.end(); fit_result++) {
				// if fit was not successful, skip this fit result
				if (fit_result->second.getModelFitResult().getFitStatus() != 0)
					continue;
				const std::set<ModelStructs::minimization_parameter> &fit_parameters =
						fit_result->second.getModelFitResult().getFitParameters();

				// here we would need to filter out the bad fits
				/*if (fabs(fit_result->second->getRedChiSquare() - 1.0) > 0.5) {
				 std::cout << "chi2 = " << fit_result->second->getRedChiSquare()
				 << " ... Skipping this fit!"<<std::endl;
				 continue;
				 }*/

				//std::cout << fit_result->second->getRedChiSquare() << std::endl;
				for (std::set<ModelStructs::minimization_parameter>::const_iterator fit_param =
						fit_parameters.begin(); fit_param != fit_parameters.end();
						fit_param++) {

					if (selection_dim_graph_map[selection_dimensions][fit_param->name]
							== 0) {
						selection_dim_graph_map[selection_dimensions][fit_param->name] =
								new PndLmdLumiHelper::lmd_graph();
						selection_dim_graph_map[selection_dimensions][fit_param->name]->lab_momentum =
								resolutions[i].getLabMomentum();
					}

					selection_dim_graph_map[selection_dimensions][fit_param->name]->resolution_dimensions.push_back(
							resolutions[i].getPrimaryDimension());
					if (resolutions[i].getSecondaryDimension().is_active) {
						selection_dim_graph_map[selection_dimensions][fit_param->name]->resolution_dimensions.push_back(
								resolutions[i].getSecondaryDimension());
					}

					lmd_graph_data_point dp;
					dp.theta =
							theta_selection_dimension.dimension_range.getDimensionMean();
					dp.phi = phi_selection_dimension.dimension_range.getDimensionMean();
					dp.parameter = *fit_param;

					selection_dim_graph_map[selection_dimensions][fit_param->name]->dependencies =
							dependencies;

					selection_dim_graph_map[selection_dimensions][fit_param->name]->data.push_back(
							dp);
					selection_dim_graph_map[selection_dimensions][fit_param->name]->parameter_name_stack[1] =
							fit_param->name;
					PndLmdLumiFitOptions *fit_opt = new PndLmdLumiFitOptions(
							fit_result->first);
					fit_opt->est_opt = est_opt;
					selection_dim_graph_map[selection_dimensions][fit_param->name]->fit_options =
							fit_opt;

					if (selection_dim_graph_map[selection_dimensions][std::make_pair(
							fit_parameters.begin()->name.first, "chi2")] == 0) {
						selection_dim_graph_map[selection_dimensions][std::make_pair(
								fit_parameters.begin()->name.first, "chi2")] =
								new PndLmdLumiHelper::lmd_graph();
						selection_dim_graph_map[selection_dimensions][std::make_pair(
								fit_parameters.begin()->name.first, "chi2")]->lab_momentum =
								resolutions[i].getLabMomentum();
					}

					dp.parameter = ModelStructs::minimization_parameter(
							std::make_pair(fit_parameters.begin()->name.first, "chi2"),
							fit_result->second.getRedChiSquare(), 0.0);

					selection_dim_graph_map[selection_dimensions][std::make_pair(
							fit_parameters.begin()->name.first, "chi2")]->data.push_back(dp);
					selection_dim_graph_map[selection_dimensions][std::make_pair(
							fit_parameters.begin()->name.first, "chi2")]->parameter_name_stack[1] =
							std::make_pair(fit_parameters.begin()->name.first, "chi2");
					fit_opt = new PndLmdLumiFitOptions(fit_result->first);
					fit_opt->est_opt = est_opt;
					selection_dim_graph_map[selection_dimensions][std::make_pair(
							fit_parameters.begin()->name.first, "chi2")]->fit_options =
							fit_opt;
				}
			}
		}
	}
// go through map and make TGraph objects
	finalizeLmdGraphObjects(selection_dim_graph_map);

//construct return vector
	std::vector<PndLmdLumiHelper::lmd_graph*> return_vec;

	for (std::map<std::set<LumiFit::LmdDimension>,
			std::map<std::pair<std::string, std::string>,
					PndLmdLumiHelper::lmd_graph*, ModelStructs::stringpair_comp> >::iterator graphs =
			selection_dim_graph_map.begin(); graphs != selection_dim_graph_map.end();
			graphs++) {
		for (std::map<std::pair<std::string, std::string>,
				PndLmdLumiHelper::lmd_graph*, ModelStructs::stringpair_comp>::iterator graph =
				graphs->second.begin(); graph != graphs->second.end(); graph++) {
			return_vec.push_back(graph->second);
		}

	}
	return return_vec;
}

std::vector<PndLmdLumiHelper::lmd_graph*> PndLmdLumiHelper::filterLmdGraphs(
		std::vector<PndLmdLumiHelper::lmd_graph*> &util_lmd_graphs,
		std::set<LumiFit::LmdDimensionOptions> dep) {
	std::vector<PndLmdLumiHelper::lmd_graph*> return_vec;

	for (unsigned int i = 0; i < util_lmd_graphs.size(); i++) {
		if (util_lmd_graphs[i]->dependencies == dep) {
			return_vec.push_back(util_lmd_graphs[i]);
		}
	}
	std::cout << "shrunk from " << util_lmd_graphs.size() << " to "
			<< return_vec.size() << std::endl;
	return return_vec;
}

std::vector<PndLmdLumiHelper::lmd_graph*> PndLmdLumiHelper::filterLmdGraphs(
		const std::vector<PndLmdLumiHelper::lmd_graph*> &util_lmd_graphs,
		const std::set<LumiFit::LmdDimension> &selections) const {
	std::vector<PndLmdLumiHelper::lmd_graph*> return_vec;

	for (unsigned int i = 0; i < util_lmd_graphs.size(); i++) {
		const std::set<LumiFit::LmdDimension> lhs =
				PndLmdFitFacade::findBySelectionDimensionsIgnoringSecondaryTrackFilters::removeSecondaryTrackFilter(
						util_lmd_graphs[i]->remaining_selections);
		const std::set<LumiFit::LmdDimension> rhs =
				PndLmdFitFacade::findBySelectionDimensionsIgnoringSecondaryTrackFilters::removeSecondaryTrackFilter(
						selections);
		if (lhs == rhs) {
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
			graph->fit_options->getFitModelOptions());

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
// apply estimator options
	chi2_est->applyEstimatorOptions(graph->fit_options->getEstimatorOptions());

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
		if (lmd_graphs[graph_index]->dependencies.size() == 1) {
			fitParametrizationModelToData(lmd_graphs[graph_index]);
		}
	}
}

std::map<double, ModelFitResult*> PndLmdLumiHelper::checkFitParameters(
		const std::map<PndLmdHistogramData*, ModelFitResult*> &fit_results) const {
// remove all points that are bogus
	std::map<double, ModelFitResult*> return_map;
// loop over all entries of the map
	for (std::map<PndLmdHistogramData*, ModelFitResult*>::const_iterator it =
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
			const std::set<LumiFit::LmdDimension>& selection_dimensions =
					it->first->getSelectorSet();
			std::set<LumiFit::LmdDimension>::iterator selection = std::find_if(
					selection_dimensions.begin(), selection_dimensions.end(),
					findByDimensionType(LumiFit::THETA));

			if (selection != selection_dimensions.end()) {
				return_map.insert(
						std::make_pair(selection->dimension_range.getDimensionMean(),
								it->second));
			}
		}
	}
	return return_map;
}
