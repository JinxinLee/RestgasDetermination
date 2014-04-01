/*
 * RunConfigGenerator.cxx
 *
 *  Created on: Mar 17, 2014
 *      Author: steve
 */

#include "PndLmdSimConfigReader.h"
#include <sstream>
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

using boost::property_tree::ptree;

PndLmdSimConfig PndLmdSimConfigReader::readConfigFromFile(std::string dir_path,
		std::string file_name) {
	PndLmdSimConfig sim_config;
	// Create an empty property tree object
	boost::property_tree::ptree pt;
	// read the config file
	std::stringstream url;
	url << dir_path << "/" << file_name;
	read_ini(url.str(), pt);


	std::cout<<pt.data()<<std::endl;
	//Section:Basics
	sim_config.beam_momentum = pt.get<double>("Basics.beam_momentum");
	sim_config.num_events = pt.get<int>("Basics.num_events");
	sim_config.generator_filename_template = pt.get<std::string>(
			"Basics.generator_filename_template");

	//Section:Reco
	sim_config.use_missing_plane = pt.get<bool>("Reco.use_missing_plane");
	sim_config.use_track_cuts = pt.get<bool>("Reco.use_track_cuts");
	sim_config.merge_hits = pt.get<bool>("Reco.merge_hits");

	sim_config.use_x_theta_cut = pt.get<bool>("Reco.use_x_theta_cut");
	sim_config.use_y_phi_cut = pt.get<bool>("Reco.use_y_phi_cut");
	sim_config.use_box_cut = pt.get<bool>("Reco.use_box_cut");

	sim_config.clean_backpropagated_data = pt.get<bool>(
			"Reco.clean_backpropagated_data");
	sim_config.write_all_MC_info = pt.get<bool>("Reco.write_all_MC_info");

	//Section:IP-Beam
	sim_config.ip_offset_x = pt.get<double>("IP-Beam.ip_offset_x");
	sim_config.ip_offset_y = pt.get<double>("IP-Beam.ip_offset_y");
	sim_config.ip_offset_z = pt.get<double>("IP-Beam.ip_offset_z");
	sim_config.ip_spread_x = pt.get<double>("IP-Beam.ip_spread_x");
	sim_config.ip_spread_y = pt.get<double>("IP-Beam.ip_spread_y");
	sim_config.ip_spread_z = pt.get<double>("IP-Beam.ip_spread_z");

	sim_config.beam_gradient_x = pt.get<double>("IP-Beam.beam_gradient_x");
	sim_config.beam_gradient_y = pt.get<double>("IP-Beam.beam_gradient_y");
	sim_config.beam_divergence_x = pt.get<double>("IP-Beam.beam_divergence_x");
	sim_config.beam_divergence_y = pt.get<double>("IP-Beam.beam_divergence_y");

	// in case an older config is read in by the newer reader,
	// some key value pairs can be missing.
	// then we need to print a warninghere we have to thro

	return sim_config;
}
