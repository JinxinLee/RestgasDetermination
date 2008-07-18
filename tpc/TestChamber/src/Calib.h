#ifndef CALIB_H
#define CALIB_H

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#define BASEDIR "/afs/e18/panda/TPC/Test_Chamber_Data/"


class CalibData{
 private:
  CalibData();
  virtual ~CalibData();
  double vdr;
  double vdr_err;
  std::string pedFile;

  friend class Calib;
};

class Calib{
 public:
  virtual ~Calib();
  static Calib* getInstance() {
	if(theInstance == NULL) {
	  theInstance = new Calib();
	}
	return theInstance;
  }

  void setKey(std::string val) {accessKey=val;}

  double get_vdr() {
	if(data[accessKey]==NULL) return -1;
	return data[accessKey]->vdr;
  }
  double get_vdr_err() {
	if(data[accessKey]==NULL) return -1;
	return data[accessKey]->vdr_err;
  }
  std::string get_pedFile() {
	if(data[accessKey]==NULL) return std::string("DREGGN");
	return data[accessKey]->pedFile;
  }
  void set_vdr(std::string key, double val) {
	if(data[key]==NULL) data[key] = new CalibData();
	data[key]->vdr = val;
  }
  void set_vdr_err(std::string key, double val) {
	if(data[key]==NULL) data[key] = new CalibData();
	data[key]->vdr_err = val;
  }
  void set_pedFile(std::string key, std::string val) {
	if(data[key]==NULL) data[key] = new CalibData();
	data[key]->pedFile = val;
  }
 private:
  Calib();
  std::string accessKey;
  static Calib*  theInstance;
  void readCalibFile(std::string infile="calib.txt");
  std::map<std::string,CalibData*> data;
};

#endif
 
 
