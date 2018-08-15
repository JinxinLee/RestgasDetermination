// macro to fit hits with a straight line
// Simone Bianco 15/07/2010


int SmartAli()
{

  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  Int_t iVerbose = 0;
  Int_t nEvents = 10000;
  Int_t nLoops = 5;


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // --------------
  
  TString trackFile = "data27.hit.hits.root";
  
  TString trkfitFile = "buff.root";

  //Run Reconstruction
  // ------------

  FairRunAna *fRun = new FairRunAna();
  fRun->SetInputFile(trackFile);
  fRun->SetOutputFile(trkfitFile);

  TtAliTask* linfit = new TtAliTask();

  // Initialize and run

  for (Int_t j = 0 ; j < nLoops ; j++)
    {
      
      for (Int_t p = 1 ; p < 7 ; p++)
	{
	  linfit->SetExcludedBox(p);
      
	  if (j==0 && p == 1)
	    {
	      fRun->AddTask(linfit);
	      fRun->Init();
	    }

	  cout << "Loop: " << j << " Excluded Box: " << p << endl;
	  
	  //  if (j == (nLoops-1))   linfit->FlagPrint(1);

	  fRun->Run(0, nEvents);

	  linfit->PrintVal();
  
	} // end loop on excluded sensors
	  
      linfit->PrintMeanResiduals();

    } // end loops



  
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
	
  //exit();
  return 0;
}
