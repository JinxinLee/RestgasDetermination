int runLumiPixel2Reco(const int nEvents = 10, const int startEvent = 0,
		TString storePath = "tmpOutput", string alignment_matrices_path = "", const int verboseLevel = 0) {
	// ========================================================================
	TString DigiFile = storePath + "/Lumi_digi_"; //"/Lumi_digi_noise_";//"/Lumi_digi_";
	DigiFile += startEvent;
	DigiFile += ".root";
	// Parameter file
	TString parFile = storePath + "/Lumi_Params_";
	parFile += startEvent;
	parFile += ".root";

	// Output file
	TString outFile = storePath + "/Lumi_reco_";
	outFile += startEvent;
	outFile += ".root";

	//std::cout << "MCFile  : " << MCFile.Data()<< std::endl;
	//std::cout << "DigiFile: " << DigiFile.Data() << std::endl;
	//std::cout << "RecoFile: " << outFile.Data() << std::endl;

	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();
	// ------------------------------------------------------------------------

	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *fRun = new FairRunAna();
	FairFileSource input_source(DigiFile);
	fRun->SetSource(&input_source);
	fRun->SetOutputFile(outFile);
	// ------------------------------------------------------------------------

	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
	parInput1->open(parFile.Data(), "UPDATE");
//   FairParAsciiFileIo* parInput1 = new FairParAsciiFileIo();
//   parInput1->open(parFile.Data(),"in");
	rtdb->setFirstInput(parInput1);

	// alignment parameter file
	//  TString alignparFile = "lumi.align.par";//perfect! OLD
	//  TString alignparFile = "lumi.misalign.par";//misalignment!  OLD
	//  TString alignparFile = "lumi.align_pixel_ideal.par";//perfect
	/* TString alignparFile;
	 if(misalign){
	 alignparFile = storePath+"/lumi.misalign_pixel.par";//misalign
	 }
	 else{
	 alignparFile = "LMD_alignment/lumi.align_pixel_ideal.par";//perfect
	 }

	 FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
	 parInput2->open(alignparFile.Data(),"in");
	 rtdb->setSecondInput(parInput2);*/

	/*Bool_t kParameterMerged=kTRUE;
	 FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
	 output->open(parOutFile);
	 rtdb->setOutput(output);
	 */
// fRun->LoadGeometry();
	// ------------------------------------------------------------------------
	// =========================================================================
	// ======                       Hit Producers                         ======
	// =========================================================================
	// -----    LMD hit producer   --------------------------------------------
	PndLmdPixelClusterTask* lmdmccls = new PndLmdPixelClusterTask();
	lmdmccls->SetVerbose(verboseLevel);

	fRun->AddTask(lmdmccls);
	rtdb->setOutput(parInput1);
	rtdb->print();
	// =====                 End of HitProducers                           =====
	// =========================================================================

	// -----   Intialise and run   --------------------------------------------
	fRun->Init();

	if (alignment_matrices_path != "") {
		//load matrices
		TFile *alignmentMatrixRootfile = new TFile(alignment_matrices_path.c_str(), "READ");

		if (alignmentMatrixRootfile->IsOpen()) {
			std::map < std::string, TGeoHMatrix > *matrices;

			gDirectory->GetObject("PndLmdMisalignMatrices", matrices);
			alignmentMatrixRootfile->Close();

			cout << matrices->size() << " matrices successfully read from file.\n";

			//iterate over matrices
			fRun->SetAlignmentMatrices(*matrices);
			fRun->AlignGeometry();
		}
		else {
			cout << "file could not be read\n";
			return 1;
		}
	}

	fRun->Run(0, nEvents);
	// ------------------------------------------------------------------------

	rtdb->saveOutput();
	rtdb->print();
	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is " << outFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------

	// temporary fix to avoid double frees at the destruction of te program for pandaroot/fairroot with root6
	gGeoManager->GetListOfVolumes()->Delete();
	gGeoManager->GetListOfShapes()->Delete();
	delete gGeoManager;

	return 0;
}
