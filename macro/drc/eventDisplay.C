eventDisplay()
{
   // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  gSystem->Load("libEve");
  gSystem->Load("libEventDisplay");

                                     
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(" testrun1.root");
  fRun->SetOutputFile("tst.root");
 
  
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open("./params_testrun1.root");
       
  rtdb->setFirstInput(parInput1);
 

  FairEventManager *fMan= new FairEventManager();
  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
  FairMCPointDraw *PndBarPoint = new FairMCPointDraw ("DrcBarPoint",kViolet, kFullSquare);
  FairMCPointDraw *PndPdPoint = new FairMCPointDraw ("DrcPDPoint",kBlue, kFullSquare);

                                                               
  fMan->AddTask(Track);
  fMan->AddTask( PndBarPoint);
  fMan->AddTask( PndPdPoint);

    
// fRun->Init();
 fMan->Init();                     
  

}
