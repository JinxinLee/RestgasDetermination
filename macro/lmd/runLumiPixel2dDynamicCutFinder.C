/*
 * Finds pixel hit pairs and stores them to pair File
 *
 *	Author: Roman Klasen, roklasen@uni-mainz.de or klasen@kph.uni-mainz.de
 *
 */

using namespace std;

/*
#include <TROOT.h>
#include <TString.h>
#include <TStopwatch.h>
#include <FairRunAna.h>
#include <FairRuntimeDb.h>
#include <FairParRootFileIo.h>
#include <FairParAsciiFileIo.h>
#include <LmdPairFinderTask.h>
#include <PndSdsGeoPar.h>
 */

#include <stdio.h>

void runLumiPixel2dDynamicCutFinder(const int nEvents=0, const int startEvent=00000, TString storePath="test/fullrun-1.5/digi", const int verboseLevel=0)
{
	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();

	gSystem->Load("libLmd");
	gSystem->Load("libLmdPairFinder");
	gSystem->Load("libLmdSensorAligner");

	cout << "***********************\n";
	cout << "Running PairFinderTask.\n";
	cout << "***********************\n";

//	// -----   Input File   ----------------------------------------------------
	TString inFile=storePath+"/Lumi_digi_";
	inFile += startEvent;
	inFile += ".root";


	/*
	 * This is very ugly. I don't need an output file here, but the FairRunAna segfaults when none is specified.
	 * So I create a temp file and delete it afterwards.
	 */
//	// -----   Parameter Files   ------------------------------------------------
	TString parFile=storePath+"/Lumi_Params_";
	parFile += startEvent;
	parFile += ".root";
	TString digiparFile = "lumi.digi.par";
	std::string cutParameterfile = storePath + "/cutParameters.json";

	// -----   Output File   --------------------------------------------------
	TString outFile = storePath+"/Lumi_TEMP_";
	outFile += startEvent;
	outFile += ".root";
	std::cout << "DigiFileName: " << outFile.Data() << std::endl;

	// -----   Pair Finder / Cut Finder Runs   -------------------------------------------
	FairRunAna *fRun= new FairRunAna();
	fRun->SetInputFile(inFile);
	fRun->SetOutputFile(outFile);
	fRun->SetEventMeanTime(50);// TODO: 50 ???

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

	//find dynamic cut parameters
	LmdPairFinderTask* lmdPairFinder = new LmdPairFinderTask();
	lmdPairFinder->findDynamicCutParameters(true, cutParameterfile);
	fRun->AddTask(lmdPairFinder);

	//rtdb->setOutput(parInput1);
	//rtdb->print();
	// =====                 End of PairFinder                             =====
	// =========================================================================

	// -----   No Idea what this does   ----------------------------------------
	//PndSdsGeoPar* geoPar  = (PndSdsGeoPar*)(rtdb->getContainer("PndSdsGeoPar"));

	// -----   Initialize and run PairFinder ----------------------------------
	fRun->Init();
	fRun->Run(0,nEvents);
	//rtdb->saveOutput();
	//rtdb->print();
	remove(outFile);

	return;
}
