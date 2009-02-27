{
	gSystem->Load("Libs.C");
	gSystem->Load("libEventDisplay");
	fRun->SetInputFile("MVD_D+D-_wMag.root");
	fRun->LoadGeomentry();
	
	FairEventManager *fMan = new FairEventManager();
	FairMCTracks *Track = new FairMCTracks("MCTrack");
	FairMCPointDraw *MvdPoints = new FairMCPointDraw("MVDPoint");
	
	fMan->AddTask(Track);
	fMan->AddTask(MvdPoints);
	
	fMan->Init();
}