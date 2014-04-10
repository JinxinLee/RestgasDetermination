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
#include "PndLmdVertexData.h"
#include "PndLmdDataFacade.h"

#include <vector>
#include <map>
#include <iostream>

#include "TFile.h"
#include "TString.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TLine.h"

/*void drawGraphRow(TCanvas *c, int i, std::vector<TGraphErrors*> &graphs,
 std::vector<std::string> &infos, int row_length) {
 //c->Divide(5,4);
 for (int j = 0; j < graphs.size(); j++) {
 c->cd(row_length * i + j + 1);
 std::cout << "Drawing into subpad # " << row_length * i + j + 1
 << std::endl;
 graphs[j]->Draw("A*");
 }
 std::cout << "Drawing into subpad # " << row_length * (i + 1) << std::endl;
 c->cd(row_length * (i + 1));
 for (int j = 0; j < infos.size(); j++) {
 TLatex *l = new TLatex(0.2, 0.82 - j * 0.08, infos[j].c_str());
 l->Draw();
 }
 }

 void drawGraphColumn(TCanvas *c, int i, std::vector<TGraphErrors*> &graphs,
 std::vector<std::string> &infos, int row_length) {
 for (int j = 0; j < graphs.size(); j++) {
 c->cd((row_length * j) + i + 1);
 gPad->SetBorderSize(0);
 //std::cout<<"Drawing into subpad # "<<5*i+j+1<<std::endl;
 //if first column then keep y axis labels ... otherwise drop them
 if (i != 0) {
 graphs[j]->GetYaxis()->SetTitle("");
 gPad->SetLeftMargin(0);
 gPad->SetRightMargin(0);
 gPad->SetTicky(2);
 } else {
 gPad->SetLeftMargin(0.23);
 gPad->SetRightMargin(0);
 }
 if (j < graphs.size() - 1) {
 graphs[j]->GetXaxis()->SetTitle("");
 gPad->SetTopMargin(0);
 gPad->SetBottomMargin(0);
 gPad->SetTickx(2);
 } else {
 gPad->SetTopMargin(0);
 gPad->SetBottomMargin(0.26);
 }
 graphs[j]->GetYaxis()->SetLabelOffset(0.01);
 graphs[j]->GetXaxis()->SetLabelOffset(0.01);
 graphs[j]->Draw("A*");

 c->Update();
 // if its row 0 or 2 then we want a zero line
 if (j == 0 || j == 2) {
 TLine *line = new TLine(gPad->GetUxmin(), 0.0, gPad->GetUxmax(), 0.0);
 std::cout << "Drawing line from " << gPad->GetUxmin() << " to "
 << gPad->GetUxmax() << std::endl;
 line->Draw();
 }
 if (j == 0 || j == 3) {
 TLatex *l = new TLatex(
 gPad->GetUxmin() + (gPad->GetUxmax() - gPad->GetUxmin()) * 0.2,
 gPad->GetUymin() + (gPad->GetUymax() - gPad->GetUymin()) * 0.73,
 infos[j].c_str());
 l->SetTextSize(0.13);
 if (infos[j].find("varied") > 0
 && infos[j].find("varied") < infos[j].size())
 l->SetTextColor(2);
 l->Draw();
 } else {
 TLatex *l = new TLatex(
 gPad->GetUxmin() + (gPad->GetUxmax() - gPad->GetUxmin()) * 0.2,
 gPad->GetUymin() + (gPad->GetUymax() - gPad->GetUymin()) * 0.15,
 infos[j].c_str());
 l->SetTextSize(0.13);
 if (infos[j].find("varied") > 0
 && infos[j].find("varied") < infos[j].size())
 l->SetTextColor(2);
 l->Draw();
 }

 }
 //std::cout<<"Drawing into subpad # "<<5*(i+1)<<std::endl;
 //c->cd((row_length*graphs.size())+i+1);
 //for(int j = 0; j < infos.size(); j++) {
 //		TLatex *l = new TLatex(0.2, 0.8-j*0.18, infos[j].c_str());
 //                l->SetTextSize(0.15);
 //		l->Draw();
 //	}
 }*/

/*void makeOverviewCanvas(
 std::vector<std::pair<std::vector<std::string>, int> > &dep) {
 //gStyle->SetPadLeftMargin(1);
 //gStyle->SetPadRightMargin(0);
 //gStyle->SetPadBottomMargin(1);
 //gStyle->SetPadTopMargin(0);
 //gStyle->SetPadBorderMode(0);
 //gStyle->SetOptStat(0);
 //gStyle->SetPadLeftMargin(1.5);
 TCanvas *c = new TCanvas("c", "c", 800, 700);
 std::cout << "Dividing canvas into 4 by " << dep.size() << " pads!"
 << std::endl;
 c->Divide(dep.size(), 4, 0, 0);
 std::pair<std::vector<TGraphErrors*>, std::pair<std::string, int> > graphs;
 for (int i = 0; i < dep.size(); i++) {
 std::vector<std::string> infos;
 /*	graphs = makeGraphs(dep[i].first, dep[i].second);
 char cc[30];
 sprintf(cc, "x^{MC}_{off} = %.2f cm", getTrueValue(dep[i].first[0], 1));
 infos.push_back(cc);
 sprintf(cc, "#sigma^{MC}_{x} = %.2f cm", getTrueValue(dep[i].first[0], 3));
 infos.push_back(cc);
 sprintf(cc, "y^{MC}_{off} = %.2f cm", getTrueValue(dep[i].first[0], 2));
 infos.push_back(cc);
 sprintf(cc, "#sigma^{MC}_{y} = %.2f cm", getTrueValue(dep[i].first[0], 4));
 infos.push_back(cc);
 /*sprintf(cc, "beam gradiant mean_{x} = %.2f", getTrueValue(dep[i].first[0], 5));
 infos.push_back(cc);
 sprintf(cc, "beam gradiant mean_{y} = %.2f", getTrueValue(dep[i].first[0], 6));
 infos.push_back(cc);
 sprintf(cc, "beam gradiant #sigma_{x} = %.2f", getTrueValue(dep[i].first[0], 7));
 infos.push_back(cc);
 sprintf(cc, "beam gradiant #sigma_{y} = %.2f", getTrueValue(dep[i].first[0], 8));
 infos.push_back(cc);*/
// ok rename the text for the variated variable...
/*			std::string temps(infos[graphs.second.second - 1]);
 temps = temps.substr(0, temps.find_first_of("="));
 temps += "was varied";
 infos[graphs.second.second - 1] = temps;
 drawGraphColumn(c, i, graphs.first, infos, dep.size());
 }
 c->cd(0);
 c->SaveAs("overview.pdf");
 c->SaveAs("overview.eps");
 }*/

void plotIPDistribution(std::vector<TString> paths) {
	std::cout << "Generating lumi plots for fit results....\n";

	PndLmdDataFacade lmd_data_facade;

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
	plotter.setThetaPlotRange(-2.0, 2.0);

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

	// ok here we should allow for multiple input root files of some pattern
	// which can then all be combined to results (different ip properties)

	std::vector<PndLmdVertexData> data_vec;

	for (unsigned int i = 0; i < paths.size(); i++) {
		// ------ get files -------------------------------------------------------
		TFile *fdata = new TFile(paths[i] + "/lmd_fitted_vertex_data.root",
				"UPDATE");

		// read in data from a root file which will return a vector of pointers to PndLmdAngularData objects
		std::vector<PndLmdVertexData> file_data = lmd_data_facade.getDataFromFile<
				PndLmdVertexData>(fdata);

		std::vector<std::map<int, PndLmdResultPlotter::graph_bundle> > graph_bundles_vec;
		graph_bundles_vec.push_back(plotter.makeVertexGraphBundles1D(file_data));

		if (file_data.size() > 0) {
			std::stringstream filepath;
			filepath << "ip-tilt/ip-fit_results-";
			filepath << file_data[0].getSimulationIPParameters().getLabel();

			plotter.makeVertexFitResultBooky(graph_bundles_vec,
					filepath.str().c_str());
		}

		data_vec.insert(data_vec.end(), file_data.begin(), file_data.end());
	}

	std::map<LumiFit::LmdSimIPParameters,
			std::map<LumiFit::LmdSimIPParameters,
					std::map<LumiFit::LmdDimensionType, std::vector<PndLmdVertexData> > > > clustered_ip_data =
			lmd_data_facade.clusterVertexData(data_vec);

	// =============================== BEGIN PLOTTING =============================== //
	// if you only have a single data object (mostly the case)

	std::cout << clustered_ip_data.size() << " "
			<< clustered_ip_data.begin()->second.size() << " "
			<< clustered_ip_data.begin()->second.begin()->second.size() << " "
			<< clustered_ip_data.begin()->second.begin()->second.begin()->second.size()
			<< std::endl;

	plotter.plotIPDependencyGraphs(clustered_ip_data);
	// ================================ END PLOTTING ================================ //
}

int main(int argc, char* argv[]) {
	if (argc > 1) {
		std::vector<TString> data_paths;
		for (int i = 1; i < argc; i++)
			data_paths.push_back(TString(argv[i]));
		plotIPDistribution(data_paths);
	}

	return 0;
}
