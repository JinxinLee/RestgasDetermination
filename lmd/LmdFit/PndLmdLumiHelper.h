/*
 * PndLmdLumiFitBase.h
 *
 *  Created on: Jun 27, 2012
 *      Author: steve
 */

#ifndef PNDLMDLUMIHELPER_H_
#define PNDLMDLUMIHELPER_H_

#include <vector>

#include "TString.h"

class PndLmdAcceptance;
class PndLmdData;
class PndLmdDataBase;

class TFile;
class TVector3;
class TH1D;

/**
 * \brief This class contains a few functions helping the user with some basics.
 *
 * Main use of this class will be the actual interface to the data created by the lmd macros for filling #PndLmdDataBase objects.
 */
class PndLmdLumiHelper {
  /*TODO:
    - save the histograms read in from the panda data simulation in a separate root file so
      that we don't have to read in the data every time we change only the fit type...
    - inform: how to make the things faster... threads via std::threads or boost::threads
      and then really how to speed up things on a cluster via openmpi??*/
private:
	std::vector<PndLmdData*> registered_data;
	std::vector<PndLmdAcceptance*> registered_acceptances;

	void clearRegisters(bool is_acceptance);

	void removeFinished(std::vector<PndLmdDataBase*> *lmd_vec, int num_events);
	int getNextMinEventIndex(std::vector<PndLmdDataBase*> *lmd_vec);

public:
    enum acceptance_mode {
        FULL, GEOMETRY_ONLY, NO_BACKTRACKING
    };

	int registerData(PndLmdData* data);
	int registerData(std::vector<PndLmdData*> &data_vec);
	int registerAcceptance(PndLmdAcceptance* acc);
	int registerAcceptances(std::vector<PndLmdAcceptance*> &acc_vec);

	void fillHists(double plab, TString dir_path, bool is_acceptance, acceptance_mode mode =
			FULL, TString MC_file = "Lumi_MC_*.root", TString geane_file =
			"Lumi_Geane_*.root", TString track_file = "Lumi_Track_*.root");

	std::vector<std::pair<PndLmdData*, PndLmdAcceptance*> > generatePhiSlicedDataSet(
			TFile *f, double mom, int th_bins_, int phi_bins_per_slice_,
			unsigned int num_phi_slices_, double data_th_range_low_,
			double data_th_range_high_, double acc_th_range_low_,
			double acc_th_range_high_);
};

#endif /* PNDLMDLUMIHELPER_H_ */
