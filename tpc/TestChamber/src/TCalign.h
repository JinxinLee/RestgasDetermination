#ifndef TCALIGN_H
#define TCALIGN_H

#include <map>
#include <iostream>
#include <string>
#include "TVector3.h"
#include <vector>
#include <TMatrixT.h>

class TCalign{
public:
    static TCalign* getInstance(std::string filename = "alignement.txt"){
	if(inst==NULL) {
	    inst = new TCalign(filename);
	}
	return inst;
    }
    
    
    /*
      Building euler matrixes using the x-convention:
      temporary is theta and phi switched. ie theta has means the angle phi usualy points to.
      see:
      http://mathworld.wolfram.com/EulerAngles.html
      quote:
      "The so-called "x-convention," illustrated above, is the most common definition. 
      In this convention, the rotation given by Euler angles (phi,theta,psi), 
      where the first rotation is by an angle phi about the z-axis, 
      the second is by an angle theta in [0,pi] about the x-axis, 
      and the third is by an angle psi about the z-axis (again)"
    */
    
    static TMatrixT<double> eulerMatrix(double theta, double phi, double psi);

    void getConv(int id,TVector3& trans,TMatrixT<double>& rot,double& pitch){
	if(T[id]==NULL) quit(id);
	if(R[id]==NULL) quit(id);
	if(P[id]==NULL) quit(id);
	trans=*(T[id]);
	rot=*(R[id]);
	pitch=*(P[id]);
    }
    void getConv(int id,TVector3& trans,TMatrixT<double>& rot,double& pitch, double& theta_,double& phi_, double& psi_, double& resolution_){
	getConv(id, trans, rot, pitch);
	if(Theta[id]==NULL) quit(id);
	if(Phi[id]==NULL) quit(id);
	if(Psi[id]==NULL) quit(id);
	theta_=*(Theta[id]);
	phi_=*(Phi[id]);
	psi_=*(Psi[id]);
	resolution_=*(Resolution[id]);
    }
    
    void setConv(int id,const TVector3& trans,
		 const TMatrixT<double>& rot, 
                 const double& pitch, 
                 const double& theta_=0, 
                 const double& phi_=0, 
                 const double& psi_=0,
                 const double& resolution_=0.1){
	T[id]=new TVector3(trans);
	R[id]=new TMatrixT<double>(rot);
	P[id]=new double(pitch);
	Theta[id] = new double(theta_);
	Phi[id] = new double(phi_);
	Psi[id] = new double(psi_);
        Resolution[id] = new double(resolution_);
    }

    std::vector<int> getLoadedIDs();
    
    TVector3 notranslXYZtoUVW(int id,TVector3 xyz);
    TVector3 notranslUVWtoXYZ(int id,TVector3 uvw);
    TVector3 XYZtoUVW(int id,TVector3 xyz);
    TVector3 UVWtoXYZ(int id,TVector3 uvw);

    
    
    void write(std::string filename);
    void read(std::string filename = "alignement.txt");
    
    void clear();
    
private:
    std::map<int,TVector3* > T;
    std::map<int,TMatrixT<double>* > R;
    std::map<int,double*> P;
    /*
      Euler angles. To use equal conventions for angle naming for the
      tpc and the x-y-detectors, theta here is the azimuth-angle
      which in most other conventions is named phi. ie theta and phi
      is switched.
    */
    std::map<int,double*> Theta; 
    std::map<int,double*> Phi;
    std::map<int,double*> Psi;
    std::map<int,double*> Resolution;
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
