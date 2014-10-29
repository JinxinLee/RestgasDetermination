void GammaSpectraAnalysis_NoH_Task(TString Filename)
{
	Int_t nEvents= 0;//1000000;
	Int_t iVerbose = 0;
	
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();

	gSystem->Load("libHyp");
	gSystem->Load("libHypGe");
	gSystem->Load("libSpectrum");
	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();
	
	//---------compose inFile----------------------------------------------------
	if(Filename.EndsWith(".root",1))
	{
		Filename.ReplaceAll(".root","");
		cout << "Filename ending chopped!" << endl;
	}
	TString Path = getenv("SIMDATADIR");
	TString inFile = Path+"/"+Filename+".root";
	
	//---------compose parFile---------------------------------------------------
	TString parFile = Path+"/"+Filename+"__Simparams.root";
	//---------compose outFile---------------------------------------------------
	
	TString outFile= Path+"/Data_Marcell/";
	outFile += Filename;
	TString txtfileName =  outFile;
	outFile +="_Spectrum_test.root";		//changed for development
	txtfileName += "_Spectrum.txt";
	
	//---------get gamma energy from filename-----------------------------------
	Double_t GammaEnergy;
	Int_t IndexPreEnergy = Filename.Index("_",1,0,0);
	Int_t IndexPostEnergy = Filename.Index("MeV",3,0,0);
	TString EnergyFromFileName = Filename(IndexPreEnergy+1,IndexPostEnergy-IndexPreEnergy-1);
	cout << EnergyFromFileName << endl;
	GammaEnergy = EnergyFromFileName.Atof()/1000.;
	 if(GammaEnergy == 0)
		GammaEnergy = 0.001;				//set default value
	
	cout << "Extracted GammaEnergy " << GammaEnergy << endl;
	//--------- gamma ana run
	FairRunAna *fRun= new FairRunAna();
	fRun->SetInputFile(inFile.Data());
	fRun->SetOutputFile(outFile.Data());
	
	 // -----  Parameter database   --------------------------------------------
  
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	FairParRootFileIo* parInput1 = new FairParRootFileIo();
	parInput1->open(parFile.Data());
	
	Bool_t kParameterMerged=kTRUE;
	rtdb->setFirstInput(parInput1);
	
	
		
	//--------analysis task----------------------------------------------------
	PndHypGeGammaAna* GammaAna = new PndHypGeGammaAna(txtfileName,GammaEnergy,nEvents);
	GammaAna->SetVerbose(iVerbose);
	fRun->AddTask(GammaAna);
	

	fRun->Init();

	fRun->Run(0,nEvents);
	
	
	// -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Text file is " << txtfileName << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
}
