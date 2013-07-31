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
#include "TLegend.h"

#include <iostream>               // for std::cout
#include <vector>
#include <sstream>

void makeOverviewPlot(TString suffix) {
	TDirectory *topdir = gDirectory;
	gStyle->SetOptFit(1111);
	TCanvas c;
	c.Print(TString("lumidist_") + suffix + ".pdf[");
	TIter next(gDirectory->GetListOfKeys());
	TDirectory *dir;
	while ((dir = (TDirectory*) next())) {
		TH1D* hist;
		gDirectory->cd(dir->GetName());
		gDirectory->GetObject("reldiff_dist", hist);
		hist->Draw("E1");

		TLatex *label;
		gDirectory->GetObject("error_label", label);
		label->Draw();
		TLatex *label2;
		gDirectory->GetObject("fit_range_label", label2);
		label2->Draw();
		c.Print(TString("lumidist_") + suffix + ".pdf");
		gDirectory->cd("..");
	}
	c.Print(TString("lumidist_") + suffix + ".pdf]");

	gDirectory->cd(topdir->GetPath());
	TCanvas co;
	co.Divide(4, 3);
	int counter = 1;
	co.Print(TString("lumidist_overview_") + suffix + ".pdf["); // No actual print, just open file
	TIter next2(gDirectory->GetListOfKeys());
	while ((dir = (TDirectory*) next2())) {
		co.cd(counter);
		TH1D* hist;
		gDirectory->cd(dir->GetName());
		gDirectory->GetObject("reldiff_dist", hist);
		hist->Draw("E1");

		TLatex *label;
		gDirectory->GetObject("error_label", label);
		label->Draw();
		TLatex *label2;
		gDirectory->GetObject("fit_range_label", label2);
		label2->Draw();

		if (counter == 12 || counter >= topdir->GetListOfKeys()->GetEntries() - 1) {
			co.Print(TString("lumidist_overview_") + suffix + ".pdf"); // actually print canvas to file
			if (counter >= topdir->GetListOfKeys()->GetEntries() - 1) {
				// clear all pads
				for (unsigned int temp = 1; temp <= 12; temp++) {
					co.cd(temp)->Clear();
				}
			}
			counter = 0;
		}
		counter++;
		gDirectory->cd("..");
	}
	co.Print(TString("lumidist_overview_") + suffix + ".pdf]");
}

void combineLumiFitResults(std::vector<std::string> paths) {
	std::cout << "Generating lumi comparison plots for fit results....\n";

	TMultiGraph mean_all;
	TMultiGraph sigma_all;

	int colors[] = { 1, 2, 8, 9 };

	TLegend legend1(0.1, 0.7, 0.48, 0.9);
	legend1.SetHeader("");
	TLegend legend2(0.1, 0.7, 0.48, 0.9);
	legend2.SetHeader("");

	for (unsigned int j = 0; j < paths.size(); j++) {

		TGraphErrors *mean;
		TGraphErrors *sigma;
		TGraphErrors *lumifit_err;

		TLatex *suffix;
		// get graphs
		TFile *f = new TFile(TString(paths[j]) + "/lumifit_systematics.root",
				"OPEN");
		f->GetObject("reldiff_sys_mean", mean);
		f->GetObject("reldiff_sys_sigma", sigma);
		f->GetObject("reldiff_lumifit_error", lumifit_err);
		f->GetObject("suffix", suffix);

		mean->SetLineColor(colors[j]);
		mean->SetMarkerColor(colors[j]);
		mean->SetMarkerStyle(5);
		mean->SetMarkerSize(1.0);
		mean_all.Add(mean, "P");
		legend1.AddEntry(mean, suffix->GetTitle(), "pe");

		sigma->SetLineColor(colors[j]);
		sigma->SetMarkerColor(colors[j]);
		sigma->SetMarkerStyle(5);
		sigma->SetMarkerSize(1.0);
		sigma_all.Add(sigma, "P");
		legend2.AddEntry(sigma, TString(suffix->GetTitle()) + " sys.", "lep");
		lumifit_err->SetLineColor(colors[j]);
		lumifit_err->SetMarkerColor(colors[j]);
		lumifit_err->SetMarkerStyle(5);
		lumifit_err->SetMarkerSize(1.0);
		sigma_all.Add(lumifit_err, "C");
		legend2.AddEntry(lumifit_err, TString(suffix->GetTitle()) + " lmdfit.",
				"l");

		gDirectory->cd("individuals");
		makeOverviewPlot(suffix->GetTitle());
	}

	TCanvas c_dep;
	c_dep.Divide(2, 1);
	c_dep.cd(1);
	mean_all.Draw("A");
	mean_all.GetYaxis()->SetTitle("fit dist. mean [mrad]");
	mean_all.GetYaxis()->SetTitleOffset(1.5);
	mean_all.GetXaxis()->SetTitle("lower fit range [mrad]");
	mean_all.GetXaxis()->SetTitleOffset(1.1);
	legend1.Draw();
	gPad->Update();
	TLine *zero = new TLine(gPad->GetUxmin(), 0, gPad->GetUxmax(), 0);
	zero->Draw();
	c_dep.cd(2);
	sigma_all.Draw("A");
	sigma_all.GetYaxis()->SetTitle("fit dist. sigma [mrad]");
	sigma_all.GetYaxis()->SetTitleOffset(1.5);
	sigma_all.GetXaxis()->SetTitle("lower fit range [mrad]");
	sigma_all.GetXaxis()->SetTitleOffset(1.1);
	legend2.Draw();
	c_dep.SaveAs("lumidist_dependency.pdf");
}

int main(int argc, char* argv[]) {
	std::vector<std::string> paths;
	for (int i = 1; i < argc; i++) {
		paths.push_back(std::string(argv[i]));
	}
	combineLumiFitResults(paths);
	return 0;
}
