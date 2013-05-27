/*
 * PndLmdLumiFitBase.h
 *
 *  Created on: Jun 27, 2012
 *      Author: steve
 */

#ifndef PNDLMDLUMIHELPER_H_
#define PNDLMDLUMIHELPER_H_

#ifndef __CINT__
	#include "ModelFitResult.h"
#endif /* __CINT __ */


#include <vector>

#include "TString.h"
#include "TDatabasePDG.h"

class PndLmdAcceptance;
class PndLmdData;
class PndLmdDataInterface;
class PndLmdResolution;
class PndLmdLumiFitOptions;

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
public:
	enum acceptance_type {
		FULL, GEOMETRY_ONLY, NO_BACKTRACKING
	};
	enum data_type {
		DATA, ACCEPTANCE, RESOLUTION
	};
private:
	TDatabasePDG *pdg;
	std::vector<PndLmdData*> registered_data;
	std::vector<PndLmdAcceptance*> registered_acceptances;
	std::vector<PndLmdResolution*> registered_resolutions;

	void clearRegisters(data_type data_mode);

	void removeFinished(std::vector<PndLmdDataInterface*> *lmd_vec,
			int num_events);
	int getNextMinEventIndex(std::vector<PndLmdDataInterface*> *lmd_vec);

public:
	PndLmdLumiHelper();

	static double getMomentumTransferFromTheta(double plab, double theta);

	int registerData(PndLmdData* data);
	int registerData(std::vector<PndLmdData*> &data_vec);
	int registerAcceptance(PndLmdAcceptance* acc);
	int registerAcceptances(std::vector<PndLmdAcceptance*> &acc_vec);
	int registerResolution(PndLmdResolution* res);
	int registerResolutions(std::vector<PndLmdResolution*> &res_vec);

	void fillData(double plab, TString dir_path, data_type data_mode,
			acceptance_type acc_mode = FULL, TString MC_file = "Lumi_MC_*.root",
			TString geane_file = "Lumi_Geane_*.root", TString track_file =
					"Lumi_Track_*.root");

#ifndef __CINT__
	ModelFitResult determineResolutionForSlice(PndLmdResolution* lmd_resolution,
			const PndLmdLumiFitOptions *fit_options);
#endif /* __CINT __ */


	void determineResolution(std::vector<PndLmdResolution*> &lmd_resolutions,
			const PndLmdLumiFitOptions *fit_options);
};

#endif /* PNDLMDLUMIHELPER_H_ */
