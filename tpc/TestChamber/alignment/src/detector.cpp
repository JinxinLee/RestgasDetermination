#include "detector.h"
#include "track.h"
#include <cmath>
#include <TRandom3.h>
#include <iostream>
using namespace std;
Detector::Detector(int id_,
		   double x_,
		   double y_,
		   double z_,
		   double dx_,
		   double dy_,
		   double dz_,
		   double T_,
		   double dT_,
		   double pitch_,
		   double biasdU_,
		   double biasdZ_,
		   double biasdT_,
		   double biasdP_,
		   double sigma_
    ){
    id=id_;
    x=x_;
    y=y_;
    z=z_;
    dx=dx_;
    dy=dy_;
    dz=dz_;
    T=T_;
    dT=dT_;
    pitch=pitch_;
    biasdU=biasdU_;
    biasdZ=biasdZ_;
    biasdT=biasdT_;
    biasdP=biasdP_;
    sigma=sigma_;
    cosT=cos(T);
    sinT=sin(T);
}

void Detector::setErrors(const Detector* const det){
    dx=(det->getX()-x);
    dy=(det->getY()-y);
    dz=(det->getZ()-z);
    dT=(det->getT()-T);
    if(dT!=0){
      cout<<"ID:"<<id<<" dTheta: "<<dT<<endl;
    }
    if(dx!=0){
      cout<<"ID:"<<id<<" dx "<<dx<<endl;
    }
    if(dy!=0){
      cout<<"ID:"<<id<<" dy "<<dy<<endl;
    }
    if(dz!=0){
      cout<<"ID:"<<id<<" dz "<<dx<<endl;
    }
}

double Detector::round(double u_){
    int n = (int)(u_/pitch);
    return (double)(n*pitch);
}
pair<double,double> Detector::getHitU(Track& track){
    pair<double,double> trackxy = track.intersect(z+dz);
    pair<double,double> trackxy_a = track.intersect(z);//actual
    /*
      x,y,z,T center of detector with rotation T
      x0,y0,tx0,ty0 track parameters
      dT, dz, du is eventual alignment parameters
      
      u = cos(T+dT)(x0 + tx0(z + dz - z0) - x)
         -sin(T+dT)(y0 + ty0(z + dz - z0) - y)
	 +du
      du = dx*cos(T+dT)-dy*sin(T+dT)
      x0 + tx0(z + dz - z0=  trackxy.x
      y0 + ty0(z + dz - z0=  trackxy.y

    */
    double u_mean= (trackxy.first-x-dx)*cos(T+dT)-(trackxy.second-y-dy)*sin(T+dT);
    double u_a=(trackxy_a.first-x)*cos(T)-(trackxy_a.second-y)*sin(T);
    TRandom3* random = new TRandom3(0);
    double u = random->Gaus(u_mean,sigma);
    pair<double,double> ret;
    ret.first=u;
    ret.second=u_a;
    return ret;
}
