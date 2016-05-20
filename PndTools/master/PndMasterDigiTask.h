/**
 * @class PndMasterDigiTask
 * @brief The default digitization tasks
 * @details # Master Digitization Task Class
 * This class includes all the digitization tasks which need to be used in the default digitization macros. 
 * @remark If you find some obsolete task which needs to be changed, contact the computing coordinator.
 * @author Stefano Spataro <spataro@to.infn.it>, Torino University
 * @version 1.0
 * @date Feb 1, 2016
 **
 **/

#ifndef PNDMASTERDIGITASK_H
#define PNDMASTERDIGITASK_H

#include "PndMasterTask.h"

class TClonesArray;

class PndMasterDigiTask : public PndMasterTask
{
 public:

  /**
   * @brief Default constructor 
   */
  PndMasterDigiTask();
  
  /** 
   * @brief Destructor 
   */
  virtual ~PndMasterDigiTask();

  /** 
   * @brief Set the persistency of all the tasks 
   * @param pers Persistency level: 0 no TCA, 1 all TCA
   */
  virtual void SetPersistency(Bool_t pers = kTRUE);
  
 private:
  
  /**
   * @brief enum of the task list
   * @detail This enum avoids to retrieve tasks using the integer value (misleading), but uses an easier scheme. The enum is "k" + the class name, i.e. "kPndMvdDigiTask" for the class PndMvdDigiTask. The function PrintTaskList() can be used to check the list of the tasks and the corresponding number
   */
  enum digiTaskList {
    kPndSttHitProducerRealFast,
    kPndMvdDigiTask, kPndMvdClusterTask,
    kPndEmcHitsToWaveform, kPndEmcWaveformToDigi, kPndEmcMakeCluster, kPndEmcMakeBump,
    //kPndSciTHitProducerIdeal,
    kPndSciTDigiTask,
    kPndMdtHitProducerIdeal, kPndMdtTrkProducer,
    kPndDrcHitProducerReal,
    kPndGemDigitize, kPndGemFindHits,
    kPndFtsHitProducerRealFast,
    kPndFtofHitProducerIdeal
  };
  
  /** @cond CLASSIMP */
  ClassDef(PndMasterDigiTask,1);
  /** @endcond */
};

#endif /* PNDMASTERDIGITASK_H */
