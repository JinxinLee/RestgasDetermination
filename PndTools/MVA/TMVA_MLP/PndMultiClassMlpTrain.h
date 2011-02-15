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
//#include "TMVA/Config.h"
//#include "TMVA/Tools.h"
//#include "TMVA/MethodMLP.h"

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
  inline void SetJobName       (std::string const& name);
  inline void SetTransformation(std::string const& tran);
  inline void SetMlpOptions    (std::string const& opts);
  
  inline std::string const& GetJobName();
  inline std::string const& GetTransformation();
  inline std::string const& GetMlpOptions();
  //----------------------------------------
  //================== protected ============
  //protected:
  //----------------------------------------
  //================== private =============
 private:
  PndMultiClassMlpTrain(PndMultiClassMlpTrain const& oth);
  PndMultiClassMlpTrain& operator=(PndMultiClassMlpTrain const& oth);
  void Init();
  TMVA::Factory* m_factory;
  std::string m_JName;
  std::string m_transform;
  std::string m_MlpOptions;
};
//=============== inline functions ========
//__________________________________________
inline void PndMultiClassMlpTrain::SetJobName(std::string const& name)
{
  m_JName = name;
};
inline void PndMultiClassMlpTrain::SetTransformation(std::string const& tr)
{
  m_transform = tr;
};

inline void PndMultiClassMlpTrain::SetMlpOptions(std::string const& opt)
{
  m_MlpOptions = opt;
};

inline std::string const& PndMultiClassMlpTrain::GetJobName()
{
  return m_JName;
};
inline std::string const& PndMultiClassMlpTrain::GetTransformation()
{
  return m_transform;
};
inline std::string const& PndMultiClassMlpTrain::GetMlpOptions()
{
  return m_MlpOptions;
};
#endif
