/**
 * @class PndMasterRecoTask
 * @brief The default reconstruction tasks
 * @details # Master Reconstruction Task Class
 * This class includes all the reconstruction tasks which need to be used in the default reconstruction macros. 
 * @remark If you find some obsolete task which needs to be changed, contact the computing coordinator.
 * @author Stefano Spataro <spataro@to.infn.it>, Torino University
 * @version 1.0
 * @date Feb 1, 2016
 **
 **/

#ifndef PNDMASTERRECOTASK_H
#define PNDMASTERRECOTASK_H

#include "PndMasterTask.h"

class TClonesArray;

class PndMasterRecoTask : public PndMasterTask
{
 public:

    /** Default constructor **/
    PndMasterRecoTask();

    /** Destructor **/
    virtual ~PndMasterRecoTask();

 private:

    /**
     * @brief enum of the task list
     * @detail This enum avoids to retrieve tasks using the integer value (misleading), but uses an easier scheme. The enum is "k" + the class name, i.e. "kPndTrkTracking2" for the class PndTracking2. The function PrintTaskList() can be used to check the list of the tasks and the corresponding number.
     * @remarks There are 4 kPndMCTrackAssociatorTask  and 2 kPndRecoKalmanTask, called kPndMCTrackAssociator1 kPndMCTrackAssociator2 etc...
     **/
    enum recoTaskList {
      kFairGeane,
      kPndTrkTracking2,
      kPndSttMvdGemTracking,
      kPndMCTrackAssociator1,
      kPndRecoKalmanTask1,
      kPndMCTrackAssociator2,
      kPndFtsTrackerIdeal,
      kPndMCTrackAssociator3,
      kPndRecoKalmanTask2,
      kPndMCTrackAssociator4,
      kPndEventCounterTask
    };
    
    /** @cond CLASSIMP */
    ClassDef(PndMasterRecoTask,1);
    /** @endcond */
};

#endif /* PNDMASTERRECOTASK_H */
