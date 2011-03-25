/* ***************************************
 * MultiClass MLP  Training functions    *
 * Author: M.Babai@rug.nl                *
 * Version:                              *
 * LICENSE:                              *
 * ***************************************
 */
#ifndef PND_MULTI_CLASS_MLP_TRAIN_H
#define PND_MULTI_CLASS_MLP_TRAIN_H

//Local includes
#include "PndMvaTrainer.h"

// TMVA && ROOT
#include "TMVA/Factory.h"
#include "TMVA/Config.h"
//#include "TMVA/MethodMLP.h"
//#include "TMVA/Tools.h"

//! Interface definition for Multiclass MLP trainers.
class PndMultiClassMlpTrain : public PndMvaTrainer
{
  //----------------------------------------
  //================== public ==============
 public:
  PndMultiClassMlpTrain(std::string const& InPut,
			std::vector<std::string> const& ClassNames, 
			std::vector<std::string> const& VarNames,
			bool trim = true);

  ~PndMultiClassMlpTrain();
  
  void Train();
  void Initialize();

  inline void SetJobName       (std::string const& name);
  inline void SetTransformation(std::string const& tran);
  inline void SetMlpOptions    (std::string const& opts);
  inline void SetEvalFileName  (std::string const& fname);
  inline void SetWeightsOutDir (std::string const& dirName);
  inline void SetEvaluation(bool evaluate);

  inline std::string const& GetJobName()const;
  inline std::string const& GetTransformation()const;
  inline std::string const& GetMlpOptions()const;
  inline std::string const& GetEvalFileName()const;
  inline std::string const& GetWeightsOutDir()const;
  //----------------------------------------
  
  //================== protected ============
  //protected:
  //----------------------------------------
  
  //================== private =============
 private:
  // To avoid mistakes.
  PndMultiClassMlpTrain(PndMultiClassMlpTrain const& oth);
  PndMultiClassMlpTrain& operator=(PndMultiClassMlpTrain const& oth);

  void InitMlp();
  void AddVariables();

  //==============================
  TMVA::Factory* m_factory;
  TFile* EvalFile;
  std::string m_JName;
  std::string m_transform;
  std::string m_MlpOptions;
  std::string m_evalFileName;
  std::string m_weightDirName;
  bool m_Evaluate;
};
//=============== inline functions ========
//__________________________________________
inline void PndMultiClassMlpTrain::SetJobName(std::string const& name)
{
  this->m_JName = name;
};

inline void PndMultiClassMlpTrain::SetTransformation(std::string const& tr)
{
  this->m_transform = tr;
};

inline void PndMultiClassMlpTrain::SetMlpOptions(std::string const& opt)
{
  this->m_MlpOptions = opt;
};

inline std::string const& PndMultiClassMlpTrain::GetJobName() const
{
  return m_JName;
};

inline std::string const& PndMultiClassMlpTrain::GetTransformation() const
{
  return m_transform;
};

inline std::string const& PndMultiClassMlpTrain::GetMlpOptions() const
{
  return m_MlpOptions;
};

inline void PndMultiClassMlpTrain::SetEvalFileName(std::string const& fname)
{
  this->m_evalFileName = fname;
};

inline std::string const& PndMultiClassMlpTrain::GetEvalFileName() const
{
  return m_evalFileName;
};

inline void PndMultiClassMlpTrain::SetWeightsOutDir (std::string const& dirName)
{
  this->m_weightDirName = dirName;
};

inline std::string const& PndMultiClassMlpTrain::GetWeightsOutDir()const
{
  return m_weightDirName;
};

inline void PndMultiClassMlpTrain::SetEvaluation(bool evaluate)
{
  this->m_Evaluate = evaluate;
};
#endif
