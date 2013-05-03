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
 *
 * Note that in this class both the ROOT and ROOFIT data structures are handled.
 */
class PndLmdDataBase: public PndLmdDataInterface {
protected:
	/** the number of theta bins */
	int th_bins;
	/** the number of phi bins */
	int phi_bins;
	/** lower theta histogram range */
	double th_range_low;
	/** upper theta histogram range */
	double th_range_high;
	/** lower phi histogram range */
	double phi_range_low;
	/** upper phi histogram range */
	double phi_range_high;
	/** bin size for theta. This will be automatically calculated. Is required when using a binned fit to make fit result independent of the theta binning. */
	double th_bin_size;
	/** bin size for phi. This will be automatically calculated. Is required when using a 2D binned fit to make fit result independent of the phi binning. */
	double phi_bin_size;

	/** the number of t bins */
	int t_bins;
	/** lower t histogram range */
	double t_range_low;
	/** upper t histogram range */
	double t_range_high;
	/** bin size for t. This will be automatically calculated. Is required when using a binned fit to make fit result independent of the t binning. */
	double t_bin_size;

	/** Root file where data is written to. */
	TFile *f;

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
	PndLmdDataBase(TFile*, int num_events_, double plab_, int th_bins_ = 50, int phi_bins_ = 50,
			double th_range_low_ = 0.0, double th_range_high_ = 14.0,
			double phi_range_low_ = -TMath::Pi(), double phi_range_high_ = TMath::Pi());

	virtual ~PndLmdDataBase();

	// getter methods
	int getThBins() const;
	int getPhiBins() const;
	double getThRangeLow() const;
	double getThRangeHigh() const;
	double getPhiRangeLow() const;
	double getPhiRangeHigh() const;
	double getThBinSize() const;
	double getPhiBinSize() const;

	double getTBinSize() const;
	double getTRangeLow() const;
	double getTRangeHigh() const;


	// other helper methods
	double getMomentumTransferFromTheta(double theta) const;
	void makeDir();
	virtual void saveToRootFile();

	void setUnbinnedRecoData(TTree* reco_tree);
	TTree* getUnbinnedRecoData() const;

	void fillHistograms(std::vector<std::pair<lmd_values, lmd_values> > &event_data);

	virtual bool operator==(const PndLmdDataBase &lmd_data_base) const;
	virtual bool operator!=(const PndLmdDataBase &lmd_data_base) const;

ClassDef(PndLmdDataBase,1)
};

#endif /* PNDLMDDATABASE_H_ */
