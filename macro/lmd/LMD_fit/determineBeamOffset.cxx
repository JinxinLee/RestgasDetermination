#include <iostream>
#include <vector>
#include <string>
#include <cmath>

#include <boost/regex.hpp>

#include "TROOT.h"
#include "TKey.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGraphErrors.h"
#include "TString.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TLine.h"

struct beam_params {
	double offset_x_mean;
	double offset_x_sigma;
	double offset_y_mean;
	double offset_y_sigma;

	double tilt_x_mean;
	double tilt_x_sigma;
	double tilt_y_mean;
	double tilt_y_sigma;
};

beam_params getTrueValues(std::string input) {
	beam_params bp;

	// naming scheme is beam_offsetXYDXDY_...._gradXYDXDY.... so order is meanX meanY
	// sigmaX sigmaY for both offset and tilt
	boost::regex e(
			"^.*beam_offsetXYDXDY_(-?\\d\\.\\d+)_(-?\\d\\.\\d+)_(\\d\\.\\d+)_(\\d\\.\\d+)_gradXYDXDY_(-?\\d\\.\\d+)_(-?\\d\\.\\d+)_(\\d\\.\\d+)_(\\d\\.\\d+).*$");
	boost::match_results<std::string::const_iterator> what;
	if (0 == boost::regex_match(input, what, e)) {
		// the input so far could not possibly be valid so reject it:
		throw std::runtime_error(
				"Invalid data entered - this could not possibly be a valid card number");
	}
	// OK so far so good, but have we finished?
	else {
		// excellent, we have a result:

		bp.offset_x_mean = std::atof(
				std::string(what[1].first, what[1].second).c_str());
		bp.offset_y_mean = std::atof(
				std::string(what[2].first, what[2].second).c_str());
		bp.offset_x_sigma = std::atof(
				std::string(what[3].first, what[3].second).c_str());
		bp.offset_x_sigma = std::atof(
				std::string(what[4].first, what[4].second).c_str());

		bp.tilt_x_mean = std::atof(
				std::string(what[5].first, what[5].second).c_str());
		bp.tilt_y_mean = std::atof(
				std::string(what[6].first, what[6].second).c_str());
		bp.tilt_x_sigma = std::atof(
				std::string(what[7].first, what[7].second).c_str());
		bp.tilt_y_sigma = std::atof(
				std::string(what[8].first, what[8].second).c_str());
	}
	return bp;
}

std::pair<std::vector<TGraphErrors*>, std::pair<std::string, int> > makeGraphs(
		std::vector<std::string> &dirs, int dependent_var) {
	double x[dirs.size()];
	double vx_y_reco[dirs.size()];
	double vx_y_errors_reco[dirs.size()];
	double vx_y_sigma_reco[dirs.size()];
	double vx_y_errors_sigma_reco[dirs.size()];
	double vx_y_mc[dirs.size()];
	double vx_y_errors_mc[dirs.size()];
	double vx_y_sigma_mc[dirs.size()];
	double vx_y_errors_sigma_mc[dirs.size()];
	double vy_y_reco[dirs.size()];
	double vy_y_errors_reco[dirs.size()];
	double vy_y_sigma_reco[dirs.size()];
	double vy_y_errors_sigma_reco[dirs.size()];
	double vy_y_mc[dirs.size()];
	double vy_y_errors_mc[dirs.size()];
	double vy_y_sigma_mc[dirs.size()];
	double vy_y_errors_sigma_mc[dirs.size()];

	double sigma_high = 0.0, sigma_low = 10e20, mean_high = 0.0, mean_low = 10e20;

	std::vector<std::pair<TH1I*, TH1I*> > hists;
	std::vector<std::pair<TF1*, TF1*> > fits;

	TH1I* hist_vertex_x_reco, *hist_vertex_x_mc;
	TH1I* hist_vertex_y_reco, *hist_vertex_y_mc;

	for (int i = 0; i < dirs.size(); i++) {
		// ---- Input files file ----------------------------------------------------------------
		TString in = dirs[i] + "/plots.root";
		TFile *f = new TFile(in, "OPEN");
		// ---------------------------------------------------------------------------------

		std::cout << "opening " << in << std::endl;
		// get histograms
		hist_vertex_x_reco = (TH1I*) (f->GetKey(
				"track_pos_x; Reco track vertex x pos [cm]")->ReadObj());
		hist_vertex_x_mc = (TH1I*) (f->GetKey(
				"pbarMCtrack_pos_x; MC track vertex x pos [cm]")->ReadObj());
		hist_vertex_y_reco = (TH1I*) (f->GetKey(
				"track_pos_y; Reco track vertex y pos [cm]")->ReadObj());
		hist_vertex_y_mc = (TH1I*) (f->GetKey(
				"pbarMCtrack_pos_y; MC track vertex y pos [cm]")->ReadObj());
		hists.push_back(std::make_pair(hist_vertex_x_reco, hist_vertex_x_mc));
		hists.push_back(std::make_pair(hist_vertex_y_reco, hist_vertex_y_mc));

		// do fit
		std::pair<double, double> tvals = getTrueValues(dirs[i], 1);
		TF1* fit_x_reco = doFit(hist_vertex_x_reco, tvals.first,
				tvals.second + 0.4);
		TF1* fit_x_mc = doFit(hist_vertex_x_mc, tvals.first, tvals.second);
		tvals = getTrueValues(dirs[i], 2);
		TF1* fit_y_reco = doFit(hist_vertex_y_reco, tvals.first,
				tvals.second + 0.4);
		TF1* fit_y_mc = doFit(hist_vertex_y_mc, tvals.first, tvals.second);

		int tempdepvar = dependent_var;
		if (dependent_var == 2)
			tempdepvar = 3;
		else if (dependent_var == 3)
			tempdepvar = 2;
		x[i] = getTrueValue(dirs[i], tempdepvar);
		vx_y_reco[i] = fit_x_reco->GetParameter(1);
		vx_y_errors_reco[i] = fit_x_reco->GetParError(1);
		vx_y_sigma_reco[i] = fit_x_reco->GetParameter(2);
		vx_y_errors_sigma_reco[i] = fit_x_reco->GetParError(2);
		vx_y_mc[i] = fit_x_mc->GetParameter(1);
		vx_y_errors_mc[i] = fit_x_mc->GetParError(1);
		vx_y_sigma_mc[i] = fit_x_mc->GetParameter(2);
		vx_y_errors_sigma_mc[i] = fit_x_mc->GetParError(2);
		vy_y_reco[i] = fit_y_reco->GetParameter(1);
		vy_y_errors_reco[i] = fit_y_reco->GetParError(1);
		vy_y_sigma_reco[i] = fit_y_reco->GetParameter(2);
		vy_y_errors_sigma_reco[i] = fit_y_reco->GetParError(2);
		vy_y_mc[i] = fit_y_mc->GetParameter(1);
		vy_y_errors_mc[i] = fit_y_mc->GetParError(1);
		vy_y_sigma_mc[i] = fit_y_mc->GetParameter(2);
		vy_y_errors_sigma_mc[i] = fit_y_mc->GetParError(2);

		if (vx_y_reco[i] > mean_high)
			mean_high = vx_y_reco[i];
		else if (vx_y_reco[i] < mean_low)
			mean_low = vx_y_reco[i];
		if (vx_y_mc[i] > mean_high)
			mean_high = vx_y_mc[i];
		else if (vx_y_mc[i] < mean_low)
			mean_low = vx_y_mc[i];

		if (vx_y_sigma_reco[i] > sigma_high)
			sigma_high = vx_y_sigma_reco[i];
		else if (vx_y_sigma_reco[i] < sigma_low)
			sigma_low = vx_y_sigma_reco[i];
		if (vx_y_sigma_mc[i] > sigma_high)
			sigma_high = vx_y_sigma_mc[i];
		else if (vx_y_sigma_mc[i] < sigma_low)
			sigma_low = vx_y_sigma_mc[i];

		if (vy_y_reco[i] > mean_high)
			mean_high = vy_y_reco[i];
		else if (vy_y_reco[i] < mean_low)
			mean_low = vy_y_reco[i];
		if (vy_y_mc[i] > mean_high)
			mean_high = vy_y_mc[i];
		else if (vy_y_mc[i] < mean_low)
			mean_low = vy_y_mc[i];

		if (vy_y_sigma_reco[i] > sigma_high)
			sigma_high = vy_y_sigma_reco[i];
		else if (vy_y_sigma_reco[i] < sigma_low)
			sigma_low = vy_y_sigma_reco[i];
		if (vy_y_sigma_mc[i] > sigma_high)
			sigma_high = vy_y_sigma_mc[i];
		else if (vy_y_sigma_mc[i] < sigma_low)
			sigma_low = vy_y_sigma_mc[i];

		fits.push_back(std::make_pair(fit_x_reco, fit_x_mc));
		fits.push_back(std::make_pair(fit_y_reco, fit_y_mc));
	}

	double vx_mean_difference_y[dirs.size()];
	double vx_mean_difference_y_errors[dirs.size()];
	double vx_sigma_difference_y[dirs.size()];
	double vx_sigma_difference_y_errors[dirs.size()];
	double vy_mean_difference_y[dirs.size()];
	double vy_mean_difference_y_errors[dirs.size()];
	double vy_sigma_difference_y[dirs.size()];
	double vy_sigma_difference_y_errors[dirs.size()];

	for (int i = 0; i < dirs.size(); i++) {
		vx_mean_difference_y[i] = vx_y_reco[i] - vx_y_mc[i];
		vx_mean_difference_y_errors[i] = sqrt(
				vx_y_errors_reco[i] * vx_y_errors_reco[i]
						+ vx_y_errors_mc[i] * vx_y_errors_mc[i]);
		vx_sigma_difference_y[i] = vx_y_sigma_reco[i] - vx_y_sigma_mc[i];
		vx_sigma_difference_y_errors[i] = sqrt(
				vx_y_errors_sigma_reco[i] * vx_y_errors_sigma_reco[i]
						+ vx_y_errors_sigma_mc[i] * vx_y_errors_sigma_mc[i]);
		vy_mean_difference_y[i] = vy_y_reco[i] - vy_y_mc[i];
		vy_mean_difference_y_errors[i] = sqrt(
				vy_y_errors_reco[i] * vy_y_errors_reco[i]
						+ vy_y_errors_mc[i] * vy_y_errors_mc[i]);
		vy_sigma_difference_y[i] = vy_y_sigma_reco[i] - vy_y_sigma_mc[i];
		vy_sigma_difference_y_errors[i] = sqrt(
				vy_y_errors_sigma_reco[i] * vy_y_errors_sigma_reco[i]
						+ vy_y_errors_sigma_mc[i] * vy_y_errors_sigma_mc[i]);
	}

	TGraphErrors *comparison_graph_vx_mean_diff = new TGraphErrors(dirs.size(), x,
			vx_mean_difference_y, 0, vx_mean_difference_y_errors);
	TGraphErrors *comparison_graph_vx_sigma_diff = new TGraphErrors(dirs.size(),
			x, vx_sigma_difference_y, 0, vx_sigma_difference_y_errors);
	TGraphErrors *comparison_graph_vy_mean_diff = new TGraphErrors(dirs.size(), x,
			vy_mean_difference_y, 0, vy_mean_difference_y_errors);
	TGraphErrors *comparison_graph_vy_sigma_diff = new TGraphErrors(dirs.size(),
			x, vy_sigma_difference_y, 0, vy_sigma_difference_y_errors);

	std::string xtitle;

	if (dependent_var == 1) {
		xtitle = "x^{MC}_{off} [cm]";
	} else if (dependent_var == 3) {
		xtitle = "y^{MC}_{off} [cm]";
	} else if (dependent_var == 2) {
		xtitle = "#sigma^{MC}_{x} [cm]";
	} else if (dependent_var == 4) {
		xtitle = "#sigma^{MC}_{y} [cm]";
	} else if (dependent_var == 5) {
		xtitle = "generated beam grad mean x [mrad]";
	}

	comparison_graph_vx_mean_diff->SetTitle("");
	comparison_graph_vx_mean_diff->GetXaxis()->SetTitle((xtitle).c_str());
	comparison_graph_vx_mean_diff->GetYaxis()->SetTitle(
			"#Delta x_{off} (Rec-MC) [cm]");
	comparison_graph_vx_mean_diff->GetXaxis()->SetLabelSize(0.1);
	comparison_graph_vx_mean_diff->GetYaxis()->SetLabelSize(0.1);
	comparison_graph_vx_mean_diff->GetXaxis()->SetTitleSize(0.1);
	comparison_graph_vx_mean_diff->GetYaxis()->SetTitleSize(0.1);
	comparison_graph_vx_mean_diff->GetXaxis()->SetTitleOffset(1.2);
	comparison_graph_vx_mean_diff->GetYaxis()->SetTitleOffset(1.2);
	comparison_graph_vx_mean_diff->GetYaxis()->SetRangeUser(-0.035, 0.035);

	comparison_graph_vx_sigma_diff->SetTitle("");
	comparison_graph_vx_sigma_diff->GetXaxis()->SetTitle((xtitle).c_str());
	comparison_graph_vx_sigma_diff->GetYaxis()->SetTitle(
			"#Delta #sigma_{x} (Rec-MC) [cm]");
	comparison_graph_vx_sigma_diff->GetXaxis()->SetLabelSize(0.1);
	comparison_graph_vx_sigma_diff->GetYaxis()->SetLabelSize(0.1);
	comparison_graph_vx_sigma_diff->GetXaxis()->SetTitleSize(0.1);
	comparison_graph_vx_sigma_diff->GetYaxis()->SetTitleSize(0.1);
	comparison_graph_vx_sigma_diff->GetXaxis()->SetTitleOffset(1.2);
	comparison_graph_vx_sigma_diff->GetYaxis()->SetTitleOffset(1.2);
	comparison_graph_vx_sigma_diff->GetYaxis()->SetRangeUser(0.2, 0.45);

	comparison_graph_vy_mean_diff->SetTitle("");
	comparison_graph_vy_mean_diff->GetXaxis()->SetTitle((xtitle).c_str());
	comparison_graph_vy_mean_diff->GetYaxis()->SetTitle(
			"#Delta y_{off} (Rec-MC) [cm]");
	comparison_graph_vy_mean_diff->GetXaxis()->SetLabelSize(0.1);
	comparison_graph_vy_mean_diff->GetYaxis()->SetLabelSize(0.1);
	comparison_graph_vy_mean_diff->GetXaxis()->SetTitleSize(0.1);
	comparison_graph_vy_mean_diff->GetYaxis()->SetTitleSize(0.1);
	comparison_graph_vy_mean_diff->GetXaxis()->SetTitleOffset(1.2);
	comparison_graph_vy_mean_diff->GetYaxis()->SetTitleOffset(1.2);
	comparison_graph_vy_mean_diff->GetYaxis()->SetRangeUser(-0.035, 0.035);

	comparison_graph_vy_sigma_diff->SetTitle("");
	comparison_graph_vy_sigma_diff->GetXaxis()->SetTitle((xtitle).c_str());
	comparison_graph_vy_sigma_diff->GetYaxis()->SetTitle(
			"#Delta #sigma_{y} (Rec-MC) [cm]");
	comparison_graph_vy_sigma_diff->GetXaxis()->SetLabelSize(0.1);
	comparison_graph_vy_sigma_diff->GetYaxis()->SetLabelSize(0.1);
	comparison_graph_vy_sigma_diff->GetXaxis()->SetTitleSize(0.1);
	comparison_graph_vy_sigma_diff->GetYaxis()->SetTitleSize(0.1);
	comparison_graph_vy_sigma_diff->GetXaxis()->SetTitleOffset(1.2);
	comparison_graph_vy_sigma_diff->GetYaxis()->SetTitleOffset(1.2);
	comparison_graph_vy_sigma_diff->GetYaxis()->SetRangeUser(0.2, 0.45);

	std::vector<TGraphErrors*> vec;
	vec.push_back(comparison_graph_vx_mean_diff);
	vec.push_back(comparison_graph_vx_sigma_diff);
	vec.push_back(comparison_graph_vy_mean_diff);
	vec.push_back(comparison_graph_vy_sigma_diff);
	return std::make_pair(vec, std::make_pair(dirs[0], dependent_var));
}

void drawGraphRow(TCanvas *c, int i, std::vector<TGraphErrors*> &graphs,
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
}

void makeOverviewCanvas(
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
		graphs = makeGraphs(dep[i].first, dep[i].second);
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
		std::string temps(infos[graphs.second.second - 1]);
		temps = temps.substr(0, temps.find_first_of("="));
		temps += "was varied";
		infos[graphs.second.second - 1] = temps;
		drawGraphColumn(c, i, graphs.first, infos, dep.size());
	}
	c->cd(0);
	c->SaveAs("overview.pdf");
	c->SaveAs("overview.eps");
}

TH1D* createHist(TString &dirname) {
	// ---- Input files --------------------------------------------------------
	TString simMC = dirname + "/Lumi_MC_*.root";
	TChain tMC("cbmsim");
	tMC.Add(simMC);
	tMC.SetBranchStatus("*", 0);

	TString recoFile = dirname + "/Lumi_Track_*.root";
	TChain tTrack("cbmsim");
	tTrack.Add(recoFile);
	tTrack.SetBranchStatus("*", 0);

	TString geaneFile = dirname + "/Lumi_Geane_*.root";
	TChain tgeane("cbmsim");
	tgeane.Add(geaneFile);
	tgeane.SetBranchStatus("*", 0);

	// ---------------------------------------------------------------------------------
	// ---- Output file ----------------------------------------------------------------
	TString out = storePath + "/" + output_filename;
	TFile *f = new TFile(out, "RECREATE");
	// ---------------------------------------------------------------------------------

	//--- MC info --------------------------------------------------------------------
	TClonesArray* true_tracks = new TClonesArray("PndMCTrack");
	tMC.SetBranchAddress("MCTrack", &true_tracks); //True Track to compare
	tMC.SetBranchStatus("MCTrack*", 1);

	TClonesArray* true_points = new TClonesArray("PndSdsMCPoint");
	tMC.SetBranchAddress("LMDPoint", &true_points); //True Points to compare
	tMC.SetBranchStatus("LMDPoint*", 1);
	//--------------------------------------------------------------------------------

	//--- Track info -----------------------------------------------------------------
	TClonesArray* reco_tracks = new TClonesArray("PndTrack");
	tTrack.SetBranchAddress("LMDPndTrack", &reco_tracks); //Reco Track to compare (at lumi monitor system, so not backtracked)
	tTrack.SetBranchStatus("LMDPndTrack*", 1);
	//--------------------------------------------------------------------------------

	//--- Geane info -----------------------------------------------------------------
	// TClonesArray* geaneArray =new TClonesArray("FairTrackParP");
	TClonesArray* geaneArray = new TClonesArray("FairTrackParH");
	tgeane.SetBranchAddress("GeaneTrackFinal", &geaneArray); //Tracks with parabolic parametrisation
	tgeane.SetBranchStatus("GeaneTrackFinal*", 1); //Tracks with parabolic parametrisation

	std::cout << "Processing " << nEvents << " events" << std::endl;
	for (Int_t j = 0; j < nEvents; j++) {
		//  cout<<"Event #"<<j<<endl;
		if (j % 50000 == 0)

		// Read GEANE & MC info -----------------------------------------------------------------
		tgeane.GetEntry(j);
		tMC.GetEntry(j);
		tTrack.GetEntry(j);
		// number of reconstucted tracks (backtracked with geane)
		const int nGeaneTrks = geaneArray->GetEntriesFast();
		//cout<<"number of geane tracks: "<<nGeaneTrks<<endl;
		// number of MC tracks
		const int nParticles = true_tracks->GetEntriesFast();
		//cout<<"number of MC tracks: "<<nParticles<<endl;
		//
		//   if(nParticles < 2) continue;
		gHist.Fill("num_tracks_2d;# of geane tracks; # of MC tracks", nGeaneTrks,
				nParticles, 20, 0, 20, 20, 0, 20);

		TLorentzVector beam;
		TLorentzVector proton_target;
		proton_target.SetXYZM(0, 0, 0, 0.938);

		TVector3 mc_vertex;

		///-- Read info about MC tracks -------------------------------------------
		for (Int_t jk = 0; jk < nParticles; jk++) {
			PndMCTrack *mctrk = (PndMCTrack*) true_tracks->At(jk);

			beam = beam + mctrk->Get4Momentum();
			Int_t mcID = mctrk->GetPdgCode();
			TVector3 MomMC_all = mctrk->GetMomentum();
			TVector3 StartMC_all = mctrk->GetStartVertex();
			mc_vertex = StartMC_all;

			if (mcID == 2212 && mctrk->IsGeneratorCreated()) {
				double t = (mctrk->Get4Momentum() - proton_target)
						* (mctrk->Get4Momentum() - proton_target);
				gHist.Fill("pMCtrack_t; MC abs momentum transfer t [GeV^{2}/c^{4}]",
						TMath::Abs(t), 400, 0, 0.1);

				gHist.Fill("pMCtrack_mom_x; MC track momentum x [GeV/c]", MomMC_all.X(),
						200, 0, 1.5);
				gHist.Fill("pMCtrack_mom_y; MC track momentum y [GeV/c]", MomMC_all.Y(),
						200, 0, 1.5);
				gHist.Fill("pMCtrack_mom_z; MC track momentum z [GeV/c]", MomMC_all.Z(),
						200, 0, 10);

				gHist.Fill("pMCtrack_theta; MC track theta [rad]", MomMC_all.Theta(),
						400, 0, 3.2);
				gHist.Fill("pMCtrack_phi; MC track phi [rad]", MomMC_all.Phi(), 400,
						-3.2, 3.2);
				gHist.Fill("pMCtrack_pos_x; MC track vertex x pos [cm]",
						StartMC_all.X(), 400, -3.0, 3.0);
				gHist.Fill("pMCtrack_pos_y; MC track vertex y pos [cm]",
						StartMC_all.Y(), 400, -3.0, 3.0);
				gHist.Fill("pMCtrack_pos_z; MC track vertex z pos [cm]",
						StartMC_all.Z(), 400, -3.0, 3.0);
			}
			if (mcID == -2212 && mctrk->IsGeneratorCreated()) {
				gHist.Fill("pbarMCtrack_mom_x; MC track momentum x [GeV/c]",
						MomMC_all.X(), 200, 0, 1.5);
				gHist.Fill("pbarMCtrack_mom_y; MC track momentum y [GeV/c]",
						MomMC_all.Y(), 200, 0, 1.5);
				gHist.Fill("pbarMCtrack_mom_z; MC track momentum z [GeV/c]",
						MomMC_all.Z(), 200, 0, 10);

				gHist.Fill("pbarMCtrack_theta; MC track theta [rad]", MomMC_all.Theta(),
						600, 0, 1.6);
				gHist.Fill("pbarMCtrack_theta_zoom; MC track theta [rad]",
						MomMC_all.Theta(), 600, 0.0, 0.02);
				gHist.Fill("pbarMCtrack_phi; MC track phi [rad]", MomMC_all.Phi(), 400,
						-3.2, 3.2);
				gHist.Fill("pbarMCtrack_pos_x; MC track vertex x pos [cm]",
						StartMC_all.X(), 400, -3.0, 3.0);
				gHist.Fill("pbarMCtrack_pos_y; MC track vertex y pos [cm]",
						StartMC_all.Y(), 400, -3.0, 3.0);
				gHist.Fill("pbarMCtrack_pos_z; MC track vertex z pos [cm]",
						StartMC_all.Z(), 400, -3.0, 3.0);
				gHist.Fill(
						"pbarMCtrack_pos_x_vs_pos_y; MC track vertex x pos [cm]; MC track vertex y pos [cm]",
						StartMC_all.X(), StartMC_all.Y(), 200, -3.0, 3.0, 200, -3.0, 3.0);

				if (true_points->GetEntriesFast() > 0) {
					gHist.Fill("pbarMCtrack_theta_zoom_reco_only; MC track theta [rad]",
							MomMC_all.Theta(), 600, 0.0, 0.02);
				}
			}

			if (nGeaneTrks > 0) {
				gHist.Fill("MCtrack_mom_x_reco_success; MC track momentum x [GeV/c]",
						MomMC_all.X(), 200, -0.2, 0.2);
				gHist.Fill("MCtrack_mom_y_reco_success; MC track momentum y [GeV/c]",
						MomMC_all.Y(), 200, -0.2, 0.2);
				gHist.Fill("MCtrack_mom_z_reco_success; MC track momentum z [GeV/c]",
						MomMC_all.Z(), 200, 0, 10);
			}
		}

		// ok subtract the target proton at rest
		/* beam = beam - proton_target;

		 gHist.Fill("MCbeamtrack_mom_x; MC beam track momentum x [GeV/c]", beam.Px(), 200, -0.2, 0.2);
		 gHist.Fill("MCbeamtrack_mom_y; MC beam track momentum y [GeV/c]", beam.Py(), 200, -0.2, 0.2);
		 gHist.Fill("MCbeamtrack_mom_z; MC beam track momentum z [GeV/c]", beam.Pz(), 200, 0, 20);
		 gHist.Fill("MCbeamtrack_mass; MC beam track mass [GeV/c^{2}]", beam.M(), 200, 0, 2);
		 gHist.Fill("MCbeamtrack_grad_x; MC beam track grad x [rad]", beam.Px()/beam.Pz(), 200, -0.01, 0.01);
		 gHist.Fill("MCbeamtrack_grad_y; MC beam track grad y [rad]", beam.Py()/beam.Pz(), 200, -0.01, 0.01);*/

		///------------------------------------------------------------------------
		TRotation inv_lmdrotation;
		//inv_lmdrotation.RotateY(-0.04025);
		inv_lmdrotation.RotateY(-40.068e-3);
		TVector3 inv_lmdtranslation(-26.2461, 0.000000, -1130.);

		///-- Read info about MCHits in detector planes--------------------------
		for (Int_t iN = 0; iN < true_points->GetEntriesFast(); iN++) {
			PndSdsMCPoint* mcpoint = (PndSdsMCPoint*) true_points->At(iN);
			PndMCTrack *mctrk = (PndMCTrack*) true_tracks->At(mcpoint->GetTrackID());
			if (mctrk->IsGeneratorCreated()) { // take only anti-protons from the generator
				if (mcpoint) {
					TVector3 _mcpoint((mcpoint->GetPosition())); // position at the sensor entrance

					if (1) { // translate it into the reference system of the lumi monitor
						_mcpoint = inv_lmdtranslation + _mcpoint;
						_mcpoint = inv_lmdrotation * _mcpoint;
					}

					int det_id = mcpoint->GetDetectorID(); // store the detector id
					int sens_id = mcpoint->GetSensorID(); // store the sensor id
					// calculate the plane and sensor on this plane
					int plane = sens_id / 12;
					int sensor = sens_id - plane * 12;

					char ch[100];
					sprintf(
							ch,
							"MCtrack_pos_x_vs_pos_y_at_det_plane_%i; Reco track vertex x pos [cm]; Reco track vertex y pos [cm]",
							plane);
					gHist.Fill(ch, mcpoint->GetPosition().X(), mcpoint->GetPosition().Y(),
							400, 15.0, 36.0, 400, -10.0, 10.0);

					sprintf(
							ch,
							"MCtrack_pos_x_vs_pos_y_at_det_plane_%i_rot; Reco track vertex x pos [cm]; Reco track vertex y pos [cm]",
							plane);
					gHist.Fill(ch, _mcpoint.X(), _mcpoint.Y(), 400, -10.0, 10.0, 400,
							-10.0, 10.0);

				}
			}
		}

		///-- Read info about GEANE(reconstructed) tracks--------------------------
		for (Int_t iN = 0; iN < nGeaneTrks; iN++) {
			FairTrackParH *fRes = (FairTrackParH*) geaneArray->At(iN);
			PndLinTrack *fTrack = (PndLinTrack*) reco_tracks->At(iN);
			TVector3 posRec = fRes->GetPosition();

			TVector3 posLum_rot = inv_lmdtranslation + fTrack->GetStartVec();
			posLum_rot = inv_lmdrotation * posLum_rot;

			gHist.Fill("track_pos_x; Reco track vertex x pos [cm]", posRec.X(), 200,
					-3.0, 3.0);
			gHist.Fill("track_pos_y; Reco track vertex y pos [cm]", posRec.Y(), 200,
					-3.0, 3.0);
			gHist.Fill("track_pos_z; Reco track vertex z pos [cm]", posRec.Z(), 200,
					-3.0, 3.0);
			gHist.Fill(
					"track_pos_x_vs_pos_y; Reco track vertex x pos [cm]; Reco track vertex y pos [cm]",
					posRec.X(), posRec.Y(), 200, -3.0, 3.0, 200, -3.0, 3.0);
			gHist.Fill(
					"track_pos_x_vs_pos_y_at_det; Reco track vertex x pos [cm]; Reco track vertex y pos [cm]",
					fTrack->GetStartVec().X(), fTrack->GetStartVec().Y(), 400, 15.0, 36.0,
					400, -10.0, 10.0);
			gHist.Fill(
					"track_pos_x_vs_pos_y_at_det_rot; Reco track vertex x pos [cm]; Reco track vertex y pos [cm]",
					posLum_rot.X(), posLum_rot.Y(), 400, -10.0, 10.0, 400, -10.0, 10.0);
			TVector3 MomRec = fRes->GetMomentum();
			TVector3 MomRec_rot = inv_lmdrotation * MomRec;

			gHist.Fill("track_mom_x; Reco track momentum x [GeV/c]", MomRec.X(), 200,
					0, 0.2);
			gHist.Fill("track_mom_y; Reco track momentum y [GeV/c]", MomRec.Y(), 200,
					0, 0.2);
			gHist.Fill("track_mom_z; Reco track momentum z [GeV/c]", MomRec.Z(), 200,
					0, 10);

			gHist.Fill("track_theta; Reco track theta [rad]", MomRec.Theta(), 600, 0,
					1.6);
			gHist.Fill("track_theta_zoom; Reco track theta [rad]", MomRec.Theta(),
					600, 0.0, 0.02);
			gHist.Fill("track_theta_rot_zoom; Reco track theta [rad]",
					MomRec_rot.Theta(), 600, 0.0, 0.02);
			gHist.Fill("track_phi; Reco track phi [rad]", MomRec.Phi(), 400, -3.2,
					3.2);
			gHist.Fill("track_phi_rot; Reco track phi [rad]", MomRec_rot.Phi(), 400,
					-3.2, 3.2);
			gHist.Fill(
					"track_theta_vs_phi; Reco track theta [rad]; Reco track phi [rad]",
					MomRec.Theta(), MomRec.Phi(), 600, 0.0, 0.02, 400, -3.2, 3.2);
			gHist.Fill(
					"track_theta_vs_phi_rot; Reco track theta [rad]; Reco track phi [rad]",
					MomRec_rot.Theta(), MomRec_rot.Phi(), 600, 0.0, 0.02, 400, -3.2, 3.2);
			if (1.5 < MomRec.Phi() && MomRec.Phi() < 2.5)
				gHist.Fill(
						"track_pos_x_vs_pos_y special; Reco track vertex x pos [cm]; Reco track vertex y pos [cm]",
						posRec.X(), posRec.Y(), 400, -3.0, 3.0, 400, -3.0, 3.0);

			gHist.Fill(
					"pbarMCtrack_pos_x_vs_pos_y_reco_only; MC track vertex x pos [cm]; MC track vertex y pos [cm]",
					mc_vertex.X(), mc_vertex.Y(), 400, -3.0, 3.0, 400, -3.0, 3.0);
		}
	}

	fitData()
	{

	}

	int main(int argc, char* argv[]) {
		// beam parameter case dirnames
		std::vector<std::string> beam_param_case_dirnames;
		for (int i = 1; i < argc; i++) {
			beam_param_case_dirnames.push_back(std::string(argv[i]));
		}

		std::vector<std::pair<beam_params, beam_params> > beam_param_cases;

		for (unsigned int i = 0; i < beam_param_case_dirnames.size(); i++) {
			// first create read true beam parameter values from the dirname
			beam_params bp = getTrueValues(beam_param_case_dirnames[i]);

			// read in data and create histograms
			TH1D* hist = createHist(beam_param_case_dirnames[i]);
			// then fit models to data
			fitData();
			// extract fit results, compare with "true" values
beam_param_cases		.push_back(std::make_pair(bp, )));
	}
		makeComparisonPlots();
		return 0;
	}
