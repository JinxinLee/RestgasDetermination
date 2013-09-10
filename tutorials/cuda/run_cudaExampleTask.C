void run_cudaExampleTask(){
	// ========================================================================
	// Simple sample task to run over some sim files
	// Adding of task happens in line 26
	// ========================================================================

	TString MCFile 	= "sim_toy.root";
	TString parFile 	= "sim_toy_params.root" ;
	Int_t nEvents =0;

	TString outFile = "output.root";

	std::cout << "MCFile  : " << MCFile.Data()<< std::endl;

	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();

	// -----   Reconstruction   -----------------------------------------------
	FairRunAna *fRun= new FairRunAna();

	fRun->SetInputFile(MCFile);

	fRun->SetOutputFile(outFile);

	// -----   Add Cuda Example Task   ----------------------------------------
	PndCudaExampleTask * cudaDummy = new PndCudaExampleTask();
	fRun->AddTask(cudaDummy);

	fRun->Init();
	fRun->Run(0,nEvents);

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is "    << outFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}
