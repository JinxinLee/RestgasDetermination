#include "Pedestals.h"
#include<sstream>
#include<iostream>
#include<fstream>

#include "Calib.h"

using namespace std;

Pedestals* Pedestals::theInstance = NULL;
std::map<int,double> Pedestals::_mean;
std::map<int,double> Pedestals::_sigma;

Pedestals::Pedestals(std::string pedFileName) {
  if(theInstance != NULL) {
	std::cerr << "Pedestals::getPedestals(...): theInstance pointer is not NULL -> ctor was called for the second time ->abort" << std::endl;
	throw;
  }
  readPedestals(pedFileName);
  theInstance = this;
}

void Pedestals::getPedestals(std::map<int,double>& m,std::map<int,double>& s){
  if(theInstance == NULL) {
	std::cerr << "Pedestals::getPedestals(...): theInstance pointer is NULL -> ctor wasnt called before ->abort" << std::endl;
	throw;
  }
  m = _mean;
  s = _sigma;
}
void Pedestals::getPedestal(int padID,double& mean, double& sigma){
  if(theInstance == NULL) {
	std::cerr << "Pedestals::getPedestals(...): theInstance pointer is NULL -> ctor wasnt called before ->abort" << std::endl;
	throw;
  }
  mean=_mean[padID];
  sigma = _sigma[padID];
}


void Pedestals::readPedestals(std::string pedFileName) {


  if (pedFileName=="DREGGN") {
	pedFileName = "pedestals.dat";
  }


  ifstream input(pedFileName.c_str());

  char line[200];
  while(input.getline(line,199)) {
	std::istringstream istr(line);
	double mean=-1.E10;
	double sigma=-1.E10;
	int channelNumber = -10000;
	istr >> channelNumber >> mean >> sigma;
	if(channelNumber < 0 || channelNumber > 127 ||
	   mean<-1.E9 ||
	   sigma<-1.E9) {
	  std::cerr << "in tracking::readPedestals(): problem with file format -> abort" << std::endl;
	  throw;
	}
	_mean[channelNumber] = mean;
	_sigma[channelNumber] = sigma;
  }



  if(_mean.size()!=128 || _sigma.size()!=128){
	  std::cerr << "in tracking::readPedestals(): not all channels seem to have ped -> abort" << std::endl;
	  throw;
	}


  
  
}
 
 
