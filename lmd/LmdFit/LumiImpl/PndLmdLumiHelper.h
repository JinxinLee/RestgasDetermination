/*
 * PndLmdLumiFitBase.h
 *
 *  Created on: Jun 27, 2012
 *      Author: steve
 */

#ifndef PNDLMDLUMIHELPER_H_
#define PNDLMDLUMIHELPER_H_

#if !defined(__CINT__)
#include "fit/ModelFitResult.h"
#include "core/ModelStructs.h"

#include "SharedPtr.h"

#endif /* __CINT __ */

#include "LumiFitStructs.h"
#include "fit/PndLmdLumiFitOptions.h"

#include <vector>
#include <map>

#include "TString.h"
#include "TGraphErrors.h"
#include "TGraph2DErrors.h"
#include "TDatabasePDG.h"

class Model;

class PndLmdAcceptance;
class PndLmdAngularData;
class PndLmdAbstractData;
class PndLmdHistogramData;

class TFile;
class TVector3;
class TH1D;
class TH2D;

/**
 * \brief This class contains a few functions helping the user with some basics.
 *
 * Main use of this class will be the actual interface to the data created by the lmd macros for filling #PndLmdDataBase objects.
 */
class PndLmdLumiHelper {
public:
	struct bin_dimension {
		DataStructs::DimensionRange x_range;
		DataStructs::DimensionRange y_range;

		bin_dimension() {
		}

		bin_dimension(LumiFit::LmdDimensionRange x_range_,
				LumiFit::LmdDimensionRange y_range_) {
			x_range.range_low = x_range_.getRangeLow();
			x_range.range_high = x_range_.getRangeHigh();
			y_range.range_low = y_range_.getRangeLow();
			y_range.range_high = y_range_.getRangeHigh();
		}

		bool operator<(const bin_dimension &rhs) const {
			if (x_range < rhs.x_range)
				return true;
			else if (x_range > rhs.x_range)
				return false;
			if (y_range < rhs.y_range)
				return true;
			else if (y_range > rhs.y_range)
				return false;

			return false;
		}
		bool operator>(const bin_dimension &rhs) const {
			return (rhs < *this);
		}
	};

	struct lmd_graph_data_point: public TObject {
		double theta;
		double phi;
		ModelStructs::minimization_parameter parameter;

		bool operator<(const lmd_graph_data_point &rhs) const {
			if (theta < rhs.theta)
				return true;
			else if (theta > rhs.theta)
				return false;
			if (phi < rhs.phi)
				return true;
			else if (phi > rhs.phi)
				return false;
			if (parameter.value < rhs.parameter.value)
				return true;
			else if (parameter.value > rhs.parameter.value)
				return false;

			return false;
		}
		bool operator>(const lmd_graph_data_point &rhs) const {
			return (rhs < *this);
		}

	ClassDef(lmd_graph_data_point, 1)
		;
	};

	struct lmd_graph: public TObject {
		double lab_momentum;

		TGraphErrors* graph;
		TGraph2DErrors* graph2d;
		std::vector<lmd_graph_data_point> data;

		ModelFitResult* fit_result;
		const PndLmdLumiFitOptions *fit_options;

		std::map<unsigned int, std::pair<std::string, std::string> > parameter_name_stack;

		std::vector<LumiFit::LmdDimension> resolution_dimensions;

		std::set<LumiFit::LmdDimension> remaining_selections;

		std::set<LumiFit::LmdDimensionOptions> dependencies;

		lmd_graph() :
				lab_momentum(0.0), graph(0), graph2d(0), fit_result(0), fit_options(0) {
		}
		virtual ~lmd_graph() {
		}

		std::string getDependencyString() const {
			std::stringstream ss;
			bool first(true);
			for (std::set<LumiFit::LmdDimensionOptions>::iterator dep_it =
					dependencies.begin(); dep_it != dependencies.end(); dep_it++) {
				if (first)
					first = false;
				else
					ss << "_";
				if (dep_it->dimension_type == LumiFit::THETA)
					ss << "theta";
				else if (dep_it->dimension_type == LumiFit::PHI)
					ss << "phi";
			}
			return ss.str();
		}

		bool operator<(const lmd_graph &rhs) const {
			if (lab_momentum < rhs.lab_momentum)
				return true;
			else if (lab_momentum > rhs.lab_momentum)
				return false;
			if (remaining_selections < rhs.remaining_selections)
				return true;
			else if (remaining_selections > rhs.remaining_selections)
				return false;
			if (data < rhs.data)
				return true;
			else if (data > rhs.data)
				return false;
			if (parameter_name_stack < rhs.parameter_name_stack)
				return true;
			else if (parameter_name_stack > rhs.parameter_name_stack)
				return false;
			if (dependencies < rhs.dependencies)
				return true;
			else if (dependencies > rhs.dependencies)
				return false;
			if (fit_options) {
				if (*fit_options < *rhs.fit_options)
					return true;
				else if (*fit_options > *rhs.fit_options)
					return false;
			}
			return false;
		}
		bool operator>(const lmd_graph &rhs) const {
			return (rhs < (*this));
		}
	ClassDef(lmd_graph, 1)
		;
	};

	struct findByDimensionType {
		findByDimensionType(const LumiFit::LmdDimensionType &dim_type_) :
				dim_type(dim_type_) {
		}
		bool operator()(const LumiFit::LmdDimension &lmd_dim) {
			return lmd_dim.dimension_options.dimension_type == dim_type;
		}
	private:
		LumiFit::LmdDimensionType dim_type;
	};

private:
	void finalizeLmdGraphObjects(
			std::map<std::set<LumiFit::LmdDimension>,
					std::map<std::pair<std::string, std::string>,
							PndLmdLumiHelper::lmd_graph*, ModelStructs::stringpair_comp> > &graph_map);

	double calculateBinOverlap(bin_dimension &reco_bin, bin_dimension &reso_bin);
public:
	PndLmdLumiHelper();

	static double getMomentumTransferFromTheta(double plab, double theta);

	double calcHistIntegral(const TH1D* hist,
			std::vector<DataStructs::DimensionRange> range);
	double calcHistIntegral(const TH2D* hist,
			std::vector<DataStructs::DimensionRange> range);

#ifndef __CINT__
	void removePointsAboveChi2(
			std::map<std::string, TGraphErrors*, ModelStructs::string_comp> &graph_map,
			double chi2);

	void fitParametrizationModelToData(PndLmdLumiHelper::lmd_graph *graph);

	std::map<double, ModelFitResult*> checkFitParameters(
			const std::map<PndLmdHistogramData*, ModelFitResult*> &fit_results) const;
#endif /* __CINT __ */

	std::vector<PndLmdLumiHelper::lmd_graph*> filterLmdGraphs(
			std::vector<PndLmdLumiHelper::lmd_graph*> &util_lmd_graphs,
			std::set<LumiFit::LmdDimensionOptions> dep);

	std::vector<PndLmdLumiHelper::lmd_graph*> filterLmdGraphs(
			const std::vector<PndLmdLumiHelper::lmd_graph*> &util_lmd_graphs,
			const std::set<LumiFit::LmdDimension> &selections) const;

	std::vector<PndLmdLumiHelper::lmd_graph*> getResolutionModelResultsFromFile(
			const std::string &file_url) const;

	std::vector<PndLmdLumiHelper::lmd_graph*> generateLmdGraphs(
			std::vector<PndLmdHistogramData>& resolutions,
			DataStructs::DimensionRange& fit_range);

	void saveLmdGraphsToFile(
			std::vector<PndLmdLumiHelper::lmd_graph*>& graph_vec);

	void fitParametrizationModelToGraphs(
			std::vector<PndLmdLumiHelper::lmd_graph*> &lmd_graphs);

	std::map<std::pair<double, double>,
			std::map<PndLmdLumiHelper::bin_dimension, double> > generate2DResolutionMap(
			const std::vector<PndLmdHistogramData>& resolutions,
			const PndLmdAngularData &data);
}
;

#endif /* PNDLMDLUMIHELPER_H_ */
