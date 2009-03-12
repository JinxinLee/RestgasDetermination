/*
 * runLumiReco.C
 *
 *  Created on: Nov 19, 2008
 *      Author: tsito
 */

{
//#include <exception>

	// Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  Int_t iVerbose = 1;
  Int_t nEvents = 0;

  // -----   Timer   --------------------------------------------------------
   TStopwatch timer;
   timer.Start();
   // --------------


  TString inFile = "lumiclust.root";
  TString outFile = "lumireco.root";

  //Run Reconstruction
  // ------------
  FairRunAna *fRun = new FairRunAna();
    fRun->SetInputFile(inFile);
    fRun->SetOutputFile(outFile);

    Double_t m   = 100.;
    Double_t cm  = 1.;
    Double_t mm  = 0.1;
    Double_t mum = 0.0001;//cm
    Double_t pi = TMath::Pi();
      Double_t deg = pi/180.;

    Double_t pitch = 50. * mum;
    Double_t sigma = 8. * mum;
    Double_t orient = 30.* deg;
  PndLumiRecoTask* lumiReco = new PndLumiRecoTask(pitch, sigma, orient);

  fRun->AddTask(lumiReco);

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
