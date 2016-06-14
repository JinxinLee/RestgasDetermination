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
#include "PndBoxGenerator.h"
#include "PndEvtGenDirect.h"
#include "PndMasterSimTask.h"
#include "PndEventCounterTask.h"
#include "PndFileNameCreator.h"

#include "FairFileSource.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "FairRuntimeDb.h"
#include "FairSystemInfo.h"
#include "FairModule.h"
#include "FairDetector.h"
#include "FairPrimaryGenerator.h"
#include "FairFilteredPrimaryGenerator.h"
#include "FairBoxGenerator.h"
#include "FairLogger.h"

#include "TLorentzVector.h"
#include "TDatabasePDG.h"

#include <fstream>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndMasterRunSim::PndMasterRunSim() :
  FairRunSim(), fParamRootFile(), fParamAsciiFile(), fRtdb(), fTimer(), fInput(), fInputDir(""), fOutFile(), fDpmFlag(1), fFtfFlag(0), fNEvents(0), fEventCounterRate(100)
{
  fTimer.Start();
}

// -----   Setup   ---------------------------------------------------------
Bool_t PndMasterRunSim::Setup(TString outprefix)
{
  TString inputName = outprefix;
  if (inputName=="") inputName = fInput;
  inputName.ToLower(); 
  if (inputName.EndsWith(".dec")) inputName.Remove(inputName.Length()-4,4);
  inputName.ReplaceAll(":","_");
  
  PndFileNameCreator creator(inputName.Data());
  SetOutputFile(creator.GetSimFileName().data());
  fOutFile = creator.GetSimFileName().data();
  SetParamRootFile(creator.GetParFileName().data());
  SetMaterials("media_pnd.geo");
  SetGenerateRunInfo(kFALSE);  
  SetUseFairLinks(kTRUE);

  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
  allDigiFile += "/macro/params/";
  allDigiFile += fParamAsciiFile;
  
  fRtdb = this->GetRuntimeDb();
  Bool_t kParameterMerged=kFALSE; // No use until now
  FairParRootFileIo* parOutput = new FairParRootFileIo(kParameterMerged);
  parOutput->open(fParamRootFile,"RECREATE");
  
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
        
  fRtdb->setFirstInput(parIo1);
  fRtdb->setOutput(parOutput);

  // -----  Create and Set the Field(s) ------------------------------------
  PndMultiField *field= new PndMultiField("AUTO");
  SetField(field);

  // ---- Defining PANDA particles -----------------------------------------
  Double_t mom = GetBeamMom();
  TLorentzVector fIni(0, 0, mom, sqrt(mom*mom+9.3827203e-01*9.3827203e-01)+9.3827203e-01);
  TDatabasePDG::Instance()->AddParticle("pbarpSystem" ,"pbarpSystem",  fIni.M(), kFALSE, 0.1, 0, "", 88888);
  TDatabasePDG::Instance()->AddParticle("pbarpSystem0","pbarpSystem0", fIni.M(), kFALSE, 0.1, 0, "", 88880);
  TDatabasePDG::Instance()->AddParticle("pbarpSystem1","pbarpSystem1", fIni.M(), kFALSE, 0.1, 0, "", 88881);
  TDatabasePDG::Instance()->AddParticle("pbarpSystem2","pbarpSystem2", fIni.M(), kFALSE, 0.1, 0, "", 88882);

  return kTRUE;
}

// -----   CreateGeometry   -------------------------------------------------
void PndMasterRunSim::CreateGeometry()
{
  //-------------------------  CAVE      -----------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  AddModule(Cave);
  //-------------------------  Magnet   -----------------
  //FairModule *Magnet= new PndMagnet("MAGNET");
  //Magnet->SetGeometryFileName("FullSolenoid_V842.root");
  //Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
  //AddModule(Magnet);
  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  AddModule(Dipole);
  //-------------------------  Pipe     -----------------
  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("beampipe_201309.root");
  AddModule(Pipe);
  //-------------------------  STT       -----------------
  FairDetector *Stt= new PndStt("STT", kTRUE);
  Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
  AddModule(Stt);
  //-------------------------  MVD       -----------------
  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
  AddModule(Mvd);
  //-------------------------  GEM       -----------------
  FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
  Gem->SetGeometryFileName("gem_3Stations_Tube.root");
  AddModule(Gem);
  //-------------------------  EMC       -----------------
  PndEmc *Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryVersion(1);
  Emc->SetStorageOfData(kFALSE);
  AddModule(Emc);
  //-------------------------  SCITIL    -----------------
  FairDetector *SciT = new PndSciT("SCIT",kTRUE);
  SciT->SetGeometryFileName("SciTil_201601.root");
  AddModule(SciT);
  //-------------------------  DRC       -----------------
  PndDrc *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetGeometryFileName("dirc_l0_p0_updated.root");
  Drc->SetRunCherenkov(kFALSE);
  AddModule(Drc);
  //-------------------------  DISC      -----------------
  PndDsk* Dsk = new PndDsk("DSK", kTRUE);
  Dsk->SetStoreCerenkovs(kFALSE);
  Dsk->SetStoreTrackPoints(kFALSE);
  AddModule(Dsk);
  //-------------------------  MDT       -----------------
  PndMdt *Muo = new PndMdt("MDT",kTRUE);
  Muo->SetBarrel("fast");
  Muo->SetEndcap("fast");
  Muo->SetMuonFilter("fast");
  Muo->SetForward("fast");
  Muo->SetMdtMagnet(kTRUE);
  Muo->SetMdtCoil(kTRUE);
  Muo->SetMdtMFIron(kTRUE);
  AddModule(Muo);
  //-------------------------  FTS       -----------------
  FairDetector *Fts= new PndFts("FTS", kTRUE);
  Fts->SetGeometryFileName("fts.geo");
  AddModule(Fts);
  //-------------------------  FTOF      -----------------
  FairDetector *FTof = new PndFtof("FTOF",kTRUE);
  FTof->SetGeometryFileName("ftofwall.root");
  AddModule(FTof);
  //-------------------------  RICH       ----------------
  FairDetector *Rich= new PndRich("RICH",kFALSE);
  Rich->SetGeometryFileName("rich_v2_shift.geo");
  AddModule(Rich);
}

// -----   AddSimTasks   ---------------------------------------------------
void PndMasterRunSim::AddSimTasks()
{
  // -----   Event Counter   --------------------------------
  AddTask(new PndEventCounterTask("Event Counter", fNEvents, fEventCounterRate));
  
  PndMasterSimTask *sim = new PndMasterSimTask();
  AddTask(sim);
  
  return;
}

// -----   SetGenerator   --------------------------------------------------
void PndMasterRunSim::SetGenerator()
{
  fGen = new FairFilteredPrimaryGenerator();
  
  TString input = fInput;
  input.ToLower();
  
  if (input.EndsWith(".dec") || input.Contains(".dec:")) 
    {
      UseEvtGenGenerator(fInput);
    }
  else if (input.BeginsWith("dpm"))
    {
      UseDpmGenerator();
    }
  else if (input.BeginsWith("ftf"))
    {
      UseFtfGenerator();
    }
  else if (input.BeginsWith("box"))
    {
      UseBoxGenerator(fInput);
    }  
  else 
    {
      LOG(FATAL)<< "Generator could not be identified from input '"<<fInput.Data()<<"'!!" <<  FairLogger::endl;
    }
  
}

void PndMasterRunSim::UseBoxGenerator(TString fBoxConfig)
{
  // use BOX generator; defaults

  Double_t BoxMomMin  = 0.05;   // minimum momentum for box generator
  Double_t BoxMomMax  = 10.;    // maximum   "       "
  Double_t BoxThtMin  = 0. ;    // minimum theta for box generator
  Double_t BoxThtMax  = 180.;   // maximum   "       "
  Double_t BoxPhiMin  = 0. ;    // minimum phi for box generator
  Double_t BoxPhiMax  = 360.;   // maximum   "       "
  Bool_t   BoxCosTht  = false;  // isotropic in cos(theta) instead theta
  
  Int_t    BoxType    = 13;     // default particle muon
  Int_t    BoxMult    = 1;      // default particle multiplicity
  Double_t type=0,mult=0;       // ref. parameters for range function
  
  fBoxConfig.ToLower();
  
  if (fBoxConfig!="box")
  {
    fBoxConfig.ReplaceAll("box","");
    fBoxConfig.ReplaceAll(" ","");
    fBoxConfig += ":";
    
    while (fBoxConfig.Contains(":"))
    {
      TString curpar = fBoxConfig(0,fBoxConfig.Index(":"));
      fBoxConfig = fBoxConfig(fBoxConfig.Index(":")+1,1000);

      if (curpar.BeginsWith("type(")) {GetRange(curpar,type,mult); BoxType = (Int_t)type; BoxMult = (Int_t)mult; }
      if (curpar.BeginsWith("p("))    GetRange(curpar,BoxMomMin,BoxMomMax);
      if (curpar.BeginsWith("tht("))   GetRange(curpar,BoxThtMin,BoxThtMax);
      if (curpar.BeginsWith("ctht(")) {GetRange(curpar,BoxThtMin,BoxThtMax); BoxCosTht=true;}
      if (curpar.BeginsWith("phi("))   GetRange(curpar,BoxPhiMin,BoxPhiMax);
    }
  }

  PndBoxGenerator* boxGen = new PndBoxGenerator(BoxType, BoxMult); 
  
  boxGen->SetPRange(BoxMomMin,BoxMomMax);      // GeV/c
  boxGen->SetPhiRange(BoxPhiMin, BoxPhiMax);   // Azimuth angle range [degree]
  boxGen->SetThetaRange(BoxThtMin, BoxThtMax); // Polar angle in lab system range [degree]
  
  if (BoxCosTht) boxGen->SetCosTheta();
  
  boxGen->SetXYZ(0., 0., 0.); //cm
		
  LOG(INFO) << "Using PndBoxGenerator(" << GetBeamMom() <<", pdg="<<BoxType<<" mult="<<BoxMult
	    <<" ) generator with range p["<<BoxMomMin<<","<<BoxMomMax<<"]  tht["<<BoxThtMin<<","<<BoxThtMax<<"]"<<(BoxCosTht?"*":"")<<"  phi["<<BoxPhiMin<<","<<BoxPhiMax<<"]" << FairLogger::endl;
  
  //  cout <<"BOX generator range: p["<<BoxMomMin<<","<<BoxMomMax<<"]  tht["<<BoxThtMin<<","<<BoxThtMax<<"]"<<(BoxCosTht?"*":"")<<"  phi["<<BoxPhiMin<<","<<BoxPhiMax<<"]"<<endl;
  
  fGen->AddGenerator(boxGen);
}

// -----   SetGenerator   --------------------------------------------------
 void PndMasterRunSim::SetGenerator(PndBoxGenerator *boxGen)
{
  LOG(INFO) << "Using PndBoxGenerator generator" << FairLogger::endl;
  fGen = new FairFilteredPrimaryGenerator();
  fGen->AddGenerator(boxGen);
}
 
// -----   SetGenerator   --------------------------------------------------
void PndMasterRunSim::SetGenerator(FairBoxGenerator *boxGen)
 {
  LOG(INFO) << "Using FairBoxGenerator generator" << FairLogger::endl;
  fGen = new FairFilteredPrimaryGenerator();
  fGen->AddGenerator(boxGen);
}

// -----   UseDpmGenerator   -----------------------------------------------
void PndMasterRunSim::UseDpmGenerator()
{
  LOG(INFO) << "Using PndDpmDirect(" << GetBeamMom() << ", " << fDpmFlag << ") generator" << FairLogger::endl;
  PndDpmDirect *Dpm= new PndDpmDirect(GetBeamMom(), fDpmFlag);
  fGen->AddGenerator(Dpm);
}
 
// -----   UseFtfGenerator   -----------------------------------------------
void PndMasterRunSim::UseFtfGenerator()
{
  //if ( strncmp(fName,"TGeant4",7 ) == 0 ) LOG(FATAL) << "FTF does not run with Geant4 !!!"  << FairLogger::endl;
  LOG(INFO) << "Using PndFtfDirect(anti_proton, G4_H, 1, ftfp, " << GetBeamMom() << ", " << gRandom->GetSeed() <<", "<<fFtfFlag<< ") generator" << FairLogger::endl;
  PndFtfDirect *Ftf = new PndFtfDirect("anti_proton", "G4_H", 1, "ftfp", GetBeamMom(), gRandom->GetSeed(), fFtfFlag);
  fGen->AddGenerator(Ftf);
}

// -----   UseEvtGenGenerator   --------------------------------------------
void PndMasterRunSim::UseEvtGenGenerator(TString fEvtGenFile)
{
  
  TString IniRes="";
  
  if (fEvtGenFile.Contains(":")) // is the initial resonance provide as <decfile>.dec:iniRes ? 
  {
    IniRes = fEvtGenFile(fEvtGenFile.Index(":")+1,1000);
    fEvtGenFile = fEvtGenFile(0,fEvtGenFile.Index(":"));
  }
  
  if (IniRes=="") // we need to search the decay file
  {
    TString fnamepath=fInputDir+fEvtGenFile;
    std::ifstream fs(fnamepath.Data());	
    char line[250];
  
    while (fs)
    {
      fs.getline(line,249);
      TString s(line);
      s.ReplaceAll("\r","");
      if (IniRes=="" && s.Contains("Decay "))
      {
        if (s.Contains("#")) s=s(0,s.Index("#"));
        s.ReplaceAll("Decay ","");
        s.ReplaceAll(" ","");
        IniRes = s;
      }	 
    } 
    fs.close();
  }
  /*
  // Looping over the dec file trying to find the first string "Decay", in order to find the initai
  // state as the following string
  FILE *dec = fopen(fInputDir+fEvtGenFile,"r");
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
  */
  if (IniRes=="") LOG(FATAL) << "The input file is not a proper .dec!! " << FairLogger::endl;
  
    //   TString  EvtInput =gSystem->Getenv("VMCWORKDIR");
  //   EvtInput+="/macro/run/psi2s_Jpsi2pi_Jpsi_mumu.dec";
  LOG(INFO) << "Using PndEvtGenDirect(" <<IniRes << ", " << (fInputDir+fEvtGenFile).Data() << ", " << GetBeamMom() << ") generator" << FairLogger::endl;
  PndEvtGenDirect *EvtGen = new PndEvtGenDirect(IniRes, (fInputDir+fEvtGenFile).Data(), GetBeamMom());
  EvtGen->SetStoreTree(kTRUE);
  fGen->AddGenerator(EvtGen);
  
}

// -----   Finish   ---------------------------------------------------------
void PndMasterRunSim::Finish()
{
  fRtdb->saveOutput();

  // write the summary of event filter to output root file
  ((FairFilteredPrimaryGenerator*)fGen)->WriteEvtFilterStatsToRootFile();   

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
  LOG(INFO) << "Output file is\t\t"    << fOutFile << FairLogger::endl;
  LOG(INFO) << "Parameter ROOT file is\t" << fParamRootFile << FairLogger::endl;
  LOG(INFO) << "Parameter ASCII file is\t" << fParamAsciiFile << FairLogger::endl;
  LOG(INFO) << "Real time " << rtime << " s, CPU time " << ctime
	    << "s" << FairLogger::endl;
  LOG(INFO) << "CPU usage " << cpuUsage*100. << "%" << FairLogger::endl;
  LOG(INFO) << "Max Memory " << maxMemory << " MB" << FairLogger::endl;
   
  LOG(INFO) << "Macro finished successfully." << FairLogger::endl;
  
}

// -----Helper function for parameter parsing  ---------------------------------------------------------
// par = string parameter like 'varname(min,max)' of 'varname(value)'

void PndMasterRunSim::GetRange(TString par, double &min, double &max)
{
	par.ReplaceAll(" ","");
	par = par(par.Index("(")+1, par.Length()-par.Index("(")-2);
	
	TString smin=par, smax=par;
	
	if (par.Contains(",")) 
	{
		smin = par(0,par.Index(","));
		smax = par(par.Index(",")+1,1000);
	}
	
	min = smin.Atof();
	max = smax.Atof();
}

/** @cond CLASSIMP */
ClassImp(PndMasterRunSim);
/** @endcond */
