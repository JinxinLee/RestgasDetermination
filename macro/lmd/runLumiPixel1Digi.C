void runLumiPixel1Digi(const int nEvents=10, const int startEvent=0, TString storePath="tmpOutput", const int verboseLevel=0, const int pitch=1)
{
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();

  // gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  // gSystem->Load("libSds");
  // gSystem->Load("libSdsReco");
  // gSystem->Load("libLmd");
  // gSystem->Load("libLmdReco");
  // gSystem->Load("libLmdTrk");

  // Input file (MC events)
  TString inFile=storePath+"/Lumi_MC_";
  inFile += startEvent;
  inFile += ".root";
  // Parameter file
  TString parFile=storePath+"/Lumi_Params_";
  parFile += startEvent;
  parFile += ".root";

  // Parameter file
  TString digiparFile = "lumi.digi.par";
  //TString digiparFile = "noMClumi.digi.par";
  // TString digiparFile = "lumi";
  // switch(pitch){
  //   case 0:{
  //     digiparFile += "50";
  //     break;}
  // }

  // digiparFile += ".digi.par";

  // cout<<"digiparFile = "<<digiparFile<<endl;
  // In general, the following parts need not be touched
  // ========================================================================
  // Output file
  TString outFile = storePath+"/Lumi_digi_";
  outFile += startEvent;
  outFile += ".root";
  std::cout << "DigiFileName: " << outFile.Data() << std::endl;

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  fRun->SetEventMeanTime(50);//[ns] 50= interaction rate 20 MHz; 
  // example: cross-section(DPM el+inlel, 1.5 GeV/c)= 125 mb, L=2*10^32 -> interaction rate 12.5 MHz

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parInput1);
//   Bool_t kParameterMerged=kTRUE;
//   FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
//   output->open(parOutFile);
//   rtdb->setOutput(output);
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  parInput2->open(digiparFile.Data(),"in");
  rtdb->setSecondInput(parInput2);

  // fRun->LoadGeometry();


  // -----   Digitization   ---------------------------------------
//  PndMvdDigiTask* mvddigi = new PndMvdDigiTask();
//   mvddigi->SetVerbose(verboseLevel);
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

  PndLmdHybridHitProducer* lmdStripProd = new PndLmdHybridHitProducer();
  //  PndLmdStripHitProducerDif* ssdStripProd = new PndLmdStripHitProducerDif(); //TEST
  lmdStripProd->SetVerbose(verboseLevel);
  fRun->AddTask(lmdStripProd);
  // -----    MVD Pixel hit producer   ---------------------------------------
//   Double_t  lx=0.01, ly=0.01, threshold=600, noise=200;
//   PndMvdHybridHitProducer* mvdPixProd = new PndMvdHybridHitProducer(lx,ly,threshold,noise);
 // PndSsdHybridHitProducer* ssdPixProd = new PndSsdHybridHitProducer();
 // ssdPixProd->SetVerbose(verboseLevel);
 // fRun->AddTask(ssdPixProd);

  // PndLmdNoiseProducer* lmdNoiseMaker = new PndLmdNoiseProducer();
  // lmdNoiseMaker->SetVerbose(verboseLevel);
  // fRun->AddTask(lmdNoiseMaker);


//   FairParRootFileIo* output=new FairParRootFileIo(kTRUE);
//   output->open(parOutFile.Data());
//   rtdb->setOutput(output);
  rtdb->setOutput(parInput1);
  rtdb->print();
  // =====                 End of HitProducers                           =====
  // =========================================================================
  PndSdsGeoPar* geoPar  = (PndSdsGeoPar*)(rtdb->getContainer("PndSdsGeoPar")); 
  
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
