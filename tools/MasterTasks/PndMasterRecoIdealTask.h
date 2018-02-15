/**
 * @class PndMasterRecoIdealTask
 * @brief The default reconstruction tasks
 * @details # Master Reconstruction Task Class
 * This class includes all the reconstruction tasks which need to be used in the default reconstruction macros. 
 * @remark If you find some obsolete task which needs to be changed, contact the computing coordinator.
 * @author Stefano Spataro <spataro@to.infn.it>, Torino University
 * @version 1.0
 * @date Feb 1, 2016
 **
 **/

#ifndef PNDMASTERRECOIDEALTASK_H
#define PNDMASTERRECOIDEALTASK_H

#include "PndMasterTask.h"

class TClonesArray;

class PndMasterRecoIdealTask : public PndMasterTask
{
 public:

  /**
   * @brief Default constructor with options
   * @detail This string can be:
   * ""                          -> default settings full setup
   * "day1"                      -> Setup for day1 experiment: no GEM, FTS1234, NO DISC, NO RICH
   * "gem" (added to "day1")     -> Setup for day1 experiment with 3 GEM planes
   * "fts1256" (added to "day1") -> Setup for day1 experiment with FTS1256 insted of FTS1234
   * Example: "day1+gem+fts1256" means day1 setup + GEM planes + fst1256 
   */
  PndMasterRecoIdealTask(TString fOptions="");
  
  /**
   * @brief Destructor 
   */
  virtual ~PndMasterRecoIdealTask();

  /** 
   * @brief Set the persistency of all the tasks 
   * @param pers Persistency level: 0 no TCA, 1 all TCA
   */
  virtual void SetPersistency(Bool_t pers = kTRUE);
  
 private:

  /**
   * @brief struct of the task list
   * @detail This struct avoids to retrieve tasks using the integer value (misleading), but uses an easier scheme. The enum is "k" + the class name, i.e. "kPndTrkTracking2" for the class PndTracking2. The function PrintTaskList() can be used to check the list of the tasks and the corresponding number.
   * @remarks There are 4 kPndMCTrackAssociatorTask  and 2 kPndRecoKalmanTask, called kPndMCTrackAssociator1 kPndMCTrackAssociator2 etc...
   */
  struct recoIdealTaskList {
    Short_t kPndBarrelIdealTracking;
    Short_t kPndRecoKalmanTask1;
    Short_t kPndFtsTrackerIdeal;
    Short_t kPndRecoKalmanTask2;
  } reco;
  
  TString fOptions;          ///< Options parsed to the reconstruction
   
  /** @cond CLASSIMP */
  ClassDef(PndMasterRecoIdealTask,2);
  /** @endcond */
};

#endif /* PNDMASTERRECOTASK_H */
