
{
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  gSystem->Load("libEve");
  gSystem->Load("libEventDisplay");
  
  Int_t verbose = 1;
  FairRunAna *fRun= new FairRunAna();

  fRun->SetInputFile("lumisim.root");
  fRun->SetOutputFile("lumieve.root");

  fRun->LoadGeometry();


  FairEventManager *fMan= new FairEventManager();

  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks",verbose);

  FairMCPointDraw *LumiPoint = new FairMCPointDraw ("LumiPoint",kRed, kFullSquare, verbose);

  fMan->AddTask(Track);

  fMan->AddTask( LumiPoint);

  fRun->Init();
  fMan->Init();

}
