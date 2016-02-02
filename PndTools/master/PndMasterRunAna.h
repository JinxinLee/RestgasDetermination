/**
 * @class PndMasterRunAna
 * @brief Abstract class for all the master task list classes
 * @details # Master task Class
 * This class is the basic for all the master task classes, digi, reco, pid and so on.
 * It provides basic functionalities, such as the possibility to set the Verbosity for each task separately or for all the tasks together, the possibility to getretrieve a Task in order to use some setter, and a print of all the included tasks 
 * @author Stefano Spataro <spataro@to.infn.it>, Torino University
 * @version 1.0
 * @date Feb 1, 2016
 **
 **/


#ifndef PNDMASTERRUNANA_H
#define PNDMASTERRUNANA_H

#include "FairRunAna.h"

#include "TStopwatch.h"

class PndMasterRunAna : public FairRunAna
{
 public:

    /** Default constructor **/
    PndMasterRunAna();

    /*
     * @brief Initial setup 
     * @details # Master Inital setup
     * This command set the source files, load the proper parameters,
     * and set the relevant flags. If something fails, it returns
     * a kFALSE value.
     */
    Bool_t Setup();

     /*
     * @brief Final diagnostics
     * @details # Master Final diagnostics
     * It prints CPU time, memory usage, used parameters, and eventually
     * send the information to CDash
     */
    void Finish();

     /*
     * @brief Add digitization tasks
     * @details # Add Master digi tasks
     * It calls PndMasterDigiTask, adding all the standard digitization tasks
     */
    void AddDigiTasks();

     /*
     * @brief Add reconstruction tasks
     * @details # Add Master reco tasks
     * It calls PndMasterRecoTask, adding all the standard reconstruction tasks
     */
    void AddRecoTasks();

     /*
     * @brief Add pid tasks
     * @details # Add Master pid tasks
     * It calls PndMasterPidTask, adding all the standard pid tasks
     */
    void AddPidTasks();
    
    /* /\** Setter of the input root file **\/ */
    //void SetInputFile(TString par)      { fInputFile      = par;}
    
    /** Setter of the parameter root file **/
    void SetParamRootFile(TString par)  { fParamRootFile  = par;}
    
    /** Setter of the parameter ascii file **/
    void SetParamAsciiFile(TString par) { fParamAsciiFile = par;}
    
    /** Setter of the 1st friend root file **/
    void SetFriendFile1(TString par)    { fFriendFile1    = par;}
    
    /** Setter of the 2nd friend root file **/
    void SetFriendFile2(TString par)    { fFriendFile2    = par;}
    
    /** Setter of the 3rd friend root file **/
    void SetFriendFile3(TString par)    { fFriendFile3    = par;}
    
    /** Setter of the 4th friend root file **/
    void SetFriendFile4(TString par)    { fFriendFile4    = par;}
     
 private:

    
    //TString fInputFile;        //< Name of the input root file
    TString fParamRootFile;    //< Name of the parameter root file
    TString fParamAsciiFile;   //< Name of the parameter ascii file
    TString fFriendFile1;      //< Name of the 1st friend root file
    TString fFriendFile2;      //< Name of the 2nd friend root file
    TString fFriendFile3;      //< Name of the 3rd friend root file
    TString fFriendFile4;      //< Name of the 4th friend root file 
    
    TStopwatch fTimer;         //< Timer 
    
    /** @cond CLASSIMP */
    ClassDef(PndMasterRunAna,1);  ///< 1st Implementation -> 1
    /** @endcond */

};

#endif /* PNDMASTERTASK_H */
