// --------------------------------------------------------------------------------
// -----                    PndMasterMultiPidTask source file                 -----
// -----                   Created 07/05/18  by W. Ikegami Andersson          -----
// -----                       Wrapper for pid tasks when running multikalman -----
// --------------------------------------------------------------------------------

#include "PndMasterMultiPidTask.h"
#include "PndMasterTask.h"

#include "PndPidCorrelator.h"
#include "PndPidBremCorrector.h"
#include "PndMcCloner.h"
#include "PndMcCloner2.h"
#include "PndPidIdealAssociatorTask.h"
#include "PndPidMvdAssociatorTask.h"
#include "PndPidMdtHCAssociatorTask.h"
#include "PndPidDrcAssociatorTask.h"
#include "PndPidDiscAssociatorTask.h"
#include "PndPidSttAssociatorTask.h"
#include "PndPidEmcBayesAssociatorTask.h"
#include "PndPidSciTAssociatorTask.h"
#include "PndPidFtofAssociatorTask.h"
#include "PndPidRichAssociatorTask.h"

#include <array>
/**
 * @brief Default Constructor
 * @details # Pid task list
 * Here all the pid tasks are added to the task, with the standard settings. A check is done after each task if the tasklist enum is broken or not. At the end the event counter is added (each 100 events), and the verbosity is set to 0 to all the tasks (it can be changed afterwards with SetVerbosity() functions.
 **/
// -----   Default constructor   -------------------------------------------
PndMasterMultiPidTask::PndMasterMultiPidTask(TString options) :
  PndMasterTask("Master Pid Task"), fOptions(options)
{
  std::array<Int_t,5> hypoPdg= {11,13,211,321,2212};
  std::array<TString,5> hypoName= {"Electron","Muon","Pion","Kaon","Proton"};

  unsigned int nfits = 0;
  if (fOptions.Contains("electron")) {
    nfits++;
    fHypoFlag[0] = true;
  }
  if (fOptions.Contains("muon")) {
    nfits++;
    fHypoFlag[1] = true;
  }
  if (fOptions.Contains("pion")) {
    nfits++;
    fHypoFlag[2] = true;
  }
  if (fOptions.Contains("kaon")) {
    nfits++;
    fHypoFlag[3] = true;
  }
  if (fOptions.Contains("proton")) {
    nfits++;
    fHypoFlag[4] = true;
  }
  if (nfits == 0) {
    std::cout<<" -I- PndMasterMultiPidTask: No hypotheses given, running pid with all 5 hypothesis" << std::endl;
    fHypoFlag[0] = true;
    fHypoFlag[1] = true;
    fHypoFlag[2] = true;
    fHypoFlag[3] = true;
    fHypoFlag[4] = true;
  }

  for (int iHyp=0; iHyp<5; iHyp++)
  {
    fPidTasks[iHyp] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

    // -----   Correlation   ---------------------------------
    PndPidCorrelator* correlator = new PndPidCorrelator();
    correlator->SetPidHyp(hypoPdg[iHyp]);
    correlator->SetOutputBranch(hypoName[iHyp]); // suffixes for branches
    //correlator->SetFlagCut(kFALSE);

    this->Add(correlator); // Adding to task list
    fPidTasks[iHyp].kPndPidCorrelator = GetListOfTasks()->GetSize()-1;

    TString brlbranchname="SttMvdGemGenTrack"+hypoName[iHyp];
    if (fOptions.Contains("nogem")||fOptions.Contains("gem0")) {
      brlbranchname="SttMvdGenTrack"+hypoName[iHyp];
    }
    if (fOptions.Contains("filtered")) brlbranchname+="_filtered";
    if (fOptions.Contains("fakeonline")) brlbranchname+="_fakeonline";
    //if ( (!fOptions.Contains("day1")) || (fOptions.Contains("gem")) )
    //{
    //} else

    // TODO: Go over day1 and PID!
    correlator->SetBarrelTrackBranch(brlbranchname);

    TString fwdbranchname="FtsIdealGenTrack"+hypoName[iHyp];
    if (fOptions.Contains("fakeonline")) fwdbranchname+="_fakeonline";
    correlator->SetForwardTrackBranch(fwdbranchname);

    if (fOptions.Contains("piddebug"))  correlator->SetDebugMode(kTRUE);
    if (fOptions.Contains("pidfast"))   correlator->SetFast(kTRUE);
    if (fOptions.Contains("pidnoswim")) correlator->SetBackPropagate(kFALSE);

    // -----   Bremsstrahlung Correction ----------------------
    PndPidBremCorrector *PidBrem = new PndPidBremCorrector();
    PidBrem->SetOutputBranch(hypoName[iHyp]);
    this->Add(PidBrem); // 2
    fPidTasks[iHyp].kPndPidBremCorrector = GetListOfTasks()->GetSize()-1;

    //  // -----   MC Cloner   ------------------------------------
    //  PndMcCloner *clone = new PndMcCloner();
    //  clone->SetOutputBranch(hypoName[iHyp]);
    //  this->Add(clone); // 3
    //  fPidTasks[iHyp].kPndMcCloner = GetListOfTasks()->GetSize()-1;
    //  // Option to clean the MCTrack TClonesArray from particles which were not interacting with sensitive detectors
    //  clone->SetCleanMc();

    //  // -----   Classifiers   ----------------------------------
    //  PndPidIdealAssociatorTask *PidIdeal = new PndPidIdealAssociatorTask();
    //  PidIdeal->SetOutputBranch(hypoName[iHyp]);
    //  this->Add(PidIdeal); // 4
    //  fPidTasks[iHyp].kPndPidIdealAssociatorTask = GetListOfTasks()->GetSize()-1;

    PndPidMvdAssociatorTask *PidMvd = new PndPidMvdAssociatorTask();
    PidMvd->SetOutputBranch(hypoName[iHyp]);
    this->Add(PidMvd); // 5
    fPidTasks[iHyp].kPndPidMvdAssociatorTask = GetListOfTasks()->GetSize()-1;

    PndPidMdtHCAssociatorTask *PidMdt = new PndPidMdtHCAssociatorTask();
    PidMdt->SetOutputBranch(hypoName[iHyp]);
    this->Add(PidMdt); // 6
    fPidTasks[iHyp].kPndPidMdtHCAssociatorTask = GetListOfTasks()->GetSize()-1;

    PndPidDrcAssociatorTask *PidDrc = new PndPidDrcAssociatorTask();
    PidDrc->SetOutputBranch(hypoName[iHyp]);
    this->Add(PidDrc); // 7
    fPidTasks[iHyp].kPndPidDrcAssociatorTask = GetListOfTasks()->GetSize()-1;

    if ( !fOptions.Contains("day1") && !fOptions.Contains("phase1") )
    {
      PndPidDiscAssociatorTask *PidDisc = new PndPidDiscAssociatorTask();
      PidDisc->SetOutputBranch(hypoName[iHyp]);
      this->Add(PidDisc); // 8
      fPidTasks[iHyp].kPndPidDiscAssociatorTask = GetListOfTasks()->GetSize()-1;
    }

    PndPidSttAssociatorTask *PidStt = new PndPidSttAssociatorTask();
    PidStt->SetOutputBranch(hypoName[iHyp]);
    this->Add(PidStt); // 9
    fPidTasks[iHyp].kPndPidSttAssociatorTask = GetListOfTasks()->GetSize()-1;

    PndPidEmcBayesAssociatorTask *PidEmcBayes = new PndPidEmcBayesAssociatorTask();
    PidEmcBayes->SetOutputBranch(hypoName[iHyp]);
    this->Add(PidEmcBayes); // 10
    fPidTasks[iHyp].kPndPidEmcBayesAssociatorTask = GetListOfTasks()->GetSize()-1;

    PndPidSciTAssociatorTask *PidSciT = new PndPidSciTAssociatorTask();
    PidSciT->SetOutputBranch(hypoName[iHyp]);
    this->Add(PidSciT); // 11
    fPidTasks[iHyp].kPndPidSciTAssociatorTask = GetListOfTasks()->GetSize()-1;

    PndPidFtofAssociatorTask *PidFtof = new PndPidFtofAssociatorTask();
    PidFtof->SetOutputBranch(hypoName[iHyp]);
    this->Add(PidFtof); // 12
    fPidTasks[iHyp].kPndPidFtofAssociatorTask = GetListOfTasks()->GetSize()-1;

    if ( !fOptions.Contains("day1") && !fOptions.Contains("phase1") )
    {
      PndPidRichAssociatorTask *PidRich = new PndPidRichAssociatorTask();
      PidRich->SetOutputBranch(hypoName[iHyp]);
      this->Add(PidRich); // 13
      fPidTasks[iHyp].kPndPidRichAssociatorTask = GetListOfTasks()->GetSize()-1;
    }
  }

  // -----   MC Cloner   ------------------------------------
  //cloner that goes through all candidates. Run only once, and after all correlators
  PndMcCloner2 *clone = new PndMcCloner2();
  this->Add(clone); // 3
  // Option to clean the MCTrack TClonesArray from particles which were not interacting with sensitive detectors
  clone->SetCleanMc();


  for (int iHyp=0; iHyp<5; iHyp++)
  {
    PndPidIdealAssociatorTask *PidIdeal = new PndPidIdealAssociatorTask();
    PidIdeal->SetOutputBranch(hypoName[iHyp]);
    this->Add(PidIdeal); // 4
    fPidTasks[iHyp].kPndPidIdealAssociatorTask = GetListOfTasks()->GetSize()-1;

  }

  SetVerbose(0);
}
// -------------------------------------------------------------------------

/** Set the Persistency of all the tasks in the same way **/
void PndMasterMultiPidTask::SetPersistency(Bool_t pers)
{
  if (!pers) LOG(INFO) << "It makes no sense to have pid persistency switched OFF!" << FairLogger::endl;
  if (!pers) LOG(INFO) << "Or, if you prefer... this functionality has not been implemented yet" << FairLogger::endl;

  return;
}

// -----   Destructor   ----------------------------------------------------
PndMasterMultiPidTask::~PndMasterMultiPidTask()
{
}
// -------------------------------------------------------------------------

/** @cond CLASSIMP */
ClassImp(PndMasterMultiPidTask);
/** @endcond */


