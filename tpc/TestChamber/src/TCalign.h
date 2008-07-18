#ifndef TCALIGN_H
#define TCALIGN_H

#include<map>
#include<iostream>
#include<string>
#include"TVector3.h"

class TCalign{
 public:
  static TCalign* getInstance(std::string filename = "alignement.txt"){
    if(inst==NULL) {
      inst = new TCalign(filename);
    }
    return inst;
  }

  void getConv(int id,TVector3& trans,TMatrixT<double>& rot,double pitch){
	if(T[id]==NULL) quit(id);
	if(R[id]==NULL) quit(id);
	if(P[id]==NULL) quit(id);
	trans=*(T[id]);
	rot=*(R[id]);
	pitch=*(P[id]);
  }

  void setConv(int id,const TVector3& trans,
			   const TMatrixT<double>& rot, const double& pitch){
	T[id]=new TVector3(trans);
	R[id]=new TMatrixT<double>(rot);
	P[id]=new double(pitch);
  }

  TVector3 dirXYZtoUVW(int id,TVector3 xyz);
  TVector3 XYZtoUVW(int id,TVector3 xyz);
  TVector3 UVWtoXYZ(int id,TVector3 uvw);

  void write(std::string filename);
  void read(std::string filename = "alignement.txt");
  
  void clear();

  private:
  std::map<int,TVector3* > T;
  std::map<int,TMatrixT<double>* > R;
  std::map<int,double*> P;
  static TCalign *inst;
  TCalign(std::string filename){
    read(filename);
  }
  void quit(int id){
	std::cerr << "TCalign: info for id " << id << " requested, which is not known ->abort" << std::endl;
	throw;
  }
};

#endif
