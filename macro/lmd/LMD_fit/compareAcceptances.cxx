#include "data/PndLmdDataFacade.h"
#include "PndLmdResultPlotter.h"
#include "fit/PndLmdLumiFitResult.h"
#include "data/PndLmdAcceptance.h"
#include "model/PndLmdROOTDataModel1D.h"

#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TAxis.h"

#include <iostream>               // for std::cout
#include <utility>
#include <vector>
#include <sstream>

// create an instance of PndLmdResultPlotter the plotting helper class
PndLmdResultPlotter plotter;

TGraphAsymmErrors* createDifferenceGraph(PndLmdAcceptance *acceptance,
		PndLmdAcceptance *ref_acceptance, int color) {

	TGraphAsymmErrors *gdiff = plotter.makeRatioGraph(
			plotter.createAcceptanceGraph(acceptance),
			plotter.createAcceptanceGraph(ref_acceptance));

	gdiff->SetTitle("");
	gdiff->GetXaxis()->SetTitle("#Theta_{MC} [mrad]");
	gdiff->GetYaxis()->SetTitle("#Delta Acc");
	gdiff->SetMarkerColor(color);
	gdiff->SetLineColor(color);
	return gdiff;
}

TGraphErrors* createGraph(std::vector<double>& x_values,
		std::vector<double>& y_values) {
	unsigned int size = x_values.size();
	double x[size];
	double y[size];

	for (unsigned int i = 0; i < size; i++) {
		x[i] = x_values[i];
		y[i] = y_values[i];
	}

	TGraphErrors *graph = new TGraphErrors(size, x, y, 0, 0);
	graph->SetTitle("");
	graph->SetMarkerStyle(2);
	return graph;
}

void compareAcceptances(
		std::vector<std::pair<TString, TString> > &acceptance_dirs,
		std::pair<TString, TString> ref_acceptance_dir) {
	std::cout << "Generating acceptance comparison plots ....\n";

	TString acceptance_filename("lmd_acc_data.root");

	PndLmdDataFacade lmd_data_facade;

	TFile *fref = new TFile(ref_acceptance_dir.first, "READ");
	std::vector<PndLmdAcceptance> acc_ref = lmd_data_facade.getDataFromFile<
			PndLmdAcceptance>(fref);

	std::vector<double> x_values;
	std::vector<double> y_values;

	int colors[10] = { 1, 2, 8, 9, 6, 7, 3, 4 };

	std::vector<std::pair<TString, TGraphAsymmErrors*> > ratios;

	for (unsigned int i = 0; i < acceptance_dirs.size(); i++) {
		TFile *f = new TFile(acceptance_dirs[i].first, "READ");
		std::vector<PndLmdAcceptance> acc = lmd_data_facade.getDataFromFile<
				PndLmdAcceptance>(f);

		// in principle some kind of matching between acceptances should be made here
		// atm we assume that only one acceptance was calculated and they are equal in binning etc

		// create difference graphs
		if (acc_ref.size() > 0 && acc.size() > 0) {
			ratios.push_back(
					std::make_pair(acceptance_dirs[i].second,
							createDifferenceGraph(&acc[0], &acc_ref[0], colors[i])));
		}

		if (false) {
			PndLmdROOTDataModel1D acc_model("acceptance");

			TCanvas can;
			acc[0].getAcceptance1D()->Draw();
			can.Update();

			acc_model.setGraph(acc[0].getAcceptance1D()->GetPaintedGraph());
			acc_model.setIntpolType(LumiFit::LINEAR);
			acc_model.setDataDimension(acc[0].getPrimaryDimension().dimension_range);

			LumiFit::LmdSimIPParameters bp =
					lmd_data_facade.readSimulationIPParameters(
							std::string(acceptance_dirs[i].first.Data()));
			double lower_acc_bound = acc_model.getAcceptanceBounds().first;

			x_values.push_back(bp.offset_x_mean);
			y_values.push_back(lower_acc_bound);
		}
	}

	TCanvas c;
	TLegend leg(0.05, 0.75, 0.3, 0.95);
	for(unsigned int i =0; i < ratios.size(); i++) {
    if(i == 0) {
    	ratios[i].second->GetYaxis()->SetRangeUser(0.95, 1.05);
    	ratios[i].second->Draw("AP");
    }
    else
    	ratios[i].second->Draw("PSAME");
    leg.AddEntry(ratios[i].second, ratios[i].first);
	}
	leg.Draw();
	c.SaveAs("acceptance_comparison.pdf");

	TGraphErrors* graph = createGraph(x_values, y_values);

	graph->Draw("AP");
	c.SaveAs("acceptance_dependency.pdf");

	// ================================ END PLOTTING ================================ //
}

int main(int argc, char* argv[]) {
	if (argc >= 4) {
		std::vector<std::pair<TString, TString> > acceptance_file_urls;
		for (unsigned int i = 3; i < argc; i = i + 2)
			acceptance_file_urls.push_back(
					std::make_pair(TString(argv[i]), TString(argv[i + 1])));
		compareAcceptances(acceptance_file_urls,
				std::make_pair(TString(argv[1]), TString(argv[2])));
		return 0;
	}
	return 1;
}
