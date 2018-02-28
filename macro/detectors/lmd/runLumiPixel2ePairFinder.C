/*
 * Finds pixel hit pairs and stores them to pair File
 *
 *	Author: Roman Klasen, roklasen@uni-mainz.de or klasen@kph.uni-mainz.de
 *
 */


void runLumiPixel2ePairFinder(const int nEvents=0, const int startEvent=00000, TString storePath="test/fullrun-1.5/digi", const int verboseLevel=0)
{
	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();

	gSystem->Load("libLmd");
	gSystem->Load("libLmdSensorAligner");

	cout << "***********************\n";
	cout << "Running PairFinderTask.\n";
	cout << "***********************\n";

	//	// -----   Input File   ----------------------------------------------------
	TString inFile=storePath+"/Lumi_digi_";
	inFile += startEvent;
	inFile += ".root";
	TString inFileReco=storePath+"/Lumi_reco_";
	inFileReco += startEvent;
	inFileReco += ".root";

	//	// -----   Parameter Files   ------------------------------------------------
	TString parFile=storePath+"/Lumi_Params_";
	parFile += startEvent;
	parFile += ".root";
	TString digiparFile = "lumi.digi.par";
	TString cutParameterfile = storePath + TString("/cutParameters.json");


	// -----   Output File   --------------------------------------------------
	TString outFile = storePath+"/Lumi_Pairs_";
	outFile += startEvent;
	outFile += ".root";
	std::cout << "DigiFileName: " << outFile.Data() << std::endl;

	// -----   Pair Finder / Cut Finder Runs   -------------------------------------------
	FairRunAna *fRun = new FairRunAna();
	FairFileSource input_source(inFile);
	input_source.AddFriend(inFileReco);
	fRun->SetSource(&input_source);
	fRun->SetOutputFile(outFile);
	// ------------------------------------------------------------------------

	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
	parInput1->open(parFile.Data(),"UPDATE");
	rtdb->setFirstInput(parInput1);
	FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
	parInput2->open(digiparFile.Data(),"in");
	rtdb->setSecondInput(parInput2);


	// =========================================================================
	// =====                 Start of PairFinder                           =====
	// -----   Actual Task   --------------------------------------------------

	//use dynamic cut parameters
	PndLmdPairFinderTask* pndLmdPairFinder = new PndLmdPairFinderTask();
	pndLmdPairFinder->useDynamicCut(true, cutParameterfile.Data() );
	fRun->AddTask(pndLmdPairFinder);

	rtdb->setOutput(parInput1);
	rtdb->print();
	// =====                 End of PairFinder                             =====
	// =========================================================================

	// -----   No Idea what this does   ----------------------------------------
	PndSdsGeoPar* geoPar  = (PndSdsGeoPar*)(rtdb->getContainer("PndSdsGeoPar"));

	// -----   Initialize and run PairFinder ----------------------------------
	fRun->Init();
	fRun->Run(0,nEvents);
	rtdb->saveOutput();
	rtdb->print();

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished successfully." << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s\a\a" << endl;
	cout << endl;

	// temporary fix to avoid double frees at the destruction of te program for pandaroot/fairroot with root6
	gGeoManager->GetListOfVolumes()->Delete();
	gGeoManager->GetListOfShapes()->Delete();
	delete gGeoManager;

	return;
}
