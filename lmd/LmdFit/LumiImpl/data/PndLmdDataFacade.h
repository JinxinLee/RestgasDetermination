/*
 * PndLmdDataFacade.h
 *
 *  Created on: Aug 26, 2013
 *      Author: steve
 */

#ifndef PNDLMDDATAFACADE_H_
#define PNDLMDDATAFACADE_H_

#include "LumiFitStructs.h"
#include "PndLmdAbstractData.h"

#include "PndLmdSeperateDataReader.h"
#include "PndLmdCombinedDataReader.h"

#include "PndLmdComparisonStructs.h"

#include <vector>

#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations

#include "TFile.h"
#include "TKey.h"

class PndLmdAngularData;
class PndLmdVertexData;
class PndLmdAcceptance;
/**
 * Class providing a simplified UI for constructing, reading and filling lmd
 * data objects. This class should be used primarily used by the standard user.
 */
class PndLmdDataFacade {
	PndLmdDataReader *data_reader;

	std::vector<PndLmdAngularData> lmd_angular_data;
	std::vector<PndLmdVertexData> lmd_vertex_data;
	std::vector<PndLmdHistogramData> lmd_hist_data;
	std::vector<PndLmdAcceptance> lmd_acceptances;

	LumiFit::LmdDimension constructPrimaryDimension() const;
	LumiFit::LmdDimension constructSecondaryDimension() const;

	std::map<std::string, std::vector<LumiFit::LmdDimension> > selections_map;

	std::map<std::string, std::vector<LumiFit::LmdDimension> > extendSelectionsMapByDimensionBundle(
			std::map<std::string, std::vector<LumiFit::LmdDimension> > &current_selections_map,
			LumiFit::LmdDimension &selection_dimension_bundle) const;

	void addMultipleInstancesBasedOnSelections(const PndLmdAngularData & data);
	void addMultipleInstancesBasedOnSelections(const PndLmdAcceptance & data);
	void addMultipleInstancesBasedOnSelections(const PndLmdHistogramData & data);
	void addMultipleInstancesBasedOnSelections(const PndLmdVertexData & data);

	void initialize1DData(std::string name, PndLmdAbstractData &data) const;
	void initialize2DData(std::string name, PndLmdAbstractData &data) const;

public:
	double lab_momentum;

	// data dimension templates
	LumiFit::LmdDimension primary_dimension_template;
	LumiFit::LmdDimension secondary_dimension_template;

	// selection dimension bundle vector
	std::vector<LumiFit::LmdDimension> selection_dimension_bundles;

	double current_reference_luminosity_per_event;

	LumiFit::LmdSimIPParameters current_simulation_ip_parameters;

	PndLmdDataFacade();
	virtual ~PndLmdDataFacade();

	void setDataReader(PndLmdDataReader* data_reader_);

	std::vector<PndLmdAcceptance> getLmdAcceptances() const;
	std::vector<PndLmdAngularData> getLmdAngularData() const;
	std::vector<PndLmdVertexData> getLmdVertexData() const;
	std::vector<PndLmdHistogramData> getLmdHistogramData() const;

	void addDataDirectory(TString directory);
	void addFileList(std::string filelist);

	void createAcceptance1D(std::string name, unsigned int num_events);
	void createAcceptance2D(std::string name, unsigned int num_events);

	void createData1D(std::string name, unsigned int num_events);
	void createData2D(std::string name, unsigned int num_events);

	void createVertexData1D(std::string name, unsigned int num_events);
	void createVertexData2D(std::string name, unsigned int num_events);

	void createHistogramData1D(std::string name, unsigned int num_events);
	void createHistogramData2D(std::string name, unsigned int num_events);

	void create1DVertexDataBundle(unsigned int num_events);
	void create1DAngularDataBundle(unsigned int num_events);
	void create2DAngularDataBundle(unsigned int num_events);
	void create1DAngularResolutionDataBundle(unsigned int num_events);
	void create2DAngularResolutionDataBundle(unsigned int num_events);

	void createSelectionDimensionCombinations();
	void clearSelectionDimensionMap();

	void fillAll();

	std::vector<std::string> findFilesByName(
			const boost::filesystem::path &top_dir_path_to_search,
			const std::string dir_name_filter, const std::string file_name);

	template<class T> std::vector<T> getDataFromFile(TFile *f) {
		std::vector<T> lmd_data_vec;

		unsigned int counter = 0;

		if (0 != f) {
			TIter next(f->GetListOfKeys());
			TKey *key;
			while ((key = (TKey*) next())) {

				T* data;
				f->GetObject(key->GetName(), data);
				if (data) {
					counter++;
					lmd_data_vec.push_back(*data);
				}
			}
		}
		std::cout << "Found " << counter << " objects!" << std::endl;
		return lmd_data_vec;
	}

	template<class T> std::vector<T> filterData(
			std::vector<T> all_data, LumiFit::Comparisons::AbstractLmdDataFilter &filter) {
		std::vector<T> lmd_data_vec;

		for (unsigned int i = 0; i < all_data.size(); i++) {
			PndLmdAbstractData *lmd_abs_data = (PndLmdAbstractData*) &all_data[i];
			if (lmd_abs_data != 0) {
				if (filter.check(*lmd_abs_data))
					lmd_data_vec.push_back(all_data[i]);
			}
		}

		return lmd_data_vec;
	}

	LumiFit::LmdSimIPParameters readSimulationIPParameters(std::string dir_path);

	std::map<LumiFit::LmdSimIPParameters,
			std::map<LumiFit::LmdSimIPParameters,
					std::map<LumiFit::LmdDimensionType, std::vector<PndLmdVertexData> > > > clusterVertexData(
			std::vector<PndLmdVertexData> &lmd_vertex_vec);

};

#endif /* PNDLMDDATAFACADE_H_ */
