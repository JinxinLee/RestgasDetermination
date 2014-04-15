#include "data/PndLmdDataFacade.h"
#include "PndLmdResultPlotter.h"
#include "fit/PndLmdLumiFitResult.h"
#include "fit/PndLmdLumiFitOptions.h"
#include "data/PndLmdAngularData.h"
#include "data/PndLmdAcceptance.h"
#include "LumiFitStructs.h"

#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TLine.h"
#include "TMultiGraph.h"
#include "TLegend.h"

#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/regex.hpp"

#include <iostream>               // for std::cout
#include <map>
#include <vector>
#include <sstream>

using std::cout;
using std::endl;
using std::cerr;
using std::map;
using std::vector;
using boost::filesystem::path;
using boost::filesystem::directory_iterator;

const double xmin = -7.0;
const double xmax = 7.0;

struct gauss_pars {
		std::pair<double, double> mean;
		std::pair<double, double> sigma;
};

struct ValueBundle {
		double luminosity;
		double luminosity_error;

		gauss_pars gp;
};

struct histBundle {
		TH1D *hist;
		double mean_rel_diff_lumi_error;
		int num_entries;

		histBundle() {
			hist = new TH1D("h", "h", 70, xmin, xmax);
			mean_rel_diff_lumi_error = 0.0;
			num_entries = 0;
		}
};

struct GraphBundle {
		TGraphErrors* mean;
		TGraphErrors* sigma;
		TGraphErrors* lumifit_error; // mean error obtained from luminosity fits

		int type;
};

vector<std::string> find_file(const path & dir_path, // in this directory,
		const std::string & dir_name_filter, const std::string & file_name =
				"lmd_fitted_data.root") // search for this name
		{

	std::cout << dir_name_filter << std::endl;

	const boost::regex my_filter(dir_name_filter,
			boost::regex::extended | boost::regex::icase);
	const boost::regex my_filename_filter(file_name,
			boost::regex::extended | boost::regex::icase);

	vector < std::string > all_matching_files;

	if (exists(dir_path)) {
		std::cout << dir_path.string() << " exists... Looping over this directory.."
				<< std::endl;
		directory_iterator end_itr; // default construction yields past-the-end
		for (directory_iterator itr(dir_path); itr != end_itr; ++itr) {
			// Skip if not a file
			if (boost::filesystem::is_regular_file(itr->status()))
				continue;

			boost::smatch what;

			// Skip if no match
			//std::cout << "trying find " << dir_name_filter << " within "
			//		<< itr->path().string() << std::endl;
			if (!boost::regex_search(itr->path().string(), what, my_filter))
				continue;

			//std::cout << "This directory matches the filter " << dir_name_filter
			//		<< std::endl;
			// File matches, check if fit_result.root file resides in this directory
			if (boost::filesystem::is_directory(itr->status())) {
				for (directory_iterator fitr(itr->path()); fitr != end_itr; ++fitr) {

					boost::smatch fwhat;

					// Skip if no match
					if (!boost::regex_search(fitr->path().string(), fwhat,
							my_filename_filter))
						continue;

					all_matching_files.push_back(itr->path().string());
				}
			}
		}
	}
	return all_matching_files;
}

gauss_pars fitGauss(TH1D *hist) {
	hist->SetStats(1);
	hist->GetXaxis()->SetTitle("#frac{L_{fit}-L_{ref}}{L_{ref}} [%]");
	hist->GetXaxis()->SetTitleOffset(1.4);
	TF1 gausfit("gauss", "gaus(0)", xmin, xmax);
	gausfit.SetParameters(10, hist->GetMean(), hist->GetRMS());
	gausfit.SetParNames("A", "#mu", "#sigma");
	hist->Fit(&gausfit, "+");
	gauss_pars gp;
	gp.mean = std::make_pair(gausfit.GetParameter(1), gausfit.GetParError(1));
	gp.sigma = std::make_pair(gausfit.GetParameter(2), gausfit.GetParError(2));
	return gp;
}

TGraphErrors* makeGraph(vector<double> xvals,
		vector<std::pair<double, double> > yvals) {

	TGraphErrors *graph = new TGraphErrors(xvals.size());
	graph->SetTitle("");
	for (unsigned int i = 0; i < xvals.size(); i++) {
		graph->SetPoint(i, xvals[i], yvals[i].first);
		graph->SetPointError(i, 0.0, yvals[i].second);
	}
	return graph;
}

std::map<PndLmdLumiFitOptions, ValueBundle> convertToValueBundleMap(
		std::map<PndLmdLumiFitOptions, histBundle> &comb_map) {
	std::map<PndLmdLumiFitOptions, ValueBundle> vb_map;

	// do gaus fits
	for (std::map<PndLmdLumiFitOptions, histBundle>::iterator it =
			comb_map.begin(); it != comb_map.end(); it++) {
		vb_map[it->first].gp = fitGauss(it->second.hist);

		double mean_error = it->second.mean_rel_diff_lumi_error
				/ it->second.num_entries;
		std::cout << it->second.mean_rel_diff_lumi_error << " "
				<< it->second.num_entries << std::endl;
		vb_map[it->first].luminosity_error = mean_error;
	}
	return vb_map;
}

GraphBundle makeDependencyGraph(
		std::map<PndLmdLumiFitOptions, ValueBundle> &comb_map) {
	GraphBundle graph_bundle;

	vector<double> xvals;
	vector < std::pair<double, double> > mean_vals;
	vector < std::pair<double, double> > sigma_vals;
	vector < std::pair<double, double> > lumifit_sigma_vals;

	// do gaus fits
	for (std::map<PndLmdLumiFitOptions, ValueBundle>::iterator it =
			comb_map.begin(); it != comb_map.end(); it++) {

		xvals.push_back(it->first.getEstimatorOptions().getFitRangeX().range_low);
		mean_vals.push_back(it->second.gp.mean);
		sigma_vals.push_back(it->second.gp.sigma);

		double mean_error = it->second.luminosity_error;
		lumifit_sigma_vals.push_back(std::make_pair(mean_error, 0.0));
	}

	graph_bundle.mean = makeGraph(xvals, mean_vals);
	graph_bundle.sigma = makeGraph(xvals, sigma_vals);
	graph_bundle.lumifit_error = makeGraph(xvals, lumifit_sigma_vals);

	return graph_bundle;
}

void saveToRootfile(std::map<PndLmdLumiFitOptions, histBundle> &comb_map
		, TFile *file) {

	if (comb_map.size() == 0)
		return;

	std::stringstream topdir_name;
	topdir_name << "fit_opt-"
			<< comb_map.begin()->first.getFitModelOptions();

	file->mkdir(topdir_name.str().c_str());
	gDirectory->cd(topdir_name.str().c_str());
	for (std::map<PndLmdLumiFitOptions, histBundle>::iterator it =
			comb_map.begin(); it != comb_map.end(); it++) {
		double mean_error = it->second.mean_rel_diff_lumi_error
				/ it->second.num_entries;
		it->second.hist->Draw("E1");
		std::stringstream s;
		s.precision(3);
		s << "lumifit error on rel diff: " << mean_error;
		TLatex *label = new TLatex(xmin * 0.9, 40.0, s.str().c_str());
		label->Draw();
		s.str("");
		s << "#Theta fit range: "
				<< it->first.getEstimatorOptions().getFitRangeX().range_low << " - "
				<< it->first.getEstimatorOptions().getFitRangeX().range_high;
		TLatex *label2 = new TLatex(xmin * 0.9, 36.0, s.str().c_str());
		label2->Draw();

		file->cd();
		gDirectory->cd(topdir_name.str().c_str());
		s.str("");
		s << it->first.getEstimatorOptions().getFitRangeX().range_low << " - "
				<< it->first.getEstimatorOptions().getFitRangeX().range_high;
		gDirectory->mkdir(s.str().c_str());
		gDirectory->cd(s.str().c_str());
		it->second.hist->Write("reldiff_dist");
		label->Write("error_label");
		label2->Write("fit_range_label");
	}
}

std::map<PndLmdLumiFitOptions, ValueBundle> bundleSingleFileLuminosityValues(
		PndLmdAngularData &data, LumiFit::PndLmdFitModelOptions &model_opt) {
	std::map<PndLmdLumiFitOptions, ValueBundle> return_map;
	// create an instance of PndLmdResultPlotter the plotting helper class
	PndLmdResultPlotter plotter;

	double lumi_ref = data.getReferenceLuminosity();

	const map<PndLmdLumiFitOptions, PndLmdLumiFitResult*> &fit_results =
			data.getFitResults();

	for (map<PndLmdLumiFitOptions, PndLmdLumiFitResult*>::const_iterator iter =
			fit_results.begin(); iter != fit_results.end(); iter++) {

		if (model_opt.equalBinaryOptions(iter->first.getFitModelOptions())) {
			std::pair<double, double> lumival = plotter.calulateLumiRelDiff(
					iter->second->getLuminosity(), iter->second->getLuminosityError(),
					lumi_ref);
			if (lumival.second < 100 && lumival.second > 0) { // if error of the fit was larger then 10% something is wrong
				return_map[iter->first].gp.mean = std::make_pair(lumival.first,
						lumival.second);
			}
		}
	}
	return return_map;
}

void fillData(PndLmdAngularData &data, LumiFit::PndLmdFitModelOptions &model_opt,
		map<PndLmdLumiFitOptions, histBundle> &result_map) {
	// create an instance of PndLmdResultPlotter the plotting helper class
	PndLmdResultPlotter plotter;

	double lumi_ref = data.getReferenceLuminosity();

	const map<PndLmdLumiFitOptions, PndLmdLumiFitResult*> &fit_results =
			data.getFitResults();

	for (map<PndLmdLumiFitOptions, PndLmdLumiFitResult*>::const_iterator iter =
			fit_results.begin(); iter != fit_results.end(); iter++) {

		if (model_opt.lessThanBinaryOptions(iter->first.getFitModelOptions())) {
			std::pair<double, double> lumival = plotter.calulateLumiRelDiff(
					iter->second->getLuminosity(), iter->second->getLuminosityError(),
					lumi_ref);
			if (lumival.second < 100 && lumival.second > 0) { // if error of the fit was larger then 10% something is wrong
				result_map[iter->first].hist->Fill(lumival.first);
				result_map[iter->first].mean_rel_diff_lumi_error += lumival.second;
				result_map[iter->first].num_entries++;
			}
		}
	}
}

void saveGraphBundlesToFile(GraphBundle &gb_mc, GraphBundle &gb_mc_acc,
		GraphBundle &gb_reco, std::string dependency_suffix) {

	TLatex suffix(0.0, 0.0, dependency_suffix.c_str());
	suffix.Write("suffix");

	gDirectory->mkdir("mc");
	gDirectory->cd("mc");
	std::cout<<gb_mc.mean<<std::endl;
	gDirectory->pwd();
	gb_mc.mean->Write("mean");
	gb_mc.sigma->Write("sigma");
	gb_mc.lumifit_error->Write("lumifit_error");
	std::cout<<"saved stuff"<<std::endl;
	gDirectory->cd("..");
	gDirectory->mkdir("mc_acc");
	gDirectory->cd("mc_acc");
	gb_mc_acc.mean->Write("mean");
	gb_mc_acc.sigma->Write("sigma");
	gb_mc_acc.lumifit_error->Write("lumifit_error");
	gDirectory->cd("..");
	gDirectory->mkdir("reco");
	gDirectory->cd("reco");
	gb_reco.mean->Write("mean");
	gb_reco.sigma->Write("sigma");
	gb_reco.lumifit_error->Write("lumifit_error");
	std::cout<<"saved stuff"<<std::endl;
}

std::vector<PndLmdResultPlotter::graph_bundle> convertToPlotterGraphs(
		map<PndLmdLumiFitOptions, histBundle> &hist_bundle) {
	std::vector < PndLmdResultPlotter::graph_bundle > return_vec;

	for (map<PndLmdLumiFitOptions, histBundle>::iterator it = hist_bundle.begin();
			it != hist_bundle.end(); it++) {
		PndLmdResultPlotter::graph_bundle gb;
		gb.hist1d = new TH1D(*it->second.hist);
		gb.is_residual = false;
		gb.model = 0;
		std::stringstream s;
		s.precision(3);
		s << it->first.getEstimatorOptions().getFitRangeY().range_low << "-"
				<< it->first.getEstimatorOptions().getFitRangeY().range_high;
		gb.labels.push_back(std::make_pair(TString(s.str().c_str()), 1));
		return_vec.push_back(gb);
	}
	return return_vec;
}

void determineLumiFitSystematics(std::string pathname,
		std::string dir_name_filter, std::string dependency_suffix) {
	std::cout << "Generating lumi comparison plots for fit results....\n";

	vector < std::string > paths = find_file(pathname, dir_name_filter);

	// create an instance of PndLmdDataFacade for retrieving data
	PndLmdDataFacade lmd_data_facade;

	map<PndLmdLumiFitOptions, histBundle> mc_comb_map;
	map<PndLmdLumiFitOptions, histBundle> mc_acc_comb_map;
	map<PndLmdLumiFitOptions, histBundle> reco_comb_map;

	for (unsigned int j = 0; j < paths.size(); j++) {
		std::cout << "working on: " << paths[j] << std::endl;

		// ------ get file -------------------------------------------------------
		TFile *fdata = new TFile((paths[j] + "/lmd_fitted_data.root").c_str(),
				"UPDATE");

		// read in data from a root file which will return a map of pairs dimension options to
		// vectors of pointers to PndLmdAngularData objects
		vector<PndLmdAngularData> full_data_vec = lmd_data_facade.getDataFromFile<PndLmdAngularData>(fdata);

		// MC data case
		LumiFit::LmdDimensionOptions lmd_dim_opt;
		lmd_dim_opt.track_type = LumiFit::MC;

		LumiFit::PndLmdFitModelOptions model_op;

		vector<PndLmdAngularData> data_vec = lmd_data_facade.filterData(full_data_vec, lmd_dim_opt);

		// standard case would be just a single reco data object
		if (data_vec.size() > 0) {
			fillData(data_vec[0], model_op, mc_comb_map);
		}

		// MC ACC data case
		lmd_dim_opt.track_type = LumiFit::MC_ACC;
		model_op.acceptance_correction_active = true;

		data_vec = lmd_data_facade.filterData(full_data_vec, lmd_dim_opt);

		// standard case would be just a single reco data object
		if (data_vec.size() > 0) {
			fillData(data_vec[0], model_op, mc_acc_comb_map);
		}

		// RECO data case
		lmd_dim_opt.track_type = LumiFit::RECO;
		model_op.resolution_smearing_active = true;

		data_vec = lmd_data_facade.filterData(full_data_vec, lmd_dim_opt);

		// standard case would be just a single reco data object
		if (data_vec.size() > 0) {
			fillData(data_vec[0], model_op, reco_comb_map);
		}
	}

	// create dependency graph bundles
	std::map<PndLmdLumiFitOptions, ValueBundle> vm_mc = convertToValueBundleMap(
			mc_comb_map);
	std::map<PndLmdLumiFitOptions, ValueBundle> vm_mc_acc =
			convertToValueBundleMap(mc_acc_comb_map);
	std::map<PndLmdLumiFitOptions, ValueBundle> vm_reco = convertToValueBundleMap(
			reco_comb_map);

	// make bookys of fits
	std::vector < PndLmdResultPlotter::graph_bundle > mc_graph_bundles =
			convertToPlotterGraphs(mc_comb_map);
	PndLmdResultPlotter lmd_plotter;
	lmd_plotter.makeBooky(mc_graph_bundles, TString("lumi_reldiff_mc_")+dependency_suffix);
	std::vector < PndLmdResultPlotter::graph_bundle > mc_acc_graph_bundles =
			convertToPlotterGraphs(mc_acc_comb_map);
	lmd_plotter.makeBooky(mc_acc_graph_bundles, TString("lumi_reldiff_mc_acc_")+dependency_suffix);
	std::vector < PndLmdResultPlotter::graph_bundle > reco_graph_bundles =
			convertToPlotterGraphs(reco_comb_map);
	lmd_plotter.makeBooky(reco_graph_bundles, TString("lumi_reldiff_reco_")+dependency_suffix);

	GraphBundle gb_mc = makeDependencyGraph(vm_mc);
	GraphBundle gb_mc_acc = makeDependencyGraph(vm_mc_acc);
	GraphBundle gb_reco = makeDependencyGraph(vm_reco);
	gb_mc.type = 0;
	gb_mc_acc.type = 0;
	gb_reco.type = 0;

	// save results
	TFile *file = new TFile(
			TString(pathname) + "/lumifit_systematics-"
					+ TString(dependency_suffix.c_str()) + ".root", "RECREATE");

	saveGraphBundlesToFile(gb_mc, gb_mc_acc, gb_reco, dependency_suffix);

	saveToRootfile(mc_comb_map, file);
	saveToRootfile(mc_acc_comb_map, file);
	saveToRootfile(reco_comb_map, file);
}

void createDependencyGraphs(std::string pathname,
		std::string dependency_suffix) {
	TFile *fdata = new TFile(pathname.c_str(), "READ");

	// create an instance of PndLmdDataFacade for retrieving data
	PndLmdDataFacade lmd_data_facade;

	// read in data from a root file which will return a map of pairs dimension options to
	// vectors of pointers to PndLmdAngularData objects
	vector<PndLmdAngularData> full_data_vec =
			lmd_data_facade.getDataFromFile<PndLmdAngularData>(fdata);

	// MC data case
	LumiFit::LmdDimensionOptions lmd_dim_opt;
	lmd_dim_opt.track_type = LumiFit::MC;

	LumiFit::PndLmdFitModelOptions model_op;

	vector<PndLmdAngularData> data_vec = lmd_data_facade.filterData(full_data_vec, lmd_dim_opt);
	std::cout<<"Got "<<data_vec.size()<<" data entries!"<<std::endl;

	GraphBundle gb_mc;

	// standard case would be just a single reco data object
	if (data_vec.size() > 0) {
		std::map<PndLmdLumiFitOptions, ValueBundle> vm =
				bundleSingleFileLuminosityValues(data_vec[0], model_op);
		std::cout<<model_op<<" got "<<vm.size()<<" entries!"<<std::endl;
		gb_mc = makeDependencyGraph(vm);
	}

	// MC ACC data case
	lmd_dim_opt.track_type = LumiFit::MC_ACC;
	model_op.acceptance_correction_active = true;

	data_vec = lmd_data_facade.filterData(full_data_vec, lmd_dim_opt);

	GraphBundle gb_mc_acc;

	// standard case would be just a single reco data object
	if (data_vec.size() > 0) {
		std::map<PndLmdLumiFitOptions, ValueBundle> vm =
				bundleSingleFileLuminosityValues(data_vec[0], model_op);
		std::cout<<model_op<<" got "<<vm.size()<<" entries!"<<std::endl;
		gb_mc_acc = makeDependencyGraph(vm);
	}

	// RECO data case
	lmd_dim_opt.track_type = LumiFit::RECO;
	model_op.resolution_smearing_active = true;

	data_vec = lmd_data_facade.filterData(full_data_vec, lmd_dim_opt);

	GraphBundle gb_reco;

	// standard case would be just a single reco data object
	if (data_vec.size() > 0) {
		std::map<PndLmdLumiFitOptions, ValueBundle> vm =
				bundleSingleFileLuminosityValues(data_vec[0], model_op);
		std::cout<<model_op<<" got "<<vm.size()<<" entries!"<<std::endl;
		gb_reco = makeDependencyGraph(vm);
	}

	gb_mc.type = 1;
	gb_mc_acc.type = 1;
	gb_reco.type = 1;

	// save results

	path fullpath(pathname);

	TFile *file = new TFile(
			TString(fullpath.remove_filename().c_str()) + "/lumifit_systematics-"
					+ TString(dependency_suffix.c_str()) + ".root", "RECREATE");

	saveGraphBundlesToFile(gb_mc, gb_mc_acc, gb_reco, dependency_suffix);
	file->Close();
}

void displayInfo() {
// display info
	cout << "Required arguments are: " << endl;
	cout << "-d [path to data]" << endl;
	cout << "-l [suffix label for this data]" << endl;
	cout << "Optional arguments are: " << endl;
	cout << "-b [bunch folder prefix]" << endl;
}

int main(int argc, char* argv[]) {
	bool is_data_set = false, is_suffix_label_set = false,
			is_bunch_folder_prefix_set = false;
	std::string data_path, suffix_label, bunch_folder_prefix;

	int c;

	while ((c = getopt(argc, argv, "hd:l:b:")) != -1) {
		switch (c) {
			case 'd':
				data_path = optarg;
				is_data_set = true;
				break;
			case 'l':
				suffix_label = optarg;
				is_suffix_label_set = true;
				break;
			case 'b':
				bunch_folder_prefix = optarg;
				is_bunch_folder_prefix_set = true;
				break;
			case '?':
				if (optopt == 'd' || optopt == 'l' || optopt == 'b')
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

	if (is_data_set && is_suffix_label_set && is_bunch_folder_prefix_set)
		determineLumiFitSystematics(data_path, bunch_folder_prefix, suffix_label);
	else if (is_data_set && is_suffix_label_set) {
		createDependencyGraphs(data_path, suffix_label);
	} else
		displayInfo();
	return 0;
}

