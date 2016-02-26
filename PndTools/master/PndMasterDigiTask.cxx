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
  Add(new PndSttHitProducerRealFast()); // 0
  if ((GetListOfTasks()->GetSize()-1) != kPndSttHitProducerRealFast) Error("PndMasterDigiTask","Error in task #%i", (GetListOfTasks()->GetSize()-1));
  
  // -----   MVD hit producers   ---------------------------------
  Add(new PndMvdDigiTask()); // 1
  if ((GetListOfTasks()->GetSize()-1) != kPndMvdDigiTask) Error("PndMasterDigiTask","Error in task #%i", (GetListOfTasks()->GetSize()-1));
  Add(new PndMvdClusterTask()); // 2
  if ((GetListOfTasks()->GetSize()-1) != kPndMvdClusterTask) Error("PndMasterDigiTask","Error in task #%i", (GetListOfTasks()->GetSize()-1));
  
  // -----   EMC hit producers   ---------------------------------
  Add(new PndEmcHitsToWaveform()); // 3
  if ((GetListOfTasks()->GetSize()-1) != kPndEmcHitsToWaveform) Error("PndMasterDigiTask","Error in task #%i", (GetListOfTasks()->GetSize()-1));
  ((PndEmcHitsToWaveform*)GetListOfTasks()->At(kPndEmcHitsToWaveform))->SetStorageOfData(kFALSE);
  Add(new PndEmcWaveformToDigi()); // 4
  if ((GetListOfTasks()->GetSize()-1) != kPndEmcWaveformToDigi) Error("PndMasterDigiTask","Error in task #%i", (GetListOfTasks()->GetSize()-1));
  Add(new PndEmcMakeCluster()); // 5
  if ((GetListOfTasks()->GetSize()-1) != kPndEmcMakeCluster) Error("PndMasterDigiTask","Error in task #%i", (GetListOfTasks()->GetSize()-1));
  Add(new PndEmcMakeBump()); // 6
  if ((GetListOfTasks()->GetSize()-1) != kPndEmcMakeBump) Error("PndMasterDigiTask","Error in task #%i", (GetListOfTasks()->GetSize()-1));
  
  // -----   SciT hit producers   -------------------------------
  Add(new PndSciTHitProducerIdeal()); // 7
  if ((GetListOfTasks()->GetSize()-1) != kPndSciTHitProducerIdeal) Error("PndMasterDigiTask","Error in task #%i", (GetListOfTasks()->GetSize()-1));
  
  // -----   MDT hit producers   ---------------------------------
  Add(new PndMdtHitProducerIdeal()); // 8
  if ((GetListOfTasks()->GetSize()-1) != kPndMdtHitProducerIdeal) Error("PndMasterDigiTask","Error in task #%i", (GetListOfTasks()->GetSize()-1));
  ((PndMdtHitProducerIdeal*)GetListOfTasks()->At(8))->SetPositionSmearing(.3); // position smearing [cm]
  Add(new PndMdtTrkProducer()); // 9
  if ((GetListOfTasks()->GetSize()-1) != kPndMdtTrkProducer) Error("PndMasterDigiTask","Error in task #%i", (GetListOfTasks()->GetSize()-1));
  
  // -----   DRC hit producers   ---------------------------------
  Add(new PndDrcHitProducerReal()); // 10
  if ((GetListOfTasks()->GetSize()-1) != kPndDrcHitProducerReal) Error("PndMasterDigiTask","Error in task #%i", (GetListOfTasks()->GetSize()-1));
  
  // -----   GEM hit producers   ---------------------------------
  Add(new PndGemDigitize("GEM Digitizer", 0)); // 11
  if ((GetListOfTasks()->GetSize()-1) != kPndGemDigitize) Error("PndMasterDigiTask","Error in task #%i", (GetListOfTasks()->GetSize()-1));
  Add(new PndGemFindHits("GEM Hit Finder", 0)); // 12
  if ((GetListOfTasks()->GetSize()-1) != kPndGemFindHits) Error("PndMasterDigiTask","Error in task #%i", (GetListOfTasks()->GetSize()-1));
  
  // -----   FTS hit producers   ---------------------------------
  Add(new PndFtsHitProducerRealFast());
  if ((GetListOfTasks()->GetSize()-1) != kPndFtsHitProducerRealFast) Error("PndMasterDigiTask","Error in task #%i", (GetListOfTasks()->GetSize()-1));
  
  // -----   Ftof hit producers   ---------------------------
  Add(new PndFtofHitProducerIdeal());
  if ((GetListOfTasks()->GetSize()-1) != kPndFtofHitProducerIdeal) Error("PndMasterDigiTask","Error in task #%i", (GetListOfTasks()->GetSize()-1));
  
  SetVerbose(0);
}
// -------------------------------------------------------------------------

/** Set the Persistency of all the tasks in the same way **/
void PndMasterDigiTask::SetPersistency(Bool_t pers)
{
  // -----   STT digi producers   --------------------------------
  ((PndSttHitProducerRealFast*)GetListOfTasks()->At(kPndSttHitProducerRealFast))->SetPersistence(pers);
  
  // -----   MVD hit producers   ---------------------------------
  ((PndMvdDigiTask*)GetListOfTasks()->At(kPndMvdDigiTask))->SetPersistance(pers);
  ((PndMvdClusterTask*)GetListOfTasks()->At(kPndMvdClusterTask))->SetPersistance(pers);
  
  // -----   EMC hit producers   ---------------------------------
  ((PndEmcHitsToWaveform*)GetListOfTasks()->At(kPndEmcHitsToWaveform))->SetStorageOfData(kFALSE);
  ((PndEmcWaveformToDigi*)GetListOfTasks()->At(kPndEmcWaveformToDigi))->SetStorageOfData(kFALSE);
  ((PndEmcMakeCluster*)GetListOfTasks()->At(kPndEmcMakeCluster))->SetStorageOfData(kFALSE);
  ((PndEmcMakeBump*)GetListOfTasks()->At(kPndEmcMakeBump))->SetStorageOfData(kFALSE);
  
  // -----   SciT hit producers   -------------------------------
  ((PndSciTHitProducerIdeal*)GetListOfTasks()->At(kPndSciTHitProducerIdeal))->SetPersistence(pers);
  
  // -----   MDT hit producers   ---------------------------------
  ((PndMdtHitProducerIdeal*)GetListOfTasks()->At(kPndMdtHitProducerIdeal))->SetPersistency(pers);
  ((PndMdtTrkProducer*)GetListOfTasks()->At(kPndMdtTrkProducer))->SetPersistency(pers);
 
  // -----   DRC hit producers   ---------------------------------
  ((PndDrcHitProducerReal*)GetListOfTasks()->At(kPndDrcHitProducerReal))->SetPersistency(pers);
  
  // -----   GEM hit producers   ---------------------------------
  ((PndGemDigitize*)GetListOfTasks()->At(kPndGemDigitize))->SetPersistency(pers);
  ((PndGemFindHits*)GetListOfTasks()->At(kPndGemFindHits))->SetPersistency(pers);
  
  // -----   FTS hit producers   ---------------------------------
  ((PndFtsHitProducerRealFast*)GetListOfTasks()->At(kPndFtsHitProducerRealFast))->SetPersistence(pers);
  
  // -----   Ftof hit producers   ---------------------------
  ((PndFtofHitProducerIdeal*)GetListOfTasks()->At(kPndFtofHitProducerIdeal))->SetPersistency(pers);
  
  return;
}

// -----   Destructor   ----------------------------------------------------
PndMasterDigiTask::~PndMasterDigiTask()
{
}
// -------------------------------------------------------------------------

/** @cond CLASSIMP */
ClassImp(PndMasterDigiTask);
/** @endcond */
