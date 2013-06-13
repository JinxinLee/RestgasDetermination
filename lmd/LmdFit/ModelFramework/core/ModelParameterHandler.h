/*
 * ModelParameterHandler.h
 *
 *  Created on: Mar 19, 2013
 *      Author: steve
 */

#ifndef MODELPARAMETERHANDLER_H_
#define MODELPARAMETERHANDLER_H_

#include "ModelParSet.h"
#include "Parametrization.h"
#include "ParametrizationModel.h"
#include "ParametrizationProxy.h"

#include <map>
#include <set>
//#include <tr1/memory>

//using std::tr1::shared_ptr;

class ModelParameterHandler {
private:
  /**
   * The parameter set of the model.
   */
  ModelParSet model_par_set;

  /**
   * Map of ParametrizationProxys, which is either a Parametrization or a
   * ParametrizationModel, for every ModelPar of this Model that is being
   * parametrized.
   */
  std::map<const shared_ptr<ModelPar>, ParametrizationProxy> parametrizations;

  /**
   * Set of model parametrizations that are called at each iteration step to
   * update parameters due to connections to free parameters that have changed.
   * This set can only be equal to the full set of parametrizations or less.
   * Since ParametrizationModels already update on each iteration, this
   * procedure is not required for them.
   */
  std::set<shared_ptr<Parametrization> > updating_parametrizations;

public:
  ModelParameterHandler(std::string model_name_);
  virtual ~ModelParameterHandler();

  ModelParSet& getModelParameterSet();

  int checkParametrizations() const;

  int checkParameters();

  void registerUpdaters();

  void reinitModelParametrizations();

  void executeParametrizationModels(const double *x);


  void registerParametrization(shared_ptr<ModelPar> model_par, shared_ptr<Parametrization> parametrization);

  void registerParametrizations(ModelParSet &model_par_set_, shared_ptr<Parametrization> parametrization);

  void registerParametrizationModel(shared_ptr<ModelPar> model_par, shared_ptr<ParametrizationModel> parametrization_model);

  void registerParametrizationModels(ModelParSet &model_par_set_, shared_ptr<ParametrizationModel> parametrization_model);


  /**
   * This method updates the model parameters that are set via
   * parametrizations, and is automatically called from the #Model::updateModel
   * function.
   */
  void updateModelParameters();
};

#endif /* MODELPARAMETERHANDLER_H_ */
