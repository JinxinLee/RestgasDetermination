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

    /** Default constructor **/
    PndMasterDigiTask();

    /** Destructor **/
    virtual ~PndMasterDigiTask();

 private:

    /**
 * @brief enum of the task list
 * @detail This enum avoids to retrieve tasks using the integer value (misleading), but uses an easier scheme. The enum is "k" + the class name, i.e. "kPndMvdDigiTask" for the class PndMvdDigiTask. The function PrintTaskList() can be used to check the list of the tasks and the corresponding number
 **/
    enum digiTaskList {
      kPndSttHitProducerRealFast,
      kPndMvdDigiTask, kPndMvdClusterTask,
      kPndEmcHitsToWaveform, kPndEmcWaveformToDigi, kPndEmcMakeCluster, kPndEmcMakeBump,
      kPndSciTHitProducerIdeal,
      kPndMdtHitProducerIdeal, kPndMdtTrkProducer,
      kPndDrcHitProducerReal,
      kPndGemDigitize, kPndGemFindHits,
      kPndFtsHitProducerRealFast,
      kPndFtofHitProducerIdeal,
      kPndEventCounterTask
    };
    
    /** @cond CLASSIMP */
    ClassDef(PndMasterDigiTask,1);
    /** @endcond */
};

#endif /* PNDMASTERDIGITASK_H */
