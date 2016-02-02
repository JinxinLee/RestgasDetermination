#include "PndMasterRunAna.h"

#include "PndMasterDigiTask.h"
#include "PndMasterRecoTask.h"
#include "PndMasterPidTask.h"

#include "FairFileSource.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "FairRuntimeDb.h"
#include "FairSystemInfo.h"
#include "FairLogger.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndMasterRunAna::PndMasterRunAna() :
  FairRunAna(), fParamRootFile(), fParamAsciiFile(), fFriendFile1(), fFriendFile2(), fFriendFile3(), fFriendFile4(), fTimer() //, fInputFile()
{
  fTimer.Start();
}

// -----   Setup   ---------------------------------------------------------
Bool_t PndMasterRunAna::Setup()
{
  //FairFileSource *fFileSource = new FairFileSource(fInputFile);
  //this->SetSource(fFileSource);

  if (fFriendFile1!="") this->AddFriend(fFriendFile1);
  if (fFriendFile2!="") this->AddFriend(fFriendFile2);
  if (fFriendFile3!="") this->AddFriend(fFriendFile3);
  if (fFriendFile4!="") this->AddFriend(fFriendFile4);
  
  this->SetGenerateRunInfo(kFALSE);  
  this->SetUseFairLinks(kTRUE); 
  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
  allDigiFile += "/macro/params/";
  allDigiFile += fParamAsciiFile;
  
  FairRuntimeDb* rtdb = this->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(fParamRootFile.Data());
  
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);

  return kTRUE;
}

// -----   AddDigiTasks   ---------------------------------------------------
void PndMasterRunAna::AddDigiTasks()
{
  PndMasterDigiTask *digi = new PndMasterDigiTask();
  this->AddTask(digi);
}

// -----   AddRecoTasks   ---------------------------------------------------
void PndMasterRunAna::AddRecoTasks()
{
  PndMasterRecoTask *reco = new PndMasterRecoTask();
  this->AddTask(reco);
}

// -----   AddPidTasks   ----------------------------------------------------
void PndMasterRunAna::AddPidTasks()
{
  PndMasterPidTask *pid = new PndMasterPidTask();
  this->AddTask(pid);
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
ClassImp(PndMasterRunAna);
/** @endcond */
