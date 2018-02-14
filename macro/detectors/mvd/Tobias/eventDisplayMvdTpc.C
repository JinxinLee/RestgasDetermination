

eventDisplayMvdTpc()
{
//  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");


	  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
/*  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();  
 
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libGen");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libStt"); 
  gSystem->Load("libMuo");
  gSystem->Load("libEmc");  
  gSystem->Load("libTof");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");
  gSystem->Load("libtpc");
//  gSystem->Load("libDch");
  gSystem->Load("libMvd");
  gSystem->Load("libMvdReco");
  gSystem->Load("libHyp");
//  gSystem->Load("libDsk");*/
  gSystem->Load("libEve");
  gSystem->Load("libEventDisplay");
  gSystem->Load("libGeom");
                                     
  // -----   Reconstruction run   -------------------------------------------
  CbmRunAna *fRun= new CbmRunAna();
  
  TString MCFile = "/home/stockman/fairroot/cbmsoft/pandaroot/macro/data/MvdTpc_D+D-_2Disks/Combined.mc.root";
  TString RecoFile = "/home/stockman/fairroot/cbmsoft/pandaroot/macro/data/MvdTpc_D+D-_2Disks/digiMVD/recoMVD/Combined.reco.root";
  TString RecoFileTPC = "/home/stockman/fairroot/cbmsoft/pandaroot/macro/data/MvdTpc_D+D-_2Disks/digiTPC/recoTPC/mergeTPC/Combined.merged.root";
  TString TFindFile = "/home/stockman/fairroot/cbmsoft/pandaroot/macro/data/MvdTpc_D+D-_2Disks/digiMVD/recoMVD/riemannMVD/Combined.riemann.root"; 
  TString TFindFileComb = "/home/stockman/fairroot/cbmsoft/pandaroot/macro/data/MvdTpc_D+D-_2Disks/digiMVD/recoMVD/riemannMVD/combinedmergedMVDTPC/Combined.combined.root"; 
    
//  PndFileNameCreator creator(MCFile.Data());
//  TString RecoFile = creator.GetRecoFileName(false).c_str();
//  TString TFindFile  = creator.GetTrackFindingFileName(false).c_str();
  
  fRun->SetInputFile(MCFile.Data());
  fRun->AddFriend(RecoFile.Data());
  fRun->AddFriend(RecoFileTPC.Data());
  fRun->AddFriend(TFindFile.Data());
  fRun->AddFriend(TFindFileComb.Data());
  //fRun->SetInputFile("../dsk/sim_dsk.root");
 // fRun->SetInputFile("../dsk/sim_dsk.g4native.root");
  fRun->SetOutputFile("test.root");
  fRun->LoadGeometry();

  FairEventManager *fMan= new FairEventManager();
  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
  FairMCPointDraw *MvdMCPoints =   new FairMCPointDraw ("MVDPoint", 1,7);
  FairRecoPointDraw *MvdRecoPoints =   new FairRecoPointDraw ("MVDHitsPixel",3);
  FairRecoPointDraw *MvdStripRecoPoints = new FairRecoPointDraw("MVDHitsStrip",3);
  FairTPCRecoPointDraw *TPCRecoPoints = new FairTPCRecoPointDraw("PndTpcClusterMerged",3);
  FairTrackCandDraw* MvdTrackCand = new FairTrackCandDraw("MVDRiemannTrackCand", 3);
  FairTrackCandDraw* CombinedTrackCand = new FairTrackCandDraw("MVD_TPC_CombinedRiemannTrack", 3);
//  FairMCPointDraw *EMCPoints =   new FairMCPointDraw ("EmcPoint");
//  FairMCPointDraw *TofPoint =    new FairMCPointDraw ("TofPoint");
//  FairMCPointDraw *TofSciFPoint= new FairMCPointDraw ("TofSciFPoint");
//  FairMCPointDraw *MuoPoint =    new FairMCPointDraw ("MuoPoint");
//  FairMCPointDraw *PndDrcPoint = new FairMCPointDraw ("PndDrcPoint");
//  FairMCPointDraw *PndDchPoint = new FairMCPointDraw ("PndDchPoint");
  FairMCPointDraw *PndTpcPoint = new FairMCPointDraw ("PndTpcPoint",2,7);
 // FairMCPointDraw *PndSTTPoint = new FairMCPointDraw ("STTPoint");

                                                               
  fMan->AddTask(Track);
  
  fMan->AddTask(MvdMCPoints);
  fMan->AddTask(MvdRecoPoints);
  fMan->AddTask(MvdStripRecoPoints);
  fMan->AddTask(TPCRecoPoints);
  fMan->AddTask(MvdTrackCand);
  fMan->AddTask(CombinedTrackCand);
// fMan->AddTask(EMCPoints);   
// fMan->AddTask(TofPoint);   
// fMan->AddTask( TofSciFPoint);
//  fMan->AddTask( MuoPoint);
//  fMan->AddTask( PndDrcPoint);
 // fMan->AddTask( PndDchPoint);
  fMan->AddTask( PndTpcPoint);
 // fMan->AddTask( PndSTTPoint);

    
  fMan->Init();                     
  gStyle->SetPalette(1);


}
