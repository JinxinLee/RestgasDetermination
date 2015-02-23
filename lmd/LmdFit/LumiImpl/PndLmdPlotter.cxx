/*
 * PndLmdResultPlotter.cxx
 *
 *  Created on: Mar 25, 2013
 *      Author: steve
 */

#include "PndLmdPlotter.h"

#include "ROOTPlotHelper.hpp"

#include "fit/data/ROOT/ROOTDataHelper.h"
#include "visualization/ModelVisualizationProperties1D.h"
#include "model/PndLmdModelFactory.h"
#include "fit/PndLmdLumiFitResult.h"
#include "data/PndLmdAngularData.h"
#include "data/PndLmdVertexData.h"
#include "data/PndLmdAcceptance.h"
#include "fit/PndLmdFitFacade.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iosfwd>
#include <ostream>
#include <iomanip>

#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/regex.hpp"

#include "TMath.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TLatex.h"
#include "TString.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLine.h"
#include "TLegend.h"
#include "TEfficiency.h"
#include "TExec.h"
#include "TStyle.h"
#include "TGaxis.h"
#include "TColor.h"
#include "TPaletteAxis.h"
#include "TROOT.h"

using boost::filesystem::path;
using boost::filesystem::directory_iterator;
using std::stringstream;
using std::cout;
using std::endl;

namespace LumiFit {

PndLmdPlotter::PndLmdPlotter() {
}

PndLmdPlotter::~PndLmdPlotter() {
	// TODO Auto-generated destructor stub
}

// some general helper function

/*
 void PndLmdPlotter::makeComparisonCanvas(TString name,
 std::map<TString, std::vector<NeatPlotting::GraphPoint> >& result_map) {

 std::vector<TGraphErrors*> graphs;
 int colors[] = { 1, 2, 8, 9, 3, 4, 5, 6, 7, 10, 11, 12, 13, 14, 15, 16, 17 };
 TLegend* leg = new TLegend(0.50, 0.85 - 0.05 * result_map.size(), .9, .9);

 TMultiGraph *mg = new TMultiGraph();
 int counter = 0;

 for (std::map<TString, std::vector<NeatPlotting::GraphPoint> >::iterator it =
 result_map.begin(); it != result_map.end(); it++) {

 TGraphErrors* ge = neat_plot_helper.makeGraph(it->second);
 graphs.push_back(ge);
 mg->Add(ge, "*");

 leg->AddEntry(ge, it->first, "lp");
 counter++;
 }

 std::pair<double, double> plot_range = determineDiffPlotRange(result_map);

 if (graphs.size() > 0) {
 TCanvas *c = new TCanvas("lmd_comp_plot", "luminosity comparison", 1000,
 700);
 c->Divide(1, 2);
 c->cd(1);
 mg->Draw("a"); // draw first to be able to set axis stuff below without segfaulting
 mg->GetYaxis()->SetRangeUser(plot_range.first, plot_range.second);
 mg->GetXaxis()->SetTitle("# of bins");
 mg->GetYaxis()->SetTitle("int. luminosity");
 mg->GetYaxis()->SetTitleOffset(1.35);
 mg->Draw("a");

 c->cd(2);
 leg->Draw();

 /*if (nominal_lumi) {
 std::cout << "nominal luminosity: " << nominal_lumi << std::endl;
 TLine *line = new TLine(mg->GetXaxis()->GetXmin(), nominal_lumi,
 mg->GetXaxis()->GetXmax(), nominal_lumi);
 line->Draw();
 }

 c->SaveAs(name + ".pdf");

 delete (c);
 }
 }*/

std::pair<double, double> PndLmdPlotter::calulateLumiRelDiff(double lumi,
		double lumi_err, double lumi_ref) const {
	return std::make_pair(100.0 * (lumi - lumi_ref) / lumi_ref,
			100.0 * lumi_err / lumi_ref);
}

std::vector<DataStructs::DimensionRange> PndLmdPlotter::generatePlotRange(
		const PndLmdAbstractData &lmd_abs_data,
		const EstimatorOptions &est_options) const {
	PndLmdFitFacade fit_facade;

	std::vector<DataStructs::DimensionRange> ranges = fit_facade.calcRange(
			lmd_abs_data, est_options);

	if (primary_dimension_plot_range.is_active
			&& lmd_abs_data.getPrimaryDimension().is_active) {
		ranges[0].range_low = primary_dimension_plot_range.range_low;
		ranges[0].range_high = primary_dimension_plot_range.range_high;
	}
	return ranges;
}

std::pair<ModelVisualizationProperties1D, ModelVisualizationProperties1D> PndLmdPlotter::create2DVisualizationProperties(
		const EstimatorOptions &est_options,
		const PndLmdAbstractData &lmd_abs_data) const {
	// in case we want to make a difference graph in 2d the binnings have to match
	// hence if the fit range is shortened also the binning has to be reduced accordingly

	// ok just evaluate the function at 500 points in the range (is default);
	ModelVisualizationProperties1D vis_prop1;
	ModelVisualizationProperties1D vis_prop2;

	vis_prop1.setEvaluations(lmd_abs_data.getPrimaryDimension().bins);
	vis_prop1.setBinningFactor(lmd_abs_data.getPrimaryDimension().bin_size);
	DataStructs::DimensionRange dim_range(
			lmd_abs_data.getPrimaryDimension().dimension_range.getRangeLow(),
			lmd_abs_data.getPrimaryDimension().dimension_range.getRangeHigh());
	vis_prop1.setPlotRange(dim_range);

	vis_prop2.setEvaluations(lmd_abs_data.getSecondaryDimension().bins);
	vis_prop2.setBinningFactor(lmd_abs_data.getSecondaryDimension().bin_size);
	DataStructs::DimensionRange dim_range2(
			lmd_abs_data.getSecondaryDimension().dimension_range.getRangeLow(),
			lmd_abs_data.getSecondaryDimension().dimension_range.getRangeHigh());
	vis_prop2.setPlotRange(dim_range2);

	return std::make_pair(vis_prop1, vis_prop2);
}

ModelVisualizationProperties1D PndLmdPlotter::create1DVisualizationProperties(
		const EstimatorOptions &est_options,
		const PndLmdAbstractData &lmd_abs_data) const {
// ok just evaluate the function at 500 points in the range (is default);
	ModelVisualizationProperties1D vis_prop;

	vis_prop.setBinningFactor(lmd_abs_data.getBinningFactor(1));
	DataStructs::DimensionRange plot_range = generatePlotRange(lmd_abs_data,
			est_options)[0];
	vis_prop.setPlotRange(plot_range);

	return vis_prop;
}

// histogram and graph creation from lmd objects

TH2D* PndLmdPlotter::create2DHistogramFromFitResult(
		const PndLmdLumiFitOptions &fit_opt, const PndLmdAngularData &data) const {

	PndLmdModelFactory model_factory;
	shared_ptr<Model> model = model_factory.generateModel(
			fit_opt.getFitModelOptions(), data.getLabMomentum());
	if (model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

	// now just overwrite all parameters in the model from the fit result
	model->getModelParameterHandler().initModelParametersFromFitResult(
			data.getFitResult(fit_opt).getModelFitResult());

	const std::set<ModelStructs::minimization_parameter> &fit_params =
			data.getFitResult(fit_opt).getModelFitResult().getFitParameters();
	std::set<ModelStructs::minimization_parameter>::const_iterator fit_param_it;

	for (fit_param_it = fit_params.begin(); fit_param_it != fit_params.end();
			fit_param_it++) {
		std::cout << fit_param_it->name.first << " : " << fit_param_it->name.second
				<< " = " << fit_param_it->value << " +- " << fit_param_it->error
				<< std::endl;
	}

	std::pair<ModelVisualizationProperties1D, ModelVisualizationProperties1D> vis_props =
			create2DVisualizationProperties(fit_opt.getEstimatorOptions(), data);

	TH2D* hist = root_plotter.createHistogramFromModel2D(model, vis_props);

	return hist;
}

TGraphAsymmErrors* PndLmdPlotter::createGraphFromFitResult(
		const PndLmdLumiFitOptions &fit_opt, PndLmdAngularData &data) const {

	PndLmdModelFactory model_factory;
	shared_ptr<Model1D> model = model_factory.generate1DModel(
			fit_opt.getFitModelOptions(), data.getLabMomentum());
	if (model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

// now just overwrite all parameters in the model from the fit result
	model->getModelParameterHandler().initModelParametersFromFitResult(
			data.getFitResult(fit_opt).getModelFitResult());

	ModelVisualizationProperties1D vis_prop = create1DVisualizationProperties(
			fit_opt.getEstimatorOptions(), data);

	return root_plotter.createGraphFromModel1D(model, vis_prop);
}

TGraphAsymmErrors* PndLmdPlotter::createVertexGraphFromFitResult(
		const PndLmdLumiFitOptions &fit_opt, const PndLmdVertexData &data) const {

	PndLmdModelFactory model_factory;
	shared_ptr<Model1D> model = model_factory.generate1DVertexModel(
			fit_opt.getFitModelOptions());
	if (model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

// now just overwrite all parameters in the model from the fit result
	model->getModelParameterHandler().initModelParametersFromFitResult(
			data.getFitResult(fit_opt).getModelFitResult());

	ModelVisualizationProperties1D vis_prop = create1DVisualizationProperties(
			fit_opt.getEstimatorOptions(), data);

	return root_plotter.createGraphFromModel1D(model, vis_prop);
}

TH2D* PndLmdPlotter::create2DHistogramFromFitResult(
		const PndLmdLumiFitOptions &fit_opt,
		const PndLmdHistogramData &res_data) const {

	PndLmdModelFactory model_factory;
	shared_ptr<Model2D> model = model_factory.generate2DResolutionModel(
			fit_opt.getFitModelOptions());
	if (model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

	// now just overwrite all parameters in the model from the fit result
	model->getModelParameterHandler().initModelParametersFromFitResult(
			res_data.getFitResult(fit_opt).getModelFitResult());

	std::pair<ModelVisualizationProperties1D, ModelVisualizationProperties1D> vis_props =
			create2DVisualizationProperties(fit_opt.getEstimatorOptions(), res_data);

	TH2D* hist = root_plotter.createHistogramFromModel2D(model, vis_props);

	return hist;
}

TGraphAsymmErrors* PndLmdPlotter::createSmearingGraphFromFitResult(
		const PndLmdLumiFitOptions &fit_opt,
		const PndLmdHistogramData &res_data) const {
	PndLmdModelFactory model_factory;
	shared_ptr<Model1D> model = model_factory.generate1DResolutionModel(
			fit_opt.getFitModelOptions());
	if (model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

// now just overwrite all parameters in the model from the fit result
	model->getModelParameterHandler().initModelParametersFromFitResult(
			res_data.getFitResult(fit_opt).getModelFitResult());

// all parameters of the smearing model have to be freed otherwise the
// parametrization model will overwrite the values during the evaluation
	model->getModelParameterSet().freeAllModelParameters();

	ModelVisualizationProperties1D vis_prop = create1DVisualizationProperties(
			fit_opt.getEstimatorOptions(), res_data);

	return root_plotter.createGraphFromModel1D(model, vis_prop);
}

/*
 std::pair<TGraphAsymmErrors*, TGraphAsymmErrors*> PndLmdPlotter::createResidual(
 const PndLmdLumiFitOptions &fit_opt, PndLmdAngularData &data) {
 const TH1D *data_hist = data.get1DHistogram();

 PndLmdModelFactory model_factory;

 shared_ptr<Model1D> model = model_factory.generate1DModel(
 fit_opt.getFitModelOptions(), data.getLabMomentum());
 if (model->init()) {
 std::cout << "Error: not all parameters have been set!" << std::endl;
 }

 // now just overwrite all parameters in the model from the fit result
 model->getModelParameterHandler().initModelParametersFromFitResult(
 data.getFitResult(fit_opt)->getModelFitResult());

 int counter = 0;
 double xvals[data_hist->GetNbinsX()];
 double yvals[data_hist->GetNbinsX()];
 double yerrs[data_hist->GetNbinsX()];
 double model_yvals[data_hist->GetNbinsX()];
 double model_errs_low[data_hist->GetNbinsX()];
 double model_errs_high[data_hist->GetNbinsX()];

 for (unsigned int i = 0; i < data_hist->GetNbinsX(); i++) {
 double eval_point = data_hist->GetBinCenter(i);
 if (!fit_opt.getEstimatorOptions().getFitRangeX().isDataWithinRange(
 eval_point)) {
 continue;
 }
 xvals[counter] = eval_point;
 yvals[counter] = data_hist->GetBinContent(i)
 - model->evaluate(&eval_point) * data.getBinningFactor(1);
 model_yvals[counter] = 0.0;
 yerrs[counter] = data_hist->GetBinError(i);

 if (yvals[counter] > 0.0) {
 model_errs_low[counter] = model->getUncertaincy(&eval_point).second
 * data.getBinningFactor(1);
 model_errs_high[counter] = model->getUncertaincy(&eval_point).first
 * data.getBinningFactor(1);
 } else {
 model_errs_low[counter] = model->getUncertaincy(&eval_point).first
 * data.getBinningFactor(1);
 model_errs_high[counter] = model->getUncertaincy(&eval_point).second
 * data.getBinningFactor(1);
 }
 counter++;
 }
 return std::make_pair(
 new TGraphAsymmErrors(counter, xvals, yvals, 0, 0, yerrs, yerrs),
 new TGraphAsymmErrors(counter, xvals, model_yvals, 0, 0, model_errs_low,
 model_errs_high));
 }*/

TGraphAsymmErrors* PndLmdPlotter::createAcceptanceGraph(
		const PndLmdAcceptance *acc) const {
	TVirtualPad *current_pad = gPad;

	TEfficiency *eff = acc->getAcceptance1D(); // false = angular acceptance
	TCanvas c;
	eff->Draw();
	c.Update();

	gPad = current_pad; // reset the pad to previous
	return new TGraphAsymmErrors(*eff->GetPaintedGraph());
}

TGraphAsymmErrors* PndLmdPlotter::generateDPMModelPartGraph(double plab,
		LumiFit::DPMElasticParts dpm_elastic_part,
		DataStructs::DimensionRange& plot_range) {
	PndLmdModelFactory model_factory;

	ROOTDataHelper data_helper;
	ModelVisualizationProperties1D vis_prop;
	vis_prop.setPlotRange(plot_range);

	LumiFit::PndLmdFitModelOptions fit_op_full;
	fit_op_full.dpm_elastic_parts = dpm_elastic_part;

	shared_ptr<Model1D> model = model_factory.generate1DModel(fit_op_full, plab);
	if (model->init()) {
		std::cout << "Error: not all parameters have been set!" << std::endl;
	}

	TGraphAsymmErrors* model_graph = root_plotter.createGraphFromModel1D(model,
			vis_prop);

	return model_graph;
}

// special plots which are a combination of several data objects
TGraphAsymmErrors* PndLmdPlotter::makeXYOverviewGraph(
		const std::vector<PndLmdVertexData> &data_vec,
		double error_scaling_factor) const {
	std::vector<NeatPlotting::GraphPoint> graph_data;

	NeatPlotting::GraphPoint current_graph_point;

	// first group all vertex data plots of some specific ip property setting together
	std::map<LumiFit::LmdSimIPParameters, std::vector<PndLmdVertexData> > sim_settings_ip_data_map;
	for (unsigned int i = 0; i < data_vec.size(); i++) {
		sim_settings_ip_data_map[data_vec[i].getSimulationIPParameters()].push_back(
				data_vec[i]);
	}

	std::map<LumiFit::LmdSimIPParameters, std::vector<PndLmdVertexData> >::iterator ip_setting_case;
	for (ip_setting_case = sim_settings_ip_data_map.begin();
			ip_setting_case != sim_settings_ip_data_map.end(); ip_setting_case++) {
		current_graph_point.x = ip_setting_case->first.offset_x_mean * 10; // *10 from cm to mm
		current_graph_point.y = ip_setting_case->first.offset_y_mean * 10; // *10 from cm to mm
		current_graph_point.x_err_low = 0.0;
		current_graph_point.x_err_high = 0.0;
		current_graph_point.y_err_low = 0.0;
		current_graph_point.y_err_high = 0.0;

		std::cout << "vertex data count for " << ip_setting_case->first.getLabel()
				<< ": " << ip_setting_case->second.size() << std::endl;

		for (unsigned int i = 0; i < ip_setting_case->second.size(); i++) {
			if (ip_setting_case->second[i].getPrimaryDimension().dimension_options.dimension_type
					== LumiFit::X) {
				PndLmdLumiFitResult fit_result =
						ip_setting_case->second[i].getFitResults().begin()->second;

				if (fit_result.getModelFitResult().getFitParameters().size() > 0) {
					double diff =
							error_scaling_factor
									* (fit_result.getModelFitResult().getFitParameter(
											"gauss_mean").value * 10 - current_graph_point.x);
					double absdiff = fabs(diff);

					std::cout << diff << " " << absdiff << std::endl;
					//	if (absdiff > 0.1 * max_offset && absdiff < max_offset) {
					if (diff < 0.0)
						current_graph_point.x_err_low = absdiff;
					else
						current_graph_point.x_err_high = absdiff;
					//	}
				}
			} else if (ip_setting_case->second[i].getPrimaryDimension().dimension_options.dimension_type
					== LumiFit::Y) {
				PndLmdLumiFitResult fit_result =
						ip_setting_case->second[i].getFitResults().begin()->second;

				if (fit_result.getModelFitResult().getFitParameters().size() > 0) {
					double diff =
							error_scaling_factor
									* (fit_result.getModelFitResult().getFitParameter(
											"gauss_mean").value * 10 - current_graph_point.y);
					double absdiff = fabs(diff);

					std::cout << diff << " " << absdiff << std::endl;
					//	if (absdiff > 0.1 * max_offset && absdiff < max_offset) {
					if (diff < 0.0)
						current_graph_point.y_err_low = absdiff;
					else
						current_graph_point.y_err_high = absdiff;
					//	}
				}
			}
		}

		// add values to vector
		graph_data.push_back(current_graph_point);
	}

	NeatPlotting::GraphAndHistogramHelper graph_helper;
	return graph_helper.makeGraph(graph_data);
}

// functions creating NeatPlotting::PlotBundles from lmd data objects

void PndLmdPlotter::applyPlotRanges(
		NeatPlotting::PlotBundle &plot_bundle) const {
	if (primary_dimension_plot_range.is_active) {
		if (primary_dimension_plot_range.range_low
				< primary_dimension_plot_range.range_high) {
			plot_bundle.plot_axis.x_axis_range.low =
					primary_dimension_plot_range.range_low;
			plot_bundle.plot_axis.x_axis_range.high =
					primary_dimension_plot_range.range_high;
			plot_bundle.plot_axis.x_axis_range.active = true;
		}
	}
	if (secondary_dimension_plot_range.is_active) {
		if (secondary_dimension_plot_range.range_low
				< secondary_dimension_plot_range.range_high) {
			plot_bundle.plot_axis.y_axis_range.low =
					secondary_dimension_plot_range.range_low;
			plot_bundle.plot_axis.y_axis_range.high =
					secondary_dimension_plot_range.range_high;
			plot_bundle.plot_axis.y_axis_range.active = true;
		}
	}
}

NeatPlotting::PlotBundle PndLmdPlotter::makeAcceptanceBundle1D(
		PndLmdAngularData& data, const PndLmdLumiFitOptions &fit_options) const {
	NeatPlotting::PlotBundle acc_bundle;

	if (fit_options.getFitModelOptions().acceptance_correction_active) {
		const PndLmdAcceptance *acc = fit_options.getFitModelOptions().acceptance;
		if (acc) {
			std::cout << "found a lmd acceptance object!" << std::endl;
			NeatPlotting::DataObjectStyle style;
			style.draw_option = "PE";

			style.line_style.line_color = kRed;
			style.marker_style.marker_color = kRed;
			style.marker_style.marker_style = 20;

			acc_bundle.addGraph(createAcceptanceGraph(acc), style);

			acc_bundle.plot_axis.x_axis_title =
					data.getPrimaryDimension().createAxisLabel();
			acc_bundle.plot_axis.y_axis_title = "efficiency";

			// add full efficiency line
			acc_bundle.plot_decoration.x_parallel_lines.push_back(1.0);
		}
	}

	applyPlotRanges(acc_bundle);
	return acc_bundle;
}

NeatPlotting::PlotBundle PndLmdPlotter::makeAcceptanceBundle2D(
		PndLmdAngularData& data, const PndLmdLumiFitOptions &fit_options) const {
	NeatPlotting::PlotBundle acc_bundle;

	if (fit_options.getFitModelOptions().acceptance_correction_active) {
		const PndLmdAcceptance *acc = fit_options.getFitModelOptions().acceptance;
		if (acc) {
			if (acc->getSecondaryDimension().is_active) {
				TVirtualPad *current_pad = gPad;

				TEfficiency *eff = acc->getAcceptance2D();
				TCanvas c;
				eff->Draw("COLZ");
				c.Update();

				NeatPlotting::DataObjectStyle style;
				style.draw_option = "COL";

				acc_bundle.addHistogram(new TH2D(*(TH2D*) eff->GetPaintedHistogram()),
						style);

				acc_bundle.plot_axis.x_axis_title =
						data.getPrimaryDimension().createAxisLabel();
				acc_bundle.plot_axis.y_axis_title =
						data.getSecondaryDimension().createAxisLabel();
				acc_bundle.plot_axis.z_axis_title = "efficiency";

				gPad = current_pad; // reset the pad to the previous
			}
		}
	}
	applyPlotRanges(acc_bundle);
	return acc_bundle;
}

NeatPlotting::PlotBundle PndLmdPlotter::makeGraphBundle1D(
		PndLmdAngularData& data, const PndLmdLumiFitOptions &fit_options,
		bool draw_model) {
	std::cout << "Creating graph bundle!" << std::endl;

	NeatPlotting::PlotBundle ang_plot_bundle;

	NeatPlotting::DataObjectStyle style;
	style.draw_option = "E1";

	TH1D* hist = data.get1DHistogram();
	ang_plot_bundle.addHistogram(hist, style);

	std::stringstream ss;
	if (data.getPrimaryDimension().dimension_options.dimension_type
			== LumiFit::T) {
		if (primary_dimension_plot_range.range_low
				< primary_dimension_plot_range.range_high
				&& primary_dimension_plot_range.is_active) {
			DataStructs::DimensionRange temp_primary_dimension_plot_range =
					primary_dimension_plot_range;

			primary_dimension_plot_range.range_low =
					-lumi_helper.getMomentumTransferFromTheta(data.getLabMomentum(),
							primary_dimension_plot_range.range_low);
			primary_dimension_plot_range.range_high =
					-lumi_helper.getMomentumTransferFromTheta(data.getLabMomentum(),
							primary_dimension_plot_range.range_high);
			applyPlotRanges(ang_plot_bundle);
			primary_dimension_plot_range = temp_primary_dimension_plot_range;
		}
		ss << "# of tracks / " << hist->GetXaxis()->GetBinWidth(1)
				<< " GeV^{2}/c^{2}";
	} else {
		applyPlotRanges(ang_plot_bundle);
		ang_plot_bundle.plot_axis.x_axis_title =
				data.getPrimaryDimension().createAxisLabel();
		ss << "# of tracks / " << 1000.0 * hist->GetXaxis()->GetBinWidth(1)
				<< " mrad";
	}
	ang_plot_bundle.plot_axis.y_axis_title = ss.str();

	if (draw_model) {
		PndLmdLumiFitResult fit_res = data.getFitResult(fit_options);

		if (fit_options.getFitModelOptions().fit_dimension == 1) {
			if (0 == fit_res.getModelFitResult().getFitStatus()) {
				style.draw_option = "C";
				style.line_style.line_color = kRed;
				style.marker_style.marker_color = kRed;
				ang_plot_bundle.addGraph(createGraphFromFitResult(fit_options, data),
						style);
			}
		}
	}
	stringstream strstream;
	strstream.precision(3);

	strstream << "p_{lab} = " << data.getLabMomentum() << " GeV";

	NeatPlotting::TextStyle text_style;
	NeatPlotting::PlotLabel plot_label(strstream.str(), text_style);
	ang_plot_bundle.plot_decoration.label_text_leftpos = 0.7;
	ang_plot_bundle.plot_decoration.label_text_toppos = 0.95;
	ang_plot_bundle.plot_decoration.label_text_spacing = 0.06;
	ang_plot_bundle.plot_decoration.use_line_layout = true;
	ang_plot_bundle.plot_decoration.labels.push_back(plot_label);
	strstream.str("");

	if (draw_model) {
		double lumi_ref = data.getReferenceLuminosity();

		PndLmdLumiFitResult fit_res = data.getFitResult(fit_options);

		if (fit_options.getFitModelOptions().fit_dimension == 1) {
			if (fit_res.getModelFitResult().getFitStatus() == 0) {
				strstream << std::setprecision(3) << "#Delta L/L = "
						<< calulateLumiRelDiff(fit_res.getLuminosity(),
								fit_res.getLuminosityError(), lumi_ref).first << " #pm "
						<< calulateLumiRelDiff(fit_res.getLuminosity(),
								fit_res.getLuminosityError(), lumi_ref).second << " %";
				plot_label.setTitle(strstream.str());
				ang_plot_bundle.plot_decoration.labels.push_back(plot_label);
				strstream.str("");
			}
		}
	}

	return ang_plot_bundle;
}

NeatPlotting::PlotBundle PndLmdPlotter::makeResidualPlotBundle1D(
		PndLmdAngularData& data, const PndLmdLumiFitOptions &fit_options) {
	NeatPlotting::PlotBundle residual_plot_bundle;

	if (fit_options.getFitModelOptions().fit_dimension != 1) {
		return residual_plot_bundle;
	}

	NeatPlotting::DataObjectStyle data_style;
	data_style.draw_option = "PE";
	data_style.marker_style.marker_style = 20;

	TH1D* hist = data.get1DHistogram();
	TGraphAsymmErrors *graph(0);

	PndLmdLumiFitResult fit_res = data.getFitResult(fit_options);
	if (0 == fit_res.getModelFitResult().getFitStatus()) {
		graph = createGraphFromFitResult(fit_options, data);
	}

	std::stringstream ss;
	if (fit_options.getFitModelOptions().momentum_transfer_active) {
		if (primary_dimension_plot_range.range_low
				< primary_dimension_plot_range.range_high
				&& primary_dimension_plot_range.is_active) {
			DataStructs::DimensionRange temp_primary_dimension_plot_range =
					primary_dimension_plot_range;

			primary_dimension_plot_range.range_low =
					-lumi_helper.getMomentumTransferFromTheta(data.getLabMomentum(),
							primary_dimension_plot_range.range_low);
			primary_dimension_plot_range.range_high =
					-lumi_helper.getMomentumTransferFromTheta(data.getLabMomentum(),
							primary_dimension_plot_range.range_high);
			applyPlotRanges(residual_plot_bundle);
			primary_dimension_plot_range = temp_primary_dimension_plot_range;
		}
		ss << "data - model";
	} else {
		applyPlotRanges(residual_plot_bundle);
		residual_plot_bundle.plot_axis.x_axis_title =
				data.getPrimaryDimension().createAxisLabel();
		ss << "data - model";
	}
	residual_plot_bundle.plot_axis.y_axis_title = ss.str();

	if (hist && graph) {
		TGraphAsymmErrors *residual = neat_plot_helper.makeDifferenceGraph(hist,
				graph);
		residual_plot_bundle.addGraph(residual, data_style);

		data_style.draw_option = "C";
		data_style.line_style.line_color = kRed;
		data_style.marker_style.marker_color = kRed;

		TGraphAsymmErrors *zero_line_graph = neat_plot_helper.makeResidualZeroGraph(
				graph);
		residual_plot_bundle.addGraph(residual, data_style);
	}

	return residual_plot_bundle;
}

NeatPlotting::PlotBundle PndLmdPlotter::makeResolutionGraphBundle(
		const PndLmdHistogramData & res) const {
	cout << "Creating resolution graph bundle!" << endl;

	const map<PndLmdLumiFitOptions, PndLmdLumiFitResult>& fit_results =
			res.getFitResults();

	bool is_2d = res.getSecondaryDimension().is_active;
	bool is_fit_2d(false);
// check if we have some fit results and if they are 1d or 2d
	if (fit_results.size() > 0) {
		if (fit_results.begin()->second.getModelFitResult().getFitStatus() == 0) {
			if (fit_results.begin()->first.getFitModelOptions().fit_dimension == 2) {
				is_fit_2d = true;
				if (!is_2d)
					cout
							<< "thats weird you have successfully fitted a 2d model on 1d data..."
							<< endl;
			} else {
				is_fit_2d = false;
				is_2d = false;
			}
		}
	}

	NeatPlotting::PlotBundle res_plot_bundle;

	NeatPlotting::DataObjectStyle style;

	res_plot_bundle.plot_axis.x_axis_title =
			res.getPrimaryDimension().createAxisLabel();

	TH2D* hist2d;
	if (is_2d) {
		hist2d = res.get2DHistogram();
		style.draw_option = "COL";
		if (!is_fit_2d) // make diff hist later
			res_plot_bundle.addHistogram(hist2d, style);
	} else {
		TH1D* hist1d = res.get1DHistogram();
		style.draw_option = "E1";
		res_plot_bundle.addHistogram(hist1d, style);
		std::stringstream yaxis_label;
		yaxis_label.precision(2);
		yaxis_label << "# rec tracks / " << std::fixed
				<< 1000.0 * hist1d->GetXaxis()->GetBinWidth(1) << " mrad";
		res_plot_bundle.plot_axis.y_axis_title = yaxis_label.str();
	}

	if (fit_results.size() > 0) {
		if (fit_results.begin()->second.getModelFitResult().getFitStatus() == 0) {
			if (is_fit_2d) {
				TH2D* model_hist = create2DHistogramFromFitResult(
						fit_results.begin()->first, res);
				TH2D* diff = neat_plot_helper.makeDifferenceHistogram(hist2d,
						model_hist);
				res_plot_bundle.addHistogram(diff, style);
			} else {
				style.draw_option = "C";
				style.line_style.line_color = kRed;
				style.marker_style.marker_color = kRed;
				style.marker_style.marker_style = 20;
				res_plot_bundle.addGraph(
						createSmearingGraphFromFitResult(fit_results.begin()->first, res),
						style);
			}
		}
	}

	stringstream strstream;
	strstream.precision(3);

	strstream << "p_{lab} = " << res.getLabMomentum() << " GeV";
	NeatPlotting::TextStyle text_style;
	NeatPlotting::PlotLabel plot_label(strstream.str(), text_style);
	res_plot_bundle.plot_decoration.label_text_leftpos = 0.7;
	res_plot_bundle.plot_decoration.label_text_toppos = 0.95;
	res_plot_bundle.plot_decoration.label_text_spacing = 0.06;
	res_plot_bundle.plot_decoration.use_line_layout = true;
	res_plot_bundle.plot_decoration.labels.push_back(plot_label);
	strstream.str("");

	if (res.getSelectorSet().size() > 0) {
		bool found_phi_selection(false);
		const std::set<LumiFit::LmdDimension> &selection_set = res.getSelectorSet();
		std::set<LumiFit::LmdDimension>::const_iterator selection_set_it;
		for (selection_set_it = selection_set.begin();
				selection_set_it != selection_set.end(); selection_set_it++) {
			if (selection_set_it->dimension_options.dimension_type
					== LumiFit::THETA) {
				found_phi_selection = true;
				break;
			}
		}
		if (found_phi_selection) {
			strstream << "#theta_{MC} = "
					<< 1000 * selection_set_it->dimension_range.getRangeLow() << "-"
					<< 1000 * selection_set_it->dimension_range.getRangeHigh() << " mrad";
			plot_label.setTitle(strstream.str());
			res_plot_bundle.plot_decoration.labels.push_back(plot_label);
		}
	}

	applyPlotRanges(res_plot_bundle);

	return res_plot_bundle;
}

NeatPlotting::PlotBundle PndLmdPlotter::makeVertexGraphBundle1D(
		const PndLmdVertexData & data) const {
	std::cout << "Creating vertex graph bundle!" << std::endl;

	const map<PndLmdLumiFitOptions, PndLmdLumiFitResult>& fit_results =
			data.getFitResults();

	NeatPlotting::PlotBundle ver_plot_bundle;

	NeatPlotting::DataObjectStyle style;
	style.draw_option = "E1";

	TH1D* hist = data.get1DHistogram();
	ver_plot_bundle.addHistogram(hist, style);

	ver_plot_bundle.plot_axis.x_axis_title =
			data.getPrimaryDimension().createAxisLabel();

	std::stringstream ss;
	ss << "# of tracks / " << hist->GetXaxis()->GetBinWidth(1) << " cm";
	ver_plot_bundle.plot_axis.y_axis_title = ss.str();

	std::cout << "num fit results: " << fit_results.size() << std::endl;
	if (fit_results.size() > 0) {
		if (fit_results.begin()->second.getModelFitResult().getFitStatus() == 0) {
			style.draw_option = "PE";
			style.line_style.line_color = kRed;
			style.marker_style.marker_color = kRed;
			style.marker_style.marker_style = 20;
			ver_plot_bundle.addGraph(
					createVertexGraphFromFitResult(fit_results.begin()->first, data),
					style);
		}
	}

	applyPlotRanges(ver_plot_bundle);
	return ver_plot_bundle;
}

/*std::map<PndLmdLumiFitOptions, NeatPlotting::PlotBundle> PndLmdPlotter::makeGraphBundles1D(
 std::vector<PndLmdAngularData> &data_vec,
 LumiFit::PndLmdFitModelOptions & fitop) {

 std::map<PndLmdLumiFitOptions, NeatPlotting::PlotBundle> return_map;

 for (std::vector<PndLmdAngularData>::iterator data = data_vec.begin();
 data != data_vec.end(); data++) {
 // we only want IP info, so throw out everything else
 if (data->getPrimaryDimension().dimension_options.track_param_type
 != LumiFit::IP) {
 continue;
 }

 // loop over fit results
 map<PndLmdLumiFitOptions, PndLmdLumiFitResult*> fit_results =
 data->getFitResults();
 for (map<PndLmdLumiFitOptions, PndLmdLumiFitResult*>::iterator it =
 fit_results.begin(); it != fit_results.end(); it++) {

 if (it->first.getFitModelOptions().equalBinaryOptions(fitop)) {
 return_map[it->first] = makeGraphBundle1D(*data, it->first);
 }
 }
 }
 return return_map;
 }*/

NeatPlotting::PlotBundle PndLmdPlotter::makeIPXYOverviewGraphBundle(
		const std::vector<PndLmdVertexData> &vertex_data) const {

	NeatPlotting::PlotBundle plot_bundle;

// this should be made a bit better...
	unsigned int num_values = 1;
	double scales[] = { 20.0 };
	int colors[] = { kRed };

	NeatPlotting::DataObjectStyle style;
	style.draw_option = "PE";

	for (unsigned int i = 0; i < num_values; i++) {
		TGraphAsymmErrors *gae = makeXYOverviewGraph(vertex_data, scales[i]);

		style.line_style.line_color = colors[i];
		style.marker_style.marker_color = colors[i];
		plot_bundle.addGraph(gae, style);

		// draw precision labels on points
		for (unsigned int point = 0; point < gae->GetN(); point++) {
			double x, y;
			gae->GetPoint(point, x, y);

			bool negative_x(false), negative_y(false);

			double errorx = gae->GetErrorXhigh(point);
			if (errorx < gae->GetErrorXlow(point)) {
				errorx = gae->GetErrorXlow(point);
				negative_x = true;
			}
			double errory = gae->GetErrorYhigh(point);
			if (errory < gae->GetErrorYlow(point)) {
				errory = gae->GetErrorYlow(point);
				negative_y = true;
			}

			std::stringstream ss;
			NeatPlotting::TextStyle text_style;
			text_style.text_size = 0.02;
			ss << std::fixed << std::setprecision(0) << 1000.0 * errorx / scales[i];
			NeatPlotting::PlotLabel xlabel(ss.str(), text_style);
			xlabel.setAbsolutionPosition(x - errorx - 0.1, y + 0.2);
			plot_bundle.plot_decoration.labels.push_back(xlabel);

			ss.str("");
			ss << std::fixed << std::setprecision(0) << 1000.0 * errory / scales[i];
			NeatPlotting::PlotLabel ylabel(ss.str(), text_style);
			double y_offset = 0.2 + errory;
			if (negative_y)
				y_offset = -0.4 - errory;
			ylabel.setAbsolutionPosition(x - 0.1, y + y_offset);
			plot_bundle.plot_decoration.labels.push_back(ylabel);

			/*std::stringstream ss;
			 NeatPlotting::TextStyle text_style;
			 text_style.text_size = 0.02;
			 ss << std::fixed << std::setprecision(0) << "("
			 << 10000.0 * errorx / scales[i] << "," << 10000.0 * errory / scales[i]
			 << ")#mum";
			 NeatPlotting::PlotLabel xlabel(ss.str(), text_style);
			 xlabel.setAbsolutionPosition(x - 0.08, y + 0.02);
			 plot_bundle.plot_decoration.labels.push_back(xlabel);*/

		}
		std::stringstream ss;
		ss << "diff scaled #times" << scales[i];
		NeatPlotting::TextStyle text_style;
		text_style.text_size = 0.04;
		NeatPlotting::PlotLabel label(ss.str(), text_style);
		label.setRelativePosition(0.60, 0.77);
		plot_bundle.plot_decoration.labels.push_back(label);
		label.setTitle("units #mum");
		label.setRelativePosition(0.60, 0.70);
		plot_bundle.plot_decoration.labels.push_back(label);

	}

	plot_bundle.plot_axis.x_axis_title = "x_{IP} / mm";
	plot_bundle.plot_axis.y_axis_title = "y_{IP} / mm";

	return plot_bundle;
}

NeatPlotting::PlotBundle PndLmdPlotter::makeXYOverviewHistogram(
		const std::map<LumiFit::LmdSimIPParameters, PndLmdAngularData> &sim_settings_ip_data_map) const {

	NeatPlotting::PlotBundle plot_bundle;

	NeatPlotting::DataObjectStyle style;
	style.draw_option = "COL";

	NeatPlotting::TextStyle text_style;
	text_style.text_size = 0.03;

	// determine boundaries
	double x_max = 0.0, y_max = 0.0;

	std::map<LumiFit::LmdSimIPParameters, PndLmdAngularData>::const_iterator ip_setting_case;
	for (ip_setting_case = sim_settings_ip_data_map.begin();
			ip_setting_case != sim_settings_ip_data_map.end(); ip_setting_case++) {

		if (fabs(ip_setting_case->first.offset_x_mean) > x_max)
			x_max = 10.0 * fabs(ip_setting_case->first.offset_x_mean); // *10 for cm to mm

		if (fabs(ip_setting_case->first.offset_y_mean) > y_max)
			y_max = 10.0 * fabs(ip_setting_case->first.offset_y_mean);
	}

	TH2D *hist = new TH2D("ip_shift_xy_lumi_results", "", 51, -1.1 * x_max,
			1.1 * x_max, 51, -1.1 * y_max, 1.1 * y_max);

	std::pair<double, double> lumi;

	for (ip_setting_case = sim_settings_ip_data_map.begin();
			ip_setting_case != sim_settings_ip_data_map.end(); ip_setting_case++) {
		if (ip_setting_case->second.getFitResults().size() > 0) {
			PndLmdLumiFitResult fit_result =
					ip_setting_case->second.getFitResults().begin()->second;

			lumi = calulateLumiRelDiff(fit_result.getLuminosity(),
					fit_result.getLuminosityError(),
					ip_setting_case->second.getReferenceLuminosity());

			hist->Fill(10.0 * ip_setting_case->first.offset_x_mean,
					10.0 * ip_setting_case->first.offset_y_mean, fabs(lumi.first));

			std::stringstream ss;
			ss << std::fixed << std::setprecision(2) << lumi.first << " %"; // << "#pm" << lumi.second << " %";
			NeatPlotting::PlotLabel label(ss.str(), text_style);
			label.setAbsolutionPosition(
					10.0 * (ip_setting_case->first.offset_x_mean - 0.035),
					10.0 * (ip_setting_case->first.offset_y_mean + 0.02));
			plot_bundle.plot_decoration.labels.push_back(label);

		}
	}

	//style.line_style.line_color = kRed;
	//style.marker_style.marker_color = kRed;
	//style.marker_style.marker_style = 20;

	plot_bundle.addHistogram(hist, style);

	std::stringstream ss;
	ss << "stat. err.: " << std::fixed << std::setprecision(2) << lumi.second
			<< " %";
	NeatPlotting::PlotLabel label(ss.str(), text_style);
	label.setRelativePosition(0.6, 0.8);
	plot_bundle.plot_decoration.labels.push_back(label);

	plot_bundle.plot_axis.x_axis_title = "x_{IP} / mm";
	plot_bundle.plot_axis.y_axis_title = "y_{IP} / mm";
	plot_bundle.plot_axis.z_axis_title = "#frac{|L-L_{ref}|}{L_{ref}} / %";

	return plot_bundle;
}

// booky creation

NeatPlotting::Booky PndLmdPlotter::makeLumiFitResultOverviewBooky(
		std::vector<PndLmdAngularData> &data_vec) {

	std::map<PndLmdLumiFitOptions,
			std::map<NeatPlotting::SubpadCoordinates, NeatPlotting::PlotBundle>,
			LumiFit::Comparisons::fit_options_compare> ordered_plots;

	NeatPlotting::PlotStyle log_y_plot_style;
	log_y_plot_style.y_axis_style.log_scale = true;
	NeatPlotting::PlotStyle normal_plot_style;

// model types
	LumiFit::PndLmdFitModelOptions fitop_tmctruth(LumiFit::MC, LumiFit::T, false);
	LumiFit::PndLmdFitModelOptions fitop_thmctruth(LumiFit::MC, LumiFit::THETA,
			false);
	LumiFit::PndLmdFitModelOptions fitop_mcacc(LumiFit::MC_ACC, LumiFit::THETA,
			false);
	LumiFit::PndLmdFitModelOptions fitop_normal(LumiFit::RECO, LumiFit::THETA,
			false);

// go through data map
	for (std::vector<PndLmdAngularData>::iterator top_it = data_vec.begin();
			top_it != data_vec.end(); top_it++) {

		// we only want IP info, so throw out everything else
		if (top_it->getPrimaryDimension().dimension_options.track_param_type
				!= LumiFit::IP) {
			continue;
		}
		// ok we are actually only interested in the dimension type and track type

		// loop over fit results
		map<PndLmdLumiFitOptions, PndLmdLumiFitResult> fit_results =
				top_it->getFitResults();
		for (map<PndLmdLumiFitOptions, PndLmdLumiFitResult>::iterator it =
				fit_results.begin(); it != fit_results.end(); it++) {

			// PAD1: if we have MC data and momentum transfer and we fit that with mc_t model
			// skip this pad in the beginning to find the matching panels later due to different fit ranges

			// mc stuff:
			if (top_it->getPrimaryDimension().dimension_options.track_type
					== LumiFit::MC
					&& top_it->getPrimaryDimension().dimension_options.dimension_type
							== LumiFit::THETA) {
				NeatPlotting::SubpadCoordinates pad_coord(2, 1);

				if (fitop_thmctruth.equalBinaryOptions(
						it->first.getFitModelOptions())) {
					ordered_plots[it->first][pad_coord] = makeGraphBundle1D(*top_it,
							it->first);
				}
			}

			// mc acc stuff:
			if (top_it->getPrimaryDimension().dimension_options.track_type
					== LumiFit::MC_ACC
					&& top_it->getPrimaryDimension().dimension_options.dimension_type
							== LumiFit::THETA) {
				NeatPlotting::SubpadCoordinates pad_coord(3, 1);

				if (fitop_mcacc.equalBinaryOptions(it->first.getFitModelOptions())) {
					ordered_plots[it->first][pad_coord] = makeAcceptanceBundle1D(*top_it,
							it->first);

					pad_coord = std::make_pair(1, 2);

					ordered_plots[it->first][pad_coord] = makeGraphBundle1D(*top_it,
							it->first);

					// create residual beneath this plot
					pad_coord.second = 3;

					ordered_plots[it->first][pad_coord] = makeResidualPlotBundle1D(
							*top_it, it->first);
				}

			}
			// reco stuff:
			if (top_it->getPrimaryDimension().dimension_options.track_type
					== LumiFit::RECO
					&& top_it->getPrimaryDimension().dimension_options.dimension_type
							== LumiFit::THETA) {
				bool with_secondaries(true);

				const std::set<LumiFit::LmdDimension> &selection_set =
						top_it->getSelectorSet();

				std::set<LumiFit::LmdDimension>::const_iterator selection_set_it;
				for (selection_set_it = selection_set.begin();
						selection_set_it != selection_set.end(); selection_set_it++) {
					if (selection_set_it->dimension_options.dimension_type
							== LumiFit::SECONDARY) {
						if (selection_set_it->dimension_range.getDimensionMean() < 0) {
							with_secondaries = false;
							break;
						}
					}
				}

				if (with_secondaries) {
					NeatPlotting::SubpadCoordinates pad_coord(3, 2);

					if (fitop_normal.equalBinaryOptions(it->first.getFitModelOptions())) {
						ordered_plots[it->first][pad_coord] = makeGraphBundle1D(*top_it,
								it->first);

						// create residual beneath this plot
						pad_coord.second = 3;

						ordered_plots[it->first][pad_coord] = makeResidualPlotBundle1D(
								*top_it, it->first);
					}
				} else {
					NeatPlotting::SubpadCoordinates pad_coord(2, 2);

					if (fitop_normal.equalBinaryOptions(it->first.getFitModelOptions())) {
						ordered_plots[it->first][pad_coord] = makeGraphBundle1D(*top_it,
								it->first);

						// create residual beneath this plot
						pad_coord.second = 3;

						ordered_plots[it->first][pad_coord] = makeResidualPlotBundle1D(
								*top_it, it->first);
					}
				}
			}
		}
	}

	/*
	 // go through data map again and try to match the momentum transfer data fits
	 for (std::vector<PndLmdAngularData>::iterator top_it = data_vec.begin();
	 top_it != data_vec.end(); top_it++) {

	 // first determine which data objects we have here
	 if (top_it->getPrimaryDimension().dimension_options.track_param_type
	 != LumiFit::IP) {
	 break;
	 }
	 // ok we are actually only interested in the dimension type and track type

	 // PAD1: if we have MC data and momentum transfer and we fit that with mc_t model
	 if (top_it->getPrimaryDimension().dimension_options.track_type
	 == LumiFit::MC
	 && top_it->getPrimaryDimension().dimension_options.dimension_type
	 == LumiFit::T) {
	 int pad_number = 1;
	 map<PndLmdLumiFitOptions, PndLmdLumiFitResult*> fit_results =
	 top_it->getFitResults();
	 for (map<PndLmdLumiFitOptions, PndLmdLumiFitResult*>::iterator it =
	 fit_results.begin(); it != fit_results.end(); it++) {
	 if (fitop_tmctruth.equalBinaryOptions(it->first.getFitModelOptions())) {

	 // loop over current return map and find appropriate canvas
	 for (std::map<PndLmdLumiFitOptions,
	 std::map<int, NeatPlotting::PlotBundle>,
	 LumiFit::Comparisons::fit_options_compare>::iterator return_map_it =
	 ordered_plots.begin(); return_map_it != ordered_plots.end();
	 return_map_it++) {
	 DataStructs::DimensionRange lmd_range =
	 return_map_it->first.getEstimatorOptions().getFitRangeX();

	 if (it->first.getEstimatorOptions().getFitRangeX().range_low
	 == lumi_helper.getMomentumTransferFromTheta(
	 top_it->getLabMomentum(), lmd_range.range_low)) {
	 if (it->first.getEstimatorOptions().getFitRangeX().range_high
	 == lumi_helper.getMomentumTransferFromTheta(
	 top_it->getLabMomentum(), lmd_range.range_high)) {
	 return_map_it->second[pad_number] = makeGraphBundle1D(*top_it,
	 it->first);
	 break;
	 }
	 }
	 }
	 }
	 }
	 }
	 }*/

	NeatPlotting::Booky booky;

	for (std::map<PndLmdLumiFitOptions,
			std::map<NeatPlotting::SubpadCoordinates, NeatPlotting::PlotBundle>,
			LumiFit::Comparisons::fit_options_compare>::iterator data_for_booky_page_it =
			ordered_plots.begin(); data_for_booky_page_it != ordered_plots.end();
			data_for_booky_page_it++) {

		std::map<NeatPlotting::SubpadCoordinates, NeatPlotting::PlotBundle>::iterator single_pad_plot_bundle_it;
		for (single_pad_plot_bundle_it = data_for_booky_page_it->second.begin();
				single_pad_plot_bundle_it != data_for_booky_page_it->second.end();
				single_pad_plot_bundle_it++) {
			booky.addPlotToCurrentBookyPage(single_pad_plot_bundle_it->second,
					normal_plot_style, single_pad_plot_bundle_it->first);
		}
		booky.addCurrentPageToBooky();
	}

	return booky;
}

NeatPlotting::PlotBundle PndLmdPlotter::createLumiFit2DPlotBundle(
		TH2D* hist) const {
	NeatPlotting::PlotBundle ang_plot_bundle;

	NeatPlotting::DataObjectStyle style;
	style.draw_option = "COL";

	ang_plot_bundle.addHistogram(hist, style);

	std::stringstream ss;

	applyPlotRanges(ang_plot_bundle);
	ang_plot_bundle.plot_axis.x_axis_title = "#theta";
	ss << "# of tracks / " << 1000.0 * hist->GetXaxis()->GetBinWidth(1)
			<< " mrad";

	ang_plot_bundle.plot_axis.y_axis_title = ss.str();

	return ang_plot_bundle;
}

NeatPlotting::Booky PndLmdPlotter::create2DFitResultPlots(
		const std::vector<PndLmdAngularData> &data_vec) const {

	std::map<PndLmdLumiFitOptions,
			std::map<NeatPlotting::SubpadCoordinates, NeatPlotting::PlotBundle>,
			LumiFit::Comparisons::fit_options_compare> ordered_plots;

	NeatPlotting::PlotStyle normal_plot_style;
	normal_plot_style.palette_color_style = 1;

// model types
	LumiFit::PndLmdFitModelOptions fitop_thmctruth(LumiFit::MC, LumiFit::THETA,
			false);
	LumiFit::PndLmdFitModelOptions fitop_mcacc(LumiFit::MC_ACC, LumiFit::THETA,
			false);
	LumiFit::PndLmdFitModelOptions fitop_normal(LumiFit::RECO, LumiFit::THETA,
			false);

// go through data map
	for (std::vector<PndLmdAngularData>::const_iterator top_it = data_vec.begin();
			top_it != data_vec.end(); top_it++) {

		// we only want IP info, so throw out everything else
		if (top_it->getPrimaryDimension().dimension_options.track_param_type
				!= LumiFit::IP) {
			continue;
		}
		// ok we are actually only interested in the dimension type and track type

		// loop over fit results
		map<PndLmdLumiFitOptions, PndLmdLumiFitResult> fit_results =
				top_it->getFitResults();
		for (map<PndLmdLumiFitOptions, PndLmdLumiFitResult>::iterator it =
				fit_results.begin(); it != fit_results.end(); it++) {

			// PAD1: if we have MC data and momentum transfer and we fit that with mc_t model
			// skip this pad in the beginning to find the matching panels later due to different fit ranges

			// mc stuff:
			if (top_it->getPrimaryDimension().dimension_options.track_type
					== LumiFit::MC
					&& (top_it->getPrimaryDimension().dimension_options.dimension_type
							== LumiFit::THETA_X
							|| top_it->getPrimaryDimension().dimension_options.dimension_type
									== LumiFit::THETA)) {

				if (fitop_thmctruth.equalBinaryOptions(
						it->first.getFitModelOptions())) {
					NeatPlotting::SubpadCoordinates pad_coord(1, 1);
					TH2D *data = top_it->get2DHistogram();
					ordered_plots[it->first][pad_coord] = createLumiFit2DPlotBundle(data);

					PndLmdLumiFitResult fit_res = top_it->getFitResult(it->first);
					if (0 == fit_res.getModelFitResult().getFitStatus()) {
						TH2D* model = create2DHistogramFromFitResult(it->first, *top_it);
						pad_coord = std::make_pair(1, 2);
						ordered_plots[it->first][pad_coord] = createLumiFit2DPlotBundle(
								model);
						TH2D* diff = neat_plot_helper.makeDifferenceHistogram(data, model);
						pad_coord = std::make_pair(1, 3);
						ordered_plots[it->first][pad_coord] = createLumiFit2DPlotBundle(
								diff);

						double lumi_ref = top_it->getReferenceLuminosity();

						stringstream strstream;
						strstream.precision(3);

						strstream << "p_{lab} = " << top_it->getLabMomentum() << " GeV";

						NeatPlotting::TextStyle text_style;
						NeatPlotting::PlotLabel plot_label(strstream.str(), text_style);
						ordered_plots[it->first][pad_coord].plot_decoration.label_text_leftpos =
								0.7;
						ordered_plots[it->first][pad_coord].plot_decoration.label_text_toppos =
								0.95;
						ordered_plots[it->first][pad_coord].plot_decoration.label_text_spacing =
								0.06;
						ordered_plots[it->first][pad_coord].plot_decoration.use_line_layout =
								true;
						ordered_plots[it->first][pad_coord].plot_decoration.labels.push_back(
								plot_label);
						strstream.str("");

						if (fit_res.getModelFitResult().getFitStatus() == 0) {
							strstream.str("");
							strstream << std::setprecision(3) << "#Delta L/L = "
									<< calulateLumiRelDiff(fit_res.getLuminosity(),
											fit_res.getLuminosityError(), lumi_ref).first << " #pm "
									<< calulateLumiRelDiff(fit_res.getLuminosity(),
											fit_res.getLuminosityError(), lumi_ref).second << " %";
							plot_label.setTitle(strstream.str());
							ordered_plots[it->first][pad_coord].plot_decoration.labels.push_back(
									plot_label);

							const std::set<ModelStructs::minimization_parameter> fit_params =
									fit_res.getModelFitResult().getFitParameters();
							std::set<ModelStructs::minimization_parameter>::const_iterator fit_param_it;
							for (fit_param_it = fit_params.begin();
									fit_param_it != fit_params.end(); fit_param_it++) {
								strstream.str("");
								strstream << std::setprecision(3) << fit_param_it->name.second
										<< "= " << fit_param_it->value << " #pm "
										<< fit_param_it->error;
								plot_label.setTitle(strstream.str());

								ordered_plots[it->first][pad_coord].plot_decoration.labels.push_back(
										plot_label);
							}
						}
					}
				}
			}

			// mc acc stuff:
			if (top_it->getPrimaryDimension().dimension_options.track_type
					== LumiFit::MC_ACC
					&& (top_it->getPrimaryDimension().dimension_options.dimension_type
							== LumiFit::THETA_X
							|| top_it->getPrimaryDimension().dimension_options.dimension_type
									== LumiFit::THETA)) {

				if (fitop_mcacc.equalBinaryOptions(it->first.getFitModelOptions())) {
					NeatPlotting::SubpadCoordinates pad_coord(2, 1);
					TH2D *data = top_it->get2DHistogram();
					ordered_plots[it->first][pad_coord] = createLumiFit2DPlotBundle(data);

					PndLmdLumiFitResult fit_res = top_it->getFitResult(it->first);
					if (0 == fit_res.getModelFitResult().getFitStatus()) {
						TH2D* model = create2DHistogramFromFitResult(it->first, *top_it);
						pad_coord = std::make_pair(2, 2);
						ordered_plots[it->first][pad_coord] = createLumiFit2DPlotBundle(
								model);
						TH2D* diff = neat_plot_helper.makeDifferenceHistogram(data, model);
						pad_coord = std::make_pair(2, 3);
						ordered_plots[it->first][pad_coord] = createLumiFit2DPlotBundle(
								diff);

						double lumi_ref = top_it->getReferenceLuminosity();

						stringstream strstream;
						strstream.precision(3);

						strstream << "p_{lab} = " << top_it->getLabMomentum() << " GeV";

						NeatPlotting::TextStyle text_style;
						NeatPlotting::PlotLabel plot_label(strstream.str(), text_style);
						ordered_plots[it->first][pad_coord].plot_decoration.label_text_leftpos =
								0.7;
						ordered_plots[it->first][pad_coord].plot_decoration.label_text_toppos =
								0.95;
						ordered_plots[it->first][pad_coord].plot_decoration.label_text_spacing =
								0.06;
						ordered_plots[it->first][pad_coord].plot_decoration.use_line_layout =
								true;
						ordered_plots[it->first][pad_coord].plot_decoration.labels.push_back(
								plot_label);
						strstream.str("");

						if (fit_res.getModelFitResult().getFitStatus() == 0) {
							strstream.str("");
							strstream << std::setprecision(3) << "#Delta L/L = "
									<< calulateLumiRelDiff(fit_res.getLuminosity(),
											fit_res.getLuminosityError(), lumi_ref).first << " #pm "
									<< calulateLumiRelDiff(fit_res.getLuminosity(),
											fit_res.getLuminosityError(), lumi_ref).second << " %";
							plot_label.setTitle(strstream.str());
							ordered_plots[it->first][pad_coord].plot_decoration.labels.push_back(
									plot_label);

							const std::set<ModelStructs::minimization_parameter> fit_params =
									fit_res.getModelFitResult().getFitParameters();
							std::set<ModelStructs::minimization_parameter>::const_iterator fit_param_it;
							for (fit_param_it = fit_params.begin();
									fit_param_it != fit_params.end(); fit_param_it++) {
								strstream.str("");
								strstream << std::setprecision(3) << fit_param_it->name.second
										<< "= " << fit_param_it->value << " #pm "
										<< fit_param_it->error;
								plot_label.setTitle(strstream.str());

								ordered_plots[it->first][pad_coord].plot_decoration.labels.push_back(
										plot_label);
							}
						}
					}
				}

			}
			// reco stuff:
			if (top_it->getPrimaryDimension().dimension_options.track_type
					== LumiFit::RECO
					&& (top_it->getPrimaryDimension().dimension_options.dimension_type
							== LumiFit::THETA_X
							|| top_it->getPrimaryDimension().dimension_options.dimension_type
									== LumiFit::THETA)) {
				bool with_secondaries(true);

				const std::set<LumiFit::LmdDimension> &selection_set =
						top_it->getSelectorSet();

				std::set<LumiFit::LmdDimension>::const_iterator selection_set_it;
				for (selection_set_it = selection_set.begin();
						selection_set_it != selection_set.end(); selection_set_it++) {
					if (selection_set_it->dimension_options.dimension_type
							== LumiFit::SECONDARY) {
						if (selection_set_it->dimension_range.getDimensionMean() < 0) {
							with_secondaries = false;
							break;
						}
					}
				}

				if (with_secondaries) {
					if (fitop_normal.equalBinaryOptions(it->first.getFitModelOptions())) {
						NeatPlotting::SubpadCoordinates pad_coord(3, 1);
						TH2D *data = top_it->get2DHistogram();
						ordered_plots[it->first][pad_coord] = createLumiFit2DPlotBundle(
								data);

						PndLmdLumiFitResult fit_res = top_it->getFitResult(it->first);
						if (0 == fit_res.getModelFitResult().getFitStatus()) {
							TH2D* model = create2DHistogramFromFitResult(it->first, *top_it);
							pad_coord = std::make_pair(3, 2);
							ordered_plots[it->first][pad_coord] = createLumiFit2DPlotBundle(
									model);

							TH2D* diff = neat_plot_helper.makeDifferenceHistogram(data,
									model);
							pad_coord = std::make_pair(3, 3);
							ordered_plots[it->first][pad_coord] = createLumiFit2DPlotBundle(
									diff);

							double lumi_ref = top_it->getReferenceLuminosity();

							stringstream strstream;
							strstream.precision(3);

							strstream << "p_{lab} = " << top_it->getLabMomentum() << " GeV";

							NeatPlotting::TextStyle text_style;
							NeatPlotting::PlotLabel plot_label(strstream.str(), text_style);
							ordered_plots[it->first][pad_coord].plot_decoration.label_text_leftpos =
									0.7;
							ordered_plots[it->first][pad_coord].plot_decoration.label_text_toppos =
									0.95;
							ordered_plots[it->first][pad_coord].plot_decoration.label_text_spacing =
									0.06;
							ordered_plots[it->first][pad_coord].plot_decoration.use_line_layout =
									true;
							ordered_plots[it->first][pad_coord].plot_decoration.labels.push_back(
									plot_label);
							strstream.str("");

							if (fit_res.getModelFitResult().getFitStatus() == 0) {
								strstream.str("");
								strstream << std::setprecision(3) << "#Delta L/L = "
										<< calulateLumiRelDiff(fit_res.getLuminosity(),
												fit_res.getLuminosityError(), lumi_ref).first << " #pm "
										<< calulateLumiRelDiff(fit_res.getLuminosity(),
												fit_res.getLuminosityError(), lumi_ref).second << " %";
								plot_label.setTitle(strstream.str());
								ordered_plots[it->first][pad_coord].plot_decoration.labels.push_back(
										plot_label);

								const std::set<ModelStructs::minimization_parameter> fit_params =
										fit_res.getModelFitResult().getFitParameters();
								std::set<ModelStructs::minimization_parameter>::const_iterator fit_param_it;
								for (fit_param_it = fit_params.begin();
										fit_param_it != fit_params.end(); fit_param_it++) {
									strstream.str("");
									strstream << std::setprecision(3) << fit_param_it->name.second
											<< "= " << fit_param_it->value << " #pm "
											<< fit_param_it->error;
									plot_label.setTitle(strstream.str());

									ordered_plots[it->first][pad_coord].plot_decoration.labels.push_back(
											plot_label);
								}
							}
						}
					}
				} else {
					/*NeatPlotting::SubpadCoordinates pad_coord(2, 2);

					 if (fitop_normal.equalBinaryOptions(it->first.getFitModelOptions())) {
					 NeatPlotting::SubpadCoordinates pad_coord(1, 1);
					 TH2D *data = top_it->get2DHistogram();
					 ordered_plots[it->first][pad_coord] = createLumiFit2DPlotBundle(
					 data);

					 PndLmdLumiFitResult *fit_res = top_it->getFitResult(it->first);
					 if (0 == fit_res->getModelFitResult().getFitStatus()) {
					 TH2D* model = create2DHistogramFromFitResult(it->first, *top_it);
					 NeatPlotting::SubpadCoordinates pad_coord(1, 2);
					 ordered_plots[it->first][pad_coord] = createLumiFit2DPlotBundle(
					 model);
					 TH2D* diff = neat_plot_helper.makeDifferenceHistogram(data,
					 model);
					 NeatPlotting::SubpadCoordinates pad_coord(1, 3);
					 ordered_plots[it->first][pad_coord] = createLumiFit2DPlotBundle(
					 diff);
					 }*/
				}
			}
		}
	}

	NeatPlotting::Booky booky;

	for (std::map<PndLmdLumiFitOptions,
			std::map<NeatPlotting::SubpadCoordinates, NeatPlotting::PlotBundle>,
			LumiFit::Comparisons::fit_options_compare>::iterator data_for_booky_page_it =
			ordered_plots.begin(); data_for_booky_page_it != ordered_plots.end();
			data_for_booky_page_it++) {

		std::map<NeatPlotting::SubpadCoordinates, NeatPlotting::PlotBundle>::iterator single_pad_plot_bundle_it;
		for (single_pad_plot_bundle_it = data_for_booky_page_it->second.begin();
				single_pad_plot_bundle_it != data_for_booky_page_it->second.end();
				single_pad_plot_bundle_it++) {
			booky.addPlotToCurrentBookyPage(single_pad_plot_bundle_it->second,
					normal_plot_style, single_pad_plot_bundle_it->first);
		}
		booky.addCurrentPageToBooky();
	}

	return booky;

}

// here only a specific ip setting and dimensioning should be used
NeatPlotting::Booky PndLmdPlotter::makeVertexFitResultBooky(
		const std::vector<PndLmdVertexData> &data_vec) {

	NeatPlotting::Booky booky;

	NeatPlotting::PlotStyle plot_style;

	for (unsigned int i = 0; i < data_vec.size(); i++) {
		NeatPlotting::SubpadCoordinates pad_coord;

		NeatPlotting::PlotBundle plot_bundle(makeVertexGraphBundle1D(data_vec[i]));

		if (data_vec[i].getName().EqualTo("mc_x")) {
			pad_coord = std::make_pair(1, 1);
			plot_style.y_axis_style.log_scale = true;
		} else if (data_vec[i].getName().EqualTo("mc_y")) {
			pad_coord = std::make_pair(2, 1);
			plot_style.y_axis_style.log_scale = true;
		} else if (data_vec[i].getName().EqualTo("mc_z")) {
			pad_coord = std::make_pair(3, 1);
			plot_style.y_axis_style.log_scale = true;
		} else if (data_vec[i].getName().EqualTo("reco_x")) {
			pad_coord = std::make_pair(1, 2);
			plot_style.y_axis_style.log_scale = false;
		} else if (data_vec[i].getName().EqualTo("reco_y")) {
			pad_coord = std::make_pair(2, 2);
			plot_style.y_axis_style.log_scale = false;
		} else if (data_vec[i].getName().EqualTo("reco_z")) {
			pad_coord = std::make_pair(3, 2);
			plot_style.y_axis_style.log_scale = false;
		}

		booky.addPlotToCurrentBookyPage(plot_bundle, plot_style, pad_coord);
	}
	booky.addCurrentPageToBooky();
	return booky;
}

void PndLmdPlotter::makeVertexDifferencesBooky(
		std::vector<PndLmdVertexData> &res_vec,
		std::vector<PndLmdVertexData> &res_vec_ref) {
}

void PndLmdPlotter::createAcceptanceComparisonBooky(
		std::vector<std::pair<PndLmdAcceptance, PndLmdAcceptance> > &acc_matches) {

	NeatPlotting::Booky booky;

	NeatPlotting::DataObjectStyle style;
	style.draw_option = "PE";
	style.marker_style.marker_style = 20;

	NeatPlotting::PlotStyle plot_style;

// create new page for each matched pair
	for (unsigned int i = 0; i < acc_matches.size(); i++) {
		std::pair<PndLmdAcceptance, PndLmdAcceptance> acc_pair = acc_matches[i];

		NeatPlotting::PlotBundle plot_bundle;

		style.line_style.line_color = kRed;
		style.marker_style.marker_color = kRed;

		TGraphAsymmErrors *acc = createAcceptanceGraph(&acc_pair.second);
		plot_bundle.addGraph(acc, style);

		plot_bundle.plot_axis.x_axis_title =
				acc_pair.first.getPrimaryDimension().createAxisLabel();
		plot_bundle.plot_axis.y_axis_title = "efficiency";

		style.line_style.line_color = kBlack;
		style.marker_style.marker_color = kBlack;
		TGraphAsymmErrors *ref_acc = createAcceptanceGraph(&acc_pair.first);
		plot_bundle.addGraph(ref_acc, style);

		// add full efficiency line
		plot_bundle.plot_decoration.x_parallel_lines.push_back(1.0);

		booky.addPlotToCurrentBookyPage(plot_bundle, plot_style,
				NeatPlotting::SubpadCoordinates(1, 1));

		// ---------- ratio ----------
		NeatPlotting::PlotBundle ratio_bundle;
		ratio_bundle.plot_axis.y_axis_range.active = true;
		ratio_bundle.plot_axis.y_axis_range.low = 0.9;
		ratio_bundle.plot_axis.y_axis_range.high = 1.1;

		style.line_style.line_color = kRed;
		style.marker_style.marker_color = kRed;

		TGraphAsymmErrors *ratio = neat_plot_helper.makeRatioGraph(acc, ref_acc);
		ratio_bundle.addGraph(ratio, style);

		ratio_bundle.plot_axis.x_axis_title =
				acc_pair.first.getPrimaryDimension().createAxisLabel();
		ratio_bundle.plot_axis.y_axis_title = "efficiency ratio";

		// add full efficiency line
		ratio_bundle.plot_decoration.x_parallel_lines.push_back(1.0);

		booky.addPlotToCurrentBookyPage(ratio_bundle, plot_style,
				NeatPlotting::SubpadCoordinates(2, 1));

		booky.addCurrentPageToBooky();
	}
	booky.createBooky("acceptance_comparison_booky.pdf");
}

// resolution booky stuff
NeatPlotting::Booky PndLmdPlotter::makeResolutionFitResultBooky(
		const std::vector<PndLmdHistogramData> &data_vec, int x, int y) const {

	NeatPlotting::Booky booky;

	NeatPlotting::PlotStyle plot_style;
	plot_style.palette_color_style = 1;

	for (unsigned int i = 0; i < data_vec.size(); i++) {
		NeatPlotting::SubpadCoordinates pad_coord(i % x + 1, (i % (x * y)) / x + 1);

		NeatPlotting::PlotBundle plot_bundle = makeResolutionGraphBundle(
				data_vec[i]);

		booky.addPlotToCurrentBookyPage(plot_bundle, plot_style, pad_coord);

		// if i is multiple of x*y append page
		if ((i + 1) % (x * y) == 0 || i + 1 == data_vec.size())
			booky.addCurrentPageToBooky();
	}
	return booky;
}

/*std::vector<PndLmdLumiHelper::lmd_graph*> determineReferenceGraphs(
 std::map<std::set<LumiFit::LmdDimension>,
 std::vector<PndLmdLumiHelper::lmd_graph*> > &selection_sorted_graphs) {
 // try to find an entry without any selections if not take the first
 std::vector<PndLmdLumiHelper::lmd_graph*> reference_graphs;

 std::map<std::set<LumiFit::LmdDimension>,
 std::vector<PndLmdLumiHelper::lmd_graph*> >::iterator selection_sorted_graphs_iter;

 for (selection_sorted_graphs_iter = selection_sorted_graphs.begin();
 selection_sorted_graphs_iter != selection_sorted_graphs.end();
 selection_sorted_graphs_iter++) {
 if (selection_sorted_graphs_iter->first.size() == 0) {
 break;
 }
 }
 if (selection_sorted_graphs_iter != selection_sorted_graphs.end()) {
 if (selection_sorted_graphs.size() > 0) {
 selection_sorted_graphs_iter = selection_sorted_graphs.begin();
 }
 }

 reference_graphs = selection_sorted_graphs_iter->second;
 selection_sorted_graphs.erase(selection_sorted_graphs_iter);

 return reference_graphs;
 }*/

NeatPlotting::PlotBundle PndLmdPlotter::createResolutionParameterizationPlot(
		PndLmdLumiHelper::lmd_graph* graph) {
	NeatPlotting::PlotBundle plot_bundle;

	NeatPlotting::TextStyle text_style;
	NeatPlotting::DataObjectStyle data_obj_style;

	if (graph->fit_options->getFitModelOptions().fit_dimension == 1) {
		// add graph
		data_obj_style.draw_option = "PE";
		plot_bundle.addGraph(graph->graph, data_obj_style);
	} else if (graph->fit_options->getFitModelOptions().fit_dimension == 2) {
		// add graph
		data_obj_style.draw_option = "COLZ";
		plot_bundle.addGraph(graph->graph, data_obj_style);
	}

	LumiFit::LmdDimension lmd_dim;
	lmd_dim.dimension_options = *graph->dependencies.begin();
	plot_bundle.plot_axis.x_axis_title = lmd_dim.createAxisLabel();

	std::string variable_name = graph->parameter_name_stack[1].second;
	std::stringstream yaxis_label;
	std::cout << variable_name << std::endl;
	if (variable_name.compare("asymm_gauss_sigma_left") == 0) {
		yaxis_label << "#sigma_{left} / "
				<< graph->resolution_dimensions[0].createUnitLabel();
	} else if (variable_name.compare("asymm_gauss_sigma_right") == 0) {
		yaxis_label << "#sigma_{right} / "
				<< graph->resolution_dimensions[0].createUnitLabel();
	} else if (variable_name.compare("asymm_gauss_mean") == 0) {
		yaxis_label << "#mu / "
				<< graph->resolution_dimensions[0].createUnitLabel();
	}
	plot_bundle.plot_axis.y_axis_title = yaxis_label.str();

	// add label
	stringstream labeltext;
	labeltext << graph->parameter_name_stack[1].first << ":"
			<< graph->parameter_name_stack[1].second;
	NeatPlotting::PlotLabel label(labeltext.str(), text_style);
	label.setRelativePosition(0.1, 0.95);
	plot_bundle.plot_decoration.labels.push_back(label);

	if (graph->fit_result) {
		bool model_exists = true;
		PndLmdModelFactory model_factory; // construct model factory
		// specify which of type of smearing model we want to generate

		// generate the model
		shared_ptr<Model> model = model_factory.generate1DResolutionModel(
				graph->fit_options->getFitModelOptions());
		// get the model that we have to fit to the data
		for (std::map<unsigned int, std::pair<std::string, std::string> >::const_iterator parameter_name =
				graph->parameter_name_stack.begin();
				parameter_name != graph->parameter_name_stack.end(); parameter_name++) {
			ParametrizationProxy par_proxy =
					model->getModelParameterHandler().getParametrizationProxyForModelParameter(
							parameter_name->second.second);
			std::cout << "Finding parametrization model for "
					<< parameter_name->second.second << std::endl;

			if (par_proxy.hasParametrizationModel()) {
				model = par_proxy.getParametrizationModel()->getModel();
			} else {
				std::cout
						<< "ERROR: Not able to obtain parametrization model for parameter "
						<< parameter_name->second.second << "!" << std::endl;
				model_exists = false;
				break;
			}
		}

		// create graph for model if it exists
		if (model_exists) {
			std::cout << graph->fit_result << std::endl;
			std::cout << graph->fit_result->getFinalEstimatorValue() << std::endl;
			model->getModelParameterHandler().initModelParametersFromFitResult(
					*graph->fit_result);
			model->getModelParameterSet().printInfo();

			ROOTDataHelper data_helper;

			shared_ptr<Data> data(new Data(1));
			data_helper.fillBinnedData(data, graph->graph);
			ModelVisualizationProperties1D vis_prop(data);

			DataStructs::DimensionRange plot_range =
					graph->fit_options->getEstimatorOptions().getFitRangeX();
			vis_prop.setPlotRange(plot_range);
			TGraphAsymmErrors *model_graph = root_plotter.createGraphFromModel1D(
					model, vis_prop);
			data_obj_style.line_style.line_color = 2;
			data_obj_style.draw_option = "C";
			plot_bundle.addGraph(model_graph, data_obj_style);
		}
	}

	return plot_bundle;
}

NeatPlotting::Booky PndLmdPlotter::createResolutionParameterizationOverviewPlot(
		std::vector<PndLmdLumiHelper::lmd_graph*> &graph_vec) {

	NeatPlotting::Booky booky;
	NeatPlotting::PlotStyle plot_style;

	double row_col_size = sqrt(graph_vec.size());
	int num_pad_col = row_col_size;
	int num_pad_rows = row_col_size;

	if (row_col_size > num_pad_col)
		++num_pad_col;
	if (graph_vec.size() > num_pad_col * num_pad_rows)
		++num_pad_rows;

	for (unsigned int i = 0; i < graph_vec.size(); i++) {

		NeatPlotting::SubpadCoordinates pad_coord(i % num_pad_col + 1,
				(i % (num_pad_col * num_pad_rows)) / num_pad_col + 1);

		NeatPlotting::PlotBundle plot_bundle = createResolutionParameterizationPlot(
				graph_vec[i]);

		booky.addPlotToCurrentBookyPage(plot_bundle, plot_style, pad_coord);
	}

	booky.addCurrentPageToBooky();

	return booky;
}

/*void PndLmdPlotter::createResolutionParameterizationComparisonPlots(
 std::vector<PndLmdLumiHelper::lmd_graph*> &graphs,
 std::vector<PndLmdLumiHelper::lmd_graph*> &reference) {
 // go through the two vectors and make comparison graphs
 // here we just have to make sure we always compare the same model parameters

 if (graphs.size() > 0) {
 TCanvas c("c", (graphs[0]->getDependencyString()).c_str(), 1000, 700);
 if (graphs[0]->fit_options->getFitModelOptions().smearing_model
 == LumiFit::GAUSSIAN)
 c.Divide(2, 2);
 else if (graphs[0]->fit_options->getFitModelOptions().smearing_model
 == LumiFit::DOUBLE_GAUSSIAN)
 c.Divide(3, 3);
 else
 c.Divide(3, 2);

 int canvas_pad_counter = 1;

 // lets make difference graphs
 for (unsigned int graph_index = 0; graph_index < graphs.size();
 graph_index++) {
 PndLmdLumiHelper::lmd_graph *ref_graph(0);
 for (unsigned int ref_index = 0; ref_index < reference.size();
 ref_index++) {
 if (graphs[graph_index]->parameter_name_stack
 == reference[ref_index]->parameter_name_stack) {
 ref_graph = reference[ref_index];
 break;
 }
 }

 // if we found a matching graph pair
 if (ref_graph) {
 TGraphErrors* diff_graph = ROOTPlotHelper::makeDifferenceGraph(
 graphs[graph_index]->graph, ref_graph->graph);

 diff_graph->SetTitle(
 TString(graphs[graph_index]->parameter_name_stack[1].first) + ":"
 + TString(graphs[graph_index]->parameter_name_stack[1].second));

 c.cd(canvas_pad_counter);
 diff_graph->Draw("AP");
 ++canvas_pad_counter;
 }

 stringstream filename;
 filename.precision(3);
 filename << "resolution_parameters_difference_"
 << graphs[graph_index]->getDependencyString();
 for (std::set<LumiFit::LmdDimension>::const_iterator selection =
 graphs[graph_index]->remaining_selections.begin();
 selection != graphs[graph_index]->remaining_selections.end();
 selection++) {
 filename << "_" << *selection;
 }

 c.cd();
 TLatex title(0.2, 0.50, filename.str().c_str());
 title.SetTextSize(0.05);
 title.Draw();
 filename << ".pdf";
 c.SaveAs(filename.str().c_str());
 }
 }
 }*/

// systematics plotting stuff
/*
 void createFitRangeDependencyPlot(
 std::vector<std::pair<TGraphErrors*, TString> > &dependency_graphs,
 bool lower_fit_range, std::string filename_prefix) {
 TCanvas c("c", "", 1000, 700);

 TLegend legend(0.8, 0.8, 0.95, 0.95);
 bool first(true);
 for (unsigned int i = 0; i < dependency_graphs.size(); i++) {
 if (lower_fit_range)
 dependency_graphs[i].first->GetXaxis()->SetTitle(
 "lower fit range / mrad");
 else
 dependency_graphs[i].first->GetXaxis()->SetTitle(
 "upper fit range / mrad");
 dependency_graphs[i].first->GetYaxis()->SetTitle("#mu(#Delta L/L) / %");
 if (first) {
 dependency_graphs[i].first->Draw("AP");
 first = false;
 } else
 dependency_graphs[i].first->Draw("PSAME");
 legend.AddEntry(dependency_graphs[i].first, dependency_graphs[i].second,
 "lp");
 }
 legend.Draw();

 std::stringstream ss;
 ss << filename_prefix;
 if (lower_fit_range)
 ss << "lower";
 else
 ss << "upper";
 ss << "_fit_range_dependency.pdf";
 c.SaveAs(ss.str().c_str());
 }

 void cleanFitRangeClusterMap(
 std::map<double,
 std::map<LumiFit::PndLmdFitModelOptions,
 std::vector<ROOTPlotHelper::GraphPoint> > > &clusters) {

 //clear all data vectors that are smaller then 2
 std::map<double,
 std::map<LumiFit::PndLmdFitModelOptions,
 std::vector<ROOTPlotHelper::GraphPoint> > >::iterator fit_range_cluster_iter;
 for (fit_range_cluster_iter = clusters.begin();
 fit_range_cluster_iter != clusters.end(); fit_range_cluster_iter++) {
 std::map<LumiFit::PndLmdFitModelOptions,
 std::vector<ROOTPlotHelper::GraphPoint> >::iterator model_option_iter =
 fit_range_cluster_iter->second.begin();
 while (model_option_iter != fit_range_cluster_iter->second.end()) {
 if (model_option_iter->second.size() < 2)
 fit_range_cluster_iter->second.erase(model_option_iter++);
 else
 ++model_option_iter;
 }

 if (fit_range_cluster_iter->second.size() == 0)
 clusters.erase(fit_range_cluster_iter);
 }
 }

 std::map<double,
 std::vector<std::pair<PndLmdAngularData, PndLmdLumiFitOptions> > > clusterIntoFitRangeScans(
 std::vector<PndLmdAngularData> &data_vec, bool lower_fit_range) {

 std::map<double,
 std::vector<std::pair<PndLmdAngularData, PndLmdLumiFitOptions> > > fit_range_clustered_results;

 for (unsigned int j = 0; j < data_vec.size(); j++) {
 const map<PndLmdLumiFitOptions, PndLmdLumiFitResult*> fit_results =
 data_vec[j].getFitResults();
 map<PndLmdLumiFitOptions, PndLmdLumiFitResult*>::const_iterator fit_result_iter;
 for (fit_result_iter = fit_results.begin();
 fit_result_iter != fit_results.end(); fit_result_iter++) {
 if (lower_fit_range)
 fit_range_clustered_results[fit_result_iter->first.getEstimatorOptions().getFitRangeX().range_low].push_back(
 std::make_pair(data_vec[j], fit_result_iter->first));
 else
 fit_range_clustered_results[fit_result_iter->first.getEstimatorOptions().getFitRangeX().range_high].push_back(
 std::make_pair(data_vec[j], fit_result_iter->first));
 }
 }
 return fit_range_clustered_results;
 }

 std::map<double,
 std::map<LumiFit::PndLmdFitModelOptions,
 std::vector<ROOTPlotHelper::GraphPoint> > > createFitRangeDependencyGraphData(
 std::vector<PndLmdAngularData> &data_vec, bool lower_fit_range,
 LumiFit::LmdTrackType track_type) {

 std::map<double,
 std::map<LumiFit::PndLmdFitModelOptions,
 std::vector<ROOTPlotHelper::GraphPoint> > > return_map;

 // filter events for track type
 PndLmdDataFacade lmd_data_facade;

 LumiFit::LmdDimensionOptions lmd_dim_opt;
 lmd_dim_opt.track_type = track_type;
 LumiFit::Comparisons::data_primary_dimension_options_filter filter(
 lmd_dim_opt);
 std::vector<PndLmdAngularData> filtered_data_vec = lmd_data_facade.filterData<
 PndLmdAngularData>(data_vec, filter);

 std::map<double,
 std::vector<std::pair<PndLmdAngularData, PndLmdLumiFitOptions> > > fit_range_clusters =
 clusterIntoFitRangeScans(filtered_data_vec, lower_fit_range);

 std::map<double,
 std::vector<std::pair<PndLmdAngularData, PndLmdLumiFitOptions> > >::iterator fit_range_cluster_iter;
 for (fit_range_cluster_iter = fit_range_clusters.begin();
 fit_range_cluster_iter != fit_range_clusters.end();
 fit_range_cluster_iter++) {
 // for each cluster create a vector of graph points
 std::vector<std::pair<PndLmdAngularData, PndLmdLumiFitOptions> >::iterator data_it;
 for (data_it = fit_range_cluster_iter->second.begin();
 data_it != fit_range_cluster_iter->second.end(); data_it++) {

 double lumi_ref = data_it->first.getReferenceLuminosity();

 PndLmdLumiFitResult* fit_result = data_it->first.getFitResult(
 data_it->second);
 if (fit_result->getModelFitResult().getFitStatus() == 0) {
 double lumi = fit_result->getLuminosity();
 double lumi_err = fit_result->getLuminosityError();
 ROOTPlotHelper::GraphPoint gp;
 if (lower_fit_range)
 gp.x = data_it->second.getEstimatorOptions().getFitRangeX().range_low;
 else
 gp.x =
 data_it->second.getEstimatorOptions().getFitRangeX().range_high;
 gp.y = 100.0 * (lumi - lumi_ref) / lumi_ref;
 gp.y_err_low = 100.0 * lumi_err / lumi_ref;
 gp.y_err_high = gp.y_err_low;
 double fit_range_value =
 data_it->second.getEstimatorOptions().getFitRangeX().range_low;
 if (lower_fit_range)
 fit_range_value =
 data_it->second.getEstimatorOptions().getFitRangeX().range_high;
 return_map[fit_range_value][data_it->second.getFitModelOptions()].push_back(
 gp);
 }
 }
 }

 cleanFitRangeClusterMap(return_map);
 return return_map;
 }

 std::vector<std::pair<TGraphErrors*, TString> > createFitRangeDependencyGraphsFromFullPhiData(
 std::map<double,
 std::map<LumiFit::PndLmdFitModelOptions,
 std::vector<ROOTPlotHelper::GraphPoint> > >& fit_boundary_graph_data,
 std::string filename_addition, int marker_style) {
 std::vector<std::pair<TGraphErrors*, TString> > fit_range_dependency;
 std::map<double,
 std::map<LumiFit::PndLmdFitModelOptions,
 std::vector<ROOTPlotHelper::GraphPoint> > >::iterator fit_boundary_iter;
 int marker_color = 0;
 for (fit_boundary_iter = fit_boundary_graph_data.begin();
 fit_boundary_iter != fit_boundary_graph_data.end(); fit_boundary_iter++) {
 std::map<LumiFit::PndLmdFitModelOptions,
 std::vector<ROOTPlotHelper::GraphPoint> >::iterator graph_data_iter;
 for (graph_data_iter = fit_boundary_iter->second.begin();
 graph_data_iter != fit_boundary_iter->second.end(); graph_data_iter++) {
 std::stringstream label;
 label << filename_addition;
 if (graph_data_iter->first.smearing_model == LumiFit::GAUSSIAN) {
 label << " + gaussian";
 marker_color++;
 } else if (graph_data_iter->first.smearing_model
 == LumiFit::ASYMMETRIC_GAUSSIAN) {
 label << " + asymm. gaussian";
 marker_color++;
 }
 label << " " << fit_boundary_iter->first;

 TGraphErrors* graph = (TGraphErrors*) ROOTPlotHelper::makeGraph(
 graph_data_iter->second, marker_style, marker_color);
 fit_range_dependency.push_back(
 std::make_pair(graph, TString(label.str())));
 }
 }

 return fit_range_dependency;
 }

 std::map<PndLmdLumiFitOptions, std::vector<ROOTPlotHelper::GraphPoint> > createPhiDependencyGraphData(
 std::map<LumiFit::LmdDimension, std::vector<PndLmdAngularData> > &phi_slice_map,
 LumiFit::LmdTrackType track_type) {
 std::map<PndLmdLumiFitOptions, std::vector<ROOTPlotHelper::GraphPoint> > values;

 std::map<LumiFit::LmdDimension, std::vector<PndLmdAngularData> >::iterator phi_slice_iter;

 std::cout << "got " << phi_slice_map.size() << " entries in phi slice map!"
 << std::endl;
 for (phi_slice_iter = phi_slice_map.begin();
 phi_slice_iter != phi_slice_map.end(); phi_slice_iter++) {
 for (unsigned int j = 0; j < phi_slice_iter->second.size(); j++) {
 if (phi_slice_iter->second[j].getPrimaryDimension().dimension_options.track_type
 == track_type) {
 const map<PndLmdLumiFitOptions, PndLmdLumiFitResult*> fit_results =
 phi_slice_iter->second[j].getFitResults();
 double lumi_ref = phi_slice_iter->second[j].getReferenceLuminosity();

 // loop over fit results
 map<PndLmdLumiFitOptions, PndLmdLumiFitResult*>::const_iterator fit_result_iter;
 for (fit_result_iter = fit_results.begin();
 fit_result_iter != fit_results.end(); fit_result_iter++) {
 if (fit_result_iter->second->getModelFitResult().getFitStatus()
 == 0) {
 double lumi = fit_result_iter->second->getLuminosity();
 double lumi_err = fit_result_iter->second->getLuminosityError();
 ROOTPlotHelper::GraphPoint gp;
 gp.x = phi_slice_iter->first.dimension_range.getDimensionMean();
 gp.y = 100.0 * (lumi - lumi_ref) / lumi_ref;
 gp.y_err_low = 100.0 * lumi_err / lumi_ref;
 gp.y_err_high = gp.y_err_low;
 values[fit_result_iter->first].push_back(gp);
 }
 }
 }
 }
 }
 return values;
 }
 */
/*void makeFitDependencyBooky() {
 TCanvas c("c", "", 1000, 700);

 std::stringstream filename;
 filename << "lumifit_results_phi_sliced" << filename_suffix << ".pdf[";
 c.Print(filename.str().c_str());
 filename.str("");
 filename << "lumifit_results_phi_sliced" << filename_suffix << ".pdf";

 std::map<PndLmdLumiFitOptions, std::vector<ROOTPlotHelper::GraphPoint> >::iterator value_iter;

 for (value_iter = values.begin(); value_iter != values.end(); value_iter++) {
 TGraphErrors *graph = new TGraphErrors(value_iter->second.size());

 std::vector<ROOTPlotHelper::GraphPoint> points = value_iter->second;
 if (points.size() > 0) {
 for (unsigned int i = 0; i < points.size(); i++) {
 graph->SetPoint(i, points[i].x, points[i].y);
 graph->SetPointError(i, 0.0, points[i].y_err);
 }
 }

 graph->SetTitle("");
 graph->GetXaxis()->SetTitle("slice #mu(#phi_{LMD}) / rad");
 graph->GetYaxis()->SetTitle("#Delta L/L / %");
 graph->Draw("AP");
 graph->SetMarkerStyle(24);
 graph->SetMarkerSize(0.6);
 gPad->Update();
 TLine zero(gPad->GetUxmin(), 0.0, gPad->GetUxmax(), 0.0);
 TLine meanline(gPad->GetUxmin(), mean, gPad->GetUxmax(), mean);
 meanline.SetLineColor(2);
 zero.Draw();
 meanline.Draw();

 std::stringstream ss;
 ss << "#mu = " << mean << " #pm " << mean_err << " %";
 TLatex meantitle(
 0.6 * (gPad->GetUxmax() - gPad->GetUxmin()) + gPad->GetUxmin(),
 0.95 * (gPad->GetUymax() - gPad->GetUymin()) + gPad->GetUymin(),
 ss.str().c_str());
 meantitle.Draw();
 ss.str("");
 ss << "#theta_{fit} = ["
 << value_iter->first.getEstimatorOptions().getFitRangeX().range_low
 * 1000 << " mrad, "
 << value_iter->first.getEstimatorOptions().getFitRangeX().range_high
 * 1000 << " mrad]";
 TLatex fitrangetitle(
 0.6 * (gPad->GetUxmax() - gPad->GetUxmin()) + gPad->GetUxmin(),
 0.88 * (gPad->GetUymax() - gPad->GetUymin()) + gPad->GetUymin(),
 ss.str().c_str());
 fitrangetitle.Draw();

 c.Print(filename.str().c_str());
 }
 filename.str("");
 filename << "lumifit_results_phi_sliced" << filename_suffix << ".pdf]";
 c.Print(filename.str().c_str());
 }*/
/*
 std::map<double,
 std::map<LumiFit::PndLmdFitModelOptions,
 std::vector<ROOTPlotHelper::GraphPoint> > > createFitRangeDependencyGraphsFromPhiSlicedData(
 std::map<PndLmdLumiFitOptions, std::vector<ROOTPlotHelper::GraphPoint> > &values,
 bool lower_fit_range) {

 std::map<double,
 std::map<LumiFit::PndLmdFitModelOptions,
 std::vector<ROOTPlotHelper::GraphPoint> > > mean_fit_range_dependency_graph_data;

 std::map<PndLmdLumiFitOptions, std::vector<ROOTPlotHelper::GraphPoint> >::iterator value_iter;

 for (value_iter = values.begin(); value_iter != values.end(); value_iter++) {
 double mean = 0.0;
 double mean_err = 0.0;

 std::vector<ROOTPlotHelper::GraphPoint> points = value_iter->second;
 if (points.size() > 0) {
 for (unsigned int i = 0; i < points.size(); i++) {
 mean += points[i].y;
 mean_err += pow(points[i].y, 2.0);
 }

 mean = mean / points.size();
 mean_err = sqrt(mean_err) / points.size();
 ROOTPlotHelper::GraphPoint gp;
 if (lower_fit_range)
 gp.x = value_iter->first.getEstimatorOptions().getFitRangeX().range_low;
 else
 gp.x =
 value_iter->first.getEstimatorOptions().getFitRangeX().range_high;
 gp.y = mean;
 gp.y_err_low = mean_err;
 gp.y_err_high = mean_err;
 double fit_range_value =
 value_iter->first.getEstimatorOptions().getFitRangeX().range_low;
 if (lower_fit_range)
 fit_range_value =
 value_iter->first.getEstimatorOptions().getFitRangeX().range_high;
 mean_fit_range_dependency_graph_data[fit_range_value][value_iter->first.getFitModelOptions()].push_back(
 gp);
 }
 }

 cleanFitRangeClusterMap(mean_fit_range_dependency_graph_data);

 return mean_fit_range_dependency_graph_data;

 /*std::vector<std::pair<TGraphErrors*, TString> > fit_range_dependency;

 std::map<double,
 std::map<LumiFit::PndLmdFitModelOptions,
 std::vector<ROOTPlotHelper::GraphPoint> > >::iterator outer_mean_graph_data_iter;
 for (outer_mean_graph_data_iter = mean_fit_range_dependency_graph_data.begin();
 outer_mean_graph_data_iter != mean_fit_range_dependency_graph_data.end();
 outer_mean_graph_data_iter++) {
 std::map<LumiFit::PndLmdFitModelOptions,
 std::vector<ROOTPlotHelper::GraphPoint> >::iterator mean_graph_data_iter;
 for (mean_graph_data_iter = outer_mean_graph_data_iter->second.begin();
 mean_graph_data_iter != outer_mean_graph_data_iter->second.end();
 mean_graph_data_iter++) {
 int marker_style = 24;
 int marker_color = 1;
 TString label(filename_addition);
 if (mean_graph_data_iter->first.smearing_model == LumiFit::GAUSSIAN) {
 label += " + gaussian";
 } else if (mean_graph_data_iter->first.smearing_model
 == LumiFit::ASYMMETRIC_GAUSSIAN) {
 label += " + asymm. gaussian";
 marker_color = 2;
 }
 if (mean_graph_data_iter->second.size() > 2) {
 TGraphErrors* graph = (TGraphErrors*) ROOTPlotHelper::makeGraph(
 mean_graph_data_iter->second, marker_style, marker_color);
 fit_range_dependency.push_back(std::make_pair(graph, label));
 }
 }
 }
 return fit_range_dependency;*//*
 }

 void makeOverviewCanvas(
 std::map<LumiFit::LmdSimIPParameters, PndLmdResultPlotter::graph_bundle> &reco_graph_map,
 TString filename, PndLmdResultPlotter &plotter, bool use_2d) {
 std::cout << "size of reco graph map: " << reco_graph_map.size() << std::endl;
 // check for different magnitudes in x and y offsets
 std::set<double> x_offsets;
 std::set<double> y_offsets;
 std::map<LumiFit::LmdSimIPParameters, PndLmdResultPlotter::graph_bundle>::iterator map_it;
 for (map_it = reco_graph_map.begin(); map_it != reco_graph_map.end();
 map_it++) {
 x_offsets.insert(map_it->first.offset_x_mean);
 y_offsets.insert(map_it->first.offset_y_mean);
 }

 std::cout << "dividing canvas into: " << x_offsets.size() << " x "
 << y_offsets.size() << std::endl;
 TCanvas c;
 c.Divide(x_offsets.size(), y_offsets.size());
 for (map_it = reco_graph_map.begin(); map_it != reco_graph_map.end();
 map_it++) {
 unsigned int x_coord = std::distance(x_offsets.begin(),
 x_offsets.find(map_it->first.offset_x_mean)) + 1;	// add 1 because pad starts counting at zero
 unsigned int y_coord = std::distance(
 y_offsets.find(map_it->first.offset_y_mean), --y_offsets.end());

 std::stringstream ss;
 ss << "(" << map_it->first.offset_x_mean << ","
 << map_it->first.offset_y_mean << ")";
 map_it->second.labels.push_back(std::make_pair(ss.str().c_str(), 1));

 std::cout << "filling pad " << y_coord * x_offsets.size() + x_coord
 << std::endl;
 c.cd(y_coord * x_offsets.size() + x_coord);
 plotter.fillSinglePad(&c, map_it->second, use_2d);
 }

 c.SaveAs(filename);
 }

 void createFitDependencyComparisonGraphs(
 std::map<LumiFit::LmdDimension, std::vector<PndLmdAngularData> > &phi_slice_map,
 std::vector<PndLmdAngularData> &full_phi_vec, bool lower_fit_range) {

 std::map<PndLmdLumiFitOptions, std::vector<ROOTPlotHelper::GraphPoint> > phi_sliced_graph_data =
 createPhiDependencyGraphData(phi_slice_map, LumiFit::RECO);
 std::map<double,
 std::map<LumiFit::PndLmdFitModelOptions,
 std::vector<ROOTPlotHelper::GraphPoint> > > phi_sliced_meand_graph_data =
 createFitRangeDependencyGraphsFromPhiSlicedData(phi_sliced_graph_data,
 lower_fit_range);
 std::cout << phi_sliced_meand_graph_data.size() << std::endl;
 std::vector<std::pair<TGraphErrors*, TString> > phi_sliced_graphs =
 createFitRangeDependencyGraphsFromFullPhiData(phi_sliced_meand_graph_data,
 "#phi sliced", 5);

 std::map<double,
 std::map<LumiFit::PndLmdFitModelOptions,
 std::vector<ROOTPlotHelper::GraphPoint> > > full_phi_graph_data =
 createFitRangeDependencyGraphData(full_phi_vec, lower_fit_range,
 LumiFit::RECO);
 std::cout << full_phi_graph_data.size() << std::endl;
 std::vector<std::pair<TGraphErrors*, TString> > full_phi_graphs =
 createFitRangeDependencyGraphsFromFullPhiData(full_phi_graph_data,
 "full #phi", 24);

 phi_sliced_graphs.insert(phi_sliced_graphs.end(), full_phi_graphs.begin(),
 full_phi_graphs.end());

 createFitRangeDependencyPlot(phi_sliced_graphs, lower_fit_range,
 "lumifit_results_reco_");

 // mc acc case

 phi_sliced_graph_data = createPhiDependencyGraphData(phi_slice_map,
 LumiFit::MC_ACC);
 phi_sliced_meand_graph_data = createFitRangeDependencyGraphsFromPhiSlicedData(
 phi_sliced_graph_data, lower_fit_range);

 phi_sliced_graphs = createFitRangeDependencyGraphsFromFullPhiData(
 phi_sliced_meand_graph_data, "#phi sliced", 5);

 full_phi_graph_data = createFitRangeDependencyGraphData(full_phi_vec,
 lower_fit_range, LumiFit::MC_ACC);
 full_phi_graphs = createFitRangeDependencyGraphsFromFullPhiData(
 full_phi_graph_data, "full #phi", 24);

 phi_sliced_graphs.insert(phi_sliced_graphs.end(), full_phi_graphs.begin(),
 full_phi_graphs.end());

 createFitRangeDependencyPlot(phi_sliced_graphs, lower_fit_range,
 "lumifit_results_mc_acc_");
 }
 */

NeatPlotting::PlotBundle PndLmdPlotter::createLowerFitRangeDependencyPlotBundle(
		const std::vector<PndLmdAngularData> &data_vec,
		LumiFit::PndLmdFitModelOptions model_opt) const {
// cluster data
	std::map<PndLmdHistogramData, std::vector<PndLmdAngularData> > clustered_data =
			clusterIntoGroups(data_vec);

// now create a graph for each cluster and add to bundle
	NeatPlotting::PlotBundle plot_bundle;

	NeatPlotting::DataObjectStyle data_style;
	data_style.draw_option = "PE";

	unsigned int max_graph_number = 9;
	int colors[9] = { 1, 2, 8, 9, 6, 3, 4, 30, 38 };
	int *pcolor = &colors[0];

	std::map<PndLmdHistogramData, std::vector<PndLmdAngularData> >::const_iterator it;
	if (max_graph_number >= clustered_data.size()) {
		for (it = clustered_data.begin(); it != clustered_data.end(); it++) {
			TGraphAsymmErrors *tgraph = createLowerFitRangeDependencyGraph(it->second,
					model_opt);

			data_style.line_style.line_color = *pcolor;
			data_style.marker_style.marker_color = *pcolor;

			plot_bundle.addGraph(tgraph, data_style);
			pcolor++;
		}
	}

	return plot_bundle;
}

std::map<PndLmdHistogramData, std::vector<PndLmdAngularData> > PndLmdPlotter::clusterIntoGroups(
		const std::vector<PndLmdAngularData> &data_vec) const {
	std::map<PndLmdHistogramData, std::vector<PndLmdAngularData> > clustered_data;

	for (unsigned int i = 0; i < data_vec.size(); i++) {
		clustered_data[data_vec[i]].push_back(data_vec[i]);
	}

	return clustered_data;
}

// we assume that the filtered_data list contains only the correct data types
TGraphAsymmErrors* PndLmdPlotter::createLowerFitRangeDependencyGraph(
		const std::vector<PndLmdAngularData> &prefiltered_data,
		LumiFit::PndLmdFitModelOptions model_opt) const {

	std::vector<NeatPlotting::GraphPoint> graph_data;

	for (unsigned int i = 0; i < prefiltered_data.size(); i++) {
		double lumi_ref = prefiltered_data[i].getReferenceLuminosity();

		NeatPlotting::GraphPoint graph_point;

		const map<PndLmdLumiFitOptions, PndLmdLumiFitResult> &fit_results =
				prefiltered_data[i].getFitResults();

		for (map<PndLmdLumiFitOptions, PndLmdLumiFitResult>::const_iterator iter =
				fit_results.begin(); iter != fit_results.end(); iter++) {

			if (model_opt.lessThanBinaryOptions(iter->first.getFitModelOptions())) {
				std::pair<double, double> lumival = calulateLumiRelDiff(
						iter->second.getLuminosity(), iter->second.getLuminosityError(),
						lumi_ref);
				graph_point.x =
						iter->first.getEstimatorOptions().getFitRangeX().range_low;
				graph_point.y = lumival.first;
				graph_point.y_err_low = lumival.second;
				graph_point.y_err_high = lumival.second;

				graph_data.push_back(graph_point);
			}
		}
	}

	NeatPlotting::GraphAndHistogramHelper graph_helper;
	return graph_helper.makeGraph(graph_data);
}

NeatPlotting::SystematicsAnalyser::SystematicDependencyGraphBundle PndLmdPlotter::createLowerFitRangeDependencyGraphBundle(
		const std::vector<PndLmdAngularData> &prefiltered_data,
		LumiFit::PndLmdFitModelOptions model_opt) const {

	NeatPlotting::SystematicsAnalyser systematics_analyzer;

	for (unsigned int i = 0; i < prefiltered_data.size(); i++) {
		double lumi_ref = prefiltered_data[i].getReferenceLuminosity();

		NeatPlotting::GraphPoint graph_point;

		const map<PndLmdLumiFitOptions, PndLmdLumiFitResult> &fit_results =
				prefiltered_data[i].getFitResults();

		for (map<PndLmdLumiFitOptions, PndLmdLumiFitResult>::const_iterator iter =
				fit_results.begin(); iter != fit_results.end(); iter++) {

			if (model_opt.lessThanBinaryOptions(iter->first.getFitModelOptions())) {
				std::pair<double, double> lumival = calulateLumiRelDiff(
						iter->second.getLuminosity(), iter->second.getLuminosityError(),
						lumi_ref);
				systematics_analyzer.insertDependencyValues(
						iter->first.getEstimatorOptions().getFitRangeX().range_low,
						lumival.first, lumival.second);
			}
		}
	}

	return systematics_analyzer.createSystematicsGraphBundle();
}

/*
 // we could create some plot stuff here from the graphBundle
 std::vector<PndLmdResultPlotter::graph_bundle> convertToPlotterGraphs(
 map<PndLmdLumiFitOptions, histBundle> &hist_bundle) {
 std::vector<PndLmdResultPlotter::graph_bundle> return_vec;

 for (map<PndLmdLumiFitOptions, histBundle>::iterator it = hist_bundle.begin();
 it != hist_bundle.end(); it++) {
 PndLmdResultPlotter::graph_bundle gb;
 gb.hist1d = new TH1D(*it->second.hist);
 gb.is_residual = false;
 gb.model = 0;
 std::stringstream s;
 s.precision(3);
 s << it->first.getEstimatorOptions().getFitRangeY().range_low << "-"
 << it->first.getEstimatorOptions().getFitRangeY().range_high;
 gb.labels.push_back(std::make_pair(TString(s.str().c_str()), 1));
 return_vec.push_back(gb);
 }
 return return_vec;
 }*/

/*
 std::pair<TGraphErrors*, TGraphErrors*> PndLmdPlotter::makeIPParameterDependencyGraphBundle(
 std::vector<PndLmdVertexData> &vertex_data,
 const LumiFit::LmdSimIPParameters &dependency, TString ytitle_subscript) {

 TGraphErrors *mean = 0;
 TGraphErrors *width = 0;

 if (vertex_data.size() > 0) {
 std::vector<double> x;
 std::vector<double> y_mean;
 std::vector<double> y_err_mean;

 std::vector<double> y_width;
 std::vector<double> y_err_width;

 for (unsigned int i = 0; i < vertex_data.size(); i++) {
 PndLmdLumiFitResult *fit_result =
 vertex_data[i].getFitResults().begin()->second;
 if (fit_result && fit_result->getModelFitResult().getFitStatus() == 0) {
 x.push_back(
 vertex_data[i].getSimulationIPParameters().getDependencyValue(
 dependency));

 double ideal_mean(0.0);
 double ideal_width(0.0);

 if (vertex_data[i].getPrimaryDimension().dimension_options.dimension_type
 == LumiFit::X) {
 ideal_mean = vertex_data[i].getSimulationIPParameters().offset_x_mean;
 ideal_width =
 vertex_data[i].getSimulationIPParameters().offset_x_width;
 } else if (vertex_data[i].getPrimaryDimension().dimension_options.dimension_type
 == LumiFit::X) {
 ideal_mean = vertex_data[i].getSimulationIPParameters().offset_y_mean;
 ideal_width =
 vertex_data[i].getSimulationIPParameters().offset_y_width;
 } else if (vertex_data[i].getPrimaryDimension().dimension_options.dimension_type
 == LumiFit::X) {
 ideal_mean = vertex_data[i].getSimulationIPParameters().offset_z_mean;
 ideal_width =
 vertex_data[i].getSimulationIPParameters().offset_z_width;
 }

 y_mean.push_back(
 fit_result->getModelFitResult().getFitParameter("gauss_mean").value
 - ideal_mean);
 y_err_mean.push_back(
 fit_result->getModelFitResult().getFitParameter("gauss_mean").error);

 y_width.push_back(
 fit_result->getModelFitResult().getFitParameter("gauss_sigma").value
 - ideal_width);
 y_err_width.push_back(
 fit_result->getModelFitResult().getFitParameter("gauss_sigma").error);
 }
 }

 mean = new TGraphErrors(x.size(), &x[0], &y_mean[0], 0, &y_err_mean[0]);
 mean->GetXaxis()->SetTitle(
 vertex_data[0].getSimulationIPParameters().getDependencyName(dependency).c_str());
 mean->GetYaxis()->SetTitle(
 TString("#delta#mu_{") + ytitle_subscript + "} [cm]");
 width = new TGraphErrors(x.size(), &x[0], &y_width[0], 0, &y_err_width[0]);
 width->GetXaxis()->SetTitle(
 vertex_data[0].getSimulationIPParameters().getDependencyName(dependency).c_str());
 width->GetYaxis()->SetTitle(
 TString("#delta#sigma_{") + ytitle_subscript + "} [cm]");

 }
 return std::make_pair(mean, width);
 }

 void PndLmdPlotter::plotIPDependencyGraphs(
 std::map<LumiFit::LmdSimIPParameters,
 std::map<LumiFit::LmdSimIPParameters,
 std::map<LumiFit::LmdDimensionType, std::vector<PndLmdVertexData> > > > &ip_data_clusters) {

 std::cout << "looping over clustered ip data map..." << std::endl;
 std::map<LumiFit::LmdSimIPParameters,
 std::map<LumiFit::LmdSimIPParameters,
 std::map<LumiFit::LmdDimensionType, std::vector<PndLmdVertexData> > > >::iterator dependency_it;
 for (dependency_it = ip_data_clusters.begin();
 dependency_it != ip_data_clusters.end(); dependency_it++) {

 std::cout << "dependency: " << dependency_it->first.getDependencyLabel()
 << std::endl;

 std::map<LumiFit::LmdSimIPParameters,
 std::map<LumiFit::LmdDimensionType, std::vector<PndLmdVertexData> > >::iterator ip_params_it;
 for (ip_params_it = dependency_it->second.begin();
 ip_params_it != dependency_it->second.end(); ip_params_it++) {

 std::cout << "remaining parameters: " << ip_params_it->first.getLabel()
 << std::endl;

 std::map<LumiFit::LmdDimensionType, std::vector<PndLmdVertexData> >::iterator dimension_type_it;
 for (dimension_type_it = ip_params_it->second.begin();
 dimension_type_it != ip_params_it->second.end();
 dimension_type_it++) {

 // get all mc objects
 std::vector < PndLmdVertexData > mc_ip_data;
 std::vector<PndLmdVertexData>::iterator start =
 dimension_type_it->second.begin();
 while (start != dimension_type_it->second.end()) {
 std::vector<PndLmdVertexData>::iterator it = std::find_if(start,
 dimension_type_it->second.end(), mc_dimension_compare);
 if (it != dimension_type_it->second.end()) {
 mc_ip_data.push_back(*it);
 it++;
 }
 start = it;
 }
 std::cout << "collected " << mc_ip_data.size()
 << " mc ip data for these settings!" << std::endl;

 // get all reco objects
 std::vector < PndLmdVertexData > reco_ip_data;
 start = dimension_type_it->second.begin();
 while (start != dimension_type_it->second.end()) {
 std::vector<PndLmdVertexData>::iterator it = std::find_if(start,
 dimension_type_it->second.end(), reco_dimension_compare);
 if (it != dimension_type_it->second.end()) {
 reco_ip_data.push_back(*it);
 it++;
 }
 start = it;
 }
 std::cout << "collected " << reco_ip_data.size()
 << " reco ip data for these settings!" << std::endl;

 std::stringstream dimension_str;
 if (dimension_type_it->first == LumiFit::X) {
 dimension_str << "x";
 } else if (dimension_type_it->first == LumiFit::Y) {
 dimension_str << "y";
 } else if (dimension_type_it->first == LumiFit::Z) {
 dimension_str << "z";
 }

 std::pair<TGraphErrors*, TGraphErrors*> mc_graph_pair =
 makeIPParameterDependencyGraphBundle(mc_ip_data,
 dependency_it->first, dimension_str.str());

 std::pair<TGraphErrors*, TGraphErrors*> reco_graph_pair =
 makeIPParameterDependencyGraphBundle(reco_ip_data,
 dependency_it->first, dimension_str.str());

 //combine reco and mc graph of each type
 std::vector < std::pair<TGraphErrors*, TString> > mean_graphs;
 mean_graphs.push_back(std::make_pair(mc_graph_pair.first, "MC"));
 mean_graphs.push_back(std::make_pair(reco_graph_pair.first, "Reco"));

 std::vector < std::pair<TGraphErrors*, TString> > width_graphs;
 width_graphs.push_back(std::make_pair(mc_graph_pair.second, "MC"));
 width_graphs.push_back(std::make_pair(reco_graph_pair.second, "Reco"));

 std::cout << "about to draw stuff!" << std::endl;
 //then generate new filename and save to pdf
 std::stringstream dir_path_str;
 dir_path_str << boost::filesystem::current_path().c_str() << "/";
 dir_path_str << "ip-tilt/"; // basedir
 dir_path_str << dependency_it->first.getDependencyLabel() << "/";
 dir_path_str << ip_params_it->first.getLabel();

 std::cout << "creating directory " << dir_path_str.str()
 << " before saving..." << std::endl;

 const path dir_path(dir_path_str.str());
 if (!exists(dir_path)) {
 if (!boost::filesystem::create_directories(dir_path)) {
 std::cout << "could not create directories... exiting...";
 return;
 }
 }

 TCanvas c;
 ROOTPlotHelper::drawMultipleGraphsOnCanvas (mean_graphs);
 std::stringstream file_url;
 file_url << dir_path_str.str() << "/" << dimension_str.str()
 << "_mean.pdf";

 c.SaveAs(file_url.str().c_str());

 c.Clear();
 ROOTPlotHelper::drawMultipleGraphsOnCanvas (width_graphs);
 file_url.str("");
 file_url << dir_path_str.str() << "/" << dimension_str.str()
 << "_width.pdf";

 c.SaveAs(file_url.str().c_str());
 }
 }
 }
 }

 */

/*
 NeatPlotting::PlotBundle PndLmdPlotter::makeResolutionGraphBundle2D(
 PndLmdHistogramData & res) {
 std::cout << "Creating resolution graph bundle!" << std::endl;

 const map<PndLmdLumiFitOptions, PndLmdLumiFitResult*>& fit_results =
 res.getFitResults();

 NeatPlotting::PlotBundle res_plot_bundle;

 TH2D* hist = res.get2DHistogram();
 TH2D* model(0);
 if (fit_results.size() > 0) {
 lmd_graph_bundle.est_options =
 fit_results.begin()->first.getEstimatorOptions();

 if (fit_results.begin()->second->getModelFitResult().getFitStatus() == 0) {
 model = create2DHistogramFromFitResult(fit_results.begin()->first, res);
 }
 }
 if (model)
 hist = ROOTPlotHelper::makeDifferenceHistogram(hist, model);
 hist->SetTitle("");
 hist->SetStats(0);

 hist->GetXaxis()->SetTitle(
 res.getPrimaryDimension().createAxisLabel().c_str());
 hist->GetYaxis()->SetTitle(
 res.getSecondaryDimension().createAxisLabel().c_str());

 lmd_graph_bundle.hist2d = hist;

 stringstream strstream;
 strstream.precision(3);

 strstream << "p_{lab} = " << res.getLabMomentum() << " GeV";
 lmd_graph_bundle.labels.push_back(
 std::make_pair(TString(strstream.str()), 1));

 const std::set<LumiFit::LmdDimension>& selc_set = res.getSelectorSet();
 for (std::set<LumiFit::LmdDimension>::const_iterator selc = selc_set.begin();
 selc != selc_set.end(); selc++) {

 lmd_graph_bundle.labels.push_back(
 std::make_pair(TString(selc->createSelectionLabel()), 1));
 }

 return lmd_graph_bundle;
 }
 */

/*
 void PndLmdPlotter::makeFitResultBooky(
 std::map<PndLmdLumiFitOptions, std::map<int, NeatPlotting::PlotBundle>,
 LumiFit::Comparisons::fit_options_compare> &graph_bundle_map,
 TString filename) {

 NeatPlotting::PlotStyle plot_style;

 TCanvas c("fit_overview", "fit_overview", 1000, 700);

 c.Divide(3, 3);

 c.Print(filename + "_overview.pdf["); // No actual print, just open file
 for (std::map<PndLmdLumiFitOptions, std::map<int, NeatPlotting::PlotBundle>,
 LumiFit::Comparisons::fit_options_compare>::iterator i =
 graph_bundle_map.begin(); i != graph_bundle_map.end(); i++) {
 c.cd();
 neat_plot_helper.fillGridCanvas(i->second, plot_style);
 c.Print(filename + "_overview.pdf"); // actually print canvas to file
 }
 c.Print(filename + "_overview.pdf]");

 }



 void PndLmdPlotter::makeResolutionBooky(
 std::vector<PndLmdHistogramData> &res_vec, TString filename) {
 std::vector<PndLmdResultPlotter::graph_bundle> graph_bundles;
 if (res_vec.size() > 0) {
 if (res_vec[0].getSecondaryDimension().is_active) { // 2d plots
 setDiffColorScale();
 for (unsigned int i = 0; i < res_vec.size(); i++) {
 graph_bundles.push_back(makeResolutionGraphBundle2D(res_vec[i]));
 }
 makeBooky(graph_bundles, filename, 8, 7, true);
 } else { // 1d plots
 for (unsigned int i = 0; i < res_vec.size(); i++) {
 graph_bundles.push_back(makeResolutionGraphBundle1D(res_vec[i]));
 }

 makeBooky(graph_bundles, filename, 5, 4);

 for (unsigned int i = 0; i < graph_bundles.size(); i++) {
 graph_bundles[i].is_residual = true;
 }
 makeBooky(graph_bundles, filename + "_residual", 5, 4);
 }
 }
 }

 void PndLmdPlotter::makeResolutionDifferencesBooky(
 std::vector<PndLmdHistogramData> &res_vec,
 std::vector<PndLmdHistogramData> &res_vec_ref) {

 std::vector<PndLmdResultPlotter::graph_bundle> graph_bundles;
 for (unsigned int i = 0; i < res_vec.size(); i++) {
 graph_bundles.push_back(makeResolutionGraphBundle1D(res_vec[i]));
 }

 std::vector<PndLmdResultPlotter::graph_bundle> graph_bundles_ref;
 for (unsigned int i = 0; i < res_vec_ref.size(); i++) {
 graph_bundles_ref.push_back(makeResolutionGraphBundle1D(res_vec_ref[i]));
 }

 std::vector<PndLmdResultPlotter::graph_bundle> diff_graph_bundles =
 createGraphRatioBundle(graph_bundles, graph_bundles_ref);

 makeBooky(diff_graph_bundles, "resolution_difference", 5, 4);
 }

 */

}
