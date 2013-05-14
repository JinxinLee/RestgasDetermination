eventdisplay()
{
 gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
//   gROOT->Macro("$VMCWORKDIR/macro/mvd/Libs.C");
  gSystem->Load("libGem");
  gSystem->Load("libEve");
  gSystem->Load("libEventDisplay");

  // -----   Reconstruction run   -------------------------------------------
  TString parFile = "GemParams.root";
  std::string simFile = "Gem_Test.root";;
//   std::string recoFile = namecreator.GetRecoFileName();
//   std::string trackingFile = namecreator.GetTrackFindingFileName();
  std::string outFile = "EVEout.root";

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(simFile.c_str());
//   fRun->AddFriend(recoFile.c_str());
//   fRun->AddFriend(trackingFile.c_str());
  fRun->SetOutputFile(outFile.c_str());

  fRun->LoadGeometry();



  FairEventManager *fMan= new FairEventManager();

  // --- MC ---
  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");

//   FairMCPointDraw *MvdPoints =   new FairMCPointDraw ("MVDPoint",kBlue,  kFullSquare);
  FairMCPointDraw *GemPoints =   new FairMCPointDraw ("GEMPoint",kBlue,  kFullSquare);
//  FairMCPointDraw *EMCPoints =   new FairMCPointDraw ("EmcPoint",kOrange,  kFullSquare);
//  FairMCPointDraw *TofPoint =    new FairMCPointDraw ("TofPoint",kYellow,  kFullSquare);
//  FairMCPointDraw *TofSciFPoint= new FairMCPointDraw ("TofSciFPoint",kTeal, kFullSquare);
//  FairMCPointDraw *MuoPoint =    new FairMCPointDraw ("MuoPoint",kAzure, kFullSquare);
//  FairMCPointDraw *PndDrcPoint = new FairMCPointDraw ("PndDrcPoint",kViolet, kFullSquare);
//   FairMCPointDraw *PndTpcPoint = new FairMCPointDraw ("PndTpcPoint",kCyan,  kFullSquare);
//  FairMCPointDraw *PndSTTPoint = new FairMCPointDraw ("STTPoint",kMagenta, kFullSquare);

  fMan->AddTask(Track);

//   fMan->AddTask(MvdPoints);
  fMan->AddTask(GemPoints);
//  fMan->AddTask(EMCPoints);
//  fMan->AddTask(TofPoint);
//  fMan->AddTask( TofSciFPoint);
//  fMan->AddTask( MuoPoint);
//  fMan->AddTask( PndDrcPoint);
//   fMan->AddTask( PndTpcPoint);
//  fMan->AddTask( PndSTTPoint);



// --- RECO ---
//   FairRecoPointDraw *pixelHits = new FairRecoPointDraw("MVDHitsPixel");
//   fMan->AddTask(pixelHits);
// 
//   FairRecoPointDraw *stripHits = new FairRecoPointDraw("MVDHitsStrip");
//   fMan->AddTask(stripHits);
// 
//   FairTPCRecoPointDraw *tpcHits = new FairTPCRecoPointDraw("PndTpcCluster");
//   fMan->AddTask(tpcHits);

//  FairTrackCandDraw *TrkCands = new FairTrackCandDraw ("LhePidTrack");
//   FairTrackCandDraw *TrkCands = new FairTrackCandDraw ("MVD_TPC_CombinedRiemannTrack");
//  FairTrackCandDraw *TrkCands = new FairTrackCandDraw ("MVDRiemannTrackCand");
//   fMan->AddTask(TrkCands);



  fRun->Init();
  fMan->Init();


}
