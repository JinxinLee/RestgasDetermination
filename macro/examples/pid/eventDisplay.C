

eventDisplay()
{

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  gSystem->Load("libEve");
  gSystem->Load("libEventDisplay");
  gSystem->Load("libPndEventDisplay");

  gStyle->SetPalette(1);

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();

  //TString MCFile = "./data/Mvd_Test.root";
  TString MCFile = "points_sttcombi.root";
  TString DigiFile = "digi_sttcombi.root";
  TString RecoFile = "reco_sttcombi.root";
  TString parFile = "params_sttcombi.root";


  fRun->SetInputFile(MCFile.Data());
  fRun->AddFriend(DigiFile.Data());
  fRun->AddFriend(RecoFile.Data());

  fRun->SetOutputFile("test.root");


  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);

  FairEventManager *fMan= new FairEventManager();

  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);

  // --- MC ---
  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");

  FairMCPointDraw *MvdPoints =   	new FairMCPointDraw ("MVDPoint", kRed, kFullSquare);
  FairMCPointDraw *SttPoints = 		new FairMCPointDraw("STTPoint", kBlue, kFullSquare);
  FairMCPointDraw *GEMPoint = 		new FairMCPointDraw ("GEMPoint", kGreen, kFullSquare);
  FairMCPointDraw *MdtPoint =		new FairMCPointDraw ("MdtPoint", kYellow, kFullSquare);
  FairMCPointDraw *TofSciFPoint =		new FairMCPointDraw ("SciTPoint", kBlue, kFullSquare);
  FairMCPointDraw *DrcBarPoint =	new FairMCPointDraw ("DrcBarPoint", kBlue, kFullSquare);
  
  FairHitDraw	*EmcHit = new FairHitDraw ("EmcHit");
  FairHitDraw	*DrcHit = new FairHitDraw("DrcHit");
  FairHitDraw	*GEMHit = new FairHitDraw("GEMHit");
  FairHitDraw	*MvdHitsPixel = new FairHitDraw("MVDHitsPixel");
  FairHitDraw	*MvdHitsStrip = new FairHitDraw("MVDHitsStrip");
  FairHitDraw	*MdtHit = new FairHitDraw("MdtHit");
  FairHitDraw	*SttHit = new FairHitDraw("STTHit");
  FairHitDraw	*TofSciFHit = new FairHitDraw("SciTHit");

  PndTrackDraw *kalmanTrack = new PndTrackDraw("SttMvdGemGenTrackCand");


  fMan->AddTask(Track);
  fMan->AddTask(MvdPoints);
  fMan->AddTask(SttPoints);
  fMan->AddTask(GEMPoint);
  fMan->AddTask(MdtPoint);
  fMan->AddTask(TofSciFPoint);
  fMan->AddTask(DrcBarPoint);
  fMan->AddTask(DrcHit);
  fMan->AddTask(GEMHit);
  fMan->AddTask(MvdHitsPixel);
  fMan->AddTask(MvdHitsStrip);
  fMan->AddTask(MdtHit);
  fMan->AddTask(SttHit);
  fMan->AddTask(TofSciFHit);

  fMan->AddTask(kalmanTrack);	// if PndTrackDraw is chosen it will be very slow
  fMan->AddTask(EmcHit);

  fMan->Init();


}
