

eventDisplay()
{
    //-----User Settings:-----------------------------------------------
  TString  SimEngine      ="TGeant3"; 
  TString  InputFile     ="llbar_sim_evtgen.root";
  TString  ParFile       ="llbar_params_evtgen.root";
  //------------------------------------------------------------------

// Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  gSystem->Load("libEve");
  gSystem->Load("libEventDisplay");
  gSystem->Load("libPndEventDisplay");

  PndFileNameCreator creator(InputFile.Data());
  TString digiFile = creator.GetDigiFileName();
  TString recoFile = creator.GetRecoFileName();
  TString trackF = creator.GetTrackFindingFileName();

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(InputFile.Data());
  fRun->AddFriend("llbar_digi_evtgen.root");
  fRun->AddFriend("llbar_reco_evtgen.root");
 // fRun->AddFriend(trackF.Data());
  fRun->SetOutputFile("tst.root");

   FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(ParFile.Data());
  rtdb->setFirstInput(parInput1);
  FairEventManager *fMan= new FairEventManager();

  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);
//
 //     fRun->Init();
 
 // SetPalette(1);

 //----------------------Traks and points -------------------------------------
//  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
  FairMCTracks *Track =  new FairMCTracks ("MCTrack");
  FairMCPointDraw *MvdPoints =   new FairMCPointDraw ("MVDPoint",kBlue,  kFullSquare);
  FairMCPointDraw *EMCPoints =   new FairMCPointDraw ("EmcHit",kOrange,  kFullSquare);
  FairMCPointDraw *TofPoint =    new FairMCPointDraw ("TofPoint",kYellow,  kFullSquare);
//  FairMCPointDraw *TofSciFPoint= new FairMCPointDraw ("TofSciFPoint",kTeal, kFullSquare);
//  FairMCPointDraw *MdtPoint =    new FairMCPointDraw ("MdtPoint",kAzure, kFullSquare);
//  FairMCPointDraw *PndDrcPoint = new FairMCPointDraw ("PndDrcPoint",kViolet, kFullSquare);
//  FairMCPointDraw *PndDchPoint = new FairMCPointDraw ("PndDchPoint",kPink, kFullSquare);
//  FairMCPointDraw *PndTpcPoint = new FairMCPointDraw ("PndTpcPoint",kCyan,  kFullSquare);
  FairMCPointDraw *PndSTTPoint = new FairMCPointDraw ("STTPoint",kMagenta, kFullSquare);
  FairMCPointDraw *PndGEMPoint = new FairMCPointDraw ("GEMPoint",kRed, kFullSquare);
//  FairMCPointDraw *PndDskPoint = new FairMCPointDraw ("DskCerenkov",kGreen, kFullSquare);

  PndTrackCandDraw* SttTrack = new PndTrackCandDraw ("STTTrackCand");
//
  PndTrackCandDraw* RiemannCand = new PndTrackCandDraw("MVDRiemannTrackCand");
  PndTrackDraw* PndTrackRiemann = new PndTrackDraw("MVDTrack");
//  PndRiemannTrackDraw* RiemannTrack = new PndRiemannTrackDraw("MVDRiemannTrack");
//PndMvdDigiPixelDraw* MvdDigiPixel = new PndMvdDigiPixelDraw("MVDPixelDigis");

  PndTrackDraw* KalmanCand = new PndTrackDraw("SttMvdGemGenTrack");

  FairHitDraw *EMCRecoHit = new FairHitDraw("EmcRecoHit");
  FairHitDraw *MvdRecoHit =   new FairHitDraw ("MVDHitsPixel");
  FairHitDraw *MvdRecoStrip = new FairHitDraw ("MVDHitsStrip");


  /////////////////////////////////////////////////////////////////////////////////////////////////////////////

  fMan->AddTask(Track);
  fMan->AddTask(MvdPoints);
//  fMan->AddTask(MvdDigiPixel);
  fMan->AddTask(EMCPoints);
  fMan->AddTask(TofPoint);
//  fMan->AddTask( TofSciFPoint);
//  fMan->AddTask( MdtPoint);
//  fMan->AddTask( PndDrcPoint);
//  fMan->AddTask( PndDchPoint);
//  fMan->AddTask( PndTpcPoint);
  fMan->AddTask( PndSTTPoint);
  fMan->AddTask( PndGEMPoint);
//  fMan->AddTask( PndDskPoint);
//
  fMan->AddTask(EMCRecoHit);
  fMan->AddTask(MvdRecoHit);
  fMan->AddTask(MvdRecoStrip);
  fMan->AddTask(KalmanCand);
  fMan->AddTask(RiemannCand);
  fMan->AddTask(PndTrackRiemann);
  fMan->AddTask(SttTrack);

  fMan->Init();

}
