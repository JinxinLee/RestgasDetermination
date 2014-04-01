/*
 * testmacro.C
 *
 *  Created on: Mar 18, 2014
 *      Author: steve
 */

void testmacro(std::string path) {
	gSystem->Load("libSimConfigReader");
	//gSystem->SetIncludePath("");

	PndLmdSimConfigReader reader;

	PndLmdSimConfig sim_conf = reader.readConfigFromFile(path);

	cout<<"num events: "<<sim_conf.num_events<<endl;
	cout<<"use_y_phi_cut: "<<sim_conf.use_y_phi_cut<<endl;

	cout<<"ip_offset_x: "<<sim_conf.ip_offset_x<<endl;
}
