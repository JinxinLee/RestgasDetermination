void runLumiPixel4bNoiseTrkFilter(const int nEvents=100000, const int startEvent=0, TString storePath="tmpOutput", const int verboseLevel=0, const double mom=1.5)
{
// ========================================================================
// Input Track file
  TString TrkFile = storePath+"/Lumi_Track_";
  TrkFile += startEvent;
  TrkFile += ".root";

  // Output file
  TString outFile = storePath+"/Lumi_TrackFiltered_";
  outFile += startEvent;
  outFile += ".root";

  std::cout << "TrackInFile: " << TrkFile.Data()<< std::endl;
  std::cout << "TrackOutFile: " << outFile.Data()<< std::endl;

 // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(TrkFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // ======================================
  // ======   Noise hits Track Filter                     ======
  // ======================================
  Double_t fpBeam = mom;
 //  TString dir = "TMVAweights_noise_hits/weights_";//trained on samples where noise produced in 1 readout frame
  TString dir = "TMVAweights_noise_hits/weights_3fr_mom";//trained on samples where noise produced in 3 readout frames
  if(fabs(fpBeam-1.5)<0.1) dir+="_1_5/";
  else
    if(fabs(fpBeam-15)<0.1) dir+="_15/";
    else
      cout<<"!! Attention! There is no trained TMVA method for momentum "<<mom<<end;
  
  PndLmdNoiseTrkSuppressionTask* lmdfilt = new PndLmdNoiseTrkSuppressionTask(fpBeam, dir);
  lmdfilt->SetVerbose(verboseLevel);
  fRun->AddTask(lmdfilt);
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(startEvent,nEvents);
  cout<<"fRun->Run(0, "<<nEvents<<") was done!"<<endl;
  // ------------------------------------------------------------------------
}
