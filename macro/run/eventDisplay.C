

eventDisplay()
{
    //-----User Settings:-----------------------------------------------
  TString  SimEngine      ="TGeant3"; 
  TString  InputFile     ="sim_complete.root";
  TString  DigiFile		 ="digi_complete.root";
  TString  RecoFile      ="reco_complete.root";
  TString  ParFile       ="simparams.root";

  Bool_t enablePointDraw = kTRUE;
  Bool_t enableHitDraw = kTRUE;
  Bool_t enableTrackDraw = kTRUE;
  //------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(InputFile.Data());
  fRun->SetOutputFile("tst.root");

  TFile* testFile;
  testFile = new TFile(DigiFile.Data());
  if (!testFile->IsZombie()){
	  fRun->AddFriend(DigiFile.Data());
  }
  else {
	  enableHitDraw = kFALSE;
  }
  testFile->Close();

  testFile = new TFile(RecoFile.Data());
  if (!testFile->IsZombie()){
	  fRun->AddFriend(RecoFile.Data());
	  FairGeane *Geane = new FairGeane();
	  fRun->AddTask(Geane);
  }
  else {
	  enableTrackDraw = kFALSE;
  }
  testFile->Close();

   FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(ParFile.Data());
  rtdb->setFirstInput(parInput1);
  FairEventManager *fMan= new FairEventManager();
 
 
 //----------------------Traks and points -------------------------------------
  if (enablePointDraw) {
	  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
	  FairMCPointDraw *MvdPoints =   new FairMCPointDraw ("MVDPoint",kBlue,  kFullSquare);
	  FairHitDraw *EMCPoints =   new FairHitDraw ("EmcHit");
	  PndEmcHitDraw *EMCPoints2 = new PndEmcHitDraw("EmcHit");
		EMCPoints->SetBoxDimensions(2.,2.,2.);
		EMCPoints2->SetBoxDimensions(2.,2.,2.);
	  FairMCPointDraw *TofSciFPoint= new FairMCPointDraw ("SciTPoint",kTeal, kFullSquare);
	  FairMCPointDraw *MdtPoint =    new FairMCPointDraw ("MdtPoint",kAzure, kFullSquare);
	  FairMCPointDraw *PndDrcBarPoint = new FairMCPointDraw ("DrcBarPoint",kGreen, kFullSquare);
	  FairMCPointDraw *PndDrcPDPoint = new FairMCPointDraw ("DrcPDPoint",kViolet, kFullSquare);
	  FairMCPointDraw *PndDskParticle = new FairMCPointDraw ("DskParticle",kYellow, kFullSquare);
	  FairMCPointDraw *PndDskFLGHit = new FairMCPointDraw ("PndDskFLGHit",kPink, kFullSquare);
	  FairMCPointDraw *PndSTTPoint = new FairMCPointDraw ("STTPoint",kMagenta, kFullSquare);
	  FairMCPointDraw *PndGEMPoint = new FairMCPointDraw ("GEMPoint",kRed, kFullSquare);
	  FairMCPointDraw *PndFTSPoint = new FairMCPointDraw ("FTSPoint",kMagenta, kFullSquare);
	  FairMCPointDraw *PndFtofPoint = new FairMCPointDraw ("FtofPoint",kGreen, kFullSquare);

	  PndEmcHitCaloDraw *EmcHitCalo = new PndEmcHitCaloDraw("EmcHit");

	  fMan->AddTask(Track);
	  fMan->AddTask(MvdPoints);
	  fMan->AddTask(EMCPoints);
		fMan->AddTask(EMCPoints2);
	  fMan->AddTask( TofSciFPoint);
	  fMan->AddTask( MdtPoint);
	  fMan->AddTask( PndDrcBarPoint);
	  fMan->AddTask( PndDrcPDPoint);
	  fMan->AddTask( PndDskParticle);
	  fMan->AddTask( PndDskFLGHit);
	  fMan->AddTask( PndSTTPoint);
	  fMan->AddTask( PndGEMPoint);
	  fMan->AddTask( PndFTSPoint);
	  fMan->AddTask( PndFtofPoint);
	  fMan->AddTask(EmcHitCalo);

  }

  //--------------- Hits ----------------------

  if (enableHitDraw) {
	  FairHitDraw *MvdRecoHit =   new FairHitDraw ("MVDHitsPixel");
	  FairHitDraw *MvdRecoStrip = new FairHitDraw ("MVDHitsStrip");
	  FairHitDraw *STTHits = new FairHitDraw ("STTHit");
	  PndSttIsochroneDraw* STTIsochrone = new PndSttIsochroneDraw("STTHit");
//	  STTIsochrone->UseIsochroneTime();
	  FairHitDraw *SciTHit = new FairHitDraw("SciTHit");
	  FairHitDraw *MdtHit = new FairHitDraw("MdtHit");
	  FairHitDraw *DrcHit = new FairHitDraw("DrcHit");
	  FairHitDraw *DrcPDHit = new FairHitDraw("DrcPDHit");
	  FairHitDraw *GEMHit = new FairHitDraw("GEMHit");
	  FairHitDraw *FTSHit = new FairHitDraw("FTSHit");
	  FairHitDraw *FtofHit = new FairHitDraw("FtofHit");
	  PndEmcClusterDraw *EmcBump = new PndEmcClusterDraw("EmcBump");
	  EmcBump->SetBoxDimensions(4.0,4.0,4.0);
	  fMan->AddTask(MvdRecoHit);
	  fMan->AddTask(MvdRecoStrip);
	  fMan->AddTask(STTHits);
	  fMan->AddTask(STTIsochrone);
	  fMan->AddTask(SciTHit);
	  fMan->AddTask(MdtHit);
	  fMan->AddTask(DrcHit);
	  fMan->AddTask(DrcPDHit);
	  fMan->AddTask(GEMHit);
	  fMan->AddTask(FTSHit);
	  fMan->AddTask(FtofHit);
	  fMan->AddTask(EmcBump);
  }

  if (enableTrackDraw) {
	  PndTrackDraw* SttMvdTrack = new PndTrackDraw("SttMvdTrack");
	  PndTrackDraw* SttMvdGemTrack = new PndTrackDraw("SttMvdGemTrack", kTRUE);
	  PndTrackDraw* FtsIdealTrack = new PndTrackDraw("FtsIdealTrack");
      PndTrackDraw* SttMvdGemGenTrack = new PndTrackDraw("SttMvdGemGenTrack", kTRUE);

	  fMan->AddTask(SttMvdTrack);
	  fMan->AddTask(SttMvdGemTrack);
      fMan->AddTask(SttMvdGemGenTrack);
	  fMan->AddTask(FtsIdealTrack);
  }



  
  fMan->Init();                     

}
