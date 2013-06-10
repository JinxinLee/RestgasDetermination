eventDisplay()
{
  // Load libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  gSystem->Load("libEve");
  gSystem->Load("libEventDisplay");
  gSystem->Load("libPndEventDisplay");
                                     
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile("sim.root");
  TString digifile = "digi.root";  
  //TString digifile = "digiG410k.root"; 
  fRun->AddFriend(digifile);
  fRun->SetOutputFile("tst.root");
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open("par.root");      
  rtdb->setFirstInput(parInput1);
 
  FairEventManager *fMan= new FairEventManager();
  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
  FairMCPointDraw *PndBarPoint = new FairMCPointDraw ("DrcBarPoint",kViolet, kFullSquare);
  FairMCPointDraw *PndPdPoint = new FairMCPointDraw ("DrcPDPoint",kBlue, kFullSquare);
  PndDrcAccuDigiPixelDraw *PndAccuDigi = new PndDrcAccuDigiPixelDraw ("DrcPDHit",digifile,10.); //0.05
                                                               
  fMan->AddTask(Track);
  fMan->AddTask(PndBarPoint);
  fMan->AddTask(PndPdPoint);
  fMan->AddTask(PndAccuDigi);
   
  fMan->Init();                     
 
}
