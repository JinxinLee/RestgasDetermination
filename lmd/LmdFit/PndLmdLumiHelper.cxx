/*
 * PndLmdLumiHelper.cxx
 *
 *  Created on: Jun 27, 2012
 *      Author: steve
 */

#include "PndLmdLumiHelper.h"
#include "PndLmdData.h"
#include "PndLmdAcceptance.h"
#include "PndLmdModelFactory.h"

#include <iostream>

#include "boost/progress.hpp"

#include "TFile.h"
#include "TMath.h"
#include "TRandom.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TH1D.h"
#include "TF1.h"

#include "FairTrackParH.h"
#include "PndMCTrack.h"

PndLmdLumiHelper::PndLmdLumiHelper() {
	pdg = TDatabasePDG::Instance();
}

int PndLmdLumiHelper::registerData(PndLmdData* data) {
	registered_data.push_back(data);
	return 0;
}
int PndLmdLumiHelper::registerData(std::vector<PndLmdData*> &data_vec) {
	for (unsigned int i = 0; i < data_vec.size(); i++) {
		registered_data.push_back(data_vec[i]);
	}
	return 0;
}
int PndLmdLumiHelper::registerAcceptance(PndLmdAcceptance* acc) {
	registered_acceptances.push_back(acc);
	return 0;
}
int PndLmdLumiHelper::registerAcceptances(
		std::vector<PndLmdAcceptance*> &acc_vec) {
	for (unsigned int i = 0; i < acc_vec.size(); i++) {
		registered_acceptances.push_back(acc_vec[i]);
	}
	return 0;
}

void PndLmdLumiHelper::clearRegisters(bool is_acceptance) {
	if (!is_acceptance)
		registered_data.clear();
	else
		registered_acceptances.clear();
}

void PndLmdLumiHelper::removeFinished(std::vector<PndLmdDataBase*> *lmd_vec,
		int current_event_index) {
	std::vector<PndLmdDataBase*>::iterator it = lmd_vec->begin();
	while (it != lmd_vec->end()) {
		if (current_event_index == (*it)->getNumEvents()) {
			lmd_vec->erase(it);
			it--;
		}
		it++;
	}
}

int PndLmdLumiHelper::getNextMinEventIndex(
		std::vector<PndLmdDataBase*> *lmd_vec) {
	int next_min_event_index = -1;
	for (unsigned int i = 0; i < lmd_vec->size(); i++) {
		if ((*lmd_vec)[i]->getNumEvents() < next_min_event_index)
			next_min_event_index = (*lmd_vec)[i]->getNumEvents();
		else if (next_min_event_index == -1) {
			next_min_event_index = (*lmd_vec)[i]->getNumEvents();
		}
	}
	return next_min_event_index;
}

void PndLmdLumiHelper::fillHists(double plab, TString dir_path,
		bool is_acceptance, acceptance_mode mode, TString MC_file,
		TString geane_file, TString track_file) {
	std::vector<PndLmdDataBase*> *lmd_vec;
	if (is_acceptance)
		lmd_vec = (std::vector<PndLmdDataBase*>*) &registered_acceptances;
	else
		lmd_vec = (std::vector<PndLmdDataBase*>*) &registered_data;

	if (lmd_vec->size() == 0) {
		std::cout
				<< "No data or acceptance objects were registered, hence no data can be filled. Please register objects via the register methods of this helper class"
				<< std::endl;
		return;
	}

	TString MC_file_path = dir_path + "/" + MC_file;
	TString geane_file_path = dir_path + "/" + geane_file;
	TString track_file_path = dir_path + "/" + track_file;

	TChain tMC("cbmsim");
	tMC.Add(MC_file_path);
	tMC.SetBranchStatus("*", 0);

	TChain tTrack("cbmsim");
	if (track_file != "")
		tTrack.Add(track_file_path);
	tTrack.SetBranchStatus("*", 0);

	TChain tgeane("cbmsim");
	tgeane.Add(geane_file_path);
	tgeane.SetBranchStatus("*", 0);

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

	// TTree for unbinned data histograms for roofit
	TTree *unbinned = new TTree("utree", "utree");
	double theta = 0.0;
	unbinned->Branch("th", &theta, "th/D");

	int temp_num_events = tMC.GetEntries();
	int num_events = 0;

	// get maximum event number for all registered data objects
	for (unsigned int i = 0; i < lmd_vec->size(); i++) {
		if (num_events < (*lmd_vec)[i]->getNumEvents())
			num_events = (*lmd_vec)[i]->getNumEvents();
		else if ((*lmd_vec)[i]->getNumEvents() == -1) {
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
	for (unsigned int i = 0; i < lmd_vec->size(); i++) {
		if (num_events < (*lmd_vec)[i]->getNumEvents())
			(*lmd_vec)[i]->setNumEvents(num_events);
		else if ((*lmd_vec)[i]->getNumEvents() == -1) {
			(*lmd_vec)[i]->setNumEvents(num_events);
		}
	}
	int min_num_events = getNextMinEventIndex(lmd_vec);

	std::cout << "Processing " << num_events << " events" << std::endl;

	boost::progress_display show_progress(num_events);

	TLorentzVector
	beam(0, 0, plab, sqrt(pow(plab, 2.0) + pow(pdg->GetParticle(-2212)->Mass(), 2.0)));

	// number of MC events
	const int nMCEvents = tMC.GetEntriesFast();

	for (Int_t j = 0; j < num_events; j++) {
		if (nMCEvents > 0)
			tMC.GetEntry(j);
		tgeane.GetEntry(j);

		// this check is to remove the data registered data objects for which
		// the event count has reached the specified value
		if (j == min_num_events) {
			// remove the data objects that hit their specified event count
			removeFinished(lmd_vec, j);
			// adjust the minimum number of events to the remaining data objects
			min_num_events = getNextMinEventIndex(lmd_vec);
		}

		// now specify if an event is accepted or not
		bool event_accepted = false;

		// standard case just look at geane array
		if (mode == FULL) {
			if (geaneArray->GetEntriesFast() > 1)
				continue;
			for (Int_t iN = 0; iN < geaneArray->GetEntriesFast(); iN++) {
				FairTrackParH *fRes = (FairTrackParH*) geaneArray->At(iN);
				if (fRes->GetLambda() != 0) {
					event_accepted = true;
					break;
				} else {
					std::cout
							<< "event contains tracks which failed in backtracking stage!"
							<< std::endl;
				}
			}
		}
		// ok this is actually the standard for acceptance data
		// in case material budget is negligible mc info can be used for pure geometrical acceptance
		else if (mode == GEOMETRY_ONLY) {
			// in this case we just check for more than 2 mc hits
			if (true_points->GetEntriesFast() > 2)
				event_accepted = true;
		} else if (mode == NO_BACKTRACKING) {
			// this is for debugging only
			// in this case we just check for tracks at the lumi monitor instead of as interaction point
			tTrack.GetEntry(j);
			event_accepted = reco_tracks->GetEntriesFast();
		}

		// number of MC tracks
		const int nMCParticles = true_tracks->GetEntriesFast();
		const int nRecoParticles = geaneArray->GetEntriesFast();

		std::vector<
				std::pair<PndLmdDataInterface::lmd_values,
						PndLmdDataInterface::lmd_values> > event_data;
		PndLmdDataInterface::lmd_values mc_values = { 0.0, 0.0, 0.0, 0 };
		PndLmdDataInterface::lmd_values reco_values = { 0.0, 0.0, 0.0, 0 };

		/*
		 * The acceptance is a special case because we cannot really be sure
		 * that we have a reconstructed particle in this event (which should
		 * be quite often the case just because of the geometrical acceptance).
		 * Also it could be the case that we have no MC information but a
		 * reconstructed track due to noise etc...
		 *
		 * The other case is either real/dpm data or a resolution simulation.
		 * In both cases we need at least one reconstructed track to do the work.
		 * The difficult part here is the resolution case in which we need a
		 *
		 *
		 * ok the idea is the following:
		 * loop over all mc tracks and create a new entry in the vector
		 * then loop over the geane tracks and make the correspondence
		 * to the mc tracks or just add them to the vector depending on
		 * if this is acceptance data or real data etc...
		 *
		 */
		for (int ik = 0; ik < nMCParticles; ik++) {
			PndMCTrack *mctrk = (PndMCTrack*) true_tracks->At(ik);
			TLorentzVector lv_mc = mctrk->Get4Momentum();
			Int_t mcID = mctrk->GetPdgCode();
			TVector3 MomMC_all = mctrk->GetMomentum();

			if (mcID == -2212 && mctrk->IsGeneratorCreated()) {
				mc_values.t = -(lv_mc - beam).M2();
				mc_values.theta = MomMC_all.Theta();
				mc_values.phi = MomMC_all.Phi();
				mc_values.reconstructed = true;
				reco_values.reconstructed = event_accepted;
				/* if(event_accepted) {
				 for (Int_t iN = 0; iN < geaneArray->GetEntriesFast(); iN++) {
				 FairTrackParH *fRes = (FairTrackParH*) geaneArray->At(iN);
				 TVector3 MomRec = fRes->GetMomentum();
				 if(MomMC_all.Theta() > 0.014) {
				 std::cout<<j<<std::endl;
				 std::cout<<geaneArray->GetEntriesFast()<<std::endl;
				 MomRec.Print();
				 MomMC_all.Print();
				 }
				 }
				 }*/
				event_data.push_back(std::make_pair(mc_values, reco_values));
			}
		}

		unsigned int pbar_tracks = event_data.size();
		// loop over geane tracks
		for (Int_t iN = 0; iN < nRecoParticles; iN++) {
			///-- Read info about GEANE(reconstructed) tracks--------------------------
			FairTrackParH *fRes = (FairTrackParH*) geaneArray->At(iN);
			if (fRes->GetLambda() != 0) {
				TVector3 MomRec = fRes->GetMomentum();
				TLorentzVector lv_rec;
				lv_rec.SetVect(MomRec);
				lv_rec.SetE(sqrt(MomRec.Mag2() + pow(pdg->GetParticle(-2212)->Mass(), 2.0)));

				/*
				 * ok now we should actually go through all entries of the event_data
				 * vector and find the corresponding mc track, but atm we only accept
				 * single track events so we'll just check if we have an mc entry
				 * already. if so we adjust the reco information of that entry,
				 * otherwise we'll just add all reco tracks with no mc information
				 */
				if (0 < pbar_tracks) {
					event_data[0].second.t = -(lv_rec - beam).M2();
					event_data[0].second.theta = MomRec.Theta();
					event_data[0].second.phi = MomRec.Phi();
					break; // ok we just take the first geane track this part has to be extended
				} else {
					reco_values.t = -(lv_rec - beam).M2();
					reco_values.theta = MomRec.Theta();
					reco_values.phi = MomRec.Phi();
					reco_values.reconstructed = true;
					event_data.push_back(
							std::make_pair(mc_values, reco_values));
				}
			}
		}

		for (unsigned int k = 0; k < lmd_vec->size(); k++) {
			(*lmd_vec)[k]->fillHistograms(event_data);
		}
		//-------------------------------------------------------------------------

		++show_progress;
	}

	if (is_acceptance) {
		for (unsigned int k = 0; k < registered_acceptances.size(); k++) {
			registered_acceptances[k]->makeFitterHists();
		}
	}

	delete (true_tracks);
	delete (true_points);
	delete (geaneArray);

	clearRegisters(is_acceptance);
}

std::vector<std::pair<PndLmdData*, PndLmdAcceptance*> > PndLmdLumiHelper::generatePhiSlicedDataSet(
		TFile *f, double mom, int th_bins_, int phi_bins_per_slice_,
		unsigned int num_phi_slices_, double data_th_range_low_,
		double data_th_range_high_, double acc_th_range_low_,
		double acc_th_range_high_) {

	std::vector<std::pair<PndLmdData*, PndLmdAcceptance*> > dataset;

	double slicewidth = 2 * TMath::Pi() / num_phi_slices_;
	for (unsigned int i = 0; i < num_phi_slices_; i++) {
		PndLmdData *data = new PndLmdData(f, mom, th_bins_, phi_bins_per_slice_,
				data_th_range_low_, data_th_range_high_,
				-TMath::Pi() + 1.0 * i * slicewidth,
				-TMath::Pi() + 1.0 * (i + 1) * slicewidth);

		PndLmdAcceptance *acc = new PndLmdAcceptance(f, mom, i * 10, 50,
				acc_th_range_low_, acc_th_range_high_,
				-TMath::Pi() + 1.0 * i * slicewidth,
				-TMath::Pi() + 1.0 * (i + 1) * slicewidth, 0);

		registerData(data);
		registerAcceptance(acc);
		dataset.push_back(std::make_pair(data, acc));
	}

	return dataset;
}
