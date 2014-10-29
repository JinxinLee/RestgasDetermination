void COSYBackgroundAnalysis(TString Filename, Int_t StartEvent=0, Int_t NoOfEvents=0) 
{
	Int_t nEvents= NoOfEvents;//1000000;
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
	Path += "/COSY";
	TString inFile = Path+"/"+Filename+".root";
	
	//---------compose parFile---------------------------------------------------
	TString parFile = Path+"/"+Filename+"__Simparams.root";
	//---------compose outFile---------------------------------------------------
	
	TString outFile= Path;
	outFile += "/Ana/ToCombine/";
	outFile += Filename;
	outFile += "/Ana";
	outFile += Filename;
	outFile.ReplaceAll(".root","");
	outFile += "_Start_";
	outFile += StartEvent;
	outFile += "_nEvents_";
	outFile += NoOfEvents;
	outFile += ".root";
	//TString txtfileName =  outFile;
	//txtfileName += ".txt";	
	//cout <<endl << outFile << endl << endl;

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
	PndHypGeCOSYBackgroundAna* COSYAna = new PndHypGeCOSYBackgroundAna(nEvents);
	COSYAna->SetVerbose(iVerbose);
	//fRun->AddTask(COSYAna);
	

	fRun->Init();

	fRun->Run(StartEvent,nEvents);				//Run(Int_t NStart ,Int_t NStop);
	

	
	// -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  //cout << "Text file is " << txtfileName << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
}
