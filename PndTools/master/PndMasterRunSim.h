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

class FairFilteredPrimaryGenerator;
class FairBoxGenerator;
class PndBoxGenerator;


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
  Bool_t Setup(TString outprefix="");
  
  /**
   * @brief Final diagnostics
   * @details # Master Final diagnostics
   * It prints CPU time, memory usage, used parameters, and eventually
   * send the information to CDash
   */
  void Finish();
  
  /**
   * @brief It switches between different standard geometry volumes
   * @details # Master Geometry List
   * According to fOptions, it creates the standard geometry volumes for the
   * full setup, or dedicated geometries (such as day1)
   */
  void CreateGeometry();
  
  /**
   * @brief It creates all the standard geometry volumes
   * @details # Master Geometry List
   * It creates all the standard geometry volumes which have to be used in simulation. 
   * All the MCPoint will be stored, expect for EMC.
   */
  void CreateGeometryDefault();
  
  /**
   * @brief It creates the standard geometry volumes for day1 phase
   * @details # Master Geometry List
   * It creates all the standard geometry volumes which have to be used in simulation,
   * with the setup for day1 experiments. All the MCPoint will be stored, expect for EMC
   */
  void CreateGeometryDay1();   

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
   * using the input name as namefile of the .dec file. If the input name contains
   * "box" the macro breaks, since in that case the SetGenerator(PndBoxGenerator *boxGen)
   * function must be used.
   */
  void SetGenerator();

  /** 
   * @brief Set the event generator for FairBoxGenerator
   * @details # Master event generator for FairBoxGenerator
   * This call set the FairBoxGenerator as event generator. The user should create a
   * FairBoxGenerator object with all the settings, and pass it as argument to the
   * function. 
   */
  void SetGenerator(FairBoxGenerator *boxGen);

  /** 
   * @brief Set the event generator for PndBoxGenerator
   * @details # Master event generator for PndBoxGenerator
   * This call set the PndBoxGenerator as event generator. The user should create a
   * PndBoxGenerator object with all the settings, and pass it as argument to the
   * function. 
   */
  void SetGenerator(PndBoxGenerator *boxGen);

   /** 
   * @brief Set the DPM flag
   * @param Mode = 0. - DPM - No elastic scattering, only inelastic
   * @param Mode = 1. - DPM - Elastic and inelastic interactions (default)
   * @param Mode = 2. - DPM - Only elastic scattering, no inelastic one
   */
  void SetDpmFlag(Int_t Mode)   { fDpmFlag = Mode; };
  
   /** 
   * @brief Set the FTF noelastic flag
   * @param Mode = 0. - FTF - Elastic and inelastic interactions (default)
   * @param Mode = 1. - FTF - No elastic scattering, only inelastic
   */
  void SetFtfFlag(Int_t Mode)   { fFtfFlag = Mode; };
  
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
   * @brief Use BoxGen as event generator
   * @details # Box event generator
   * This call sets BoxGenerator as event generator. 
   * The format of the config string is 
   *   for isotrop events in theta:      'BOX:type(pdg,mult):p(min,max):phi(min,max):tht(min,max)'
   *   for isotrop events in cos(theta): 'BOX:type(pdg,mult):p(min,max):phi(min,max):ctht(min,max)'
   * Instead of range 'var(min,max)' also a fixed value can be set with 'var(value)'
   * All variables left out are set to defaults.
   * @param fBoxConfig configuration string of the BOX generator
   */
  void UseBoxGenerator(TString fBoxConfig);

  /**
   * @brief Input of the simulation
   * @detail This string can be:
   * a) the name of the dec file for EvtGen, ending w/ .dec
   * b) "dpm" if you want to use dpm
   * c) "ftf" if you want to use ftf
   * d) "box:[...]" if you want to use box
   */  
  void SetInput(TString par)          { fInput          = par;}

  /**
   * @brief Input directory of the simulation
   */  
  void SetInputDir(TString par)          { fInputDir          = par;}
  
  /**
   * @brief  Setter of the parameter root file 
   */
  void SetParamRootFile(TString par)  { fParamRootFile  = par;}
  
  /** 
   * @brief Setter of the parameter ascii file 
   */
  void SetParamAsciiFile(TString par) { fParamAsciiFile = par;}

  /** 
   * @brief Setter of the simulation options
   * @detail This string can be:
   * ""                          -> default settings full setup
   * "day1"                      -> Setup for day1 experiment: no GEM, FTS1234, NO DISC, NO RICH
   * "gem" (added to "day1")     -> Setup for day1 experiment with 3 GEM planes
   * "fts1256" (added to "day1") -> Setup for day1 experiment with FTS1256 insted of FTS1234
   * Example: "day1+gem+fts1256" means day1 setup + GEM planes + fst1256 
   */
  void SetOptions(TString par) { fOptions = par; fOptions.ToLower();}
  
  /** 
   * @brief Setter of the number of events
   */
  void SetNumberOfEvents(Int_t par) { fNEvents = par;}

  /** 
   * @brief Setter of the event counter rate
   */
  void SetEventCounterRate(Int_t par) { fEventCounterRate = par;}

  /** 
   * @brief Setter of the target mode
   * @details #Target mode
   * 0 - No IP smearing (default)
   * 1 - Cluster Jet
   * 2 - Pellet target 
   * 3 - Pellet Tracking target
   */
  void SetTargetMode(Short_t par) { fTargetMode = par;}

  /** 
   * @brief Getter for the primary generator, e.g. to configure the event filter
   */
  FairFilteredPrimaryGenerator* GetFilteredPrimaryGenerator() {return (FairFilteredPrimaryGenerator*)fGen;}

 private:

  void GetRange(TString par, double &min, double &max);

  TString fInput;            ///< Name of the input for the simulation
  TString fInputDir;         ///< Name of the input directory for the simulation
  TString fOutFile;          ///< Name of the output file
  TString fParamRootFile;    ///< Name of the parameter root file
  TString fParamAsciiFile;   ///< Name of the parameter ascii file
  TString fOptions;          ///< Options parsed to the simulation
  
  Int_t fDpmFlag;            ///< Flag for DPM event generator
  Int_t fFtfFlag;            ///< Flag for FTF event generator
  Int_t fNEvents;            ///< Number of events
  Int_t fEventCounterRate;   ///< After how many events the counter will print
  Short_t fTargetMode;       ///< Target mode
  
  FairRuntimeDb *fRtdb;      ///< Runtime DB
  TStopwatch fTimer;         ///< Timer 
  
  /** @cond CLASSIMP */
  ClassDef(PndMasterRunSim,2);  ///< 1st Implementation -> 1; Added day1 options -> 2
  /** @endcond */
  
};

#endif /* PNDMASTERRUNSIM_H */
