/**
 * @class PndMasterMultiPidTask
 * @brief The default pid tasks
 * @details # Master Pid Task Class
 * This class includes all the pid tasks which need to be used in the default pid macros.
 * @remark If you find some obsolete task which needs to be changed, contact the computing coordinator.
 * @author Stefano Spataro <spataro@to.infn.it>, Torino University
 * @version 1.0
 * @date Feb 1, 2016
 **
 **/

#ifndef PndMasterMultiPidTask_H
#define PndMasterMultiPidTask_H

#include "PndMasterTask.h"
#include <array>

class TClonesArray;

class PndMasterMultiPidTask : public PndMasterTask
{
  public:

    /**
     * @brief Default constructor with options
     * @detail This string can be:
     * ""                          -> default settings full setup
     * "day1"                      -> Setup for day1 experiment: no GEM, FTS1234, NO DISC, NO RICH
     * "gem" (added to "day1")     -> Setup for day1 experiment with 3 GEM planes
     * "fts1256" (added to "day1") -> Setup for day1 experiment with FTS1256 insted of FTS1234
     * "filtered"					 -> the option filtered was used in the reco stage to remove tracks with wrong Pz
     * Example: "day1+gem+fts1256" means day1 setup + GEM planes + fst1256
     */
    PndMasterMultiPidTask(TString options="");

    /**
     * @brief Destructor
     */
    virtual ~PndMasterMultiPidTask();

    /**
     * @brief Set the persistency of all the tasks
     * @param pers Persistency level: 0 no TCA, 1 all TCA
     */
    virtual void SetPersistency(Bool_t pers = kTRUE);

  private:

    /**
     * @brief struct of the task list
     * @detail This struct avoids to retrieve tasks using the integer value (misleading), but uses an easier scheme. The enum is "k" + the class name, i.e. "kPndPidCorrelator" for the class PndPidCorrelator. The function PrintTaskList() can be used to check the list of the tasks and the corresponding number.
     */
    struct pidTaskList
    {
      Short_t kPndPidCorrelator;
      Short_t kPndPidBremCorrector;
      Short_t kPndMcCloner;
      Short_t kPndPidIdealAssociatorTask;
      Short_t kPndPidMvdAssociatorTask;
      Short_t kPndPidMdtHCAssociatorTask;
      Short_t kPndPidDrcAssociatorTask;
      Short_t kPndPidDiscAssociatorTask;
      Short_t kPndPidSttAssociatorTask;
      Short_t kPndPidEmcBayesAssociatorTask;
      Short_t kPndPidSciTAssociatorTask;
      Short_t kPndPidRichAssociatorTask;
      Short_t kPndPidFtofAssociatorTask;
    };
    std::array<pidTaskList,5> fPidTasks;


    TString fOptions;           ///< Options parsed to the pid
    std::array<Bool_t,5> fHypoFlag;				//! Flag to check which hypotheses to do pid with

    /** @cond CLASSIMP */
    ClassDef(PndMasterMultiPidTask,2);
    /** @endcond */
};

#endif /* PndMasterMultiPidTask_H */

