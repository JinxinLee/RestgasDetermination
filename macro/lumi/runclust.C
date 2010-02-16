{
	// Load basic libraries\par
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();

	Int_t iVerbose = 0;
	Int_t nEvents = 0;

	// -----   Timer   --------------------------------------------------------

   TStopwatch timer;
   timer.Start();

   // --------------

   TString suffix="_sm_1250";

   TString simFile = "sim_"+ suffix +".root";
   TString digiFile = "digi_"+ suffix +".root";
   TString parFile = "params_"+ suffix +".root";


   TString clustFile = "clust_"+ suffix + ".root";


   TString digiparFile = gSystem->Getenv("VMCWORKDIR");
      digiparFile += "/macro/params/lumi.digi.par";

   FairRunAna *fRun = new FairRunAna();
   fRun->SetInputFile(digiFile);
   fRun->AddFriend(simFile);
   fRun->SetOutputFile(clustFile);


   FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
   FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
   parInput1->open(parFile.Data(),"UPDATE");
   rtdb->setFirstInput(parInput1);


   FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
   parInput2->open(digiparFile.Data(),"in");
   rtdb->setSecondInput(parInput2);

   fRun->LoadGeometry();

   /** Run Clusterization **/
   Double_t charge_cut = 0;

   PndLumiClusterTask* lumiCluster = new PndLumiClusterTask(charge_cut,0,1,0, iVerbose);
   fRun->AddTask(lumiCluster);

   // Initialize and run\par

   fRun->Init();
   fRun->Run(0, nEvents);

   rtdb->saveOutput();
   rtdb->print();

   timer.Stop();
   Double_t rtime = timer.RealTime();
   Double_t ctime = timer.CpuTime();
   cout << endl << endl;
   cout << "Macro finished succesfully." << endl;
   cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
   cout << endl;
   exit();
}
