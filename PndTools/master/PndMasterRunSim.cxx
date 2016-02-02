#include "PndMasterRunSim.h"

#include "PndMultiField.h"
#include "PndCave.h"
#include "PndMagnet.h"
#include "PndPipe.h"
#include "PndStt.h"
#include "PndMvdDetector.h"
#include "PndGemDetector.h"
#include "PndEmc.h"
#include "PndSciT.h"
#include "PndDrc.h"
#include "PndDsk.h"
#include "PndMdt.h"
#include "PndFts.h"
#include "PndFtof.h"
#include "PndRich.h"
#include "PndEmcHitProducer.h"
#include "PndDpmDirect.h"
#include "PndFtfDirect.h"
#include "PndEvtGenDirect.h"
#include "PndEventCounterTask.h"

#include "FairFileSource.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "FairRuntimeDb.h"
#include "FairSystemInfo.h"
#include "FairModule.h"
#include "FairDetector.h"
#include "FairPrimaryGenerator.h"
#include "FairLogger.h"

#include "TLorentzVector.h"
#include "TDatabasePDG.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndMasterRunSim::PndMasterRunSim() :
  FairRunSim(), fParamRootFile(), fParamAsciiFile(), fRtdb(), fTimer() //, fInputFile()
{
  fTimer.Start();
  gRandom->SetSeed();
}

// -----   Setup   ---------------------------------------------------------
Bool_t PndMasterRunSim::Setup()
{
  //FairFileSource *fFileSource = new FairFileSource(fInputFile);
  //this->SetSource(fFileSource);
  
  this->SetMaterials("media_pnd.geo");
  this->SetGenerateRunInfo(kFALSE);  
  this->SetUseFairLinks(kTRUE);
  
  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
  allDigiFile += "/macro/params/";
  allDigiFile += fParamAsciiFile;
  
  fRtdb = this->GetRuntimeDb();
  Bool_t kParameterMerged=kFALSE; // No use until now
  FairParRootFileIo* parOutput = new FairParRootFileIo(kParameterMerged);
  parOutput->open(fParamRootFile.Data(),"RECREATE");
  
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
        
  fRtdb->setFirstInput(parIo1);
  fRtdb->setOutput(parOutput);

  // -----  Create and Set the Field(s) ------------------------------------
  PndMultiField *fField= new PndMultiField("AUTO");
  this->SetField(fField);

  // ---- Defining PANDA particles -----------------------------------------
  Double_t mom = this->GetBeamMom();
  TLorentzVector fIni(0, 0, mom, sqrt(mom*mom+9.3827203e-01*9.3827203e-01)+9.3827203e-01);
  TDatabasePDG::Instance()->AddParticle("pbarpSystem" ,"pbarpSystem",  fIni.M(), kFALSE, 0.1, 0, "", 88888);
  TDatabasePDG::Instance()->AddParticle("pbarpSystem0","pbarpSystem0", fIni.M(), kFALSE, 0.1, 0, "", 88880);
  TDatabasePDG::Instance()->AddParticle("pbarpSystem1","pbarpSystem1", fIni.M(), kFALSE, 0.1, 0, "", 88881);
  TDatabasePDG::Instance()->AddParticle("pbarpSystem2","pbarpSystem2", fIni.M(), kFALSE, 0.1, 0, "", 88882);

// ---- Initialization of primary generator --------------------------------
  fGen = new FairPrimaryGenerator();

  return kTRUE;
}

// -----   CreateGeometry   -------------------------------------------------
void PndMasterRunSim::CreateGeometry()
{
  //-------------------------  CAVE      -----------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  this->AddModule(Cave);
  //-------------------------  Magnet   -----------------
  //FairModule *Magnet= new PndMagnet("MAGNET");
  //Magnet->SetGeometryFileName("FullSolenoid_V842.root");
  //Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
  //this->AddModule(Magnet);
  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  this->AddModule(Dipole);
  //-------------------------  Pipe     -----------------
  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("beampipe_201309.root");
  this->AddModule(Pipe);
  //-------------------------  STT       -----------------
  FairDetector *Stt= new PndStt("STT", kTRUE);
  Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
  this->AddModule(Stt);
  //-------------------------  MVD       -----------------
  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
  this->AddModule(Mvd);
  //-------------------------  GEM       -----------------
  FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
  Gem->SetGeometryFileName("gem_3Stations_Tube.root");
  this->AddModule(Gem);
  //-------------------------  EMC       -----------------
  PndEmc *Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryVersion(1);
  Emc->SetStorageOfData(kFALSE);
  this->AddModule(Emc);
  //-------------------------  SCITIL    -----------------
  FairDetector *SciT = new PndSciT("SCIT",kTRUE);
  SciT->SetGeometryFileName("SciTil_201504.root");
  this->AddModule(SciT);
  //-------------------------  DRC       -----------------
  PndDrc *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetGeometryFileName("dirc_l0_p0_updated.root");
  Drc->SetRunCherenkov(kFALSE);
  this->AddModule(Drc);
  //-------------------------  DISC      -----------------
  PndDsk* Dsk = new PndDsk("DSK", kTRUE);
  Dsk->SetStoreCerenkovs(kFALSE);
  Dsk->SetStoreTrackPoints(kFALSE);
  this->AddModule(Dsk);
  //-------------------------  MDT       -----------------
  PndMdt *Muo = new PndMdt("MDT",kTRUE);
  Muo->SetBarrel("fast");
  Muo->SetEndcap("fast");
  Muo->SetMuonFilter("fast");
  Muo->SetForward("fast");
  Muo->SetMdtMagnet(kTRUE);
  Muo->SetMdtCoil(kTRUE);
  Muo->SetMdtMFIron(kTRUE);
  this->AddModule(Muo);
  //-------------------------  FTS       -----------------
  FairDetector *Fts= new PndFts("FTS", kTRUE);
  Fts->SetGeometryFileName("fts.geo");
  this->AddModule(Fts);
  //-------------------------  FTOF      -----------------
  FairDetector *FTof = new PndFtof("FTOF",kTRUE);
  FTof->SetGeometryFileName("ftofwall.root");
  this->AddModule(FTof);
  //-------------------------  RICH       ----------------
  FairDetector *Rich= new PndRich("RICH",kFALSE);
  Rich->SetGeometryFileName("rich_v2_shift.geo");
  this->AddModule(Rich);
}

// -----   AddSimTasks   ---------------------------------------------------
void PndMasterRunSim::AddSimTasks()
{
  // -----   EMC hit producers   ---------------------------------
  this->AddTask(new PndEmcHitProducer());
  // -----   Event Counter   --------------------------------
  this->AddTask(new PndEventCounterTask("Event Counter", 0, 100));
}

// -----   UseDpmGenerator   -- --------------------------------------------
void PndMasterRunSim::UseDpmGenerator(Int_t Mode)
{
  PndDpmDirect *Dpm= new PndDpmDirect(this->GetBeamMom(), Mode);
  fGen->AddGenerator(Dpm);
}
 
// -----   UseFtfGenerator   -----------------------------------------------
void PndMasterRunSim::UseFtfGenerator()
{
  if ( strncmp(fName,"TGeant4",7 ) == 0 ) LOG(FATAL) << "FTF does not run with Geant4 !!!"  << FairLogger::endl;
  PndFtfDirect *Ftf = new PndFtfDirect("anti_proton", "G4_H", 1, "ftfp", this->GetBeamMom(), 123456);
  fGen->AddGenerator(Ftf);
}

// -----   UseEvtGenGenerator   --------------------------------------------
void PndMasterRunSim::UseEvtGenGenerator(TString fEvtGenFile)
{
  // Looping over the dec file trying to find the first string "Decay", in order to find the initai
  // state as the following string
  FILE *dec = fopen(fEvtGenFile,"r");
  if (dec==NULL) LOG(FATAL) << "The EvtGen dec file does not exist!! " << fEvtGenFile << FairLogger::endl;
  
  char temp[6], particle[20];
  Bool_t found = kFALSE;
  while(fgets(temp, 6, dec) !=NULL)
    {
      if((strstr(temp, "Decay")) != NULL)
	{	
	  fscanf(dec, "%s",particle);
	  LOG(INFO) << "It was found a " << particle << " as initial state." << FairLogger::endl;
	  found = kTRUE;
	  break;
	}    
    }
  if (!found) LOG(FATAL) << "The input file is not a proper .dec!! " << FairLogger::endl;
  
    //   TString  EvtInput =gSystem->Getenv("VMCWORKDIR");
  //   EvtInput+="/macro/run/psi2s_Jpsi2pi_Jpsi_mumu.dec";
  PndEvtGenDirect *EvtGen = new PndEvtGenDirect(particle, fEvtGenFile.Data(), this->GetBeamMom());
  EvtGen->SetStoreTree(kTRUE);
  fGen->AddGenerator(EvtGen);
}

// -----   Finish   ---------------------------------------------------------
void PndMasterRunSim::Finish()
{
  fRtdb->saveOutput();
  
  cout << endl;
  
  // Extract the maximal used memory an add is as Dart measurement
  // This line is filtered by CTest and the value send to CDash
  FairSystemInfo sysInfo;
  Float_t maxMemory=sysInfo.GetMaxMemory();
  cout << "<DartMeasurement name=\"MaxMemory\" type=\"numeric/double\">";
  cout << maxMemory;
  cout << "</DartMeasurement>" << endl;
  
  fTimer.Stop();
  Double_t rtime = fTimer.RealTime();
  Double_t ctime = fTimer.CpuTime();
  
  Float_t cpuUsage=ctime/rtime;
  cout << "<DartMeasurement name=\"CpuLoad\" type=\"numeric/double\">";
  cout << cpuUsage;
  cout << "</DartMeasurement>" << endl;
  
  cout << endl;
  LOG(INFO) << "Output file is "    << fOutname << FairLogger::endl;
  LOG(INFO) << "Parameter ROOT file is " << fParamRootFile << FairLogger::endl;
  LOG(INFO) << "Parameter ASCII file is " << fParamAsciiFile << FairLogger::endl << FairLogger::endl;;
  LOG(INFO) << "Real time " << rtime << " s, CPU time " << ctime
       << "s" << FairLogger::endl << FairLogger::endl;
  LOG(INFO) << "CPU usage " << cpuUsage*100. << "%" << FairLogger::endl;
  LOG(INFO) << "Max Memory " << maxMemory << " MB" << FairLogger::endl << FairLogger::endl;
   
  LOG(INFO) << "Macro finished successfully." << FairLogger::endl;
  
}

/** @cond CLASSIMP */
ClassImp(PndMasterRunSim);
/** @endcond */
