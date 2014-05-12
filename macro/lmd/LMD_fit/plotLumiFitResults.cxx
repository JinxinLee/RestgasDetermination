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

#include "PndLmdResultPlotter.h"
#include "data/PndLmdAngularData.h"
#include "data/PndLmdAcceptance.h"
#include "data/PndLmdDataFacade.h"

#include <vector>
#include <map>
#include <iostream>
#include <iterator>

#include "TString.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TFile.h"
#include "TLatex.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"

#include "boost/filesystem.hpp"

void plotLumiFitResults(TString path, TString filename_prefix = "fitresults") {
	std::cout << "Generating lumi plots for fit results....\n";

	// A small helper class that helps to construct lmd data objects
	PndLmdDataFacade lmd_data_facade;

	// create an instance of PndLmdResultPlotter the plotting helper class
	PndLmdResultPlotter plotter;

	// ================================ BEGIN CONFIG ================================ //
	// PndLmdResultPlotter sets default pad margins etc that should be fine for most cases
	// you can fine tune it and overwrite the default values
	//gStyle->SetPadTopMargin(0.06);
	//gStyle->SetPadBottomMargin(0.12);
	gStyle->SetPadLeftMargin(0.15);
	//gStyle->SetPadRightMargin(0.1);

	// overwrite the default theta plot range if possible
	//plotter.setThetaPlotRange(0.5, 16.0);

	// The plotter has more options for text positioning and tex sizes for which you can
	// overwrite the default values here
	plotter.setTextLeftPos(0.40);
	plotter.setTextTopPos(0.9);
	//plotter.setTextSpacing(0.08);
	//plotter.setTextSize(0.06);
	//plotter.setLabelSize(0.06);

	//plotter.setLabelOffsetX(0.007);
	//plotter.setLabelOffsetY(0.007);
	//plotter.setTitleOffsetX(1.0);
	//plotter.setTitleOffsetY(1.5);
	// ================================= END CONFIG ================================= //

	// ------ get files -------------------------------------------------------
	TFile *fdata = new TFile(path + "/lmd_fitted_data.root", "OPEN");

	// read in data from a root file which will return a map of pointers to PndLmdAngularData objects
	std::vector<PndLmdAngularData> data_vec = lmd_data_facade.getDataFromFile<
			PndLmdAngularData>(fdata);

	// =============================== BEGIN PLOTTING =============================== //

	// create a vector of graph bundles (one entry for each fit option)
	std::map<PndLmdLumiFitOptions,
			std::map<int, PndLmdResultPlotter::graph_bundle>,
			PndLmdResultPlotter::fit_options_compare> graph_bundle_map =
			plotter.makeGraphBundles1D(data_vec);

	// make an overview canvas for these fit specs
	plotter.makeFitResultBooky(graph_bundle_map, filename_prefix);

	plotter.setThetaPlotRange(0.0, 0.011);
	// get reco graph bundle
	PndLmdResultPlotter::graph_bundle gb = graph_bundle_map.begin()->second[6];
	TCanvas c("c", "", 1000, 700);
	plotter.fillSinglePad(&c, gb);
	c.SaveAs("lumifit_reco.pdf");

	if (false) {
		//save it as root file as well
		TFile outputfile("reco-results.root", "RECREATE");
		gb.hist1d->Write("data");
		gb.model->Write("model");
		TLatex l_plab(0.5, 0.5, gb.labels[0].first);
		l_plab.Write("plab");
		TLatex l_rdiff(0.5, 0.5, gb.labels[1].first);
		l_rdiff.Write("reldiff");
		outputfile.Close();
	}

	// ================================ END PLOTTING ================================ //
}

void makeOverviewCanvas(
		std::map<LumiFit::LmdSimIPParameters, PndLmdResultPlotter::graph_bundle> &reco_graph_map,
		TString filename, PndLmdResultPlotter &plotter, bool use_2d) {
	std::cout << "size of reco graph map: " << reco_graph_map.size() << std::endl;
	// check for different magnitudes in x and y offsets
	std::set<double> x_offsets;
	std::set<double> y_offsets;
	std::map<LumiFit::LmdSimIPParameters, PndLmdResultPlotter::graph_bundle>::iterator map_it;
	for (map_it = reco_graph_map.begin(); map_it != reco_graph_map.end();
			map_it++) {
		x_offsets.insert(map_it->first.offset_x_mean);
		y_offsets.insert(map_it->first.offset_y_mean);
	}

	std::cout << "dividing canvas into: " << x_offsets.size() << " x "
			<< y_offsets.size() << std::endl;
	TCanvas c;
	c.Divide(x_offsets.size(), y_offsets.size());
	for (map_it = reco_graph_map.begin(); map_it != reco_graph_map.end();
			map_it++) {
		unsigned int x_coord = std::distance(x_offsets.begin(),
				x_offsets.find(map_it->first.offset_x_mean)) + 1;	// add 1 because pad starts counting at zero
		unsigned int y_coord = std::distance(
				y_offsets.find(map_it->first.offset_y_mean), --y_offsets.end());

		std::stringstream ss;
		ss << "(" << map_it->first.offset_x_mean << ","
				<< map_it->first.offset_y_mean << ")";
		map_it->second.labels.push_back(std::make_pair(ss.str().c_str(), 1));

		std::cout << "filling pad " << y_coord * x_offsets.size() + x_coord
				<< std::endl;
		c.cd(y_coord * x_offsets.size() + x_coord);
		plotter.fillSinglePad(&c, map_it->second, use_2d);
	}

	c.SaveAs(filename);
}

void plotMultipleLumiFitResults(std::vector<TString> paths,
		TString filename_suffix) {
	std::cout << "Generating lumi plots for fit results....\n";

	// A small helper class that helps to construct lmd data objects
	PndLmdDataFacade lmd_data_facade;

	// create an instance of PndLmdResultPlotter the plotting helper class
	PndLmdResultPlotter plotter;

	// ================================ BEGIN CONFIG ================================ //
	// PndLmdResultPlotter sets default pad margins etc that should be fine for most cases
	// you can fine tune it and overwrite the default values
	//gStyle->SetPadTopMargin(0.06);
	//gStyle->SetPadBottomMargin(0.12);
	gStyle->SetPadLeftMargin(0.15);
	//gStyle->SetPadRightMargin(0.1);

	// overwrite the default theta plot range if possible
	//plotter.setThetaPlotRange(0.5, 16.0);

	// The plotter has more options for text positioning and tex sizes for which you can
	// overwrite the default values here
	plotter.setTextLeftPos(0.50);
	plotter.setTextTopPos(0.9);
	//plotter.setTextSpacing(0.08);
	//plotter.setTextSize(0.06);
	//plotter.setLabelSize(0.06);

	//plotter.setLabelOffsetX(0.007);
	//plotter.setLabelOffsetY(0.007);
	//plotter.setTitleOffsetX(1.0);
	//plotter.setTitleOffsetY(1.5);
	// ================================= END CONFIG ================================= //

	std::map<LumiFit::LmdSimIPParameters, PndLmdResultPlotter::graph_bundle> reco_graph_map;
	std::map<LumiFit::LmdSimIPParameters, PndLmdResultPlotter::graph_bundle> reco_resid_graph_map;
	std::map<LumiFit::LmdSimIPParameters, PndLmdResultPlotter::graph_bundle> acc2d_graph_map;

	for (unsigned int i = 0; i < paths.size(); i++) {
		// ------ get files -------------------------------------------------------
		TFile *fdata = new TFile(paths[i] + "/lmd_fitted_data.root", "READ");

		// get simulation ip distribution properties
		LumiFit::LmdSimIPParameters true_ip_values =
				lmd_data_facade.readSimulationIPParameters(
						(boost::filesystem::path(paths[i]).branch_path().branch_path()).string());

		// read in data from a root file which will return a map of pointers to PndLmdAngularData objects
		std::vector<PndLmdAngularData> data_vec = lmd_data_facade.getDataFromFile<
				PndLmdAngularData>(fdata);

		// create a vector of graph bundles (one entry for each fit option)
		std::map<PndLmdLumiFitOptions,
				std::map<int, PndLmdResultPlotter::graph_bundle>,
				PndLmdResultPlotter::fit_options_compare> graph_bundle_map =
				plotter.makeGraphBundles1D(data_vec);

		plotter.setThetaPlotRange(0.0, 0.012);
		// get reco graph bundle
		PndLmdResultPlotter::graph_bundle gb = graph_bundle_map.begin()->second[6];
		PndLmdResultPlotter::graph_bundle gb_resid =
				graph_bundle_map.begin()->second[9];
		PndLmdResultPlotter::graph_bundle gb_acc =
				graph_bundle_map.begin()->second[3];

		reco_graph_map[true_ip_values] = gb;
		reco_resid_graph_map[true_ip_values] = gb_resid;
		acc2d_graph_map[true_ip_values] = gb_acc;
	}

	TString s("lumifit_results_reco_overview");
	s += filename_suffix + ".pdf";
	makeOverviewCanvas(reco_graph_map, s, plotter, false);
	s = "lumifit_results_reco_residuals_overview";
	s += filename_suffix + ".pdf";
	makeOverviewCanvas(reco_resid_graph_map, s, plotter, false);
	s = "acc2d_overview";
	s += filename_suffix + ".pdf";
	makeOverviewCanvas(acc2d_graph_map, s, plotter, true);

	// ================================ END PLOTTING ================================ //
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
	std::string filename_suffix("");

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

	if (argc == 2) {
		plotLumiFitResults(TString(argv[1]));
	} else if (argc == 3) {
		plotLumiFitResults(TString(argv[1]), TString(argv[2]));
	} else {
		std::vector<TString> paths;
		for (unsigned int i = argoffset; i < argc; i++) {
			paths.push_back(TString(argv[i]));
		}
		plotMultipleLumiFitResults(paths, filename_suffix);
	}

	return 0;
}
