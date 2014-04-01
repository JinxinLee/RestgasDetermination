#include "PndLmdResultPlotter.h"
#include "PndLmdLumiFitResult.h"
#include "PndLmdLumiFitOptions.h"
#include "PndLmdAngularData.h"
#include "PndLmdAcceptance.h"

#include "TFile.h"
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

struct GraphBundle {
		TGraphErrors* mean;
		TGraphErrors* sigma;
		TGraphErrors* lumifit_error; // mean error obtained from luminosity fits

		TString settings;
};

GraphBundle getGraphsFromFile(TFile *file, TString type) {
	GraphBundle gb;
	TLatex *suffix;

	file->GetObject("suffix", suffix);
	gb.settings = suffix->GetTitle();

	file->GetObject(type + "/mean", gb.mean);
	file->GetObject(type + "/sigma", gb.sigma);
	file->GetObject(type + "/lumifit_error", gb.lumifit_error);

	if (gb.mean && gb.sigma && gb.lumifit_error) {
		std::cout << "found graphs!" << std::endl;
	} else {
		std::cout << "ooops! did not find graphs!" << std::endl;
	}

	return gb;
}

TCanvas* makeDependencyCanvas(std::map<int, GraphBundle> &scenarios) {
	//TLegend *legend1 = new TLegend(0.1, 0.7, 0.48, 0.9);
	TLegend *legend1 = new TLegend(0.6, 0.7, 0.98, 0.9);
	legend1->SetHeader("");
	TLegend *legend2 = new TLegend(0.1, 0.7, 0.48, 0.9);
	legend2->SetHeader("");

	TCanvas *test = new TCanvas("test", "", 1000, 700);

	TCanvas *c = new TCanvas("c", "", 1000, 700);

	if (scenarios.size() == 0)
		return c;

	/*if(scenarios.begin()->second.sigma && scenarios.begin()->second.lumifit_error)
	 c->Divide(2, 1);*/

	bool first = true;

	double c1_ymax = 0.0;
	double c1_ymin = 0.0;
	double c2_ymax = 0.0;
	double c2_ymin = 0.0;
	TAxis *axis_y1;
	TAxis *axis_y2;

	for (std::map<int, GraphBundle>::iterator it = scenarios.begin();
			it != scenarios.end(); it++) {

		int color = it->first;

		c->cd();

		it->second.mean->SetLineColor(color);
		it->second.mean->SetMarkerColor(color);
		it->second.mean->SetMarkerStyle(5);
		it->second.mean->SetMarkerSize(1.0);

		if (first) {
			it->second.mean->Draw("AC*");
			it->second.mean->GetYaxis()->SetTitle("reldiff dist fit mean [%]");
			it->second.mean->GetYaxis()->SetTitleOffset(1.5);
			axis_y1 = it->second.mean->GetYaxis();
			it->second.mean->GetXaxis()->SetTitle("lower fit range [mrad]");
			it->second.mean->GetXaxis()->SetTitleOffset(1.1);

			gPad->Update();
			TLine *zero = new TLine(gPad->GetUxmin(), 0, gPad->GetUxmax(), 0);
			zero->Draw();
		} else
			it->second.mean->Draw("CSAME*");

		legend1->AddEntry(it->second.mean, it->second.settings, "pe");

		/*if (it->second.sigma && it->second.lumifit_error) {
		 c->cd(2);

		 it->second.sigma->SetLineColor(color);
		 it->second.sigma->SetMarkerColor(color);
		 it->second.sigma->SetMarkerStyle(5);
		 it->second.sigma->SetMarkerSize(1.0);
		 legend2->AddEntry(it->second.sigma,
		 it->second.settings + " samples fit mean #sigma", "lep");

		 it->second.lumifit_error->SetLineColor(color);
		 it->second.lumifit_error->SetLineStyle(2);
		 it->second.lumifit_error->SetMarkerColor(color);
		 it->second.lumifit_error->SetMarkerStyle(5);
		 it->second.lumifit_error->SetMarkerSize(1.0);
		 legend2->AddEntry(it->second.lumifit_error,
		 it->second.settings + " mean lumifit error", "l");

		 if (first) {
		 it->second.sigma->Draw("AC*");
		 it->second.sigma->GetYaxis()->SetTitle("reldiff dist fit #sigma [%]");
		 it->second.sigma->GetYaxis()->SetTitleOffset(1.5);
		 axis_y2 = it->second.sigma->GetYaxis();
		 it->second.sigma->GetXaxis()->SetTitle("lower fit range [mrad]");
		 it->second.sigma->GetXaxis()->SetTitleOffset(1.1);
		 } else
		 it->second.sigma->Draw("CSAME*");

		 it->second.lumifit_error->Draw("CSAME*");
		 }*/

		test->cd();
		it->second.mean->Draw("AC*");
		gPad->Update();

		if (first) {
			c1_ymax = gPad->GetUymax();
			c1_ymin = gPad->GetUymin();
		} else {
			if (c1_ymax < gPad->GetUymax())
				c1_ymax = gPad->GetUymax();
			if (c1_ymin > gPad->GetUymin())
				c1_ymin = gPad->GetUymin();
		}

		/*if (it->second.sigma && it->second.lumifit_error) {
			it->second.sigma->Draw("AC*");
			gPad->Update();

			if (first) {
				c2_ymax = gPad->GetUymax();
				c2_ymin = gPad->GetUymin();
			} else {
				if (c2_ymax < gPad->GetUymax())
					c2_ymax = gPad->GetUymax();
				if (c2_ymin > gPad->GetUymin())
					c2_ymin = gPad->GetUymin();
			}

			it->second.lumifit_error->Draw("AC*");
			gPad->Update();

			if (c2_ymax < gPad->GetUymax())
				c2_ymax = gPad->GetUymax();
			if (c2_ymin > gPad->GetUymin())
				c2_ymin = gPad->GetUymin();

		}*/
		first = false;
	}

	axis_y1->SetRangeUser(c1_ymin, c1_ymax);
	//axis_y2->SetRangeUser(c2_ymin, c2_ymax);
	gPad->Update();

	c->cd();
	legend1->Draw();
	//c->cd(2);
	//legend2->Draw();

	return c;
}

void combineLumiFitResults(std::vector<std::string> paths) {
	std::cout << "Generating lumi comparison plots for fit results....\n";

	int colors[] = { 1, 2, 8, 9, 6, 7, 30, 38, 46 };

	std::map<int, GraphBundle> mc_scenarios;
	std::map<int, GraphBundle> mc_acc_scenarios;
	std::map<int, GraphBundle> reco_scenarios;

	for (unsigned int j = 0; j < paths.size(); j++) {
		TFile *f = new TFile(paths[j].c_str(), "READ");
		mc_scenarios[colors[j]] = getGraphsFromFile(f, "mc");
		mc_acc_scenarios[colors[j]] = getGraphsFromFile(f, "mc_acc");
		reco_scenarios[colors[j]] = getGraphsFromFile(f, "reco");
	}

	TCanvas *c_dep = makeDependencyCanvas(mc_scenarios);
	c_dep->SaveAs("lumidist_dependency_mc.pdf");

	c_dep = makeDependencyCanvas(mc_acc_scenarios);
	c_dep->SaveAs("lumidist_dependency_mc_acc.pdf");

	c_dep = makeDependencyCanvas(reco_scenarios);
	c_dep->SaveAs("lumidist_dependency_reco.pdf");
}

int main(int argc, char* argv[]) {
	std::vector<std::string> paths;
	for (int i = 1; i < argc; i++) {
		paths.push_back(std::string(argv[i]));
	}
	combineLumiFitResults(paths);
	return 0;
}
