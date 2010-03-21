

eventDisplay()
{
    //-----User Settings:-----------------------------------------------
  TString  SimEngine      ="TGeant3"; 
  TString  InputFile     ="HCalMC.root";
  TString  ParFile       ="HCalMC_Params.root";
  //------------------------------------------------------------------


// Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  gSystem->Load("libEve");
  gSystem->Load("libEventDisplay");
                                     
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(InputFile.Data());
  fRun->SetOutputFile("tst.root");

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(ParFile.Data());
  rtdb->setFirstInput(parInput1);
  FairEventManager *fMan= new FairEventManager();
 
 
 //----------------------Traks and points -------------------------------------
  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
  FairMCPointDraw *MvdPoints =   new FairMCPointDraw ("MVDPoint",kBlue,  kFullSquare);


  fMan->AddTask(Track);
  fMan->AddTask(MvdPoints);
  
  fMan->Init();                     

  gGeoManager->DefaultColors(); // repair buggy coloring

  // Draw with Eve (browsable paths, modify displaying...):
  TEveManager::Create();
  TGeoNode *N=  gGeoManager->GetTopNode();
  TEveGeoTopNode *TNod=new  TEveGeoTopNode(gGeoManager, N);
  TNod->SetVisLevel(10); // mandatory to see the Mvd
  gEve->AddGlobalElement(TNod);
  gEve->FullRedraw3D(kTRUE);

}
