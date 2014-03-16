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
 * -First you read in a PndLmdData object that was saved by the runLumi6Fit.C macro
 * -Then you create so graph bundles that contain all the information of the fit
 *  results and data in form of ROOT objects.
 * -Finally you can pass these bundles to functions of the plotter and generate
 *  different type of plots
 */

#include "PndLmdResultPlotter.h"
#include "PndLmdData.h"
#include "PndLmdAcceptance.h"
#include "PndLmdDataFacade.h"

#include <vector>
#include <map>
#include <iostream>

#include "TString.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TFile.h"
#include "TLatex.h"
#include "TGraphAsymmErrors.h"

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

	// read in data from a root file which will return a map of pointers to PndLmdData objects
	std::vector<PndLmdData> data_vec =
			lmd_data_facade.getDataFromFile<PndLmdData>(fdata);

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
	plotter.fillSinglePad(&c, gb, false, true);
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

int main(int argc, char* argv[]) {
	if (argc == 2) {
		plotLumiFitResults(TString(argv[1]));
	} else if (argc == 3) {
		plotLumiFitResults(TString(argv[1]), TString(argv[2]));
	}

	return 0;
}
