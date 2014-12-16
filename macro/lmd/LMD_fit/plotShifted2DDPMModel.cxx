#include "PndLmdPlotter.h"
#include "model/PndLmdModelFactory.h"
#include "visualization/ModelVisualizationProperties1D.h"

#include <stdlib.h>
#include <utility>
#include <sstream>
#include <cmath>

#include "TVector3.h"
#include "TCanvas.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TGaxis.h"
#include "TColor.h"
#include "TLegend.h"
#include "TMath.h"

int main(int argc, char* argv[]) {
	// some style stuff
	TPad foo; // never remove this line :-)))
	if (1) {
		gROOT->SetStyle("Plain");
		const Int_t NRGBs = 5;
		const Int_t NCont = 255;
		Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
		Double_t red[NRGBs] = { 0.00, 0.00, 0.87, 1.00, 0.51 };
		Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
		Double_t blue[NRGBs] = { 0.51, 1.00, 0.12, 0.00, 0.00 };
		TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
		gStyle->SetNumberContours(NCont);
		gStyle->SetTitleFont(10 * 13 + 2, "xyz");
		gStyle->SetTitleSize(0.06, "xyz");
		gStyle->SetTitleOffset(1.3, "y");
		gStyle->SetTitleOffset(1.3, "z");
		gStyle->SetLabelFont(10 * 13 + 2, "xyz");
		gStyle->SetLabelSize(0.06, "xyz");
		gStyle->SetLabelOffset(0.009, "xyz");
		gStyle->SetPadBottomMargin(0.16);
		gStyle->SetPadTopMargin(0.16);
		gStyle->SetPadLeftMargin(0.16);
		gStyle->SetPadRightMargin(0.16);
		gStyle->SetOptTitle(1);
		gStyle->SetOptStat(0);
		gROOT->ForceStyle();
		gStyle->SetFrameFillColor(0);
		gStyle->SetFrameFillStyle(0);
		TGaxis::SetMaxDigits(3);
	}

	if (argc == 4) {
		const double plab(atof(argv[1]));
		const double tilt_x = atof(argv[2]);
		const double tilt_y = atof(argv[3]);

		const double theta_min = 0.002;
		const double theta_max = 0.01;

		DataStructs::DimensionRange plot_range_th(theta_min, theta_max);
		DataStructs::DimensionRange plot_range_phi(-TMath::Pi(), TMath::Pi());

		TVector3 tilt(tan(tilt_x), tan(tilt_y), 1.0);
		std::cout << "tilt phi: " << tilt.Phi() << std::endl;

		// create 2d model and initialize tilt parameters
		PndLmdModelFactory model_factory;

		LumiFit::PndLmdFitModelOptions fit_op_full;

		shared_ptr<Model2D> model = model_factory.generate2DModel(fit_op_full,
				plab);
		if (model->init()) {
			std::cout << "Error: not all parameters have been set!" << std::endl;
		}

		model->getModelParameterSet().freeModelParameter("tilt_x");
		model->getModelParameterSet().freeModelParameter("tilt_y");
		model->getModelParameterSet().setModelParameterValue("tilt_x", tilt_x);
		model->getModelParameterSet().setModelParameterValue("tilt_y", tilt_y);

		model->getModelParameterSet().printInfo();

		// integral - just for testing purpose
		std::vector<DataStructs::DimensionRange> int_range;
		DataStructs::DimensionRange dr_th(0.001, 0.01);
		int_range.push_back(dr_th);
		DataStructs::DimensionRange dr_phi(-TMath::Pi(), TMath::Pi());
		int_range.push_back(dr_phi);
		double integral_value = model->Integral(int_range, 1e-3);
		std::cout<<"integral: "<<integral_value<<std::endl;

		//plot result
		ROOTPlotter root_plotter;
		ModelVisualizationProperties1D vis_prop_th;
		vis_prop_th.setPlotRange(plot_range_th);

		ModelVisualizationProperties1D vis_prop_phi;
		vis_prop_phi.setPlotRange(plot_range_phi);

		std::pair<ModelVisualizationProperties1D, ModelVisualizationProperties1D> vis_prop_pair =
				std::make_pair(vis_prop_th, vis_prop_phi);

		TH2D* model_hist = root_plotter.createHistogramFromModel2D(model,
				vis_prop_pair);

		TCanvas c;

		model_hist->Draw("colz");
		model_hist->GetXaxis()->SetTitle("#theta / rad");
		model_hist->GetYaxis()->SetTitle("#phi / rad");
		std::stringstream titletext;
		titletext << "tilt_{x}=" << tilt_x << " rad, tilt_{y}=" << tilt_y<<" rad";
		model_hist->SetTitle(titletext.str().c_str());

		c.SetLogz(1);

		std::stringstream strstream;
		strstream.precision(3);

		strstream << "DPMModel2D_tiltxy-" << tilt_x << "-" << tilt_y << "_plab-"
				<< plab << ".pdf";
		c.SaveAs(strstream.str().c_str());
		strstream.str("");
		strstream << "DPMModel2D_tiltxy-" << tilt_x << "-" << tilt_y << "_plab-"
				<< plab << ".png";
		c.SaveAs(strstream.str().c_str());
	}
	return 0;
}
