
#ifndef __CINT__
#include "iostream"

#include "stdio.h"
#include <getopt.h>

#include "TStopwatch.h"
#include "TString.h"
#include "TRandom.h"

#include "FairRunSim.h"
#include "FairRuntimeDb.h"
#include "FairParAsciiFileIo.h"
#include "FairParRootFileIo.h"
#include "FairModule.h"

#include "PndCave.h"
#include "PndMagnet.h"
#include "PndPipe.h"
#include "PndStt.h"
#include "PndMvdDetector.h"
#include "PndEmc.h"
#include "PndMdt.h"
#include "PndGemDetector.h"
#include "PndDsk.h"
#include "PndDrc.h"
#include "PndFts.h"
#include "PndSciT.h"
#include "PndFtof.h"
#include "PndRich.h"

#include "FairEvtGenGenerator.h"
#include "PndDpmDirect.h"
#include "FairPrimaryGenerator.h"
#include "FairBoxGenerator.h"
#include "PndEvtGenDirect.h"
#include "PndDpmDirect.h"


#include "PndMultiField.h"
#include "PndMultiFieldPar.h"

#include "PndEmcHitProducer.h"

#include "TMemStat.h"


#endif

#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h> 


#define DEVURANDOM "/dev/urandom"

const char * help_str(){
  return "Usage: [executable] [parameters]\n\n"
    "\033[0;31m  -o\t--output  [name]\t\tName of the [o]utput file\033[0m\n"
    "\033[0;31m  -n\t--number  [#]\t\tnumber of events to simulate\033[0m\n"
    "\033[0;31m  -t\t--tracker  [tracker]\t\tWhich tracker will be used [Geant3, Geant4]\033[0m\n"
    //    "\n-----------------------------------------------------------------\n"
    ;
}

//void sim(TString output="/tmp/test.root", 
void sim(TString output, 
         // TString input, 
         Int_t nEvents,  
         const char* TransportModel, 
         UInt_t seed);

unsigned int GetRandomSeed(){
  unsigned int rnum = 0;
  int fd = open(DEVURANDOM, 0);
  if (fd != -1){
    (void) read(fd, (void *)&rnum, sizeof(int));
    (void) close(fd);
  }
  return rnum;
}

int main(int argc, char ** argv){

  int c;
  unsigned nevt = 2;
  string out = "RadMap_Out_Sim.root";
  string tra = "TGeant3";

  while ((c = getopt(argc, argv, "n:o:h")) != -1)
    switch (c) {
    case 'n': 
      nevt = atoi(optarg);
      break;

    case 'o': 
      out = optarg;
      break;
    
    case 't': 
      tra = optarg;
      break;
    
    case 'h': 
      printf("%s\n", help_str()); 
      break;

    default: 
      printf("%s\n", help_str()); 
      abort ();
    }


  
  int RandN = GetRandomSeed();
  std::cout << "***************************\n";
  std::cout << "sim: " << RandN << std::endl;
  std::cout << "***************************\n";

  // sim("blablabla2.root", 1000, "TGeant3", RandN);
  sim(TString(out.c_str()), nevt, tra.c_str(), RandN);
}

// Macro sim_radmap
// It creates a geant simulation file with the RadMap branch

void sim(TString output, 
         Int_t nEvents,  
         const char* TransportModel, 
         UInt_t seed){

  //   activate TMemStat info file (ROOT >=5.28)
  //   TMemStat mm("gnubuiltin");
  
  gRandom->SetSeed(seed);
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  unsigned int RandN = 0;

  
  FairRunSim *fRun = new FairRunSim();
  
  fRun->SetName(TransportModel);  
  fRun->SetOutputFile(output);
  //  fRun->GetOutputFile()->SetCompressionSettings(ROOT::CompressionSettings(ROOT::kLZMA, 1));
  // fRun->GetOutputFile()->SetCompressionLevel(9);
  
  fRun->SetMaterials("media_pnd.geo");
  

  // Create and add detectors
  //-------------------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

  FairModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("FullSolenoid_V842.root");
  // Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
  fRun->AddModule(Magnet);

  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);

  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("beampipe_201112.root");
  // Pipe->SetGeometryFileName("beampipe_201309.root");
  fRun->AddModule(Pipe);

  FairDetector *Stt= new PndStt("STT", kFALSE);
  // Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe_electronics.geo");
  Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
  fRun->AddModule(Stt);

  FairDetector *Mvd = new PndMvdDetector("MVD", kFALSE);
  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
  // Mvd->SetGeometryFileName("Mvd-2.1_AddDisks_FullVersion.root");
  fRun->AddModule(Mvd);

  FairDetector *Gem = new PndGemDetector("GEM", kFALSE);
  // Gem->SetGeometryFileName("gem_3Stations.root");
  Gem->SetGeometryFileName("gem_4Stations.root");
  fRun->AddModule(Gem);
  
  PndEmc *Emc = new PndEmc("EMC",kFALSE);
  Emc->SetGeometryVersion(1);
  Emc->SetStorageOfData(kFALSE);
  fRun->AddModule(Emc);

  // FairDetector *Tof = new PndTof("TOF",kFALSE);
  // Tof->SetGeometryFileName("tofbarrel.geo");
  // fRun->AddModule(Tof);
  
  // including forward detectors
    
  FairModule* SciT = new PndSciT("SCIT",kTRUE);
  //SciT->SetGeometryFileName("barrel-SciTil_07022013.root");
  SciT->SetGeometryFileName("SciTil_201508.root");
  fRun->AddModule(SciT);

  PndDrc *Drc = new PndDrc("DIRC", kFALSE);
  Drc->SetGeometryFileName("dirc_l4_p0_R2.root"); 
  // Drc->SetGeometryFileName("dirc_l0_p0_updated.root"); 
    // Drc->SetGeometryFileName("dirc_l0_p0.root");
  Drc->SetRunCherenkov(kFALSE); // for fast sim Cherenkov -> kFALSE
  fRun->AddModule(Drc);

  PndDsk* Dsk = new PndDsk("DSK", kFALSE);
  Dsk->SetGeometryFileName("dsk.root");
  Dsk->SetStoreCerenkovs(kFALSE);
  Dsk->SetStoreTrackPoints(kFALSE);
  fRun->AddModule(Dsk);


  // PndDsk* Dsk = new PndDsk("DSK", kFALSE);
  // Dsk->SetGeometryFileName("dsk.geo");
  // fRun->AddModule(Dsk);
 
  PndMdt *Muo = new PndMdt("MDT",kFALSE);
  Muo->SetMdtMagnet(kTRUE);
  Muo->SetBarrel("muon_TS_barrel_strip_v1_noGeo.root");
  Muo->SetEndcap("muon_TS_endcap_strip_v1_noGeo.root");
  Muo->SetForward("muon_Forward_strip_v1_noGeo.root");
  Muo->SetMuonFilter("muon_MuonFilter_strip_v1_noGeo.root");
  fRun->AddModule(Muo);

  //-------------------------  FTS       -----------------
  FairDetector *Fts= new PndFts("FTS", kFALSE);
  Fts->SetGeometryFileName("fts.geo");
  fRun->AddModule(Fts);

  //---------------------------  FTOF      -----------------
  FairDetector *FTof = new PndFtof("FTOF",kFALSE);
  FTof->SetGeometryFileName("ftofwall.root"); // not default
  fRun->AddModule(FTof);


  FairModule* Rich= new PndRich("RICH",kFALSE);
  Rich->SetGeometryFileName("rich_v2.geo");
  fRun->AddModule(Rich);

  // Create and Set Event Generator
  //-------------------------------
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  
  // PndFlukaGenerator* flugen = new PndFlukaGenerator(input);
  // primGen->AddGenerator(flugen);

  Double_t P = 15.0;
  RandN = GetRandomSeed();
  std::cout << "***************************\n";
  std::cout << "Dpm: " << RandN << std::endl;
  std::cout << "***************************\n";
  PndDpmDirect* dpmGen = new PndDpmDirect(P, 1, GetRandomSeed());
  primGen->AddGenerator(dpmGen);

 
  std::cout << "111111111111111111\n";
  fRun->GetListOfModules()->Print();
  std::cout << "222222222222222222\n";
  fRun->SetStoreTraj(kFALSE); // to store particle trajectories 
  fRun->SetRadMapRegister(kTRUE); // radiation map manager
    
  std::cout << "333333333333333333\n";
  fRun->SetBeamMom(P);
  // fRun->SetBeamMom(15.0);
  PndMultiField *fField= new PndMultiField("FULL");
  fRun->SetField(fField);
  std::cout << "444444444444444444\n";


  timer.Stop();
  
  Double_t preinitrtime = timer.RealTime();
  Double_t preinitctime = timer.CpuTime();
  std::cout << "555555555555555555\n";

  timer.Continue();

  std::cout << "666666666666666666\n";
  fRun->Init();
  std::cout << "777777777777777777\n";

  timer.Stop();
  
  Double_t postinitrtime = timer.RealTime();
  Double_t postinitctime = timer.CpuTime();

  timer.Continue();

  fRun->Run(nEvents);
   
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("Preinit: %f seconds, Postinit: %f seconds; RealTime=%f seconds, CpuTime=%f seconds\n", preinitrtime, postinitrtime, rtime, ctime);

  
  
}  
 
