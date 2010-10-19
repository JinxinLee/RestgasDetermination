#include "PndTpcAlignmentManager.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <time.h>
//#include <boost/algorithm/string/trim.hpp>

#include "TLorentzVector.h"
using namespace std;

PndTpcAlignmentManager* PndTpcAlignmentManager::inst=NULL;

void PndTpcAlignmentManager::read(string filename){
  clear();

  transformationFileName=filename;
  ifstream input(filename.c_str());
  if(!input){
    cerr << "unable to open input file: "
	 << filename << " --bailing out! \n";
    throw;
  }
  string line;
  while(getline(input,line)){
    istringstream lineStream(line);
    string word;
    lineStream>>word;
    //  boost::algorithm::trim(word);
    if(word=="#"||word==""){
      continue;
    }else if(word=="detID"){
      int id;
      lineStream>>id;
      readDet(id,input);
    }
  }
  std::cout<<"PndTpcAlignmentManager read file: "<<filename<<" with "<<detIDs.size()<<" entries"<<endl;
  
}
PndTpcAlignmentManager::~PndTpcAlignmentManager(){
  std::map<int,TGeoCombiTrans*>::iterator itT=fTransformations.begin();
  while(itT!=fTransformations.end()){
    delete itT->second;
    ++itT;
  }
  fTransformations.clear();
  fRes.clear();
  fPitch.clear();
}
void PndTpcAlignmentManager::readDet(int detID, ifstream &input){
  //TODO check for missing information 
  string line;
  double res;
  TGeoTranslation* trans;
  TGeoRotation* rot;
  double pitch;
  while(getline(input,line)){
    istringstream lineStream(line);
    string word;
    lineStream>>word;
    //    boost::algorithm::trim(word);
    if(word=="#"||word==""){
      continue;
    }
    if(word=="detID"){
      setConv(detID, trans,rot,pitch,res);
      double phi,theta,psi;
      rot->GetAngles(phi,theta,psi);
      //      cout<<detID<<" trans "
      //  <<trans->GetTranslation()[0]<<" "
      //  <<trans->GetTranslation()[1]<<" "
      //  <<trans->GetTranslation()[2]<<endl
      //  <<"isrotation "<<rot->IsRotation()<<" angles "<<phi<<" "<<theta<<" "<<psi<<" "<<endl
      //  <<pitch<<" "<<res<<endl;
      detIDs.push_back(detID);
      int id;
      lineStream>>id;
      readDet(id,input);
    }
    if(word=="Resolution"){
      lineStream>>res;
      fRes[detID]=res;
    }
    if(word=="translation"){
      double x,y,z;      
      lineStream>>x>>y>>z;
      trans=new TGeoTranslation(x,y,z);
    }
    if(word=="euler"){
      double phi,theta,psi;
      lineStream>>phi>>theta>>psi;
      ostringstream name;
      name<<"rotation_detID_"<<detID;
      // cout<<name.str()<<" "<<phi<<" "<<theta<<" "<<psi<<endl;
      rot=new TGeoRotation(name.str().c_str(),phi,theta,psi);
    }
    if(word=="pitch"){
      lineStream>>pitch;
      fPitch[detID];
    }
  }
  setConv(detID, trans,rot,pitch,res);
}
void PndTpcAlignmentManager::clear(){
  std::map<int,TGeoCombiTrans*>::iterator itT=fTransformations.begin();
  while(itT!=fTransformations.end()){
    delete itT->second;
    ++itT;
  }
  fTransformations.clear();
  fPitch.clear();
  fRes.clear();
}

int PndTpcAlignmentManager::whichDetZ(TLorentzVector pos,double lim){
  std::map<int,TGeoCombiTrans*>::iterator itT=fTransformations.begin();
  while(itT!=fTransformations.end()){
    if(TMath::Abs(itT->second->GetTranslation()[2]-pos.Z())<lim){
      return itT->first;
    }
  }
  return -1;
}
void PndTpcAlignmentManager::getPlaneDef(int detID, 
				   TVector3 &o,
				   TVector3 &u, 
				   TVector3 &v){
  if(fTransformations.count(detID)){
    o=TVector3(fTransformations[detID]->GetTranslation());
    TVector3 u_l(1,0,0);
    TVector3 v_l(0,1,0);
    u=localToMasterVect(detID,u_l);
    v=localToMasterVect(detID,v_l);
    return;
  }else{
    quit(detID);
  } 
}
TGeoCombiTrans* PndTpcAlignmentManager::getTransformation(int detID){
    if(fTransformations.count(detID)){
      return fTransformations[detID];
    }else{
      quit(detID);
    }
  }
void PndTpcAlignmentManager::getEulerAngles(int detID, double &phi, double &theta, double &psi){
    if(fTransformations.count(detID)){
      if(fTransformations[detID]->GetRotation()==0){
	      phi=0;psi=0;theta=0;
      }else{
	      fTransformations[detID]->GetRotation()->GetAngles(phi,theta,psi);
      }
    }else{
      quit(detID);
    }
  }
double PndTpcAlignmentManager::getPitch(int detID){
    if(fPitch.count(detID)){
      return fPitch[detID];
    }else{
      quit(detID);
    }
    return -1;
  }
double PndTpcAlignmentManager::getRes(int detID){
    if(fRes.count(detID)){
      return fRes[detID];
    }else{
      quit(detID);
    }
    return -1;
  }
void PndTpcAlignmentManager::setConv(int detID, 
				     TGeoTranslation* trans, 
				     TGeoRotation* rot,
				     double pitch, 
				     double res){
  detIDs.push_back(detID);
  fTransformations[detID]=new TGeoCombiTrans(*trans,*rot);
  fRes[detID]=res;
  fPitch[detID]=pitch;
}

TVector3 PndTpcAlignmentManager::localToMaster(int detID, TVector3 uvw){
    if(fTransformations.count(detID)){
      vector<double> uvw_=tVector3ToStlVector(uvw);
      double xyz[3];
      fTransformations[detID]->LocalToMaster(&uvw_[0],xyz);
      return TVector3(xyz);
    }else{
      quit(detID);
    }
}
TMatrixD PndTpcAlignmentManager::localToMaster(int detID, TMatrixD covUVW){
  if(covUVW.GetNcols()!=3||covUVW.GetNrows()!=3){
    std::cerr << "PndTpcAlignmentManager: trying to transform covariance of invalid dimension "
	      <<covUVW.GetNcols()<<" x "<<covUVW.GetNrows()<<" -> abort"<<std::endl;
  }
  if(fTransformations.count(detID)){
    TMatrixD rot(3,3,fTransformations[detID]->GetRotationMatrix());
    //    TMatrixD ret(3,3);
    TMatrixD rotT(3,3);
    rotT.Transpose(rot);
    TMatrixD ret=(rot*covUVW)*rotT;
    return ret;
  }else{
    quit(detID);
  }
}
void PndTpcAlignmentManager::localToMaster(int detID, TVector3 uvw,TVector3 &xyz){
  xyz=localToMaster(detID, uvw);
}
void PndTpcAlignmentManager::localToMaster(int detID, TMatrixD covUVW, TMatrixD &covXYZ){
  covXYZ=localToMaster(detID, covUVW);
}
TVector3 PndTpcAlignmentManager::localToMasterVect(int detID, TVector3 uvw){
    if(fTransformations.count(detID)){
      vector<double> uvw_=tVector3ToStlVector(uvw);
      double xyz[3];
      fTransformations[detID]->LocalToMasterVect(&uvw_[0],xyz);
      return TVector3(xyz);
    }else{
      quit(detID);
    }
  }
void PndTpcAlignmentManager::localToMasterVect(int detID, TVector3 uvw,TVector3 &xyz){
  xyz=localToMasterVect(detID, uvw);
}
TVector3 PndTpcAlignmentManager::masterToLocal(int detID, TVector3 xyz){
    if(fTransformations.count(detID)){
      vector<double> xyz_=tVector3ToStlVector(xyz);
      double uvw[3];
      fTransformations[detID]->MasterToLocal(&xyz_[0],uvw);
      return TVector3(uvw);
    }else{
      quit(detID);
    }
  }
TMatrixD PndTpcAlignmentManager::masterToLocal(int detID, TMatrixD covXYZ){
  if(covXYZ.GetNcols()!=3||covXYZ.GetNrows()!=3){
    std::cerr << "PndTpcAlignmentManager: trying to transform covariance of invalid dimension "
	      <<covXYZ.GetNcols()<<" x "<<covXYZ.GetNrows()<<" -> abort"<<std::endl;
  }
  if(fTransformations.count(detID)){
    TMatrixD rot(3,3,fTransformations[detID]->GetRotationMatrix());
    TMatrixD ret(3,3);
    TMatrixD rotT(3,3);
    rotT.Transpose(rot);
    ret=rotT*covXYZ*rot;
    return ret;
  }else{
    quit(detID);
  }
}


void PndTpcAlignmentManager::masterToLocal(int detID, TVector3 xyz,TVector3 &uvw){
  uvw=localToMaster(detID, xyz);
}
void PndTpcAlignmentManager::masterToLocal(int detID, TMatrixD covXYZ, TMatrixD &covUVW){
  covUVW=localToMaster(detID, covXYZ);
}
TVector3 PndTpcAlignmentManager::masterToLocalVect(int detID, TVector3 xyz){
    if(fTransformations.count(detID)){
      vector<double> xyz_=tVector3ToStlVector(xyz);
      double uvw[3];
      fTransformations[detID]->MasterToLocalVect(&xyz_[0],uvw);
      return TVector3(uvw);
    }else{
      quit(detID);
    }
}
void PndTpcAlignmentManager::masterToLocalVect(int detID, TVector3 xyz,TVector3 &uvw){
  uvw=localToMasterVect(detID, xyz);
}
void PndTpcAlignmentManager::write(string outfilename){
  ofstream outfile(outfilename.c_str());
  time_t rawtime;
  struct tm * timeinfo;
  time(&rawtime);
  timeinfo=localtime(&rawtime);
  outfile<<"#alignment file written at "<<asctime(timeinfo)<<endl;
  for(unsigned int i=0;i<detIDs.size();++i){
    outfile<<"##########################################################"<<endl;
    outfile<<"detID "<<detIDs.at(i)<<endl;
    if(fTransformations.count(detIDs.at(i))){
      TGeoCombiTrans *tr=fTransformations[detIDs.at(i)];
      outfile<<"translation "
	     <<scientific<<tr->GetTranslation()[0]<<" "
	     <<scientific<<tr->GetTranslation()[1]<<" "
	     <<scientific<<tr->GetTranslation()[2]<<endl;
      double phi,theta,psi;
      getEulerAngles(detIDs.at(i),phi,theta,psi);
      outfile<<"euler       "
	     <<scientific<<phi<<" "
	     <<scientific<<theta<<" "
	     <<scientific<<psi<<endl;
    }
    if(fPitch.count(detIDs.at(i))){
      outfile<<"pitch       "<<scientific<<fPitch[detIDs.at(i)]<<endl;
    }

  }
}

