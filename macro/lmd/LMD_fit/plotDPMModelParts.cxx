#include "PndLmdResultPlotter.h"

#include <stdlib.h>
#include <utility>
#include <sstream>
#include <cmath>

#include "TCanvas.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"

int main(int argc, char* argv[]) {
	if (argc == 2) {
		double plab = atof(argv[1]);

		const double theta_min = 0.0025;
		const double theta_max = 0.01;

		const double log_scale = true;

		PndLmdResultPlotter plotter;
		DataStructs::DimensionRange plot_range(theta_min, theta_max);

		// model part choices are: COUL, INT, HAD, HAD_RHO_B_SIGTOT, ALL_RHO_B_SIGTOT, ALL

		TGraphAsymmErrors* full_model_graph = plotter.generateDPMModelPartGraph(plab,
				LumiFit::ALL, plot_range);
		TGraphAsymmErrors* coul_model_graph = plotter.generateDPMModelPartGraph(plab,
				LumiFit::COUL, plot_range);
		TGraphAsymmErrors* had_model_graph = plotter.generateDPMModelPartGraph(plab,
				LumiFit::HAD, plot_range);
		TMultiGraph* model_graph = new TMultiGraph();
		model_graph->Add(full_model_graph);
		model_graph->Add(coul_model_graph);
		model_graph->Add(had_model_graph);
		

		TCanvas c("c", "", 1000, 700);
		c.SetLogy(log_scale);

		// determine y axis range (mainly required for interference part)
		model_graph->Draw("AC");
		gPad->Update();
		double top_pos = gPad->GetUymax();
		if (log_scale)
			top_pos = pow(10, gPad->GetUymax());
		gPad->Update();
		double bottom_pos = gPad->GetUymin();
		if (log_scale)
			bottom_pos = pow(10, gPad->GetUymin());
		model_graph->GetXaxis()->SetRangeUser(theta_min, theta_max);
		model_graph->GetYaxis()->SetRangeUser(bottom_pos, top_pos);

		full_model_graph->SetLineWidth(2);
		//full_model_graph->Draw("AC");
		coul_model_graph->SetLineWidth(2);
		coul_model_graph->SetLineColor(2);
		//coul_model_graph->Draw("CSAME");
		had_model_graph->SetLineWidth(2);
		had_model_graph->SetLineColor(9);
		//had_model_graph->Draw("CSAME");

		std::stringstream strstream;
		strstream.precision(3);

		strstream << "DPMModels_" << plab << ".pdf";
		c.SaveAs(strstream.str().c_str());
		c.Print(strstream.str().c_str());
	}
	return 0;
}
