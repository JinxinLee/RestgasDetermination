/**
 * @class PndMasterTask
 * @brief Abstract class for all the master task list classes
 * @details # Master task Class
 * This class is the basic for all the master task classes, digi, reco, pid and so on.
 * It provides basic functionalities, such as the possibility to set the Verbosity for each task separately or for all the tasks together, the possibility to getretrieve a Task in order to use some setter, and a print of all the included tasks 
 * @author Stefano Spataro <spataro@to.infn.it>, Torino University
 * @version 1.0
 * @date Feb 1, 2016
 **
 **/


#ifndef PNDMASTERTASK_H
#define PNDMASTERTASK_H

#include "PndBlackBoxTask.h"

class TClonesArray;

class PndMasterTask : public PndBlackBoxTask
{
 public:

    /** Default constructor **/
    PndMasterTask();
    
    /** Proper Constructor **/
    PndMasterTask(const char* name);

    /** Destructor **/
    virtual ~PndMasterTask();

    /** It prints the list of tasks **/
    void PrintTaskList();

    /** Set the Verbosity to all the tasks **/
    void SetVerbose(Int_t iVerbose = 1);

    /*
     * @brief Set the Verbosity to a single task
     * @details This function has to be used when you want to change the verbosity to a single task
     * 
     * @param nTask Index of the task you want to modify. Instead of the integer you can use the enum k + task name (i.e. kMvdDigiTask, hSttHitProducerRealFast
     * @param iVerbose Verbosity. 0 -> no messages; the higher -> more messages
     */
    void SetVerbose(Int_t nTask, Int_t iVerbose);

     /*
     * @brief Return the pointer to a single task
     * @details This function allows to retrieve the pointer of a task inside the task list, when you want to use some particular setter or change someting in the task.
     * 
     * @param nTask Index of the task you want to modify. Instead of the integer you can use the enum "k" + task name (i.e. kMvdDigiTask, kSttHitProducerRealFast, etc...)
     */   
    FairTask* GetTask(Int_t nTask);
    
 private:

    /** @cond CLASSIMP */
    ClassDef(PndMasterTask,1);  ///< 1st Implementation -> 1
    /** @endcond */

};

#endif /* PNDMASTERTASK_H */
