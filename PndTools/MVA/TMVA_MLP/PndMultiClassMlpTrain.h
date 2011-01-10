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

//! Interface definition for Multiclass MLP trainers.
class PndMultiClassMlpTrain : public PndMvaTrainer
{
  //----------------------------------------
  //================== public ==============
 public:
  PndMultiClassMlpTrain();
  PndMultiClassMlpTrain(std::string const& InPut,
			std::vector<std::string> const& ClassNames, 
			std::vector<std::string> const& VarNames,
			bool trim = true);

  ~PndMultiClassMlpTrain();
  
  void Train();
  
  //----------------------------------------
  //================== protected ============
 protected:
  //----------------------------------------
  //================== private =============
 private:
};
#endif
