/*
 * This is an app that produces plots from the luminosity fitting procedure
 * General information about the individual classes of the LmdFit framework can be
 * found in the doxygen manual
 * Run it like this:
 * 
 * ./makeLumiFitPlots dir_to_elastic_scattering_data
 *
 * note: the dir_to_elastic_scattering_data should contain the fit_result.root root
 * file that contains the data and fit result on which fit were performed...
 *
 * The usage of this macro is straight forward and uses primarily the
 * PndLmdResultPlotter class (which is a helper class for creating plots on luminosity
 * fits).
 * -First you read in a PndLmdAngularData object that was saved by the runLumi6Fit.C macro
 * -Then you create so graph bundles that contain all the information of the fit
 *  results and data in form of ROOT objects.
 * -Finally you can pass these bundles to functions of the plotter and generate
 *  different type of plots
 */

#include "PndLmdPlotter.h"
#include "data/PndLmdAngularData.h"
#include "data/PndLmdAcceptance.h"
#include "data/PndLmdDataFacade.h"

#include <vector>
#include <map>
#include <iostream>
#include <iterator>
#include <sstream>

#include "boost/filesystem.hpp"
#include "boost/regex.hpp"

#include "TGaxis.h"

void plotLumiFitResults(std::vector<std::string> paths,
		TString filename_suffix) {
	std::cout << "Generating lumi plots for fit results....\n";

// A small helper class that helps to construct lmd data objects
	PndLmdDataFacade lmd_data_facade;

	LumiFit::PndLmdPlotter lmd_plotter;

	lmd_plotter.primary_dimension_plot_range.range_low = 0.5;
	lmd_plotter.primary_dimension_plot_range.range_high = 16.0;

// ================================ BEGIN CONFIG ================================ //
// PndLmdResultPlotter sets default pad margins etc that should be fine for most cases
// you can fine tune it and overwrite the default values
	gStyle->SetPadRightMargin(0.18);
	gStyle->SetPadLeftMargin(0.115);
	gStyle->SetPadBottomMargin(0.12);
	gStyle->SetPadColor(10);
	gStyle->SetCanvasColor(10);
	gStyle->SetStatColor(10);

	TGaxis::SetMaxDigits(3);
	gStyle->SetOptStat(0);
	gStyle->SetOptFit(0);

// overwrite the default theta plot range if possible
//plotter.setThetaPlotRange(0.5, 16.0);
// ================================= END CONFIG ================================= //

// create and fill data map first of all
	std::map<LumiFit::LmdSimIPParameters, std::vector<PndLmdAngularData> > data_map;

	for (unsigned int i = 0; i < paths.size(); i++) {
		// ------ get files -------------------------------------------------------
		/*	std::vector<std::string> file_paths = lmd_data_facade.findFilesByName(
		 paths[i], "merge_data", "lmd_fitted_data.*.root");*/
		std::vector<std::string> file_paths = lmd_data_facade.findFilesByName(
				paths[i], "merge_data", "lmd_fitted_data.root");

		for (unsigned int j = 0; j < file_paths.size(); j++) {
			std::string fullpath = file_paths[j];
			TFile *fdata = new TFile(fullpath.c_str(), "READ");

			// get simulation ip distribution properties
			LumiFit::LmdSimIPParameters true_ip_values =
					lmd_data_facade.readSimulationIPParameters(
							(boost::filesystem::path(paths[i]).branch_path().branch_path().branch_path()).string());

			// read in data from a root file which will return a map of PndLmdAngularData objects
			std::vector<PndLmdAngularData> data_vec = lmd_data_facade.getDataFromFile<
					PndLmdAngularData>(fdata);

			// append all data objects to the end of the corresponding data map vectors
			data_map[true_ip_values].insert(data_map[true_ip_values].end(),
					data_vec.begin(), data_vec.end());
		}
	}

	// =============================== BEGIN PLOTTING =============================== //

	std::stringstream basepath;
	basepath << std::getenv("HOME") << "/plots";

	std::map<LumiFit::LmdSimIPParameters, PndLmdAngularData> reco_data_ip_map;

	// now loop over this map and create plots for each ip parameter setting
	std::map<LumiFit::LmdSimIPParameters, std::vector<PndLmdAngularData> >::iterator data_map_iter;
	for (data_map_iter = data_map.begin(); data_map_iter != data_map.end();
			data_map_iter++) {
		std::vector<PndLmdAngularData> data_vec = data_map_iter->second;
		// group data into same selections
		// and make sure that one selection is of the phi slice type
		std::map<LumiFit::LmdDimension, std::vector<PndLmdAngularData> > phi_slice_map;

		std::vector<PndLmdAngularData> full_phi_vec;

		for (unsigned int j = 0; j < data_vec.size(); j++) {
			bool found_phi_selection = false;
			const std::set<LumiFit::LmdDimension> &selection_set =
					data_vec[j].getSelectorSet();
			std::set<LumiFit::LmdDimension>::const_iterator selection_set_it;
			for (selection_set_it = selection_set.begin();
					selection_set_it != selection_set.end(); selection_set_it++) {
				if (selection_set_it->dimension_options.dimension_type
						== LumiFit::PHI_FIRST_LMD_PLANE) {
					phi_slice_map[*selection_set_it].push_back(data_vec[j]);
					found_phi_selection = true;
					break;
				}
			}
			if (!found_phi_selection) {
				full_phi_vec.push_back(data_vec[j]);
			}
		}

		if (full_phi_vec.size() > 0) {
			std::stringstream filepath_base;
			filepath_base << basepath.str() << "/";
			filepath_base << "plab_" << full_phi_vec[0].getLabMomentum() << "/";
			filepath_base << data_map_iter->first.getLabel();

			boost::filesystem::create_directories(filepath_base.str());

			// ---------- reco -- full phi stuff
			LumiFit::LmdDimensionOptions lmd_dim_opt;
			lmd_dim_opt.track_type = LumiFit::RECO;

			LumiFit::Comparisons::data_primary_dimension_options_filter filter(
					lmd_dim_opt);
			std::vector<PndLmdAngularData> full_phi_reco_data_vec =
					lmd_data_facade.filterData(full_phi_vec, filter);

			for (unsigned int reco_data_obj_index = 0;
					reco_data_obj_index < full_phi_reco_data_vec.size();
					reco_data_obj_index++) {

				if (full_phi_reco_data_vec[reco_data_obj_index].getSelectorSet().size()
						> 0)
					continue;

				if (reco_data_ip_map.find(data_map_iter->first)
						== reco_data_ip_map.end())
					reco_data_ip_map[data_map_iter->first] =
							full_phi_reco_data_vec[reco_data_obj_index];
			}

			LumiFit::PndLmdFitModelOptions model_op;
			model_op.acceptance_correction_active = true;
			model_op.resolution_smearing_active = true;

			std::stringstream filepath;
			TCanvas c("", "", 1000, 700);

			if (false) {
				NeatPlotting::PlotBundle plot_bundle =
						lmd_plotter.createLowerFitRangeDependencyPlotBundle(
								full_phi_reco_data_vec, model_op);

				filepath << filepath_base.str()
						<< "/reco_lower_fit_range_dependency.pdf";

				NeatPlotting::PlotStyle plot_style;
				plot_bundle.drawOnCurrentPad(plot_style);
				c.SaveAs(filepath.str().c_str());

				// create a vector of graph bundles (one entry for each fit option)

				filepath.str("");
				filepath << filepath_base.str() << "/fit_result_overview_booky.pdf";

				NeatPlotting::Booky booky = lmd_plotter.makeLumiFitResultOverviewBooky(
						full_phi_vec);
				booky.createBooky(filepath.str());

				// create single plots
				for (unsigned int reco_data_obj_index = 0;
						reco_data_obj_index < full_phi_reco_data_vec.size();
						reco_data_obj_index++) {

					if (full_phi_reco_data_vec[reco_data_obj_index].getSelectorSet().size()
							> 0)
						continue;

					const map<PndLmdLumiFitOptions, PndLmdLumiFitResult>& fit_results =
							full_phi_reco_data_vec[reco_data_obj_index].getFitResults();
					if (fit_results.size() > 0) {
						NeatPlotting::PlotStyle single_plot_style;

						c.Clear();
						NeatPlotting::PlotBundle reco_plot_bundle =
								lmd_plotter.makeGraphBundle1D(full_phi_reco_data_vec[0],
										fit_results.begin()->first);
						reco_plot_bundle.drawOnCurrentPad(single_plot_style);
						filepath.str("");
						filepath << filepath_base.str() << "/fit_result_reco.pdf";
						c.SaveAs(filepath.str().c_str());

						// save additional root file for tdr...
						TGraph* grmodel = reco_plot_bundle.getGraphs()[0].data_object;
						TH1D* histdata =
								(TH1D*) reco_plot_bundle.getHistograms()[0].data_object;
						filepath.str("");
						filepath << filepath_base.str() << "/fit_result_reco.root";
						TDirectory *curdir = gDirectory;
						TFile *ftemp = new TFile(filepath.str().c_str(), "RECREATE");
						grmodel->Write("model");
						histdata->Write("data");
						TNamed label("reldiff_label",
								reco_plot_bundle.plot_decoration.labels[reco_plot_bundle.plot_decoration.labels.size()
										- 1].getTitle());
						label.Write();
						ftemp->Write();
						ftemp->Close();
						if (curdir)
							curdir->cd();

						NeatPlotting::PlotBundle acc_bundle_1d =
								lmd_plotter.makeAcceptanceBundle1D(full_phi_reco_data_vec[0],
										fit_results.begin()->first);
						acc_bundle_1d.drawOnCurrentPad(single_plot_style);
						filepath.str("");
						filepath << filepath_base.str() << "/acceptance1d.pdf";
						c.SaveAs(filepath.str().c_str());

						NeatPlotting::PlotBundle acc_bundle_2d =
								lmd_plotter.makeAcceptanceBundle2D(full_phi_reco_data_vec[0],
										fit_results.begin()->first);
						acc_bundle_2d.plot_axis.x_axis_range.active = true;
						acc_bundle_2d.plot_axis.x_axis_range.low = 0.002;
						acc_bundle_2d.plot_axis.x_axis_range.high = 0.01;
						acc_bundle_2d.drawOnCurrentPad(single_plot_style);
						filepath.str("");
						filepath << filepath_base.str() << "/acceptance2d.png";
						c.SaveAs(filepath.str().c_str());
					}
				}

				lmd_dim_opt.track_type = LumiFit::MC;

				LumiFit::Comparisons::data_primary_dimension_options_filter filter_mc(
						lmd_dim_opt);
				std::vector<PndLmdAngularData> full_phi_mc_data_vec =
						lmd_data_facade.filterData(full_phi_vec, filter_mc);

				if (full_phi_mc_data_vec.size() == 1) {
					const map<PndLmdLumiFitOptions, PndLmdLumiFitResult>& fit_results =
							full_phi_mc_data_vec[0].getFitResults();
					if (fit_results.size() > 0) {
						NeatPlotting::PlotStyle single_plot_style;

						c.Clear();
						NeatPlotting::PlotBundle mc_plot_bundle =
								lmd_plotter.makeGraphBundle1D(full_phi_mc_data_vec[0],
										fit_results.begin()->first);
						mc_plot_bundle.drawOnCurrentPad(single_plot_style);
						filepath.str("");
						filepath << filepath_base.str() << "/fit_result_mc.pdf";
						c.SaveAs(filepath.str().c_str());
					}
				}

				std::cout << "mc accepted case..." << std::endl;
				lmd_dim_opt.track_type = LumiFit::MC_ACC;

				LumiFit::Comparisons::data_primary_dimension_options_filter filter_mc_acc(
						lmd_dim_opt);
				std::vector<PndLmdAngularData> full_phi_mc_acc_data_vec =
						lmd_data_facade.filterData(full_phi_vec, filter_mc_acc);

				if (full_phi_mc_acc_data_vec.size() == 1) {
					const map<PndLmdLumiFitOptions, PndLmdLumiFitResult>& fit_results =
							full_phi_mc_acc_data_vec[0].getFitResults();

					if (fit_results.size() > 0) {
						NeatPlotting::PlotStyle single_plot_style;

						c.Clear();
						NeatPlotting::PlotBundle mc_acc_plot_bundle =
								lmd_plotter.makeGraphBundle1D(full_phi_mc_acc_data_vec[0],
										fit_results.begin()->first);
						mc_acc_plot_bundle.drawOnCurrentPad(single_plot_style);
						filepath.str("");
						filepath << filepath_base.str() << "/fit_result_mc_acc.pdf";
						c.SaveAs(filepath.str().c_str());

						// different acceptances overview plot
						TCanvas acc_systematic_canvas;

						NeatPlotting::GraphAndHistogramHelper gh_helper;
						std::vector<NeatPlotting::GraphPoint> graph_data;

						map<PndLmdLumiFitOptions, PndLmdLumiFitResult>::const_iterator fit_result;
						for (fit_result = fit_results.begin();
								fit_result != fit_results.end(); fit_result++) {
							std::pair<double, double> lumi = lmd_plotter.calulateLumiRelDiff(
									fit_result->second.getLuminosity(),
									fit_result->second.getLuminosityError(),
									full_phi_mc_acc_data_vec[0].getReferenceLuminosity());
							NeatPlotting::GraphPoint gp;
							gp.x =
									fit_result->first.getFitModelOptions().acceptance->getPrimaryDimension().bin_size;
							gp.y = lumi.first;
							gp.y_err_high = lumi.second;
							gp.y_err_low = lumi.second;
							graph_data.push_back(gp);
						}

						TGraphAsymmErrors* graph = gh_helper.makeGraph(graph_data);
						graph->Draw("A*");
						filepath.str("");
						filepath << filepath_base.str() << "/lumi_systematic_acc.pdf";
						acc_systematic_canvas.SaveAs(filepath.str().c_str());
					}
				}

			}

			// testing 2d fit result plotting
			if (true) {
				filepath.str("");
				filepath << filepath_base.str() << "/fit_result_overview_booky_2d.pdf";

				NeatPlotting::Booky booky2 = lmd_plotter.create2DFitResultPlots(
						full_phi_vec);
				booky2.createBooky(filepath.str());
			}
			// ================================ END PLOTTING ================================ //
		}
	}

	if (reco_data_ip_map.size() > 1) {
		std::stringstream filename;
		filename << basepath.str() << "/";
		filename << "plab_" << reco_data_ip_map.begin()->second.getLabMomentum()
				<< "/lumifit_result_ip_overview.pdf";

		NeatPlotting::PlotBundle bundle = lmd_plotter.makeXYOverviewHistogram(
				reco_data_ip_map);

		TCanvas c;
		NeatPlotting::PlotStyle plot_style;
		plot_style.palette_color_style = 0;
		plot_style.z_axis_style.log_scale = true;
		bundle.drawOnCurrentPad(plot_style);
		c.SaveAs(filename.str().c_str());
	}
}

void displayInfo() {
// display info
	std::cout << "Required arguments are: " << std::endl;
	std::cout << "list of directories to be scanned for vertex data" << std::endl;
	std::cout << "Optional arguments are: " << std::endl;
	std::cout << "-f [output filename suffix]" << std::endl;
}

int main(int argc, char* argv[]) {
	bool is_filename_suffix_set = false;
	std::string filename_suffix("fitresults");

	int c;

	while ((c = getopt(argc, argv, "hf:")) != -1) {
		switch (c) {
			case 'f':
				filename_suffix = optarg;
				is_filename_suffix_set = true;
				break;
			case '?':
				if (optopt == 'f')
					std::cerr << "Option -" << optopt << " requires an argument."
							<< std::endl;
				else if (isprint(optopt))
					std::cerr << "Unknown option -" << optopt << "." << std::endl;
				else
					std::cerr << "Unknown option character" << optopt << "." << std::endl;
				return 1;
			case 'h':
				displayInfo();
				return 1;
			default:
				return 1;
		}
	}

	int argoffset = optind;

	if (argc > 1) {
		std::vector<std::string> paths;
		for (unsigned int i = argoffset; i < argc; i++) {
			paths.push_back(std::string(argv[i]));
		}
		plotLumiFitResults(paths, filename_suffix);
	}

	return 0;
}
