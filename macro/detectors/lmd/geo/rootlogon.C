{
	std::cout<<"processing rootlogon.C...\n";
	std::string pandaroot_dir(getenv("VMCWORKDIR"));
	gROOT->ProcessLine((std::string(".include ")+pandaroot_dir).c_str());
	gSystem->Load("libLmd");
}
