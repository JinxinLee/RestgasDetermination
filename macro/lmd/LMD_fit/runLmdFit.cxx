/*
 * runLmdFit.cxx
 *
 *  Created on: Dec 18, 2014
 *      Author: steve
 */

#include "PndLmdRuntimeControl.h"

#include <iostream>
#include <string>

using std::string;
using std::cout;
using std::cerr;
using std::endl;

void runLmdFit(string input_file_dir, string config_file_url,
		string acceptance_file_dir, string reference_acceptance_file_dir) {

	PndLmdRuntimeControl runtime_control;

	runtime_control.setFitPaths(input_file_dir, acceptance_file_dir,
			reference_acceptance_file_dir);

	runtime_control.initFitRuntimeConfiguration(config_file_url);

	runtime_control.doLuminosityFits();
}

void displayInfo() {
	// display info
	cout << "Required arguments are: " << endl;
	cout << "-d [path to data]" << endl;
	cout << "-c [path to config file] " << endl;
	cout << "Optional arguments are: " << endl;
	cout << "-a [path to box gen data] (acceptance)" << endl;
	cout << "-r [path to reference box gen data] (acceptance)" << endl;
}

int main(int argc, char* argv[]) {
	string data_path;
	string acc_path("");
	string config_path("");
	string ref_acc_path("");
	bool is_data_set(false), is_config_set(false), is_acc_set(false),
			do_phi_slice_fitting(false);

	int c;

	while ((c = getopt(argc, argv, "hsc:a:r:d:")) != -1) {
		switch (c) {
			case 's':
				do_phi_slice_fitting = true;
				break;
			case 'a':
				acc_path = optarg;
				is_acc_set = true;
				break;
			case 'c':
				config_path = optarg;
				is_config_set = true;
				break;
			case 'r':
				ref_acc_path = optarg;
				break;
			case 'd':
				data_path = optarg;
				is_data_set = true;
				break;
			case '?':
				if (optopt == 'd' || optopt == 'a' || optopt == 'c' || optopt == 'r')
					cerr << "Option -" << optopt << " requires an argument." << endl;
				else if (isprint(optopt))
					cerr << "Unknown option -" << optopt << "." << endl;
				else
					cerr << "Unknown option character" << optopt << "." << endl;
				return 1;
			case 'h':
				displayInfo();
				return 1;
			default:
				return 1;
		}
	}

	if (is_data_set && is_config_set)
		runLmdFit(data_path, config_path, acc_path, ref_acc_path);
	else
		displayInfo();
	return 0;
}

