/*
 * ModelParSet.h
 *
 *  Created on: Feb 9, 2013
 *      Author: steve
 */

#ifndef MODELPARSET_H_
#define MODELPARSET_H_

#include "ModelPar.h"

#include <map>
#include <vector>
//#include <tr1/memory>

//using std::tr1::shared_ptr;

class ModelParSet {
public:
	/**
	 * Small structure defining the comparison operator used in the map
	 * #model_par_map. Will return true only if this model parameter is equal to
	 * the parameter. It is used when concatenating multiple ModelPar objects to
	 * avoid having multiple instances of the same parameter. The check is based
	 * on a name comparison.
	 */
	struct stringcomp {
		bool operator()(const std::string& lhs, const std::string& rhs) const {
			return lhs.compare(rhs) < 0;
		}
	};

private:
	/**
	 * Map containing all the model parameters which are unique, independent
	 * and belong to this model.
	 */
	std::map<std::string, shared_ptr<ModelPar>, stringcomp> model_par_map;

public:
	ModelParSet();
	virtual ~ModelParSet();

	/**
	 * This function returns the total number of parameters that are required by
	 * the current specification of the model.
	 */
	unsigned int getNumberOfParameters() const;

	/**
	 * This function returns the total number of free parameters that are
	 * required by the current specification of the model.
	 */
	unsigned int getNumberOfFreeParameters() const;

	bool modelParameterExists(const shared_ptr<ModelPar> &model_par) const;
	bool modelParameterExists(const std::string &name_) const;

	void printInfo() const;

	/**
	 * This function should be used to do create model parameters within a model
	 */
	const shared_ptr<ModelPar> addModelParameter(std::string name_,
			double value_ = 0.0, bool fixed_ = true);
	/**
	 * use this function when combining to models via some operation etc.
	 */
	void addModelParameter(shared_ptr<ModelPar> model_par);

	void reassignParameter(shared_ptr<ModelPar> model_par);

	/**
	 * Attempts to add all parameters of this daughter model to the parameter set.
	 * If there is a global parameter of the daughter model set with equal name
	 * already in this set, then the parameter in the daughter set will be
	 * reassigned and the return value will be nonzero.
	 * @param daughter_model_par_set daughter model parameter set
	 * @returns number of parameters in the daughter parameter set that were reassigned
	 */
	int addModelParameters(ModelParSet &daughter_model_par_set);

	int setModelParameterValue(const std::string &name_, double value_);

	/**
	 * Used by the parametrizations to get a set of parameters on which the
	 * parametrization depends on.
	 */
	const double& getModelParameterValue(const std::string &name_) const;

	shared_ptr<ModelPar> getModelParameter(const std::string &name_);

	void freeModelParameter(const std::string &name_);

	/**
	 * This function frees all non-superior parameters of this model.
	 */
	void freeAllModelParameters();

	int checkParameters() const;

	bool checkSuperiorParameters() const;

	std::vector<shared_ptr<ModelPar> > getFreeModelParameters() const;

	std::map<std::string, shared_ptr<ModelPar>, stringcomp>& getModelParameterMap();
};

#endif /* MODELPARSET_H_ */
