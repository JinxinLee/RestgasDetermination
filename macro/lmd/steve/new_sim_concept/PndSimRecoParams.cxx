/*
 * PndSimRecoParams.cxx
 *
 *  Created on: Jun 26, 2015
 *      Author: steve
 */

#include "PndSimRecoParams.h"

#include "boost/lexical_cast.hpp"

using boost::lexical_cast;
using boost::bad_lexical_cast;

PndSimRecoParams::PndSimRecoParams() {
	// TODO Auto-generated constructor stub

}

PndSimRecoParams::~PndSimRecoParams() {
	// TODO Auto-generated destructor stub
}

void PndSimRecoParams::insertSimulationParameters(const std::string &name_base,
		const boost::property_tree::ptree &simulation_parameters_) {
	boost::property_tree::ptree::const_iterator iter;
	for (iter = simulation_parameters_.begin();
			iter != simulation_parameters_.end(); iter++) {
		if (iter->second.empty()) {
			simulation_parameters[name_base + iter->first] = iter->second.data();
		} else {
			insertSimulationParameters(name_base + "." + iter->first, iter->second);
		}
	}
}

void PndSimRecoParams::setSimulationParameters(
		const boost::property_tree::ptree &simulation_parameters_) {
	boost::property_tree::ptree::const_iterator iter;
	// convert the ptree to simple format...
	for (iter = simulation_parameters_.begin();
			iter != simulation_parameters_.end(); iter++) {
		if (iter->second.empty()) {
			simulation_parameters[iter->first] = iter->second.data();
		} else {
			insertSimulationParameters(iter->first, iter->second);
		}
	}
}

boost::property_tree::ptree PndSimRecoParams::getSimRecoParamsPropertyTree() const {
	boost::property_tree::ptree sim_reco_params_ptree;

	std::map<std::string, std::string>::const_iterator simulation_parameter;
	for (simulation_parameter = simulation_parameters.begin();
			simulation_parameter != simulation_parameters.end();
			++simulation_parameter) {
		bool success(false);
		// check if its a bool
		try {
			bool value = lexical_cast<bool>(simulation_parameter->second);
			sim_reco_params_ptree.put(simulation_parameter->first, value);
			success = true;
		} catch (const bad_lexical_cast &) {
		}
		// check if its an int
		try {
			if (!success) {
				int value = lexical_cast<int>(simulation_parameter->second);
				sim_reco_params_ptree.put(simulation_parameter->first, value);
				success = true;
			}
		} catch (const bad_lexical_cast &) {
		}
		// check if its a double
		try {
			if (!success) {
				double value = lexical_cast<double>(simulation_parameter->second);
				sim_reco_params_ptree.put(simulation_parameter->first, value);
				success = true;
			}
		} catch (const bad_lexical_cast &) {
		}
		// otherwise just take it as a string
		sim_reco_params_ptree.put(simulation_parameter->first,
				simulation_parameter->second);
	}

	return sim_reco_params_ptree;
}

template<typename T> T PndSimRecoParams::getParameterValueByName(
		const std::string &param_name) const {
	T value;
	std::map<std::string, std::string>::const_iterator search_result =
			simulation_parameters.find(param_name);
	if (search_result != simulation_parameters.end()) {
		// check if its a double
		try {
			value = lexical_cast<T>(search_result->second);
		} catch (const bad_lexical_cast &) {
			std::cout << "ERROR: parameter with name " << search_result->first
					<< " exists, but has value " << search_result->second
					<< " which seems not to be the type you requested."
					<< " Returning a default instanciated variable of this type!"
					<< std::endl;
		}
	}

	return value;
}

template int PndSimRecoParams::getParameterValueByName<int>(
		const std::string &param_name) const;
template bool PndSimRecoParams::getParameterValueByName<bool>(
		const std::string &param_name) const;
template double PndSimRecoParams::getParameterValueByName<double>(
		const std::string &param_name) const;
template std::string PndSimRecoParams::getParameterValueByName<std::string>(
		const std::string &param_name) const;
