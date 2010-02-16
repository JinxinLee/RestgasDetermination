/*
 * runtrackfitter.C
 *
 *  Created on: Jan 11, 2010
 *      Author: tsito
 */

{

	// Load basic libraries
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();

	Int_t iVerbose = 0;
	Int_t nEvents = 0;

	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();
	// --------------
	TString suffix="_sm_1250";

	TString simFile = "sim_" + suffix +".root";
	TString clustFile = "clust_"+ suffix +".root";
	TString parFile = "params_"+ suffix +".root";
	TString trackFile = "track_"+ suffix+".root";

	TString trkfitFile = "linetrk_"+suffix+".root";


	//Run Reconstruction
	// ------------
	FairRunAna *fRun = new FairRunAna();
	fRun->SetInputFile(trackFile);
	fRun->AddFriend(simFile);
	fRun->SetOutputFile(trkfitFile);

	PndDmlLinFitTask* linfit = new PndDmlLinFitTask();
	fRun->AddTask(linfit);


	// Initialize and run
	fRun->Init();
	fRun->Run(0, nEvents);

	//rtdb->saveOutput();
	//rtdb->print();

	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	exit();
}
