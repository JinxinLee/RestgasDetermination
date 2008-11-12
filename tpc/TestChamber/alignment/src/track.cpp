#include "track.h"



std::pair<double,double> Track::intersect(double z){
    double x = startpoint.first + tangent.first*(z - z0);
    double y = startpoint.second + tangent.second*(z - z0);
    return std::pair<double,double>(x,y);
}
Track::Track(double x0, double y0, double z0_, double tx0, double ty0){
    startpoint=std::pair<double,double>(x0,y0);
    z0=z0_;
    tangent=std::pair<double,double>(tx0,ty0);
}
