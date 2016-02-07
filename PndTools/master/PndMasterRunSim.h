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
   * @brief Set the event generator
   * @details # Master event generator
   * This call set the event generator according to the input name. If the input name
   * contains "dpm" it uses dpm, if it contains "ftf" then ftf, if ".dec" it runs evtgen
   * using the input name as namefile of the .dec file. 
   */
  void SetGenerator();

   /** 
   * @brief Set the DPM flag
   * @param Mode = 0. - DPM - No elastic scattering, only inelastic
   * @param Mode = 1. - DPM - Elastic and inelastic interactions (default)
   * @param Mode = 2. - DPM - Only elastic scattering, no inelastic one
   */
  void SetDpmFlag(Int_t Mode)   { fDpmFlag = Mode; };
  
  /** 
   * @brief Use DPM as event generator
   */
  void UseDpmGenerator();
  
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
   * @brief Input of the simulation
   * @detail This string can be:
   * a) the name of the dec file for EvtGen, w/ or w/o .dec
   * b) "dpm" if you want to use dpm
   * c) "ftf" if you want to use ftf
   */  
  void SetInput(TString par)          { fInput          = par;}
  
  /**
   * @brief  Setter of the parameter root file 
   */
  void SetParamRootFile(TString par)  { fParamRootFile  = par;}
  
  /** 
   * @brief Setter of the parameter ascii file 
   */
  void SetParamAsciiFile(TString par) { fParamAsciiFile = par;}
  
  /** 
   * @brief Setter of the number of events
   */
  void SetNumberOfEvents(Int_t par) { fNEvents = par;}

  /** 
   * @brief Setter of the event counter rate
   */
  void SetEventCounterRate(Int_t par) { fEventCounterRate = par;}
  
 private:

  TString fInput;            ///< Name of the input for the simulation
  TString fOutFile;          ///< Name of the output file
  TString fParamRootFile;    ///< Name of the parameter root file
  TString fParamAsciiFile;   ///< Name of the parameter ascii file

  Int_t fDpmFlag;            ///< Flag for DPM event generator
  Int_t fNEvents;            ///< Number of events
  Int_t fEventCounterRate;   ///< After how many events the counter will print
  FairRuntimeDb *fRtdb;      ///< Runtime DB
  TStopwatch fTimer;         ///< Timer 
  
  /** @cond CLASSIMP */
  ClassDef(PndMasterRunSim,1);  ///< 1st Implementation -> 1
  /** @endcond */
  
};

#endif /* PNDMASTERRUNSIM_H */
