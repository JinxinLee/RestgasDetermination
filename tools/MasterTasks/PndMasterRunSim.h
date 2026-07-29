/**
 * @class PndMasterRunSim
 * @brief Class for the master simulation chain
 * @details
 * # Master Simulation Class
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
#include "FairTask.h"

#include "TStopwatch.h"
#include "TString.h"

class FairFilteredPrimaryGenerator;
class PndFilteredPrimaryGenerator;
class FairGenerator;

class FairBoxGenerator;

class PndBoxGenerator;

class PndMasterRunSim : public FairRunSim {
 public:
  /**
   * @brief Default constructor
   */
  PndMasterRunSim();

  /**
   * @brief Default destructor
   */
  virtual ~PndMasterRunSim();

  /**
   * @brief Initial setup
   * @details
   * This command set the source files, load the proper parameters,
   * and set the relevant flags. If something fails, it returns
   * a kFALSE value.
   */
  Bool_t Setup(TString outprefix = "");

  /**
   * @brief Final diagnostics
   * @details
   * It prints CPU time, memory usage, used parameters, and eventually
   * send the information to CDash
   */
  void Finish();

  /**
   * @brief It switches between different standard geometry volumes
   * @details
   * According to fOptions, it creates the standard geometry volumes for the
   * full setup, or dedicated geometries (such as day1)
   */
  void CreateGeometry();

  /**
   * @brief It creates all the standard geometry volumes
   * @details
   * It creates all the standard geometry volumes which have to be used in simulation.
   * All the MCPoint will be stored, expect for EMC.
   */
  void CreateGeometryDefault();

  /**
   * @brief It creates the standard geometry volumes for phase1 of the experiment
   * @details
   * It creates all the standard geometry volumes which have to be used in simulation,
   * with the setup for phase1 experiments. All the MCPoint will be stored, except for EMC
   */

  void CreateGeometryPhase1();

  /**
   * @brief It creates the standard geometry volumes for day1 phase
   * @details
   * It creates all the standard geometry volumes which have to be used in simulation,
   * with the setup for day1 experiments. All the MCPoint will be stored, except for EMC
   */

  void CreateGeometryDay1();

  /**
   * @brief Add simulation tasks
   * @details
   * It adds all the standard simulation tasks
   */
  void AddSimTasks();

  /**
   * @brief Set the event generator
   * @details
   * This call set the event generator according to the input name. If the input name
   * contains "dpm" it uses dpm, if it contains "ftf" then ftf, if ".dec" it runs evtgen
   * using the input name as namefile of the .dec file. If the input name contains
   * "box" the macro breaks, since in that case the SetGenerator(PndBoxGenerator *boxGen)
   * function must be used.
   */
  void SetGenerator();

  /**
   * @brief Add a generator to existing setup
   * @details
   * Adds a generator to the existing one. ALL generators will be active
   * and produce their particle content in each event simultaneously.
   */
  void AddGenerator(FairGenerator *aGen);

  /**
   * @brief Set the event generator
   * @details
   * The user should create a FairGenerator derived object with all the settings,
   * and pass it as argument to the function.
   */
  void SetGenerator(FairGenerator *aGen);

  /**
   * @brief Set the DPM flag
   * @param Mode = 0. - DPM - No elastic scattering, only inelastic
   * @param Mode = 1. - DPM - Elastic and inelastic interactions (default)
   * @param Mode = 2. - DPM - Only elastic scattering, no inelastic one
   */
  void SetDpmFlag(Int_t Mode) { fDpmFlag = Mode; };

  void SetDpmTheta_min(Double_t theta_min) {fDpmTheta_min = theta_min; };

  void SetDpmTheta_max(Double_t theta_max) {fDpmTheta_max = theta_max; };
  
  void SetIP(Double_t beam_X0, Double_t beam_Y0, Double_t target_Z0) {fbeam_X0 = beam_X0; fbeam_Y0 = beam_Y0; ftarget_Z0 = target_Z0; };

  /**
   * @brief Set the FTF noelastic flag
   * @param Mode = 0. - FTF - Elastic and inelastic interactions (default)
   * @param Mode = 1. - FTF - No elastic scattering, only inelastic
   */
  void SetFtfFlag(Int_t Mode) { fFtfFlag = Mode; };

  /**
   * @brief Use DPM as event generator
   */
  void UseDpmGenerator();

  /**
   * @brief Use FTF as event generator
   * @details
   * This call set FTF as event generator.
   * If no input file is given FtfDirect is used.
   * With input file the events are taken from ftfData.
   */
  void UseFtfGenerator(TString ftfData);

  /**
   * @brief Use Pythia8 as event generator
   * @details
   * This call sets Pythia8 as event generator.
   * PndPythia8Direct is used. An input file will
   * be interpreted as a sequence of Pyhtia8 options
   * separated by semicolons
   */
  void UsePythia8Generator(TString py8Data);

  /**
   * @brief Use EvtGen as event generator
   * @details
   * This call set EvtGen as event generator. The user should set the
   * .dec file, and the function will retrieve automaticall beam
   * momentum and initial state.
   * @param EvtGenFile Filename of the .dec file
   */
  void UseEvtGenGenerator(TString EvtGenFile);

  /**
   * @brief Use BoxGen as event generator
   * @details
   * This call sets BoxGenerator as event generator.
   * The format of the config string is
   *   for isotrop events in theta:      'BOX:type(pdg,mult):p(min,max):phi(min,max):tht(min,max)'
   *   for isotrop events in cos(theta): 'BOX:type(pdg,mult):p(min,max):phi(min,max):ctht(min,max)'
   * Instead of range 'var(min,max)' also a fixed value can be set with 'var(value)'
   * The origin of particle generation can be specified using 'xyz(X,Y,Z)'.
   * All variables left out are set to defaults.
   * @param BoxConfig configuration string of the BOX generator
   */
  void UseBoxGenerator(TString BoxConfig);

  /**
   * @brief Use PiPiGenerator as event generator
   * @details
   * This call sets the PndPiPiGenerator as event generator.
   * The format of the config string is:
   *  'PIPI:cosTheta(min, max)'
   *  @param pipiConfig configuration string of the PiPi generator
   */
  void UsePiPiGenerator(TString pipiConfig);

  /**
   * @brief Use LepLepGenerator as event generator
   * @details
   * This call sets the PndLepLepGenerator as event generator.
   * The format of the config string is:
   *  'LEPLEP:pid(value):gegm(value):cosTheta(min, max)'
   *  @param leplepConfig configuration string of the LepLep generator
   */
  void UseLepLepGenerator(TString leplepConfig);
  void UseAsciiGenerator(TString AsciiFile);

  /**
   * @brief Input of the simulation
   * @details
   * @param par = the name of the dec file for EvtGen, ending w/ .dec
   * @param par = "dpm" if you want to use dpm
   * @param par = "ftf" if you want to use ftf
   * @param par = "box:[...]" if you want to use box
   */
  void SetInput(TString par) { fInput = par; }

  /**
   * @brief Input directory of the simulation
   */
  void SetInputDir(TString par) { fInputDir = par; }

  /**
   * @brief  Setter of the parameter root file
   */
  void SetParamRootFile(TString par) { fParamRootFile = par; }

  /**
   * @brief Setter of the parameter ascii file
   */
  void SetParamAsciiFile(TString par) { fParamAsciiFile = par; }

  /**
   * @brief Setter of the simulation options
   * @details
   * @param par = ""         -> default settings full setup
   * @param par = "day1"     -> Setup for day1 experiment: no GEM, FTS1234, NO DISC, NO RICH
   * @param par = "gem2"    (added to "day1")    -> Setup for day1 experiment with 2 GEM planes (first two)
   * @param par = "gem3"    (added to "day1")	 -> Setup for day1 experiment with 3 GEM planes
   * @param par = "fts1256" (added to "day1")    -> Setup for day1 experiment with FTS1256 insted of FTS1234
   * @param par = "strip"   (added to "day1")    -> Setup for day1 experiment with only the strip part of the MVD instead of the full MVD
   * Example: "day1+gem+fts1256" means day1 setup + GEM planes + fst1256
   */
  void SetOptions(TString par)
  {
    fOptions = par;
    fOptions.ToLower();
  }
  TString GetOptions() { return fOptions; }

  /**
   * @brief Setter of the number of events
   */
  void SetNumberOfEvents(Int_t par) { fNEvents = par; }

  /**
   * @brief Setter of the event counter rate
   */
  void SetEventCounterRate(Int_t par) { fEventCounterRate = par; }

  /**
   * @brief Setter of the target mode
   * @details
   * @param par = 0 - No IP smearing (default)
   * @param par = 1 - Cluster Jet
   * @param par = 2 - Pellet target
   * @param par = 3 - Pellet Tracking target
   */
  void SetTargetMode(Short_t par) { fTargetMode = par; }

  /** Select the density profile used by distributed restgas target modes. */
  void SetRestGasProfile(TString par) { fRestGasProfile = par; }

  /**
   * @brief Getter for the primary generator, e.g. to configure the event filter
   */
  FairFilteredPrimaryGenerator *GetFilteredPrimaryGenerator() { return (FairFilteredPrimaryGenerator *)fGen; }
  PndFilteredPrimaryGenerator *GetPndFilteredPrimaryGenerator() { return (PndFilteredPrimaryGenerator *)fGen; }

  void PrintListOfTasks(TList *list, TString prefix = " - ");
  void PrintListOfTasks() { PrintListOfTasks(GetMainTask()->GetListOfTasks()); }

 private:
  void GetRange(TString par, double &min, double &max);
  void GetCoords(TString par, double &X, double &Y, double &Z);
  /**
   * @brief Check and create primary generator
   * @details
   * Checks if the priomary generator is there and if not, creates it.
   */
  void CreatePrimGen();

  TString fInput;          ///< Name of the input for the simulation
  TString fInputDir;       ///< Name of the input directory for the simulation
  TString fOutFile;        ///< Name of the output file
  TString fParamRootFile;  ///< Name of the parameter root file
  TString fParamAsciiFile; ///< Name of the parameter ascii file
  TString fOptions;        ///< Options parsed to the simulation

  Int_t fDpmFlag;          ///< Flag for DPM event generator
  Int_t fFtfFlag;          ///< Flag for FTF event generator
  Int_t fNEvents;          ///< Number of events
  Int_t fEventCounterRate; ///< After how many events the counter will print
  Short_t fTargetMode;     ///< Target mode
  TString fRestGasProfile; ///< Restgas profile basename or absolute path

  FairRuntimeDb *fRtdb; ///< Runtime DB
  TStopwatch fTimer;    ///< Timer

  Double_t fDpmTheta_min;
  Double_t fDpmTheta_max;
  Double_t fbeam_X0;
  Double_t fbeam_Y0;
  Double_t fbeam_width_sigma_X;
  Double_t fbeam_width_sigma_Y; 
  Double_t ftarget_Z0;
  Double_t ftarget_width_Z;

  /** @cond CLASSIMP */
  ClassDef(PndMasterRunSim, 3); ///< Added configurable restgas density profile -> 3
                                /** @endcond */
};

#endif /* PNDMASTERRUNSIM_H */
