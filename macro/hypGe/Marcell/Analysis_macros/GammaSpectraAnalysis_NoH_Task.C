//void GammaSpectraAnalysis_NoH_Task(TString Filename = "Sim_Geo36_E0.500MeV_Evts1000_FileEvts1000_Gen1_ST0__0.root", TString SubFolder ="Sim_Geo36_E0.500MeV_Evts1000_FileEvts1000_Gen1_ST0")//, Double_t Energy)
void GammaSpectraAnalysis_NoH_Task(
		TString Filename = "Sim_Geo43_E0.519MeV_Evts2860000_FileEvts11440_Gen1_ST12__248.root",
		TString SubFolder ="Sim_Geo43_E0.519MeV_Evts2860000_FileEvts11440_Gen1_ST12",
		Bool_t OmegaQuadrupolMode = 1,
		Double_t PeakWidthStretchfactor=1
				)//, Double_t Energy)

{

	Int_t iVerbose = 0;
	
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	//rootlogon();
	gROOT->LoadMacro("$VMCWORKDIR/macro/hypGe/Marcell/SharedMacros/SharedMacroFunctions.C");

	gSystem->Load("libHypGe");
	gSystem->Load("librazhyp");
	gSystem->Load("libHyp");
	gSystem->Load("libSpectrum");
	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();
	
	// get energy from filename
	Double_t Energy = GetEnergyFromFilename(Filename);			// now in MeV

	// get number of events from filename

	Int_t nEvents = GetNumberOfEventsInFileFromFilename(Filename);

	Int_t TargetSimulated= GetIfTargetIsSimulated(Filename);
	cout << "TargetSimulated " << TargetSimulated << endl;
	if(Filename.EndsWith(".root",1))
	{
		Filename.ReplaceAll(".root","");
		cout << "Filename ending chopped!" << endl;
	}
	TString CompleteFilename = "$SIMDATADIR/Gamma/"+SubFolder+"/"+Filename+".root";
	TString CompleteParameterFilename = "$SIMDATADIR/Gamma/"+SubFolder+"/"+Filename+"__Simparams.root";
	//Output Files
	Filename.ReplaceAll("Sim","Ana");
	SubFolder.ReplaceAll("Sim","Ana");
	TString TextAdd = "_OQP_Psf";					//text added to files /folders in Omega Quadropole mode
			TextAdd +=PeakWidthStretchfactor;
	if (OmegaQuadrupolMode)
	{
		SubFolder+=TextAdd;
	}
	TString Path = getenv("SIMDATADIR");
	TString FullPath= Path+"/Gamma/Ana/"+SubFolder;
	char CommandBuffer[400];
	sprintf(CommandBuffer,".!mkdir -p %s",FullPath.Data());
		cout << "Processing " << CommandBuffer<< endl;
		gROOT->ProcessLine(CommandBuffer);		// create subfolder for the output
	TString outfile;
	TString txtfileName;
		if (OmegaQuadrupolMode)
	{

		outfile = FullPath + "/" + Filename;
		outfile.Insert(outfile.Index("__"),TextAdd.Data());
		//cout << outfile.Data()<<endl;

		txtfileName = outfile+ ".txt";
		outfile+=".root";

	}
	else
	{
		outfile = FullPath + "/" + Filename + ".root";
		txtfileName = FullPath + "/" + Filename + ".txt";
	}
	//--------- gamma ana run
	FairRunAna *fRun= new FairRunAna();
	fRun->SetInputFile(CompleteFilename.Data());
	fRun->SetOutputFile(outfile.Data());
	
	 // -----  Parameter database   --------------------------------------------
  
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	FairParRootFileIo* parInput1 = new FairParRootFileIo();
	parInput1->open(CompleteParameterFilename.Data());
	
	Bool_t kParameterMerged=kTRUE;
	rtdb->setFirstInput(parInput1);
	
	
		
	//--------analysis task----------------------------------------------------
	PndHypGeGammaAna* GammaAna = new PndHypGeGammaAna(txtfileName,Energy,nEvents);
	GammaAna->SetVerbose(iVerbose);
	GammaAna->SetTarget(TargetSimulated);
	if (OmegaQuadrupolMode)
	{
		GammaAna->SetOmegaQuadrupolMode(1);
		GammaAna->SetPeakWidtchStrechFactor(PeakWidthStretchfactor);
	}
	fRun->AddTask(GammaAna);
	

	fRun->Init();

	fRun->Run(0,nEvents);
	
	
	// -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outfile << endl;
  cout << "Text file is " << txtfileName << endl;
  //cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
}
