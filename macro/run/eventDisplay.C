

eventDisplay()
{
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();  
  gSystem->Load("libEve");
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libGen");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libStt"); 
  gSystem->Load("libMdt");
  gSystem->Load("libEmc");  
  gSystem->Load("libTof");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");
  gSystem->Load("libtpc");
  gSystem->Load("libgenfit");
  gSystem->Load("libDch");
  gSystem->Load("libMvd");
  gSystem->Load("libHyp");
// gSystem->Load("libDsk");
  gSystem->Load("libEventDisplay");
  gSystem->Load("libGeom");
                                     
  // -----   Reconstruction run   -------------------------------------------
  CbmRunAna *fRun= new CbmRunAna();
  //fRun->SetInputFile("points.x3872.jpsipipi.phsp.root");
  fRun->SetInputFile("testsimu10.root");
  
  //fRun->SetInputFile("../dsk/sim_dsk.root");
 // fRun->SetInputFile("../dsk/sim_dsk.g4native.root");
  fRun->SetOutputFile("test.root");
  fRun->LoadGeometry();

  FairEventManager *fMan= new FairEventManager();
  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
  FairMCPointDraw *MvdPoints =   new FairMCPointDraw ("MVDPoint",kBlue,  kFullSquare);
  FairMCPointDraw *EMCPoints =   new FairMCPointDraw ("EmcPoint",kOrange,  kFullSquare);
  FairMCPointDraw *TofPoint =    new FairMCPointDraw ("TofPoint",kYellow,  kFullSquare);
  FairMCPointDraw *TofSciFPoint= new FairMCPointDraw ("TofSciFPoint",kTeal, kFullSquare);
  FairMCPointDraw *MuoPoint =    new FairMCPointDraw ("MuoPoint",kAzure, kFullSquare);
  FairMCPointDraw *PndDrcPoint = new FairMCPointDraw ("PndDrcPoint",kViolet, kFullSquare);
  FairMCPointDraw *PndDchPoint = new FairMCPointDraw ("PndDchPoint",kPink, kFullSquare);
  FairMCPointDraw *PndTpcPoint = new FairMCPointDraw ("PndTpcPoint",kCyan,  kFullSquare);
 // FairMCPointDraw *PndSTTPoint = new FairMCPointDraw ("STTPoint",kMagenta, kFullSquare);

                                                               
  fMan->AddTask(Track);
  
  fMan->AddTask(MvdPoints);
  fMan->AddTask(EMCPoints);   
  fMan->AddTask(TofPoint);   
  fMan->AddTask( TofSciFPoint);
  fMan->AddTask( MuoPoint);
  fMan->AddTask( PndDrcPoint);
  fMan->AddTask( PndDchPoint);
  fMan->AddTask( PndTpcPoint);
 // fMan->AddTask( PndSTTPoint);

    
  fMan->Init();                     
  

}
