// -------------------------------------------------------------------------
// -----                    PndMasterDigiTask source file              -----
// -----                   Created 31/01/16  by S. Spataro             -----
// -----                   Wrapper for  digitizing tasks               -----
// -------------------------------------------------------------------------


#include "PndMasterDigiTask.h"
#include "PndMasterTask.h"

#include "PndSttHitProducerRealFast.h"
#include "PndMvdDigiTask.h"
#include "PndMvdClusterTask.h"
#include "PndEmcHitsToWaveform.h"
#include "PndEmcWaveformToDigi.h"
#include "PndEmcMakeCluster.h"
#include "PndEmcMakeBump.h"
#include "PndSciTHitProducerIdeal.h"
#include "PndMdtHitProducerIdeal.h"
#include "PndMdtTrkProducer.h"
#include "PndDrcHitProducerReal.h"
#include "PndGemDigitize.h"
#include "PndGemFindHits.h"
#include "PndFtsHitProducerRealFast.h"
#include "PndFtofHitProducerIdeal.h"
#include "PndEventCounterTask.h"


/**
 * @brief Default Constructor
 * @details # Digitization task list
 * Here all the digitization tasks are added to the task, with the standard settings. A check is done after each task if the tasklist enum is broken or not. At the end the event counter is added (each 100 events), and the verbosity is set to 0 to all the tasks (it can be changed afterwards with SetVerbosity() functions.
 **/
// -----   Default constructor   -------------------------------------------
PndMasterDigiTask::PndMasterDigiTask() :
  PndMasterTask("Master Digitization Task")
{
  // -----   STT digi producers   --------------------------------
  this->Add(new PndSttHitProducerRealFast()); // 0
  if ((this->GetListOfTasks()->GetSize()-1) != kPndSttHitProducerRealFast) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  
  // -----   MVD hit producers   ---------------------------------
  this->Add(new PndMvdDigiTask()); // 1
  if ((this->GetListOfTasks()->GetSize()-1) != kPndMvdDigiTask) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  this->Add(new PndMvdClusterTask()); // 2
  if ((this->GetListOfTasks()->GetSize()-1) != kPndMvdClusterTask) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  
  // -----   EMC hit producers   ---------------------------------
  this->Add(new PndEmcHitsToWaveform()); // 3
  if ((this->GetListOfTasks()->GetSize()-1) != kPndEmcHitsToWaveform) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  ((PndEmcHitsToWaveform*)this->GetListOfTasks()->At(kPndEmcHitsToWaveform))->SetStorageOfData(kFALSE);
  this->Add(new PndEmcWaveformToDigi()); // 4
  if ((this->GetListOfTasks()->GetSize()-1) != kPndEmcWaveformToDigi) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  this->Add(new PndEmcMakeCluster()); // 5
  if ((this->GetListOfTasks()->GetSize()-1) != kPndEmcMakeCluster) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  this->Add(new PndEmcMakeBump()); // 6
  if ((this->GetListOfTasks()->GetSize()-1) != kPndEmcMakeBump) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  
  // -----   SciT hit producers   -------------------------------
  this->Add(new PndSciTHitProducerIdeal()); // 7
  if ((this->GetListOfTasks()->GetSize()-1) != kPndSciTHitProducerIdeal) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  
  // -----   MDT hit producers   ---------------------------------
  this->Add(new PndMdtHitProducerIdeal()); // 8
  if ((this->GetListOfTasks()->GetSize()-1) != kPndMdtHitProducerIdeal) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  ((PndMdtHitProducerIdeal*)this->GetListOfTasks()->At(8))->SetPositionSmearing(.3); // position smearing [cm]
  this->Add(new PndMdtTrkProducer()); // 9
  if ((this->GetListOfTasks()->GetSize()-1) != kPndMdtTrkProducer) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  
  // -----   DRC hit producers   ---------------------------------
  this->Add(new PndDrcHitProducerReal()); // 10
  if ((this->GetListOfTasks()->GetSize()-1) != kPndDrcHitProducerReal) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  
  // -----   GEM hit producers   ---------------------------------
  this->Add(new PndGemDigitize("GEM Digitizer", 0)); // 11
  if ((this->GetListOfTasks()->GetSize()-1) != kPndGemDigitize) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  this->Add(new PndGemFindHits("GEM Hit Finder", 0)); // 12
  if ((this->GetListOfTasks()->GetSize()-1) != kPndGemFindHits) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  
  // -----   FTS hit producers   ---------------------------------
  this->Add(new PndFtsHitProducerRealFast());
  if ((this->GetListOfTasks()->GetSize()-1) != kPndFtsHitProducerRealFast) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  
  // -----   Ftof hit producers   ---------------------------
  this->Add(new PndFtofHitProducerIdeal());
  if ((this->GetListOfTasks()->GetSize()-1) != kPndFtofHitProducerIdeal) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  
  // -----   Event Counter   --------------------------------
  this->Add(new PndEventCounterTask("Event Counter", 0, 100));
  if ((this->GetListOfTasks()->GetSize()-1) != kPndEventCounterTask) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  
  SetVerbose(0);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMasterDigiTask::~PndMasterDigiTask()
{
}
// -------------------------------------------------------------------------

/** @cond CLASSIMP */
ClassImp(PndMasterDigiTask);
/** @endcond */
