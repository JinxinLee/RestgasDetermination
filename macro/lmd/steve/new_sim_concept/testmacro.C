/*
 * testmacro.C
 *
 *  Created on: Mar 18, 2014
 *      Author: steve
 */

void testmacro(std::string config_url) {
	gSystem->Load("libSimRecoParams.so");
	//gSystem->SetIncludePath("");

	PndSimRecoParamsReader reader;

	PndSimRecoParams sim_conf = reader.readConfigFromFile(config_url);

	cout<<"num events: "<<sim_conf.getParameterValueByName<int>("sim.num_events")<<endl;
	cout<<"use_y_phi_cut: "<<sim_conf.getParameterValueByName<bool>("reco.use_y_phi_cut")<<endl;
	cout<<"ip_offset_x: "<<sim_conf.getParameterValueByName<bool>("ip_beam.ip_mean_x")<<endl;
}
