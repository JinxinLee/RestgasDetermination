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
#include "PndFtfGenerator.h"
#include "PndBoxGenerator.h"
#include "PndEvtGenDirect.h"
#include "PndPiPiGenerator.h"
#include "PndLepLepGenerator.h"
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
#include "TGeoManager.h"
#include "TROOT.h"

#include <fstream>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndMasterRunSim::PndMasterRunSim() :
  FairRunSim(), fInput(), fInputDir(""), fOutFile(), fParamRootFile(), fParamAsciiFile(), fOptions(), fDpmFlag(1), fFtfFlag(0), fNEvents(0), fEventCounterRate(100), fTargetMode(0), fRtdb(), fTimer()
{
  fTimer.Start();
}
// -----   Default destructor   -------------------------------------------
PndMasterRunSim::~PndMasterRunSim()
{
  if (gROOT->GetVersionInt() >= 60602 && gGeoManager!=NULL) {
    gGeoManager->GetListOfVolumes()->Delete();
    gGeoManager->GetListOfShapes()->Delete();
    delete gGeoManager;
  }
}

// -----   Setup   ---------------------------------------------------------
Bool_t PndMasterRunSim::Setup(TString outprefix)
{
  TString inputName = outprefix;

  // If no prefix is given, we create one from fInput and force lower-case
  if (inputName=="")
  {
    inputName = fInput;
    inputName.ToLower();
  }

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

  if (fOptions.Contains("day1")) fTargetMode = 1;
  return kTRUE;
}

// -----   CreateGeometry   -------------------------------------------------
void PndMasterRunSim::CreateGeometry()
{
  if (fOptions=="") CreateGeometryDefault();
  if (fOptions.Contains("day1")) CreateGeometryDay1();
}

// -----   CreateGeometry   -------------------------------------------------
void PndMasterRunSim::CreateGeometryDefault()
{
  //-------------------------  CAVE      -----------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  AddModule(Cave);
  //-------------------------  Magnet   -----------------
  // This part is commented because the MDT geometry contains the magnet now
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
  Gem->SetGeometryFileName("gem_3Stations_realistic_v2.root");
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
  Drc->SetGeometryFileName("dirc_e3_b3_l6_m40.root");
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
  PndRich *Rich= new PndRich("RICH",kTRUE);
  Rich->SetGeometryFileName("rich_v313.root");
  AddModule(Rich);
}

// -----   CreateGeometryDay1   ---------------------------------------------
void PndMasterRunSim::CreateGeometryDay1()
{
  //-------------------------  CAVE      -----------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  AddModule(Cave);
  //-------------------------  Magnet   -----------------
  // This part is commented because the MDT geometry contains the magnet now
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
  if (fOptions.Contains("strip")){
	  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
	  Mvd->SetGeometryFileName("Mvd-2.1-Strips.root");
	  AddModule(Mvd);
  } else {
	  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
	  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
	  AddModule(Mvd);
  }
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
  Drc->SetGeometryFileName("dirc_e3_b3_l6_m40.root");
  Drc->SetRunCherenkov(kFALSE);
  AddModule(Drc);
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
  //-------------------------  FTOF      -----------------
  FairDetector *FTof = new PndFtof("FTOF",kTRUE);
  FTof->SetGeometryFileName("ftofwall.root");
  AddModule(FTof);

  if (fOptions.Contains("gem2"))
    {
      //-------------------------  GEM       -----------------
      FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
      Gem->SetGeometryFileName("gem_2Stations_realistic_v2.root");
      AddModule(Gem);
    }

  else if (fOptions.Contains("gem3"))
  {
	  FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
	  Gem->SetGeometryFileName("gem_3Stations_realistic_v2.root");
	  AddModule(Gem);
  }

  if (fOptions.Contains("fts1256"))
    {
      //-------------------------  FTS       -----------------
      FairDetector *Fts= new PndFts("FTS", kTRUE);
      Fts->SetGeometryFileName("fts_1256.geo");
      AddModule(Fts);
    }
  else
    {
      //-------------------------  FTS       -----------------
      FairDetector *Fts= new PndFts("FTS", kTRUE);
      Fts->SetGeometryFileName("fts_reduced.geo");
      AddModule(Fts);
    }
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

  switch (fTargetMode)
    {
    case 0:
      LOG(INFO) << "Using no Vertex smearing" << FairLogger::endl;
      break;
    case 1:
      LOG(INFO) << "Using Cluster Jet Target" << FairLogger::endl;
      // a cluster-jet beam at the interaction zone with a horizontal width
      // of e.g. dx = 1 mm and a length in accelerator beam direction of dz = 10 mm.
      // Target TDR, page 44
      fGen->SetTarget(0., 1./2.355); // From FWHM to sigma
      fGen->SmearVertexZ(kTRUE);
      fGen->SmearGausVertexZ(kTRUE);
      fGen->SetBeam(0., 0., 0.1, 0.1);
      fGen->SmearVertexXY(kTRUE);
      break;
    case 2:
      LOG(INFO) << "Using Pellet Target" << FairLogger::endl;
      // At PANDA a beam diameter around 3mm is needed and one may want even
      // smaller size when PTR is not possible.
      // Target TDR, page 61
      fGen->SetTarget(0., 0.3);
      fGen->SmearVertexZ(kTRUE);
      fGen->SmearGausVertexZ(kTRUE);
      fGen->SetBeam(0., 0., 0.3, 0.3);
      fGen->SmearVertexXY(kTRUE);
      break;
    case 3:
      LOG(INFO) << "Using Pellet Tracking Target" << FairLogger::endl;
      // A position resolution σ(x, y, z) < 0.2 mm in the in- teraction position
      // is desirable for event reconstruc- tion.
      // Target TDR, page 61
      fGen->SetTarget(0., 0.02);
      fGen->SmearVertexZ(kTRUE);
      fGen->SmearGausVertexZ(kTRUE);
      fGen->SetBeam(0., 0., 0.02, 0.02);
      fGen->SmearVertexXY(kTRUE);
      break;
    default:
      LOG(INFO) << "Unkwown target mode - Using no vertex smearing" << FairLogger::endl;
    }

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
      UseFtfGenerator(fInput);
    }
  else if (input.BeginsWith("box"))
    {
      UseBoxGenerator(fInput);
    }
  else if (input.BeginsWith("pipi"))
  {
	  UsePiPiGenerator(fInput);
  }
  else
    {
      LOG(FATAL)<< "Generator could not be identified from input '"<<fInput.Data()<<"'!!" <<  FairLogger::endl;
    }

}

void PndMasterRunSim::UseBoxGenerator(TString BoxConfig)
{
  // use BOX generator; defaults

  Double_t BoxMomMin  = 0.05;   // minimum momentum for box generator
  Double_t BoxMomMax  = 10.;    // maximum   "       "
  Double_t BoxThtMin  = 0. ;    // minimum theta for box generator
  Double_t BoxThtMax  = 180.;   // maximum   "       "
  Double_t BoxPhiMin  = 0. ;    // minimum phi for box generator
  Double_t BoxPhiMax  = 360.;   // maximum   "       "
  Bool_t   BoxCosTht  = false;  // isotropic in cos(theta) instead theta
  Bool_t   BoxPt	  = false;  // is pt given instead of p

  Int_t    BoxType    = 13;     // default particle muon
  Int_t    BoxMult    = 1;      // default particle multiplicity
  Double_t type=0,mult=0;       // ref. parameters for range function

  BoxConfig.ToLower();

  if (BoxConfig!="box")
  {
    BoxConfig.ReplaceAll("box","");
    BoxConfig.ReplaceAll(" ","");
    BoxConfig += ":";

    while (BoxConfig.Contains(":"))
    {
      TString curpar = BoxConfig(0,BoxConfig.Index(":"));
      BoxConfig = BoxConfig(BoxConfig.Index(":")+1,1000);
      curpar.ReplaceAll("[","(");
      curpar.ReplaceAll("]",")");

      if (curpar.BeginsWith("type(")) {GetRange(curpar,type,mult); BoxType = (Int_t)type; BoxMult = (Int_t)mult; }
      if (curpar.BeginsWith("p("))    GetRange(curpar,BoxMomMin,BoxMomMax);
      if (curpar.BeginsWith("pt("))    {GetRange(curpar,BoxMomMin,BoxMomMax); BoxPt = true;}
      if (curpar.BeginsWith("tht("))   GetRange(curpar,BoxThtMin,BoxThtMax);
      if (curpar.BeginsWith("ctht(")) {GetRange(curpar,BoxThtMin,BoxThtMax); BoxCosTht=true;}
      if (curpar.BeginsWith("phi("))   GetRange(curpar,BoxPhiMin,BoxPhiMax);
    }
  }

  PndBoxGenerator* boxGen = new PndBoxGenerator(BoxType, BoxMult);
  boxGen->SetDebug(0);

  if (BoxPt == true){
	  boxGen->SetPtRange(BoxMomMin, BoxMomMax);
  } else {
	  boxGen->SetPRange(BoxMomMin,BoxMomMax);      // GeV/c
  }
  boxGen->SetPhiRange(BoxPhiMin, BoxPhiMax);   // Azimuth angle range [degree]
  boxGen->SetThetaRange(BoxThtMin, BoxThtMax); // Polar angle in lab system range [degree]

  if (BoxCosTht) boxGen->SetCosTheta();

  boxGen->SetXYZ(0., 0., 0.); //cm

  LOG(INFO) << "Using PndBoxGenerator(" << GetBeamMom() <<", pdg="<<BoxType<<" mult="<<BoxMult
	    <<" ) generator with range p["<<BoxMomMin<<","<<BoxMomMax<<"]  tht["<<BoxThtMin<<","<<BoxThtMax<<"]"<<(BoxCosTht?"*":"")<<"  phi["<<BoxPhiMin<<","<<BoxPhiMax<<"]" << FairLogger::endl;

  //  cout <<"BOX generator range: p["<<BoxMomMin<<","<<BoxMomMax<<"]  tht["<<BoxThtMin<<","<<BoxThtMax<<"]"<<(BoxCosTht?"*":"")<<"  phi["<<BoxPhiMin<<","<<BoxPhiMax<<"]"<<endl;

  fGen->AddGenerator(boxGen);
}

void PndMasterRunSim::UsePiPiGenerator(TString pipiConfig)
{
  // use BOX generator; defaults

  Double_t cosThetaMin = 0.0;
  Double_t cosThetaMax = 1.0;

  pipiConfig.ToLower();

  if (pipiConfig!="pipi")
  {
    pipiConfig.ReplaceAll("pipi","");
    pipiConfig.ReplaceAll(" ","");
    pipiConfig += ":";

    while (pipiConfig.Contains(":"))
    {
      TString curpar = pipiConfig(0,pipiConfig.Index(":"));
      pipiConfig = pipiConfig(pipiConfig.Index(":")+1,1000);
      curpar.ReplaceAll("[","(");
      curpar.ReplaceAll("]",")");

      if (curpar.BeginsWith("cosTheta(")) GetRange(curpar, cosThetaMin, cosThetaMax);
    }
  }

  PndPiPiGenerator* pipiGen = new PndPiPiGenerator();
  pipiGen->SetBeamMom(GetBeamMom());
  pipiGen->SetCosThetaMin(cosThetaMin);
  pipiGen->SetCosThetaMax(cosThetaMax);


  LOG(INFO) << "Using PndPiPiGenerator(BeamMom = " << GetBeamMom() << " GeV/c, cosThetaRange = "<< cosThetaMin << " : " << cosThetaMax << ")" << FairLogger::endl;

  //  cout <<"BOX generator range: p["<<BoxMomMin<<","<<BoxMomMax<<"]  tht["<<BoxThtMin<<","<<BoxThtMax<<"]"<<(BoxCosTht?"*":"")<<"  phi["<<BoxPhiMin<<","<<BoxPhiMax<<"]"<<endl;

  fGen->AddGenerator(pipiGen);
}

void PndMasterRunSim::UseLepLepGenerator(TString leplepConfig)
{
  // use BOX generator; defaults

  Double_t cosThetaMin = 0.0;
  Double_t cosThetaMax = 1.0;
  Double_t pid = -1.0;
  Double_t GeGmRatio = -1.0;
  Double_t dummy = 1.0;

  leplepConfig.ToLower();

  if (leplepConfig!="")
  {
    leplepConfig.ReplaceAll("leplep","");
    leplepConfig.ReplaceAll(" ","");
    leplepConfig += ":";

    while (leplepConfig.Contains(":"))
    {
      TString curpar = leplepConfig(0,leplepConfig.Index(":"));
      leplepConfig = leplepConfig(leplepConfig.Index(":")+1,1000);
      curpar.ReplaceAll("[","(");
      curpar.ReplaceAll("]",")");

      if (curpar.BeginsWith("pid(")) GetRange(curpar, pid, dummy);
      if (curpar.BeginsWith("gegm(")) GetRange(curpar, GeGmRatio, dummy);
      if (curpar.BeginsWith("cosTheta(")) GetRange(curpar, cosThetaMin, cosThetaMax);
    }
  }

  PndLepLepGenerator* leplepGen = new PndLepLepGenerator();
  leplepGen->SetBeamMom(GetBeamMom());

  leplepGen->SetCosThetaMin(cosThetaMin);
  leplepGen->SetCosThetaMax(cosThetaMax);


  LOG(INFO) << "Using PndleplepGenerator(BeamMom = " << GetBeamMom() << " GeV/c, cosThetaRange = "<< cosThetaMin << " : " << cosThetaMax << ")" << FairLogger::endl;

  //  cout <<"BOX generator range: p["<<BoxMomMin<<","<<BoxMomMax<<"]  tht["<<BoxThtMin<<","<<BoxThtMax<<"]"<<(BoxCosTht?"*":"")<<"  phi["<<BoxPhiMin<<","<<BoxPhiMax<<"]"<<endl;

  fGen->AddGenerator(leplepGen);
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
void PndMasterRunSim::UseFtfGenerator(TString ftfData)
{
  //if ( strncmp(fName,"TGeant4",7 ) == 0 ) LOG(FATAL) << "FTF does not run with Geant4 !!!"  << FairLogger::endl;
  if (ftfData.Contains(".root")){
	  LOG(INFO) << "Using PndFtfGenerator with input file " << ftfData << FairLogger::endl;
	  PndFtfGenerator* Ftf = new PndFtfGenerator(ftfData);
	  fGen->AddGenerator(Ftf);
  } else {
	  LOG(INFO) << "Using PndFtfDirect(anti_proton, G4_H, 1, ftfp, " << GetBeamMom() << ", " << gRandom->GetSeed() <<", "<<fFtfFlag<< ") generator" << FairLogger::endl;
	  PndFtfDirect *Ftf = new PndFtfDirect("anti_proton", "G4_H", 1, "ftfp", GetBeamMom(), gRandom->GetSeed(), fFtfFlag);
	  fGen->AddGenerator(Ftf);
  }
}

// -----   UseEvtGenGenerator   --------------------------------------------
void PndMasterRunSim::UseEvtGenGenerator(TString EvtGenFile)
{

  TString IniRes="";

  if (EvtGenFile.Contains(":")) // is the initial resonance provide as <decfile>.dec:iniRes ?
  {
    IniRes = EvtGenFile(EvtGenFile.Index(":")+1,1000);
    EvtGenFile = EvtGenFile(0,EvtGenFile.Index(":"));
  }

  if (IniRes=="") // we need to search the decay file
  {
    TString fnamepath=fInputDir+EvtGenFile;
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
  FILE *dec = fopen(fInputDir+EvtGenFile,"r");
  if (dec==NULL) LOG(FATAL) << "The EvtGen dec file does not exist!! " << EvtGenFile << FairLogger::endl;

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
  LOG(INFO) << "Using PndEvtGenDirect(" <<IniRes << ", " << (fInputDir+EvtGenFile).Data() << ", " << GetBeamMom() << ") generator" << FairLogger::endl;
  PndEvtGenDirect *EvtGen = new PndEvtGenDirect(IniRes, (fInputDir+EvtGenFile).Data(), GetBeamMom());
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
  if (gROOT->GetVersionInt() >= 60602) {
    gGeoManager->GetListOfVolumes()->Delete();
    gGeoManager->GetListOfShapes()->Delete();
    delete gGeoManager;
  }

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
  cout << "Output file is\t\t"    << fOutFile << endl;
  cout << "Parameter ROOT file is\t" << fParamRootFile << endl;
  cout << "Parameter ASCII file is\t" << fParamAsciiFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime
	    << "s" << endl;
  cout << "CPU usage " << cpuUsage*100. << "%" << endl;
  cout << "Max Memory " << maxMemory << " MB" << endl;

  cout << "Macro finished successfully." << endl;

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
