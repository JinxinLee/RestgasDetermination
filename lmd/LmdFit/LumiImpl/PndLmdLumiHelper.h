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
#include "ModelStructs.h"
#endif /* __CINT __ */

#include <vector>
#include <map>

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
class TGraphErrors;

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
	std::map<std::string, TGraphErrors*, ModelStructs::string_comp> generateGraphsFromFitResults(
			std::vector<std::pair<double, ModelFitResult> > &fit_results);

	ModelFitResult determineResolutionForSlice(PndLmdResolution* lmd_resolution,
			const PndLmdLumiFitOptions *fit_options);
#endif /* __CINT __ */

	/**
	 * The idea would be to cut down the dimensionality of the problem
	 * by using the parametrization models. So models that describe the
	 * evolution of parameters of the parent model.
	 * In this case:
	 * We have three variables on which the parameters of the smearing models
	 * can depend "plab, theta, phi". Generate/simulate box gen data for the
	 * required ranges of these variables and create the resolution objects.
	 * Fit each of these resolution objects with the base smearing model, which
	 * returns fit results for the parameters of this model.
	 *
	 * 1) Then for a fixed beam momentum and phi (so cut out a "slice" in these two
	 * variables), plot the parameters of the model and fit them with an
	 * appropriate "parametrization model". Do this for all slices...
	 * Then we have again a set of parameters which depend only on phi and plab.
	 * keep doing this procedure until there are no more remaining variables!
	 *
	 * 2) Another way to perform such a dimensionality breakdown, would be to
	 * isolate two variables at a time instead of just one at a time. This means
	 * that for each parameter of the raw smearing model (gaussians) for example
	 * "sigma" or "mean" we would have a 2D histogram along theta and phi, which
	 * has to be fitted with an appropriate 2D "parametrization" model.
	 *
	 * So once one of these two methods have been chosen. The deepest
	 * parametrization model (in this case for plab) would set parameters of his
	 * parent parametrization and so on...
	 * gauss(plabmodel(plab, thetamodel(theta, phimodel(phi, params))))
	 *
	 * I think this is the best (probably only) way to handle this complex
	 * parametrization. TODO is if there is a way to generalize this so that
	 * this procedure can be applied to any kind of model that exists
	 * (assuming this model has a structure as described above).
	 */
	void determineResolution(std::vector<PndLmdResolution*> &lmd_resolutions,
			const PndLmdLumiFitOptions *fit_options,
			unsigned int parametrization_level);
};

#endif /* PNDLMDLUMIHELPER_H_ */
