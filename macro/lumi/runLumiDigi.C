/*
 * runLumidigi.C
 *
 *  Created on: Jul 28, 2008
 *      Author: tsito
 */
{
	// Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
 
  Int_t iVerbose = 1;
  Int_t nEvents = 0;

  // -----   Timer   --------------------------------------------------------
   TStopwatch timer;
   timer.Start();
   // --------------


  TString inFile = "lumisim.root";
  TString outFile = "lumidigi.root";

 
  FairRunAna *fRun = new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);

  Double_t m   = 100.;
  Double_t cm  = 1.;
  Double_t mm  = 0.1;
  Double_t mum = 0.0001;//cm

  Double_t pi = TMath::Pi();
  Double_t deg = pi/180.;

  Double_t Z0 = 10.5 * m;
  Double_t d = 0.2 * m; // distance between 2 consecutive planes
  Double_t l = 0.05 * m; // length of sensor
  Double_t w = 0.02 * m; // width of a sensor
  Double_t r = 0.03 * m; // radial distance of sensors from the beam axis 
  Double_t pitch = 50. * mum;
  Double_t skew = 30.* deg;// front face strip orientation 
  
  Double_t thr = 10000.;// threshold [eV]
  Double_t noise = 5000;// noise

  Double_t sigma = 8. * mum;//Gaussian width of the smearing
  
  // Run digitization 
  //------------------
  
  PndLumiDigiProducer* lumiDigiProducer = new PndLumiDigiProducer( Z0, pitch,
		  skew, w , l , r , d , thr , noise, 1., sigma, iVerbose);

  fRun->AddTask(lumiDigiProducer);
  
  // Intialize and run
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
