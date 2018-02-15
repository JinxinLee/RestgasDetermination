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
#include "PndSciTDigiTask.h"
#include "PndMdtHitProducerIdeal.h"
#include "PndMdtTrkProducer.h"
#include "PndDrcHitProducerReal.h"
#include "PndGemDigitize.h"
#include "PndGemFindHits.h"
#include "PndFtsHitProducerRealFast.h"
#include "PndFtofHitProducerIdeal.h"
#include "PndRichHitProducer.h"


/**
 * @brief Default Constructor
 * @details # Digitization task list
 * Here all the digitization tasks are added to the task, with the standard settings. A check is done after each task if the tasklist enum is broken or not. At the end the event counter is added (each 100 events), and the verbosity is set to 0 to all the tasks (it can be changed afterwards with SetVerbosity() functions.
 **/
// -----   Default constructor   -------------------------------------------
PndMasterDigiTask::PndMasterDigiTask(TString options) :
  PndMasterTask("Master Digitization Task"), fOptions(options)
{
  digi = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
  
  // -----   STT digi producers   --------------------------------
  Add(new PndSttHitProducerRealFast()); // 0
  digi.kPndSttHitProducerRealFast = GetListOfTasks()->GetSize()-1;
  
  // -----   MVD hit producers   ---------------------------------
  Add(new PndMvdDigiTask()); // 1
  digi.kPndMvdDigiTask = GetListOfTasks()->GetSize()-1;
  Add(new PndMvdClusterTask()); // 2
  digi.kPndMvdClusterTask = GetListOfTasks()->GetSize()-1;
  
  // -----   EMC hit producers   ---------------------------------
  Add(new PndEmcHitsToWaveform()); // 3
  digi.kPndEmcHitsToWaveform = GetListOfTasks()->GetSize()-1;
  Add(new PndEmcWaveformToDigi()); // 4
  digi.kPndEmcWaveformToDigi = GetListOfTasks()->GetSize()-1;
  Add(new PndEmcMakeCluster()); // 5
  digi.kPndEmcMakeCluster = GetListOfTasks()->GetSize()-1;
  Add(new PndEmcMakeBump()); // 6
  digi.kPndEmcMakeBump = GetListOfTasks()->GetSize()-1;
  
  // -----   SciT hit producers   -------------------------------
  //Add(new PndSciTHitProducerIdeal()); // 7
  Add(new PndSciTDigiTask()); // 7
  digi.kPndSciTDigiTask = GetListOfTasks()->GetSize()-1;
  
  // -----   MDT hit producers   ---------------------------------
  Add(new PndMdtHitProducerIdeal()); // 8
  digi.kPndMdtHitProducerIdeal = GetListOfTasks()->GetSize()-1;
  ((PndMdtHitProducerIdeal*)GetListOfTasks()->At(digi.kPndMdtHitProducerIdeal))->SetPositionSmearing(.3); // position smearing [cm]
  Add(new PndMdtTrkProducer()); // 9
  digi.kPndMdtTrkProducer = GetListOfTasks()->GetSize()-1;
  
  // -----   DRC hit producers   ---------------------------------
  Add(new PndDrcHitProducerReal()); // 10
  digi.kPndDrcHitProducerReal = GetListOfTasks()->GetSize()-1;
  
  // -----   GEM hit producers   ---------------------------------
  if ( (!fOptions.Contains("day1")) || (fOptions.Contains("gem")) )
    {
      Add(new PndGemDigitize("GEM Digitizer", 0)); // 11
      digi.kPndGemDigitize = GetListOfTasks()->GetSize()-1;
      Add(new PndGemFindHits("GEM Hit Finder", 0)); // 12
      digi.kPndGemFindHits = GetListOfTasks()->GetSize()-1;
    }
  
  // -----   FTS hit producers   ---------------------------------
  Add(new PndFtsHitProducerRealFast()); // 13
  digi.kPndFtsHitProducerRealFast = GetListOfTasks()->GetSize()-1;
   
  // -----   Ftof hit producers   ---------------------------
  Add(new PndFtofHitProducerIdeal()); // 14
  digi.kPndFtofHitProducerIdeal = GetListOfTasks()->GetSize()-1;
 
  // -----   Rich hit producers   ---------------------------
  if ( !fOptions.Contains("day1") ) 
    {
      Add(new PndRichHitProducer()); // 15
      digi.kPndRichHitProducer = GetListOfTasks()->GetSize()-1;
    }
  
  SetVerbose(0);
}
// -------------------------------------------------------------------------

/** Set the Persistency of all the tasks in the same way **/
void PndMasterDigiTask::SetPersistency(Bool_t pers)
{
  // -----   STT digi producers   --------------------------------
  ((PndSttHitProducerRealFast*)GetListOfTasks()->At(digi.kPndSttHitProducerRealFast))->SetPersistence(pers);
  
  // -----   MVD hit producers   ---------------------------------
  ((PndMvdDigiTask*)GetListOfTasks()->At(digi.kPndMvdDigiTask))->SetPersistance(pers);
  ((PndMvdClusterTask*)GetListOfTasks()->At(digi.kPndMvdClusterTask))->SetPersistance(pers);
  
  // -----   EMC hit producers   ---------------------------------
  ((PndEmcHitsToWaveform*)GetListOfTasks()->At(digi.kPndEmcHitsToWaveform))->SetStorageOfData(kFALSE);
  ((PndEmcWaveformToDigi*)GetListOfTasks()->At(digi.kPndEmcWaveformToDigi))->SetStorageOfData(kFALSE);
  ((PndEmcMakeCluster*)GetListOfTasks()->At(digi.kPndEmcMakeCluster))->SetStorageOfData(kFALSE);
  ((PndEmcMakeBump*)GetListOfTasks()->At(digi.kPndEmcMakeBump))->SetStorageOfData(kFALSE);
  
  // -----   SciT hit producers   -------------------------------
  //((PndSciTHitProducerIdeal*)GetListOfTasks()->At(digi.kPndSciTHitProducerIdeal))->SetPersistence(pers);
  ((PndSciTDigiTask*)GetListOfTasks()->At(digi.kPndSciTDigiTask))->SetPersistence(pers);
  
  // -----   MDT hit producers   ---------------------------------
  ((PndMdtHitProducerIdeal*)GetListOfTasks()->At(digi.kPndMdtHitProducerIdeal))->SetPersistency(pers);
  ((PndMdtTrkProducer*)GetListOfTasks()->At(digi.kPndMdtTrkProducer))->SetPersistency(pers);
 
  // -----   DRC hit producers   ---------------------------------
  ((PndDrcHitProducerReal*)GetListOfTasks()->At(digi.kPndDrcHitProducerReal))->SetPersistency(pers);

  if ( (!fOptions.Contains("day1")) || (fOptions.Contains("gem")) )
    {
      // -----   GEM hit producers   ---------------------------------
      ((PndGemDigitize*)GetListOfTasks()->At(digi.kPndGemDigitize))->SetPersistency(pers);
      ((PndGemFindHits*)GetListOfTasks()->At(digi.kPndGemFindHits))->SetPersistency(pers);
    }
  
  // -----   FTS hit producers   ---------------------------------
  ((PndFtsHitProducerRealFast*)GetListOfTasks()->At(digi.kPndFtsHitProducerRealFast))->SetPersistence(pers);
  
  // -----   Ftof hit producers   ---------------------------
  ((PndFtofHitProducerIdeal*)GetListOfTasks()->At(digi.kPndFtofHitProducerIdeal))->SetPersistency(pers);

   if ( (!fOptions.Contains("day1")) || (fOptions.Contains("gem")) )
     {
       // -----   Rich hit producers   ---------------------------
       ((PndRichHitProducer*)GetListOfTasks()->At(digi.kPndRichHitProducer))->SetPersistency(pers);
     }
   
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
