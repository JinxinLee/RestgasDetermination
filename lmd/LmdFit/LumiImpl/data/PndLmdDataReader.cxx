/*
 * PndLmdDataReader.cxx
 *
 *  Created on: Aug 24, 2013
 *      Author: steve
 */

#include "PndLmdDataReader.h"
#include "PndLmdAbstractData.h"
#include "PndLmdHistogramData.h"
#include "PndLmdVertexData.h"
#include "PndLmdAngularData.h"
#include "PndLmdAcceptance.h"
#include "PndLmdResolution.h"

#include "boost/progress.hpp"

#include "FairTrackParH.h"
#include "FairTrackParP.h"
#include "PndMCTrack.h"
#include "PndSdsMCPoint.h"
#include "PndTrack.h"
#include "PndLmdTrackQ.h"
#include "TClonesArray.h"

#include "TDatabasePDG.h"
#include "TMath.h"
#include "TChain.h"
#include "TClonesArray.h"

PndLmdDataReader::PndLmdDataReader() :
		beam(0.0, 0.0, 0.0, 0.0) {
	pdg = TDatabasePDG::Instance();
}

PndLmdDataReader::~PndLmdDataReader() {
}

int PndLmdDataReader::registerData(PndLmdHistogramData* data) {
	registered_data.push_back(data);
	return 0;
}

int PndLmdDataReader::registerData(std::vector<PndLmdVertexData> &data_vec) {
	for (unsigned int i = 0; i < data_vec.size(); i++) {
		registered_data.push_back(&data_vec[i]);
	}
	return 0;
}

int PndLmdDataReader::registerData(std::vector<PndLmdAngularData> &data_vec) {
	for (unsigned int i = 0; i < data_vec.size(); i++) {
		registered_data.push_back(&data_vec[i]);
	}
	return 0;
}

int PndLmdDataReader::registerAcceptance(PndLmdAcceptance* acc) {
	registered_acceptances.push_back(acc);
	return 0;
}

int PndLmdDataReader::registerAcceptances(
		std::vector<PndLmdAcceptance> &acc_vec) {
	for (unsigned int i = 0; i < acc_vec.size(); i++) {
		registered_acceptances.push_back(&acc_vec[i]);
	}
	return 0;
}

int PndLmdDataReader::registerResolution(PndLmdResolution* res) {
	registered_resolutions.push_back(res);
	return 0;
}

int PndLmdDataReader::registerResolutions(
		std::vector<PndLmdResolution> &res_vec) {
	for (unsigned int i = 0; i < res_vec.size(); i++) {
		registered_resolutions.push_back(&res_vec[i]);
	}
	return 0;
}

void PndLmdDataReader::clearRegisters() {
	registered_data.clear();
	registered_acceptances.clear();
	registered_resolutions.clear();
}

void PndLmdDataReader::removeFinished(
		std::vector<PndLmdAbstractData*> &lmd_vec, int current_event_index) {
	std::vector<PndLmdAbstractData*>::iterator it = lmd_vec.begin();
	while (it != lmd_vec.end()) {
		if (current_event_index == (*it)->getNumEvents()) {
			(*it)->setNumEvents(current_event_index);
			lmd_vec.erase(it);
			it--;
		}
		it++;
	}
}

int PndLmdDataReader::getNextMinEventIndex(
		std::vector<PndLmdAbstractData*> &lmd_vec) {
	int next_min_event_index = -1;
	for (unsigned int i = 0; i < lmd_vec.size(); i++) {
		if (lmd_vec[i]->getNumEvents() < next_min_event_index)
			next_min_event_index = lmd_vec[i]->getNumEvents();
		else if (next_min_event_index == -1) {
			next_min_event_index = lmd_vec[i]->getNumEvents();
		}
	}
	return next_min_event_index;
}

void PndLmdDataReader::addFilePath(TString file_path) {
	file_paths.push_back(file_path);
}

std::vector<PndLmdAbstractData*> PndLmdDataReader::combineAllRegisteredDataObjects() {
	std::vector<PndLmdAbstractData*> lmd_vec;
	for (unsigned int i = 0; i < registered_data.size(); i++) {
		lmd_vec.push_back(registered_data[i]);
	}
	for (unsigned int i = 0; i < registered_acceptances.size(); i++) {
		lmd_vec.push_back(registered_acceptances[i]);
	}
	for (unsigned int i = 0; i < registered_resolutions.size(); i++) {
		lmd_vec.push_back(registered_resolutions[i]);
	}
	return lmd_vec;
}

void PndLmdDataReader::cleanup() {
	clearDataStream();
	clearRegisters();
}

void PndLmdDataReader::setBeam(double lab_momentum) {
	beam = TLorentzVector(0, 0, lab_momentum,
			sqrt(pow(lab_momentum, 2.0) + pow(pdg->GetParticle(-2212)->Mass(), 2.0)));
}

void PndLmdDataReader::read() {
	std::vector<PndLmdAbstractData*> lmd_vec = combineAllRegisteredDataObjects();

	if (0 == lmd_vec.size()) {
		std::cout
				<< "No data or acceptance objects were registered, hence no data can be filled."
						" Please register objects via the register methods of this helper class"
				<< std::endl;
		return;
	}

	initDataStream();

	int temp_num_events = getEntries();
	int num_events = 0;

	// all data objects initialized with -1 as number of events
	// should be filled with all found data -> adjust that now
	for (unsigned int i = 0; i < lmd_vec.size(); i++) {
		if (0 == lmd_vec[i]->getNumEvents()) {
			lmd_vec[i]->setNumEvents(temp_num_events);
		}
	}

	// get maximum event number for all registered data objects
	for (unsigned int i = 0; i < lmd_vec.size(); i++) {
		if (num_events < lmd_vec[i]->getNumEvents())
			num_events = lmd_vec[i]->getNumEvents();
		else if (lmd_vec[i]->getNumEvents() == -1) {
			num_events = temp_num_events;
			break;
		}
		if (num_events > temp_num_events) {
			num_events = temp_num_events;
			break;
		}
	}
	// now run over all registered data objects once again and adjust
	// the number of events accordingly
	for (unsigned int i = 0; i < lmd_vec.size(); i++) {
		if (num_events < lmd_vec[i]->getNumEvents())
			lmd_vec[i]->setNumEvents(num_events);
		else if (lmd_vec[i]->getNumEvents() == -1) {
			lmd_vec[i]->setNumEvents(num_events);
		}
	}
	int min_num_events = getNextMinEventIndex(lmd_vec);

	std::cout << "Processing " << num_events << " events" << std::endl;

	boost::progress_display show_progress(num_events);

	for (Int_t j = 0; j < num_events; j++) {
		// this check is to remove the data registered data objects for which
		// the event count has reached the specified value
		if (j == min_num_events) {
			// remove the data objects that hit their specified event count
			removeFinished(lmd_vec, j);
			// adjust the minimum number of events to the remaining data objects
			min_num_events = getNextMinEventIndex(lmd_vec);
		}

		TClonesArray* tracks = getEntry(j);
		for (unsigned int track_index = 0; track_index < tracks->GetEntries();
				track_index++) {
			fillData((PndLmdTrackQ*) tracks->At(track_index));
		}

		++show_progress;
	}

	cleanup();
}

void PndLmdDataReader::fillData(PndLmdTrackQ *track_pars) {
	bool reconstructed(false);
	if (0 == track_pars->GetTrkRecStatus()) {
		reconstructed = true;
	}

	for (unsigned int i = 0; i < registered_data.size(); i++) {
		if (skipDataObject(registered_data[i], reconstructed)) {
			continue;
		}
		if (registered_data[i]->getSecondaryDimension().is_active) {
			registered_data[i]->addData(
					getTrackParameterValue(track_pars,
							registered_data[i]->getPrimaryDimension()),
					getTrackParameterValue(track_pars,
							registered_data[i]->getSecondaryDimension()));
		} else {
			registered_data[i]->addData(
					getTrackParameterValue(track_pars,
							registered_data[i]->getPrimaryDimension()));
		}

	}
	for (unsigned int i = 0; i < registered_resolutions.size(); i++) {
		if (skipDataObject(registered_resolutions[i], reconstructed)) {
			continue;
		}
		if (!registered_resolutions[i]->getPrimarySelectionDimension().dimension_range.isDataWithinRange(
				getTrackParameterValue(track_pars,
						registered_resolutions[i]->getPrimarySelectionDimension()))) {
			continue;
		}
		if (!registered_resolutions[i]->getSecondarySelectionDimension().dimension_range.isDataWithinRange(
				getTrackParameterValue(track_pars,
						registered_resolutions[i]->getSecondarySelectionDimension()))) {
			continue;
		}
		if (registered_resolutions[i]->getSecondaryDimension().is_active) {
			registered_resolutions[i]->addData(
					getTrackParameterDifference(track_pars,
							registered_resolutions[i]->getPrimaryDimension()),
					getTrackParameterDifference(track_pars,
							registered_resolutions[i]->getSecondaryDimension()));
		} else {
			registered_resolutions[i]->addData(
					getTrackParameterDifference(track_pars,
							registered_resolutions[i]->getPrimaryDimension()));
		}
	}
	for (unsigned int i = 0; i < registered_acceptances.size(); i++) {
		if (skipDataObject(registered_acceptances[i], reconstructed)) {
			continue;
		}
		if (registered_acceptances[i]->getSecondaryDimension().is_active) {
			registered_acceptances[i]->addData(
					reconstructed,
					getTrackParameterValue(track_pars,
							registered_acceptances[i]->getPrimaryDimension()),
					getTrackParameterValue(track_pars,
							registered_acceptances[i]->getSecondaryDimension()));
		} else {
			registered_acceptances[i]->addData(
					reconstructed,
					getTrackParameterValue(track_pars,
							registered_acceptances[i]->getPrimaryDimension()));
		}
	}
}

bool PndLmdDataReader::skipDataObject(const PndLmdAbstractData *data,
		bool reconstructed) const {
	if (!reconstructed) {
		if (LumiFit::RECO
				== data->getPrimaryDimension().dimension_options.track_type
				|| LumiFit::MC_ACC
						== data->getPrimaryDimension().dimension_options.track_type)
			return true;
		if (data->getSecondaryDimension().is_active) {
			if (LumiFit::RECO
					== data->getSecondaryDimension().dimension_options.track_type
					|| LumiFit::MC_ACC
							== data->getSecondaryDimension().dimension_options.track_type)
				return true;
		}
	}
	return false;
}

double PndLmdDataReader::getTrackParameterValue(PndLmdTrackQ *track_pars,
		const LumiFit::LmdDimension &lmd_dim) const {
	TVector3 pos(0.0, 0.0, 0.0);
	double theta(0.0);
	double phi(0.0);
	double plab(0.0);

	if (lmd_dim.dimension_options.track_type == LumiFit::MC
			|| lmd_dim.dimension_options.track_type == LumiFit::MC_ACC) {
		if (lmd_dim.dimension_options.track_param_type == LumiFit::IP) {
			track_pars->GetMCpoint(pos);
			theta = track_pars->GetMCtheta();
			phi = track_pars->GetMCphi();
			plab = track_pars->GetMCmom();
		} else if (lmd_dim.dimension_options.track_param_type == LumiFit::LMD) {
			track_pars->GetMCpointLMD(pos);
			theta = track_pars->GetMCthetaLMD();
			phi = track_pars->GetMCphiLMD();
			plab = track_pars->GetMCmomLMD();
		}
	} else if (lmd_dim.dimension_options.track_type == LumiFit::RECO) {
		if (lmd_dim.dimension_options.track_param_type == LumiFit::IP) {
			track_pars->GetIPpoint(pos);
			theta = track_pars->GetIPtheta();
			phi = track_pars->GetIPphi();
			plab = track_pars->GetIPmom();
		} else if (lmd_dim.dimension_options.track_param_type == LumiFit::LMD) {
			track_pars->GetLMDpoint(pos);
			theta = track_pars->GetLMDtheta();
			phi = track_pars->GetLMDphi();
			plab = beam.Vect().Mag();
		}
	}

	if (lmd_dim.dimension_options.dimension_type == LumiFit::X) {
		return pos.X();
	} else if (lmd_dim.dimension_options.dimension_type == LumiFit::Y) {
		return pos.Y();
	} else if (lmd_dim.dimension_options.dimension_type == LumiFit::Z) {
		return pos.Z();
	} else if (lmd_dim.dimension_options.dimension_type == LumiFit::T) {
		TVector3 mom(1, 1, 1);
		mom.SetTheta(theta);
		mom.SetPhi(phi);
		mom.SetMag(plab);
		TLorentzVector track(mom,
				sqrt(pow(plab, 2.0) + pow(pdg->GetParticle(-2212)->Mass(), 2.0)));
		return -(track - beam).M2();
	} else if (lmd_dim.dimension_options.dimension_type == LumiFit::THETA) {
		return theta;
	} else if (lmd_dim.dimension_options.dimension_type == LumiFit::PHI) {
		return phi;
	} else
		return 0.0;
}

double PndLmdDataReader::getTrackParameterDifference(PndLmdTrackQ *track_pars,
		LumiFit::LmdDimension lmd_dim) const {
	lmd_dim.dimension_options.track_type = LumiFit::MC;
	double mc_value = getTrackParameterValue(track_pars, lmd_dim);
	lmd_dim.dimension_options.track_type = LumiFit::RECO;
	double reco_value = getTrackParameterValue(track_pars, lmd_dim);
	return reco_value - mc_value; // this is actually important for the smearing that it is rec - mc
}
