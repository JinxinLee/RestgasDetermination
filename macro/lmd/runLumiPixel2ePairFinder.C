/*
 * Finds pixel hit pairs and stores them to pair File
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

void runLumiPixel2ePairFinder(const int nEvents=0, const int startEvent=00000, TString storePath="test/fullrun-1.5", const int verboseLevel=0, bool storeSorted=false)
{
	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();

	// -----   Initialize Shit   ----------------------------------------------
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

	//gSystem->Load("libLmd");
	//gSystem->Load("libLmdPairFinder");


	cout << "Running PairFinder in Task. LOLOLOLOLOL." << endl;

	// -----   Input File   ----------------------------------------------------
	TString inFile=storePath+"/Lumi_digi_";
	inFile += startEvent;
	inFile += ".root";

	// -----   Parameter File   ------------------------------------------------
	TString parFile=storePath+"/Lumi_Params_";
	parFile += startEvent;
	parFile += ".root";
	TString digiparFile = "lumi.digi.par";

	// -----   Output File   --------------------------------------------------
	TString outFile = storePath+"/Lumi_Pairs_";
	outFile += startEvent;
	outFile += ".root";
	std::cout << "DigiFileName: " << outFile.Data() << std::endl;

	// -----   Reconstruction run   -------------------------------------------
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

	LmdPairFinderTask* lmdPairFinder = new LmdPairFinderTask();
	lmdPairFinder->storeSorted(storeSorted);	//for easier access, but takes longer
	lmdPairFinder->setMaxDistance(320e-4);		//two pixels
	fRun->AddTask(lmdPairFinder);

	rtdb->setOutput(parInput1);
	rtdb->print();
	// =====                 End of PairFinder                             =====
	// =========================================================================

	// -----   No Idea what this does   ----------------------------------------
	PndSdsGeoPar* geoPar  = (PndSdsGeoPar*)(rtdb->getContainer("PndSdsGeoPar"));

	// -----   Initialize and run   --------------------------------------------
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

}
