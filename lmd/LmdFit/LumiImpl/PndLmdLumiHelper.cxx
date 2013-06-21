/*
 * PndLmdLumiHelper.cxx
 *
 *  Created on: Jun 27, 2012
 *      Author: steve
 */

#include "PndLmdLumiHelper.h"
#include "PndLmdData.h"
#include "PndLmdAcceptance.h"
#include "PndLmdResolution.h"
#include "PndLmdLumiFitOptions.h"
#include "ROOTMinimizer.h"
#include "PndLmdModelFactory.h"
#include "PndLmdDPMAngModel1D.h"
#include "PndLmdDPMModelParametrization.h"
#include "PndLmdLumiFitResult.h"
#include "ROOTDataHelper.h"
#include "Data.h"

#include "ModelStructs.h"

#include <iostream>
#include <sstream>

#include "boost/progress.hpp"

#include "TFile.h"
#include "TMath.h"
#include "TRandom.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGraphErrors.h"
#include "TCanvas.h"

#include "FairTrackParH.h"
#include "PndMCTrack.h"

using std::tr1::shared_ptr;

PndLmdLumiHelper::PndLmdLumiHelper() {
	pdg = TDatabasePDG::Instance();
}

double PndLmdLumiHelper::getMomentumTransferFromTheta(double plab,
		double theta) {
	PndLmdDPMAngModel1D model("dpm_angular_1d");
	shared_ptr<Parametrization> para(
			new PndLmdDPMModelParametrization(model.getModelParameterSet()));
	model.getModelParameterHandler().registerParametrizations(
			model.getModelParameterSet(), para);
	model.getModelParameterSet().setModelParameterValue("p_lab", plab);
	((Model1D*) &model)->init();
	return -model.getMomentumTransferFromTheta(theta);
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
int PndLmdLumiHelper::registerResolution(PndLmdResolution* res) {
	registered_resolutions.push_back(res);
	return 0;
}
int PndLmdLumiHelper::registerResolutions(
		std::vector<PndLmdResolution*> &res_vec) {
	for (unsigned int i = 0; i < res_vec.size(); i++) {
		registered_resolutions.push_back(res_vec[i]);
	}
	return 0;
}

void PndLmdLumiHelper::clearRegisters(data_type data_mode) {
	if (DATA == data_mode)
		registered_data.clear();
	else if (ACCEPTANCE == data_mode)
		registered_acceptances.clear();
	else
		registered_resolutions.clear();
}

void PndLmdLumiHelper::removeFinished(
		std::vector<PndLmdDataInterface*> *lmd_vec, int current_event_index) {
	std::vector<PndLmdDataInterface*>::iterator it = lmd_vec->begin();
	while (it != lmd_vec->end()) {
		if (current_event_index == (*it)->getNumEvents()) {
			lmd_vec->erase(it);
			it--;
		}
		it++;
	}
}

int PndLmdLumiHelper::getNextMinEventIndex(
		std::vector<PndLmdDataInterface*> *lmd_vec) {
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

void PndLmdLumiHelper::fillData(double plab, TString dir_path,
		data_type data_mode, acceptance_type acc_mode, TString MC_file,
		TString geane_file, TString track_file) {
	std::vector<PndLmdDataInterface*> *lmd_vec;
	if (ACCEPTANCE == data_mode)
		lmd_vec = (std::vector<PndLmdDataInterface*>*) &registered_acceptances;
	else if (DATA == data_mode)
		lmd_vec = (std::vector<PndLmdDataInterface*>*) &registered_data;
	else
		lmd_vec = (std::vector<PndLmdDataInterface*>*) &registered_resolutions;

	if (0 == lmd_vec->size()) {
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

	TLorentzVector beam(0, 0, plab,
			sqrt(pow(plab, 2.0) + pow(pdg->GetParticle(-2212)->Mass(), 2.0)));

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
		if (acc_mode == FULL) {
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
		else if (acc_mode == GEOMETRY_ONLY) {
			// in this case we just check for more than 2 mc hits
			if (true_points->GetEntriesFast() > 2)
				event_accepted = true;
		} else if (acc_mode == NO_BACKTRACKING) {
			// this is for debugging only
			// in this case we just check for tracks at the lumi monitor instead of as interaction point
			tTrack.GetEntry(j);
			event_accepted = reco_tracks->GetEntriesFast();
		}

		// number of MC tracks
		const int nMCParticles = true_tracks->GetEntriesFast();
		const int nRecoParticles = geaneArray->GetEntriesFast();

		std::vector<std::pair<PndLmdFit::lmd_values, PndLmdFit::lmd_values> > event_data;
		PndLmdFit::lmd_values mc_values = { 0.0, 0.0, 0.0, 0 };
		PndLmdFit::lmd_values reco_values = { 0.0, 0.0, 0.0, 0 };

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
				lv_rec.SetE(
						sqrt(MomRec.Mag2() + pow(pdg->GetParticle(-2212)->Mass(), 2.0)));

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
					event_data.push_back(std::make_pair(mc_values, reco_values));
				}
			}
		}

		for (unsigned int k = 0; k < lmd_vec->size(); k++) {
			(*lmd_vec)[k]->fillHistograms(event_data);
		}
		//-------------------------------------------------------------------------

		++show_progress;
	}

	if (ACCEPTANCE == data_mode) {
		for (unsigned int k = 0; k < registered_acceptances.size(); k++) {
			registered_acceptances[k]->makeFitterHists();
		}
	}

	delete (true_tracks);
	delete (true_points);
	delete (geaneArray);

	clearRegisters(data_mode);
}

// create estimators from root objects (hits, graphs...)

void PndLmdLumiHelper::fillFitData2D(TH2D* hist_2d,
		std::pair<double, double> &fit_range_x
		, std::pair<double, double> &fit_range_y) {

}

std::vector<PndLmdResolution*> PndLmdLumiHelper::getFittedResolutionsFromPath(
		TFile* f) {
	std::vector<PndLmdResolution*> resolutions;

	TIter phidir_next(f->GetListOfKeys());
	TDirectory *phidir;
	while ((phidir = (TDirectory*) phidir_next())) {
		std::cout << "Found dir " << phidir->GetName() << std::endl;
		std::string phiname(phidir->GetName());
		gDirectory->cd(phidir->GetName());
		TIter thetadir_next(gDirectory->GetListOfKeys());
		TDirectory *thetadir;
		while ((thetadir = (TDirectory*) thetadir_next())) {
			std::cout << "Found dir " << thetadir->GetName() << std::endl;
			PndLmdResolution* resolution;
			f->GetObject(
					TString(phiname) + "/" + TString(thetadir->GetName())
							+ "/lmdresolution", resolution);
			if (resolution) {
				std::cout << "adding resolution from " << thetadir->GetName()
						<< std::endl;
				resolutions.push_back(resolution);
			}
		}
	}
	return resolutions;
}

std::vector<PndLmdLumiHelper::lmd_graph> PndLmdLumiHelper::getResolutionModelResultsFromFile(
		TFile* f) {
	std::vector<PndLmdLumiHelper::lmd_graph> return_vec;

	f->cd();
	TIter next(f->GetListOfKeys());
	PndLmdLumiHelper::lmd_graph* ptemp_lmd_graph;
	while ((ptemp_lmd_graph = (PndLmdLumiHelper::lmd_graph*) next())) {
		if (ptemp_lmd_graph) {
			std::cout << "Found graph " << ptemp_lmd_graph->GetName() << std::endl;
			return_vec.push_back(*ptemp_lmd_graph);
		}
	}

	if (return_vec.size() == 0) {
		std::cout
				<< "WARNING: The given file does not contain any objects of the required type!"
				<< std::endl;
	}
	return return_vec;
}

void PndLmdLumiHelper::saveLmdGraphsToFile(
		std::vector<PndLmdLumiHelper::lmd_graph>& graph_vec) {
	for (unsigned int i = 0; i < graph_vec.size(); i++) {
		TString name(graph_vec[i].dependency);
		std::ostringstream strstream;
		strstream.precision(3);
		for (std::map<unsigned int, std::pair<std::string, std::string> >::const_iterator j =
				graph_vec[i].parameter_name_stack.begin();
				j != graph_vec[i].parameter_name_stack.end(); j++) {
			strstream << "_" << j->second.first << "-" << j->second.second;
		}
		for (std::map<std::string, double, ModelStructs::string_comp>::const_iterator dependency =
				graph_vec[i].remaining_dependencies.begin();
				dependency != graph_vec[i].remaining_dependencies.end(); dependency++) {
			strstream << "_" << dependency->first << "-" << dependency->second;
		}
		name = name + strstream.str();
		graph_vec[i].Write(name);
	}
}

void PndLmdLumiHelper::finalizeLmdGraphObjects(
		std::map<
				double,
				std::map<std::pair<std::string, std::string>
						, PndLmdLumiHelper::lmd_graph , ModelStructs::stringpair_comp> > &graph_map
		, int mode) {

	bool is_dependent = (graph_map.size() > 0);
	for (std::map<
			double,
			std::map<std::pair<std::string, std::string>, PndLmdLumiHelper::lmd_graph
					, ModelStructs::stringpair_comp> >::iterator graphs =
			graph_map.begin(); graphs != graph_map.end(); graphs++) {
		for (std::map<std::pair<std::string, std::string>
				, PndLmdLumiHelper::lmd_graph , ModelStructs::stringpair_comp>::iterator graph =
				graphs->second.begin(); graph != graphs->second.end(); graph++) {
			if (is_dependent) {
				if (0 == mode) {
					graph->second.remaining_dependencies["phi"] = graphs->first;
					graph->second.dependency = "theta";
				} else if (1 == mode) {
					graph->second.remaining_dependencies["theta"] = graphs->first;
					graph->second.dependency = "phi";
				}
			}

			graph->second.graph = new TGraphErrors(graph->second.data.size());
			for (unsigned int i = 0; i < graph->second.data.size(); i++) {
				graph->second.graph->SetPoint(i, graph->second.data[i].first,
						graph->second.data[i].second.value);
				graph->second.graph->SetPointError(i, 0.0,
						graph->second.data[i].second.error);
			}
		}
	}
}

std::vector<PndLmdLumiHelper::lmd_graph> PndLmdLumiHelper::generateLmdGraphs(
		std::vector<PndLmdResolution*> resolutions) {

	std::map<
			double,
			std::map<std::pair<std::string, std::string>, PndLmdLumiHelper::lmd_graph
					, ModelStructs::stringpair_comp> > phi_slice_map;
	std::map<
			double,
			std::map<std::pair<std::string, std::string>, PndLmdLumiHelper::lmd_graph
					, ModelStructs::stringpair_comp> > theta_slice_map;

	for (unsigned int i = 0; i < resolutions.size(); i++) {
		std::set<PndLmdLumiFitResult*> fit_results =
				resolutions[i]->getFitResults();
		for (std::set<PndLmdLumiFitResult*>::const_iterator fit_result =
				fit_results.begin(); fit_result != fit_results.end(); fit_result++) {
			const std::set<ModelStructs::minimization_parameter> &fit_parameters =
					(*fit_result)->getModelFitResult()->getFitParameters();
			for (std::set<ModelStructs::minimization_parameter>::const_iterator fit_param =
					fit_parameters.begin(); fit_param != fit_parameters.end();
					fit_param++) {
				phi_slice_map[resolutions[i]->getPhiSliceMean()][fit_param->name].data.push_back(
						std::make_pair(resolutions[i]->getThetaSliceMean(), *fit_param));
				phi_slice_map[resolutions[i]->getPhiSliceMean()][fit_param->name].parameter_name_stack[1] =
						fit_param->name;
				phi_slice_map[resolutions[i]->getPhiSliceMean()][fit_param->name].fit_options =
						(*fit_result)->getLumiFitOptions();

				theta_slice_map[resolutions[i]->getThetaSliceMean()][fit_param->name].data.push_back(
						std::make_pair(resolutions[i]->getPhiSliceMean(), *fit_param));
				theta_slice_map[resolutions[i]->getThetaSliceMean()][fit_param->name].parameter_name_stack[1] =
						fit_param->name;
				theta_slice_map[resolutions[i]->getThetaSliceMean()][fit_param->name].fit_options =
						(*fit_result)->getLumiFitOptions();
			}
			phi_slice_map[resolutions[i]->getPhiSliceMean()][std::make_pair(
					fit_parameters.begin()->name.first, "chi2")].data.push_back(
					std::make_pair(
							resolutions[i]->getThetaSliceMean(),
							ModelStructs::minimization_parameter(
									std::make_pair(fit_parameters.begin()->name.first, "chi2"),
									(*fit_result)->getRedChiSquare(), 0.0)));
			phi_slice_map[resolutions[i]->getPhiSliceMean()][std::make_pair(
					fit_parameters.begin()->name.first, "chi2")].parameter_name_stack[1] =
					std::make_pair(fit_parameters.begin()->name.first, "chi2");
			phi_slice_map[resolutions[i]->getPhiSliceMean()][std::make_pair(
					fit_parameters.begin()->name.first, "chi2")].fit_options =
					(*fit_result)->getLumiFitOptions();
			theta_slice_map[resolutions[i]->getThetaSliceMean()][std::make_pair(
					fit_parameters.begin()->name.first, "chi2")].data.push_back(
					std::make_pair(
							resolutions[i]->getPhiSliceMean(),
							ModelStructs::minimization_parameter(
									std::make_pair(fit_parameters.begin()->name.first, "chi2"),
									(*fit_result)->getRedChiSquare(), 0.0)));
			theta_slice_map[resolutions[i]->getThetaSliceMean()][std::make_pair(
					fit_parameters.begin()->name.first, "chi2")].parameter_name_stack[1] =
					std::make_pair(fit_parameters.begin()->name.first, "chi2");
			theta_slice_map[resolutions[i]->getThetaSliceMean()][std::make_pair(
					fit_parameters.begin()->name.first, "chi2")].fit_options =
					(*fit_result)->getLumiFitOptions();
		}
	}
// go through map and make TGraph objects
	finalizeLmdGraphObjects(phi_slice_map, 0);
	finalizeLmdGraphObjects(theta_slice_map, 1);

	//construct return vector
	std::vector<PndLmdLumiHelper::lmd_graph> return_vec;
	for (std::map<
			double,
			std::map<std::pair<std::string, std::string>, PndLmdLumiHelper::lmd_graph
					, ModelStructs::stringpair_comp> >::iterator graphs =
			phi_slice_map.begin(); graphs != phi_slice_map.end(); graphs++) {
		for (std::map<std::pair<std::string, std::string>
				, PndLmdLumiHelper::lmd_graph , ModelStructs::stringpair_comp>::iterator graph =
				graphs->second.begin(); graph != graphs->second.end(); graph++) {
			return_vec.push_back(graph->second);
		}

	}
	for (std::map<
			double,
			std::map<std::pair<std::string, std::string>, PndLmdLumiHelper::lmd_graph
					, ModelStructs::stringpair_comp> >::iterator graphs =
			theta_slice_map.begin(); graphs != theta_slice_map.end(); graphs++) {
		for (std::map<std::pair<std::string, std::string>
				, PndLmdLumiHelper::lmd_graph , ModelStructs::stringpair_comp>::iterator graph =
				graphs->second.begin(); graph != graphs->second.end(); graph++) {
			return_vec.push_back(graph->second);
		}

	}
	return return_vec;
}

std::vector<PndLmdLumiHelper::lmd_graph> PndLmdLumiHelper::generateNewLmdGraphs(
		std::vector<PndLmdLumiHelper::lmd_graph> &util_lmd_graphs) {

	std::vector<PndLmdLumiHelper::lmd_graph> return_vec;

	for (unsigned int i = 0; i < util_lmd_graphs.size(); i++) {
		const std::set<ModelStructs::minimization_parameter> &fit_parameters =
				util_lmd_graphs[i].fit_result->getFitParameters();
		for (std::set<ModelStructs::minimization_parameter>::const_iterator fit_param =
				fit_parameters.begin(); fit_param != fit_parameters.end();
				fit_param++) {

			std::cout << fit_param->name.first << " " << fit_param->name.second
					<< std::endl;
		}
	}

	/*
	 phi_slice_map[resolutions[i]->getPhiSliceMean()][fit_param->name].data.push_back(
	 std::make_pair(resolutions[i]->getThetaSliceMean(), *fit_param));
	 phi_slice_map[resolutions[i]->getPhiSliceMean()][fit_param->name].parameter_name_stack[1] =
	 fit_param->name;
	 phi_slice_map[resolutions[i]->getPhiSliceMean()][fit_param->name].fit_options =
	 (*fit_result)->getLumiFitOptions();

	 theta_slice_map[resolutions[i]->getThetaSliceMean()][fit_param->name].data.push_back(
	 std::make_pair(resolutions[i]->getPhiSliceMean(), *fit_param));
	 theta_slice_map[resolutions[i]->getThetaSliceMean()][fit_param->name].parameter_name_stack[1] =
	 fit_param->name;
	 theta_slice_map[resolutions[i]->getThetaSliceMean()][fit_param->name].fit_options =
	 (*fit_result)->getLumiFitOptions();
	 }
	 phi_slice_map[resolutions[i]->getPhiSliceMean()][std::make_pair(
	 fit_parameters.begin()->name.first, "chi2")].data.push_back(
	 std::make_pair(
	 resolutions[i]->getThetaSliceMean(),
	 ModelStructs::minimization_parameter(
	 std::make_pair(fit_parameters.begin()->name.first, "chi2"),
	 (*fit_result)->getRedChiSquare(), 0.0)));
	 phi_slice_map[resolutions[i]->getPhiSliceMean()][std::make_pair(
	 fit_parameters.begin()->name.first, "chi2")].parameter_name_stack[1] =
	 std::make_pair(fit_parameters.begin()->name.first, "chi2");
	 phi_slice_map[resolutions[i]->getPhiSliceMean()][std::make_pair(
	 fit_parameters.begin()->name.first, "chi2")].fit_options =
	 (*fit_result)->getLumiFitOptions();
	 theta_slice_map[resolutions[i]->getThetaSliceMean()][std::make_pair(
	 fit_parameters.begin()->name.first, "chi2")].data.push_back(
	 std::make_pair(
	 resolutions[i]->getPhiSliceMean(),
	 ModelStructs::minimization_parameter(
	 std::make_pair(fit_parameters.begin()->name.first, "chi2"),
	 (*fit_result)->getRedChiSquare(), 0.0)));
	 theta_slice_map[resolutions[i]->getThetaSliceMean()][std::make_pair(
	 fit_parameters.begin()->name.first, "chi2")].parameter_name_stack[1] =
	 std::make_pair(fit_parameters.begin()->name.first, "chi2");
	 theta_slice_map[resolutions[i]->getThetaSliceMean()][std::make_pair(
	 fit_parameters.begin()->name.first, "chi2")].fit_options =
	 (*fit_result)->getLumiFitOptions();
	 }
	 }
	 // go through map and make TGraph objects
	 finalizeLmdGraphObjects(phi_slice_map, 0);
	 finalizeLmdGraphObjects(theta_slice_map, 1);

	 //construct return vector
	 std::vector<PndLmdLumiHelper::lmd_graph> return_vec;
	 for (std::map<
	 double,
	 std::map<std::pair<std::string, std::string>, PndLmdLumiHelper::lmd_graph
	 , ModelStructs::stringpair_comp> >::iterator graphs =
	 phi_slice_map.begin(); graphs != phi_slice_map.end(); graphs++) {
	 for (std::map<std::pair<std::string, std::string>
	 , PndLmdLumiHelper::lmd_graph , ModelStructs::stringpair_comp>::iterator graph =
	 graphs->second.begin(); graph != graphs->second.end(); graph++) {
	 return_vec.push_back(graph->second);
	 }

	 }*/

	return return_vec;
}

void PndLmdLumiHelper::fitParametrizationModelToData(
		PndLmdLumiHelper::lmd_graph &graph) {
	PndLmdModelFactory model_factory; // construct model factory
	// specify which of type of smearing model we want to generate

	// generate the model
	shared_ptr<Model> model = model_factory.generate1DResolutionModel(
			graph.fit_options);

	// get the model that we have to fit to the data
	for (std::map<unsigned int, std::pair<std::string, std::string> >::const_iterator parameter_name =
			graph.parameter_name_stack.begin();
			parameter_name != graph.parameter_name_stack.end(); parameter_name++) {
		ParametrizationProxy par_proxy =
				model->getModelParameterHandler().getParametrizationProxyForModelParameter(
						parameter_name->second.second);
		if (par_proxy.hasParametrizationModel()) {
			model = par_proxy.getParametrizationModel()->getModel();
		} else {
			std::cout
					<< "ERROR: Not able to obtain parametrization model for parameter "
					<< parameter_name->second.second << "!" << std::endl;
			return;
		}
	}
	model->getModelParameterSet().getModelParameter("poly_poly_factor_0")->setValue(
			graph.graph->GetMean(2));
	model->getModelParameterSet().getModelParameter("poly_poly_factor_0")->setParameterFixed(
			false);
	model->getModelParameterSet().getModelParameter("poly_poly_factor_1")->setValue(
			1.0);
	model->getModelParameterSet().getModelParameter("poly_poly_factor_1")->setParameterFixed(
			false);

	model->getModelParameterSet().printInfo();

	// create chi2 estimator
	Chi2Estimator chi2_est;

	// set the resolution model as the active model of the fitter
	chi2_est.setModel(model);

	// create and set data
	ROOTDataHelper data_helper;
	chi2_est.setData(data_helper.createBinnedData(graph.graph));

	// create estimator options
	EstimatorOptions est_opt;
	std::pair<double, double> fit_range = std::make_pair(3.0, 7.0);
	est_opt.setFitRangeX(fit_range);
	est_opt.setWithIntegralScaling(true);

	// apply estimator options
	chi2_est.applyEstimatorOptions(est_opt);

	// now fit the resolution model to the data
	// create Minuit Fitter
	ROOTMinimizer fitter(chi2_est);

	int fit_status = fitter.doMinimization();
	std::cout << "fit status: " << fit_status << std::endl;
	// call minimization procedure
	if (fit_status) {
		std::cout << "ERROR: Problem while performing fit. Returning NULL pointer!"
				<< std::endl;
		return;
	}

	// store fit results
	ModelFitResult fit_result = fitter.createModelFitResult();
	fit_result.setFitStatus(fit_status);

	fit_result.setChiSquare(chi2_est.evaluate(fitter.getROOTMinimizer()->X()));
	fit_result.setNDF(
			chi2_est.getData()->getNumberOfDataPoints()
					- fitter.getROOTMinimizer()->NFree());

	graph.fit_result = new ModelFitResult(fit_result);

}

void PndLmdLumiHelper::fitParametrizationModelToGraphs(
		std::vector<PndLmdLumiHelper::lmd_graph> &lmd_graphs) {
	// perform fits
	for (unsigned int graph_index = 0; graph_index < lmd_graphs.size();
			graph_index++) {
		// TODO: we need some kind of check at which stage we are at and which graphs should be fitted...
		// or if they exist at all...
		if (lmd_graphs[graph_index].dependency.compare("theta") == 0)
			fitParametrizationModelToData(lmd_graphs[graph_index]);
	}
}

void PndLmdLumiHelper::fitResolutionForSlice(PndLmdResolution* lmd_resolution,
		const PndLmdLumiFitOptions *fit_options) {
	PndLmdModelFactory model_factory; // construct model factory
	// specify which of type of smearing model we want to generate

	PndLmdLumiFitResult *res_fit_result;

	// generate the model
	shared_ptr<Model1D> resolution_model =
			model_factory.generate1DResolutionModel(fit_options);

	// get histogram
	TH1D* hist = lmd_resolution->getResolutionHistogram1D();
	if (hist->GetEntries() < 1500) {
		return;
	}

	// now we have to set good starting values and free parameters
	// amplitude of gauss is equal to number of events in the histogram
	resolution_model->getModelParameterSet().getModelParameter("gauss_amplitude")->setValue(
			hist->GetEntries());
	resolution_model->getModelParameterSet().getModelParameter("gauss_amplitude")->setParameterFixed(
			false);

	if (0 == fit_options->getSmearingModelType()) { // simple gaussian
		resolution_model->getModelParameterSet().getModelParameter("gauss_mean")->setValue(
				0.0);
		resolution_model->getModelParameterSet().getModelParameter("gauss_mean")->setParameterFixed(
				false);
		resolution_model->getModelParameterSet().getModelParameter("gauss_sigma")->setValue(
				hist->GetRMS());
		resolution_model->getModelParameterSet().getModelParameter("gauss_sigma")->setParameterFixed(
				false);
	} else if (1 == fit_options->getSmearingModelType()) { // double gaussian
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_mean_narrow")->setValue(0.0);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_mean_narrow")->setParameterFixed(false);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_mean_wide")->setValue(0.0);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_mean_wide")->setParameterFixed(false);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_sigma_narrow")->setValue(hist->GetRMS() * 0.65);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_sigma_narrow")->setParameterFixed(false);

		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_sigma_ratio_narrow_wide")->setValue(0.6);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_sigma_ratio_narrow_wide")->setParameterFixed(false);

		/*resolution_model->getModelParameterSet().getModelParameter(
		 "gauss_sigma_wide")->setValue(hist->GetRMS() * 1.35);
		 resolution_model->getModelParameterSet().getModelParameter(
		 "gauss_sigma_wide")->setParameterFixed(false);*/

		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_ratio_narrow_wide")->setValue(2.0);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_ratio_narrow_wide")->setParameterFixed(false);
	}

	// create chi2 estimator
	Chi2Estimator chi2_est;

	// set the resolution model as the active model of the fitter
	chi2_est.setModel(resolution_model);

	// create and set data
	ROOTDataHelper data_helper;
	chi2_est.setData(data_helper.createBinnedData(hist));

	// create estimator options
	EstimatorOptions est_opt;
	std::pair<double, double> fit_range = std::make_pair(
			fit_options->getThetaFitRangeLow(), fit_options->getThetaFitRangeHigh());
	if (fit_options->isFitRaw())
		fit_range = std::make_pair(fit_options->getTFitRangeLow(),
				fit_options->getTFitRangeHigh());

	est_opt.setFitRangeX(
			std::make_pair(hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax()));
	est_opt.setWithIntegralScaling(true);

	// apply estimator options
	chi2_est.applyEstimatorOptions(est_opt);

	// now fit the resolution model to the data
	// create Minuit Fitter
	ROOTMinimizer fitter(chi2_est);

	int fit_status = fitter.doMinimization();
	std::cout << "fit status: " << fit_status << std::endl;
	// call minimization procedure
	if (fit_status) {
		std::cout << "ERROR: Problem while performing fit. Returning NULL pointer!"
				<< std::endl;
	}

	// store fit results
	ModelFitResult fit_result = fitter.createModelFitResult();
	fit_result.setFitStatus(fit_status);

	fit_result.setChiSquare(chi2_est.evaluate(fitter.getROOTMinimizer()->X()));
	fit_result.setNDF(
			chi2_est.getData()->getNumberOfDataPoints()
					- fitter.getROOTMinimizer()->NFree());

	res_fit_result = new PndLmdLumiFitResult(fit_options);
	res_fit_result->setModelFitResult(fit_result);
}

void PndLmdLumiHelper::fitSmearingModelToResolutions(
		std::vector<PndLmdResolution*> &lmd_resolutions,
		const PndLmdLumiFitOptions *fit_options) {

	for (unsigned int index_resolution = 0;
			index_resolution < lmd_resolutions.size(); index_resolution++) {
		lmd_resolutions[index_resolution]->makeDir();
		fitResolutionForSlice(lmd_resolutions[index_resolution], fit_options);
		lmd_resolutions[index_resolution]->saveToRootFile();
	}
}

bool checkNeighbourhood(ModelStructs::minimization_parameter value,
		std::vector<ModelStructs::minimization_parameter> &neighbours) {
	double neighbour_mean = 0.0;
	for (unsigned int i = 0; i < neighbours.size(); i++) {

	}
}

std::map<double, ModelFitResult*> PndLmdLumiHelper::checkFitParameters(
		const std::map<PndLmdResolution*, ModelFitResult*> &fit_results) const {
	// remove all points that are bogus
	std::map<double, ModelFitResult*> return_map;
	// loop over all entries of the map
	for (std::map<PndLmdResolution*, ModelFitResult*>::const_iterator it =
			fit_results.begin(); it != fit_results.end(); it++) {
		const std::set<ModelStructs::minimization_parameter> &fit_parameters =
				it->second->getFitParameters();
		bool add = true;
		for (std::set<ModelStructs::minimization_parameter>::const_iterator fit_param =
				fit_parameters.begin(); fit_param != fit_parameters.end();
				fit_param++) {
			/*if (fit_param->name.find("red_chi2") < fit_param->name.size()) {
			 if (std::fabs(fit_param->value) > 1.35) {
			 add = false;
			 break;
			 }
			 }
			 if (fit_param->name.find("ratio_narrow_wide") < fit_param->name.size()) {
			 std::cout << fit_param->value << std::endl;
			 if (std::fabs(fit_param->value) < 0.0) {
			 add = false;
			 break;
			 }
			 }*/
		}
		if (add) {
			return_map.insert(
					std::make_pair(it->first->getThetaSliceMean(), it->second));
		}
	}
	return return_map;
}

