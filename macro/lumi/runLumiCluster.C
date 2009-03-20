/*
 * runLumiCluster.C
 *
 *  Created on: Nov 19, 2008
 *      Author: tsito
 */

{
	//#include <exception>

	// Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  Int_t iVerboseLevel = 1;
  Int_t nEvents = 0;

  // -----   Timer   --------------------------------------------------------
   TStopwatch timer;
   timer.Start();
   // --------------


  TString inFile = "lumisim.root";
  TString outFile = "lumiclust.root";

  // Clusterization run
  // ------------------
  FairRunAna *fRun = new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);

  Double_t m   = 100.;
  Double_t cm  = 1.;
  Double_t mm  = 0.1;
  Double_t mum = 0.0001;//cm

  Double_t pi = TMath::Pi();
  Double_t deg = pi/180.;

  Double_t l = 0.05 * m;
  Double_t w = 0.02 * m;
  Double_t pitch = 50. * mum;
  Double_t skew = 30.* deg;
  
  Double_t thr = 10000.;
  Double_t noise = 5000.;

  Double_t sigma = 8. * mum;

  // Build Clusters
  // ------------

  PndLumiClusterBuilder* lumiClusterBuilder = 
      new PndLumiClusterBuilder(pitch, skew, w, l , thr, noise, sigma);

  fRun->AddTask(lumiClusterBuilder);

  // Initialize and run
  fRun->Init();
  fRun->Run(0, nEvents);

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
}
