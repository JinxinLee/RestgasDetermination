#include "track.h"



std::pair<double,double> Track::intersect(double z){
    double x = startpoint.first + tangent.first*(z0 - z);
    double y = startpoint.second + tangent.second*(z0 - z);
    return std::pair<double,double>(x,y);
}
Track::Track(double x0, double y0, double z0_, double tx0, double ty0){
    startpoint=std::pair<double,double>(x0,y0);
    z0=z0_;
    tangent=std::pair<double,double>(tx0,ty0);
}
