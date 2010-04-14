

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
   FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
   parInput1->open(parFile.Data(),"UPDATE");
   rtdb->setFirstInput(parInput1);


  FairEventManager *fMan= new FairEventManager();

  FairGeane *Geane = new FairGeane();
    fRun->AddTask(Geane);

    fRun->Init();
  // --- MC ---
  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");

  FairMCPointDraw *MvdPoints =   	new FairMCPointDraw ("MVDPoint", kRed, kFullSquare);
  FairMCPointDraw *SttPoints = 		new FairMCPointDraw("STTPoint", kBlue, kFullSquare);
  //FairMCPointDraw	*EmcPoints =	new FairMCPointDraw ("EMCPoint", kGreen, kFullSquare);
  FairMCPointDraw *GEMPoint = 		new FairMCPointDraw ("GEMPoint", kGreen, kFullSquare);
  FairMCPointDraw *MdtPoint =		new FairMCPointDraw ("MdtPoint", kYellow, kFullSquare);
  FairMCPointDraw *TofPoint =		new FairMCPointDraw ("TofPoint", kBlue, kFullSquare);
  FairMCPointDraw *DrcBarPoint =	new FairMCPointDraw ("DrcBarPoint", kBlue, kFullSquare);
  FairHitDraw	  *EmcHit =			new FairHitDraw ("EmcHit");

//  PndMvdDigiPixelDraw* MvdDigis = new PndMvdDigiPixelDraw("MVDPixelDigis");

  FairHitDraw	*DrcHit = new FairHitDraw("DrcHit");
  FairHitDraw	*GEMHit = new FairHitDraw("GEMHit");
  FairHitDraw	*MvdHitsPixel = new FairHitDraw("MvdHitsPixel");
  FairHitDraw	*MvdHitsStrip = new FairHitDraw("MvdHitsStrip");
  FairHitDraw	*MdtHit = new FairHitDraw("MdtHit");
  FairHitDraw	*SttHit = new FairHitDraw("SttHit");
  FairHitDraw	*SttHelixHit = new FairHitDraw("SttHelixHit");
  FairHitDraw	*TofHit = new FairHitDraw("TofHit");

  PndTrackDraw *kalmanTrack = new PndTrackDraw("LheGenTrack"); // if PndTrackDraw is chosen it will be very slow



  fMan->AddTask(Track);
  fMan->AddTask(MvdPoints);
  fMan->AddTask(SttPoints);
  fMan->AddTask(GEMPoint);
  fMan->AddTask(MdtPoint);
  fMan->AddTask(TofPoint);
  fMan->AddTask(DrcBarPoint);

 // fMan->AddTask(MvdDigis);

  fMan->AddTask(DrcHit);
  fMan->AddTask(GEMHit);
  fMan->AddTask(MvdHitsPixel);
  fMan->AddTask(MvdHitsStrip);
  fMan->AddTask(MdtHit);
  fMan->AddTask(SttHit);
  fMan->AddTask(SttHelixHit);
  fMan->AddTask(TofHit);

  fMan->AddTask(kalmanTrack);
  //fMan->AddTask(EmcHit);

 // fMan->AddTask(MvdRecoPoints);
 // fMan->AddTask(MvdStripRecoPoints);
 // fMan->AddTask(MvdDigis);
 // fMan->AddTask(MvdRecoPoints);
 // fMan->AddTask(MvdStripRecoPoints);
//  fMan->AddTask(SttHelixHits);
//  fMan->AddTask(MvdTrackCand);
//  fMan->AddTask(MvdRiemannTrackCand);


  fMan->Init();


}
