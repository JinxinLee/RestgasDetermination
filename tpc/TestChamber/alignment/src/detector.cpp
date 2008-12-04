#include "detector.h"
#include "track.h"
#include <cmath>
#include <iostream>
#include "TRandom3.h"
using namespace std;
Detector::Detector(int id_,
		   double x_,
		   double y_,
		   double z_,
		   double x_act,
		   double y_act,
		   double z_act,
		   double T_,
		   double T_act,
		   double pitch_,
		   double biasdU_,
		   double biasdZ_,
		   double biasdT_,
		   double biasdP_,
		   double sigma_
    ) : rand(0){
    id=id_;
    x=x_;
    y=y_;
    z=z_;
    dx=x_act-x;
    dy=y_act-y;
    dz=z_act-z;
    T=T_;
    dT=T_act-T;
    pitch=pitch_;
    biasdU=biasdU_;
    biasdZ=biasdZ_;
    biasdT=biasdT_;
    biasdP=biasdP_;
    sigma=sigma_;
    cosT=cos(T);
    sinT=sin(T);
    cout<<"ID:"<<id<<" Z: "<<z<<endl;
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

void Detector::setErrors(const Detector* const det){
    dx=(det->getX()-x);
    dy=(det->getY()-y);
    dz=(det->getZ()-z);
    dT=(det->getT()-T);
    if(dT!=0){
      cout<<"ID:"<<id<<" dT: "<<dT<<endl;
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
pair<double,double> Detector::getHitU(Track& track){
  pair<double,double> trackxy = track.intersect(z+dz);//monte carlo truth
  pair<double,double> trackxy_rec = track.intersect(z);//reconstructed
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
    double u_hit= (trackxy.first-x-dx)*cos(T+dT)+(trackxy.second-y-dy)*sin(T+dT);

    double u_rec=(trackxy_rec.first-x)*cos(T)+(trackxy_rec.second-y)*sin(T);
    double u_hit_smear = rand.Gaus(u_hit,sigma);
    pair<double,double> ret;
    ret.first=u_hit_smear;
    ret.second=u_rec;
    return ret;
}
