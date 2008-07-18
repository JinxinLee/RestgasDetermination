#include "Calib.h"

Calib* Calib::theInstance = NULL;

CalibData::CalibData(){}

CalibData::~CalibData(){}


Calib::Calib() :accessKey("DREGGN") {
  readCalibFile();
}

Calib::~Calib() {}



void Calib::readCalibFile(std::string infile){

  std::ifstream input(infile.c_str());
  
  char line[200];

  while(input.getline(line,199)) {
	std::istringstream istr(line);
	double vdr(-1.); 
	double vdr_err(-1.); 
	std::string pedFile("DREGGN");
	std::string key("DREGGN");
	if(istr.str().substr(0,1) == "#") {
	  continue;
	}
	istr >> key >> pedFile >> vdr >> vdr_err;
	key = std::string(BASEDIR) + key;
	pedFile = std::string(BASEDIR) + pedFile;
	if(key!="DREGGN") {
	  set_vdr(key,vdr);
	  set_vdr_err(key,vdr_err);
	  set_pedFile(key,pedFile);
	}
  }

}

 
 
