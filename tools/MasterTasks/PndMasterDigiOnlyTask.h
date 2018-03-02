/**
 * @class PndMasterDigiOnlyTask
 * @brief The default digitization tasks
 * @details # Master Digitization Task Class
 * This class includes all the digitization tasks which need to be used in the default digitization macros.
 * @remark If you find some obsolete task which needs to be changed, contact the computing coordinator.
 * @author Stefano Spataro <spataro@to.infn.it>, Torino University
 * @version 1.0
 * @date Feb 1, 2016
 **
 **/

#ifndef PndMasterDigiOnlyTask_H
#define PndMasterDigiOnlyTask_H

#include "PndMasterTask.h"
#include "FairTask.h"
#include "PndPersistencyTask.h"

class TClonesArray;


class PndMasterDigiOnlyTask : public PndMasterTask
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
  PndMasterDigiOnlyTask(TString options="");

  /**
   * @brief Destructor
   */
  virtual ~PndMasterDigiOnlyTask();

  /**
   * @brief Set the persistency of all the tasks
   * @param pers Persistency level: 0 no TCA, 1 all TCA
   */
  virtual void SetPersistency(Bool_t pers = kTRUE);

 private:

  std::vector<PndPersistencyTask*> fBranchTasks;
  std::vector<FairTask*> fStandardTasks;
  std::map<PndPersistencyTask*, bool> fFixedPersistency;

  TString fOptions;          ///< Options parsed to the digitization

  /** @cond CLASSIMP */
  ClassDef(PndMasterDigiOnlyTask,1);
  /** @endcond */
};

#endif /* PndMasterDigiOnlyTask_H */
