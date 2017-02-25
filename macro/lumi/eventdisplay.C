
int eventdisplay()
{
	//-----User Settings:-----------------------------------------------
	TString  SimEngine      ="TGeant4";
	TString  InputFile     ="test_G4.root";
	TString  ParFile       ="params_G4.root";
	//------------------------------------------------------------------

	// Load basic libraries
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();
	gSystem->Load("libEve");
	gSystem->Load("libEventDisplay");


	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *fRun= new FairRunAna();
	fRun->SetInputFile(InputFile.Data());
	fRun->SetOutputFile("evtdis.root");

	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	FairParRootFileIo* parInput1 = new FairParRootFileIo();
	parInput1->open(ParFile.Data());
	rtdb->setFirstInput(parInput1);

	FairEventManager *fMan= new FairEventManager();

	//----------------------Traks and points -------------------------------------
	FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
	FairMCPointDraw *LmdPoint = new FairMCPointDraw ("LumiPoint",kGreen, kFullSquare);


//	TEveElement *model=	fMan->GetVizModel();
	fMan->AddTask(Track);
	fMan->AddTask(LmdPoint);

	fMan->Init();
  return 0;
}
