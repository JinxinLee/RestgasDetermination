#include "PndMasterRunAna.h"

#include "PndMasterDigiTask.h"
#include "PndMasterDigiOnlyTask.h"
#include "PndMasterRecoTask.h"
#include "PndMasterLocalRecoTask.h"
#include "PndMasterRecoIdealTask.h"
#include "PndMasterPidTask.h"
#include "PndMasterMultiPidTask.h"
#include "PndFileNameCreator.h"
#include "PndEventCounterTask.h"

#include "FairFileSource.h"
#include "FairFileHeader.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "FairRuntimeDb.h"
#include "FairSystemInfo.h"
#include "FairLogger.h"
#include "FairGeane.h"
#include "TGeoManager.h"
#include "TROOT.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndMasterRunAna::PndMasterRunAna() :
  FairRunAna(), fInput(), fParamRootFile(), fParamAsciiFile(),
  fOptions(), fEventCounterRate(100), fNoGeane(kTRUE),
  fGenerateRunInfo(kFALSE), fUseFairLinks(kTRUE), fTimer()
{
  fTimer.Start();
}
// -----   Default destructor   -------------------------------------------
PndMasterRunAna::~PndMasterRunAna()
{
  if (gROOT->GetVersionInt() >= 60602 && gGeoManager!=NULL) {
    gGeoManager->GetListOfVolumes()->Delete();
    gGeoManager->GetListOfShapes()->Delete();
    delete gGeoManager;
  }
}

// -----   Setup   ---------------------------------------------------------
Bool_t PndMasterRunAna::Setup(TString outprefix)
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

  FairFileSource *fileSource;
  if (fFriendFiles.size() == 0) {
    fileSource = new FairFileSource(creator.GetSimFileName().data());
    fFriendFiles.push_back(creator.GetSimFileName().data());
  }
  else {
    fileSource = new FairFileSource(creator.GetCustomFileName(fFriendFiles[0].Data()));
    fFriendFiles[0] = creator.GetCustomFileName(fFriendFiles[0].Data());
  }

  for (unsigned int files = 1; files < fFriendFiles.size(); files++) {
    fileSource->AddFriend(creator.GetCustomFileName(fFriendFiles[files].Data()));
    fFriendFiles[files] = creator.GetCustomFileName(fFriendFiles[files].Data());
  }

  SetSource(fileSource);

  // This set the output file name
  SetOutputFile(creator.GetCustomFileName(fOutFile.Data()).data());
  // This set the string for the output file name, used by Finish()
  SetOutput(creator.GetCustomFileName(fOutFile.Data()));
  SetParamRootFile(creator.GetParFileName().data());
  SetGenerateRunInfo(fGenerateRunInfo);
  SetUseFairLinks(fUseFairLinks);
  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
  allDigiFile += "/macro/params/";
  allDigiFile += fParamAsciiFile;

  FairRuntimeDb* rtdb = GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(fParamRootFile.Data());

  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");

  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);

  // -----   Event Counter   --------------------------------
  AddTask(new PndEventCounterTask("Event Counter", 0, fEventCounterRate));

  return kTRUE;
}

// -----   AddDigiTasks   ---------------------------------------------------
void PndMasterRunAna::AddDigiTasks(Bool_t pers)
{
  PndMasterDigiTask *digi = new PndMasterDigiTask(fOptions);
  if (!pers) digi->SetPersistency(kFALSE);
  digi->SetPersistency(pers);
  AddTask(digi);
}

// -----   AddDigiTasks   ---------------------------------------------------
void PndMasterRunAna::AddDigiOnlyTasks(Bool_t pers)
{
  PndMasterDigiOnlyTask *digi = new PndMasterDigiOnlyTask(fOptions);
  if (!pers) digi->SetPersistency(kFALSE);
  digi->SetPersistency(pers);
  AddTask(digi);
}

// -----   AddRecoTasks   ---------------------------------------------------
void PndMasterRunAna::AddRecoTasks(Bool_t pers)
{
  // -----   Geane   ---------------------------------------
  if(fNoGeane) {
    AddTask(new FairGeane());
    fNoGeane=false;
  }
  PndMasterRecoTask *reco = new PndMasterRecoTask(fOptions);
  if (!pers) reco->SetPersistency(kFALSE);
  reco->SetPersistency(pers);
  AddTask(reco);
}

// -----   AddLocalRecoTasks   ---------------------------------------------------
void PndMasterRunAna::AddLocalRecoTasks(Bool_t pers)
{
  // -----   Geane   ---------------------------------------
  if(fNoGeane) {
    AddTask(new FairGeane());
    fNoGeane=false;
  }
  PndMasterLocalRecoTask *reco = new PndMasterLocalRecoTask(fOptions);
  if (!pers) reco->SetPersistency(kFALSE);
  reco->SetPersistency(pers);
  AddTask(reco);
}

// -----   AddRecoTasks   ---------------------------------------------------
void PndMasterRunAna::AddRecoIdealTasks(Bool_t pers)
{
  // -----   Geane   ---------------------------------------
  if(fNoGeane) {
    AddTask(new FairGeane());
    fNoGeane=false;
  }
  PndMasterRecoIdealTask *recoIdeal = new PndMasterRecoIdealTask(fOptions);
  if (!pers) recoIdeal->SetPersistency(kFALSE);
  recoIdeal->SetPersistency(pers);
  AddTask(recoIdeal);
}

// -----   AddPidTasks   ----------------------------------------------------
void PndMasterRunAna::AddPidTasks(Bool_t pers)
{
  if(fNoGeane) {
    AddTask(new FairGeane());
    fNoGeane=false;
  }
  if (fOptions.Contains("multikalman"))
  {
    PndMasterMultiPidTask *pid = new PndMasterMultiPidTask(fOptions);
    if (!pers) pid->SetPersistency(kFALSE);
    pid->SetPersistency(pers);
    AddTask(pid);
  } else {
    PndMasterPidTask *pid = new PndMasterPidTask(fOptions);
    if (!pers) pid->SetPersistency(kFALSE);
    pid->SetPersistency(pers);
    AddTask(pid);
  }
}

void PndMasterRunAna::PrintListOftTasks()
{
  cout<<"Tasks that ran just now:"<<endl;
  TFile* outfile=fRootManager->GetOutFile();
  bool wasopen=outfile->IsOpen ();
  if (!wasopen)
  {
    cout<<"file is "<< ((wasopen) ? "" : "not " ) <<"open" <<endl;
    outfile=new TFile(outfile->GetName(),"UPDATE");
  }
  FairFileHeader* outheader=(FairFileHeader*)outfile->Get("FileHeader");
  for(const auto&& os : *(outheader->GetListOfTasks()) ) cout<<" - "<<((TObjString*)os)->GetString().Data()<<endl;
}

// -----   Finish   ---------------------------------------------------------
void PndMasterRunAna::Finish()
{
  cout << endl;

  TFile* outfile=fRootManager->GetOutFile();
  bool wasopen=outfile->IsOpen ();
  if (!wasopen)
  {
    cout<<"file is "<< ((wasopen) ? "" : "not " ) <<"open" <<endl;
    outfile=new TFile(outfile->GetName(),"UPDATE");
  }
  outfile->cd();

  cout<<"PndMasterRunAna::Finish():";
  PrintListOftTasks();

  TObjString outoptions(fOptions);
  outoptions.Write("PndOptions",kOverwrite);

  outfile->Write();
  if(!wasopen) outfile->Close();

  //safety delete for newer ROOT
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
  for (auto files : fFriendFiles) {
    cout << "Friend file is\t\t" << files << endl;
  }


  cout << "Parameter ROOT file is\t" << fParamRootFile << endl;
  cout << "Parameter ASCII file is\t" << fParamAsciiFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime
       << "s" << endl;
  cout << "CPU usage " << cpuUsage*100. << "%" << endl;
  cout << "Max Memory " << maxMemory << " MB" << endl;

  cout << "Macro finished successfully." << endl;


}

/** @cond CLASSIMP */
ClassImp(PndMasterRunAna);
/** @endcond */

