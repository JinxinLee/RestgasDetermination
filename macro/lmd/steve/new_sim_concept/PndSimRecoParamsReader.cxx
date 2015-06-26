/*
 * RunConfigGenerator.cxx
 *
 *  Created on: Mar 17, 2014
 *      Author: steve
 */

#include "PndSimRecoParamsReader.h"

#include <fstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using boost::property_tree::ptree;

PndSimRecoParams PndSimRecoParamsReader::readConfigFromFile(
		const std::string &config_url) const {
	// Create an empty property tree object
	boost::property_tree::ptree pt;

	// read the config file
	read_xml(config_url, pt);

	// create the simulation and reconstruction parameter object
	PndSimRecoParams sim_reco_params;
	sim_reco_params.setSimulationParameters(pt);

	return sim_reco_params;
}

void PndSimRecoParamsReader::writeConfigToFile(
		const PndSimRecoParams &sim_reco_params,
		const std::string &output_file_url) const {
	std::fstream file(output_file_url.c_str(), std::ios_base::out);
	write_xml(file, sim_reco_params.getSimRecoParamsPropertyTree());
	file.close();
}
