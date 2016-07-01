#include "PndMasterRunAna.h"

#include "PndMasterDigiTask.h"
#include "PndMasterRecoTask.h"
#include "PndMasterPidTask.h"
#include "PndFileNameCreator.h"
#include "PndEventCounterTask.h"

#include "FairFileSource.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "FairRuntimeDb.h"
#include "FairSystemInfo.h"
#include "FairLogger.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndMasterRunAna::PndMasterRunAna() :
  FairRunAna(), fInput(), fParamRootFile(), fParamAsciiFile(), fFriendFile1(), fFriendFile2(), fFriendFile3(), fFriendFile4(), fTimer(), fEventCounterRate(100)
{
  fTimer.Start();
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
  FairFileSource *fileSource = new FairFileSource(creator.GetSimFileName().data());
  if (fFriendFile1!="")
    {
      fFriendFile1 = creator.GetCustomFileName(fFriendFile1.Data());
      fileSource->AddFriend(fFriendFile1.Data());
    }
  if (fFriendFile2!="")
    {
      fFriendFile2 = creator.GetCustomFileName(fFriendFile2.Data());
      fileSource->AddFriend(fFriendFile2.Data());
    }
  if (fFriendFile3!="")
    {
      fFriendFile3 = creator.GetCustomFileName(fFriendFile3.Data());
      fileSource->AddFriend(fFriendFile3.Data());
    }
  if (fFriendFile4!="")
    {
      fFriendFile4 = creator.GetCustomFileName(fFriendFile4.Data());
      fileSource->AddFriend(fFriendFile4.Data());
    }
  SetSource(fileSource);

  // This set the output file name
  SetOutputFile(creator.GetCustomFileName(fOutFile.Data()).data());
  // This set the string for the output file name, used by Finish()
  SetOutput(creator.GetCustomFileName(fOutFile.Data()));
  SetParamRootFile(creator.GetParFileName().data()); 
  SetGenerateRunInfo(kFALSE);  
  SetUseFairLinks(kTRUE); 
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
  PndMasterDigiTask *digi = new PndMasterDigiTask();
  if (!pers) digi->SetPersistency(kFALSE);
  AddTask(digi);
}

// -----   AddRecoTasks   ---------------------------------------------------
void PndMasterRunAna::AddRecoTasks(Bool_t pers)
{
  PndMasterRecoTask *reco = new PndMasterRecoTask();
  if (!pers) reco->SetPersistency(kFALSE);
  AddTask(reco);
}

// -----   AddPidTasks   ----------------------------------------------------
void PndMasterRunAna::AddPidTasks(Bool_t pers)
{
  PndMasterPidTask *pid = new PndMasterPidTask();
  if (!pers) pid->SetPersistency(kFALSE);
  AddTask(pid);
}

// -----   Finish   ---------------------------------------------------------
void PndMasterRunAna::Finish()
{
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
  cout << "Output file is\t\t"    << fOutFile << endl;
  if (fFriendFile1!="") cout << "Friend file is\t\t"    << fFriendFile1 << endl;
  if (fFriendFile2!="") cout << "Friend file is\t\t"    << fFriendFile2 << endl;
  if (fFriendFile3!="") cout << "Friend file is\t\t"    << fFriendFile3 << endl;
  if (fFriendFile4!="") cout << "Friend file is\t\t"    << fFriendFile4 << endl;
  
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
