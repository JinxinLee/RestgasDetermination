#include "PndLmdResultPlotter.h"
#include "PndLmdLumiFitResult.h"
#include "PndLmdLumiFitOptions.h"
#include "PndLmdData.h"
#include "PndLmdAcceptance.h"

#include "TH1D.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLatex.h"

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

void compareAcceptances(TString acceptance_url,
		TString comparison_acceptance_url) {
	std::cout << "Generating acceptance comparison plots ....\n";

	// create an instance of PndLmdResultPlotter the plotting helper class
	PndLmdResultPlotter plotter;

	// A small helper class that helps to construct lmd data objects
	PndLmdLumiHelper lumifit_helper;

	TFile *f = new TFile(acceptance_url, "READ");
	TFile *fcomp = new TFile(comparison_acceptance_url, "READ");

	std::vector<PndLmdAcceptance*> acc = lumifit_helper.getAcceptanceFromFile(f);
	std::vector<PndLmdAcceptance*> acc_comp =
			lumifit_helper.getAcceptanceFromFile(fcomp);

	// in principle some kind of matching between acceptances should be made here
	// atm we assume that only one acceptance was calculated and they are equal in binning etc

	PndLmdResultPlotter::acceptance_bundle acc_bundle =
			plotter.makeAcceptanceBundle(acc[0], false);
	PndLmdResultPlotter::acceptance_bundle acc_bundle_comp =
			plotter.makeAcceptanceBundle(acc_comp[0], false);

	TGraphAsymmErrors *gdiff = plotter.makeDifferenceGraph(
			acc_bundle.acceptance_1d, acc_bundle_comp.acceptance_1d);

	TCanvas c;
	gdiff->Draw("AP");
	c.SaveAs("acceptance_comparison.pdf");
	// ================================ END PLOTTING ================================ //
}

int main(int argc, char* argv[]) {
	if (argc == 3) {
		compareAcceptances(argv[1], argv[2]);
		return 0;
	}
	return 1;
}
