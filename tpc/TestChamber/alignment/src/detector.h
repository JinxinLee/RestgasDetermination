#ifndef DETECTOR_H
#define DETECTOR_H
#include <utility>
#include <TRandom3.h>
#include <iostream>
class Track;
class Detector{
/*position of detectors origo in the global coordinates
    Actual values stored value + dValue
    dx = X_actual - x_stored
    X_actual = x_stored + dx
*/
    int id;
    double x,y,z; //stored value,
    double dx,dy, dz; //difference from actual values
//angle between Detector u-axis and global x-axis. Rotation about
//global z-axis
    double T; //Theta, angle around z-axis azimuth-angle
    double dT;
    double sigma;
    double pitch;
    int parameters;//for expansion-posibilities
    double biasdU;
    double biasdZ;
    double biasdT;
    double biasdP;
    double cosT;
    double sinT;
    TRandom3 rand;
public:
    Detector(int id_,
	     double x_,
	     double y_,
	     double z_,
	     double x_act,
	     double y_act,
	     double z_act,
	     double T_,
	     double T_act,
	     double pitch_,
	     double biasdU_=0.0,
	     double biasdZ_=0.0,
	     double biasdT_=0.0,
	     double biasdP_=0.0,
	     double sigma_=1
	);

    inline double getCosT() const {return cosT;};
    inline double  getSinT() const {return sinT;};
    inline double  getZ() const {return z;};
    inline double  getSigma() const {return sigma;};
    inline double  getT() const {return T;};
    inline double  getX() const {return x;};
    inline double  getY() const {return y;};
    inline int  getId() const {return id;};

    void setErrors(double x_act,double y_act,double z_act, double T_act){
      dx=x_act-x;
      dy=y_act-y;
      dz=z_act-z;
      dT=T_act-T;
      if(dT!=0){
        std::cout<<"ID:"<<id<<" dT: "<<dT<<std::endl;
      }
      if(dx!=0){
        std::cout<<"ID:"<<id<<" dx "<<dx<<std::endl;
      }
      if(dy!=0){
        std::cout<<"ID:"<<id<<" dy "<<dy<<std::endl;
      }
      if(dz!=0){
        std::cout<<"ID:"<<id<<" dz "<<dx<<std::endl;
      }

    }
    void setErrors(const Detector* const det);

    std::pair<double,double> getHitU(Track& track);//
					     //
					     //returning detector
					     //value in [0] and actual
					     //value in [1]
};
#endif
