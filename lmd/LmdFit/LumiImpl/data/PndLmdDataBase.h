/*
 * PndLmdDataBase.h
 *
 *  Created on: Jul 5, 2012
 *      Author: steve
 */

#ifndef PNDLMDDATABASE_H_
#define PNDLMDDATABASE_H_

#include "PndLmdDataInterface.h"

#include "TMath.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TF2.h"
#include "TTree.h"

class PndLmdLumiFitResult;
class PndLmdLumiFitOptions;

/**
 * \brief Base class for importing any kind of data created with lmd macros. User should use #PndLmdData and #PndLmdAcceptance.
 *
 * The class contains fundamental data storing objects, i.e histograms in case of binned fits or data sets for unbinned fits.
 * It includes the most basic handling routines for these objects and functions to fill these objects with data.
 */
class PndLmdDataBase: public PndLmdDataInterface {
protected:
	/** ROOT 1D histogram for mc truth theta values. In case of a simulation this exists. */
	TH1D* mc_1d;
	/** ROOT 1D histogram for accepted (reconstruction was successful) mc truth theta values. In case of a simulation this exists. */
	TH1D* mc_acc_1d;
	/** ROOT 1D histogram for reconstructed theta values. */
	TH1D* reco_1d;

	/** ROOT 1D histogram for mc truth t values. In case of a simulation this exists. */
	TH1D* t_mc_1d;
	/** ROOT 1D histogram for accepted (reconstruction was successful) mc truth t values. In case of a simulation this exists. */
	TH1D* t_mc_acc_1d;
	/** ROOT 1D histogram for reconstructed t values. */
	TH1D* t_reco_1d;

	/** ROOT 2D histogram for mc truth theta and phi values. In case of a simulation this exists. */
	TH2D* mc_2d;
	/** ROOT 2D histogram for accepted (reconstruction was successful) mc truth theta and phi values. In case of a simulation this exists. */
	TH2D* mc_acc_2d;
	/** ROOT 2D histogram for reconstructed theta and phi values. */
	TH2D* reco_2d;

	TTree* reco_unbinned;

	/** constructs the name of the object from the binning information. Note that this can be overwritten by inherited classes. */
	virtual void makeName();

public:
	/** Empty constructor required by ROOT IO */
	PndLmdDataBase();
	/** Standard constructor taking all binning information as parameters */
	PndLmdDataBase(TFile* f_, int num_events_, double plab_, PndLmdFit::lmd_dimension th_dimension_,
			PndLmdFit::lmd_dimension phi_dimension_);

	virtual ~PndLmdDataBase();


	// other helper methods
	void makeDir();
	virtual void saveToRootFile();

	void setUnbinnedRecoData(TTree* reco_tree);
	TTree* getUnbinnedRecoData() const;

	void fillHistograms(std::vector<std::pair<PndLmdFit::lmd_values, PndLmdFit::lmd_values> > &event_data);

	virtual bool operator==(const PndLmdDataBase &lmd_data_base) const;
	virtual bool operator!=(const PndLmdDataBase &lmd_data_base) const;

ClassDef(PndLmdDataBase,1)
};

#endif /* PNDLMDDATABASE_H_ */
