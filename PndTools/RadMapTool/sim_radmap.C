
#ifndef __CINT__
#include <iostream>
#include <fstream>

#include <stdio.h>
#include <getopt.h>

#include <TStopwatch.h>
#include <TString.h>
#include <TRandom.h>

#include <FairRunSim.h>
#include <FairRuntimeDb.h>
#include <FairParAsciiFileIo.h>
#include <FairParRootFileIo.h>
#include <FairModule.h>

#include <PndCave.h>
#include <PndMagnet.h>
#include <PndPipe.h>
#include <PndStt.h>
#include <PndMvdDetector.h>
#include <PndEmc.h>
#include <PndMdt.h>
#include <PndGemDetector.h>
#include <PndDsk.h>
#include <PndDrc.h>
#include <PndFts.h>
#include <PndSciT.h>
#include <PndFtof.h>
#include <PndRich.h>

#include <FairEvtGenGenerator.h>
#include <PndDpmDirect.h>
#include <FairPrimaryGenerator.h>
#include <FairBoxGenerator.h>
#include <PndEvtGenDirect.h>
#include <PndDpmDirect.h>


#include <PndMultiField.h>
#include <PndMultiFieldPar.h>

#include <PndEmcHitProducer.h>

#include <TMemStat.h>


#endif

#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h> 


#define DEVURANDOM "/dev/urandom"


void ReadFiles(string geomfile,
               std::vector<std::string>& geometryfiles);



const char * help_str(){
  return "Usage: [executable] [parameters]\n\n"
    "\033[0;31m  -o\t--output  [name]\t\tName of the [o]utput file\033[0m\n"
    "\033[0;31m  -n\t--number  [#]\t\tnumber of events to simulate\033[0m\n"
    "\033[0;31m  -t\t--tracker  [tracker]\t\tWhich tracker will be used [Geant3, Geant4]\033[0m\n"
    "\033[0;31m  -g\t--geofile  [geofile]\t\tFile that store the name of the geometry files of the detectors\033[0m\n"
    //    "\n-----------------------------------------------------------------\n"
    ;
}

//void sim(TString output="/tmp/test.root", 
void sim(TString output, 
         // TString input, 
         Int_t nEvents,  
         const char* TransportModel, 
         std::vector<std::string> geometryfiles,
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
  string tra = "TGeant4";
  string geofile = "/lustre/nyx/panda/carsten/fairsoft_nov15/pandaroot/PndTools/RadMapTool/detectors.dat";
  // std::vector<std::pair<std::string, std::string> > geometryfiles;
  std::vector<std::string> geometryfiles;
  while ((c = getopt(argc, argv, "n:o:t:g:h")) != -1)
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
    
    case 'g': 
      geofile = optarg;
      break;
    
    case 'h': 
      printf("%s\n", help_str()); 
      break;

    default: 
      printf("%s\n", help_str()); 
      abort ();
    }

  std::cout<<" using detectors from "<<geofile.c_str()<<std::endl;
  
  ReadFiles(geofile, geometryfiles);
  
  int RandN = GetRandomSeed();
  std::cout << "***************************\n";
  std::cout << "sim: " << RandN << std::endl;
  std::cout << "***************************\n";

  // sim("blablabla2.root", 1000, "TGeant3", RandN);
  sim(TString(out.c_str()), nevt, tra.c_str(), geometryfiles, RandN);
}

// Macro sim_radmap
// It creates a geant simulation file with the RadMap branch
void sim(TString output, 
         Int_t nEvents,  
         const char* TransportModel, 
         std::vector<std::string> geometryfiles,
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

  // fRun->SetMaterials("media_pnd.geo");
  fRun->SetMaterials(geometryfiles.at(0).c_str());
  

  // Create and add detectors
  //-------------------------
  FairModule *Cave= new PndCave("CAVE");
  // Cave->SetGeometryFileName("pndcave.geo");
  Cave->SetGeometryFileName(geometryfiles.at(1).c_str());
  fRun->AddModule(Cave); 

  FairModule *Magnet= new PndMagnet("MAGNET");
  // Magnet->SetGeometryFileName("FullSolenoid_V842.root");
  Magnet->SetGeometryFileName(geometryfiles.at(2).c_str());
  // Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
  fRun->AddModule(Magnet);

  FairModule *Dipole= new PndMagnet("MAGNET");
  // Dipole->SetGeometryFileName("dipole.geo");
  Dipole->SetGeometryFileName(geometryfiles.at(3).c_str());
  fRun->AddModule(Dipole);

  FairModule *Pipe= new PndPipe("PIPE");
  // Pipe->SetGeometryFileName("beampipe_201112.root");
  Pipe->SetGeometryFileName(geometryfiles.at(4).c_str());
  // Pipe->SetGeometryFileName("beampipe_201309.root");
  fRun->AddModule(Pipe);

  FairDetector *Stt= new PndStt("STT", kFALSE);
  // Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe_electronics.geo");
  // Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
  Stt->SetGeometryFileName(geometryfiles.at(5).c_str());
  fRun->AddModule(Stt);

  FairDetector *Mvd = new PndMvdDetector("MVD", kFALSE);
  Mvd->SetGeometryFileName(geometryfiles.at(6).c_str());
  // Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
  // Mvd->SetGeometryFileName("Mvd-2.1_AddDisks_FullVersion.root");
  fRun->AddModule(Mvd);

  FairDetector *Gem = new PndGemDetector("GEM", kFALSE);
  // Gem->SetGeometryFileName("gem_3Stations.root");
  // Gem->SetGeometryFileName("gem_4Stations.root");
  Gem->SetGeometryFileName(geometryfiles.at(7).c_str());
  fRun->AddModule(Gem);


  PndEmc *Emc = new PndEmc("EMC",kFALSE);
  Emc->SetGeometryVersion(1);
  Emc->SetStorageOfData(kFALSE);
  fRun->AddModule(Emc);

  // FairDetector *Tof = new PndTof("TOF",kFALSE);
  // Tof->SetGeometryFileName("tofbarrel.geo");
  // fRun->AddModule(Tof);
  
  // including forward detectors
    
  /////
  // FairModule* SciT = new PndSciT("SCIT",kTRUE);
  // SciT->SetGeometryFileName("SciTil_latest.root");
  // // SciT->SetGeometryFileName("barrel-SciTil_07022013.root");
  // // SciT->SetGeometryFileName(geometryfiles.at(8).c_str());
  // fRun->AddModule(SciT);

  PndDrc *Drc = new PndDrc("DIRC", kFALSE);
  Drc->SetGeometryFileName(geometryfiles.at(9).c_str()); 
  Drc->SetRunCherenkov(kFALSE); // for fast sim Cherenkov -> kFALSE
  fRun->AddModule(Drc);

  PndDsk* Dsk = new PndDsk("DSK", kFALSE);
  // Dsk->SetGeometryFileName("dsk.root");
  // Dsk->SetGeometryFileName(geometryfiles.at(10).c_str());
  Dsk->SetStoreCerenkovs(kFALSE);
  Dsk->SetStoreTrackPoints(kFALSE);
  fRun->AddModule(Dsk);


  // PndDsk* Dsk = new PndDsk("DSK", kFALSE);
  // Dsk->SetGeometryFileName("dsk.geo");
  // fRun->AddModule(Dsk);
 
  PndMdt *Muo = new PndMdt("MDT",kFALSE);
  // Muo->SetMdtMagnet(kTRUE);
  // Muo->SetBarrel(geometryfiles.at(10).c_str());
  // Muo->SetEndcap(geometryfiles.at(11).c_str());
  // Muo->SetForward(geometryfiles.at(12).c_str());
  // Muo->SetMuonFilter(geometryfiles.at(13).c_str());
  // fRun->AddModule(Muo);
  Muo->SetBarrel("fast");
  Muo->SetEndcap("fast");
  Muo->SetMuonFilter("fast");
  Muo->SetForward("fast");
  Muo->SetMdtMagnet(kTRUE);
  Muo->SetMdtCoil(kTRUE);
  Muo->SetMdtMFIron(kTRUE);
  fRun->AddModule(Muo);

  //-------------------------  FTS       -----------------
  FairDetector *Fts= new PndFts("FTS", kFALSE);
  Fts->SetGeometryFileName(geometryfiles.at(14).c_str());
  // Fts->SetGeometryFileName("fts.geo");
  fRun->AddModule(Fts);

  //---------------------------  FTOF      -----------------
  FairDetector *FTof = new PndFtof("FTOF",kFALSE);
  // FTof->SetGeometryFileName("ftofwall.root"); // not default
  FTof->SetGeometryFileName(geometryfiles.at(15).c_str()); // not default
  fRun->AddModule(FTof);


  FairModule* Rich= new PndRich("RICH",kFALSE);
  // Rich->SetGeometryFileName("rich_v2.geo");
  Rich->SetGeometryFileName(geometryfiles.at(16).c_str());
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

 
  fRun->GetListOfModules()->Print();
  fRun->SetStoreTraj(kFALSE); // to store particle trajectories 
  fRun->SetRadMapRegister(kTRUE); // radiation map manager
  fRun->SetBeamMom(P);
  // fRun->SetBeamMom(15.0);
  PndMultiField *fField= new PndMultiField("FULL");
  fRun->SetField(fField);

  timer.Stop();
  
  Double_t preinitrtime = timer.RealTime();
  // Double_t preinitctime = timer.CpuTime();

  timer.Continue();

  fRun->Init();

  timer.Stop();
  
  Double_t postinitrtime = timer.RealTime();
  // Double_t postinitctime = timer.CpuTime();

  timer.Continue();

  fRun->Run(nEvents);
   
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("Preinit: %f seconds, Postinit: %f seconds; RealTime=%f seconds, CpuTime=%f seconds\n", preinitrtime, postinitrtime, rtime, ctime);
}  
 
void ReadFiles(string geomfile,
               std::vector<std::string>& geometryfiles){
  //  enum _files {MEDIA   ,
  //               CAVE    , SOLENOID , DIPOLE  , PIPE     , STT ,
  //               MVD     , GEM      , SciTile , DRC      , DSK ,
  //               MUONBAR , MOUNENDC , MOUNFOR , MUONFILT , FTS ,
  //               FTOF    , RICH};

  for(int i = 0; i < 18; i++) geometryfiles.push_back(std::string(""));
  
  TObjArray *tokens;
  std::ifstream ifile(geomfile.c_str());

  
  if(ifile.is_open()){
    TString Line, help0, help1;
    std::string line;
    while (1) {
      getline (ifile,line);
      if(!ifile.good()) break;
      Line = TString(line);
      if(Line[0] != '#'){
        tokens = Line.Tokenize(":");
        if(tokens->GetEntries() == 2){
          help0 = ((TObjString*)(tokens->At(0)))->GetString().Remove(TString::kBoth, ' ');
          help1 = ((TObjString*)(tokens->At(1)))->GetString().Remove(TString::kBoth, ' ');
          cout << "\"" << ((TObjString*)(tokens->At(1)))->GetString() << "\" -> \"" << help1 << "\"" << endl;
          help0.ToUpper();
          if(help0.Contains("MEDIA")){
            geometryfiles.at(0) = help1;
          }
          if(help0.Contains("CAVE")){
            geometryfiles.at(1) = help1;
          }
          if(help0.Contains("SOLENOID")){
            geometryfiles.at(2) = help1;
          }
          if(help0.Contains("DIPOLE")){
            geometryfiles.at(3) = help1;
          }
          if(help0.Contains("PIPE")){
            geometryfiles.at(4) = help1;
          }

          if(help0.Contains("STT")){
            geometryfiles.at(5) = help1;
          }
          if(help0.Contains("MVD")){
            geometryfiles.at(6) = help1;
          }
          if(help0.Contains("GEM")){
            geometryfiles.at(7) = help1;
          }
          if(help0.Contains("SCITILE")){
            geometryfiles.at(8) = help1;
          }
          if(help0.Contains("DRC")){
            geometryfiles.at(9) = help1;
          }


          if(help0.Contains("DSK")){
            geometryfiles.at(10) = help1;
          }
          if(help0.Contains("MUONBAR")){
            geometryfiles.at(11) = help1;
          }
          if(help0.Contains("MUONENDC")){
            geometryfiles.at(12) = help1;
          }
          if(help0.Contains("MUONFOR")){
            geometryfiles.at(13) = help1;
          }
          if(help0.Contains("MUONFILT")){
            geometryfiles.at(14) = help1;
          }

          if(help0.Contains("FTS")){
            geometryfiles.at(15) = help1;
          }
          if(help0.Contains("FTOF")){
            geometryfiles.at(16) = help1;
          }
          if(help0.Contains("RICH")){
            geometryfiles.at(17) = help1;
          }
        }
      }
    }
  }
}
        
