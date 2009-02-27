{
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  Int_t nEvents  = 10;

//   gROOT->Macro("Libs.C");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  // Input file (MC events)
  //TString inFile = "Mvd_DPMfixed_4GeV_10000.root"; //"MvdG4_DPM405_Mag_5000.root";
  //TString inFile = "data/mvdparams.root";
  TString inFile = "Mvd_Test.root";
  // Parameter file
  TString parFile = "Mvd_TestParam.root";
  //TString parFile = "data/mvdparams.root";
  // Parameter output file

//   TString parOutFile = "Test/testParamsOutput.root";

  TString digiparFile = gSystem->Getenv("VMCWORKDIR");
  digiparFile += "/mvd/MvdTools/mvd.digi.par";

  
  // In general, the following parts need not be touched
  // ========================================================================
  // Output file
  
//  gROOT->Macro("Libs.C");
//  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
//     basiclibs();
//
//     // Load this example libraries
//     gSystem->Load("libGeoBase");
//     gSystem->Load("libParBase");
//     gSystem->Load("libBase");
//     gSystem->Load("libMCStack");
//     gSystem->Load("libGen");
//     gSystem->Load("libField");
//     gSystem->Load("libDpmEvtGen");
//     gSystem->Load("libPGen");
//     gSystem->Load("libPassive");
//     gSystem->Load("libStt"); 
//     gSystem->Load("libMuo");
//     gSystem->Load("libEmc");  
//     gSystem->Load("libTof");
//     gSystem->Load("libDrcProp");
//     gSystem->Load("libDrc");
//     gSystem->Load("libtpc");
//     gSystem->Load("libgenfit");
//     gSystem->Load("libDch");
//     gSystem->Load("libMvd");
//     
     PndMvdFileNameCreator creator(inFile.Data());
       TString outFile = creator.GetDigiFileName().c_str(); //"MvdG4_DPM405_Mag_5000_digi.root";
       std::cout << "DigiFileName: " << outFile.Data() << std::endl;
     
     // -----   Reconstruction run   -------------------------------------------
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);


  // -----  Parameter database   --------------------------------------------
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
 // CbmParRootFileIo* parInput1 = new CbmParRootFileIo(kTRUE);
 // parInput1->open(parFile.Data(),"UPDATE");
 // rtdb->setFirstInput(parInput1);
//  Bool_t kParameterMerged=kTRUE;

  CbmParAsciiFileIo* parInput2 = new CbmParAsciiFileIo();
  parInput2->open(digiparFile.Data(),"in");
  rtdb->setSecondInput(parInput2);

  fRun->LoadGeometry();


  // -----   Digitization   ---------------------------------------
//  PndMvdDigiTask* mvddigi = new PndMvdDigiTask();
//   mvddigi->SetVerbose(iVerbose);
//  fRun->AddTask(mvddigi);
  // =========================================================================
  // ======                       Hit Producers                         ======
  // =========================================================================
  
  // -----    MVD Strip hit producer   ---------------------------------------

//   double   topPitch=0.015,
//            botPitch=0.015,
//            orient=TMath::Pi()*(0.5),
//            skew=TMath::Pi()*(0.5);
//   TVector2 topAnchor(0.,0.);
//   TVector2 botAnchor(0.,0.);
//   int      topFE = 10,
//            botFE = 4,
//            nrFEChannels = 128;
//   double   threshold=3000., noise=1000.;
//   PndMvdStripHitProducer* mvdStripProd
//     = new PndMvdStripHitProducer(topPitch, botPitch,
//                               orient, skew,
//                               topAnchor,botAnchor,
//                               topFE, botFE, nrFEChannels,
//                               threshold, noise);

  PndMvdStripHitProducer* mvdStripProd = new PndMvdStripHitProducer();
  mvdStripProd->SetVerbose(iVerbose);
  fRun->AddTask(mvdStripProd);

  // -----    MVD Pixel hit producer   ---------------------------------------
//   Double_t  lx=0.01, ly=0.01, threshold=600, noise=200;
//   PndMvdHybridHitProducer* mvdPixProd = new PndMvdHybridHitProducer(lx,ly,threshold,noise);
  PndMvdHybridHitProducer* mvdPixProd = new PndMvdHybridHitProducer();
  mvdPixProd->SetVerbose(iVerbose);
  fRun->AddTask(mvdPixProd);



   CbmParRootFileIo* output=new CbmParRootFileIo(kTRUE);
   output->open(parFile.Data());
   rtdb->setOutput(output);
//  rtdb->setOutput(parInput1);

  PndMvdNoiseProducer* mvdNoiseMaker = new PndMvdNoiseProducer();
  mvdNoiseMaker->SetVerbose(iVerbose);
  fRun->AddTask(mvdNoiseMaker);

//   CbmParRootFileIo* output=new CbmParRootFileIo(kTRUE);
//   output->open(parOutFile.Data());
//   rtdb->setOutput(output);

  rtdb->print();
  // =====                 End of HitProducers                           =====
  // =========================================================================
  PndMvdGeoPar* geoPar  = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar")); 
  
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  fRun->Run(0,nEvents);

  rtdb->saveOutput();
  rtdb->print();

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s\a\a" << endl;
  cout << endl;

}
