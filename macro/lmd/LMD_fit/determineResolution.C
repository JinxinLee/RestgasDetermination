void determineResolution(TString input_file_dir, unsigned int parametrization_level, const double mom, int nEvents = -1,
    const int verboseLevel = 0) {
  std::cout << "Running determine resolution macro....\n";

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libLmdFit");

  // A small helper class that helps to construct lmddata objects
  PndLmdLumiHelper lumifit_helper;

  // ---- Output file -------------------------------------------------------
  char outfilename[30];
  sprintf(outfilename, "/resolution_params_%u.root", parametrization_level);
  TString outfilename_url = input_file_dir + outfilename;
  TFile *outfile = new TFile(outfilename_url, "RECREATE");

  switch (parametrization_level) {
    case 0:
      double data_theta_range_low = 2.0; //mrad
      double data_theta_range_high = 11.0;
      double data_phi_range_low = -TMath::Pi();
      double data_phi_range_high = TMath::Pi();

      // --- read in box gen data and construct lmd resolution data objects --- //
      // create a temporary vector to store the data objects
      std::vector<PndLmdResolution*> my_lmd_res_vec;

      // create data object and push it into the vector
      PndLmdFit::lmd_dimension theta(150, -2.0 , 2.0); // theta resolution dim
      PndLmdFit::lmd_dimension phi(20, -1.0, 1.0); // phi resolution dimension

      unsigned int slices_theta = 270;
      unsigned int slices_phi = 1;
      double slice_theta_width = (data_theta_range_high-data_theta_range_low)/slices_theta;
      double slice_phi_width = (data_phi_range_high-data_phi_range_low)/slices_phi;

      for(unsigned int index_slice_phi = 0; index_slice_phi < slices_phi; index_slice_phi++) {
        for(unsigned int index_slice_theta = 0; index_slice_theta < slices_theta; index_slice_theta++) {
          PndLmdResolution *res = new PndLmdResolution(outfile, nEvents, mom, data_theta_range_low+slice_theta_width*index_slice_theta, 
              data_theta_range_low+slice_theta_width*(index_slice_theta+1), data_phi_range_low+slice_phi_width*index_slice_phi, data_phi_range_low+slice_phi_width*(index_slice_phi+1), theta, phi);
          my_lmd_res_vec.push_back(res);
        }
      }

      // register data objects in helper instance
      lumifit_helper.registerResolutions(my_lmd_res_vec);

      // fill data
      lumifit_helper.fillData(mom, input_file_dir, PndLmdLumiHelper::RESOLUTION);//, PndLmdLumiHelper::NO_BACKTRACKING);

      // specify which of type of smearing model we want to generate
      PndLmdLumiFitOptions fit_options;
      fit_options.setThetaFitRange(-0.3, 0.3);
      fit_options.setSmearingModelType(0);  // 0 = single gaussian, 1 = double gaussian

      // run the resolution determination method on this data
      lumifit_helper.fitSmearingModelToResolutions(my_lmd_res_vec, &fit_options);
      break;
    case 1:
      char input_filename[30];
      sprintf(input_filename, "/resolution_params_%u.root", parametrization_level-1);
      TString input_filename_url = input_file_dir + input_filename;
      TFile *infile = new TFile(input_filename_url, "READ");
      std::vector<PndLmdResolution*> res_vec = lumifit_helper.getFittedResolutionsFromPath(infile);
      std::vector<PndLmdLumiHelper::lmd_graph*> graph_vec = lumifit_helper.generateLmdGraphs(res_vec);
      lumifit_helper.fitParametrizationModelToGraphs(graph_vec);
      outfile->cd();
      lumifit_helper.saveLmdGraphsToFile(graph_vec);
      break;
    case 2:
      char input_filename[30];
      sprintf(input_filename, "/resolution_params_%u.root", parametrization_level-1);
      TString input_filename_url = input_file_dir + input_filename;
      TFile *infile = new TFile(input_filename_url, "READ");
      std::vector<PndLmdLumiHelper::lmd_graph*> graph_vec = lumifit_helper.getResolutionModelResultsFromFile(infile);
      std::vector<PndLmdLumiHelper::lmd_graph*> filtered_graph_vec = lumifit_helper.filterLmdGraphs(graph_vec, "theta");
      std::vector<PndLmdLumiHelper::lmd_graph*> new_graph_vec = lumifit_helper.generateNewLmdGraphs(filtered_graph_vec);
      outfile->cd();
      lumifit_helper.saveLmdGraphsToFile(filtered_graph_vec);
      break;
    default:
      std::cout << "Error: The requested parametrization level " << parametrization_level
        << " does not exist. The highest level is 3." << std::endl;
      break;
  }

  // save and close file
  outfile->Close();

  // -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	std::cout << std::endl << std::endl;
	std::cout << "Macro finished succesfully." << std::endl;
	std::cout << "Real time " << rtime << " s, CPU time " << ctime << " s"
			<< std::endl;
	std::cout << std::endl;
	// ------------------------------------------------------------------------
}
