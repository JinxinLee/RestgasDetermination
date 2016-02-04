/**
 * @class PndMasterRunSim
 * @brief Class for the master simulation chain
 * @details # Master Simulation Class
 * This class is the basic for the simulation macro. It loads the environment and all the standard detectors.
 * @author Stefano Spataro <spataro@to.infn.it>, Torino University
 * @version 1.0
 * @date Feb 3, 2016
 **
 **/


#ifndef PNDMASTERRUNSIM_H
#define PNDMASTERRUNSIM_H

#include "FairRunSim.h"
#include "FairRuntimeDb.h"

#include "TStopwatch.h"
#include "TString.h"

class PndMasterRunSim : public FairRunSim
{
 public:
  
  /**
   * @brief Default constructor 
   */
  PndMasterRunSim();
  
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
   * @brief It creates all the standard geometry volumes
   * @details # Master Geometry List
   * It creates all the standard geometry volumes which have to be used in simulation. All the MCPoint will be stored, expect for EMC
   */
  void CreateGeometry();   

  /**
   * @brief Add simulation tasks
   * @details # Add Master simulation tasks
   * It adds all the standard simulation tasks
   */
  void AddSimTasks();
  
  /** 
   * @brief Use DPM as event generator
   * @details # DPM event generator
   * This call set DPM as event generator.
   *  @param Mode = 0. - No elastic scattering, only inelastic
   *  @param Mode = 1. - Elastic and inelastic interactions (default)
   *  @param Mode = 2. - Only elastic scattering, no inelastic one
   */
  void UseDpmGenerator(Int_t Mode = 1);
  
  /** 
   * @brief Use FTF as event generator
   * @details # FTF event generator
   * This call set FTF as event generator.
   */
  void UseFtfGenerator();

  /** 
   * @brief Use EvtGen as event generator
   * @details # EvtGen event generator
   * This call set EvtGen as event generator. The user should set the 
   * .dec file, and the function will retrieve automaticall beam 
   * momentum and initial state.
   * @param fEvtGenFile Filename of the .dec file
   */
  void UseEvtGenGenerator(TString fEvtGenFile);
  
  /** 
   * @brief Setter of the input root file 
   */
  //void SetInputFile(TString par)      { fInputFile      = par;}
  
  /**
   * @brief  Setter of the parameter root file 
   */
  void SetParamRootFile(TString par)  { fParamRootFile  = par;}
  
  /** 
   * @brief Setter of the parameter ascii file 
   */
  void SetParamAsciiFile(TString par) { fParamAsciiFile = par;}
  
 private:
  
  //TString fInputFile;        //< Name of the input root file
  TString fParamRootFile;    ///< Name of the parameter root file
  TString fParamAsciiFile;   ///< Name of the parameter ascii file
  
  FairRuntimeDb *fRtdb;      ///< Runtime DB
  TStopwatch fTimer;         ///< Timer 
  
  /** @cond CLASSIMP */
  ClassDef(PndMasterRunSim,1);  ///< 1st Implementation -> 1
  /** @endcond */
  
};

#endif /* PNDMASTERRUNSIM_H */
