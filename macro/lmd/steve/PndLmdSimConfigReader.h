/*
 * PndLmdSimConfigReader.h
 *
 *  Created on: Mar 18, 2014
 *      Author: steve
 */

#ifndef PNDLMDSIMCONFIGREADER_H_
#define PNDLMDSIMCONFIGREADER_H_

#include <string>

struct PndLmdSimConfig {
		//Section:Basics
		double beam_momentum;
		unsigned int num_events; //per job
		std::string generator_filename_template;

		//Section:Reco
		bool use_missing_plane;
		bool use_track_cuts; //use cuts during trk seacrh with "CA". Should be 'false' if sensors missaligned!
		bool merge_hits;

		bool use_x_theta_cut;
		bool use_y_phi_cut;
		bool use_box_cut;

		bool clean_backpropagated_data;
		bool write_all_MC_info; //Write all MC info in TrkQA array

		//Section:IP-Beam
		double ip_offset_x;
		double ip_offset_y;
		double ip_offset_z;
		double ip_spread_x;
		double ip_spread_y;
		double ip_spread_z;

		double beam_gradient_x;
		double beam_gradient_y;
		double beam_divergence_x;
		double beam_divergence_y;

		/*friend ostream& operator<<(ostream& os, PndLmdSimConfig) {

		}*/
};

class PndLmdSimConfigReader {
	public:
		PndLmdSimConfig readConfigFromFile(std::string dir_path,
				std::string file_name = "sim_prop.cfg");
};



#endif /* PNDLMDSIMCONFIGREADER_H_ */
