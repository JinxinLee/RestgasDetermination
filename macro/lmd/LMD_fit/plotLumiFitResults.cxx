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

#include <vector>
#include <map>
#include <iostream>

#include "TString.h"
#include "TCanvas.h"
#include "TStyle.h"

void plotLumiFitResults(TString path) {
	std::cout << "Generating lumi plots for fit results....\n";

	// A small helper class that helps to construct lmd data objects
	PndLmdLumiHelper lumifit_helper;

	// create an instance of PndLmdResultPlotter the plotting helper class
	PndLmdResultPlotter plotter;

	// ================================ BEGIN CONFIG ================================ //
	// PndLmdResultPlotter sets default pad margins etc that should be fine for most cases
	// you can fine tune it and overwrite the default values
	//gStyle->SetPadTopMargin(0.06);
	//gStyle->SetPadBottomMargin(0.12);
	//gStyle->SetPadLeftMargin(0.14);
	//gStyle->SetPadRightMargin(0.1);

	// overwrite the default theta plot range if possible (if its larger than the max
	// plot range then it has no effect)
	plotter.setThetaPlotRange(0.5, 16.0);

	// The plotter has more options for text positioning and tex sizes for which you can
	// overwrite the default values here
	//plotter.setTextLeftPos(0.55);
	//plotter.setTextTopPos(0.98);
	//plotter.setTextSpacing(0.08);
	//plotter.setTextSize(0.06);
	//plotter.setLabelSize(0.06);

	//plotter.setLabelOffsetX(0.007);
	//plotter.setLabelOffsetY(0.007);
	//plotter.setTitleOffsetX(1.0);
	//plotter.setTitleOffsetY(1.5);
	// ================================= END CONFIG ================================= //

	// ------ get files -------------------------------------------------------
	TFile *fdata = new TFile(path + "/lmd_data.root", "UPDATE");

	// read in data from a root file which will return a vector of pointers to PndLmdData objects
	std::vector<PndLmdData*> data_vec = lumifit_helper.getDataFromFile(fdata);

	// =============================== BEGIN PLOTTING =============================== //
	// if you only have a single data object (mostly the case)
	if (data_vec.size() > 0) {
		//std::map<PndLmdAcceptance*, std::vector<PndLmdLumiFitResult*> > &fit_map = data_vec[0]->getFitMap();
		std::vector<PndLmdAcceptance*> accs = data_vec[0]->getListOfAcceptances();
		// in case there exists only a single acceptance (with which fit were performed...
		// usually the case)
		if (accs.size() > 0) {
			// create a vector of graph bundles (one entry for each fit option)
			std::map<PndLmdLumiFitOptions,
					std::vector<PndLmdResultPlotter::graph_bundle_1D>,
					PndLmdResultPlotter::fit_options_compare> graph_bundle_map =
					plotter.makeGraphBundles1D(data_vec[0], accs[0]);

			// create an acceptance bundle for this acceptance
			PndLmdResultPlotter::acceptance_bundle acc_bundle =
					plotter.makeAcceptanceBundle(accs[0]);

			// make an overview canvas for these fit specs
			plotter.makeFitResultBooky(graph_bundle_map, acc_bundle);
		}
	}
	// ================================ END PLOTTING ================================ //
}

int main(int argc, char* argv[]) {
	if (argc == 2) {
		plotLumiFitResults(TString(argv[1]));
	}

	return 0;
}
