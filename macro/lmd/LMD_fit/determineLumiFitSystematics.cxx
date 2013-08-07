#include "PndLmdLumiHelper.h"
#include "PndLmdResultPlotter.h"
#include "PndLmdLumiFitResult.h"
#include "PndLmdLumiFitOptions.h"
#include "PndLmdData.h"
#include "PndLmdAcceptance.h"

#include "TH1D.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TLine.h"
#include "TMultiGraph.h"

#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/regex.hpp"

#include <iostream>               // for std::cout
#include <vector>
#include <sstream>

using boost::filesystem::path;
using boost::filesystem::directory_iterator;

const double xmin = -4.0;
const double xmax = 4.0;

struct histBundle {
	TH1D *hist;
	double mean_rel_diff_lumi_error;
	int num_entries;

	histBundle() {
		hist = new TH1D("h", "h", 40, xmin, xmax);
		mean_rel_diff_lumi_error = 0.0;
		num_entries = 0;
	}
};

struct gauss_pars {
	std::pair<double, double> mean;
	std::pair<double, double> sigma;
};

std::vector<std::string> find_file(const path & dir_path, // in this directory,
		const std::string & dir_name_filter, const std::string & file_name =
				"fit_results.root") // search for this name
		{

	std::cout << dir_name_filter << std::endl;

	const boost::regex my_filter(dir_name_filter,
			boost::regex::extended | boost::regex::icase);
	const boost::regex my_filename_filter(file_name,
			boost::regex::extended | boost::regex::icase);

	std::vector<std::string> all_matching_files;

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
	gausfit.SetParameters(10, 0.0, 1.0);
	gausfit.SetParNames("A", "#mu", "#sigma");
	hist->Fit(&gausfit, "+");
	gauss_pars gp;
	gp.mean = std::make_pair(gausfit.GetParameter(1), gausfit.GetParError(1));
	gp.sigma = std::make_pair(gausfit.GetParameter(2), gausfit.GetParError(2));
	return gp;
}

TGraphErrors* makeGraph(std::vector<double> xvals,
		std::vector<std::pair<double, double> > yvals) {

	TGraphErrors *graph = new TGraphErrors(xvals.size());
	graph->SetTitle("");
	for (unsigned int i = 0; i < xvals.size(); i++) {
		graph->SetPoint(i, xvals[i], yvals[i].first);
		graph->SetPointError(i, 0.0, yvals[i].second);
	}
	return graph;
}

void determineLumiFitSystematics(std::string path, std::string dir_name_filter, std::string dependency_suffix) {
	std::cout << "Generating lumi comparison plots for fit results....\n";

	std::vector<std::string> paths = find_file(path, dir_name_filter);

	// create an instance of PndLmdResultPlotter the plotting helper class
	PndLmdLumiHelper helper;
	// create an instance of PndLmdResultPlotter the plotting helper class
	PndLmdResultPlotter plotter;

	//TH1D *dist_hist = new TH1D("lumi_rel_diffs", "", 40, xmin,
	//		xmax);

	std::map<PndLmdLumiFitOptions, histBundle> comb_map;

	for (unsigned int j = 0; j < paths.size(); j++) {
		//std::cout << "found: " << paths[i] << std::endl;

		// ------ get files -------------------------------------------------------
		TFile *fdata = new TFile((paths[j] + "/lmd_data.root").c_str(), "UPDATE");

		// read in data from a root file which will return a vector of pointers to PndLmdData objects
		std::vector<PndLmdData*> data_vec = helper.getDataFromFile(fdata);

		// if you only have a single data object (mostly the case)
		if (data_vec.size() > 0) {

			//std::map<PndLmdAcceptance*, std::vector<PndLmdLumiFitResult*> > &fit_map = data_vec[0]->getFitMap();
			std::vector<PndLmdAcceptance*> accs = data_vec[0]->getListOfAcceptances();
			// in case there exists only a single acceptance (with which fit were performed...
			// usually the case)
			if (accs.size() > 0) {
				// create a vector of graph bundles (one entry for each fit option)

				double lumi_ref = data_vec[0]->getReferenceLuminosity();

				std::map<PndLmdAcceptance*, std::vector<PndLmdLumiFitResult*> >& fit_map =
						data_vec[0]->getFitMap();

				std::map<PndLmdAcceptance*, std::vector<PndLmdLumiFitResult*> >::iterator iter =
						fit_map.find(accs[0]);

				if (iter != fit_map.end()) {
					std::vector<PndLmdLumiFitResult*> fit_res = iter->second;

					for (unsigned int i = 0; i < fit_res.size(); i++) {
						if (fit_res[i]->getLumiFitOptions()->getModelBinaryOptions().isSmearingOn()) {
							if (fit_res[i]->getLumiFitOptions()->getDataBinaryOptions().isSmearingOn()) {
								std::pair<double, double> lumival = plotter.calulateLumiRelDiff(
										fit_res[i]->getLuminosity(),
										fit_res[i]->getLuminosityError(), lumi_ref);
								if (lumival.second < 100 && lumival.second > 0) { // if error of the fit was larger then 10% something is wrong
									comb_map[*(fit_res[i]->getLumiFitOptions())].hist->Fill(
											lumival.first);
									comb_map[*(fit_res[i]->getLumiFitOptions())].mean_rel_diff_lumi_error +=
											lumival.second;
									comb_map[*(fit_res[i]->getLumiFitOptions())].num_entries++;
								}
							}
						}
					}
				}
			}
		}
	}

	std::vector<double> xvals;
	std::vector<std::pair<double, double> > mean_vals;
	std::vector<std::pair<double, double> > sigma_vals;
	std::vector<std::pair<double, double> > lumifit_sigma_vals;
	// do gaus fits
	for (std::map<PndLmdLumiFitOptions, histBundle>::iterator it =
			comb_map.begin(); it != comb_map.end(); it++) {
		gauss_pars gp = fitGauss(it->second.hist);

		xvals.push_back(it->first.getThetaFitRangeLow());
		mean_vals.push_back(gp.mean);
		sigma_vals.push_back(gp.sigma);

		double mean_error = it->second.mean_rel_diff_lumi_error
				/ it->second.num_entries;
		std::cout << it->second.mean_rel_diff_lumi_error << " "
				<< it->second.num_entries << std::endl;
		lumifit_sigma_vals.push_back(std::make_pair(mean_error, 0.0));
	}

	// make dependency graphs
	TGraphErrors *graph_mean = makeGraph(xvals, mean_vals);
	TGraphErrors *graph_sigma = makeGraph(xvals, sigma_vals);
	TGraphErrors *graph_lumifit_sigma = makeGraph(xvals, lumifit_sigma_vals);

	TCanvas c_dep;

	graph_mean->Draw("AP");
	graph_mean->GetYaxis()->SetTitle("fit dist. mean [mrad]");
	graph_mean->GetYaxis()->SetTitleOffset(1.5);
	graph_mean->GetXaxis()->SetTitle("lower fit range [mrad]");
	graph_mean->GetXaxis()->SetTitleOffset(1.1);

	graph_sigma->Draw("AP");
	graph_sigma->GetYaxis()->SetTitle("fit dist. sigma [mrad]");
	graph_sigma->GetYaxis()->SetTitleOffset(1.5);
	graph_sigma->GetXaxis()->SetTitle("lower fit range [mrad]");
	graph_sigma->GetXaxis()->SetTitleOffset(1.1);

	graph_lumifit_sigma->Draw("AP");
	graph_lumifit_sigma->SetLineStyle(2);
	graph_lumifit_sigma->GetYaxis()->SetTitle("fit dist. sigma [mrad]");
	graph_lumifit_sigma->GetYaxis()->SetTitleOffset(1.5);
	graph_lumifit_sigma->GetXaxis()->SetTitle("lower fit range [mrad]");
	graph_lumifit_sigma->GetXaxis()->SetTitleOffset(1.1);

	// save results
	TFile *file = new TFile(TString(path) + "/lumifit_systematics.root", "RECREATE");

	TLatex suffix(0.0, 0.0, dependency_suffix.c_str());
	suffix.Write("suffix");

	graph_mean->Write("reldiff_sys_mean");
	graph_sigma->Write("reldiff_sys_sigma");
	graph_lumifit_sigma->Write("reldiff_lumifit_error");

	file->mkdir("individuals");
	gDirectory->cd("individuals");
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
		s << "#Theta fit range: " << it->first.getThetaFitRangeLow() << " - "
				<< it->first.getThetaFitRangeHigh();
		TLatex *label2 = new TLatex(xmin * 0.9, 36.0, s.str().c_str());
		label2->Draw();

		file->cd();
		gDirectory->cd("individuals");
		s.str("");
		s << it->first.getThetaFitRangeLow() << " - "
				<< it->first.getThetaFitRangeHigh();
		gDirectory->mkdir(s.str().c_str());
		gDirectory->cd(s.str().c_str());
		it->second.hist->Write("reldiff_dist");
		label->Write("error_label");
		label2->Write("fit_range_label");
	}
}

int main(int argc, char* argv[]) {
	if (argc == 4) {
		determineLumiFitSystematics(std::string(argv[1]), // in this directory,
		std::string(argv[2]), std::string(argv[3]));
	}
	return 0;
}
