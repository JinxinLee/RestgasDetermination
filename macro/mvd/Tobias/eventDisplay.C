

eventDisplay()
{

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  gSystem->Load("libEve");
  gSystem->Load("libEventDisplay");

  gStyle->SetPalette(1);

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();

  //TString MCFile = "./data/Mvd_Test.root";
  TString MCFile = "MvdTpc_D+D-_10G_1000.root";

//  PndFileNameCreator creator(MCFile.Data());
  //TString RecoFile = creator.GetRecoFileName(false).c_str();
//  TString TFindFile  = creator.GetTrackFindingFileName(false).c_str();

 // TString TFindFile  = creator.GetIdealTrackFindingFileName(false).c_str();
 // TString DigiFile = creator.GetDigiFileName(false).c_str();

  fRun->SetInputFile(MCFile.Data());
//  fRun->AddFriend(DigiFile.Data());
//  fRun->AddFriend(RecoFile.Data());
//  fRun->AddFriend(TFindFile.Data());
  //fRun->SetInputFile("../dsk/sim_dsk.root");
 // fRun->SetInputFile("../dsk/sim_dsk.g4native.root");
  fRun->SetOutputFile("test.root");
  fRun->LoadGeometry();

  FairEventManager *fMan= new FairEventManager();

  // --- MC ---
  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");

  FairMCPointDraw *MvdPoints =   new FairMCPointDraw ("MVDPoint", kRed, kFullSquare);
//  PndSttMCPointDraw *SttPoints = new PndSttMCPointDraw("STTPoint", kRed, kFullSquare);

//  FairRiemannPointDraw *MvdRiemann = new FairRiemannPointDraw("MVDHitsPixel");
//  FairRiemannPointDraw *MvdRiemannStrip = new FairRiemannPointDraw("MVDHitsStrip");

//  FairHitDraw *MvdRecoPoints =   new FairHitDraw ("MVDHitsPixel");
//  FairHitDraw *MvdStripRecoPoints = new FairHitDraw("MVDHitsStrip");

 // FairHitDraw *SttHelixHits = new FairHitDraw("SttHelixHit");

//  PndMvdDigiPixelDraw* MvdDigis = new PndMvdDigiPixelDraw("MVDPixelDigis");

//  FairTrackCandDraw* MvdTrackCand = new FairTrackCandDraw("MVDIdealTrackCand", 3);
 // FairRiemannTrackCandDraw* MvdRiemannTrackCand = new FairRiemannTrackCandDraw("MVDIdealTrackCand", 3);


  fMan->AddTask(Track);
  fMan->AddTask(MvdPoints);
 // fMan->AddTask(SttPoints);
 // fMan->AddTask(MvdRiemann);
//  fMan->AddTask(MvdRiemannStrip);
 // fMan->AddTask(MvdDigis);
 // fMan->AddTask(MvdRecoPoints);
 // fMan->AddTask(MvdStripRecoPoints);
//  fMan->AddTask(SttHelixHits);
//  fMan->AddTask(MvdTrackCand);
//  fMan->AddTask(MvdRiemannTrackCand);

  fRun->Init();
  fMan->Init();

/*  FairEventManager *fMan= new FairEventManager();
  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
  FairMCPointDraw *MvdMCPoints =   new FairMCPointDraw ("MVDPoint", 1,7);
  FairRecoPointDraw *MvdRecoPoints =   new FairRecoPointDraw ("MVDHitsPixel",3);
  FairRecoPointDraw *MvdStripRecoPoints = new FairRecoPointDraw("MVDHitsStrip",3);
  FairTrackCandDraw* MvdTrackCand = new FairTrackCandDraw("MVDRiemannTrackCand", 3);
//  FairMCPointDraw *EMCPoints =   new FairMCPointDraw ("EmcPoint");
//  FairMCPointDraw *TofPoint =    new FairMCPointDraw ("TofPoint");
//  FairMCPointDraw *TofSciFPoint= new FairMCPointDraw ("TofSciFPoint");
//  FairMCPointDraw *MuoPoint =    new FairMCPointDraw ("MuoPoint");
//  FairMCPointDraw *PndDrcPoint = new FairMCPointDraw ("PndDrcPoint");
//  FairMCPointDraw *PndDchPoint = new FairMCPointDraw ("PndDchPoint");
//  FairMCPointDraw *PndTpcPoint = new FairMCPointDraw ("PndTpcPoint",3,7);
 // FairMCPointDraw *PndSTTPoint = new FairMCPointDraw ("STTPoint");


  fMan->AddTask(Track);

  fMan->AddTask(MvdMCPoints);
  fMan->AddTask(MvdRecoPoints);
  fMan->AddTask(MvdStripRecoPoints);
//  fMan->AddTask(MvdTrackCand);
// fMan->AddTask(EMCPoints);
// fMan->AddTask(TofPoint);
// fMan->AddTask( TofSciFPoint);
//  fMan->AddTask( MuoPoint);
//  fMan->AddTask( PndDrcPoint);
 // fMan->AddTask( PndDchPoint);
//  fMan->AddTask( PndTpcPoint);
 // fMan->AddTask( PndSTTPoint);*/


}
