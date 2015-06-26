/*
 * PndSimRecoParams.h
 *
 *  Created on: Jun 26, 2015
 *      Author: steve
 */

#ifndef PNDSIMRECOPARAMS_H_
#define PNDSIMRECOPARAMS_H_

#include <map>
#include <string>

#ifndef __CINT__
#include "boost/property_tree/ptree.hpp"
#endif /* __CINT __ */

class PndSimRecoParams {
	// usually we would just have the ptree here
	// unfortunately root cint is not capable to parse the ptree header
	// so this simpler option map was "developed"...
	std::map<std::string, std::string> simulation_parameters;

public:
	PndSimRecoParams();
	virtual ~PndSimRecoParams();

#ifndef __CINT__
	void insertSimulationParameters(const std::string &name_base,
			const boost::property_tree::ptree &simulation_parameters_);
	void setSimulationParameters(
			const boost::property_tree::ptree &simulation_parameters_);

	boost::property_tree::ptree getSimRecoParamsPropertyTree() const;
#endif /* __CINT __ */

	template<typename T> T getParameterValueByName(
			const std::string &param_name) const;
};

#endif /* PNDSIMRECOPARAMS_H_ */
