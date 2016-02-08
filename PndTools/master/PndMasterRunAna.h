/**
 * @class PndMasterRunAna
 * @brief Class for the master reconstruction chain
 * @details # Master Tasks Class
 * This class is the basic for all the reconstruction steps, digi, reco, pid and so on.
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

  /**
   * @brief Default constructor 
   */
  PndMasterRunAna();
  
  /**
   * @brief Initial setup 
   * @details # Master Inital setup
   * This command set the source files, load the proper parameters,
   * and set the relevant flags. If something fails, it returns
   * a kFALSE value.
   */
  Bool_t Setup();
  
  /**
   * @brief Final diagnostics
   * @details # Master Final diagnostics
   * It prints CPU time, memory usage, used parameters, and eventually
   * send the information to CDash
   */
  void Finish();
  
  /**
   * @brief Add digitization tasks
   * @details # Add Master digi tasks
   * It calls PndMasterDigiTask, adding all the standard digitization tasks
   */
  void AddDigiTasks();
  
  /**
   * @brief Add reconstruction tasks
   * @details # Add Master reco tasks
   * It calls PndMasterRecoTask, adding all the standard reconstruction tasks
   */
  void AddRecoTasks();

  /**
   * @brief Add pid tasks
   * @details # Add Master pid tasks
   * It calls PndMasterPidTask, adding all the standard pid tasks
   */
  void AddPidTasks();
  
  /**
   * @brief Input of the macro
   */  
  void SetInput(TString par)          { fInput          = par;}

  /**
   * @brief Tag of the output file of the macro
   */  
  void SetOutput(TString par)         { fOutFile        = par;}
  
  /**
   * @brief Setter of the parameter root file
   */
  void SetParamRootFile(TString par)  { fParamRootFile  = par;}
  
  /**
   * @brief Setter of the parameter ascii file 
   */
  void SetParamAsciiFile(TString par) { fParamAsciiFile = par;}

  /**
   * @brief Setter of the first friend root file
   */
  void SetFriend1(TString par)        { fFriendFile1    = par;}

  /**
   * @brief Setter of the 2nd friend root file
   */
  void SetFriend2(TString par)        { fFriendFile2    = par;}
  
  /**
   * @brief Setter of the 3rd friend root file 
   */
  void SetFriend3(TString par)        { fFriendFile3    = par;}
  
  /**
   * @brief Setter of the 4th friend root file 
   */
  void SetFriend4(TString par)        { fFriendFile4    = par;}
  
 private:
  
  TString fInput;            ///< Name of the input for the simulation
  TString fOutFile;          ///< Name of the output file
  TString fParamRootFile;    ///< Name of the parameter root file
  TString fParamAsciiFile;   ///< Name of the parameter ascii file
  TString fFriendFile1;      ///< Name of the 1st friend root file
  TString fFriendFile2;      ///< Name of the 2nd friend root file
  TString fFriendFile3;      ///< Name of the 3rd friend root file
  TString fFriendFile4;      ///< Name of the 4th friend root file 
  
  TStopwatch fTimer;         ///< Timer 
  
  /** @cond CLASSIMP */
  ClassDef(PndMasterRunAna,1);  ///< 1st Implementation -> 1
  /** @endcond */
  
};

#endif /* PNDMASTERRUNANA_H */
