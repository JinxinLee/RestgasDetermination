#include "PndLmdLumiHelper.h"
#include "PndLmdDataFacade.h"
#include "PndLmdResolution.h"
#include "PndLmdFitFacade.h"

#include <vector>
#include <iostream>
#include <sstream>

#include "TFile.h"
#include "TString.h"
#include "TStopwatch.h"

using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::stringstream;

void determineResolution(TString input_file_dir,
		unsigned int parametrization_level, const int verboseLevel = 0) {
	cout << "Running determine resolution app....\n";

	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();
	// ------------------------------------------------------------------------

	// A small helper class that helps to construct lmddata objects
	PndLmdLumiHelper lumifit_helper;

	// The lmd data facade class that helps to construct and fill the lmd
	// resolution objects
	PndLmdDataFacade lmd_data_facade;

	// ---- Output file -------------------------------------------------------
	stringstream ss;
	ss << input_file_dir << "/resolution_params_" << parametrization_level
			<< ".root";
	TFile *outfile = new TFile(ss.str().c_str(), "RECREATE");

	switch (parametrization_level) {
		case 0: {
			// --- read in box gen data and construct lmd resolution data objects --- //
			stringstream in_ss;
			in_ss << input_file_dir << "/lmd_res_data.root";
			TFile *infile = new TFile(in_ss.str().c_str(), "READ");

			vector<PndLmdResolution> all_lmd_res = lmd_data_facade.getDataFromFile<
					PndLmdResolution>(infile);

			// filter the vector for specific options
			LumiFit::LmdDimensionOptions lmd_dim_opt;
			lmd_dim_opt.dimension_type = LumiFit::THETA;
			lmd_dim_opt.track_param_type = LumiFit::IP;

			vector<PndLmdResolution> lmd_res_vec = lmd_data_facade.filterData<
					PndLmdResolution>(all_lmd_res, lmd_dim_opt);

			// specify which of type of smearing model we want to generate
			PndLmdFitFacade lmd_fit_facade;

			LumiFit::LmdDimensionRange res_fit_range;
			res_fit_range.setUnitPrefix(DataStructs::MILLI);
			res_fit_range.setRangeLow(-2.0);
			res_fit_range.setRangeHigh(2.0);

			lmd_fit_facade.getFitOptionTemplate().setPrimaryDimensionFitRangeActive(
					true);
			lmd_fit_facade.getFitOptionTemplate().setPrimaryDimensionFitRange(
					res_fit_range);

			// now only the smearing model type has to be set
			// all other settings are not relevant for the resolution
			// 0 = single gaussian, 1 = double gaussian, 2 = asymm gaussian
			lmd_fit_facade.getFitOptionTemplate().setSmearingModelType(12);

			// run the resolution determination method on this data
			lmd_fit_facade.fitSmearingModelToResolutions(lmd_res_vec);

			// save stuff to file
			outfile->cd();
			for (unsigned int i = 0; i < lmd_res_vec.size(); i++) {
				lmd_res_vec[i].saveToRootFile();
			}
		}
			break;
		case 1: {
			stringstream in_ss;
			in_ss << input_file_dir << "/resolution_params_"
					<< parametrization_level - 1 << ".root";
			TFile *infile = new TFile(in_ss.str().c_str(), "READ");
			vector<PndLmdResolution> all_res_vec = lmd_data_facade.getDataFromFile<
					PndLmdResolution>(infile);

			// filter the vector for specific options
			LumiFit::LmdDimensionOptions lmd_dim_opt;
			lmd_dim_opt.dimension_type = LumiFit::THETA;
			lmd_dim_opt.track_param_type = LumiFit::IP;

			vector<PndLmdResolution> lmd_res_vec = lmd_data_facade.filterData<
					PndLmdResolution>(all_res_vec, lmd_dim_opt);

			LumiFit::LmdDimensionRange fit_range;
			fit_range.setUnitPrefix(DataStructs::NONE);
			fit_range.setRangeLow(0.004);
			fit_range.setRangeHigh(0.0072);

			vector<PndLmdLumiHelper::lmd_graph*> graph_vec =
					lumifit_helper.generateLmdGraphs(lmd_res_vec, fit_range);
			lumifit_helper.fitParametrizationModelToGraphs(graph_vec);

			outfile->cd();
			lumifit_helper.saveLmdGraphsToFile(graph_vec);
		}
			break;
		case 2: {
			stringstream in_ss;
			in_ss << input_file_dir << "/resolution_params_"
					<< parametrization_level - 1 << ".root";
			TFile *infile = new TFile(in_ss.str().c_str(), "READ");
			vector<PndLmdLumiHelper::lmd_graph*> graph_vec =
					lumifit_helper.getResolutionModelResultsFromFile(infile);
			vector<PndLmdLumiHelper::lmd_graph*> filtered_graph_vec =
					lumifit_helper.filterLmdGraphs(graph_vec, "theta");
			vector<PndLmdLumiHelper::lmd_graph*> new_graph_vec =
					lumifit_helper.generateNewLmdGraphs(filtered_graph_vec);
			outfile->cd();
			lumifit_helper.saveLmdGraphsToFile(filtered_graph_vec);
		}
			break;
		default: {
			cout << "Error: The requested parametrization level "
					<< parametrization_level << " does not exist. The highest level is 3."
					<< endl;
		}
			break;
	}

// save and close file
	outfile->Close();

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}

void displayInfo() {
	// display info
	cout << "Required arguments are: " << endl;
	cout << "-d [path to data]" << endl;
	cout << "-l [parametrization level]" << endl;
}

int main(int argc, char* argv[]) {
	bool is_data_set = false, is_param_level_set = false;
	TString data_path;
	unsigned int parametrization_level;

	int c;

	while ((c = getopt(argc, argv, "hd:l:")) != -1) {
		switch (c) {
			case 'd':
				data_path = optarg;
				is_data_set = true;
				break;
			case 'l':
				parametrization_level = atoi(optarg);
				is_param_level_set = true;
				break;
			case '?':
				if (optopt == 'd' || optopt == 'l')
					cerr << "Option -" << optopt << " requires an argument." << endl;
				else if (isprint(optopt))
					cerr << "Unknown option -" << optopt << "." << endl;
				else
					cerr << "Unknown option character" << optopt << "." << endl;
				return 1;
			case 'h':
				displayInfo();
				return 1;
			default:
				return 1;
		}
	}

	if (is_data_set)
		determineResolution(data_path, parametrization_level);
	else
		displayInfo();
	return 0;
}

