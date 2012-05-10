/* ***************************************
 * MultiClass BDTG  Training functions   *
 * Author: M.Babai@rug.nl                *
 * Version:                              *
 * LICENSE:                              *
 * ***************************************
 */
/*
 * Note: This is just an interface to the original TMVA
 * implementation. To find out the available options, please read TMVA
 * manuals. In case of errors or wrong outputs produced by TMVA
 * classifiers, try to read their mailing list and send your questions
 * to the same list.
 ******* VERY IMORTANT ****
 * You NEED (TMVA version) > 4.1.X before this works.
 */
#pragma once
#ifndef PND_MULTICLASS_BDT_TRAIN_H
#define PND_MULTICLASS_BDT_TRAIN_H

//Local includes
#include "PndMvaTrainer.h"

// TMVA && ROOT
#include "TMVA/Factory.h"
#include "TMVA/Config.h"

// Interface definition for Multiclass MLP trainers.
class PndMultiClassBdtTrain : public PndMvaTrainer
{
  //----------------------------------------
  //================== public ==============
 public:
  /**
   * Constructor.
   *@param InPut The file containig the event data.
   *@param ClassNames Labels of the classes to be used.
   *@param VarNames  The name of the involved variables(features).
   *@param trim If the data set needs to be trimmed.
   */
  explicit PndMultiClassBdtTrain(std::string const& InPut,
				 std::vector<std::string> const& ClassNames, 
				 std::vector<std::string> const& VarNames,
				 bool trim = true);
  /**
   * Destructor.
   */
  virtual ~PndMultiClassBdtTrain();
  
  /**
   * Train the classifier.
   */
  void Train();

  /**
   * Store weights in the output File. If output file name is not
   * specified, then write nothing.
   */
  void storeWeights();

  /**
   *Initialize Classifier and data structures.
   */
  void Initialize();

  //______________________________________________
  //====== Getters and setters.  
  // Set the name of the current job
  inline void SetJobName (std::string const& name);

  // Set data Transformation scheme
  inline void SetTransformation (std::string const& tran);
  
  // Set the options for the MLP alg. See TMVA manuals.
  inline void SetBdtOptions (std::string const& opts);
  
  // Set the file name to store evaluation outputs.
  inline void SetEvalFileName (std::string const& fname);
  
  // Set the directory where weights are stored.
  inline void SetWeightsOutDir (std::string const& dirName);
  
  // Evaluate the classifier?
  inline void SetEvaluation (bool evaluate);
  
  // Get the current job name.
  inline std::string const& GetJobName() const;
  
  // Get the current transformation info.
  inline std::string const& GetTransformation() const;
  
  // Get the classifier options.
  inline std::string const& GetBdtOptions() const;
  
  // Get the name of the weight file.
  inline std::string const& GetEvalFileName() const;
  
  // Get the directory where the weights are stored.
  inline std::string const& GetWeightsOutDir() const;
  //----------------------------------------
  
  //================== protected ============
  //protected:
  //----------------------------------------
  
  //================== private =============
 private:
  // To avoid mistakes.
  PndMultiClassBdtTrain(PndMultiClassBdtTrain const& oth);
  PndMultiClassBdtTrain& operator=(PndMultiClassBdtTrain const& oth);
  
  // Initialize mlp object and set the options.
  void InitBdt();
  // Add the variables to the TMVA factory object.
  void AddVariables();
  
  //==============================
  TMVA::Factory* m_factory;// TMVA factory
  TFile* EvalFile; // To store evaluation file
  std::string m_JName; //Job name
  std::string m_transform;// Transformation opt.
  std::string m_BdtOptions; // Bdt options.
  std::string m_evalFileName; //evaluation file name.
  std::string m_weightDirName;// Directory name to store weights.
  bool m_Evaluate;
};// End of interface definition.
//=============== inline functions implementation. ========
//__________________________________________
inline void PndMultiClassBdtTrain::SetJobName(std::string const& name)
{
  this->m_JName = name;
};

inline void PndMultiClassBdtTrain::SetTransformation(std::string const& tr)
{
  this->m_transform = tr;
};

inline void PndMultiClassBdtTrain::SetBdtOptions(std::string const& opt)
{
  this->m_BdtOptions = opt;
};

inline std::string const& PndMultiClassBdtTrain::GetJobName() const
{
  return m_JName;
};

inline std::string const& PndMultiClassBdtTrain::GetTransformation() const
{
  return m_transform;
};

inline std::string const& PndMultiClassBdtTrain::GetBdtOptions() const
{
  return m_BdtOptions;
};

inline void PndMultiClassBdtTrain::SetEvalFileName(std::string const& fname)
{
  this->m_evalFileName = fname;
};

inline std::string const& PndMultiClassBdtTrain::GetEvalFileName() const
{
  return m_evalFileName;
};

inline void PndMultiClassBdtTrain::SetWeightsOutDir (std::string const& dirName)
{
  this->m_weightDirName = dirName;
};

inline std::string const& PndMultiClassBdtTrain::GetWeightsOutDir() const
{
  return m_weightDirName;
};

inline void PndMultiClassBdtTrain::SetEvaluation(bool evaluate)
{
  this->m_Evaluate = evaluate;
};
#endif
