

eventDisplay()
{
	// Load basic libraries
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();
	gSystem->Load("libEve");
	gSystem->Load("libEventDisplay");
	gSystem->Load("libFtof");

	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *fRun= new FairRunAna();
	fRun->SetInputFile("test.root");//sim_pidC.root");
	fRun->AddFriend("hit_ftof.root");
	fRun->SetOutputFile("tst.root");



	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	FairParRootFileIo* parInput1 = new FairParRootFileIo();
	parInput1->open("simparams.root");

	rtdb->setFirstInput(parInput1);


	FairEventManager *fMan= new FairEventManager();
	FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
	FairMCPointDraw *PndFTSPoint = new FairMCPointDraw ("FTSPoint",kMagenta, kFullSquare);
	FairMCPointDraw *FtofPoint =    new FairMCPointDraw ("FtofPoint",kYellow,  kFullSquare);
	FairHitDraw *FTSHit = new FairHitDraw("FTSHit");
	FairHitDraw *FtofHit = new FairHitDraw("FtofHit");


	fMan->AddTask(Track);

	fMan->AddTask(PndFTSPoint);
	fMan->AddTask(FtofPoint);
	fMan->AddTask(FTSHit);
	fMan->AddTask(FtofHit);

	fMan->Init();
	gGeoManager->SetVisLevel(0);

	TGeoVolume* top = gGeoManager->GetTopVolume();

}
