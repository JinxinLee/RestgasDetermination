/*#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

#include "PndLmdLumiHelper.h"
#include "PndLmdData.h"
#include "PndLmdAcceptance.h"
#include "PndLmdLumiFitOptions.h"
#include "PndLmdLumiFitResult.h"
#include "PndLmdConstants.h"*/

#include "../../lmd/LmdFit/PndLmdConstants.h"

/*
#include "TLatex.h"
#include "TStopwatch.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TROOT.h"
#include "TSystem.h"*/

// this is the macro which should be used for luminosity fitting (luminosity extraction
// procedure)

// process goes as follows
// you need to specify the directory of the elastic data via input_file dir and the lab
// momentum the other parameters are optional hopefully self explanatory
// for example:
// 
// root -l -b -q 'runLumi6Fit.C("dir_to_elastic_scattering_data", 15.0, 100000, "dir_to_acceptance/box_gen_data", 0.06424842147)'
// note: the last parameter is the generated luminosity. In case you do not specify
// this values the plot generating macro will produce unusable numbers. Ill try to work
// in that it will use MCtruth values as a fallback
//
// there are two types of data classes PndLmdData (elastic data) and PndLmdAcceptance (boxgen data)
// these objects can be easily filled with data using the helper class (PndLmdLumiHelper using registerData and fillHists())
// next you create PndLmdLumiFitOptions and fit the data using these options (everything done automatically)
// in each individual fit, a pointer to that fit result is returned
// also this fit result is saved to the lmddata where you can find helpful accessors like getFitResult(s)(...)

void runLumi6Fit(TString input_file_dir, const double mom, int nEvents = -1,
		TString acceptance_file_dir = "", const double generated_luminosity_per_event = -1.0,
		const int verboseLevel = 0) {
	std::cout << "Running LumiFit....\n";

	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();
	// ------------------------------------------------------------------------

	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
    gSystem->Load("libLumiFit");
	//-lRooFit –lRooFitCore -lMinuit
	/*gSystem->Load("libRooFit");
	gSystem->Load("libRooFitCore");
	gSystem->Load("libMinuit");
	gSystem->Load("libSds");
	gSystem->Load("libSdsReco");
	gSystem->Load("libLmd");
	gSystem->Load("libLmdReco");
	gSystem->Load("libLmdTrk");
	gSystem->Load("libLmdFit");*/


	PndLmdLumiHelper lumifit_helper;

// ========================================================================

// ---- Output file -------------------------------------------------------
	TString out = input_file_dir + "/fit_results.root";
	TFile *f = new TFile(out, "RECREATE");
// ------------------------------------------------------------------------

  // we create a vector for the data objects because we may want to create different
  // objects (with different properties, such as number of events, data range, binning
  // etc), but we only want to scan the input data once, thats why below we register
  // all of these objects in the PndLmdHelper instance, which takes care of the
  // histogram filling
	std::vector<PndLmdData*> my_lmd_data_vec;

	double data_range_low = 0.5;
	double data_range_high = 16.0;

	for (int i = 10; i < 11; i = i + 1) {
		PndLmdData *data = new PndLmdData(f, nEvents, mom, i * 10, 50, data_range_low,
				data_range_high, -C_PI, C_PI, generated_luminosity_per_event);
    // sry about this nasty constructor... 
		my_lmd_data_vec.push_back(data);
		//lumifit_helper.registerData(data);
	}

	std::vector<PndLmdAcceptance*> my_lmd_acc_vec;

	for (int i = 10; i < 11; i = i + 1) {
		PndLmdAcceptance *acc1 = new PndLmdAcceptance(f, nEvents, mom, i * 10, 50, 1.0,
				15.0, -C_PI, C_PI, 0);
		//PndLmdAcceptance *acc2 = new PndLmdAcceptance(f, mom, i * 10, 50, 1.0,
		//		15.0, -C_PI, C_PI, 1);

		//lumifit_helper.registerAcceptance(acc1);
		my_lmd_acc_vec.push_back(acc1);
		//my_lmd_acc_vec.push_back(acc2);
	}

	//register objects in helper instance which will be filled later on
	lumifit_helper.registerData(my_lmd_data_vec);
	lumifit_helper.registerAcceptances(my_lmd_acc_vec);

	//lumifit_helper.generatePhiSlicedDataSet(f, mom, 40, 4, 10, data_range_low,
	//		data_range_high, 1.0, 15.0);

	// fill all elastic scattering histograms that are registered in the gamma helper object
 	lumifit_helper.fillHists(mom, input_file_dir, false, PndLmdLumiHelper::FULL);
	// fill all box gen histograms (for acceptance) that are registered in the gamma helper object
	// note: 3rd argument (true) identifies this data as box gen data
	lumifit_helper.fillHists(mom, acceptance_file_dir, true, PndLmdLumiHelper::FULL);

// construct fit options
	std::vector<PndLmdLumiFitOptions*> fit_options_vec;
	for (double fit_range_low = 1.0; fit_range_low < 1.1; fit_range_low += 0.2) {
		fit_options_vec.push_back(
				new PndLmdLumiFitOptions(8, 0, 1, mom, data_range_low,
						data_range_high));
		fit_options_vec.push_back(
				new PndLmdLumiFitOptions(0, 0, 1, mom, data_range_low,
						data_range_high));
        fit_options_vec.push_back(
                new PndLmdLumiFitOptions(2, 0, 1, mom, data_range_low,
                        data_range_high));
		fit_options_vec.push_back(
				new PndLmdLumiFitOptions(3, 0, 1, mom, data_range_low,
						data_range_high));
	//	fit_options_vec.push_back(
	//			new PndLmdLumiFitOptions(1010, mom, data_range_low,
	//					data_range_high));  // 2D fit acc corr
		//fit_options_vec.push_back(new PndLmdLumiFitOptions(111, i, 10.0)); //roofit normal fit
	}

// -----   LumiFitter   ---------------------------------------------------
	std::cout << "Performing Luminosity Fits....\n";

	for (int i = 0; i < my_lmd_data_vec.size(); i++) {
		for (int j = 0; j < my_lmd_acc_vec.size(); j++) {
			for (int k = 0; k < fit_options_vec.size(); k++) {
				my_lmd_data_vec[i]->Fit(my_lmd_acc_vec[j], fit_options_vec[k]);
			}
		}
	}

	std::cout << "Saving data....\n";
	for (int i = 0; i < my_lmd_data_vec.size(); i++) {
		my_lmd_data_vec[i]->saveToRootFile();
	}

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	std::cout << std::endl << std::endl;
	std::cout << "Macro finished succesfully." << std::endl;
	std::cout << "Real time " << rtime << " s, CPU time " << ctime << " s"
			<< std::endl;
	std::cout << std::endl;

	f->Close();
	// ------------------------------------------------------------------------

}
/*
int main(int argc, char* argv[]) {
	if (argc == 4) {
		double mom = atof(argv[1]);
		int nevts = atoi(argv[2]);
		runLumiFit(argv[3], mom, nevts);
	} else if (argc == 5) {
		double mom = atof(argv[1]);
		int nevts = atoi(argv[2]);
		runLumiFit(argv[3], mom, nevts, argv[4]);
	} else if (argc == 6) {
		double mom = atof(argv[1]);
		int nevts = atoi(argv[2]);
		double gen_lumi = atof(argv[5]);
		runLumiFit(argv[3], mom, nevts, argv[4], gen_lumi);
	}
	return 0;
}*/
