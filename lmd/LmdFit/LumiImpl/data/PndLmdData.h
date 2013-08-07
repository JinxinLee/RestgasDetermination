/*
 * PndLmdData.h
 *
 *  Created on: Jun 27, 2012
 *      Author: steve
 */

#ifndef PNDLMDDATA_H_
#define PNDLMDDATA_H_

#include <map>

#include "PndLmdDataBase.h"

#ifndef __CINT__
#include "PndLmdModelFactory.h"
#endif /* __CINT __ */

class PndLmdLumiFitResult;
class PndLmdLumiFitOptions;
class PndLmdAcceptance;

class TFile;
class TH1D;
class TH2D;
class TF1;
class TF2;

/**
 * \brief Data class used to describe the reconstructed data.
 *
 * Contains the user interface to perform fits via #Fit().
 */
class PndLmdData: public PndLmdDataBase {
private:
	std::map<PndLmdAcceptance*, std::vector<PndLmdLumiFitResult*> > fit_map;

#ifndef __CINT__
	PndLmdModelFactory signal_model_fac;
#endif /* __CINT __ */

	/** In case this is a simulation a reference value for the luminosity can be used */
	double luminosity_per_event;

	std::pair<double, double> calcRange(const PndLmdLumiFitOptions *fit_options);

public:
	PndLmdData(int num_events_, double plab_,
			PndLmdFit::lmd_dimension th_dimension_,
			PndLmdFit::lmd_dimension phi_dimension_,
			double generated_luminosity_per_event = -1.0);
	PndLmdData();

	~PndLmdData();
	// getter methods

	void saveToRootFile(TFile *file);

	double getReferenceLuminosity() const;
	void setReferenceLuminosityPerEvent(double luminosity_per_event_);

	double getBinningFactor(const PndLmdLumiFitOptions *fit_opt) const;

	//void makeCorrectedGraph(TF1 *func, TH1D* hist);

	TH1D* getMeasuredHist1D(const PndLmdLumiFitOptions* fit_options) const;
	TH2D* getMeasuredHist2D(const PndLmdLumiFitOptions* fit_options) const;

	/**
	 * Function carrying out a fit with the specified fit options to this data instance.
	 * @params lmd_acc pointer to #PndLmdAcceptance required for acceptance fitting.
	 * In case no acceptance correction should be performed set this value to 0.
	 * @params fit_options pointer to #PndLmdLumiFitOptions object carrying all necessary fit preferences.
	 * @returns pointer to #PndLmdLumiFitResult containing relevant information such as the fitted function and the luminosity and its errors.
	 */
	PndLmdLumiFitResult* Fit(PndLmdAcceptance* lmd_acc,
			const PndLmdLumiFitOptions* fit_options);

	std::map<PndLmdAcceptance*, std::vector<PndLmdLumiFitResult*> >& getFitMap();

	std::vector<PndLmdAcceptance*> getListOfAcceptances();

	PndLmdLumiFitResult* getFitResult(PndLmdAcceptance* lmd_acc,
			PndLmdLumiFitOptions* fit_options);
	std::vector<PndLmdLumiFitResult*> getFitResults(PndLmdAcceptance* lmd_acc);
	std::vector<std::pair<PndLmdAcceptance*, PndLmdLumiFitResult*> > getFitResults(
			PndLmdLumiFitOptions* fit_options);

ClassDef(PndLmdData,1)
};

#endif /* PNDLMDDATA_H_ */
