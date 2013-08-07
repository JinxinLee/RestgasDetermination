/*
 * PndLmdAcceptance.h
 *
 *  Created on: Jul 4, 2012
 *      Author: steve
 */

#ifndef PNDLMDACCEPTANCE_H_
#define PNDLMDACCEPTANCE_H_

#include "PndLmdDataBase.h"

// these includes are necessary for ROOT IO
#include "TEfficiency.h"
#include "TGraphAsymmErrors.h"

class PndLmdLumiFitOptions;

class TFile;

/**
 * \brief Data class used to describe the acceptance of the luminosity monitor via TEfficiency objects.
 *
 *
 */
class PndLmdAcceptance: public PndLmdDataBase {
private:
	double acceptance_threshold;

#ifndef __CINT__
	double **data;
#endif /* __CINT __ */

	// higher level acceptance objects, that are passed to the fitter etc.
	// (automatically generated later in the program)
	TEfficiency* acceptance_1d;
	TEfficiency* acceptance_2d;

	TEfficiency* t_acceptance_1d;

	void makeName();

public:
	PndLmdAcceptance(int num_events_, double plab_,
			PndLmdFit::lmd_dimension th_dimension_,
			PndLmdFit::lmd_dimension phi_dimension_, double acceptance_threshold_ =
					0.95);
	PndLmdAcceptance();

	~PndLmdAcceptance();
	// getter/setter methods
	double getAcceptanceThreshold() const;

	TEfficiency* getAcceptance1D(bool is_raw) const;
	TEfficiency* getAcceptance2D() const;

	TH2D* getMCHist();
	TH2D* getRecoHist();

	double** getData(PndLmdLumiFitOptions *fit_options);

	// other helper methods
	//void setAcceptanceType(PndLmdLumiFitOptions *fit_options);

	void makeFitterHists();

	void saveToRootFile(TFile *file);

ClassDef(PndLmdAcceptance, 1)
};

#endif /* PNDLMDACCEPTANCE_H_ */
