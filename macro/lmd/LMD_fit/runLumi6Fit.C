/*
 * This is the macro which should be used for luminosity fitting (luminosity extraction procedure)
 * General information about the individual classes of the LmdFit framework can be
 * found in the doxygen manual
 * Run it like this:
 * 
 * root -l -b -q 'runLumi6Fit.C("dir_to_elastic_scattering_data", beam momentum, number of events, "dir_to_acceptance or box_gen_data", generated luminosity)'
 *
 * note: the last parameter is the generated luminosity. In case you do not specify
 * this values the plot generating macro will produce unusable numbers. Ill try to work
 * in that it will use MCtruth values as a fallback
 *
 * This macro is basically a 3 step process and each step is explained in more detail
 * below:
 * step1: read in the "raw" data and create lmddata objects on which the luminosity fit is supposed to be performed
 * step2: create fit options that specify every detail about the fit
 * step3: perform the fit and store the results
 */

void runLumi6Fit(TString input_file_dir, const double mom, int nEvents = -1,
		TString acceptance_file_dir = "", const double generated_luminosity_per_event = -1.0,
		const int verboseLevel = 0) {
	std::cout << "Running LumiFit....\n";

	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();
	// ------------------------------------------------------------------------

	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libLmdFit");

  // A small helper class that helps to construct lmddata objects
	PndLmdLumiHelper lumifit_helper;

  // ---- Output file -------------------------------------------------------
	TString out = input_file_dir + "/fit_results.root";
	TFile *f = new TFile(out, "RECREATE");
  // ------------------------------------------------------------------------

  
  // ============================== BEGIN STEP 1 ============================== //
  /**
   * There are two types of data classes PndLmdData (elastic data) and PndLmdAcceptance (boxgen data).
   * These objects can be easily filled with data using the helper class
   * (PndLmdLumiHelper using registerData and fillHists()).
   * The idea is to construct empty data objects (meaning no data are inside yet) at first and register
   * them within in the helper instance. Of course each data object has to be unique
   * from its options (such as number of events, data range, binning etc).
   * Once you have all of your data objects created that you want to work with simply
   * run fillHists() of the helper instance and all of the registerd data objects will
   * be filled in a single swoop!
   */

  double data_range_low = 0.5;
	double data_range_high = 20.0;

  // ---- dpm or elastic data ---- //
  // create a temporary vector to store the data objects
  std::vector<PndLmdData*> my_lmd_data_vec;

  // in this case the loop was used for different binnings, but here only a single
  // binning is actually created...
  for (int i = 10; i < 11; i = i + 1) {
    // create data object and push it into the vector
    PndLmdFit::lmd_dimension theta(i * 10, data_range_low, data_range_high); // theta dimension
    PndLmdFit::lmd_dimension phi(50, -TMath::Pi(), TMath::Pi()); // phi dimension
		PndLmdData *data = new PndLmdData(f, nEvents, mom, theta, phi, generated_luminosity_per_event);
		my_lmd_data_vec.push_back(data);
	}

  // ---- acceptance or box gen data ---- //
  // same procedure as for the "real" data
	std::vector<PndLmdAcceptance*> my_lmd_acc_vec;

	for (int i = 10; i < 11; i = i + 1) {
    PndLmdFit::lmd_dimension theta(i * 20, data_range_low, data_range_high); // theta dimension
    PndLmdFit::lmd_dimension phi(50, -TMath::Pi(), TMath::Pi()); // phi dimension

		PndLmdAcceptance *acc = new PndLmdAcceptance(f, nEvents, mom, theta, phi, 0);
		my_lmd_acc_vec.push_back(acc);
	}

	//register data objects in helper instance 
	lumifit_helper.registerData(my_lmd_data_vec);
	lumifit_helper.registerAcceptances(my_lmd_acc_vec);

	// fill all elastic scattering histograms that are registered in the gamma helper object
 	lumifit_helper.fillData(mom, input_file_dir, PndLmdLumiHelper::DATA, PndLmdLumiHelper::FULL);
	// fill all box gen histograms (for acceptance) that are registered in the gamma helper object
	// note: 3rd argument identifies specifies which kind of data is being read in
  lumifit_helper.fillData(mom, acceptance_file_dir, PndLmdLumiHelper::ACCEPTANCE, PndLmdLumiHelper::FULL);
  
  // =============================== END STEP 1 =============================== //
  

  // ============================== BEGIN STEP 2 ============================== //
  /* construct fit options:
   * Simply construct PndLmdLumiFitOptions objects which has the following constructor
   * parameters (there are also constructors with less parameters, then you have to use
   * the setter functions for all of the options!)
   * param 1: bit_flag_options is a number in which each digit represents a binary flag @see PndLmdLumiFitOptions(int)
   * ((
   * bit 1 turns on resolution smearing,
   * bit 2 turns on acceptance correction,
   * bit 3 turns on 2d fit
   * bit 4 turns on raw fit (momentum transfer)
   * example: 1d angular fit with acceptance correction but no resolution smearing
   * -> 0 * 2^0 + 1 * 2^1 + 0 * 2^2 + 0 * 2^3  = 2
   * ))
	 * param 2: free_parameters_code_ is a bitcode specifying which parameter is free in the fit procedure
   * ((
   * luminosity will always be fitted,
   * bit 1 turns on sigma_total as a fit parameter,
   * bit 2 turns on rho as a fit parameter,
   * bit 3 turns on b as a fit parameter
   * example: to fit luminosity, sigma_total and b
   * -> 1 * 2^0 + 0 * 2^1 + 1 * 2^2 = 5
   * ))
	 * param 3: smearing_type_ specifies which model is used for the smearing (0 = gaussian, 1 = double gaussian)
	 * param 4: acc_intpol_type_ specifies the interpolation type of the acceptance (0 =
   * constant, 1 = linear-spline, 2 = cubic-spline)
	 * param 5: plab
	 * param 6: theta_fit_range_low_ used as the new lower fit range for theta
	 * param 7: theta_fit_range_high_ used as the new upper fit range for theta
	 * param 8: phi_fit_range_low_ used as the new lower fit range for phi <-- optional
	 * param 9: phi_fit_range_high_ used as the new upper fit range for phi <-- optional
   */
  std::vector<PndLmdLumiFitOptions*> fit_options_vec;
  PndLmdLumiFitOptions *temp_fit_opt;

  double fit_range_low = 4.0;
  double fit_range_high = 8.0;

  TString resolution_parametrization_file_url = acceptance_file_dir + "/resolution_params_1.root";

  LumiFit::LmdBinaryFitOptions bit_fit_opt(0);
  bit_fit_opt.setFitAsRaw(true);
  temp_fit_opt = new PndLmdLumiFitOptions(bit_fit_opt, 0, 0, 1, mom, fit_range_low,
      fit_range_high);
  temp_fit_opt->setResolutionParametrizationFileUrl(resolution_parametrization_file_url);
  fit_options_vec.push_back(temp_fit_opt);

  bit_fit_opt.setFitAsRaw(false);
  temp_fit_opt = new PndLmdLumiFitOptions(bit_fit_opt, 0, 0, 1, mom, fit_range_low,
      fit_range_high);
  temp_fit_opt->setResolutionParametrizationFileUrl(resolution_parametrization_file_url);
  fit_options_vec.push_back(temp_fit_opt);

  bit_fit_opt.setAcceptanceCorrMode(true);
  temp_fit_opt = new PndLmdLumiFitOptions(bit_fit_opt, 0, 0, 1, mom, fit_range_low,
      fit_range_high);
  temp_fit_opt->setResolutionParametrizationFileUrl(resolution_parametrization_file_url);
  fit_options_vec.push_back(temp_fit_opt);

  temp_fit_opt = new PndLmdLumiFitOptions(bit_fit_opt, 0, 0, 1, mom, fit_range_low,
      fit_range_high);
  temp_fit_opt->setResolutionParametrizationFileUrl(resolution_parametrization_file_url);
  temp_fit_opt->getDataBinaryOptions().setSmearingMode(true);
  fit_options_vec.push_back(temp_fit_opt);
  std::cout<<temp_fit_opt->getDataBinaryOptions().getBinaryOptions()<<std::endl;

  bit_fit_opt.setSmearingMode(true);
  temp_fit_opt = new PndLmdLumiFitOptions(bit_fit_opt, 0, 0, 1, mom, fit_range_low,
      fit_range_high);
  temp_fit_opt->setResolutionParametrizationFileUrl(resolution_parametrization_file_url);
  fit_options_vec.push_back(temp_fit_opt);
  

  // =============================== END STEP 2 =============================== //
  

  // ============================== BEGIN STEP 3 ============================== //

  // perform luminosity fits
  // in this case we do all combinations of data, acceptances and fit options...
  // you probably do NOT want this so change it according to your needs
	std::cout << "\nPerforming Luminosity Fits....\n";
	for (int i = 0; i < my_lmd_data_vec.size(); i++) {
		for (int j = 0; j < my_lmd_acc_vec.size(); j++) {
			for (int k = 0; k < fit_options_vec.size(); k++) {
				my_lmd_data_vec[i]->Fit(my_lmd_acc_vec[j], fit_options_vec[k]);
			}
		}
	}
 
  // save fit results by just saving the lmd data objects. They contain both
  // the data and the fit results so from this objects the plotting macro can
  // construct some nice plots for you!
	std::cout << "Saving data....\n";
	for (int i = 0; i < my_lmd_data_vec.size(); i++) {
		my_lmd_data_vec[i]->saveToRootFile();
	}
  
  // =============================== END STEP 3 =============================== //
  

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
