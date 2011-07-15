#ifndef PNDTPCALIGNMENTMANAGER_H
#define PNDTPCALIGNMENTMANAGER_H

//ROOT headers
#include "TGeoMatrix.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TMatrixD.h"
//STL headers
#include <string>
#include <map>
#include <iostream>
#include <vector>

/*
  Euler convention 
  This represent the composition of : first a rotation about Z axis with
  angle phi, then a rotation with theta about the rotated X axis, and
  finally a rotation with psi about the new Z axis.

 */


//class TLorentzVector;

class PndTpcAlignmentManager{

 public:
  static PndTpcAlignmentManager* getInstance(std::string filename){
    if(inst==NULL){
      inst=new PndTpcAlignmentManager(filename);
    }else if(filename==inst->transformationFileName) {
      inst->read(filename);
    }
    return inst;
  }

  //special case, need to return identity-transform
  static PndTpcAlignmentManager* getInstance(){
    if(inst==NULL){
      std::cerr<<"No PndTpcAlignmentManager instance existing, "
	       <<"PndTpcAlignmentManager::getInstance(string filename)"
	       <<" needs to be called first"<<std::endl;
      throw;
    } 
    return inst;
  }
  TGeoCombiTrans* getTransformation(int detID);
  void getEulerAngles(int detID, 
		      double& phi, 
		      double& theta, 
		      double& psi);
  double getPitch(int detID);
  double getRes(int detID);
  void getPlaneDef(int detID, 
		   TVector3 &o, 
		   TVector3 &u, 
		   TVector3 &v);
  void setConv(int detID, 
	       TGeoTranslation* trans, 
	       TGeoRotation* rot,
	       double pitch, 
	       double res);
  TVector3 localToMaster(int detID, TVector3 uvw);
  TMatrixD localToMaster(int detID, TMatrixD covUVW);
  TVector3 localToMasterVect(int detID, TVector3 uvw);
  TVector3 masterToLocal(int detID, TVector3 xyz);
  TMatrixD masterToLocal(int detID, TMatrixD covXYZ);
  TVector3 masterToLocalVect(int detID, TVector3 xyz);

  void localToMaster(int detID, TVector3 uvw,TVector3 &xyz);
  void localToMaster(int detID, TMatrixD covUVW, TMatrixD &ovXYZ);
  void localToMasterVect(int detID, TVector3 uvw,TVector3 &xyz);
  void masterToLocal(int detID, TVector3 xyz, TVector3 &uvw);
  void masterToLocal(int detID, TMatrixD covXYZ, TMatrixD &ovUVW);
  void masterToLocalVect(int detID, TVector3 xyz, TVector3 &uvw);

  int whichDetZ(TLorentzVector pos,double lim);
  int whichDetR(TLorentzVector pos,double lim);
  void read(std::string filename);
  void write(std::string filename);
  ~PndTpcAlignmentManager();

  //TestBench specific methods
  
 private:
  std::map<int, TGeoCombiTrans* > fTransformations;
  std::map<int, double> fRes;
  std::map<int, double> fPitch;
  std::vector<int> detIDs;
  std::string transformationFileName;
  static PndTpcAlignmentManager *inst;
  PndTpcAlignmentManager(std::string filename){
    //  std::cout<<"creator called"<<std::endl;
    read(filename);
  }
  void clear();
  void readDet(int detID, std::ifstream &input);
  void quit(int id){
    std::cerr << "PndTpcAlignmentManager: info for detid " << id 
	      << " requested, which is not known ->abort" << std::endl;
    throw;
  }
  std::vector<double> tVector3ToStlVector(TVector3 v){
    std::vector<double> r;
    r.push_back(v[0]);
    r.push_back(v[1]);
    r.push_back(v[2]);
    return r;
  }
   ClassDef(PndTpcAlignmentManager,5); 
};

#endif

