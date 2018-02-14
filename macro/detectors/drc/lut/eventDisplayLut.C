int eventDisplayLut()
{
  gSystem->Load("libEve");
  gSystem->Load("libEventDisplay");
  //gSystem->Load("libPndEventDisplay");

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile("simlut.root");
  TString digifile = "digilut.root";  
  fRun->AddFriend(digifile);
  fRun->SetOutputFile("tst.root");
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open("parlut.root");      
  rtdb->setFirstInput(parInput1);

  FairEventManager *fMan= new FairEventManager();
  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
  FairMCPointDraw *PndBarPoint = new FairMCPointDraw ("DrcBarPoint",kViolet, kFullSquare);
  FairMCPointDraw *PndPdPoint = new FairMCPointDraw ("DrcPDPoint",kBlue, kFullSquare);
  PndDrcAccuDigiPixelDraw *PndAccuDigi = new PndDrcAccuDigiPixelDraw ("DrcPDHit",digifile,5.); //0.05
  
  fMan->AddTask(Track);
  fMan->AddTask(PndBarPoint);
  fMan->AddTask(PndPdPoint);
  fMan->AddTask(PndAccuDigi);

  fMan->Init(1,5);                     
 
  return 0;
}
