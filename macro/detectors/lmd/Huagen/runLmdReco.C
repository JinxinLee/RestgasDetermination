/*
 * runLmdReco.C which execute the reconstruction for digi hits
 * *  Created on: Jun 22, 2009
 *      Author: huagen
 */
void runReco()
{
    // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
	  // ----  Load libraries   -------------------------------------------------
	//   gROOT->Macro("Libs.C");
	  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  Int_t nEvents = 1e3;

  TString parFile = "/private/huagen/simdata/LmdParams.root";
//  TString parFile = "/private/huagen/simdata/LmdParams1.root";
  // ------------------------------------------------------------------------
  // Output file
    TString DigiFile = "/private/huagen/simdata/Lmd_Digi_DPM_elastic_3.7_6mrad_5M.root";
    TString outFile =  "/private/huagen/simdata/Lmd_Reco_DPM_elastic_3.7_6mrad_5M.root";
//    TString DigiFile = "/private/huagen/simdata/Lmd_Test_Digi.root";
//    TString outFile =  "/private/huagen/simdata/Lmd_Test_Reco.root";

    std::cout << "DigiFile: " << DigiFile.Data()<< std::endl;
    std::cout << "RecoFile: " << outFile.Data()<< std::endl;

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(DigiFile);
  fRun->SetOutputFile(outFile);

  TString digiparFile = gSystem->Getenv("VMCWORKDIR");
  digiparFile += "/macro/params/lmd.digi.par";
  std::cout<<"the Digipar file is : "<<digiparFile<<std::endl;

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();


  // -----  Parameter database   --------------------------------------------
  FairParRootFileIo* parInput2 = new FairParRootFileIo(kTRUE);
  parInput2->open(parFile.Data(),"READ");
  rtdb->setSecondInput(parInput2);

  FairParAsciiFileIo* parInput1 = new FairParAsciiFileIo();
  parInput1->open(digiparFile.Data(),"in");
  rtdb->setFirstInput(parInput1);

  // =========================================================================
  // ======                       Hit Producers                         ======
  Double_t chargecut = 1.e5;
//  PndLmdStripClusterTask* lmdmccls = new PndLmdStripClusterTask(chargecut,"Lmd_Test_10000_5mrad_0360_1.5GeV_on.root");
  PndLmdStripClusterTask* lmdmccls = new PndLmdStripClusterTask();
  lmdmccls->SetVerbose(iVerbose);
  fRun->AddTask(lmdmccls);


  rtdb->print();
  // =====                 End of HitProducers                           =====

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
//  fRun->Run(0,nEvents);
  fRun->Run();
  // ------------------------------------------------------------------------
//  rtdb->saveOutput();
  rtdb->print();

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
  exit();
  // ------------------------------------------------------------------------
}

int runLmdReco()
{
	runReco();
	  return 0;
}
