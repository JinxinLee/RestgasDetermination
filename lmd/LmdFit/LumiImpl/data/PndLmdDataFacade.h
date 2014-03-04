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
#include "PndLmdData.h"
#include "PndLmdResolution.h"
#include "PndLmdAcceptance.h"
#include "PndLmdSeperateDataReader.h"
#include "PndLmdCombinedDataReader.h"

#include <vector>

#include "TFile.h"
#include "TKey.h"

/**
 * Class providing a simplified UI for constructing, reading and filling lmd
 * data objects. This class should be used primarily used by the standard user.
 */
class PndLmdDataFacade {
private:
	double lab_momentum;

	// data dimension templates
	LumiFit::LmdDimension primary_dimension_template;
	LumiFit::LmdDimension secondary_dimension_template;

	// selection dimension templates
	LumiFit::LmdDimension primary_selection_dimension_template;
	LumiFit::LmdDimension secondary_selection_dimension_template;

	// selection dimension templates for bundle generation
	LumiFit::LmdDimension primary_selection_dimension_bundle_template;
	LumiFit::LmdDimension secondary_selection_dimension_bundle_template;

	double current_reference_luminosity_per_event;

	PndLmdDataReader *data_reader;

	std::vector<PndLmdData> lmd_datas;
	std::vector<PndLmdResolution> lmd_resolutions;
	std::vector<PndLmdAcceptance> lmd_acceptances;

	LumiFit::LmdDimension constructPrimaryDimension() const;
	LumiFit::LmdDimension constructSecondaryDimension() const;

	void initialize1DData(PndLmdAbstractData &data) const;
	void initialize2DData(PndLmdAbstractData &data) const;

public:
	PndLmdDataFacade();
	virtual ~PndLmdDataFacade();

	void setDataReader(PndLmdDataReader* data_reader_);

	double getLabMomentum() const;
	void setLabMomentum(double lab_momentum_);

	LumiFit::LmdDimension& getPrimaryDimensionTemplate();
	LumiFit::LmdDimension& getSecondaryDimensionTemplate();

	LumiFit::LmdDimension& getPrimarySelectionDimensionTemplate();
	LumiFit::LmdDimension& getSecondarySelectionDimensionTemplate();

	LumiFit::LmdDimension& getPrimarySelectionDimensionBundleTemplate();
	LumiFit::LmdDimension& getSecondarySelectionDimensionBundleTemplate();

	std::vector<PndLmdAcceptance> getLmdAcceptances() const;
	std::vector<PndLmdData> getLmdDatas() const;
	std::vector<PndLmdResolution> getLmdResolutions() const;

	void addDataDirectory(TString directory);

	void createAcceptance1D(unsigned int num_events);
	void createAcceptance2D(unsigned int num_events);

	void createData1D(unsigned int num_events);
	void createData2D(unsigned int num_events);

	void createResolution1D(unsigned int num_events);
	void createResolution2D(unsigned int num_events);

	void create1DVertexDataBundle(unsigned int num_events);
	void create1DAngularDataBundle(unsigned int num_events);
	void create1DAngularResolutionDataBundle(unsigned int num_events);

	void fillAll();

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

	template<class T> std::vector<T> filterData(std::vector<T> all_data, LumiFit::LmdDimensionOptions &lmd_dim_opt) {
		std::vector<T> lmd_data_vec;

		for(unsigned int i = 0; i < all_data.size(); i++) {
			PndLmdAbstractData* lmd_abs_data = (PndLmdAbstractData*)&all_data[i];
			if(lmd_abs_data != 0) {
				if(lmd_abs_data->getPrimaryDimension().dimension_options == lmd_dim_opt)
					lmd_data_vec.push_back(all_data[i]);
			}
		}

		return lmd_data_vec;
	}

	double getCurrentReferenceLuminosityPerEvent() const;
	void setCurrentReferenceLuminosityPerEvent(
			double current_reference_luminosity_per_event_);

	LumiFit::LmdSimIPParameters readSimulationIPParameters(std::string dir_path);

};

#endif /* PNDLMDDATAFACADE_H_ */
